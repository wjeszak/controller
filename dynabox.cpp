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
	if(comm.Crc8(frame, 2) == frame[2]) (this->*pparse)(frame);
}

void Dynabox::ReplyTimeout()
{
	(this->*ptimeout)();
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

void Dynabox::TimeoutDoor()
{
	m->SetFault(F02_DOOR);
	mb.UpdateHoldingRegister(GENERAL_ERROR_STATUS, F02_DOOR);
	mb.UpdateHoldingRegister(current_address + 1, F02_DOOR << 8);
	current_address++;
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
