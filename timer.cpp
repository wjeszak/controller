/*
 * timery.cpp
 *
 *  Created on: 15 cze 2017
 *      Author: tomek
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>			// NULL
#include "timer.h"
#include "display.h"
#include "usart.h"
#include "modbus_rtu.h"
#include "motor.h"
#include "encoder.h"

volatile TimerHandler STHandlers[8];

// obsluga timera 2, najbardziej rozbudowanego

Timer2::Timer2(T2Prescallers Prescaller, uint8_t Tick)
{
	for(uint8_t n = 0; n < 8; n++)
	{
		STHandlers[n].Active = false;
		STHandlers[n].Counter = 0;
		STHandlers[n].Interval = 0;
		STHandlers[n].fp = NULL;
	}
	TCCR2A |= (1 << WGM21) | (1 << WGM20);
	TCCR2B |= Prescaller;
	OCR2B = Tick; 					// :		F_CPU / preskaler / 200 Hz = OCR
	TIMSK2 |= (1 << OCIE2B);
}

void Timer2::Assign(uint8_t HandlerNumber, uint64_t Interval, void(*fp)())
{
	STHandlers [HandlerNumber].Interval = Interval;
	STHandlers [HandlerNumber].Counter = 0;
	STHandlers [HandlerNumber].Active = true;
	STHandlers [HandlerNumber].fp = fp;
}

void Timer2::Enable(uint8_t HandlerNumber)
{
	STHandlers [HandlerNumber].Active = true;
	STHandlers [HandlerNumber].Counter = 0;
}

void Timer2::Disable(uint8_t HandlerNumber)
{
	STHandlers [HandlerNumber].Active = false;
}
// 0
void DisplayRefresh()
{
	display.Refresh();
}
// 1
void ModbusRTU35T()
{
	usart.RTU35T();
}

// 2
void ModbusPoll()
{
	modbus_rtu.Poll();
}

void EncoderStatus()
{
	encoder.CheckStatus();
}

Timer0::Timer0()
{
	DDRB &= ~(1 << PB0);
	PORTB |= (1 << PB0);
	TCCR0A |= (1 << WGM01);
	TCCR0B |= (1 << CS02) | (1 << CS01);  //zrod³o zewnêtrzne - zbocze opadaj¹ce
	TCNT0 = 0;
	TIMSK0 |= (1 << OCIE0A);
	OCR0A = 1;
}

Timer1::Timer1()
{
	DDRB &= ~(1 << PB1);
	PORTB |= (1 << PB1);
	TCCR1B |= (1 << WGM12);
	TCCR1B |= (1<< CS12) | (1<< CS11) | (1 << CS10);  //zrod³o zewnêtrzne - zbocze narastajace
	TCNT1 = 0;
	TIMSK1 |= (1 << OCIE1A);
	OCR1A = 1;
}


ISR(TIMER2_COMPB_vect)
{
	for(uint8_t n = 0; n < 8; n++)
	{
		if ((STHandlers[n].Active) && (STHandlers[n].fp != NULL))
		{
			if ((STHandlers[n].Counter == STHandlers[n].Interval))
			{
				STHandlers[n].Counter = 0;
				STHandlers [n].fp();
			}
			else
			{
				STHandlers[n].Counter++;
			}
		}
	}
}

ISR(TIMER0_COMPA_vect)
{
	motor.EV_PhaseA(NULL);
}

ISR(TIMER1_COMPA_vect)
{
	motor.EV_PhaseB(NULL);
}
