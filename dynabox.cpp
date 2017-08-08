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
#include "eeprom.h"
#include "display.h"
#include "config.h"

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

uint8_t Dynabox::StartupTest()
{
	motor.EV_Homing();
	timer.Assign(TIMER_MODBUS_RTU_POLL, 50, ModbusPoll);
	return 2;
}
