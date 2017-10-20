/*
 * dynabox.cpp
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#include "timer.h"
#include "comm_prot.h"
#include "dynabox.h"

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
	led_same_for_all = 0;
	pstate = NULL;
}

void Dynabox::Init()
{
	dynabox_data.comm_status = CommStatusRequest;
	EV_TestLed(&dynabox_data);
}

void Dynabox::EV_EnterToConfig()
{
	SLAVE_POLL_STOP;
	timer.Disable(TIMER_FAULT_SHOW);
}

void Dynabox::SlavePoll()
{
	(this->*pstate)(&dynabox_data);
}

void Dynabox::SlaveParse(uint8_t* frame)
{
	SLAVE_POLL_TIMEOUT_OFF;
	if(comm.Crc8(frame, 2) == frame[2])
	{
		dynabox_data.comm_status = CommStatusReply;
		(this->*pstate)(&dynabox_data);
	}
}

void Dynabox::SlaveTimeout()
{
	SLAVE_POLL_TIMEOUT_OFF;
	dynabox_data.comm_status = CommStatusTimeout;
	(this->*pstate)(&dynabox_data);
}
