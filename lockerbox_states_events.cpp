/*
 * lockerbox_states_events.cpp
 *
 *  Created on: 12 sty 2018
 *      Author: wjeszak
 */

#include "lockerbox.h"
#include "timer.h"
#include "modbus_tcp.h"

// ---------------------------------- States ----------------------------------
void Lockerbox::ST0_TestingElm(LockerboxData* pdata)
{

}

void Lockerbox::ST1_Ready(LockerboxData* pdata)
{
	//comm.EV_Send(current_address, , true);
}

void Lockerbox::ST2_Processing(LockerboxData* pdata)
{
	//comm.EV_Send(current_address, , true);
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
	for(uint8_t i = 0; i < MACHINE_MAX_NUMBER_OF_DOORS; i++)
	{
		if((uint8_t)mb.Read(LOCATIONS_NUMBER + 1 + i) != 0)
		{
			door_need_open |= (1ULL << i);
		}
	}
}

void Lockerbox::EV_UserActionClearFaults(MachineData* pdata)
{
	//f.Set(NeedFaultsClear);
}
