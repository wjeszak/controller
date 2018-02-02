/*
 * lockerbox_entry_exit.cpp
 *
 *  Created on: 12 sty 2018
 *      Author: wjeszak
 */

#include "machine.h"
#include "lockerbox.h"
#include "stack.h"
#include "modbus_tcp.h"
#include "config.h"

void Lockerbox::ENTRY_TestingElm(LockerboxData* pdata)
{
	number_of_elm_faults = 0;
	SetDoorCommand(CheckElmGetStatusLockerbox);
}

void Lockerbox::EXIT_TestingElm()
{
	s.Push(ST_READY);
	if(number_of_elm_faults == functions[1].param)
		fault.SetGlobal(F17_24VMissing);
}

void Lockerbox::ENTRY_Ready()
{
	SetDoorCommand(GetStatusLockerbox);
}

void Lockerbox::EXIT_Ready()
{

}

void Lockerbox::ENTRY_Processing()
{
	SetOrderStatus(Processing);
	SetDoorCommand();
}

void Lockerbox::EXIT_Processing()
{
	for(uint8_t i = 0; i <= 29; i++)		// see 02 Registers -> Registers[187]
	{
		mb.Write(FIRST_DOOR_CONTROL + i, 0);
	}
	SetOrderStatus(Ready);
	s.Push(ST_READY);
}

void Lockerbox::ENTRY_NotReady()
{

}

void Lockerbox::EXIT_NotReady()
{

}
