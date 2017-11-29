/*
 * dynabox_states_events.cpp
 *
 *  Created on: 4 paü 2017
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
#include "display.h"

// ----------------------- States -----------------------
void Dynabox::ST_TestingLed(DynaboxData* pdata)
{
}

void Dynabox::ST_TestingElm(DynaboxData* pdata)
{
//	comm.EV_Send(current_address + LED_ADDRESS_OFFSET, 0x0C, false);
}

void Dynabox::ST_PreparingToHoming(DynaboxData* pdata)
{

}

void Dynabox::ST_ShowingOnLed(DynaboxData* pdata)
{

}

void Dynabox::ST_HomingOnEntry()
{
	for(uint8_t i = 0; i < 13; i++)
		addr_command[i] = 0x80;
	motor.EV_Homing();
}

void Dynabox::ST_Homing(DynaboxData* pdata)
{
	mb.Write(current_address + 1, d->data);
}

void Dynabox::ST_ShowingOnLedOnExit()
{
//	comm.EV_LedTrigger();
}

void Dynabox::ST_Ready(DynaboxData* pdata)
{
//	mb.Write(current_address + 1, d->data);
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
	for(uint8_t i = 0; i < 13; i++)
		addr_command[i] = COMM_LED_DIAG;
	AddToQueue(ST_TESTING_ELM);
}

void Dynabox::EV_TestElm()
{
	for(uint8_t i = 0; i < 13; i++)
		addr_command[i] = COMM_DOOR_CHECK_ELECTROMAGNET;
	AddToQueue(ST_PREPARING_TO_HOMING);
}

void Dynabox::EV_NeedHoming()
{
	for(uint8_t i = 0; i < 13; i++)
		addr_command[i] = 0x80;
	AddToQueue(ST_SHOWING_ON_LED);
}

void Dynabox::EV_PreparedToHoming()
{
	for(uint8_t i = 0; i < 13; i++)
		addr_command[i] = 0x80 + 0x05;
	AddToQueue(ST_HOMING);
}

void Dynabox::EV_LedTrigger()
{
	timer.Assign(TIMER_LED_TRIGGER, 10, LedTrigger);
}

void Dynabox::EV_HomingDone(DynaboxData* pdata)
{
	for(uint8_t i = 0; i < 13; i++)
		addr_command[i] = 0x00;
	AddToQueue(ST_SHOWING_ON_LED);
}

void Dynabox::EV_UserAction(MachineData* pdata)
{
	if(mb.Read(LOCATIONS_NUMBER) > 0)
	{
		//motor_data.pos = (mb.Read(LOCATIONS_NUMBER) - 1);
		motor_data.pos = 400 * (mb.Read(LOCATIONS_NUMBER) - 1);
//		if(motor.actual_position > motor_data.pos)
//			motor.SetDirection(motor.Backward);
//		else
//			motor.SetDirection(motor.Forward);
		motor.EV_RunToPosition(&motor_data);
	}
	if(mb.Read((uint8_t)ORDER_STATUS) == 1) display.Write(7843);
	//	BEGIN_TRANSITION_MAP								// current state
//		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_TESTING_LED
//		TRANSITION_MAP_ENTRY(ST_HOMING)					// ST_TESTING_ELM
//		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_CHECKING_DOORS_STATE
//		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_HOMING
//	END_TRANSITION_MAP(pdata)
}

void Dynabox::EV_OnF8(DynaboxData* pdata)
{
//	motor.Stop();
}

void Dynabox::EV_PositionAchieved(DynaboxData* pdata)
{

}
