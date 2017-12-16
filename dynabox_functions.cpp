/*
 * dynabox_functions.cpp
 *
 *  Created on: 20 paü 2017
 *      Author: tomek
 */

#include <avr/eeprom.h>
#include "config.h"
#include "dynabox.h"
#include "modbus_tcp.h"

#define DYNABOX_NUMBER_OF_FUNCTIONS 		11

// Functions
void f12()
{
	//display.Write(5555);
}

// Default values in EEPROM
Function EEMEM dynabox_eem_functions[DYNABOX_NUMBER_OF_FUNCTIONS] =
{
//   No.of function, 	Default value,  Function pointer
	{1, 				25, 			NULL},	// program address door or led
	{2, 				7, 				NULL},	// max doors
	{4,					0, 				f12},	// test led
	{5,					170,  			NULL},	// IP master
	{6,					0,  			f12},	// test door
	{7,					0,  			f12},	// test led
	{8,					0,  			NULL},	// offset
	{9,					0,  			f12},	// load defaults
	{10,				25,  			NULL},	// led brightness
	{12,				1,				NULL},	// serial number
	{28, 				0,				NULL},	// type of machine
};

void Dynabox::LoadParameters()
{
	eeprom_read_block(&functions, &dynabox_eem_functions, FUNCTION_RECORD_SIZE * DYNABOX_NUMBER_OF_FUNCTIONS);
	config.number_of_functions = DYNABOX_NUMBER_OF_FUNCTIONS;
	mb.Write(TYPE_OF_MACHINE, (functions[1].param << 8 | 36));
	mb.Write(SERIAL_NUMBER, functions[9].param);

	mb.Write(FIRST_DOOR_NUMBER, 1);
	mb.Write(88, 20);
	mb.Write(90, 20);
	mb.Write(91, 75);
	mb.Write(92, 50);
	mb.Write(93, 400);
	mb.Write(94, 3);
	mb.Write(98, 5);
	mb.Write(99, 10);
}

void Dynabox::SaveParameters()
{
	eeprom_update_block(&functions, &dynabox_eem_functions, FUNCTION_RECORD_SIZE * DYNABOX_NUMBER_OF_FUNCTIONS);
	mb.Write(TYPE_OF_MACHINE, (functions[1].param << 8 | 36));
	mb.Write(SERIAL_NUMBER, functions[9].param);
}
