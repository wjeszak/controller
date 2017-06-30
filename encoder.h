/*
 * encoder.h
 *
 *  Created on: 30 cze 2017
 *      Author: tomek
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include <avr/io.h>

#define ENCODER_DDR 			DDRC
#define ENCODER_PORT 			PORTC
#define ENCODER_PIN 			PINC

#define ENCODER_BUTTON_DDR		DDRD
#define ENCODER_BUTTON_PORT		PORTD
#define ENCODER_BUTTON_PIN 		PIND

#define ENCODER_PA 				4
#define ENCODER_PB 				5
#define ENCODER_BUTTONP			2
//define macros to check status
#define ROTA !((1 << ENCODER_PA) & ENCODER_PIN)
#define ROTB !((1 << ENCODER_PB) & ENCODER_PIN)
#define ROTCLICK !((1 << ENCODER_BUTTONP) & ENCODER_BUTTON_PIN)

class Encoder
{
public:
	void Init();
	void CheckStatus();
	void ResetStatus();
	uint8_t GetStatus();
	uint8_t rotarystatus;
	uint8_t wait;
};

extern Encoder encoder;

#endif /* ENCODER_H_ */
