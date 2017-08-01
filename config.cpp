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

Config::Config() : Machine(ST_MAX_STATES)
{
	ST_Start(&config_data);
}

void Config::EV_ButtonPress(ConfigData* pdata)
{
    BEGIN_TRANSITION_MAP								// current state
        TRANSITION_MAP_ENTRY(ST_CHOOSING_PARAMETER)		// ST_START
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_CHOOSING_PARAMETER
    END_TRANSITION_MAP(pdata)
}

void Config::ST_Start(ConfigData* pdata)
{

}

void Config::ST_ChoosingParameter(ConfigData* pdata)
{
	timer.Disable(TIMER_LOW_LEVEL_COUNTDOWN);
	display.Write(Parameter, 1);
	timer.Assign(TIMER_ENCODER_STATUS, 5, EncoderStatus);
}
