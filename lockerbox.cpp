/*
 * lockerbox.cpp
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#include "lockerbox.h"
#include "timer.h"
#include "display.h"
#include "fault.h"
#include "flags.h"
#include "stack.h"
#include "modbus_tcp.h"

Lockerbox::Lockerbox()
{
	SetOrderStatus(Ready);
	door_need_open = 0;
}

void Lockerbox::Init()
{
	ENTRY_TestingElm(&lockerbox_data);
	SLAVE_POLL_START;
}

void Lockerbox::Maintenance()
{

}

void Lockerbox::StateManager()
{
	uint8_t state = GetState();
	//display.Write(state);
//	static uint8_t fault_show_cnt = 0;
//	fault_show_cnt++;
//	if(fault_show_cnt == FAULT_SHOW_TICK)
	//{
	//	fault_show_cnt = 0;
		if(f.Get(NeedFaultsClear))
		{
	//		f.Clear(NeedFaultsClear);
	//		if(fault.IsGlobal(F07_DoorNotOpen)) fault.ClearGlobal(F07_DoorNotOpen);
		}
	//	fault.Show();
	//}

	if(current_address == LastAddress() + 1)
	{
		SetDestAddr(1);
		if(state_properties[state].on_exit != NULL) (this->*state_properties[state].on_exit)();
		if(!s.IsEmpty())
		{
			uint8_t new_state = s.Pop();
			ChangeState(new_state);
			if(state_properties[new_state].on_entry != NULL) (this->*state_properties[new_state].on_entry)();
		}
		return;
	}

	//DoorOpenTimeoutManager();

	InternalEventEx(state, &lockerbox_data);
	comm.EV_Send(GetDestAddr(state), current_command[current_address - 1] , true);
	current_address++;
}

void Lockerbox::SetDoorCommand()
{
	// tutaj jest na pewno zly indeks
	for(uint8_t i = 0; i < MACHINE_MAX_NUMBER_OF_DOORS; i++)
	{
		if((uint8_t)mb.Read(LOCATIONS_NUMBER + 1 + i) != 0)
			current_command[i] = OpenLockerbox + 1;
		else
			current_command[i] = GetStatusLockerbox;
	}
}

void Lockerbox::SetDoorCommand(DoorCommand command)
{
	for(uint8_t i = 0; i < MACHINE_MAX_NUMBER_OF_DOORS; i++)
	{
		current_command[i] = command;
	}
}

void Lockerbox::SetDestAddr(uint8_t addr)
{
	current_address = addr;
}

uint8_t Lockerbox::GetDestAddr(uint8_t st)
{
	return current_address;
}

void Lockerbox::EV_Reply(MachineData* pdata)
{
	uint8_t state = GetState();
	mb.Write(current_address, pdata->data);
//	SetFaults(state, pdata->data);
}

void Lockerbox::EV_Timeout(MachineData* pdata)
{
	fault.SetGlobal(F02_Door);
	fault.SetLocal(current_address - 1, F02_Door);
	mb.Write(current_address, F02_Door << 8);
}
