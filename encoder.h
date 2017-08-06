/*
 * encoder.h
 *
 *  Created on: 3 sie 2017
 *      Author: tomek
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include <avr/io.h>

#define HALF_STEP 						1 						// 0 - fullstep, 1 - halfstep

#define ENCODER_AB_PORT 				PORTC
#define ENCODER_AB_PIN 					PINC
#define ENCODER_A 						4
#define ENCODER_B 						5

#define ENCODER_A_HI 					(ENCODER_AB_PIN & (1 << ENCODER_A))
#define ENCODER_B_HI 					(ENCODER_AB_PIN & (1 << ENCODER_B))

#define ENCODER_LEFT 					0x10
#define ENCODER_RIGHT 					0x20

class Encoder
{
public:
	Encoder();
	void Poll();
	uint8_t GetCounter();
	void SetCounter(uint8_t cnt);
private:
	void Process();
	uint8_t status;
	uint8_t direction;
	uint8_t counter;
	uint8_t flag;
};

extern Encoder encoder;

#endif /* ENCODER_H_ */
