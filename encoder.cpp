/*
 * encoder.cpp
 *
 *  Created on: 30 cze 2017
 *      Author: tomek
 */

#include "encoder.h"
#include "display.h"

Encoder::Encoder()
{
	ENCODER_DDR &= ~((1 << ENCODER_PA) | (1 << ENCODER_PB));
	ENCODER_BUTTON_DDR &= ~(1 << ENCODER_BUTTONP);
	wait = 0;
}

void Encoder::Poll()
{
	static uint8_t param;
	if(ROTA & (!wait))
		wait = 1;
	if(ROTB & ROTA & (wait))
	{
		param--;
		display.Write(Parameter, param);
		wait = 2;
	}
	else if(ROTA & (!ROTB) & wait)
	{
		param++;
		display.Write(Parameter, param);
		wait = 2;
	}
	if((!ROTA) & !(ROTB) & (wait == 2))
		wait = 0;

	if(ROTCLICK)
	{
		// debounce
		for(volatile uint16_t x = 0; x < 0x0FFF; x++);
		if (ROTCLICK) {};
	}
}
