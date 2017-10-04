/*
 * dynabox_states_events.cpp
 *
 *  Created on: 4 paü 2017
 *      Author: tomek
 */

#include "machine.h"
#include "comm_prot.h"
#include "dynabox.h"
#include "motor.h"

// ----------------------- States -----------------------

void Dynabox::ST_Init(DynaboxData* pdata)
{
	InternalEvent(ST_CHECKING_LED, NULL);
}

void Dynabox::ST_CheckingLED(DynaboxData* pdata)
{
	SetCurrentCommand(COMM_LED_DIAG);
}

void Dynabox::ST_CheckingElectromagnet(DynaboxData* pdata)
{
	SetCurrentCommand(COMM_CHECK_ELECTROMAGNET);
}

void Dynabox::ST_Homing(DynaboxData* pdata)
{
	led_same_for_all = COMM_GREEN_RED_BLINK;
	SetCurrentCommand(COMM_SHOW_STATUS_ON_LED);

	//SetCurrentCommand(COMM_CHECK_TRANSOPTORS_GET_SET_STATUS);
	motor.EV_Homing();
}

// ----------------------- Events -----------------------

void Dynabox::EV_LEDChecked(DynaboxData* pdata)
{
	BEGIN_TRANSITION_MAP								// current state
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_INIT
		TRANSITION_MAP_ENTRY(ST_CHECKING_ELECTROMAGNET)	// ST_CHECKING_LED
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_CHECKING_ELECTROMAGNET
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_HOMING
	END_TRANSITION_MAP(pdata)
}

void Dynabox::EV_ElectromagnetChecked(DynaboxData* pdata)
{
	SetCurrentCommand(COMM_SHOW_STATUS_ON_LED);
	BEGIN_TRANSITION_MAP								// current state
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_INIT
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_CHECKING_LED
		TRANSITION_MAP_ENTRY(ST_HOMING)					// ST_CHECKING_ELECTROMAGNET
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_HOMING
	END_TRANSITION_MAP(pdata)
}
