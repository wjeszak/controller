/*
 * timer.cpp
 *
 *  Created on: 15 cze 2017
 *      Author: tomek
 */

#include <avr/interrupt.h>
#include "timer.h"
#include "button.h"
#include "comm.h"
#include "config.h"
#include "display.h"
#include "motor.h"
#include "encoder.h"
#include "machine.h"
#include "dynabox.h"
#include "modbus_tcp.h"

Timer::Timer(T2Prescallers prescaller)
{
	for(uint8_t n = 0; n < TNumberOfTimers; n++)
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
		for(uint8_t n = 0; n < TNumberOfTimers; n++)
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

void Timer::Assign(TimerId handler_id, uint16_t interval, void(*fp)())
{
	timer_handlers[handler_id].interval = interval;
	timer_handlers[handler_id].counter = 0;
	timer_handlers[handler_id].active = true;
	timer_handlers[handler_id].fp = fp;
}

void Timer::Enable(TimerId handler_id)
{
	timer_handlers[handler_id].active = true;
	timer_handlers[handler_id].counter = 0;
}

void Timer::Disable(TimerId handler_id)
{
	timer_handlers[handler_id].active = false;
}

ISR(TIMER2_COMPB_vect)
{
	timer.Irq();
}
// ----------------------------------------------------------------------------------

void DisplayRefresh()
{
	display.Refresh();
}

void FaultShow()
{
	fault.Show();
}

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

void EncoderPoll()
{
	encoder.Poll();
}

void SlavePollGeneral()
{
	m->StateManager();
}

void SlaveTimeoutGeneral()
{
	comm.EV_Timeout();
}

void MotorAccelerate()
{
	motor.Accelerate();
}

void MotorDecelerate()
{
	motor.Decelerate();
}

void LedTrigger()
{
	timer.Disable(TLedTrigger);
	comm.EV_Send(LED_ADDRESS_TRIGGER, 0x00, false);
}

void MotorSpeedMeas()
{
	motor.SpeedMeasure();
}

void BeforeDirectionChange()
{
	timer.Disable(TBeforeDirectionChange);
	motor.SetDirection(motor.Backward);
// --------------------- ! UWAGA NA WARTOSC PWM ! ----------------------------------
// motor_data.max_pwm_val = _minimum_pwm_val_backward + 1
// --------------------- ! UWAGA NA WARTOSC PWM ! ----------------------------------
	motor_data.pos = 0;
	//motor_data.max_pwm_val = 33;
	OCR2A = 33;
	MOTOR_START;
	motor.EV_MinPwmAchieved(&motor_data);
}

void Debug()
{
	motor.Debug();
}

void Tmp()
{
	timer.Disable(TTmp);
	dynabox.EV_HomingDone(&dynabox_data);
}

void Tmp1()
{
	timer.Disable(TTmp1);
	dynabox.EV_PositionAchieved(&dynabox_data);
}
