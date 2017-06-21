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

#define MOTOR_ENABLE 		TCCR2B |= PS_1
#define MOTOR_DISABLE		TCCR2B |= PS_0

#define MOTOR_BRAKE_ON 		MOTOR_PORT |= (1 << MOTOR_BRAKE_PIN)
#define MOTOR_BRAKE_OFF 	MOTOR_PORT &= ~(1 << MOTOR_BRAKE_PIN)

enum Prescallers
{
	PS_0 = 0,
	PS_1 = (1 << CS20),
	PS_8 = (1 << CS21),
	PS_32 = (1 <<CS21) | (1 << CS20),
	PS_64 = (1 << CS22),
	PS_128 = (1 << CS22) | (1 << CS20),
	PS_256 = (1 << CS22) | (1 << CS21),
	PS_1024 = (1 << CS22) | (1 << CS21) | (1 << CS20)
};

enum Direction {Forward, Backward};

class Motor
{
public:
	Motor();
	void Enable(Direction, uint8_t speed);
	void Disable();
	void SetDirection(Direction dir);
	private:
	Direction direction_;
	uint8_t speed_;
};

#endif /* MOTOR_H_ */



