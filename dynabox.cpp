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

Dynabox::Dynabox()
{

}

void Dynabox::Init()
{
	ENTRY_TestingLed(&dynabox_data);
	SLAVE_POLL_START;
}

void Dynabox::StateManager()
{
	uint8_t state = GetState();
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
	// wyrzucic InternalEventEx() bo jest mylacy
	// zamiast tego dodac wskaznik na funkcje wykonujaca sie ciagle
	// wywolanie funkcji od biezacego stanu: ST_...
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

void Dynabox::SetCommand(uint8_t command)
{
	for(uint8_t i = 0; i < MACHINE_MAX_NUMBER_OF_DOORS; i++)
		current_command[i] = command;
}

void Dynabox::SetFaults(uint8_t st, uint8_t reply)
{
	for(uint8_t i = 0; i < 3; i++)
	{
		if(reply_fault_set[i].state == st)
		{
			if((reply_fault_set[i].reply == reply) && reply_fault_set[i].neg == false)
			{
				fault.SetGlobal(reply_fault_set[i].fault);
				//fault.Set(F01_LED, current_address - 1);
				mb.Write(current_address, reply_fault_set[i].fault << 8);
				if(reply_fault_set[i].fp != NULL) (this->*reply_fault_set[i].fp)(NULL);
			}
			if((reply_fault_set[i].reply != reply) && reply_fault_set[i].neg == true)
			{
				fault.SetGlobal(reply_fault_set[i].fault);
				//fault.Set(F01_LED, current_address - 1);
				mb.Write(current_address, reply_fault_set[i].fault << 8);
				if(reply_fault_set[i].fp != NULL) (this->*reply_fault_set[i].fp)(NULL);
			}
		}
	}
}

void Dynabox::EV_Reply(MachineData* pdata)
{
	uint8_t state = GetState();
	SetFaults(state, pdata->data);
}

void Dynabox::EV_Timeout(MachineData* pdata)
{
	// led's fault
	if(pdata->addr > LED_ADDRESS_OFFSET)
	{
		fault.SetGlobal(F01_LED);
		fault.Set(F01_LED, current_address - 1);
		mb.Write(current_address, F01_LED << 8);
	}
	// door's fault
	else
	{
		fault.SetGlobal(F02_DOOR);
		fault.Set(F02_DOOR, current_address - 1);
		mb.Write(current_address, F02_DOOR << 8);
	}
}
