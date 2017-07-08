/*
 * system.cpp
 *
 * Created on: 8 lip 2017
 * Author: tomek
 */

#include <avr/interrupt.h>
#include "timer.h"

void SystemInit()
{
	timer.Assign(TIMER_DISPLAY_REFRESH, 4, DisplayRefresh);
	sei();
}




