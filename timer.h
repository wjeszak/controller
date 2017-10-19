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

#define MAIN_TIMER_PRESCALER 			36
#define NUMBER_OF_TIMERS 				10

#define TIMER_DISPLAY_REFRESH 			0
#define TIMER_INIT_COUNTDOWN 			1
#define TIMER_BUTTON_POLL				2
#define TIMER_ENCODER_POLL			 	3
#define TIMER_SLAVES_POLL 				4
#define TIMER_REPLY_TIMEOUT 			5
#define TIMER_MOTOR_ACCELERATE 			6
#define TIMER_SHOW_FAULT 				7

#define SLAVES_POLL_INTERVAL 			1000
#define SLAVES_REPLY_TIMEOUT 			500

#define SLAVES_POLL_START 				timer.Assign(TIMER_SLAVES_POLL, SLAVES_POLL_INTERVAL, SlavesPollGeneral)
#define SLAVES_POLL_STOP				timer.Disable(TIMER_SLAVES_POLL);
#define SLAVES_POLL_TIMEOUT_SET			timer.Assign(TIMER_REPLY_TIMEOUT, SLAVES_REPLY_TIMEOUT, ReplyTimeoutGeneral);
#define SLAVES_POLL_TIMEOUT_OFF			timer.Disable(TIMER_REPLY_TIMEOUT);
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
	void Assign(uint8_t handler_id, uint16_t interval, void(*fp)());
	void Enable (uint8_t handler_id);
	void Disable (uint8_t handler_id);
private:
	struct TimerHandler
	{
		void(*fp)();
		bool active;
		uint16_t interval;
		uint16_t counter;
	};
	TimerHandler timer_handlers[NUMBER_OF_TIMERS];
	uint8_t main_timer_prescaler;
};

extern Timer timer;

extern void DisplayRefresh();
extern void ButtonPoll();
extern void EncoderPoll();
extern void SlavesPoll();
extern void ReplyTimeoutGeneral();
extern void SlavesPollGeneral();
extern void MotorAccelerate();
extern void ShowFault();

#endif /* TIMER_H_ */
