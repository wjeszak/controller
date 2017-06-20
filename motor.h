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

enum Prescallers
{
	PS_0 = 0,
	PS_1 = (1 << CS20),
	PS_8 = (1 << CS21),
	PS_32 = (1 <<CS21) | (1 << CS20),
	PS_64 = (1 << CS22),
	PS_128 = (1 << CS22) | (1 << CS20),
	PS_256 = (1 << CS22) | (1 << CS21),
	PS_1024 = (1 < CS22) | (1 < CS21) | (1 << CS20)
};

enum TDirection {TForward, TBackward};

class Motor
{
public:
	Motor();
	void Enable(TDirection, uint8_t speed);
	void Disable();
	private:
	TDirection _direction;
	uint8_t _speed;
	void SetDirection(TDirection dir);
};

#endif /* MOTOR_H_ */



