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
	comm.EV_Send(current_address, current_command[current_address - 1], true);
	current_address++;
}

void Lockerbox::SetDoorCommand()
{
	// tutaj jest na pewno zly indeks
	for(uint8_t i = 0; i < MACHINE_MAX_NUMBER_OF_DOORS; i++)
	{
		if((uint8_t)mb.Read(LOCATIONS_NUMBER + 1 + i) != 0)
			current_command[i] = OpenLockerbox;
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

void Lockerbox::SetFaults(uint8_t st, uint8_t reply)
{
	for(uint8_t i = 0; i < 4; i++)
	{
		if(reply_fault_set[i].state == st)
		{
			if((reply_fault_set[i].reply == reply) && reply_fault_set[i].neg == false)
			{
				fault.SetGlobal(reply_fault_set[i].fault);
				fault.SetLocal(current_address - 1, reply_fault_set[i].fault);
				mb.Write(current_address, (reply_fault_set[i].fault << 8));
				//mb.Write(current_address, reply_fault_set[i].fault);
				if(reply_fault_set[i].fp != NULL) (this->*reply_fault_set[i].fp)(NULL);
			}
			if((reply_fault_set[i].reply != reply) && reply_fault_set[i].neg == true)
			{
				fault.SetGlobal(reply_fault_set[i].fault);
				fault.SetLocal(current_address - 1, reply_fault_set[i].fault);
				mb.Write(current_address, (reply_fault_set[i].fault << 8));
				//mb.Write(current_address, reply_fault_set[i].fault);
				if(reply_fault_set[i].fp != NULL) (this->*reply_fault_set[i].fp)(NULL);
			}
		}
	}
}

void Lockerbox::EV_Reply(MachineData* pdata)
{
	uint8_t state = GetState();
	mb.Write(current_address, pdata->data);
	SetFaults(state, pdata->data);

	static bool waiting_to_open = false;
	if(state == ST_PROCESSING && !waiting_to_open && current_command[current_address - 2] != GetStatusLockerbox)
	{
		waiting_to_open = true;
		SLAVE_POLL_STOP;
		return;
	}
	if(state == ST_PROCESSING && waiting_to_open && current_command[current_address - 2] != GetStatusLockerbox)
	{
		//if(pdata->data == 0x41)
		//	mb.Write(current_address, 0x41);
		//if(pdata->data == 0x07)
		//{
		//	fault.SetGlobal(F07_DoorNotOpen);
		//	mb.Write(current_address, F07_DoorNotOpen << 8);
		//}
		if(current_address <= 30)
			mb.WriteLo(current_address, pdata->data);
		else
			mb.WriteHi(current_address - 30, pdata->data);
		waiting_to_open = false;
		SLAVE_POLL_START;
	}
}

void Lockerbox::EV_Timeout(MachineData* pdata)
{
	fault.SetGlobal(F02_Door);
	fault.SetLocal(current_address - 1, F02_Door);
	if(current_address <= 31)
	{
		mb.SetBit(current_address, 5);
		mb.WriteLo(current_address, F02_Door);
	}
	//else
	//{
	//	mb.SetBit(current_address - 30, 13);
	//	mb.WriteHi(current_address - 30, F02_Door);
	//mb.Write(current_address, F02_Door << 8);
	//}
}
