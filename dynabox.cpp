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
	display.Write(5555);
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
}

void Dynabox::EV_LEDChecked(DynaboxData* pdata)
{

}

void Dynabox::Parse(uint8_t* frame)
{
	uint8_t crc = comm.Crc8(frame, 2);
	// reply from door
	if((frame[0] == m->curr_addr) && (frame[2] == crc))
	{
		uint8_t result = frame[1];
		switch(comm.curr_command)
		{
		case COMM_CHECK_ELECTROMAGNET:
			if(result == 0x00)
			{
				modbus_tcp.UpdateHoldingRegisters(m->curr_addr + 1, NO_FAULT << 8);
				comm.Prepare(TLed, m->curr_addr, COMM_GREEN_ON);
			}
			if(result == 0x01)
			{
				display.Write(TFault, F05_ELECTROMAGNET_FAULT);
				modbus_tcp.UpdateHoldingRegisters(m->curr_addr + 1, F05_ELECTROMAGNET_FAULT << 8);
				comm.Prepare(TLed, m->curr_addr, COMM_RED_3PULSES);
			}
			if(m->curr_addr == m->last_addr) { comm.Prepare(TLed); }
		break;
		}
	}
	// reply from led
	if((frame[0] == m->curr_addr + LED_ADDRESS_OFFSET) && (frame[2] == crc))
	{
		if(m->curr_addr == m->last_addr) { comm.Prepare(TLed); }
	}
	m->curr_addr++;
}

void Dynabox::ReplyTimeout()
{
	switch(comm.curr_command)
	{
	case COMM_DIAG:
		display.Write(TFault, F01_LED_FAULT);
		modbus_tcp.UpdateHoldingRegisters(GENERAL_ERROR_STATUS, F01_LED_FAULT);
		modbus_tcp.UpdateHoldingRegisters(m->curr_addr + 1, F01_LED_FAULT << 8);
		if(m->curr_addr == m->last_addr) EV_LEDChecked(NULL);
	break;
	default:
		display.Write(TFault, F02_DOOR_FAULT);
		//modbus_tcp.UpdateHoldingRegisters(GENERAL_ERROR_STATUS, F02_DOOR_FAULT);
		modbus_tcp.UpdateHoldingRegisters(m->curr_addr + 1, F02_DOOR_FAULT << 8);
		comm.Prepare(TLed, m->curr_addr, COMM_RED_1PULSE);
		if(m->curr_addr == m->last_addr) { comm.Prepare(TLed); }
	break;
	}
	m->curr_addr++;
}

void Dynabox::ST_Init(DynaboxData* pdata)
{
	// additional init commands
	InternalEvent(ST_CHECK_LED, NULL);
}

void Dynabox::ST_CheckLED(DynaboxData* pdata)
{
	// begin... checking LED
	comm.repeat = false;
	comm.dest = TLed;
	comm.curr_command = COMM_DIAG;
	timer.Assign(TIMER_SLAVES_POLL, 100, SlavesPoll);
}

void Dynabox::ST_Homing(DynaboxData* pdata)
{
//	for(uint8_t i = 1; i <= 6; i++)
//	{
//		comm.Prepare(i + LED_ADDRESS_OFFSET, 0x05);
//	}
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
