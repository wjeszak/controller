/*
 * lockerbox_functions.cpp
 *
 *  Created on: 12 sty 2018
 *      Author: wjeszak
 */

#include <avr/eeprom.h>
#include "boot.h"
#include "machine.h"
#include "config.h"
#include "lockerbox.h"
#include "modbus_tcp.h"
#include "timer.h"
#include "comm.h"

// Functions
void TestDoor(uint8_t value)
{
	comm.EV_Send(value, OpenLockerbox + 3, false);
}

// Default values in EEPROM
Function EEMEM lockerbox_eem_functions[LOCKERBOX_NUMBER_OF_FUNCTIONS] =
{
//   No.of function, 	Default value,  Function pointer
	{1, 				1,	 			NULL	},			// type of machine
	{2, 				6,				NULL	},			// max doors
	{4,					170,  			NULL	},			// IP master
	{6,					1, 				TestDoor},			// test door
	{10,				1,				NULL	},			// serial number
	{28, 				0,				NULL	},			// type of machine
	{40, 				1, 				NULL	}, 			// time for elm on
};

void Lockerbox::LoadParameters()
{
	eeprom_read_block(&functions, &lockerbox_eem_functions, FUNCTION_RECORD_SIZE * LOCKERBOX_NUMBER_OF_FUNCTIONS);
	config.number_of_functions = LOCKERBOX_NUMBER_OF_FUNCTIONS;

	mb.Write(TYPE_OF_MACHINE, (functions[1].param << 8 | 1));
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
}

void Lockerbox::SaveParameters()
{
	eeprom_update_byte(&ee_machine_type, functions[0].param);
	eeprom_update_block(&functions, &lockerbox_eem_functions, FUNCTION_RECORD_SIZE * LOCKERBOX_NUMBER_OF_FUNCTIONS);
	eeprom_update_word(&lockerbox_eem_functions[0].param, (uint8_t)MACHINE_TYPE_LOCKERBOX);
	mb.Write(TYPE_OF_MACHINE, (functions[1].param << 8 | 1));
	mb.Write(SERIAL_NUMBER, functions[9].param);
	mb.Write(MAX_ELM_ON, functions[11].param);
}