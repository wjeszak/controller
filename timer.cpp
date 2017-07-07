/*
 * timer.cpp
 *
 *  Created on: 15 cze 2017
 *      Author: tomek
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "timer.h"
#include "display.h"
#include "usart.h"
#include "modbus_rtu.h"
#include "motor.h"
#include "encoder.h"

volatile TimerHandler timer_handlers[8];

Timer::Timer(T2Prescallers prescaller, uint8_t tick)
{
	for(uint8_t n = 0; n < 8; n++)
	{
		timer_handlers[n].active = false;
		timer_handlers[n].counter = 0;
		timer_handlers[n].interval = 0;
		timer_handlers[n].fp = NULL;
	}
	TCCR2A |= (1 << WGM21) | (1 << WGM20);
	TCCR2B |= prescaller;
	OCR2B = tick; 					// :		F_CPU / preskaler / 200 Hz = OCR
	TIMSK2 |= (1 << OCIE2B);
}

void Timer::Assign(uint8_t handler_id, uint64_t interval, void(*fp)())
{
	timer_handlers[handler_id].interval = interval;
	timer_handlers[handler_id].counter = 0;
	timer_handlers[handler_id].active = true;
	timer_handlers[handler_id].fp = fp;
}

void Timer::Enable(uint8_t handler_id)
{
	timer_handlers[handler_id].active = true;
	timer_handlers[handler_id].counter = 0;
}

void Timer::Disable(uint8_t handler_id)
{
	timer_handlers[handler_id].active = false;
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
		if ((timer_handlers[n].active) && (timer_handlers[n].fp != NULL))
		{
			if ((timer_handlers[n].counter == timer_handlers[n].interval))
			{
				timer_handlers[n].counter = 0;
				timer_handlers[n].fp();
			}
			else
			{
				timer_handlers[n].counter++;
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
