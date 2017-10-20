/*
 * dynabox_states_events.cpp
 *
 *  Created on: 4 paü 2017
 *      Author: tomek
 */

#include "timer.h"
#include "config.h"
#include "comm_prot.h"
#include "dynabox.h"
#include "motor.h"
#include "modbus_tcp.h"
#include "usart.h"
#include "fault.h"
#include <util/delay.h>

// ----------------------- States -----------------------
void Dynabox::ST_TestingLed(DynaboxData* pdata)
{
	if(current_address == functions[1].param + 1)
	{
		SLAVE_POLL_TIMEOUT_OFF; SLAVE_POLL_STOP;
		EV_TestElm(pdata);
		return;
	}

	switch(pdata->comm_status)
	{
	case CommStatusRequest:
		comm.Prepare(current_address++ + LED_ADDRESS_OFFSET, COMM_LED_DIAG);
		SLAVE_POLL_TIMEOUT_SET;
		return;
	break;
	case CommStatusReply:
		//if(frame[0] == current_address + LED_ADDRESS_OFFSET)
		pdata->comm_status = CommStatusRequest;
		return;
	break;
	case CommStatusTimeout:
		fault.Set(F01_LED);
		mb.UpdateHoldingRegister(current_address, F01_LED << 8);
		pdata->comm_status = CommStatusRequest;
		return;
	break;
	}
}

void Dynabox::ST_TestingElm(DynaboxData* pdata)
{
	if(current_address == functions[1].param + 1)
	{
		SLAVE_POLL_TIMEOUT_OFF; SLAVE_POLL_STOP;
		EV_NeedMovement(pdata);
		return;
	}

	switch(pdata->comm_status)
	{
	case CommStatusRequest:
		comm.Prepare(current_address, COMM_DOOR_CHECK_ELECTROMAGNET);
		comm.Prepare(current_address + LED_ADDRESS_OFFSET, COMM_LED_GREEN_ON_FOR_TIME);
		current_address++;
		SLAVE_POLL_TIMEOUT_SET;
		return;
	break;
	case CommStatusReply:
		if(usart_data.frame[0] == current_address - 1)
		{
			if(usart_data.frame[1] == COMM_F05_ELECTROMAGNET)
			{
				fault.Set(F05_ELECTROMAGNET);
				mb.UpdateHoldingRegister(current_address, F05_ELECTROMAGNET << 8);
			}
		}
		pdata->comm_status = CommStatusRequest;
		return;
	break;
	case CommStatusTimeout:
		fault.Set(F02_DOOR);
		mb.UpdateHoldingRegister(current_address, F02_DOOR << 8);
		pdata->comm_status = CommStatusRequest;
		return;
	break;
	}
}

void Dynabox::ST_Homing(DynaboxData* pdata)
{
	led_same_for_all = COMM_LED_GREEN_RED_BLINK;
	motor.EV_Homing();
}

// ----------------------- Events -----------------------
void Dynabox::EV_TestLed(DynaboxData* pdata)
{
	pstate = &Dynabox::ST_TestingLed;
	current_address = 1;
	SLAVE_POLL_START;
//	BEGIN_TRANSITION_MAP								// current state
//        TRANSITION_MAP_ENTRY(ST_TESTING_LED)			// ST_INIT
//		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_TESTING_LED
//		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_TESTING_ELM
//		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_HOMING
//	END_TRANSITION_MAP(pdata)
}

void Dynabox::EV_TestElm(DynaboxData* pdata)
{
	pstate = &Dynabox::ST_TestingElm;
	current_address = 1;
	SLAVE_POLL_START;

	BEGIN_TRANSITION_MAP								// current state
		TRANSITION_MAP_ENTRY(ST_TESTING_ELM)			// ST_TESTING_LED
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_TESTING_ELM
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_HOMING
	END_TRANSITION_MAP(pdata)
}

void Dynabox::EV_NeedMovement(DynaboxData* pdata)
{
	for(uint8_t i = 1; i <= 7; i++)
	{
		_delay_ms(500);
		comm.Prepare(i + LED_ADDRESS_OFFSET, COMM_LED_GREEN_3PULSES); //+ 0x80);
	}
//	BEGIN_TRANSITION_MAP								// current state
//		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_TESTING_LED
//		TRANSITION_MAP_ENTRY(ST_HOMING)					// ST_TESTING_ELM
//		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_HOMING
//	END_TRANSITION_MAP(pdata)
}
