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
/*	uint8_t door_id = 0;

	for(uint8_t i = 0; i <= 29; i++)
	{
		uint16_t reg = mb.Read(FIRST_DOOR_CONTROL + i);
		uint8_t reg_hi = reg >> 8;
		uint8_t reg_lo = reg & 0xFF;
		//if((reg_lo) != 0)
		//	door_id = i + 1;

		if((reg) > 512)
			door_id = i + 1 + 30;
	}
	if(door_id > 37)
		fault.SetGlobal(F16_OrderRefused);
	else
		s.Push(ST_PROCESSING);
*/
}

void Lockerbox::EV_UserActionClearFaults(MachineData* pdata)
{
	//f.Set(NeedFaultsClear);
	if(fault.IsGlobal(F07_DoorNotOpen))
	{
		fault.ClearGlobal(F07_DoorNotOpen);
		// kasowanie bledow w drzwiach
	}

//if(fault.IsGlobal(F16_OrderRefused))
	//{
	//	fault.ClearGlobal(F16_OrderRefused);
	//	// kasowanie bledow w drzwiach
	//}
}
