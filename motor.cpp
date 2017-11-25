/*
 * motor.cpp
 *
 * Created on: 20 cze 2017
 * Author: tomek
 */

#include <avr/interrupt.h>
#include <stdlib.h>				// abs()
#include "motor.h"
#include "timer.h"
#include "modbus_tcp.h"
#include "dynabox.h"
#include "display.h"

Motor::Motor() : StateMachine(ST_MAX_STATES)
{
	delta_time_accelerate = 10;		// [ms]
	delta_time_decelerate = 80;		// [ms]
	pulses_to_decelerate = 	5; 	// [pulses]
	init_pwm_val = 			45;		// 0 .. 255
	max_speed = 			70;		// 0 .. 100 (percent of 255)
	MOTOR_INIT;
	EncoderAndHomeIrqInit();
	home_ok = false;
	actual_speed = 		0;
	desired_speed = 	0;
	actual_position = 	0;
	desired_position = 	0;
	impulses_cnt = 		0;
	distance = 			0;
}

void Motor::Start()
{
	MOTOR_START;
}

void Motor::Stop()
{
	MOTOR_STOP;
}
// from timer
void Motor::Accelerate()
{
	OCR2A = actual_speed++;
	if(actual_speed == desired_speed)
	{
		timer.Disable(TIMER_MOTOR_ACCELERATE);
		Event(ST_RUNNING, NULL);
	}
}
// from timer
void Motor::Decelerate()
{
	OCR2A = actual_speed--;
	if(actual_speed == 0) timer.Disable(TIMER_MOTOR_DECELERATE);
}

void Motor::EV_Homing(MotorData* pdata)
{
    BEGIN_TRANSITION_MAP							// current state
        TRANSITION_MAP_ENTRY(ST_ACCELERATION)		// ST_IDLE
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_ACCELERATION
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_RUNNING
    END_TRANSITION_MAP(pdata)
	mb.Write(IO_INFORMATIONS, (1 << 2) | (1 << 0));
	MOTOR_HOME_IRQ_ENABLE;
    SetDirection(Forward);
}

void Motor::GetStartPwmVal()
{

}

void Motor::SpeedMeasure()
{
//	uint16_t v = impulses_cnt;		// [imp / s] @ 100 ms timer
	//display.Write(v);
	//mb.Write(ACTUAL_SPEED, v / 10);
	//if(GetState() == ST_ACCELERATION)
//		if(v != 0)
//		{
//			display.Write(OCR2A);
//			timer.Disable(TIMER_MOTOR_SPEED_MEAS);
//		}
	//impulses_cnt = 0;
}

void Motor::NeedDeceleration()
{
	//if((actual_position == motor_data.pos - pulses_to_decelerate) && home_ok)
	if((actual_position == motor_data.pos) && home_ok)
	{
		timer.Disable(TIMER_MOTOR_ACCELERATE);
//		timer.Assign(TIMER_MOTOR_DECELERATE, delta_time_decelerate, MotorDecelerate);
		Stop();
		Event(ST_POSITION_ACHIEVED, NULL);
	}
}

void Motor::PulsePlus()
{
//	if(actual_position == ENCODER_ROWS)
//		actual_position = 0;
//	else

	actual_position++;

//	actual_position++;
}

void Motor::PulseMinus()
{
//	if(actual_position == 0)
//		actual_position = ENCODER_ROWS;
//	else
//	actual_position = actual_position % ENCODER_ROWS;
	actual_position--;
}

void Motor::EV_PhaseA(MotorData* pdata)
{
	if(MOTOR_ENCODER_PIN & (1 << MOTOR_ENCODER_PHASEB_PIN))
	{
	// right
		actual_position++;
		//PulsePlus();
	}
	else
	{
		//actual_position--;
		//PulseMinus();
	}
	mb.Write(ENCODER_CURRENT_VALUE, actual_position);
	NeedDeceleration();
//	impulses_cnt++;
}

void Motor::EV_PhaseB(MotorData* pdata)
{
	if(MOTOR_ENCODER_PIN & (1 << MOTOR_ENCODER_PHASEA_PIN))
	{
		actual_position++;
		//PulsePlus();
	}
	else
	{
		//actual_position--;
		//PulseMinus();
	}
	mb.Write(ENCODER_CURRENT_VALUE, actual_position);
	NeedDeceleration();
//	impulses_cnt++;
}

void Motor::EV_PhaseZ(MotorData* pdata)
{
    BEGIN_TRANSITION_MAP							// current state
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_IDLE
        TRANSITION_MAP_ENTRY(ST_HOME)				// ST_ACCELERATION
        TRANSITION_MAP_ENTRY(ST_HOME)				// ST_RUNNING
    END_TRANSITION_MAP(pdata)
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
		mb.Write(ORDER_STATUS, ORDER_STATUS_PROCESSING);
		mb.Write(IO_INFORMATIONS, (1 << 0) | (1 << 3));
		ComputeDirection();
	}
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

void Motor::ComputeDistance()
{
	distance = (motor_data.pos - actual_position) % ENCODER_ROWS;
}

void Motor::ComputeDirection()
{
	ComputeDistance();
	if(abs(distance) < (ENCODER_ROWS / 2))
		SetDirection(Forward);
	else
		SetDirection(Backward);
}

void Motor::SetSpeed(uint8_t speed)
{
	desired_speed = speed * 255 / 100; 	// percent
	OCR2A = 35;
}

void Motor::ST_Idle(MotorData* pdata)
{

}

void Motor::ST_Acceleration(MotorData* pdata)
{
	MOTOR_ENCODER_ENABLE;
	SetSpeed(init_pwm_val);
	actual_speed = 0;
	Start();
	timer.Assign(TIMER_MOTOR_ACCELERATE, delta_time_accelerate, MotorAccelerate);
}

void Motor::ST_Running(MotorData* pdata)
{

}

void Motor::ST_Home(MotorData* pdata)
{
	timer.Disable(TIMER_MOTOR_ACCELERATE); // if accelerating
	Stop();
	MOTOR_HOME_IRQ_DISABLE;
	mb.Write(ENCODER_CURRENT_VALUE, actual_position);
	home_ok = true;
	actual_position = 0;
	mb.Write(IO_INFORMATIONS, (0 << 2) | (0 << 0) | (1 << 3));
	dynabox.EV_HomingDone(&dynabox_data);
}

void Motor::ST_Deceleration(MotorData* pdata)
{

}

void Motor::ST_PositionAchieved(MotorData* pdata)
{
	motor_data.pos = 0;
	mb.Write(ORDER_STATUS, ORDER_STATUS_END_OF_MOVEMENT);
	mb.Write(IO_INFORMATIONS, (0 << 0) | (1 << 3));
	dynabox.EV_PositionAchieved(&dynabox_data);
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
