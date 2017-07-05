/*
 * motor.cpp
 *
 *  Created on: 20 cze 2017
 *      Author: tomek
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "motor.h"
#include "timer.h"
#include "display.h"

// call from timer :/
void MotorAccelerate()
{
	//motor.actual_speed++; //= motor.actual_speed + 1;
	OCR2A = motor.actual_speed++;
	display.Write(motor.actual_speed);
	if(motor.actual_speed == motor.desired_speed)
	{
		timer2.Disable(3);
		motor.Event(2, NULL);
	}
}

Motor::Motor() : Machine(ST_MAX_STATES)
{
	MOTOR_INIT;
	//TCCR2A |= (1 << WGM21) | (1 << WGM20) | T2_PS_1; //  Mode 3, inverting
}

void Motor::IrqInit()
{
	DDRB &= ~(1 << PB2);
	PORTB |= (1 << PB2);
	EICRA = (1 << ISC21); 		// opadajace
	EIMSK = (1 << INT2);
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

void Motor::Disable()
{
	MOTOR_DISABLE;
}

void Motor::SetSpeed(uint8_t speed)
{
	desired_speed = speed * 255 / 100; 	// percent
	OCR2A = 0;
}

void Motor::EV_Homing(MotorData* pdata)
{
	SetDirection(Forward);
	SetSpeed(50);		// percent
	IrqInit();
	const uint8_t Transitions[] =
	{
		// musi byc obsluga jesli znak przyjdzie w stanie INIT
		ST_ACCELERATION,			// ST_IDLE
		ST_NOT_ALLOWED, 			// ST_ACCELERATION
		ST_NOT_ALLOWED				// ST_RUNNING
	};
	Event(Transitions[current_state], pdata);
}
void Motor::EV_PhaseA(MotorData* pdata)
{
	if(PINB & (1 << PB1))
	{
	//display.Write(25);
	//display.Write(counter--);
	//if((counter < 0) || (counter > 9999)) counter = 0;
	// left
	}
	else
	{
	// right
	//display.Write(50); 	// kierunek
		display.Write(motor.position++);
	//if(motor.f_homing != 1)
	//{
	//	if(motor.position == motor.new_position)
	//	{
	//		motor.Disable();
	//		timer2.Disable(3);
	//	}
	//}
	}
	if(motor.position == ENCODER_ROWS) motor.position = 0;
}

void Motor::EV_PhaseB(MotorData* pdata)
{
	if(PINB & (1 << PB0))
	{
		//display.Write(counter--);
		//if((counter < 0) || (counter > 9999)) counter = 0;
	}
	else
	{
		//display.Write(50); 	// kierunek
		display.Write(motor.position++);
		if(motor.position == ENCODER_ROWS) motor.position = 0;
	}
}

void Motor::EV_PhaseZ(MotorData* pdata)
{
	Disable();
}

void Motor::ST_Idle(MotorData* pdata)
{
	display.Write(15);
}

void Motor::ST_Acceleration(MotorData* pdata)
{
	motor.actual_speed = 0;
	MOTOR_ENABLE;
	timer2.Assign(3, 100, MotorAccelerate); 	//
//	display.Write(motor.current_state);
}

void Motor::ST_Running(MotorData* pdata)
{
	display.Write(motor.current_state);
}

void Motor::RunToPosition(uint16_t pos)
{
	motor.desired_position = pos;
	//Start(Forward, 0);
	timer2.Assign(3, 250, MotorAccelerate);
}


ISR(INT2_vect)
{
	motor.EV_PhaseZ(NULL);
}
