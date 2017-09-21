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
	last_address =  6;
	d->faults = 0;
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

void Dynabox::EV_DoorsChecked(DynaboxData* pdata)
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
//	motor.EV_Homing();
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
	SLAVES_POLL_TIMEOUT_SET;
	comm.Prepare(current_address, current_command);
	comm.Prepare(current_address + LED_ADDRESS_OFFSET, COMM_GREEN_ON_FOR_TIME);
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
		if(current_address == last_address)
		{
			SLAVES_POLL_STOP;
			EV_DoorsChecked(NULL);
		}
		else
			current_address++;
	}
}
// -----------------------------------------------------------
void Dynabox::TimeoutDoor()
{
	m->SetFault(F02_DOOR);
	mb.UpdateHoldingRegister(GENERAL_ERROR_STATUS, F02_DOOR);
	mb.UpdateHoldingRegister(current_address + 1, F02_DOOR << 8);
	if(current_address == last_address)
	{
		SLAVES_POLL_STOP;
		EV_DoorsChecked(NULL);
	}
	else
		current_address++;
}
// -----------------------------------------------------------
void Dynabox::CommandShowStatusOnLed()
{
	uint16_t status = mb.GetHoldingRegister(current_address + 1);
	uint8_t command = faults_to_led_map[status >> 8];
	comm.Prepare(current_address + LED_ADDRESS_OFFSET, command + 0x80);
	if(current_address == last_address)
	{
		SLAVES_POLL_STOP;
		comm.LedTrigger();
	}
	else
		current_address++;
}

void Dynabox::ReplyTimeout()
{
	(this->*ptimeout)();
}

void Dynabox::PCCheckTransoptorsGetStatus(uint8_t res)
{
	if(res == 0xF0)
	{
		mb.UpdateHoldingRegister(current_address + 1, F03_OPTICAL_SWITCHES << 8);
	}
	else
		mb.UpdateHoldingRegister(current_address + 1, res);
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
