/*
 * timery.h
 *
 *  Created on: 15 cze 2017
 *      Author: tomek
 */

#ifndef TIMER_H_
#define TIMER_H_
#include "machine.h"
/*
#if (F_CPU == 1843200) | (F_CPU == 3686400) | (F_CPU == 4915200) | (F_CPU == 7372800) | \
	(F_CPU == 9216000) | (F_CPU == 11059200) | (F_CPU == 14745600) | (F_CPU == 18432000) | (F_CPU == 22118400)
#define TIMER0_OCR_VAL = (F_CPU / 1024 / 500) - 1 	// = 17
#else
#error "Nieobslugiwana czestotliwosc rezonatora kwarcowego"
#endif
*/
//#define TIMER0_OCR_VAL = (F_CPU / 1024 / 500) - 1 	// = 17 (2 ms)
/*
enum T0Prescallers
{
	T0_PS_0 = 0,
	T0_PS_1 = (1 << CS00),
	T0_PS_8 = (1 << CS01),
	T0_PS_64 = (1 << CS00) | (1 << CS01),
	T0_PS_256 = (1 << CS02),
	T0_PS_1024 = (1 << CS00) | (1 << CS02)
};
*/
enum T2Prescallers
{
	T2_PS_0 = 0,
	T2_PS_1 = (1 << CS20),
	T2_PS_8 = (1 << CS21),
	T2_PS_32 = (1 <<CS21) | (1 << CS20),
	T2_PS_64 = (1 << CS22),
	T2_PS_128 = (1 << CS22) | (1 << CS20),
	T2_PS_256 = (1 << CS22) | (1 << CS21),
	T2_PS_1024 = (1 << CS22) | (1 << CS21) | (1 << CS20)
};

struct TimerHandler
{
	void(*fp)();
	bool Active;
	uint64_t Interval;
	uint64_t Counter;
};

class Timer2
{
public:
	Timer2(T2Prescallers Prescaller, uint8_t Tick);
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

extern Timer2 timer2;

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
