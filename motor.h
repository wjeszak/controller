/*
 * motor.h
 *
 *  Created on: 20 cze 2017
 *      Author: tomek
 */

#ifndef MOTOR_H_
#define MOTOR_H_
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

enum Direction {Forward, Backward};

class Motor
{
public:
	Motor();
	//void Enable(Direction, uint8_t speed);
	void Enable(Direction, uint8_t speed);
	void Disable();
	void SetDirection(Direction dir);
	void SetSpeed(uint8_t speed);
	Direction GetDirection();
	void Test();
	enum {Acc, Decc, Const};
	uint8_t state;
	uint8_t v;
	uint16_t position;
	private:
	Direction _direction;
	uint8_t _speed;
};

extern Motor motor;

#endif /* MOTOR_H_ */



