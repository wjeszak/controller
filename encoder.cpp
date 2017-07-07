/*
 * encoder.cpp
 *
 *  Created on: 30 cze 2017
 *      Author: tomek
 */

#include "encoder.h"

Encoder::Encoder()
{
	ENCODER_DDR &= ~((1 << ENCODER_PA) | (1 << ENCODER_PB));
	ENCODER_BUTTON_DDR &= ~(1 << ENCODER_BUTTONP);
	ENCODER_PORT |= (1 << ENCODER_PA) | (1 << ENCODER_PB);
	ENCODER_BUTTON_PORT |= (1 << ENCODER_BUTTON_PIN);
	rotarystatus = 0;
	wait = 0;
}

void Encoder::CheckStatus()
{
	if(ROTA & (!wait))
		wait = 1;
	if(ROTB & ROTA & (wait))
	{
		rotarystatus = 2;
		wait = 2;
	}
	else if(ROTA & (!ROTB) & wait)
	{
		rotarystatus = 1;
		wait = 2;
	}
	if((!ROTA) & !(ROTB) & (wait == 2))
		wait = 0;
	//check button status
	if(ROTCLICK)
	{
		// debounce
		for(volatile uint16_t x = 0; x < 0x0FFF; x++);
		if (ROTCLICK) rotarystatus = 3;
	}
}

uint8_t Encoder::GetStatus()
{
	return rotarystatus;
}

void Encoder::ResetStatus()
{
	rotarystatus = 0;
}
