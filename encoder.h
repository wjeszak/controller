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

#define ROTA !(ENCODER_PIN & (1 << ENCODER_PA))
#define ROTB !(ENCODER_PIN & (1 << ENCODER_PB))
#define ROTCLICK !(ENCODER_BUTTON_PIN & (1 << ENCODER_BUTTONP))

class Encoder
{
public:
	Encoder();
	void Poll();
private:
	uint8_t wait;
	uint8_t debounce_click;
};

extern Encoder encoder;

#endif /* ENCODER_H_ */
