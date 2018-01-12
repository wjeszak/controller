/*
 * lockerbox_entry_exit.cpp
 *
 *  Created on: 12 sty 2018
 *      Author: wjeszak
 */

#include "machine.h"
#include "lockerbox.h"

void Lockerbox::ENTRY_TestingElm(LockerboxData* pdata)
{
	SetDoorCommand(CheckElmGetStatus);
}

void Lockerbox::EXIT_TestingElm()
{
	//s.Push(ST_TESTING_ELM);
}

void Lockerbox::ENTRY_Ready()
{
//	SetDoorCommand();
}

void Lockerbox::EXIT_Ready()
{
	//s.Push(ST_TESTING_ELM);
}

void Lockerbox::ENTRY_NotReady()
{
//	SetDoorCommand();
}

void Lockerbox::EXIT_NotReady()
{
	//s.Push(ST_TESTING_ELM);
}
