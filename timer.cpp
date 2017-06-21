/*
 * timery.cpp
 *
 *  Created on: 15 cze 2017
 *      Author: tomek
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "wyswietlacz.h"
uint8_t volatile t_flaga, t_licznik, t_flaga_dluga;
extern Wyswietlacz wysw;
void Timer_Init()
{
	TCCR0A |= (1 << WGM01); 		// CTC
	TCCR0B |= (1 << CS02) | (1 << CS00);
	OCR0A = 45; 		// :		F_CPU / preskaler / 200 Hz = OCR
	TIMSK0  |= (1 << OCIE0A);
}

ISR(TIMER0_COMPA_vect)
{
	t_licznik++;
	if(t_licznik == 200)
	{
		t_licznik = 0;
		t_flaga_dluga = 1;
	}
	wysw.Odswiez();
	//t_flaga = 1;
}


