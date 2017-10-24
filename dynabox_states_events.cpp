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
#include "usart.h"
#include "fault.h"

// ----------------------- States -----------------------
void Dynabox::ST_TestingLed(DynaboxData* pdata)
{
	comm.Prepare(current_address + LED_ADDRESS_OFFSET, COMM_LED_DIAG);
	SLAVE_POLL_TIMEOUT_SET;

	if(LastAddress())
	{
		SLAVE_POLL_STOP;
		next_EV = &Dynabox::EV_TestElm;
		timer.Assign(TIMER_DELAY_BETWEEN_STATES, DELAY_BETWEEN_STATES, DelayBetweenStates);
	}
	current_address++;
}

void Dynabox::ST_TestingElm(DynaboxData* pdata)
{
	comm.Prepare(current_address, COMM_DOOR_CHECK_ELECTROMAGNET);
	comm.Prepare(current_address + LED_ADDRESS_OFFSET, COMM_LED_GREEN_ON_FOR_TIME);
	SLAVE_POLL_TIMEOUT_SET;

	if(LastAddress())
	{
		SLAVE_POLL_STOP;
		next_EV = &Dynabox::EV_NeedHoming;
		timer.Assign(TIMER_DELAY_BETWEEN_STATES, DELAY_BETWEEN_STATES, DelayBetweenStates);
	}
	current_address++;
}

void Dynabox::ST_PreparingToHoming(DynaboxData* pdata)
{
	comm.Prepare(current_address, 0x80);
	SLAVE_POLL_TIMEOUT_SET;

	if(LastAddress())
	{
		SLAVE_POLL_STOP;
		next_EV = &Dynabox::EV_PreparedToHoming;
		timer.Assign(TIMER_DELAY_BETWEEN_STATES, DELAY_BETWEEN_STATES, DelayBetweenStates);
	}
	current_address++;
}

void Dynabox::ST_ShowingOnLed(DynaboxData* pdata)
{
	if(led_same_for_all)
	{
		comm.Prepare(current_address + LED_ADDRESS_OFFSET, faults_to_led_map[led_same_for_all_id] + 0x80);
	}
	else
	{
		//if(fault.Check(F02_DOOR, current_address))
			comm.Prepare(current_address + LED_ADDRESS_OFFSET, faults_to_led_map[mb.GetHoldingRegister(current_address + 1) >> 8] + 0x80);

	}

	if(LastAddress())
	{
		SLAVE_POLL_STOP;
		comm.LedTrigger();
		led_same_for_all = false;
	}
	current_address++;
}

void Dynabox::ST_Homing(DynaboxData* pdata)
{
	motor.EV_Homing();
}

void Dynabox::ST_Ready(DynaboxData* pdata)
{

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
	current_address = 1;
	SLAVE_POLL_START;

	BEGIN_TRANSITION_MAP								// current state
		TRANSITION_MAP_ENTRY(ST_TESTING_ELM)			// ST_TESTING_LED
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_TESTING_ELM
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_HOMING
	END_TRANSITION_MAP(pdata)
}

void Dynabox::EV_NeedHoming(DynaboxData* pdata)
{
	current_address = 1;
	SLAVE_POLL_START;

	BEGIN_TRANSITION_MAP								// current state
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_TESTING_LED
		TRANSITION_MAP_ENTRY(ST_PREPARING_TO_HOMING)	// ST_TESTING_ELM
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_HOMING
	END_TRANSITION_MAP(pdata)
}

void Dynabox::EV_PreparedToHoming(DynaboxData* pdata)
{
	bool is_fault = false;
	for(uint8_t i = 1; i <= functions[1].param; i++)
	{
		if(mb.GetHoldingRegister(i + 1) != 0xC0)
		{
			fault.SetGlobal(F06_CLOSE_THE_DOOR);
			fault.Set(F06_CLOSE_THE_DOOR, i);
			mb.UpdateHoldingRegister(i + 1, F06_CLOSE_THE_DOOR << 8);
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
	}
	EV_ShowOnLed(pdata);
	is_fault = false;
}

void Dynabox::EV_ShowOnLed(DynaboxData* pdata)
{
	current_address = 1;
	SLAVE_POLL_START;
	InternalEvent(ST_SHOWING_ON_LED);
}

void Dynabox::EV_UserAction(MachineData* pdata)
{
	BEGIN_TRANSITION_MAP								// current state
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_TESTING_LED
		TRANSITION_MAP_ENTRY(ST_HOMING)					// ST_TESTING_ELM
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_CHECKING_DOORS_STATE
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_HOMING
	END_TRANSITION_MAP(pdata)
}
