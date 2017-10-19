/*
 * dynabox_states_events.cpp
 *
 *  Created on: 4 pa� 2017
 *      Author: tomek
 */

#include "machine.h"
#include "comm_prot.h"
#include "dynabox.h"
#include "motor.h"
#include "timer.h"
#include "display.h"

// ----------------------- States -----------------------

void Dynabox::ST_Init(DynaboxData* pdata)
{
	// ponizsze przypisanie dlatego, ze w config.cpp: m->InternalEvent(ST_INIT, NULL);
	pdata = &dynabox_data;
	pdata->comm_status = CommStatusRequest;
	EV_TestLed(pdata);
}

void Dynabox::ST_TestingLed(DynaboxData* pdata)
{
//	if(current_address == last_address + 1)
//	{
//		SLAVES_POLL_TIMEOUT_OFF; SLAVES_POLL_STOP;
//		return;
//	}
	if(pdata->comm_status == CommStatusRequest)
	{
		comm.Prepare(current_address++ + LED_ADDRESS_OFFSET, COMM_LED_DIAG);
		SLAVES_POLL_TIMEOUT_SET;
		return;
	}

	if(pdata->comm_status == CommStatusReply)
	{
		pdata->comm_status = CommStatusRequest;
		//current_address++;
		return;
	}
	//if(pdata->reply_type == ReplyTypeTimeout) { display.Write(current_address); return; }


}

void Dynabox::ST_CheckingElectromagnet(DynaboxData* pdata)
{
//	SetCurrentCommand(COMM_CHECK_ELECTROMAGNET);
}

void Dynabox::ST_Homing(DynaboxData* pdata)
{
	led_same_for_all = COMM_GREEN_RED_BLINK;
//	SetCurrentCommand(COMM_SHOW_STATUS_ON_LED);

	//SetCurrentCommand(COMM_CHECK_TRANSOPTORS_GET_SET_STATUS);
	motor.EV_Homing();
}

// ----------------------- Events -----------------------
void Dynabox::EV_TestLed(DynaboxData* pdata)
{
	pstate = &Dynabox::ST_TestingLed;
	current_address = 1;
	SLAVES_POLL_START;
	BEGIN_TRANSITION_MAP								// current state
        TRANSITION_MAP_ENTRY(ST_TESTING_LED)			// ST_INIT
//		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_TESTING_LED
//		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_TESTING_ELM
//		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_HOMING
	END_TRANSITION_MAP(pdata)
}

void Dynabox::EV_TestedLed(DynaboxData* pdata)
{
	BEGIN_TRANSITION_MAP								// current state
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_INIT
		TRANSITION_MAP_ENTRY(ST_TESTING_ELM)			// ST_TESTING_LED
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_TESTING_ELM
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_HOMING
	END_TRANSITION_MAP(pdata)
}

void Dynabox::EV_TestedElm(DynaboxData* pdata)
{
//	SetCurrentCommand(COMM_SHOW_STATUS_ON_LED);
	BEGIN_TRANSITION_MAP								// current state
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_INIT
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_TESTING_LED
		TRANSITION_MAP_ENTRY(ST_HOMING)					// ST_TESTING_ELM
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_HOMING
	END_TRANSITION_MAP(pdata)
}
