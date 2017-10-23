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
#include <util/delay.h>

// ----------------------- States -----------------------
void Dynabox::ST_TestingLed(DynaboxData* pdata)
{
	switch(pdata->comm_status)
	{
	case CommStatusRequest:
		comm.Prepare(current_address++ + LED_ADDRESS_OFFSET, COMM_LED_DIAG);
		SLAVE_POLL_TIMEOUT_SET;
//		return;
	break;
	case CommStatusReply:
		//if(frame[0] == current_address + LED_ADDRESS_OFFSET)
		pdata->comm_status = CommStatusRequest;
//		return;
	break;
	case CommStatusTimeout:
		fault.SetGlobal(F01_LED);
		mb.UpdateHoldingRegister(current_address, F01_LED << 8);
		pdata->comm_status = CommStatusRequest;
//		return;
	break;
	}

	if(LastAddress())
	{
		SLAVE_POLL_TIMEOUT_OFF; SLAVE_POLL_STOP;
		EV_TestElm(pdata);
//		return;
	}
}

void Dynabox::ST_TestingElm(DynaboxData* pdata)
{
	switch(pdata->comm_status)
	{
	case CommStatusRequest:
		comm.Prepare(current_address, COMM_DOOR_CHECK_ELECTROMAGNET);
		comm.Prepare(current_address + LED_ADDRESS_OFFSET, COMM_LED_GREEN_ON_FOR_TIME);
		current_address++;
		SLAVE_POLL_TIMEOUT_SET;
//		return;
	break;
	case CommStatusReply:
		if(usart_data.frame[0] == current_address - 1)
		{
			if(usart_data.frame[1] == COMM_F05_ELECTROMAGNET)
			{
				fault.SetGlobal(F05_ELECTROMAGNET);
				mb.UpdateHoldingRegister(current_address, F05_ELECTROMAGNET << 8);
			}
		}
		pdata->comm_status = CommStatusRequest;
//		return;
	break;
	case CommStatusTimeout:
		fault.SetGlobal(F02_DOOR);
		mb.UpdateHoldingRegister(current_address, F02_DOOR << 8);
		pdata->comm_status = CommStatusRequest;
//		return;
	break;
	}

	if(LastAddress())
	{
		SLAVE_POLL_TIMEOUT_OFF; SLAVE_POLL_STOP;
		EV_GetDoorsState(pdata);
//		return;
	}
}

void Dynabox::ST_CheckingDoorsState(DynaboxData* pdata)
{
	switch(pdata->comm_status)
	{
	case CommStatusRequest:
		comm.Prepare(current_address, 0x80);
		current_address++;
		SLAVE_POLL_TIMEOUT_SET;
//		return;
	break;
	case CommStatusReply:
		if(usart_data.frame[0] == current_address - 1)
		{
			//if(usart_data.frame[1] == COMM_F05_ELECTROMAGNET)
			//{
			//	fault.Set(F05_ELECTROMAGNET);
				mb.UpdateHoldingRegister(current_address, usart_data.frame[1]);
			//}
		}
		pdata->comm_status = CommStatusRequest;
//		return;
	break;
	case CommStatusTimeout:
		fault.SetGlobal(F02_DOOR);
		mb.UpdateHoldingRegister(current_address, F02_DOOR << 8);
		pdata->comm_status = CommStatusRequest;
//		return;
	break;
	}

	if(LastAddress())
	{
		SLAVE_POLL_TIMEOUT_OFF; SLAVE_POLL_STOP;
		//EV_GetDoorsState(pdata);
//		return;
	}
}

void Dynabox::ST_Homing(DynaboxData* pdata)
{
//	led_same_for_all = COMM_LED_GREEN_RED_BLINK;
	//for(uint8_t i = 1; i <= 7; i++)
	//{
	//	_delay_ms(500);
		comm.Prepare(2 + LED_ADDRESS_OFFSET, COMM_LED_GREEN_3PULSES); //+ 0x80);
	//}
	motor.EV_Homing();
}

void Dynabox::ST_Ready(DynaboxData* pdata)
{

}

void Dynabox::ST_Config(DynaboxData* pdata)
{

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

void Dynabox::EV_ShowOnLed(DynaboxData* pdata)
{

}

void Dynabox::EV_GetDoorsState(DynaboxData* pdata)
{
	pstate = &Dynabox::ST_CheckingDoorsState;
	current_address = 1;
	SLAVE_POLL_START;

	BEGIN_TRANSITION_MAP								// current state
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_TESTING_LED
		TRANSITION_MAP_ENTRY(ST_CHECKING_DOORS_STATE)	// ST_TESTING_ELM
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

//void Dynabox::ST_Test(DynaboxData* pdata)
//{
//	display.Write(1111);
//}

void Dynabox::EV_NeedMovement(DynaboxData* pdata)
{
//comm.Prepare(i + LED_ADDRESS_OFFSET, COMM_LED_GREEN_3PULSES); //+ 0x80);
	BEGIN_TRANSITION_MAP								// current state
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_TESTING_LED
		TRANSITION_MAP_ENTRY(ST_HOMING)					// ST_TESTING_ELM
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_HOMING
	END_TRANSITION_MAP(pdata)
}
