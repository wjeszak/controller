/*
 * dynabox_entry_exit.cpp
 *
 *  Created on: 7 sty 2018
 *      Author: wjeszak
 */

#include "dynabox.h"
#include "timer.h"
#include "fault.h"
#include "modbus_tcp.h"
#include "config.h"
#include "stack.h"
#include "queue.h"
// -------------------------------------------------------------------
void Dynabox::ENTRY_TestingLed(DynaboxData* pdata)
{
	SetLedCommand(Diag, false);
}

void Dynabox::LAST_TestingLed()
{
	s.Push(ST_TESTING_ELM);
}

void Dynabox::EXIT_TestingLed()
{

}
// -------------------------------------------------------------------
void Dynabox::ENTRY_TestingElm()
{
	SetDoorCommand(CheckElm);
}

void Dynabox::LAST_TestingElm()
{
	s.Push(ST_PREPARING_TO_MOVEMENT);
}

void Dynabox::EXIT_TestingElm()
{

}
// -------------------------------------------------------------------
void Dynabox::ENTRY_PreparingToMovement()
{
	SetDoorCommand(GetStatusBeforeMovement);
}

void Dynabox::LAST_PreparingToMovement()
{
	if(fault.IsGlobal())
	{
		s.Push(ST_NOT_READY);
		SetLedCommand(true);
	}
	else
	{
		if(!GetIOInfo(HomingDone))
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

void Dynabox::EXIT_PreparingToMovement()
{

}
// -------------------------------------------------------------------
void Dynabox::ENTRY_ShowingOnLed()
{

}

void Dynabox::LAST_ShowingOnLed()
{
	EV_LedTrigger();
}

void Dynabox::EXIT_ShowingOnLed()
{

}
// -------------------------------------------------------------------
void Dynabox::ENTRY_Homing()
{
	SetDoorCommand(GetStatus);
	motor.SetDirection(motor.Forward);
	motor_data.max_pwm_val = MAX_PWM_HOMING;
	motor.EV_Start(&motor_data);
	SetIOInfo(Moving);
	SetIOInfo(MovingDirection);
	SetIOInfo(HomingInProgress);
#ifdef DEBUG
	timer.Assign(TTmp, TIMER_TMP_INTERVAL, Tmp);
#endif
}

void Dynabox::LAST_Homing()
{

}

void Dynabox::EXIT_Homing()
{

}
// -------------------------------------------------------------------
void Dynabox::ENTRY_Ready()
{
	SetOrderStatus(Ready);
	SetDoorCommand(GetStatus);
}

void Dynabox::LAST_Ready()
{
	SetDoorCommand(GetStatus);
}

void Dynabox::EXIT_Ready()
{

}
// -------------------------------------------------------------------
void Dynabox::ENTRY_Movement()
{
	SetDoorCommand(GetStatus);
	motor_data.pos = 400 * (mb.Read(LOCATIONS_NUMBER) - 1);
	motor.ComputeDirection();
	motor.ComputeMaxPwm();
	motor.EV_Start(&motor_data);
	SetOrderStatus(Processing);
	SetIOInfo(Moving);
#ifdef DEBUG
	timer.Assign(TTmp1, TIMER_TMP1_INTERVAL, Tmp1);
#endif
}

void Dynabox::LAST_Movement()
{

}

void Dynabox::EXIT_Movement()
{

}
// -------------------------------------------------------------------
void Dynabox::ENTRY_EndMovement()
{
	// for sure...
	SetDoorCommand(GetStatus);
	uint8_t n = q.GetNumberOfElements();
	if(n > functions[11].param) n = functions[11].param;

	for(uint8_t i = 0; i < n; i++)
	{
		uint8_t el = q.Get();
		current_command[el] = SetPosition + desired_doors_position[el];
		door_open_timeout[el] = 0;
	}
}

void Dynabox::LAST_EndMovement()
{
	s.Push(ST_READY);
}

void Dynabox::EXIT_EndMovement()
{

}
// -------------------------------------------------------------------
void Dynabox::ENTRY_NotReady()
{
	SetOrderStatus(NotReady);
	SetDoorCommand(GetStatus);
}

void Dynabox::LAST_NotReady()
{
//	for(uint8_t i = 0; i < MACHINE_MAX_NUMBER_OF_DOORS; i++)
//	{
//		if(fault.IsLocal(i)) return;	// still fault
//	}
//	fault.ClearGlobal(F06_CloseDoor);
//	s.Push(ST_PREPARING_TO_MOVEMENT);
}

void Dynabox::EXIT_NotReady()
{

}
