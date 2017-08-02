/*
 * timer.h
 *
 * Created on: 15 cze 2017
 * Author: wjeszak
 *
 * PWM & Main system timer
 * PWMfreq = F_CPU / preskaler / 256	(DT p. 196)
 * PWMfreq = 9216000 / 1 / 256 = 36 kHz
 * tick time [ms] = 1 / PWMfreq * MAIN_TIMER_PRESCALER * 1000
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <avr/io.h>
#include "machine.h"

#define MAIN_TIMER_PRESCALER 			36
#define NUMBER_OF_TIMERS 				10

#define TIMER_DISPLAY_REFRESH 			0
#define TIMER_LOW_LEVEL_COUNTDOWN 		1
#define TIMER_BUTTON_DEBOUNCE			2
#define TIMER_BUTTON_ACTION		 		3
#define TIMER_ENCODER_POLL	 			4
#define TIMER_MODBUS_RTU_35T			5
#define TIMER_MODBUS_RTU_POLL 			6
#define TIMER_MOTOR_ACCELERATE 			7

enum T2Prescallers
{
	T2_PS_0    = 0,
	T2_PS_1    = (1 << CS20),
	T2_PS_8    = (1 << CS21),
	T2_PS_32   = (1 << CS21) | (1 << CS20),
	T2_PS_64   = (1 << CS22),
	T2_PS_128  = (1 << CS22) | (1 << CS20),
	T2_PS_256  = (1 << CS22) | (1 << CS21),
	T2_PS_1024 = (1 << CS22) | (1 << CS21) | (1 << CS20)
};

class Timer
{
public:
	Timer(T2Prescallers prescaller);
	void Irq();
	void Assign(uint8_t handler_id, uint64_t interval, void(*fp)());
	void Enable (uint8_t handler_id);
	void Disable (uint8_t handler_id);
private:
	struct TimerHandler
	{
		void(*fp)();
		bool active;
		uint64_t interval;
		uint64_t counter;
	};
	TimerHandler timer_handlers[NUMBER_OF_TIMERS];
	uint16_t main_timer_prescaler;
};

extern Timer timer;

extern void DisplayRefresh();
extern void LowLevelCountDown();
extern void ButtonDebounce();
extern void ButtonAction();
extern void EncoderPoll();
extern void ModbusRTU35T();
extern void ModbusPoll();
extern void MotorAccelerate();

#endif /* TIMER_H_ */
