/*
 * dynabox_states_events.cpp
 *
 *  Created on: 4 paŸ 2017
 *      Author: tomek
 */

#include "comm.h"
#include "timer.h"
#include "config.h"
#include "dynabox.h"
#include "motor.h"
#include "modbus_tcp.h"
#include "usart.h"
#include "fault.h"

// ----------------------- States -----------------------
void Dynabox::ST_TestingLed(DynaboxData* pdata)
{

}

void Dynabox::ST_TestingElm(DynaboxData* pdata)
{
	comm.EV_Send(current_address + LED_ADDRESS_OFFSET, 0x0C, false);
}

void Dynabox::ST_PreparingToHoming(DynaboxData* pdata)
{

}

void Dynabox::ST_ShowingOnLed(DynaboxData* pdata)
{

}

void Dynabox::ST_HomingOnEntry()
{
	motor.EV_Homing();
}

void Dynabox::ST_Homing(DynaboxData* pdata)
{
	mb.Write(current_address + 1, d->data);
}

void Dynabox::ST_ShowingOnLedOnExit()
{
	comm.EV_LedTrigger();
}

void Dynabox::ST_Ready(DynaboxData* pdata)
{
	mb.Write(current_address + 1, d->data);
}

void Dynabox::ST_NotReady(DynaboxData* pdata)
{

}

void Dynabox::ST_Config(DynaboxData* pdata)
{

}
// ----------------------- Events -----------------------
void Dynabox::EV_TestLed(DynaboxData* pdata)
{
//	BEGIN_TRANSITION_MAP								// current state
//        TRANSITION_MAP_ENTRY(ST_TESTING_LED)			// ST_INIT
//		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_TESTING_LED
//		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_TESTING_ELM
//		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_HOMING
//	END_TRANSITION_MAP(pdata)
}

void Dynabox::EV_TestElm(DynaboxData* pdata)
{
/*	current_address = 1;
	SLAVE_POLL_START;

	BEGIN_TRANSITION_MAP								// current state
		TRANSITION_MAP_ENTRY(ST_TESTING_ELM)			// ST_TESTING_LED
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_TESTING_ELM
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_HOMING
	END_TRANSITION_MAP(pdata)
*/
}

void Dynabox::EV_NeedHoming(DynaboxData* pdata)
{
/*	current_address = 1;
	SLAVE_POLL_START;

	BEGIN_TRANSITION_MAP								// current state
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_TESTING_LED
		TRANSITION_MAP_ENTRY(ST_PREPARING_TO_HOMING)	// ST_TESTING_ELM
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_HOMING
	END_TRANSITION_MAP(pdata)
*/
}

void Dynabox::EV_HomingDone(DynaboxData* pdata)
{
	InternalEvent(ST_READY);
}

void Dynabox::EV_PreparedToHoming(DynaboxData* pdata)
{
/*	bool is_fault = false;
	for(uint8_t i = 1; i <= functions[1].param; i++)
	{
		if(mb.Read(i + 1) != 0xC0)
		{
			fault.SetGlobal(F06_CLOSE_THE_DOOR);
			fault.Set(F06_CLOSE_THE_DOOR, i);
			mb.Write(i + 1, F06_CLOSE_THE_DOOR << 8);
			is_fault = true;
		}
	}
	if(is_fault)
	{
		led_same_for_all = false;
	}
	else
	{
		led_same_for_all = true;
		led_same_for_all_id = 6;
		InternalEvent(ST_HOMING);
	}
	EV_ShowOnLed(pdata);
	is_fault = false;
	*/
}

void Dynabox::EV_ShowOnLed(DynaboxData* pdata)
{
/*	current_address = 1;
	SLAVE_POLL_START;
	InternalEvent(ST_SHOWING_ON_LED);
*/
}

void Dynabox::EV_UserAction(MachineData* pdata)
{
	if(mb.Read(LOCATIONS_NUMBER) > 0)
	{
		motor_data.pos = (mb.Read(LOCATIONS_NUMBER) - 1);
		//motor_data.pos = 100 * (mb.Read(LOCATIONS_NUMBER) - 1);
		motor.EV_RunToPosition(&motor_data);
	}
	//	BEGIN_TRANSITION_MAP								// current state
//		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_TESTING_LED
//		TRANSITION_MAP_ENTRY(ST_HOMING)					// ST_TESTING_ELM
//		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_CHECKING_DOORS_STATE
//		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_HOMING
//	END_TRANSITION_MAP(pdata)
}

void Dynabox::EV_OnF8(DynaboxData* pdata)
{
	motor.Stop();
}
