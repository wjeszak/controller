/*
 * dynabox_functions.cpp
 *
 *  Created on: 20 paü 2017
 *      Author: tomek
 */

#include <avr/eeprom.h>
#include "config.h"
#include "dynabox.h"

#define DYNABOX_NUMBER_OF_FUNCTIONS 		10

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

void Dynabox::LoadSupportedFunctions()
{
	eeprom_read_block(&functions, &dynabox_eem_functions, FUNCTION_RECORD_SIZE * DYNABOX_NUMBER_OF_FUNCTIONS);
	config.number_of_functions = DYNABOX_NUMBER_OF_FUNCTIONS;
}

void Dynabox::SaveParameters()
{
	eeprom_update_block(&functions, &dynabox_eem_functions, FUNCTION_RECORD_SIZE * DYNABOX_NUMBER_OF_FUNCTIONS);
}
