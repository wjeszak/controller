/*
 * encoder.cpp
 *
 *  Created on: 30 cze 2017
 *      Author: tomek
 */

#include "encoder.h"
#include "display.h"
#include "config.h"

Encoder::Encoder()
{
	ENCODER_DDR &= ~((1 << ENCODER_PA) | (1 << ENCODER_PB));
	ENCODER_BUTTON_DDR &= ~(1 << ENCODER_BUTTONP);
	wait = 0;
	debounce_click = 0;
}

void Encoder::Poll()
{
	if(ROTA & (!wait))
		wait = 1;
	if(ROTB & ROTA & (wait))
	{
		config.EV_EncoderLeft();
		wait = 2;
	}
	else if(ROTA & (!ROTB) & wait)
	{
		config.EV_EncoderRight();
		wait = 2;
	}
	if((!ROTA) & !(ROTB) & (wait == 2))
		wait = 0;

	if(ROTCLICK)
	{
		debounce_click++;
		// debounce 50 ms
		if (ROTCLICK && debounce_click == 10)
		{
			debounce_click = 0;
			config.EV_EncoderClick();
		};
	}
}
