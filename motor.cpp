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
	MOTOR_INIT;
	EncoderAndHomeIrqInit();
	delta_time_accelerate = 10;		// [ms]
	delta_time_decelerate = 80;		// [ms]
	pulses_to_decelerate = 	5; 		// [pulses]
	init_pwm_val = 			45;		// 0 .. 255
	max_speed = 			70;		// 0 .. 100 (percent of 255)
// -------------------------------------------------------------
	home_ok = false;
	actual_speed = 		0;
	desired_speed = 	0;
	actual_position = 	0;
	desired_position = 	0;
	impulses_cnt = 		0;
	distance = 			0;
	_direction_encoder = Forward;
	_last_encoder_val = 0;
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
    SetDirection(Backward);
}

void Motor::GetStartPwmVal()
{

}

void Motor::NeedDeceleration()
{
	if((actual_position == motor_data.pos - pulses_to_decelerate) && home_ok)
	//if((actual_position == motor_data.pos) && home_ok)
	{
		timer.Disable(TIMER_MOTOR_ACCELERATE);
//		timer.Assign(TIMER_MOTOR_DECELERATE, delta_time_decelerate, MotorDecelerate);
		Stop();
		Event(ST_POSITION_ACHIEVED, NULL);
	}
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
	SetSpeed(100);
	//SetSpeed(init_pwm_val);
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

void Motor::SpeedMeasure()
{
	uint16_t v = impulses_cnt;		// [imp / s] @ 100 ms timer
	//display.Write(v);
	mb.Write(ACTUAL_SPEED, 50 * 255 / 100);		// [imp / 100 ms]
	//actual_position = 3590;
	uint8_t act_pos_hi = actual_position >> 8;
	uint8_t act_pos_lo = actual_position & 0xFF;
	comm.EV_Send(act_pos_hi, act_pos_lo, false);
	//if(GetState() == ST_ACCELERATION)
//		if(v != 0)
//		{
//			display.Write(OCR2A);
//			timer.Disable(TIMER_MOTOR_SPEED_MEAS);
//		}
	impulses_cnt = 0;
}

void Motor::EncoderAndHomeIrqInit()
{
	MOTOR_ENCODER_DDR &= ~(1 << MOTOR_ENCODER_A_PIN);
	//MOTOR_ENCODER_PORT |= (1 << MOTOR_ENCODER_A_PIN);
	MOTOR_ENCODER_DDR &= ~(1 << MOTOR_ENCODER_B_PIN);
	//MOTOR_ENCODER_PORT |= (1 << MOTOR_ENCODER_B_PIN);
	// homing
	MOTOR_HOME_IRQ_DDR &= ~(1 << MOTOR_HOME_IRQ_PIN);

	PCICR  |= (1 << PCIE1);
	PCMSK1 |= (1 << PCINT10) | (1 << PCINT9) | (1 << PCINT8);
	EICRA  |= (1 << ISC10);
}

void Motor::EncoderIrq()
{
	uint8_t enc_a = bit_is_set(MOTOR_ENCODER_PIN, MOTOR_ENCODER_A_PIN);
	uint8_t enc_b = bit_is_set(MOTOR_ENCODER_PIN, MOTOR_ENCODER_B_PIN);

	_direction_encoder = _last_encoder_val ^ (enc_a | enc_b);
	if(_direction_encoder == Forward)
	{
		actual_position++;
		if(actual_position == ENCODER_ROWS) actual_position = 0;
	}
	if(_direction_encoder == Backward)
	{
		if(actual_position == 0) actual_position = ENCODER_ROWS;
		actual_position--;
	}
	// need to measure speed
	impulses_cnt++;
	//mb.Write(ENCODER_CURRENT_VALUE, motor.actual_position);
	display.Write(actual_position);
	_last_encoder_val = (enc_a << 1) | (enc_b >> 1);
	//if(motor.actual_position == motor_data.pos) motor.Stop();
	if(!home_ok && (MOTOR_HOME_IRQ_PIN & (1 << MOTOR_HOME_IRQ_PPIN)))
	{
		EV_PhaseZ();
	}
}

ISR(PCINT1_vect)
{
	motor.EncoderIrq();
}
