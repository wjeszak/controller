/*
 * system.cpp
 *
 * Created on: 8 lip 2017
 * Author: tomek
 */

#include <avr/interrupt.h>
#include "system.h"
#include "timer.h"

StartupConfig::StartupConfig() : Machine(ST_MAX_STATES)
{
	ST_Start(&startup_config_data);
}

void StartupConfig::ST_Start(StartupConfigData* pdata)
{
	timer.Assign(TIMER_DISPLAY_REFRESH, 4, DisplayRefresh);
	timer.Assign(TIMER_LOW_LEVEL_COUNTDOWN, 1000, LowLevelCountDown);
	sei();
}

void StartupConfig::EV_ButtonPress(StartupConfigData* pdata)
{
//	RxEnable();
}




