/*
 * dynabox.cpp
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#include "timer.h"
#include "dynabox.h"

#include "comm.h"
#include "fault.h"
#include "modbus_tcp.h"
#include "usart.h"
//#include "display.h"

Dynabox::Dynabox() : faults_to_led_map {
	0,									// not used
	0,									// F01, impossible
	COMM_LED_RED_1PULSE,					// F02
	COMM_LED_RED_2PULSES,					// F03
	COMM_LED_RED_ON,						// F04
	COMM_LED_RED_3PULSES,					// F05
	COMM_LED_GREEN_RED_BLINK, 				// F06
	COMM_LED_GREEN_RED_BLINK,				// F07
	COMM_LED_GREEN_RED_BLINK,				// F08
	0,									// not used
	0, 									// F10, not used
	COMM_LED_RED_BLINK,						// F11
	COMM_LED_RED_BLINK, 					// F12
	COMM_LED_RED_ON, 						// F13
	COMM_LED_RED_BLINK, 					// F14
	0,									// F15, not used
	0, 									// F16, not used
	COMM_LED_RED_BLINK	 					// F17
}
{

}

void Dynabox::Init()
{
	EV_TestLed(&dynabox_data);
	// never disable
	SLAVE_POLL_START;
}

void Dynabox::EV_EnterToConfig()
{
	SLAVE_POLL_STOP;
	timer.Disable(TIMER_FAULT_SHOW);
}

uint8_t Dynabox::GetDestAddr(uint8_t st)
{
	if(state_prop[st].dest == Dest_Led)
		return current_address + LED_ADDRESS_OFFSET;
	return current_address;
}

void Dynabox::SetDestAddr(uint8_t addr)
{
	current_address = addr;
}

void Dynabox::SetFaults(uint8_t st, uint8_t reply)
{
	for(uint8_t i = 0; i < 3; i++)
	{
		if(set_state_fault[i].state == st)
		{
			if((set_state_fault[i].reply == reply) && set_state_fault[i].neg == false)
			{
				fault.SetGlobal(set_state_fault[i].fault);
				//fault.Set(F01_LED, current_address - 1);
				mb.Write(current_address, set_state_fault[i].fault << 8);
			}
			if((set_state_fault[i].reply != reply) && set_state_fault[i].neg == true)
			{
				fault.SetGlobal(set_state_fault[i].fault);
				//fault.Set(F01_LED, current_address - 1);
				mb.Write(current_address, set_state_fault[i].fault << 8);
			}
		}
	}
}

void Dynabox::Poll()
{
	uint8_t state = GetState();
	if(end_state)
	{
		end_state = false;
		SetDestAddr(1);
		if(state_prop[state].repeat == false)
		{
			if(state_prop[state].on_exit != NULL) (this->*state_prop[state].on_exit)();
			state = state_prop[state].next_state;
			ChangeState(state);
			if(state_prop[state].on_entry != NULL) (this->*state_prop[state].on_entry)();
		}
		return;
	}
	//if(state == ST_SHOWING_ON_LED)
	//{
		//comm.EV_Send(GetDestAddr(state), 0x0A + 0x80, false);
		//current_address++;
	//}
	//else
		comm.EV_Send(GetDestAddr(state), state_prop[state].command , state_prop[state].need_timeout);
	InternalEvent(state, &dynabox_data);
}

void Dynabox::EV_ReplyOK(MachineData* pdata)
{
	if(current_address == LastAddress())
		end_state = true;
	else
		current_address++;
	uint8_t state = GetState();
	SetFaults(state, pdata->data);
}

void Dynabox::EV_Timeout(MachineData* pdata)
{
	if(current_address == LastAddress())
		end_state = true;
	else
		current_address++;
	// led's fault
	if(pdata->addr > 100)
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
