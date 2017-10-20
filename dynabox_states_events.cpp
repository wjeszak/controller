/*
 * dynabox_states_events.cpp
 *
 *  Created on: 4 paŸ 2017
 *      Author: tomek
 */

#include "timer.h"
#include "config.h"
#include "comm_prot.h"
#include "dynabox.h"
#include "motor.h"
#include "modbus_tcp.h"
#include "fault.h"

// ----------------------- States -----------------------
void Dynabox::ST_TestingLed(DynaboxData* pdata)
{
	if(current_address == functions[1].param + 1)
	{
		SLAVE_POLL_TIMEOUT_OFF; SLAVE_POLL_STOP;
		//EV_TestedLed(NULL);
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

void Dynabox::ST_CheckingElectromagnet(DynaboxData* pdata)
{

}

void Dynabox::ST_Homing(DynaboxData* pdata)
{
	led_same_for_all = COMM_GREEN_RED_BLINK;
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

void Dynabox::EV_TestedLed(DynaboxData* pdata)
{
	BEGIN_TRANSITION_MAP								// current state
		TRANSITION_MAP_ENTRY(ST_TESTING_ELM)			// ST_TESTING_LED
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_TESTING_ELM
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_HOMING
	END_TRANSITION_MAP(pdata)
}

void Dynabox::EV_TestedElm(DynaboxData* pdata)
{
	BEGIN_TRANSITION_MAP								// current state
 //       TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_INIT
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_TESTING_LED
		TRANSITION_MAP_ENTRY(ST_HOMING)					// ST_TESTING_ELM
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_HOMING
	END_TRANSITION_MAP(pdata)
}
