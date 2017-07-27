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
        TRANSITION_MAP_ENTRY(ST_BUTTON_DEBOUNCE)		// ST_START
        TRANSITION_MAP_ENTRY(ST_BUTTON_DOWN)			// ST_BUTTON_DEBOUNCE
        TRANSITION_MAP_ENTRY(ST_CONFIGURATION)			// ST_BUTTON_DOWN
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_CONFIGURATION, to bedzie wyjscie z konfiguracji
    END_TRANSITION_MAP(pdata)
}

void StartupConfig::ST_Start(StartupConfigData* pdata)
{
	BUTTON_DDR &= ~(1 << BUTTON_PIN_NUMBER);
	BUTTON_PORT |= (1 << BUTTON_PIN_NUMBER);
	timer.Assign(TIMER_DISPLAY_REFRESH, 4, DisplayRefresh);
	timer.Assign(TIMER_LOW_LEVEL_COUNTDOWN, 1000, LowLevelCountDown);
	timer.Assign(TIMER_BUTTON_POLL, 100, ButtonPoll);
	sei();
}

void StartupConfig::ST_ButtonDebounce(StartupConfigData* pdata)
{

}

void StartupConfig::ST_ButtonDown(StartupConfigData* pdata)
{
	timer.Disable(TIMER_LOW_LEVEL_COUNTDOWN);
	timer.Disable(TIMER_BUTTON_POLL);
	timer.Assign(TIMER_BUTTON_TIME_TO_ENTER, 2000, EnterConfig);
}

void StartupConfig::ST_Configuration(StartupConfigData* pdata)
{
	display.Write(Parameter, 45);
}
