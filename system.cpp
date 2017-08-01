/*
 * system.cpp
 *
 * Created on: 8 lip 2017
 * Author: tomek
 */

#include <avr/interrupt.h>
#include "system.h"
#include "timer.h"
#include "display.h"

StartupConfig::StartupConfig() : Machine(ST_MAX_STATES)
{
	ST_Start(&startup_config_data);
}

void StartupConfig::EV_ButtonPress(StartupConfigData* pdata)
{
    BEGIN_TRANSITION_MAP								// current state
        TRANSITION_MAP_ENTRY(ST_CHOOSING_PARAMETER)		// ST_START
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_CONFIGURATION
    END_TRANSITION_MAP(pdata)
}

void StartupConfig::ST_Start(StartupConfigData* pdata)
{
	timer.Assign(TIMER_DISPLAY_REFRESH, 4, DisplayRefresh);
	timer.Assign(TIMER_LOW_LEVEL_COUNTDOWN, 1000, LowLevelCountDown);
	sei();
}

void StartupConfig::ST_ChoosingParameter(StartupConfigData* pdata)
{
	timer.Disable(TIMER_LOW_LEVEL_COUNTDOWN);
	display.Write(Parameter, 1);
	timer.Assign(TIMER_ENCODER_STATUS, 5, EncoderStatus);
}
