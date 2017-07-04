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

Motor::Motor() : _direction(Forward), _speed(0)
{
	MOTOR_INIT;
	//TCCR2A |= (1 << WGM21) | (1 << WGM20) | T2_PS_1; //  Mode 3, inverting
	state = Acc;
	v = 0;
	f_homing = 0;
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

void Motor::Enable(Direction dir, uint8_t speed)
{
	//TCNT2 = 0;
	SetDirection(dir);
	SetSpeed(speed);
	MOTOR_ENABLE;
}

void Motor::Disable()
{
	MOTOR_DISABLE;
}

void Motor::SetSpeed(uint8_t speed)
{
	_speed = speed;
	OCR2A = speed;
}

void Motor::Homing()
{
	Enable(Forward, 0);
	timer2.Assign(3, 250, MotorTesting); 	// 500 ms
	motor.f_homing = 1;
}

void Motor::Run(uint16_t pos)
{
	motor.new_position = pos;
	Enable(Forward, 0);
	timer2.Assign(3, 250, MotorTesting);
}

ISR(INT2_vect)
{
	if(motor.f_homing)	// ST_Homing
	{
		motor.f_homing = 0;
		motor.Disable();
		motor.position = 0;		// set encoder zero
	}
}
