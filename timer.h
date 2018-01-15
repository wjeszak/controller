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

enum TimerId
{
	TDisplayRefresh, TFaultShow, TButtonPoll, TEncoderPoll, TSlavePoll, TSlaveTimeout, TMotorAccelerate,
	TLedTrigger, TMotorDecelerate, TMotorSpeedMeas, TBeforeDirectionChange, TDoorOpenTimeout,
	TDebug, TTmp, TTmp1, TNumberOfTimers
};

#define MAIN_TIMER_PRESCALER 				36

#define TIMER_TMP_INTERVAL 					5000
#define TIMER_TMP1_INTERVAL					10000

#define DISPLAY_REFRESH_INTERVAL 			4
#define ENCODER_POLL_INTERVAL 				1
#define SLAVE_POLL_INTERVAL 				50
#define FAULT_SHOW_INTERVAL 				1000
#define DOOR_OPEN_TIMEOUT_INTERVAL 			1000
#define FAULT_SHOW_TICK	 					FAULT_SHOW_INTERVAL / SLAVE_POLL_INTERVAL
#define DOOR_OPEN_TIMEOUT_TICK				DOOR_OPEN_TIMEOUT_INTERVAL / SLAVE_POLL_INTERVAL
#define SLAVE_TIMEOUT_INTERVAL 				15
#define MOTOR_SPEED_MEAS_INTERVAL 			40
#define BEFORE_DIRECTION_CHANGE_INTERVAL 	1000

#define DEBUG_INTERVAL						100
#define FAULT_SHOW_START 					timer.Assign(TFaultShow, FAULT_SHOW_INTERVAL, FaultShow)
#define FAULT_SHOW_STOP 					timer.Disable(TFaultShow)
#define SLAVE_POLL_START 					timer.Assign(TSlavePoll, SLAVE_POLL_INTERVAL, SlavePollGeneral)
#define SLAVE_POLL_STOP						timer.Disable(TSlavePoll);
#define SLAVE_POLL_TIMEOUT_SET				timer.Assign(TSlaveTimeout, SLAVE_TIMEOUT_INTERVAL, SlaveTimeoutGeneral);
#define SLAVE_POLL_TIMEOUT_OFF				timer.Disable(TSlaveTimeout);

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
	void Assign(TimerId handler_id, uint16_t interval, void(*fp)());
	void Enable (TimerId handler_id);
	void Disable (TimerId handler_id);
private:
	struct TimerHandler
	{
		void(*fp)();
		bool active;
		uint16_t interval;
		uint16_t counter;
	};
	TimerHandler timer_handlers[TNumberOfTimers];
	uint8_t main_timer_prescaler;
};

extern Timer timer;
// --------------------------------------------------------------------
extern void DisplayRefresh();
extern void FaultShow();
extern void ButtonPoll();
extern void EncoderPoll();
extern void SlavePollGeneral();
extern void SlaveTimeoutGeneral();
extern void MotorAccelerate();
extern void MotorDecelerate();
extern void LedTrigger();
extern void MotorSpeedMeas();
extern void BeforeDirectionChange();
extern void Debug();
extern void Tmp();
extern void Tmp1();

#endif /* TIMER_H_ */
