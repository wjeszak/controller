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
	SLAVE_POLL_STOP;
}

void Lockerbox::EV_UserAction(MachineData* pdata)
{
	if(mb.GetQuantity() > 1)
	{
		EV_UserActionGo(&lockerbox_data);
	}

	if(mb.GetQuantity() == 1)
	//if(GetOrderStatus() == GoAck)
	{
		//SetOrderStatus(Ready);
		EV_UserActionClearFaults(&lockerbox_data);
	}
}

void Lockerbox::EV_UserActionGo(MachineData* pdata)
{
	//display.Write(1234);
	s.Push(ST_PROCESSING);
}

void Lockerbox::EV_UserActionClearFaults(MachineData* pdata)
{
	//f.Set(NeedFaultsClear);
}
