/*
 * motor.cpp
 *
 *  Created on: 20 cze 2017
 *      Author: tomek
 */
#include <avr/io.h>
#include "motor.h"

Motor::Motor() : direction_(Forward), speed_(0)
{
	MOTOR_INIT;
	TCCR2A |= (1 << WGM21) | (1 << WGM20) | (1 << COM2A1) | (1 << COM2A0); //  Mode 3, inverting
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
	direction_ = dir;
}

void Motor::Enable(Direction dir, uint8_t speed)
{
	//TCNT2 = 0;
	OCR2A = speed;
	MOTOR_ENABLE;
	SetDirection(dir);
}

void Motor::Disable()
{
	MOTOR_DISABLE;
}
