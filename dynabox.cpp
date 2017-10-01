/*
 * dynabox.cpp
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "dynabox.h"
#include "motor.h"
#include "timer.h"
#include "display.h"
#include "config.h"
#include "comm_prot.h"
#include "modbus_tcp.h"

#define DYNABOX_NUMBER_OF_FUNCTIONS 		10
void f12()
{
	//display.Write(5555);
}
// default values
Function EEMEM dynabox_eem_functions[DYNABOX_NUMBER_OF_FUNCTIONS] =
{
//   No.of function, 	default value,  function pointer
	{1, 				25, 			NULL},	// program address door or led
	{2, 				13, 			NULL},	// max doors
	{4,					0, 				f12},	// test led
	{5,					170,  			NULL},	// IP master
	{6,					0,  			f12},	// test door
	{7,					0,  			f12},	// test led
	{8,					100,  			NULL},	// offset
	{9,					0,  			f12},	// load defaults
	{10,				25,  			NULL},	// led brightness
	{28, 				0,				NULL},	// type of machine
};

Dynabox::Dynabox() : faults_to_led_map {
	0,									// not used
	0,									// F01, impossible
	COMM_RED_1PULSE,					// F02
	COMM_RED_2PULSES,					// F03
	COMM_RED_ON,						// F04
	COMM_RED_3PULSES,					// F05
	COMM_GREEN_RED_BLINK, 				// F06
	COMM_GREEN_RED_BLINK,				// F07
	COMM_GREEN_RED_BLINK,				// F08
	0,									// not used
	0, 									// F10, not used
	COMM_RED_BLINK,						// F11
	COMM_RED_BLINK, 					// F12
	COMM_RED_ON, 						// F13
	COMM_RED_BLINK, 					// F14
	0,									// F15, not used
	0, 									// F16, not used
	COMM_RED_BLINK	 					// F17
}
{
	current_address =  1;
	first_address = 1;
	last_address =  MAX_DOORS;
	d->faults = 0;
	for(uint8_t i = 0; i <= MAX_DOORS; i++) last_fault[i] = 0;
	led_same_for_all = 0;
}

void Dynabox::EV_LEDChecked(DynaboxData* pdata)
{
	BEGIN_TRANSITION_MAP								// current state
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_INIT
		TRANSITION_MAP_ENTRY(ST_CHECKING_ELECTROMAGNET)	// ST_CHECKING_LED
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_CHECKING_ELECTROMAGNET
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_HOMING
	END_TRANSITION_MAP(pdata)
}

void Dynabox::EV_ElectromagnetChecked(DynaboxData* pdata)
{
	SetCurrentCommand(COMM_SHOW_STATUS_ON_LED);
	BEGIN_TRANSITION_MAP								// current state
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_INIT
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_CHECKING_LED
		TRANSITION_MAP_ENTRY(ST_HOMING)					// ST_CHECKING_ELECTROMAGNET
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_HOMING
	END_TRANSITION_MAP(pdata)
}

void Dynabox::ST_Init(DynaboxData* pdata)
{
	InternalEvent(ST_CHECKING_LED, NULL);
}

void Dynabox::ST_CheckingLED(DynaboxData* pdata)
{
	SetCurrentCommand(COMM_LED_DIAG);
}

void Dynabox::ST_CheckingElectromagnet(DynaboxData* pdata)
{
	SetCurrentCommand(COMM_CHECK_ELECTROMAGNET);
}

void Dynabox::ST_Homing(DynaboxData* pdata)
{
	led_same_for_all = COMM_GREEN_RED_BLINK;
	SetCurrentCommand(COMM_SHOW_STATUS_ON_LED);

	//SetCurrentCommand(COMM_CHECK_TRANSOPTORS_GET_SET_STATUS);
	motor.EV_Homing();
}
// -----------------------------------------------------------
void Dynabox::SetCurrentCommand(uint8_t command)
{
	current_address = 1;
	current_command = command;
	switch(command)
	{
		case COMM_LED_DIAG:
			pcommand = &Dynabox::CommandCheckLed;
			pparse   = &Dynabox::ParseCheckLed;
			ptimeout = &Dynabox::TimeoutLed;
		break;

		case COMM_CHECK_ELECTROMAGNET:
			pcommand = &Dynabox::CommandCheckElectromagnet;
			pparse   = &Dynabox::ParseCheckElectromagnet;
			ptimeout = &Dynabox::TimeoutDoor;
		break;

		case COMM_SHOW_STATUS_ON_LED:
			pcommand = &Dynabox::CommandShowStatusOnLed;
		break;

		case COMM_CHECK_TRANSOPTORS_GET_SET_STATUS:
			pcommand = &Dynabox::CommandGetSetState;
			pparse   = &Dynabox::ParseGetSetState;
		break;

		default:
		break;
	}
	SLAVES_POLL_START;
}

void Dynabox::SlavesPoll()
{
	(this->*pcommand)();
}

void Dynabox::Parse(uint8_t* frame)
{
	SLAVES_POLL_TIMEOUT_OFF;
	if(comm.Crc8(frame, 2) == frame[2])
		(this->*pparse)(frame);
}
// -----------------------------------------------------------
void Dynabox::CommandCheckLed()
{
	SLAVES_POLL_TIMEOUT_SET;
	comm.Prepare(current_address + LED_ADDRESS_OFFSET, current_command);
}

void Dynabox::ParseCheckLed(uint8_t* frame)
{
	if(frame[0] == current_address + LED_ADDRESS_OFFSET)
	{
		if(current_address == last_address)
		{
			SLAVES_POLL_STOP;
			EV_LEDChecked(NULL);
		}
		else
			current_address++;
	}
}

void Dynabox::TimeoutLed()
{
	m->SetFault(F01_LED);
	mb.UpdateHoldingRegister(GENERAL_ERROR_STATUS, F01_LED);
	mb.UpdateHoldingRegister(current_address + 1, F01_LED << 8);
	if(current_address == last_address)
	{
		SLAVES_POLL_STOP;
		EV_LEDChecked(NULL);
	}
	else
		current_address++;
}
// -----------------------------------------------------------
void Dynabox::CommandCheckElectromagnet()
{
	if(current_address == last_address + 1)
	{
		SLAVES_POLL_STOP;
		EV_ElectromagnetChecked(NULL);
	}
	else
	{
		SLAVES_POLL_TIMEOUT_SET;
		comm.Prepare(current_address, current_command);
		comm.Prepare(current_address + LED_ADDRESS_OFFSET, COMM_GREEN_ON_FOR_TIME);
	}
}

void Dynabox::ParseCheckElectromagnet(uint8_t* frame)
{
	if(frame[0] == current_address)
	{
		if(frame[1] == COMM_F05_ELECTROMAGNET)
		{
			m->SetFault(F05_ELECTROMAGNET);
			mb.UpdateHoldingRegister(current_address + 1, F05_ELECTROMAGNET << 8);
		}
		current_address++;
	}
}

void Dynabox::CommandGetSetState()
{
	if(current_address == last_address + 1)
	{
		SetCurrentCommand(COMM_SHOW_STATUS_ON_LED);
		current_address = first_address;
		return;
	}

	SLAVES_POLL_TIMEOUT_SET;
	uint8_t position = mb.GetHoldingRegister(LOCATIONS_NUMBER + current_address);
	if(position > 0)
	{
		comm.Prepare(current_address, 0xC0 + position);
		mb.UpdateHoldingRegister(LOCATIONS_NUMBER + current_address, 0);
	}
	else
	{
		comm.Prepare(current_address, 0x80);
	}
}

void Dynabox::ParseGetSetState(uint8_t* frame)
{
	if(frame[0] == current_address)
	{
		if(frame[1] == 0xF0)
		{
			m->SetFault(F03_OPTICAL_SWITCHES);
			mb.UpdateHoldingRegister(current_address + 1, F03_OPTICAL_SWITCHES << 8);
		}
		else
		{
			mb.UpdateHoldingRegister(current_address + 1, frame[1]);
		}
		current_address++;
	}
}
// -----------------------------------------------------------
void Dynabox::TimeoutDoor()
{
	m->SetFault(F02_DOOR);
	mb.UpdateHoldingRegister(GENERAL_ERROR_STATUS, F02_DOOR);
	mb.UpdateHoldingRegister(current_address + 1, F02_DOOR << 8);
	current_address++;
}
// -----------------------------------------------------------
void Dynabox::CommandShowStatusOnLed()
{
	static bool need_send_to_led = false;
	if(led_same_for_all == 0xFF)
	{
		uint16_t status = mb.GetHoldingRegister(current_address + 1);
		if((status >> 8) != last_fault[current_address])
		{
			uint8_t command = faults_to_led_map[status >> 8];
			comm.Prepare(current_address + LED_ADDRESS_OFFSET, command + 0x80);
			last_fault[current_address] = status >> 8;
			need_send_to_led = true;
		}
	}
	else
	{
		comm.Prepare(current_address + LED_ADDRESS_OFFSET, led_same_for_all + 0x80);
		need_send_to_led = true;
	}

	if(current_address == last_address)
	{
		if(need_send_to_led)
		{
			comm.LedTrigger();
			need_send_to_led = false;
		}
		led_same_for_all = 0xFF;
		SetCurrentCommand(COMM_CHECK_TRANSOPTORS_GET_SET_STATUS);
	}
	else
		current_address++;
}

void Dynabox::ReplyTimeout()
{
	(this->*ptimeout)();
}

void Dynabox::LoadSupportedFunctions()
{
	eeprom_read_block(&functions, &dynabox_eem_functions, FUNCTION_RECORD_SIZE * DYNABOX_NUMBER_OF_FUNCTIONS);
	config.number_of_functions = DYNABOX_NUMBER_OF_FUNCTIONS;
}

void Dynabox::SaveParameters()
{
	eeprom_update_block(&functions, &dynabox_eem_functions, FUNCTION_RECORD_SIZE * DYNABOX_NUMBER_OF_FUNCTIONS);
}
