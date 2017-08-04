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
	ENCODER_AB_PORT |= ENCODER_A | ENCODER_B;
//	ENCODER_SW_PORT |= (1 << ENCODER_SW);
	Process();
}

bool Encoder::CheckVal()
{
	if(!(ENCODER_SW_PIN & (1 << ENCODER_SW))) return true;
	return false;
}

void Encoder::Process()
{
	register uint8_t pin = ENCODER_AB_PIN;
	register uint8_t ABstate = ((pin & ENCODER_B) ? 2 : 0) | ((pin & ENCODER_A) ? 1 : 0);
	status = pgm_read_byte(&tab[status & 0x0F][ABstate]);
	ABstate = (status & 0x30);
	if(ABstate)
	{
		direction = ABstate;
		if(direction == ENCODER_RIGHT) counter++;
			else counter--;
		if(counter == 0xFF) counter = 0;
		if(counter >= (MAX_FUNCTIONS - 1)) counter = MAX_FUNCTIONS - 1;
		flag = 1;
	}
}

void Encoder::Poll()
{
	Process();

	if(flag)
	{
		flag = 0;
		config_data.val = counter;
		config.EV_Encoder(&config_data);
	}

//	if(CheckVal())
//	{
//		timer.Assign(TIMER_ENCODER_CLICK_DEBOUNCE, 100, EncoderClickDebounce);
//	}
}

uint8_t Encoder::GetCounter()
{
	return counter;
}

void Encoder::SetCounter(uint8_t cnt)
{
	counter = cnt;
}
