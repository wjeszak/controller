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
#define MOTOR_ENCODER_PHASEA_PIN 		0
#define MOTOR_ENCODER_PHASEB_PIN 		1
#define MOTOR_ENCODER_ENABLE 			TIMSK0 |=  (1 << OCIE0A); TIMSK1 |=  (1 << OCIE1A);
#define MOTOR_ENCODER_DISABLE 			TIMSK0 &= ~(1 << OCIE0A); TIMSK1 &= ~(1 << OCIE1A);

#define MOTOR_HOME_IRQ_DDR				DDRB
#define MOTOR_HOME_IRQ_PIN 				2
#define MOTOR_HOME_IRQ_ENABLE 			EIMSK |=  (1 << INT2);
#define MOTOR_HOME_IRQ_DISABLE 			EIMSK &= ~(1 << INT2);

#define ENCODER_ROWS 					3600



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
	void EV_PhaseA(MotorData* pdata = NULL);
	void EV_PhaseB(MotorData* pdata = NULL);
	void EV_PhaseZ(MotorData* pdata = NULL);
	void EV_Homing(MotorData* pdata = NULL);
	void EV_RunToPosition(MotorData* pdata);
	void Stop();
	uint8_t home_ok;
	uint8_t actual_speed;
	uint8_t desired_speed;
	uint16_t actual_position;
	uint16_t desired_position;
private:
	enum Direction {Forward, Backward};
	void EncoderAndHomeIrqInit();
	void SetDirection(Direction dir);
	void SetSpeed(uint8_t speed);
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
};

extern Motor motor;
extern MotorData motor_data;

#endif /* MOTOR_H_ */
