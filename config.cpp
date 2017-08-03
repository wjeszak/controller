/*
 * config.cpp
 *
 * Created on: 8 lip 2017
 * Author: tomek
 */

#include <avr/interrupt.h>
#include "config.h"
#include "timer.h"
#include "display.h"
#include "eeprom.h"

Config::Config() : Machine(ST_MAX_STATES)
{
	ST_Init(&config_data);
}

void Config::EV_ButtonPress(ConfigData* pdata)
{
    BEGIN_TRANSITION_MAP								// current state
        TRANSITION_MAP_ENTRY(ST_IDLE)					// ST_INIT
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_IDLE
    END_TRANSITION_MAP(pdata)
}

void Config::EV_EncoderLeft(ConfigData* pdata)
{
	index--;
    BEGIN_TRANSITION_MAP								// current state
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_INIT
    	TRANSITION_MAP_ENTRY(ST_CHOOSING_FUNCTION)		// ST_IDLE
        TRANSITION_MAP_ENTRY(ST_CHOOSING_FUNCTION)		// ST_CHOOSING_FUNCTION
    END_TRANSITION_MAP(pdata)
}

void Config::EV_EncoderRight(ConfigData* pdata)
{
	index++;
    BEGIN_TRANSITION_MAP								// current state
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_INIT
    	TRANSITION_MAP_ENTRY(ST_CHOOSING_FUNCTION)		// ST_IDLE
        TRANSITION_MAP_ENTRY(ST_CHOOSING_FUNCTION)		// ST_CHOOSING_FUNCTION
    END_TRANSITION_MAP(pdata)
}

void Config::EV_EncoderClick(ConfigData* pdata)
{
    BEGIN_TRANSITION_MAP								// current state
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_INIT
        TRANSITION_MAP_ENTRY(ST_EXECUTING_FUNCTION)		// ST_IDLE
        TRANSITION_MAP_ENTRY(ST_EXECUTING_FUNCTION)		// ST_CHOOSING_FUNCTION
    END_TRANSITION_MAP(pdata)
}

void Config::ST_Init(ConfigData* pdata)
{
	index = 1;
	eeprom.Read();
	timer.Assign(TIMER_DISPLAY_REFRESH, 4, DisplayRefresh);
	timer.Assign(TIMER_INIT_COUNTDOWN, 1000, InitCountDown);
	sei();
}

void Config::ST_Idle(ConfigData* pdata)
{
	timer.Disable(TIMER_INIT_COUNTDOWN);
	timer.Assign(TIMER_ENCODER_POLL, 1, EncoderPoll);
}

void Config::ST_ChoosingFunction(ConfigData* pdata)
{
	if(functions[index].f == NULL && functions[index].param == 0xFF) display.Write(ParameterNotSupported, index);
	else
		display.Write(Parameter, index);
}

void Config::ST_ExecutingFunction(ConfigData* pdata)
{

}
