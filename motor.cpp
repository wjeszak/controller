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
#include "config.h"
#include <util/delay.h>

Motor::Motor() : StateMachine(ST_MAX_STATES)
{
	MOTOR_INIT;
	EncoderAndHomeIrqInit();
	delta_time_accelerate = 	4;		// [ms]
	delta_time_decelerate = 	4;		// [ms]
	pulses_to_decelerate = 		1000; 	// [pulses]
	minimum_pwm_val = 			50;
	offset =					500;
// -------------------------------------------------------------
	homing = 					true;
	phaze_z_achieved = 			false;
	actual_position = 			0;
	desired_position = 			0;
	impulses_cnt = 				0;
	delta = 					0;
	distance = 					0;
	_direction_encoder = 		0;
	_last_encoder_val = 		0;
	old_imp = 					0;
	tmp = 0;
}

void Motor::Start()
{
	actual_pwm = minimum_pwm_val;
	MOTOR_START;
}

void Motor::Stop()
{
	MOTOR_STOP;
	actual_pwm = 0;
}
// from timer
void Motor::Accelerate()
{
	OCR2A = actual_pwm++;
	if(actual_pwm == maximum_pwm_val)
	{
		timer.Disable(TIMER_MOTOR_ACCELERATE);
		Event(ST_RUNNING, &motor_data);

	}
}
// from timer
void Motor::Decelerate()
{
	OCR2A = actual_pwm--;
	if(!homing && actual_pwm == minimum_pwm_val)
	{
		timer.Disable(TIMER_MOTOR_DECELERATE);
		Event(ST_RUNNING_MIN_PWM, &motor_data);
	}
	if(homing)
	{
		//if(actual_pwm > 0) {}
		if(delta > 0 && actual_pwm > 0) {}
		else
		{
			timer.Disable(TIMER_MOTOR_DECELERATE);
			timer.Assign(TIMER_BEFORE_DIRECTION_CHANGE, BEFORE_DIRECTION_CHANGE_INTERVAL, BeforeDirectionChange);
		}
	}
}

void Motor::EV_Homing(MotorData* pdata)
{
    BEGIN_TRANSITION_MAP							// current state
        TRANSITION_MAP_ENTRY(ST_ACCELERATION)		// ST_IDLE
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_ACCELERATION
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_RUNNING
    END_TRANSITION_MAP(pdata)
	mb.Write(IO_INFORMATIONS, (1 << 2) | (1 << 0));
    SetDirection(Forward);
	maximum_pwm_val = 173;
}

void Motor::NeedDeceleration()
{
	ComputeDistance();
	if(homing && phaze_z_achieved && actual_position == offset)
	{
		phaze_z_achieved = false;
		timer.Disable(TIMER_MOTOR_ACCELERATE);			// if accelerating
		timer.Assign(TIMER_MOTOR_DECELERATE, delta_time_decelerate, MotorDecelerate);
		Event(ST_DECELERATION, &motor_data);
		actual_position = 0;
	}

	if(homing && _direction_encoder == Backward && actual_position - 20 == 0)
	{
		homing = false;
		Stop();
		Event(ST_HOME, &motor_data);
	//	return;
	}

	//if(!homing && actual_position == motor_data.pos)
	//if(!homing && actual_position == motor_data.pos - pulses_to_decelerate)
	if(!homing && distance == pulses_to_decelerate)
	{
		//Stop();
		timer.Disable(TIMER_MOTOR_ACCELERATE);			// if accelerating
		timer.Assign(TIMER_MOTOR_DECELERATE, delta_time_decelerate, MotorDecelerate);
		Event(ST_DECELERATION, &motor_data);
	}

	if(!homing && distance == 20)
	//if(!homing && actual_position == motor_data.pos)
	{
		Stop();
		Event(ST_POSITION_ACHIEVED, &motor_data);
	}
}

void Motor::EV_PhaseZ(MotorData* pdata)
{
    BEGIN_TRANSITION_MAP							// current state
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_IDLE
        TRANSITION_MAP_ENTRY(ST_HOME)				// ST_ACCELERATION
        TRANSITION_MAP_ENTRY(ST_HOME)				// ST_RUNNING
    END_TRANSITION_MAP(pdata)
    phaze_z_achieved = true;
    actual_position = 0;
}

void Motor::EV_RunToZero(MotorData* pdata)
{
	mb.Write(ORDER_STATUS, ORDER_STATUS_PROCESSING);
	mb.Write(IO_INFORMATIONS, (1 << 0) | (1 << 3));
	Event(ST_ACCELERATION, &motor_data);
}

void Motor::EV_RunToPosition(MotorData* pdata)
{
//	if(home_ok)
//	{
//		BEGIN_TRANSITION_MAP						// current state
//			TRANSITION_MAP_ENTRY(ST_ACCELERATION)	// ST_IDLE
//			TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED) 	// ST_ACCELERATION
//			TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)	// ST_RUNNING
//			TRANSITION_MAP_ENTRY(ST_ACCELERATION)	// ST_HOME
//			TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)	// ST_DECELERATION
//			TRANSITION_MAP_ENTRY(ST_ACCELERATION)	// ST_POSITION_ACHIEVED
//	    END_TRANSITION_MAP(pdata)
		mb.Write(ORDER_STATUS, ORDER_STATUS_PROCESSING);
		mb.Write(IO_INFORMATIONS, (1 << 0) | (1 << 3));
		maximum_pwm_val = 173;
		ComputeDirection();
		ComputeDistance();
		Event(ST_ACCELERATION, &motor_data);
//	}
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
	distance = abs(motor_data.pos - actual_position);
}

void Motor::ComputeDirection()
{
	int16_t dist = motor_data.pos - actual_position;
	if(dist > 0)
	{
		if(dist < ENCODER_ROWS / 2)
			SetDirection(Forward);
		else
			SetDirection(Backward);
	}
	else
	{
		if(dist < ENCODER_ROWS / 2)
			SetDirection(Backward);
		else
			SetDirection(Forward);
	}
	display.Write(dist);
}

void Motor::ST_Idle(MotorData* pdata)
{

}

void Motor::ST_Acceleration(MotorData* pdata)
{
	Start();
	timer.Assign(TIMER_MOTOR_ACCELERATE, delta_time_accelerate, MotorAccelerate);
}

void Motor::ST_Running(MotorData* pdata)
{

}

void Motor::ST_RunningMinPwm(MotorData* pdata)
{
	if(homing)
	{
		//if(delta != 0) actual_pwm--;
		//if(delta == 0) Stop();
		//SetDirection(Backward);
		//SetMaxPwm(27);
		//Start();
	}

	//if(!homing)
	//{
	//	if(actual_position == motor_data.pos)
	//	{
	//		Stop();
	//		Event(ST_POSITION_ACHIEVED, &motor_data);
	//	}
	//}
}

void Motor::ST_Home(MotorData* pdata)
{
	mb.Write(IO_INFORMATIONS, (0 << 2) | (0 << 0) | (1 << 3));
	dynabox.EV_HomingDone(&dynabox_data);
	Event(ST_IDLE, &motor_data);
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
	delta = impulses_cnt;		// [imp / s] @ 100 ms timer
	//display.Write(actual_pwm);
	uint8_t act_pos_hi = actual_position >> 8;
	uint8_t act_pos_lo = actual_position & 0xFF;
	uint8_t dist_hi = distance >> 8;
	uint8_t dist_lo = distance & 0xFF;
	//uint8_t delta_hi = delta >> 8;
	//uint8_t delta_lo = delta & 0xFF;

	usart_data.frame[0] = act_pos_hi;
	usart_data.frame[1] = act_pos_lo;
	usart_data.frame[2] = actual_pwm;
	usart_data.frame[3] = dist_hi;
	usart_data.frame[4] = dist_lo;
	//usart_data.frame[3] = delta_hi;
	//usart_data.frame[4] = delta_lo;
	usart_data.len = FRAME_LENGTH_REQUEST;
	usart.SendFrame(&usart_data);

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
		impulses_cnt++;
	}
	if(_direction_encoder == Backward)
	{
		if(actual_position == 0) actual_position = ENCODER_ROWS;
		actual_position--;
		impulses_cnt++;
	}

	mb.Write(ENCODER_CURRENT_VALUE, motor.actual_position);
	NeedDeceleration();
	_last_encoder_val = (enc_a << 1) | (enc_b >> 1);

	if(homing && (MOTOR_HOME_IRQ_PIN & (1 << MOTOR_HOME_IRQ_PPIN)))
	{
		EV_PhaseZ(&motor_data);
	}
}

ISR(PCINT1_vect)
{
	motor.EncoderIrq();
}
