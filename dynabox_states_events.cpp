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
#include "display.h"

// ----------------------- States -----------------------
void Dynabox::ST_TestingLed(DynaboxData* pdata)
{
	comm.Prepare(current_address++ + LED_ADDRESS_OFFSET, COMM_LED_DIAG);
	SLAVE_POLL_TIMEOUT_SET;


//		fault.SetGlobal(F01_LED);
//		mb.UpdateHoldingRegister(current_address, F01_LED << 8);

	if(LastAddress())
	{
		SLAVE_POLL_TIMEOUT_OFF; SLAVE_POLL_STOP;
//		EV_TestElm(pdata);
	}
}

void Dynabox::ST_TestingElm(DynaboxData* pdata)
{
	comm.Prepare(current_address, COMM_DOOR_CHECK_ELECTROMAGNET);
	comm.Prepare(current_address + LED_ADDRESS_OFFSET, COMM_LED_GREEN_ON_FOR_TIME);
	current_address++;
	SLAVE_POLL_TIMEOUT_SET;

//		if(usart_data.frame[0] == current_address - 1)
//		{
//			if(usart_data.frame[1] == COMM_F05_ELECTROMAGNET)
//			{
//				fault.SetGlobal(F05_ELECTROMAGNET);
//				mb.UpdateHoldingRegister(current_address, F05_ELECTROMAGNET << 8);
//			}
//		}

//		fault.SetGlobal(F02_DOOR);
//		fault.Set(F02_DOOR, current_address - 1);
//		mb.UpdateHoldingRegister(current_address, F02_DOOR << 8);

	if(LastAddress())
	{
		SLAVE_POLL_TIMEOUT_OFF; SLAVE_POLL_STOP;
		EV_GetDoorsState(pdata);
	}
}

void Dynabox::ST_GettingDoorsState(DynaboxData* pdata)
{
		comm.Prepare(current_address++, 0x80);
		SLAVE_POLL_TIMEOUT_SET;

//		if(usart_data.frame[0] == current_address - 1)
//		{
			//if(usart_data.frame[1] == COMM_F05_ELECTROMAGNET)
			//{
			//	fault.Set(F05_ELECTROMAGNET);
//				mb.UpdateHoldingRegister(current_address, usart_data.frame[1]);
			//}
//		}

//		fault.SetGlobal(F02_DOOR);
//		mb.UpdateHoldingRegister(current_address, F02_DOOR << 8);


	if(LastAddress())
	{
		SLAVE_POLL_TIMEOUT_OFF; SLAVE_POLL_STOP;
		EV_NeedHoming(pdata);
	}
}

void Dynabox::ST_ShowingOnLed(DynaboxData* pdata)
{
	if(led_same_for_all)
	{
		comm.Prepare(current_address++ + LED_ADDRESS_OFFSET, faults_to_led_map[led_same_for_all_id] + 0x80);
	}
	else
	{
		if(fault.Check(F02_DOOR, current_address))
			comm.Prepare(current_address + LED_ADDRESS_OFFSET, faults_to_led_map[2] + 0x80);
		current_address++;
	}

	if(LastAddress())
	{
		SLAVE_POLL_STOP;
		comm.LedTrigger();
		led_same_for_all = false;
	}
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

void Dynabox::EV_ShowOnLed(DynaboxData* pdata)
{
	current_address = 1;
	SLAVE_POLL_START;
	InternalEvent(ST_SHOWING_ON_LED);
}

void Dynabox::EV_GetDoorsState(DynaboxData* pdata)
{
	current_address = 1;
	SLAVE_POLL_START;

	BEGIN_TRANSITION_MAP								// current state
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_TESTING_LED
		TRANSITION_MAP_ENTRY(ST_GETTING_DOORS_STATE)	// ST_TESTING_ELM
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_HOMING
	END_TRANSITION_MAP(pdata)
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

void Dynabox::EV_NeedHoming(DynaboxData* pdata)
{
	BEGIN_TRANSITION_MAP								// current state
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_TESTING_LED
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_TESTING_ELM
		TRANSITION_MAP_ENTRY(ST_HOMING)			// ST_GETTING_DOORS_STATE
	END_TRANSITION_MAP(pdata)
}
