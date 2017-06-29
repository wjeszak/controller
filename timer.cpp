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

volatile TimerHandler STHandlers[8];

Timer::Timer(T0Prescallers Prescaller, uint8_t Tick)
{
	for(uint8_t n = 0; n < 8; n++)
	{
		STHandlers[n].Active = false;
		STHandlers[n].Counter = 0;
		STHandlers[n].Interval = 0;
		STHandlers[n].fp = NULL;
	}
	TCCR0A |= (1 << WGM01); 		// CTC
	TCCR0B |= Prescaller;
	OCR0A = Tick; 					// :		F_CPU / preskaler / 200 Hz = OCR
	TIMSK0 |= (1 << OCIE0A);
}

void Timer::Assign(uint8_t HandlerNumber, uint64_t Interval, void(*fp)())
{
	STHandlers [HandlerNumber].Interval = Interval;
	STHandlers [HandlerNumber].Counter = 0;
	STHandlers [HandlerNumber].Active = true;
	STHandlers [HandlerNumber].fp = fp;
}

void Timer::Enable(uint8_t HandlerNumber)
{
	STHandlers [HandlerNumber].Active = true;
	STHandlers [HandlerNumber].Counter = 0;
}

void Timer::Disable(uint8_t HandlerNumber)
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

void MotorTesting()
{
	enum {Acc, Decc};
	static uint8_t state = Acc;
	static uint8_t v = 0;
	if(state == Acc)
	{
		v = v + 5;
		if(v == 100) state = Decc;
	}
	if(state == Decc)
	{
		v = v - 5;
		if(v == 0)
		{
			timer.Disable(3);
			state = Acc;
			return;
		}
	}
	motor.Enable(Forward, v);
}

ISR(TIMER0_COMPA_vect)
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

