/*
 * dynabox_functions.cpp
 *
 *  Created on: 20 paü 2017
 *      Author: tomek
 */

#include <avr/eeprom.h>
#include "boot.h"
#include "config.h"
#include "dynabox.h"
#include "modbus_tcp.h"
#include "timer.h"

// Functions
void f12()
{
	//display.Write(5555);
}

// Default values in EEPROM
Function EEMEM dynabox_eem_functions[DYNABOX_NUMBER_OF_FUNCTIONS] =
{
//   No.of function, 	Default value,  Function pointer
	{1, 				0, 				NULL},	// type of machine
	{2, 				7, 				NULL},	// max doors
	{3,					0, 				f12},	// test led
	{4,					170,  			NULL},	// IP master
	{5,					0,  			f12},	// test door
	{6,					0,  			f12},	// test led
	{7,					0,  			NULL},	// offset
	{8,					0,  			f12},	// load defaults
	{9,					25,  			NULL},	// led brightness
	{10,				1,				NULL},	// serial number
	{28, 				0,				NULL},	// type of machine
	{39, 				3, 				NULL},	// max of electromagnets
	{40, 				1, 				NULL}, 	// time for elm on
};

void Dynabox::LoadParameters()
{
	eeprom_read_block(&functions, &dynabox_eem_functions, FUNCTION_RECORD_SIZE * DYNABOX_NUMBER_OF_FUNCTIONS);
	config.number_of_functions = DYNABOX_NUMBER_OF_FUNCTIONS;

	mb.Write(TYPE_OF_MACHINE, (functions[1].param << 8 | 36));
	mb.Write(SERIAL_NUMBER, functions[9].param);
	mb.Write(MAX_ELM_ON, functions[11].param);
	mb.Write(FIRST_DOOR_NUMBER, 1);
	mb.Write(ILLEGAL_ROTATION_DISTANCE, 20);
	mb.Write(SPEED_MANUAL, 20);
	mb.Write(SPEED_NORMAL, 75);
	mb.Write(ACCELERATION_PULSES, 50);
	mb.Write(DECELERATION_PULSES, 400);
	mb.Write(TIME_FOR_MECH_WARNING, 5);
	mb.Write(TIME_FOR_MECH_FAULT, 10);
	door_open_timeout_val = functions[12].param * 10 * 1000 / SLAVE_POLL_INTERVAL / (functions[1].param & 0xFF);
}

void Dynabox::SaveParameters()
{
	eeprom_update_byte(&ee_machine_type, functions[0].param);
	eeprom_update_block(&functions, &dynabox_eem_functions, FUNCTION_RECORD_SIZE * DYNABOX_NUMBER_OF_FUNCTIONS);
	eeprom_update_word(&dynabox_eem_functions[0].param, (uint8_t)MACHINE_TYPE_DYNABOX);
	mb.Write(TYPE_OF_MACHINE, (functions[1].param << 8 | 36));
	mb.Write(SERIAL_NUMBER, functions[9].param);
	mb.Write(MAX_ELM_ON, functions[11].param);
}
