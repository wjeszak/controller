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
	curr_addr =  1;
	first_addr = 1;
	last_addr =  6;
	dynabox_data.faults = 0;
}

void Dynabox::EV_LEDChecked(DynaboxData* pdata)
{
	BEGIN_TRANSITION_MAP								// current state
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_INIT
		TRANSITION_MAP_ENTRY(ST_CHECK_ELECTROMAGNET)	// ST_CHECK_LED
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_CHECK_ELECTROMAGNET
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_HOMING
	END_TRANSITION_MAP(pdata)
}

void Dynabox::EV_ElectromagnetChecked(DynaboxData* pdata)
{
	BEGIN_TRANSITION_MAP								// current state
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_INIT
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_CHECK_LED
		TRANSITION_MAP_ENTRY(ST_HOMING)					// ST_CHECK_ELECTROMAGNET
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_HOMING
	END_TRANSITION_MAP(pdata)
}

void Dynabox::Parse(uint8_t* frame)
{
	uint8_t crc = comm.Crc8(frame, 2);
	// reply from led
	if((frame[0] == m->curr_addr + LED_ADDRESS_OFFSET) && (frame[2] == crc))
	{
		m->curr_addr++;
		if(m->curr_addr == m->last_addr + 1) EV_LEDChecked(NULL);
	}

	// reply from door
	if((frame[0] == m->curr_addr) && (frame[2] == crc))
	{
		uint8_t result = frame[1];

		switch(comm.curr_command)
		{
		case COMM_CHECK_ELECTROMAGNET:
			ParseCommandCheckElectromagnet(result);
			m->curr_addr++;
			if(m->curr_addr == m->last_addr + 1) { comm.LedTrigger(); EV_ElectromagnetChecked(NULL); }
		break;

		case COMM_CHECK_TRANSOPTORS_GET_STATUS:
			ParseCommandCheckTransoptorsGetStatus(result);
			m->curr_addr++;
		break;
		}
	}
}

void Dynabox::ParseCommandCheckElectromagnet(uint8_t res)
{
	if(res == 0x00)
	{
		modbus_tcp.UpdateHoldingRegisters(m->curr_addr + 1, NO_FAULT << 8);
		comm.Prepare(TLed, m->curr_addr, COMM_GREEN_ON);
	}
	if(res == 0x01)
	{
		//display.Write(TFault, F05_ELECTROMAGNET_FAULT);
		modbus_tcp.UpdateHoldingRegisters(m->curr_addr + 1, F05_ELECTROMAGNET_FAULT << 8);
		comm.Prepare(TLed, m->curr_addr, COMM_RED_3PULSES);
	}
}

void Dynabox::ParseCommandCheckTransoptorsGetStatus(uint8_t res)
{
	if(res == 0xF0)
	{
		//display.Write(TFault, F03_OPTICAL_SWITCHES_FAULT);
		modbus_tcp.UpdateHoldingRegisters(m->curr_addr + 1, F03_OPTICAL_SWITCHES_FAULT << 8);
	}
	else
		modbus_tcp.UpdateHoldingRegisters(m->curr_addr + 1, res);
}

void Dynabox::ReplyTimeout()
{
	switch(comm.curr_command)
	{
	case COMM_DIAG:
		//display.Write(TFault, F01_LED_FAULT);
		modbus_tcp.UpdateHoldingRegisters(GENERAL_ERROR_STATUS, F01_LED_FAULT);
		modbus_tcp.UpdateHoldingRegisters(m->curr_addr + 1, F01_LED_FAULT << 8);
		if(m->curr_addr == m->last_addr) EV_LEDChecked(NULL);
	break;
	default:
		//display.Write(TFault, F02_DOOR_FAULT);
		//modbus_tcp.UpdateHoldingRegisters(GENERAL_ERROR_STATUS, F02_DOOR_FAULT);
		modbus_tcp.UpdateHoldingRegisters(m->curr_addr + 1, F02_DOOR_FAULT << 8);
		comm.Prepare(TLed, m->curr_addr, COMM_RED_1PULSE);
		if(m->curr_addr == m->last_addr) { comm.LedTrigger(); }
	break;
	}
	m->curr_addr++;
}

void Dynabox::ST_Init(DynaboxData* pdata)
{
	// additional init commands
	//SetFault(F03_OpticalSwitches);
	//SetFault(F13_MainDoor);
	//SetFault(F01_SlaveLed);
	InternalEvent(ST_CHECK_LED, NULL);
}

void Dynabox::ST_CheckLED(DynaboxData* pdata)
{
	// begin... checking LED
	curr_addr = 1;
	comm.repeat = false;
	comm.dest = TLed;
	comm.curr_command = COMM_DIAG;
	timer.Assign(TIMER_SLAVES_POLL, 100, SlavesPoll);
}

void Dynabox::ST_CheckElectromagnet(DynaboxData* pdata)
{
	// checking transoptors
	curr_addr = 1;
	comm.repeat = false;
	comm.dest = TDoor;
	comm.curr_command = COMM_CHECK_ELECTROMAGNET;
	timer.Assign(TIMER_SLAVES_POLL, 100, SlavesPoll);
}

void Dynabox::ST_Homing(DynaboxData* pdata)
{
	curr_addr = 1;
	comm.repeat = true;
	comm.dest = TDoor;
	comm.curr_command = COMM_CHECK_TRANSOPTORS_GET_STATUS;
	timer.Assign(TIMER_SLAVES_POLL, 100, SlavesPoll);
	motor.EV_Homing();
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
