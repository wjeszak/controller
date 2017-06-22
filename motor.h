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

#define MOTOR_ENABLE 		TCCR2B |= T2_PS_1
#define MOTOR_DISABLE		TCCR2B |= T2_PS_0

#define MOTOR_BRAKE_ON 		MOTOR_PORT |= (1 << MOTOR_BRAKE_PIN)
#define MOTOR_BRAKE_OFF 	MOTOR_PORT &= ~(1 << MOTOR_BRAKE_PIN)

enum T2Prescallers
{
	T2_PS_0 = 0,
	T2_PS_1 = (1 << CS20),
	T2_PS_8 = (1 << CS21),
	T2_PS_32 = (1 <<CS21) | (1 << CS20),
	T2_PS_64 = (1 << CS22),
	T2_PS_128 = (1 << CS22) | (1 << CS20),
	T2_PS_256 = (1 << CS22) | (1 << CS21),
	T2_PS_1024 = (1 << CS22) | (1 << CS21) | (1 << CS20)
};

enum Direction {Forward, Backward};

class Motor
{
public:
	Motor();
	void Enable(Direction, uint8_t speed);
	void Disable();
	void SetDirection(Direction dir);
	void SetSpeed(uint8_t speed);
	Direction GetDirection();
	private:
	Direction _direction;
	uint8_t _speed;
};
extern Motor motor;

#endif /* MOTOR_H_ */



