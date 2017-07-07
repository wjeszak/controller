/*
 * timery.h
 *
 *  Created on: 15 cze 2017
 *      Author: tomek
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <avr/io.h>
#include "machine.h"

#define TIMER_DISPLAY_REFRESH 			0
#define TIMER_MODBUS_RTU_35T			1

enum T2Prescallers
{
	T2_PS_0    = 0,
	T2_PS_1    = (1 << CS20),
	T2_PS_8    = (1 << CS21),
	T2_PS_32   = (1 <<CS21) | (1 << CS20),
	T2_PS_64   = (1 << CS22),
	T2_PS_128  = (1 << CS22) | (1 << CS20),
	T2_PS_256  = (1 << CS22) | (1 << CS21),
	T2_PS_1024 = (1 << CS22) | (1 << CS21) | (1 << CS20)
};

struct TimerHandler
{
	void(*fp)();
	bool active;
	uint64_t interval;
	uint64_t counter;
};

class Timer
{
public:
	Timer(T2Prescallers Prescaller, uint8_t Tick);
	void Assign(uint8_t HandlerNumber, uint64_t Interval, void(*fp)());
	void UnAssign (uint8_t HandlerNumber);
	void Enable (uint8_t HandlerNumber);
	void Disable (uint8_t HandlerNumber);
	void Hold (uint8_t HandlerNumber);
	void UnHold (uint8_t HandlerNumber);
	bool Enabled (uint8_t HandlerNumber);
	void SetInterval (uint8_t HandlerNumber, uint64_t Interval);
	uint64_t GetInterval (uint8_t HandlerNumber);
	void SetPrescaller (T2Prescallers Prescaller);
	void SetPeriod (uint8_t Tick);
};

extern void DisplayRefresh();
extern void ModbusRTU35T();
extern void ModbusPoll();
extern void MotorTesting();
extern void EncoderStatus();
extern void MotorChangeState();

extern Timer timer;

class Timer0
{
public:
	Timer0();
	volatile uint8_t cnt;
};

extern Timer0 timer0;

class Timer1
{
public:
	Timer1();
	volatile uint8_t cnt;
};

extern Timer1 timer1;

void Irq_Init();
#endif /* TIMER_H_ */
