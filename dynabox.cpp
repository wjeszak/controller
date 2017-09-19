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

Dynabox::Dynabox()
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

void Dynabox::EV_ElectromagnetChecked(DynaboxData* pdata)
{
	BEGIN_TRANSITION_MAP								// current state
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_INIT
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_CHECKING_LED
		TRANSITION_MAP_ENTRY(ST_HOMING)					// ST_CHECKING_ELECTROMAGNET
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_HOMING
	END_TRANSITION_MAP(pdata)
}

void Dynabox::SetCurrentCommand(uint8_t command, bool rep)
{
	current_address = 1;
	current_command = command;
	repeat = rep;
	switch(command)
	{
		case COMM_LED_DIAG:
			pcommand = &Dynabox::CommandCheckLed;
			pparse = &Dynabox::ParseCheckLed;
		break;

		default:
		break;
	}
}

void Dynabox::CommandCheckLed()
{
	comm.Prepare(current_address + LED_ADDRESS_OFFSET, current_command);
}

void Dynabox::ParseCheckLed(uint8_t* frame)
{
	uint8_t crc = comm.Crc8(frame, 2);
	if((frame[0] == current_address + LED_ADDRESS_OFFSET) && (frame[2] == crc))
	{
		if(current_address == last_address) { EV_LEDChecked(NULL); }
		current_address++;
	}
}

void Dynabox::SlavesPoll()
{
	(this->*pcommand)();
	SLAVES_POLL_TIMEOUT_SET;
}

void Dynabox::Parse(uint8_t* frame)
{

	// reply from led
	(this->*pparse)(frame);
	// reply from door
//	if((frame[0] == current_address) && (frame[2] == crc))
//	{
//		uint8_t result = frame[1];
/*
		switch(comm.curr_command)
		{
		case COMM_CHECK_ELECTROMAGNET:
			ParseCommandCheckElectromagnet(result);
			current_address++;
			if(current_address == last_address + 1) { EV_ElectromagnetChecked(NULL); }
		break;

		case COMM_CHECK_TRANSOPTORS_GET_STATUS:
			ParseCommandCheckTransoptorsGetStatus(result);
			current_address++;
		break;

		} */
//	}
}

void Dynabox::ReplyTimeout()
{
/*	switch(comm.curr_command)
	{
	case COMM_LED_DIAG:
		m->SetFault(F01_SlaveLed);
		mb.UpdateHoldingRegisters(GENERAL_ERROR_STATUS, F01_LED_FAULT);
		mb.UpdateHoldingRegisters(current_address + 1, F01_LED_FAULT << 8);
		if(current_address == last_address) EV_LEDChecked(NULL);
	break;
	default:
		m->SetFault(F02_SlaveDoor);
		//modbus_tcp.UpdateHoldingRegisters(GENERAL_ERROR_STATUS, F02_DOOR_FAULT);
		mb.UpdateHoldingRegisters(current_address + 1, F02_DOOR_FAULT << 8);
		//comm.Prepare(TLed, m->curr_addr, COMM_RED_1PULSE);
		//if(m->curr_addr == m->last_addr) { comm.LedTrigger(); }
	break;
	}
	current_address++;
*/
}

void Dynabox::PCCheckElectromagnet(uint8_t res)
{
	if(res == 0x00)
	{
		mb.UpdateHoldingRegisters(current_address + 1, NO_FAULT << 8);
		//comm.Prepare(TLed, m->curr_addr, COMM_GREEN_ON);
	}
	if(res == 0x01)
	{
		//display.Write(TFault, F05_ELECTROMAGNET_FAULT);
		mb.UpdateHoldingRegisters(current_address + 1, F05_ELECTROMAGNET_FAULT << 8);
		//comm.Prepare(TLed, m->curr_addr, COMM_RED_3PULSES);
	}
}

void Dynabox::PCCheckTransoptorsGetStatus(uint8_t res)
{
	if(res == 0xF0)
	{
		//display.Write(TFault, F03_OPTICAL_SWITCHES_FAULT);
		mb.UpdateHoldingRegisters(current_address + 1, F03_OPTICAL_SWITCHES_FAULT << 8);
	}
	else
		mb.UpdateHoldingRegisters(current_address + 1, res);
}

void Dynabox::ST_Init(DynaboxData* pdata)
{
	// additional init commands
	InternalEvent(ST_CHECKING_LED, NULL);
}

void Dynabox::ST_CheckingLED(DynaboxData* pdata)
{
	SetCurrentCommand(COMM_LED_DIAG, false);
	SLAVES_POLL_START;
}

void Dynabox::ST_CheckingElectromagnet(DynaboxData* pdata)
{
	// checking transoptors
	//current_address = 1;
	//comm.repeat = false;
	//comm.dest = TDoor;
	//comm.curr_command = COMM_CHECK_ELECTROMAGNET;
	//SLAVES_POLL_START;
}

void Dynabox::ST_Homing(DynaboxData* pdata)
{
//	current_address = 1;
	//comm.repeat = true;
	//comm.dest = TDoor;
	//comm.curr_command = COMM_CHECK_TRANSOPTORS_GET_STATUS;
//	SLAVES_POLL_START;
//	motor.EV_Homing();
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
