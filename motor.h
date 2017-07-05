/*
 * motor.h
 *
 *  Created on: 20 cze 2017
 *      Author: tomek
 */

#ifndef MOTOR_H_
#define MOTOR_H_
#include "machine.h"
/* Zerowanie preskalera przy uruchamianiu silnika, czy trzeba zerowac TCNT2 po odlaczeniu i zalaczeniu preskalera ?
 *
 */
#define MOTOR_DDR 			DDRD
#define MOTOR_PORT 			PORTD
#define MOTOR_BRAKE_PIN 	3
#define MOTOR_FWD_PIN 		5
#define MOTOR_REV_PIN 		6
#define MOTOR_EN_PIN 		7
#define MOTOR_INIT 			MOTOR_DDR |= (1 << MOTOR_FWD_PIN) | (1 << MOTOR_REV_PIN) | (1 << MOTOR_EN_PIN) | (1 << MOTOR_BRAKE_PIN);

#define MOTOR_ENABLE 		TCCR2A |= (1 << COM2A1) | (1 << COM2A0);
#define MOTOR_DISABLE		TCCR2A &= ~((1 << COM2A1) | (1 << COM2A0)); MOTOR_PORT |= (1 << MOTOR_EN_PIN);

#define MOTOR_BRAKE_ON 		MOTOR_PORT |= (1 << MOTOR_BRAKE_PIN)
#define MOTOR_BRAKE_OFF 	MOTOR_PORT &= ~(1 << MOTOR_BRAKE_PIN)

#define ENCODER_ROWS 		64
enum Direction {Forward, Backward};

class MotorData : public EventData
{
public:
};

class Motor : public Machine
{
public:
	Motor();
	void Disable();
	void IrqInit();
	void SetDirection(Direction dir);
	void SetSpeed(uint8_t speed);
	Direction GetDirection();
	void EV_Homing(MotorData* pdata = NULL);
	void EV_PhaseA(MotorData* pdata = NULL);
	void EV_PhaseB(MotorData* pdata = NULL);
	void EV_PhaseZ(MotorData* pdata = NULL);
	void RunToPosition(uint16_t pos);
	uint8_t actual_speed;
	uint8_t desired_speed;
	uint16_t position;
	uint16_t desired_position;
	private:
	Direction _direction;
	void ST_Idle(MotorData* pdata);
	void ST_Acceleration(MotorData* pdata);
	void ST_Running(MotorData* pdata);
	enum States {ST_IDLE = 0, ST_ACCELERATION, ST_RUNNING, ST_HOMING, ST_DECCELERATION, ST_POSITION_ACHIEVED, ST_MAX_STATES};
	const StateStruct* GetStateMap()
	{
		// to jest sprytne bo StateMap jest tworzone nie na stosie dzieki temu mozna zwrocic adres
		static const StateStruct StateMap[] =
		{
			{reinterpret_cast<StateFunc>(&Motor::ST_Idle)},
			{reinterpret_cast<StateFunc>(&Motor::ST_Acceleration)},
			{reinterpret_cast<StateFunc>(&Motor::ST_Running)},
			//{reinterpret_cast<StateFunc>(&Motor::ST_Decceleration)},
			//{reinterpret_cast<StateFunc>(&Motor::ST_PositionAchieved)}
		};
		return &StateMap[0];
	}
};

extern Motor motor;

#endif /* MOTOR_H_ */
