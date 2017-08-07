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

#define DYNABOX_NUMBER_OF_FUNCTIONS 		4
void f12()
{
	display.Write(5555);
}
// default values
Function EEMEM dynabox_eem_functions[DYNABOX_NUMBER_OF_FUNCTIONS] =
{
// No. of function, default value, defualt function pointer
	{1, 			25, 			NULL},		// 1
	{2, 			3, 				NULL},		// 2
	{12,			0, 				f12},		// 12
	{28,			1,  			NULL},		// 28
};

Dynabox::Dynabox()
{

}

void Dynabox::LoadSupportedFunctions()
{
	eeprom_read_block(&functions, &dynabox_eem_functions, FUNCTION_RECORD_SIZE * DYNABOX_NUMBER_OF_FUNCTIONS);
	config.SetSupportedFunctions(DYNABOX_NUMBER_OF_FUNCTIONS);
}

uint8_t Dynabox::StartupTest()
{
	motor.EV_Homing();
	timer.Assign(TIMER_MODBUS_RTU_POLL, 50, ModbusPoll);
	return 2;
}
