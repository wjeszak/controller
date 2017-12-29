/*
 * dynabox.cpp
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#include "timer.h"
#include "dynabox.h"
#include "comm.h"
#include "dynabox_commands_faults.h"
#include "fault.h"
#include "modbus_tcp.h"
#include "usart.h"
#include "stack.h"
#include "display.h"

Dynabox::Dynabox()
{
	SetOrderStatus(Ready);
	home_ok = false;
	last_position = 1;
}

void Dynabox::Init()
{
	ENTRY_TestingLed(&dynabox_data);
	SLAVE_POLL_START;
}

void Dynabox::StateManager()
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
	comm.EV_Send(GetDestAddr(state), current_command[current_address - 1] , state_properties[state].need_timeout);
	InternalEventEx(state, &dynabox_data);
	current_address++;
}

void Dynabox::SetDestAddr(uint8_t addr)
{
	current_address = addr;
}

uint8_t Dynabox::GetDestAddr(uint8_t st)
{
	if(state_properties[st].dest == Dest_Led)
		return current_address + LED_ADDRESS_OFFSET;
	return current_address;
}

void Dynabox::SetDoorCommand()
{
	for(uint8_t i = 0; i < MACHINE_MAX_NUMBER_OF_DOORS; i++)
	{
		if(desired_doors_position[i] != 0)
			current_command[i] = SetPosition + desired_doors_position[i];
		else
			current_command[i] = GetStatus;
	}
}

void Dynabox::SetDoorCommand(DoorCommand command)
{
	for(uint8_t i = 0; i < MACHINE_MAX_NUMBER_OF_DOORS; i++)
	{
		current_command[i] = command;
	}
}

void Dynabox::SetLedCommand(bool queued)
{
	for(uint8_t i = 0; i < MACHINE_MAX_NUMBER_OF_DOORS; i++)
	{
		uint8_t fault = mb.Read(i + 2) >> 8;
		if(queued)
			current_command[i] = fault_to_led[fault] + NeedQueue;
		else
			current_command[i] = fault_to_led[fault];
	}
}

void Dynabox::SetLedCommand(LedCommand command, bool queued)
{
	for(uint8_t i = 0; i < MACHINE_MAX_NUMBER_OF_DOORS; i++)
	{
		if(queued)
			current_command[i] = command + NeedQueue;
		else
			current_command[i] = command;
	}
}

void Dynabox::SetFaults(uint8_t st, uint8_t reply)
{
	for(uint8_t i = 0; i < ST_MAX_STATES; i++)
	{
		if(reply_fault_set[i].state == st)
		{
			if((reply_fault_set[i].reply == reply) && reply_fault_set[i].neg == false)
			{
				fault.SetGlobal(reply_fault_set[i].fault);
				mb.Write(current_address, reply_fault_set[i].fault << 8);
				if(reply_fault_set[i].fp != NULL) (this->*reply_fault_set[i].fp)(NULL);
			}
			if((reply_fault_set[i].reply != reply) && reply_fault_set[i].neg == true)
			{
				fault.SetGlobal(reply_fault_set[i].fault);
				mb.Write(current_address, reply_fault_set[i].fault << 8);
				if(reply_fault_set[i].fp != NULL) (this->*reply_fault_set[i].fp)(NULL);
			}
		}
	}
}

void Dynabox::EV_Reply(MachineData* pdata)
{
	uint8_t state = GetState();
	mb.Write(current_address, d->data);
	SetFaults(state, pdata->data);
}

void Dynabox::EV_Timeout(MachineData* pdata)
{
	// led's fault
	if(state_properties[GetState()].dest == Dest_Led)
	{
		fault.SetGlobal(F01_Led);
		fault.Set(F01_Led, current_address - 1);
		mb.Write(current_address, F01_Led << 8);
	}
	// door's fault
	else
	{
		fault.SetGlobal(F02_Door);
		fault.Set(F02_Door, current_address - 1);
		mb.Write(current_address, F02_Door << 8);
	}
}
