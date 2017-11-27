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

//#define MOTOR_BRAKE_ON 			MOTOR_PORT |= (1 << MOTOR_BRAKE_PIN)
//#define MOTOR_BRAKE_OFF 			MOTOR_PORT &= ~(1 << MOTOR_BRAKE_PIN)

#define MOTOR_ENCODER_DDR 				DDRB
#define MOTOR_ENCODER_PORT 				PORTB
#define MOTOR_ENCODER_PIN 				PINB
#define MOTOR_ENCODER_A_PIN		 		0
#define MOTOR_ENCODER_B_PIN 			1

#define MOTOR_HOME_IRQ_DDR				DDRB
#define MOTOR_HOME_IRQ_PIN 				PINB
#define MOTOR_HOME_IRQ_PPIN				2

#define ENCODER_ROWS 					50

class MotorData : public EventData
{
public:
	uint16_t pos;
};

class Motor : public StateMachine
{
public:
	Motor();
	void Accelerate();
	void Decelerate();
	void SpeedMeasure();
	void EV_PhaseZ(MotorData* pdata = NULL);
	void EV_Homing(MotorData* pdata = NULL);
	void EV_RunToPosition(MotorData* pdata);
	void Start();
	void Stop();
	void EncoderIrq();
	bool home_ok;
	uint8_t actual_speed;
	uint8_t desired_speed;
	uint16_t actual_position;
	uint16_t desired_position;
	// parameters
	uint8_t delta_time_accelerate;
	uint8_t delta_time_decelerate;
	uint16_t pulses_to_decelerate;
	uint8_t max_speed;
	uint16_t impulses_cnt;
	uint8_t init_pwm_val;
	int16_t distance;
	enum Direction {Forward = 1, Backward};
private:
	void ComputeDistance();
	void ComputeDirection();
	void SetDirection(Direction dir);
	void EncoderAndHomeIrqInit();
	void NeedDeceleration();
	void SetSpeed(uint8_t speed);
	void GetStartPwmVal();
	void ST_Idle(MotorData* pdata);
	void ST_Acceleration(MotorData* pdata);
	void ST_Running(MotorData* pdata);
	void ST_Home(MotorData* pdata);
	void ST_Deceleration(MotorData* pdata);
	void ST_PositionAchieved(MotorData* pdata);
	enum States {ST_IDLE = 0, ST_ACCELERATION, ST_RUNNING, ST_HOME, ST_DECELERATION, ST_POSITION_ACHIEVED, ST_MAX_STATES};
	BEGIN_STATE_MAP
		STATE_MAP_ENTRY(&Motor::ST_Idle)
		STATE_MAP_ENTRY(&Motor::ST_Acceleration)
		STATE_MAP_ENTRY(&Motor::ST_Running)
		STATE_MAP_ENTRY(&Motor::ST_Home)
		STATE_MAP_ENTRY(&Motor::ST_Deceleration)
		STATE_MAP_ENTRY(&Motor::ST_PositionAchieved)
	END_STATE_MAP

	Direction _direction;
	uint8_t _direction_encoder;
	uint8_t _last_encoder_val;
};

extern Motor motor;
extern MotorData motor_data;

#endif /* MOTOR_H_ */
