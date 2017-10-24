/*
 * dynabox.cpp
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#include "timer.h"
#include "comm_prot.h"
#include "dynabox.h"
#include "fault.h"
#include "modbus_tcp.h"

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
	led_same_for_all = false;
	led_same_for_all_id = 0;
}

void Dynabox::Init()
{
	EV_TestLed(&dynabox_data);
}

void Dynabox::EV_EnterToConfig()
{
	SLAVE_POLL_STOP;
	timer.Disable(TIMER_FAULT_SHOW);
}

void Dynabox::Scheduler()
{
	InternalEvent(GetState(), &dynabox_data);
}

void Dynabox::EV_Parse(uint8_t* frame)
{
	SLAVE_POLL_TIMEOUT_OFF;
	if(comm.Crc8(frame, 2) == frame[2])
	{
//		dynabox_data.comm_status = CommStatusReply;
//		(this->*pstate)(&dynabox_data);
	}
}

void Dynabox::EV_Timeout()
{
	SLAVE_POLL_TIMEOUT_OFF;
	uint8_t st = GetState();
	switch (st)
	{
	case ST_TESTING_LED:
		fault.SetGlobal(F01_LED);
		fault.Set(F01_LED, current_address - 1);
		mb.UpdateHoldingRegister(current_address, F01_LED << 8);
	break;
	case ST_TESTING_ELM:
		fault.SetGlobal(F02_DOOR);
		fault.Set(F02_DOOR, current_address - 1);
		mb.UpdateHoldingRegister(current_address, F02_DOOR << 8);
	break;
	}
}
