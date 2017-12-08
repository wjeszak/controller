/*
 * motor.h
 *
 * Created on: 20 cze 2017
 * Author: tomek
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include "state_machine.h"

#define MOTOR_DDR 						DDRD
#define MOTOR_PORT 						PORTD
#define MOTOR_BRAKE_PIN 				3
#define MOTOR_FWD_PIN 					5
#define MOTOR_REV_PIN 					6
#define MOTOR_EN_PIN 					7
#define MOTOR_INIT 						MOTOR_DDR |= (1 << MOTOR_FWD_PIN) | (1 << MOTOR_REV_PIN) | (1 << MOTOR_EN_PIN) | (1 << MOTOR_BRAKE_PIN);

#define MOTOR_START 					TCCR2A |= (1 << COM2A1) | (1 << COM2A0);
#define MOTOR_STOP						TCCR2A &= ~((1 << COM2A1) | (1 << COM2A0)); MOTOR_PORT |= (1 << MOTOR_EN_PIN);

#define MOTOR_ENCODER_DDR 				DDRB
#define MOTOR_ENCODER_PORT 				PORTB
#define MOTOR_ENCODER_PIN 				PINB
#define MOTOR_ENCODER_A_PIN		 		0
#define MOTOR_ENCODER_B_PIN 			1

#define MOTOR_HOME_IRQ_DDR				DDRB
#define MOTOR_HOME_IRQ_PIN 				PINB
#define MOTOR_HOME_IRQ_PPIN				2

#define ENCODER_ROWS 					14400

class MotorData : public EventData
{
public:
	uint16_t pos;
	uint8_t max_pwm_val;
};

class Motor : public StateMachine
{
public:
	Motor();
	void ComputeDirection();
	void ComputeDistance();
	void ComputeMaxPwm();
	void EV_Start(MotorData* pdata);
	void EV_Stop(MotorData* pdata);
	void Accelerate();
	void Decelerate();
	void SpeedMeasure();
	void EV_PhaseZ(MotorData* pdata);
	void EncoderIrq();
	bool homing;

	uint16_t actual_position;
	uint16_t desired_position;
	uint16_t delta;
	uint16_t impulses_cnt;
	int16_t distance;

	enum Direction {Forward = 1, Backward};
	void SetDirection(Direction dir);
private:
	void EncoderAndHomeIrqInit();
	void MovementManager();
	void ST_NotRunning(MotorData* pdata);
	void ST_Acceleration(MotorData* pdata);
	void ST_Running(MotorData* pdata);
	void ST_Deceleration(MotorData* pdata);
	void ST_RunningMinPwm(MotorData* pdata);
	void EV_MaxPwmAchievied(MotorData* pdata);
	void EV_Decelerate(MotorData* pdata);
	void EV_MinPwmAchieved(MotorData* pdata);
	enum States {ST_NOT_RUNNING = 0, ST_ACCELERATION, ST_RUNNING, ST_DECELERATION, ST_RUNNING_MIN_PWM, ST_MAX_STATES};
	BEGIN_STATE_MAP
		STATE_MAP_ENTRY(&Motor::ST_NotRunning)
		STATE_MAP_ENTRY(&Motor::ST_Acceleration)
		STATE_MAP_ENTRY(&Motor::ST_Running)
		STATE_MAP_ENTRY(&Motor::ST_Deceleration)
		STATE_MAP_ENTRY(&Motor::ST_RunningMinPwm)
	END_STATE_MAP
// ----------------------------------------------------------
// parameters
	uint8_t  _delta_time_accelerate;
	uint8_t  _delta_time_decelerate;
	uint16_t _pulses_to_decelerate;
	uint8_t  _minimum_pwm_val;
	uint8_t  _minimum_pwm_val_forward;
	uint8_t  _minimum_pwm_val_backward;
	uint8_t  _maximum_pwm_val;
	uint8_t  _correction;
	uint16_t _offset;

	uint8_t  _actual_pwm_val;
	uint8_t  _direction_encoder;
	uint8_t  _last_encoder_val;
	bool 	 _phaze_z_achieved;
};

extern Motor motor;
extern MotorData motor_data;

#endif /* MOTOR_H_ */
