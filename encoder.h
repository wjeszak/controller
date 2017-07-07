/*
 * encoder.h
 *
 *  Created on: 30 cze 2017
 *      Author: tomek
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include <avr/io.h>

#define ENCODER_DDR 			DDRB
#define ENCODER_PORT 			PORTB
#define ENCODER_PIN 			PINB

#define ENCODER_BUTTON_DDR		DDRD
#define ENCODER_BUTTON_PORT		PORTD
#define ENCODER_BUTTON_PIN 		PIND

#define ENCODER_PA 				0
#define ENCODER_PB 				1
#define ENCODER_BUTTONP			2

#define ROTA !(ENCODER_PIN & (1 << ENCODER_PA))
#define ROTB !(ENCODER_PIN & (1 << ENCODER_PB))
#define ROTCLICK !(ENCODER_BUTTON_PIN & (1 << ENCODER_BUTTONP))

class Encoder
{
public:
	Encoder();
	void CheckStatus();
	void ResetStatus();
	uint8_t GetStatus();
	uint8_t rotarystatus;
	uint8_t wait;
};

extern Encoder encoder;

#endif /* ENCODER_H_ */
