/*
 * encoder.cpp
 *
 *  Created on: 3 sie 2017
 *      Author: tomek
 */

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "encoder.h"
#include "config.h"
#include "timer.h"
#include "eeprom.h"

#if HALF_STEP == 1
const uint8_t tab[6][4] PROGMEM =
{
	{0x03, 0x02, 0x01, 0x00}, {0x23, 0x00, 0x01, 0x00},
	{0x13, 0x02, 0x00, 0x00}, {0x03, 0x05, 0x04, 0x00},
	{0x03, 0x03, 0x04, 0x10}, {0x03, 0x05, 0x04, 0x20}
};
#else
const uint8_t tab[7][4] PROGMEM =
{
	{0x00, 0x02, 0x04, 0x00}, {0x03, 0x00, 0x01, 0x10},
	{0x03, 0x02, 0x00, 0x00}, {0x03, 0x02, 0x01, 0x00},
	{0x06, 0x00, 0x04, 0x00}, {0x06, 0x05, 0x00, 0x20},
	{0x06, 0x05, 0x04, 0x00},
};
#endif

Encoder::Encoder() : status(0), direction(0), counter(0), flag(0)
{
	ENCODER_AB_PORT |= (1 << ENCODER_A) | (1 << ENCODER_B);
	Process();
}

bool Encoder::CheckSwitch()
{
	if(!(ENCODER_SW_PIN & (1 << ENCODER_SW))) return true;
	return false;
}

void Encoder::Process()
{
	register uint8_t ABstate = ((ENCODER_B_HI) ? 2 : 0) | ((ENCODER_A_HI) ? 1 : 0);
	status = pgm_read_byte(&tab[status & 0x0F][ABstate]);
	ABstate = (status & 0x30);
	if(ABstate)
	{
		direction = ABstate;
		if(direction == ENCODER_RIGHT)
			counter++;
		else
			counter--;
		flag = 1;
	}
}

void Encoder::Poll()
{
	static uint8_t debounce;
	Process();
	if(flag)
	{
		flag = 0;
		config_data.val = counter;
		config.EV_Encoder(&config_data);
	}

	if(CheckSwitch())
	{
		debounce++;
		if(debounce == 200) // 50 ms
		{
			debounce = 0;
			config.EV_EncoderClick(&config_data);
		}
	}
}

uint8_t Encoder::GetCounter()
{
	return counter;
}

void Encoder::SetCounter(uint8_t cnt)
{
	counter = cnt;
}
