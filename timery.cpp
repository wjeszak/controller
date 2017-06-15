/*
 * timery.cpp
 *
 *  Created on: 15 cze 2017
 *      Author: tomek
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "timery.h"
void Timer_Init()
{
	licznik, t_flaga, t_flaga_licz = 0;
	TCCR0A |= (1 << WGM01); 		// CTC
	TCCR0B |= (1 << CS02) | (1 << CS00);
	OCR0A = 30;
	TIMSK0  |= (1 << OCIE0A);
}

ISR(TIMER0_COMPA_vect)
{
	licznik++;
	if(licznik == 20)
	{
		licznik = 0;
		t_flaga_licz = 1;
	}
	t_flaga = 1;
}


