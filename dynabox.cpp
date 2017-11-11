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
//	led_same_for_all = false;
//	led_same_for_all_id = 0;
//	states_map[0].state = ST_TESTING_LED;
//	states_map[0].command = COMM_LED_DIAG;
//	states_map[0].reply = 0x80;
//	states_map[0].fault = F01_LED;
//	state_poll_repeat[ST_TESTING_LED] = false;
//	state_poll_repeat[ST_TESTING_ELM] = false;
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
	comm.EV_Send(GetDestAddr(state), state_prop[state].command , state_prop[state].need_timeout);
	InternalEvent(state, &dynabox_data);
}

void Dynabox::EV_ReplyOK(MachineData* pdata)
{
//	display.Write(4444);
	if(current_address == LastAddress())
		end_state = true;
	else
		current_address++;
/*	if(comm.CrcOk(frame))
	{
		if(1)	// !!!!!!!!!!!1
		{
			uint8_t st = GetState();
			switch(st)
			{
			case ST_TESTING_ELM:
				if(usart_data.frame[1] == COMM_F05_ELECTROMAGNET)
				{
					fault.SetGlobal(F05_ELECTROMAGNET);
					fault.Set(F05_ELECTROMAGNET, current_address - 1);
					mb.Write(current_address, F05_ELECTROMAGNET << 8);
				}
			break;
			case ST_PREPARING_TO_HOMING:
				mb.Write(current_address, frame[1]);
			break;
			case ST_READY:
				mb.Write(current_address, frame[1]);
			break;
			}
		}
	}
*/
}

void Dynabox::EV_Timeout(MachineData* pdata)
{
	if(current_address == LastAddress())
		end_state = true;
	else
		current_address++;
	if(pdata->addr > 100)
	{
		fault.SetGlobal(F01_LED);
		fault.Set(F01_LED, current_address - 1);
		mb.Write(current_address, F01_LED << 8);
	}
	else
	{
		fault.SetGlobal(F02_DOOR);
		fault.Set(F02_DOOR, current_address - 1);
		mb.Write(current_address, F02_DOOR << 8);
	}
}
