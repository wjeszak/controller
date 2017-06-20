/*
 * motor.cpp
 *
 *  Created on: 20 cze 2017
 *      Author: tomek
 */
#include <avr/io.h>
#include "motor.h"



Motor::Motor() : _direction(TForward), _speed(0)
{
	MOTOR_INIT;
	TCCR2A |= (1 << WGM21) | (1 << WGM20) | (1 << COM2A1) | (1 << COM2A0);//  brak inwersji tryb 3
}

void Motor::SetDirection(TDirection dir)
{
	switch (dir)
	{
	case TForward:
		MOTOR_PORT &= ~(1 << MOTOR_REV_PIN);
		MOTOR_PORT |=  (1 << MOTOR_FWD_PIN);
		break;
	case TBackward:
		MOTOR_PORT &= ~(1 << MOTOR_FWD_PIN);
		MOTOR_PORT |=  (1 << MOTOR_REV_PIN);
		break;
	}
	_direction = dir;
}

void Motor::Enable(TDirection dir, uint8_t speed)
{

	//TCNT2 = 0;
	//MOTOR_PORT |= (1 << MOTOR_BRAKE_PIN);
	OCR2A = speed;
	MOTOR_ENABLE;
	SetDirection(dir);
	//MOTOR_PORT |=(1 << MOTOR_EN_PIN);
	//OCR2A = speed;
}

void Motor::Disable()
{
	MOTOR_DISABLE;
}

