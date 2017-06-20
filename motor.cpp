/*
 * motor.cpp
 *
 *  Created on: 20 cze 2017
 *      Author: tomek
 */
#include <avr/io.h>
#include "motor.h"

#define MOTOR_DDR 			DDRD
#define MOTOR_PORT 			PORTD
#define MOTOR_BRAKE_PIN 	3
#define MOTOR_FWD_PIN 		5
#define MOTOR_REV_PIN 		6
#define MOTOR_EN_PIN 		7
#define MOTOR_INIT 			MOTOR_DDR |= (1 << MOTOR_FWD_PIN) | (1 << MOTOR_REV_PIN) | (1 << MOTOR_EN_PIN) | (1 << MOTOR_BRAKE_PIN);

#define MOTOR_ENABLE 		TCCR2B |= PS_1024
#define MOTOR_DISABLE		TCCR2B |= PS_0

#define MOTOR_BRAKE_ON 		MOTOR_PORT |= (1 << MOTOR_BRAKE_PIN)
#define MOTOR_BRAKE_OFF 	MOTOR_PORT &= ~(1 << MOTOR_BRAKE_PIN)

Motor::Motor() : _direction(TForward), _speed(0)
{
	MOTOR_INIT;
	TCCR2A |= (1 << WGM21) | (1 << WGM20) | (1 << COM2A1);//  brak inwersji tryb 3
	TCCR2B |= (1 < CS22) | (1 < CS21) | (1 << CS20);

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
	SetDirection(dir);
	//TCNT2 = 0;
	//MOTOR_ENABLE;
	OCR2A = speed;
}

void Motor::Disable()
{
	MOTOR_DISABLE;
}

