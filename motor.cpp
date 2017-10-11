/*
 * motor.cpp
 *
 * Created on: 20 cze 2017
 * Author: tomek
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "motor.h"
#include "timer.h"
#include "display.h"
#include "modbus_tcp.h"
#include "dynabox.h"
#include "comm_prot.h"

Motor::Motor() : StateMachine(ST_MAX_STATES)
{
	MOTOR_INIT;
	EncoderAndHomeIrqInit();
	home_ok = 0;
}

void Motor::Accelerate()
{
	OCR2A = motor.actual_speed++;
	if(motor.actual_speed == motor.desired_speed)
	{
		timer.Disable(TIMER_MOTOR_ACCELERATE);
		Event(2, NULL);
	}
}

void Motor::EV_PhaseA(MotorData* pdata)
{
	if(MOTOR_ENCODER_PIN & (1 << MOTOR_ENCODER_PHASEB_PIN))
	{
		// left
	}
	else
	{
		// right
		//display.Write(actual_position);
		mb.UpdateHoldingRegister(ENCODER_CURRENT_VALUE, actual_position++);
		if(actual_position == motor_data.pos)
		{
			motor.Stop();
			timer.Disable(TIMER_MOTOR_ACCELERATE);
			Event(5, NULL);
		}
		if(actual_position == ENCODER_ROWS) actual_position = 0;
	}
}

void Motor::EV_PhaseB(MotorData* pdata)
{
	if(MOTOR_ENCODER_PIN & (1 << MOTOR_ENCODER_PHASEA_PIN))
	{
		// left
	}
	else
	{
		// right
		//display.Write(actual_position);
		mb.UpdateHoldingRegister(ENCODER_CURRENT_VALUE, actual_position++);
		if(actual_position == motor_data.pos)
		{
			motor.Stop();
			timer.Disable(TIMER_MOTOR_ACCELERATE);
			Event(5, NULL);
		}
		if(actual_position == ENCODER_ROWS) actual_position = 0;
	}
}

void Motor::EV_PhaseZ(MotorData* pdata)
{
	// disable INT2 ?
    BEGIN_TRANSITION_MAP							// current state
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_IDLE
        TRANSITION_MAP_ENTRY(ST_HOME)				// ST_ACCELERATION
        TRANSITION_MAP_ENTRY(ST_HOME)				// ST_RUNNING
    END_TRANSITION_MAP(pdata)
}

void Motor::EV_Homing(MotorData* pdata)
{
    BEGIN_TRANSITION_MAP							// current state
        TRANSITION_MAP_ENTRY(ST_ACCELERATION)		// ST_IDLE
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_ACCELERATION
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_RUNNING
    END_TRANSITION_MAP(pdata)
	mb.UpdateHoldingRegister(IO_INFORMATIONS, (1 << 2) | (1 << 0));
}

void Motor::EV_RunToPosition(MotorData* pdata)
{
	if(home_ok)
	{
		BEGIN_TRANSITION_MAP						// current state
			TRANSITION_MAP_ENTRY(ST_ACCELERATION)	// ST_IDLE
			TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED) 	// ST_ACCELERATION
			TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)	// ST_RUNNING
			TRANSITION_MAP_ENTRY(ST_ACCELERATION)	// ST_HOME
			TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)	// ST_DECELERATION
			TRANSITION_MAP_ENTRY(ST_ACCELERATION)	// ST_POSITION_ACHIEVED
	    END_TRANSITION_MAP(pdata)
		mb.UpdateHoldingRegister(ORDER_STATUS, ORDER_STATUS_PROCESSING);
		mb.UpdateHoldingRegister(IO_INFORMATIONS, (1 << 0) | (1 << 3));
	}
}

void Motor::Stop()
{
	MOTOR_STOP;
}

void Motor::EncoderAndHomeIrqInit()
{
	// phase A (timer 0)
	MOTOR_ENCODER_DDR &= ~(1 << MOTOR_ENCODER_PHASEA_PIN);
	//MOTOR_ENCODER_PORT |= (1 << MOTOR_ENCODER_PHASEA_PIN);
	TCCR0A |= (1 << WGM01);
	TCCR0B |= (1 << CS02) | (1 << CS01);  			// external source, falling edge
	TCNT0 = 0;
	OCR0A = 1;
	// phase B (timer 1)
	MOTOR_ENCODER_DDR &= ~(1 << MOTOR_ENCODER_PHASEB_PIN);
	//MOTOR_ENCODER_PORT |= (1 << MOTOR_ENCODER_PHASEB_PIN);
	TCCR1B |= (1 << WGM12);
	TCCR1B |= (1<< CS12) | (1<< CS11) | (1 << CS10);  // external source, rising edge
	TCNT1 = 0;
	OCR1A = 1;
	// homing
	MOTOR_HOME_IRQ_DDR &= ~(1 << MOTOR_HOME_IRQ_PIN);
	EICRA |= (1 << ISC21) | (1 << ISC20); 			// rising edge
}

void Motor::SetDirection(Direction dir)
{
	switch (dir)
	{
	case Forward:
		MOTOR_PORT &= ~(1 << MOTOR_REV_PIN);
		MOTOR_PORT |=  (1 << MOTOR_FWD_PIN);
		break;
	case Backward:
		MOTOR_PORT &= ~(1 << MOTOR_FWD_PIN);
		MOTOR_PORT |=  (1 << MOTOR_REV_PIN);
		break;
	}
	_direction = dir;
}

void Motor::SetSpeed(uint8_t speed)
{
	desired_speed = speed * 255 / 100; 	// percent
	OCR2A = 0;
}

void Motor::ST_Idle(MotorData* pdata)
{

}

void Motor::ST_Acceleration(MotorData* pdata)
{
	SetDirection(Forward);
	//SetDirection(Backward);
	SetSpeed(60);
	MOTOR_ENCODER_ENABLE
	MOTOR_HOME_IRQ_ENABLE;
	actual_speed = 0;
	MOTOR_START;
	timer.Assign(TIMER_MOTOR_ACCELERATE, 50, MotorAccelerate);
}

void Motor::ST_Running(MotorData* pdata)
{

}

void Motor::ST_Home(MotorData* pdata)
{
	timer.Disable(TIMER_MOTOR_ACCELERATE); // if accelerating
	Stop();
	MOTOR_ENCODER_DISABLE;
	actual_position = 0;
	//display.Write(actual_position);
	mb.UpdateHoldingRegister(ENCODER_CURRENT_VALUE, actual_position);
	home_ok = 1;
	mb.UpdateHoldingRegister(IO_INFORMATIONS, (0 << 2) | (0 << 0) | (1 << 3));
	dynabox.led_same_for_all = COMM_GREEN_RED_OFF;
	dynabox.SetCurrentCommand(COMM_SHOW_STATUS_ON_LED);
}

void Motor::ST_Deceleration(MotorData* pdata)
{

}

void Motor::ST_PositionAchieved(MotorData* pdata)
{
	motor_data.pos = 0;
	mb.UpdateHoldingRegister(ORDER_STATUS, ORDER_STATUS_END_OF_MOVEMENT);
	mb.UpdateHoldingRegister(IO_INFORMATIONS, (0 << 0) | (1 << 3));
}

ISR(TIMER0_COMPA_vect)
{
	motor.EV_PhaseA(NULL);
}

ISR(TIMER1_COMPA_vect)
{
	motor.EV_PhaseB(NULL);
}

ISR(INT2_vect)
{
	motor.EV_PhaseZ(NULL);
}
