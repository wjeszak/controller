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
#include "queue.h"

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

}
// ---------------------------------- Ready ----------------------------------
void Dynabox::ST5_Ready(DynaboxData* pdata)
{
	if(DoorPositionAchieved())
	{
		// turn off previous led
		comm.EV_Send(current_address + LED_ADDRESS_OFFSET, GreenRedOff, false);

		// open next door from queue
		if(q.GetNumberOfElements() > 0)
		{
			uint8_t addr = q.Get();
			comm.EV_Send(addr + 1 + LED_ADDRESS_OFFSET, GreenOn, false);
			current_command[addr] = SetPosition + desired_doors_position[addr];
		}
	}

	if(door_open_timeout[current_address - 1] == 30)
	{
		door_open_timeout[current_address - 1] = 0xFF;
		comm.EV_Send(current_address + LED_ADDRESS_OFFSET, GreenRedBlink, false);
		current_command[current_address - 1] = ElmOff;
	}

}

void Dynabox::ST6_Movement(DynaboxData* pdata)
{

}

void Dynabox::ST7_EndMovement(DynaboxData* pdata)
{
	if(current_command[current_address - 1] > SetPosition)
		comm.EV_Send(current_address + LED_ADDRESS_OFFSET, GreenOn, false);
}

void Dynabox::ST8_NotReady(DynaboxData* pdata)
{
	if(fault.Check(F06_CloseDoor, current_address) && (((mb.Read((uint8_t)current_address + 1) & 0xFF) == 0xD0 || (mb.Read((uint8_t)current_address + 1) & 0xFF) == 0xC0)))
	{
		fault.Clear(F06_CloseDoor, current_address);
		comm.EV_Send(current_address + LED_ADDRESS_OFFSET, GreenRedOff, false);
	}
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
	SetDoorCommand(GetStatusBeforeMovement);
}

void Dynabox::EXIT_PreparingToMovement()
{
	if(fault.CheckGlobal())
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
	SetIOInfo(Moving);
	SetIOInfo(MovingDirection);
	SetIOInfo(HomingInProgress);
#ifdef DEBUG
	timer.Assign(TIMER_TMP, TIMER_TMP_INTERVAL, Tmp);
#endif
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
	SetDoorCommand(GetStatus);
}

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
	timer.Assign(TIMER_TMP1, TIMER_TMP1_INTERVAL, Tmp1);
#endif
}

void Dynabox::EXIT_Movement()
{

}

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
	}
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
	for(uint8_t i = 0; i < MACHINE_MAX_NUMBER_OF_DOORS; i++)
	{
		if(fault.CheckAll(i)) return;	// still fault
	}
	fault.ClearGlobal(F06_CloseDoor);
	s.Push(ST_PREPARING_TO_MOVEMENT);
}
// ---------------------------------- Public events ---------------------------
void Dynabox::EV_LedTrigger()
{
	timer.Assign(TIMER_LED_TRIGGER, 10, LedTrigger);
}

void Dynabox::EV_EnterToConfig()
{
	SLAVE_POLL_STOP;
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
	if(mb.GetQuantity() > 1)
	{
		for(uint8_t i = 0; i < MACHINE_MAX_NUMBER_OF_DOORS; i++)
		{
			desired_doors_position[i] = (uint8_t)mb.Read(LOCATIONS_NUMBER + 1 + i);
			if(desired_doors_position[i] != 0)
			{
				door_open_timeout[i] = 0;
				q.Add(i);
			}
		}

		if(last_position != mb.Read(LOCATIONS_NUMBER))
			s.Push(ST_TESTING_ELM);			// need movement
		else
			s.Push(ST_END_MOVEMENT);		// not need

		last_position = mb.Read(LOCATIONS_NUMBER);
	}
	if(GetOrderStatus() == GoAck) display.Write(1234);		// clear faults
}

void Dynabox::EV_PositionAchieved(DynaboxData* pdata)
{
	motor.EV_Stop(&motor_data);
	s.Push(ST_END_MOVEMENT);
	SetLedCommand(GreenRedOff, true);
	s.Push(ST_SHOWING_ON_LED);
	ClearIOInfo(Moving);
	SetOrderStatus(EndOfMovement);
}

bool Dynabox::DoorPositionAchieved()
{
	uint16_t status = mb.Read(current_address + 1);
	if(desired_doors_position[current_address - 1] != 0 && status >= OpenedElmOff && status <= Opened15StopsElmOff)
	{
		desired_doors_position[current_address - 1] = 0;
		return true;
	}
	else
		return false;
}

void Dynabox::EV_OnF8(DynaboxData* pdata)
{
	motor.EV_Stop(&motor_data);
}
