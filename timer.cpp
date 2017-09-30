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
#include "button.h"
#include "comm_prot.h"
#include "config.h"
#include "display.h"
#include "usart.h"
#include "motor.h"
#include "encoder.h"
#include "machine.h"
#include "modbus_tcp.h"

Timer::Timer(T2Prescallers prescaller)
{
	for(uint8_t n = 0; n < NUMBER_OF_TIMERS; n++)
	{
		timer_handlers[n].active = false;
		timer_handlers[n].counter = 0;
		timer_handlers[n].interval = 0;
		timer_handlers[n].fp = NULL;
	}
	TCCR2A |= (1 << WGM21) | (1 << WGM20);
	TCCR2B |= prescaller;
	// for main system timer
	OCR2B = 255;
	TIMSK2 |= (1 << OCIE2B);
	main_timer_prescaler = 0;
}

void Timer::Irq()
{
	main_timer_prescaler++;
	if(main_timer_prescaler == MAIN_TIMER_PRESCALER)
	{
		main_timer_prescaler = 0;
		for(uint8_t n = 0; n < NUMBER_OF_TIMERS; n++)
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
}

void Timer::Assign(uint8_t handler_id, uint16_t interval, void(*fp)())
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

// ----------------------------------------------------------------------------------
// TIMER_DISPLAY_REFRESH
void DisplayRefresh()
{
	display.Refresh();
}

// TIMER_INIT_COUNTDOWN
void InitCountDown()
{
	config.CountDown(&config_data);
}

// TIMER_FAULTS_HANDLING
void FaultsHandling()
{

}

// TIMER_BUTTON_POLL
void ButtonPoll()
{
	if(button_enter_config.Pressed())
		button_enter_config.EV_Pressed(&button_data);
	else
		button_enter_config.EV_Released(&button_data);

	if(button_encoder_sw.Pressed())
		button_encoder_sw.EV_Pressed(&button_data);
	else
		button_encoder_sw.EV_Released(&button_data);
}

//TIMER_ENCODER_POLL
void EncoderPoll()
{
	encoder.Poll();
}

void SlavesPollGeneral()
{
	m->SlavesPoll();
}

void ReplyTimeoutGeneral()
{
	SLAVES_POLL_TIMEOUT_OFF;
	m->ReplyTimeout();
}

// TIMER_MOTOR_ACCELERATE
void MotorAccelerate()
{
	motor.Accelerate();
}

// TIMER_SHOW_FAULT
void ShowFault()
{
	static uint8_t i = 1;
	if(d->faults == 0) { display.Write(TNoFault, 0); return; }
	while(i <= 18)
	{
		if(i == 18) i = 1;
		if(d->faults & (1ULL << i))
		{
			display.Write(TFault, i);
			i++;
			break;
		}
		i++;
	}
}

ISR(TIMER2_COMPB_vect)
{
	timer.Irq();
}
