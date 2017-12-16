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
#include "display.h"
#include "dynabox_commands_faults.h"
#include "stack.h"

// ---------------------------------- States ----------------------------------
void Dynabox::ST0_TestingLed(DynaboxData* pdata)
{

}

void Dynabox::ST1_TestingElm(DynaboxData* pdata)
{
	comm.EV_Send(current_address + LED_ADDRESS_OFFSET, COMM_LED_GREEN_ON_FOR_TIME, false);
}

void Dynabox::ST2_PreparingToMovement(DynaboxData* pdata)
{

}

void Dynabox::ST3_ShowingOnLed(DynaboxData* pdata)
{

}

void Dynabox::ST4_Homing(DynaboxData* pdata)
{
	mb.Write(current_address + 1, d->data);
}

void Dynabox::ST5_Ready(DynaboxData* pdata)
{
	mb.Write(current_address + 1, d->data);
}

void Dynabox::ST6_Movement(DynaboxData* pdata)
{

}

void Dynabox::ST7_EndMovement(DynaboxData* pdata)
{

}

void Dynabox::ST8_NotReady(DynaboxData* pdata)
{

}

void Dynabox::ST9_Config(DynaboxData* pdata)
{

}

// ---------------------------------- Entry, exit ----------------------------------
void Dynabox::ENTRY_TestingLed(DynaboxData* pdata)
{
	SetCommand(COMM_LED_DIAG);
}

void Dynabox::EXIT_TestingLed()
{
	s.Push(ST_TESTING_ELM);
}

void Dynabox::ENTRY_TestingElm()
{
	SetCommand(COMM_DOOR_CHECK_ELM);
}

void Dynabox::EXIT_TestingElm()
{

}

void Dynabox::ENTRY_PreparingToMovement()
{
	SetCommand(COMM_DOOR_GET_STATUS);
//	AddToQueue(ST_SHOWING_ON_LED);
}

void Dynabox::EXIT_PreparingToMovement()
{
	SetCommand(0x80 + 0x05);
	AddToQueue(ST_HOMING);
	AddToQueue(ST_SHOWING_ON_LED);
}

void Dynabox::ENTRY_Homing()
{
	SetCommand(COMM_DOOR_GET_STATUS);
	mb.Write(IO_INFORMATIONS, (1 << 2) | (1 << 0));
	motor.SetDirection(motor.Forward);
	motor_data.max_pwm_val = MAX_PWM_HOMING;
	motor.EV_Start(&motor_data);
}

// ---------------------------------- Public events ---------------------------
void Dynabox::EV_LedTrigger()
{
	timer.Assign(TIMER_LED_TRIGGER, 10, LedTrigger);
}

void Dynabox::EV_EnterToConfig()
{
	SLAVE_POLL_STOP;
	timer.Disable(TIMER_FAULT_SHOW);
}

void Dynabox::EV_HomingDone(DynaboxData* pdata)
{
	SetCommand(0x00);
	mb.Write(IO_INFORMATIONS, (0 << 2) | (0 << 0) | (1 << 3));
	AddToQueue(ST_SHOWING_ON_LED);
	//AddToQueue(ST_READY);
}

void Dynabox::EV_UserAction(MachineData* pdata)
{
	if(mb.Read(LOCATIONS_NUMBER) > 0)
	{
		motor_data.pos = 400 * (mb.Read(LOCATIONS_NUMBER) - 1);
		motor.ComputeDirection();
		//motor.ComputeDistance();
		motor.ComputeMaxPwm();
		motor.EV_Start(&motor_data);
		mb.Write(LOCATIONS_NUMBER, 0);
	}
//	if(mb.Read((uint8_t)ORDER_STATUS) == 1) display.Write(7843);
	//	BEGIN_TRANSITION_MAP								// current state
//		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_TESTING_LED
//		TRANSITION_MAP_ENTRY(ST_HOMING)					// ST_TESTING_ELM
//		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_CHECKING_DOORS_STATE
//		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_HOMING
//	END_TRANSITION_MAP(pdata)
}

void Dynabox::EV_PositionAchieved(DynaboxData* pdata)
{
	mb.Write(ORDER_STATUS, ORDER_STATUS_END_OF_MOVEMENT);
	mb.Write(IO_INFORMATIONS, (0 << 0) | (1 << 3));
}

void Dynabox::EV_OnF8(DynaboxData* pdata)
{
//	motor.Stop();
}
