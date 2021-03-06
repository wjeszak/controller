/*
 * motor.cpp
 *
 * Created on: 20 cze 2017
 * Author: tomek
 */

#include <avr/interrupt.h>
#include <stdlib.h>						// abs()
#include "motor.h"
#include "timer.h"
#include "modbus_tcp.h"
#include "dynabox.h"
#include "display.h"
#include "config.h"

Motor::Motor() : StateMachine(ST_MAX_STATES)
{
	MOTOR_INIT;
	EncoderAndHomeIrqInit();
	_delta_time_accelerate = 	4;		// [ms]
	_delta_time_decelerate = 	12;//15;		// [ms]
	_pulses_to_decelerate = 	1600;//2000; 	// [pulses]
	_minimum_pwm_val = 			0;
	_minimum_pwm_val_forward =	40;
	_minimum_pwm_val_backward = 35;
	_correction = 				0;
	_offset =					500;
// -------------------------------------------------------------
	homing = 					true;
	_phaze_z_achieved = 		false;
	actual_position = 			0;
	desired_position = 			0;
	impulses_cnt = 				0;
	delta = 					0;
	distance = 					0;
	_direction_encoder = 		0;
	_last_encoder_val = 		0;
}
// ------------------------------------- States -------------------------------------
void Motor::ST_NotRunning(MotorData* pdata)
{
	MOTOR_STOP;
	_actual_pwm_val = 0;
}

void Motor::ST_Acceleration(MotorData* pdata)
{
	_actual_pwm_val = _minimum_pwm_val;
	_maximum_pwm_val = pdata->max_pwm_val;
	MOTOR_START;
	timer.Assign(TMotorAccelerate, _delta_time_accelerate, MotorAccelerate);
}

// from timer
void Motor::Accelerate()
{
	OCR2A = _actual_pwm_val++;
	if(_actual_pwm_val == _maximum_pwm_val)
	{
		EV_MaxPwmAchievied(&motor_data);
	}
}

void Motor::ST_Running(MotorData* pdata)
{
	timer.Disable(TMotorAccelerate);
}

void Motor::ST_Deceleration(MotorData* pdata)
{
	timer.Disable(TMotorAccelerate);			// if accelerating
	timer.Assign(TMotorDecelerate, _delta_time_decelerate, MotorDecelerate);
}

void Motor::ST_RunningMinPwm(MotorData* pdata)
{
	timer.Disable(TMotorDecelerate);
}
// ------------------------------------- Public events -------------------------------------
void Motor::EV_Start(MotorData* pdata)
{
	BEGIN_TRANSITION_MAP							// current state
		TRANSITION_MAP_ENTRY(ST_ACCELERATION)		// ST_NOT_RUNNING
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_ACCELERATION
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_RUNNING
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_DECELERATION
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_RUNNING_MIN_PWM
	END_TRANSITION_MAP(pdata)
}

void Motor::EV_Stop(MotorData* pdata)
{
	BEGIN_TRANSITION_MAP							// current state
		TRANSITION_MAP_ENTRY(ST_NOT_RUNNING)		// ST_NOT_RUNNING
        TRANSITION_MAP_ENTRY(ST_NOT_RUNNING)		// ST_ACCELERATION
        TRANSITION_MAP_ENTRY(ST_NOT_RUNNING)		// ST_RUNNING
		TRANSITION_MAP_ENTRY(ST_NOT_RUNNING)		// ST_DECELERATION
		TRANSITION_MAP_ENTRY(ST_NOT_RUNNING)		// ST_RUNNING_MIN_PWM
	END_TRANSITION_MAP(pdata)
}
// ------------------------------------- Private events -------------------------------------
void Motor::EV_MaxPwmAchievied(MotorData* pdata)
{
	BEGIN_TRANSITION_MAP							// current state
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_NOT_RUNNING
        TRANSITION_MAP_ENTRY(ST_RUNNING)			// ST_ACCELERATION
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_RUNNING
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_DECELERATION
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_RUNNING_MIN_PWM
	END_TRANSITION_MAP(pdata)
}

void Motor::EV_Decelerate(MotorData* pdata)
{
	BEGIN_TRANSITION_MAP							// current state
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_NOT_RUNNING
        TRANSITION_MAP_ENTRY(ST_DECELERATION)		// ST_ACCELERATION
        TRANSITION_MAP_ENTRY(ST_DECELERATION)		// ST_RUNNING
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_DECELERATION
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_RUNNING_MIN_PWM
	END_TRANSITION_MAP(pdata)
}

void Motor::EV_MinPwmAchieved(MotorData* pdata)
{
	BEGIN_TRANSITION_MAP							// current state
		TRANSITION_MAP_ENTRY(ST_RUNNING_MIN_PWM)	// ST_NOT_RUNNING
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_ACCELERATION
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_RUNNING
		TRANSITION_MAP_ENTRY(ST_RUNNING_MIN_PWM)	// ST_DECELERATION
		TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_RUNNING_MIN_PWM
	END_TRANSITION_MAP(pdata)
}

// from timer
void Motor::Decelerate()
{
	OCR2A = _actual_pwm_val--;
	if(homing)
	{
		if(delta > 0 && _actual_pwm_val > 0) {}
		else
		{
			timer.Disable(TMotorDecelerate);
			Event(ST_NOT_RUNNING);
			timer.Assign(TBeforeDirectionChange, BEFORE_DIRECTION_CHANGE_INTERVAL, BeforeDirectionChange);
		}
	}
	if(!homing && _actual_pwm_val == _minimum_pwm_val)
	{
		EV_MinPwmAchieved(&motor_data);
	}
}

void Motor::MovementManager()
{
	distance = ComputeDistance();
	if(homing && _phaze_z_achieved && actual_position == _offset)
	{
		_phaze_z_achieved = false;
		actual_position = 0;
		EV_Decelerate(&motor_data);
	}

	if(homing && _direction_encoder == Backward && actual_position == _correction)
	{
		homing = false;
		EV_Stop(&motor_data);
		dynabox.EV_HomingDone(&dynabox_data);
	}
	if(!homing && abs(distance) <= _pulses_to_decelerate)
	//if(!homing && abs(distance) <= _pulses_to_decelerate && GetState() != ST_RUNNING_MIN_PWM && _actual_pwm_val == _maximum_pwm_val)
	{
		EV_Decelerate(&motor_data);
	}

	if(!homing && distance == _correction)
	{
		EV_Stop(&motor_data);
		dynabox.EV_PositionAchieved(&dynabox_data);
		motor_data.pos = 0;
	}
}

void Motor::EV_PhaseZ(MotorData* pdata)
{
    _phaze_z_achieved = true;
    actual_position = 0;
}

void Motor::SetDirection(Direction dir)
{
	switch (dir)
	{
	case Forward:
		MOTOR_PORT &= ~(1 << MOTOR_REV_PIN);
		MOTOR_PORT |=  (1 << MOTOR_FWD_PIN);
		_minimum_pwm_val = _minimum_pwm_val_forward;
		motor_data.max_pwm_val = 173;
		break;
	case Backward:
		MOTOR_PORT &= ~(1 << MOTOR_FWD_PIN);
		MOTOR_PORT |=  (1 << MOTOR_REV_PIN);
		_minimum_pwm_val = _minimum_pwm_val_backward;
		motor_data.max_pwm_val = 158;
		break;
	}
}

int16_t Motor::ComputeDistance()
{
	int16_t dist = motor_data.pos - actual_position;
	if(dist >= 0)
	{
		if(dist < ENCODER_ROWS / 2) {}
		else
			dist = ENCODER_ROWS - motor_data.pos + actual_position;
	}
	else
	{
		if(abs(dist) < ENCODER_ROWS / 2) {}
		else
			dist = ENCODER_ROWS - actual_position + motor_data.pos;
	}
	return dist;
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
		if(abs(dist) < ENCODER_ROWS / 2)
			SetDirection(Backward);
		else
			SetDirection(Forward);
	}
}

void Motor::ComputeMaxPwm()
{
	int16_t dist = ComputeDistance();
	//if(dist < 0) display.Write(1234);
	if(abs(dist) < 120 * 4) motor_data.max_pwm_val = 130;
	if(abs(dist) >= 120 * 4 && abs(dist) < 220 * 4) motor_data.max_pwm_val = 155;
	//if(abs(dist) > 220 * 4) motor_data.max_pwm_val = 173;
}

void Motor::SpeedMeasure()
{
	delta = impulses_cnt;		// [imp / s] @ 100 ms timer
	if(GetState() == ST_RUNNING_MIN_PWM)
	{
		if(delta < 3)
		{
			OCR2A = _actual_pwm_val++;
		}
/*		if(distance <= 400 && distance > 200 && delta > 2)
		{
			OCR2A = _actual_pwm_val--;
		}
		if(distance <= 200 && delta >= 2)
		{
			OCR2A = _actual_pwm_val--;
		}
*/

	}
	//display.Write(_actual_pwm_val);
	impulses_cnt = 0;
	//display.Write(GetState());
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

	mb.Write(ENCODER_CURRENT_VALUE, motor.actual_position / 4);
	MovementManager();
	_last_encoder_val = (enc_a << 1) | (enc_b >> 1);

	if(homing && (MOTOR_HOME_IRQ_PIN & (1 << MOTOR_HOME_IRQ_PPIN)))
	{
		EV_PhaseZ(&motor_data);
	}
}

ISR(PCINT1_vect)
{
	dynabox.encoder_irq_flag = 1;
	//motor.EncoderIrq();
}

void Motor::Debug()
{
	int16_t dist = ComputeDistance();
	uint8_t act_pos_hi = actual_position >> 8;
	uint8_t act_pos_lo = actual_position & 0xFF;
	uint8_t dist_hi = dist >> 8;
	uint8_t dist_lo = dist & 0xFF;
	//uint8_t delta_hi = delta >> 8;
	//uint8_t delta_lo = delta & 0xFF;
	usart_data.frame[0] = act_pos_hi;
	usart_data.frame[1] = act_pos_lo;
	usart_data.frame[2] = _actual_pwm_val;
	usart_data.frame[3] = dist_hi;
	usart_data.frame[4] = dist_lo;
	usart_data.len = FRAME_LENGTH_REQUEST;
	usart.SendFrame(&usart_data);
}
