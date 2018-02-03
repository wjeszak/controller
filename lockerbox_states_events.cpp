/*
 * lockerbox_states_events.cpp
 *
 *  Created on: 12 sty 2018
 *      Author: wjeszak
 */

#include "lockerbox.h"
#include "machine.h"
#include "timer.h"
#include "modbus_tcp.h"
#include "stack.h"
#include "display.h"
#include "common.h"
#include "config.h"

// ---------------------------------- States ----------------------------------
void Lockerbox::ST0_TestingElm(LockerboxData* pdata)
{

}

void Lockerbox::ST1_Ready(LockerboxData* pdata)
{

}

void Lockerbox::ST2_Processing(LockerboxData* pdata)
{

}

void Lockerbox::ST3_NotReady(LockerboxData* pdata)
{

}

void Lockerbox::EV_EnterToConfig()
{
	FAULT_SHOW_STOP;
	SLAVE_POLL_STOP;
}

void Lockerbox::EV_UserAction(MachineData* pdata)
{
	if(mb.GetQuantity() > 1)
	{
		EV_UserActionClearFaults(&lockerbox_data);
		EV_UserActionGo(&lockerbox_data);
	}
	// ignore second command from Testbox reg. 150
	// clear faults command
	if(mb.Read((uint8_t)0) == 1)
	{
		EV_UserActionClearFaults(&lockerbox_data);
	}
}

void Lockerbox::EV_UserActionGo(MachineData* pdata)
{
	uint8_t door_id = 0;
	uint8_t i;
	has_been_readD0 = 0;
	for(i = 0; i <= 29; i++)
	{
		if(mb.ReadLo(FIRST_DOOR_CONTROL + i) != 0)
			door_id = i + 1;
	}

	for(i = 0; i <= 29; i++)
	{
		if(mb.ReadHi(FIRST_DOOR_CONTROL + i) != 0)
			door_id = i + 1 + 30;
	}

	if(door_id > functions[1].param)
	{
		fault.SetGlobal(F16_OrderRefused);
		return;
	}
	s.Push(ST_PROCESSING);
}

void Lockerbox::EV_UserActionClearFaults(MachineData* pdata)
{
	if(fault.IsGlobal(F07_DoorNotOpen))
		fault.ClearGlobal(F07_DoorNotOpen);

	if(fault.IsGlobal(F16_OrderRefused))
		fault.ClearGlobal(F16_OrderRefused);
}
