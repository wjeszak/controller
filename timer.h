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

#define MAIN_TIMER_PRESCALER 				36
#define NUMBER_OF_TIMERS 					14

#define TIMER_DISPLAY_REFRESH 				0
#define TIMER_BUTTON_POLL					1
#define TIMER_ENCODER_POLL			 		2
#define TIMER_SLAVE_POLL 					3
#define TIMER_SLAVE_TIMEOUT 				4
#define TIMER_MOTOR_ACCELERATE 				5
#define TIMER_LED_TRIGGER 		 			6
#define TIMER_MOTOR_DECELERATE 				7
#define TIMER_MOTOR_SPEED_MEAS 				8
#define TIMER_BEFORE_DIRECTION_CHANGE		9
#define TIMER_DOOR_OPEN_TIMEOUT 			10
#define TIMER_DEBUG							11
#define TIMER_TMP							12
#define TIMER_TMP1 							13

#define TIMER_TMP_INTERVAL 					5000
#define TIMER_TMP1_INTERVAL					10000

#define SLAVE_POLL_INTERVAL 				50
#define FAULT_SHOW_INTERVAL 				1000
#define FAULT_SHOW_TICK	 					FAULT_SHOW_INTERVAL / SLAVE_POLL_INTERVAL
#define DOOR_OPEN_TIMEOUT_TICK				FAULT_SHOW_INTERVAL / SLAVE_POLL_INTERVAL
#define SLAVE_TIMEOUT_INTERVAL 				20
#define MOTOR_SPEED_MEAS_INTERVAL 			40
#define BEFORE_DIRECTION_CHANGE_INTERVAL 	1000
#define DOOR_OPEN_TIMEOUT_INTERVAL 			1000
#define DEBUG_INTERVAL						100

#define SLAVE_POLL_START 				timer.Assign(TIMER_SLAVE_POLL, SLAVE_POLL_INTERVAL, SlavePollGeneral)
#define SLAVE_POLL_STOP					timer.Disable(TIMER_SLAVE_POLL);
#define SLAVE_POLL_TIMEOUT_SET			timer.Assign(TIMER_SLAVE_TIMEOUT, SLAVE_TIMEOUT_INTERVAL, SlaveTimeoutGeneral);
#define SLAVE_POLL_TIMEOUT_OFF			timer.Disable(TIMER_SLAVE_TIMEOUT);

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
// --------------------------------------------------------------------
extern void DisplayRefresh();
extern void ButtonPoll();
extern void EncoderPoll();
extern void SlavePollGeneral();
extern void SlaveTimeoutGeneral();
extern void MotorAccelerate();
extern void MotorDecelerate();
extern void LedTrigger();
extern void MotorSpeedMeas();
extern void BeforeDirectionChange();
//extern void DoorOpenTimeout();
extern void Debug();
extern void Tmp();
extern void Tmp1();

#endif /* TIMER_H_ */
