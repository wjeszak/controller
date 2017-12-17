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
	comm.EV_Send(current_address + LED_ADDRESS_OFFSET, GreenOnForTime, false);
}

void Dynabox::ST2_PreparingToMovement(DynaboxData* pdata)
{

}

void Dynabox::ST3_ShowingOnLed(DynaboxData* pdata)
{

}

void Dynabox::ST4_Homing(DynaboxData* pdata)
{
	mb.Write(current_address, d->data);
}

void Dynabox::ST5_Ready(DynaboxData* pdata)
{
	//mb.Write(current_address + 1, d->data);
	mb.Write(current_address, d->data);
}

void Dynabox::ST6_Movement(DynaboxData* pdata)
{

}

void Dynabox::ST7_EndMovement(DynaboxData* pdata)
{
	//if(desired_doors_position[current_address] != 0)
	//	comm.EV_Send(current_address + LED_ADDRESS_OFFSET, GreenOn, false);
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
	SetLedCommand(Diag, false);
}

void Dynabox::EXIT_TestingLed()
{
	s.Push(ST_TESTING_ELM);
}

void Dynabox::ENTRY_TestingElm()
{
	SetDoorCommand(CheckElm);
}

void Dynabox::EXIT_TestingElm()
{
	s.Push(ST_PREPARING_TO_MOVEMENT);
}

void Dynabox::ENTRY_PreparingToMovement()
{
	SetDoorCommand(GetStatus);
}

void Dynabox::EXIT_PreparingToMovement()
{
	if(fault.CheckGlobal(F02_Door))
	{
		s.Push(ST_NOT_READY);
		SetLedCommand(true);
	}
	else
	{
		if(!home_ok)
		{
			s.Push(ST_HOMING);
			SetLedCommand(GreenRedBlink, true);
		}
		else
		{
			s.Push(ST_MOVEMENT);
			SetLedCommand(GreenBlink, true);
		}
	}
	s.Push(ST_SHOWING_ON_LED);
}

void Dynabox::ENTRY_ShowingOnLed()
{

}

void Dynabox::EXIT_ShowingOnLed()
{
	EV_LedTrigger();
}

void Dynabox::ENTRY_Homing()
{
	SetDoorCommand(GetStatus);
	motor.SetDirection(motor.Forward);
	motor_data.max_pwm_val = MAX_PWM_HOMING;
	motor.EV_Start(&motor_data);
	timer.Assign(TIMER_TMP, TIMER_TMP_INTERVAL, Tmp);
	SetIOInfo(Moving);
	SetIOInfo(MovingDirection);
	SetIOInfo(HomingInProgress);
}

void Dynabox::EXIT_Homing()
{

}

void Dynabox::ENTRY_Ready()
{
	SetOrderStatus(Ready);
	SetDoorCommand(GetStatus);
}

void Dynabox::EXIT_Ready()
{

}

void Dynabox::ENTRY_Movement()
{
	SetIOInfo(Moving);
	SetDoorCommand(GetStatus);
}

void Dynabox::EXIT_Movement()
{

}

void Dynabox::ENTRY_EndMovement()
{
	SetDoorCommand();
}

void Dynabox::EXIT_EndMovement()
{
	s.Push(ST_READY);
}

void Dynabox::ENTRY_NotReady()
{
	SetOrderStatus(NotReady);
	SetDoorCommand(GetStatus);
}

void Dynabox::EXIT_NotReady()
{

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
	motor.EV_Stop(&motor_data);
	s.Push(ST_READY);
	SetLedCommand(GreenRedOff, true);
	s.Push(ST_SHOWING_ON_LED);
	home_ok = true;
	ClearIOInfo(Moving);
	ClearIOInfo(HomingInProgress);
	SetIOInfo(HomingDone);
}

void Dynabox::EV_UserAction(MachineData* pdata)
{
	if(mb.Read(LOCATIONS_NUMBER) > 0)
	{
		SetOrderStatus(Processing);
		motor_data.pos = 400 * (mb.Read(LOCATIONS_NUMBER) - 1);
		motor.ComputeDirection();
		motor.ComputeMaxPwm();
		motor.EV_Start(&motor_data);
		mb.Write(LOCATIONS_NUMBER, 0);

		for(uint8_t i = 0; i < MACHINE_MAX_NUMBER_OF_DOORS; i++)
		{
			desired_doors_position[i] = (uint8_t)mb.Read(LOCATIONS_NUMBER + 1 + i);
		}
		s.Push(ST_PREPARING_TO_MOVEMENT);
		timer.Assign(TIMER_TMP1, TIMER_TMP1_INTERVAL, Tmp1);
	}
	if(GetOrderStatus() == GoAck) display.Write(1234);
}

void Dynabox::EV_PositionAchieved(DynaboxData* pdata)
{
	SetOrderStatus(EndOfMovement);
	motor.EV_Stop(&motor_data);
	s.Push(ST_END_MOVEMENT);
	SetLedCommand(GreenRedOff, true);
	s.Push(ST_SHOWING_ON_LED);
	ClearIOInfo(Moving);
}

void Dynabox::EV_OnF8(DynaboxData* pdata)
{
//	motor.Stop();
}
