/*
 * lockerbox_entry_exit.cpp
 *
 *  Created on: 12 sty 2018
 *      Author: wjeszak
 */

#include "machine.h"
#include "lockerbox.h"
#include "stack.h"

void Lockerbox::ENTRY_TestingElm(LockerboxData* pdata)
{
	SetDoorCommand(CheckElmGetStatusLockerbox);
}

void Lockerbox::EXIT_TestingElm()
{
	s.Push(ST_READY);
}

void Lockerbox::ENTRY_Ready()
{
	SetDoorCommand(GetStatusLockerbox);
}

void Lockerbox::EXIT_Ready()
{
	//s.Push(ST_TESTING_ELM);
}

void Lockerbox::ENTRY_Processing()
{

}

void Lockerbox::EXIT_Processing()
{

}

void Lockerbox::ENTRY_NotReady()
{
//	SetDoorCommand();
}

void Lockerbox::EXIT_NotReady()
{
	//s.Push(ST_TESTING_ELM);
}
