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

Motor::Motor() : Machine(ST_MAX_STATES)
{
	MOTOR_INIT;
	EncoderAndHomeIrqInit();
	home_ok = 0;
}

void Motor::EncoderAndHomeIrqInit()
{
	// phase A (timer 0)
	MOTOR_ENCODER_DDR &= ~(1 << MOTOR_ENCODER_PHASEA_PIN);
	MOTOR_ENCODER_PORT |= (1 << MOTOR_ENCODER_PHASEA_PIN);
	TCCR0A |= (1 << WGM01);
	TCCR0B |= (1 << CS02) | (1 << CS01);  			// external source, falling edge
	TCNT0 = 0;
//	TIMSK0 |= (1 << OCIE0A);
	OCR0A = 1;
	// phase B (timer 1)
	MOTOR_ENCODER_DDR &= ~(1 << MOTOR_ENCODER_PHASEB_PIN);
	MOTOR_ENCODER_PORT |= (1 << MOTOR_ENCODER_PHASEB_PIN);
	TCCR1B |= (1 << WGM12);
	TCCR1B |= (1<< CS12) | (1<< CS11) | (1 << CS10);  // external source, rising edge
	TCNT1 = 0;
//	TIMSK1 |= (1 << OCIE1A);
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

void Motor::Stop()
{
	MOTOR_STOP;
}

void Motor::SetSpeed(uint8_t speed)
{
	desired_speed = speed * 255 / 100; 	// percent
	OCR2A = 0;
}

void Motor::EV_Homing(MotorData* pdata)
{
	SetDirection(Forward);
	SetSpeed(80);
	MOTOR_ENCODER_PHASEA_ENABLE;
	MOTOR_ENCODER_PHASEB_ENABLE;
	MOTOR_HOME_IRQ_ENABLE;

    BEGIN_TRANSITION_MAP							// current state
        TRANSITION_MAP_ENTRY(ST_ACCELERATION)		// ST_IDLE
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_ACCELERATION
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_RUNNING
    END_TRANSITION_MAP(pdata)
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
		display.Write(motor.position++);
		if(motor.position == motor_data.pos)
		{
			motor.Stop();
			timer.Disable(TIMER_MOTOR_ACCELERATE);
			Event(5, NULL);
		}
		if(motor.position == ENCODER_ROWS) motor.position = 0;
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
		display.Write(motor.position++);
		if(motor.position == motor_data.pos)
		{
			motor.Stop();
			timer.Disable(TIMER_MOTOR_ACCELERATE);
			Event(5, NULL);
		}
		if(motor.position == ENCODER_ROWS) motor.position = 0;
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

void Motor::ST_Idle(MotorData* pdata)
{

}

void Motor::ST_Acceleration(MotorData* pdata)
{
	actual_speed = 0;
	MOTOR_START;
	timer.Assign(TIMER_MOTOR_ACCELERATE, 10, MotorAccelerate);
}

void Motor::ST_Running(MotorData* pdata)
{

}

void Motor::ST_Home(MotorData* pdata)
{
	timer.Disable(TIMER_MOTOR_ACCELERATE); // w przypadku gdyby sie rozpedzal
	Stop();
	// nie zliczaj impulsow bezwladnosciowych
	// tutaj mozna zrobic offset do ew. cofniecia (nie wylaczajac przerwan), np.
	// Disable();
	// 	position = 0;
	// offset = position (poniewaz przerwania pracuja)
	TIMSK0 &= ~(1 << OCIE0A);
	TIMSK1 &= ~(1 << OCIE1A);
	position = 0;
//	display.Write(motor.current_state);
	display.Write(position);
	home_ok = 1;
//	Event(0, NULL);		// back to ST_IDLE
}

void Motor::ST_Deceleration(MotorData* pdata)
{

}

void Motor::ST_PositionAchieved(MotorData* pdata)
{
	motor_data.pos = 0;
}

void Motor::EV_RunToPosition(MotorData* pdata)
{
	if(home_ok)
	{
		TIMSK0 |= (1 << OCIE0A);
		TIMSK1 |= (1 << OCIE1A);
		SetDirection(Forward);
		SetSpeed(30);		// percent
		const uint8_t Transitions[] =
		{
			ST_ACCELERATION,			// ST_IDLE
			ST_NOT_ALLOWED, 			// ST_ACCELERATION
			ST_NOT_ALLOWED,				// ST_RUNNING
			ST_ACCELERATION,			// ST_HOME
			ST_NOT_ALLOWED,				// ST_DECCELERATION
			ST_ACCELERATION				// ST_POSITION_ACHIEVED
		};
		Event(Transitions[current_state], pdata);
	}
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
