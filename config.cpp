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
	index = 1;
	eeprom.Read();
	ST_Idle(&config_data);
}

void Config::EV_ButtonPress(ConfigData* pdata)
{
    BEGIN_TRANSITION_MAP								// current state
        TRANSITION_MAP_ENTRY(ST_CHOOSING_PARAMETER)		// ST_IDLE
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_CHOOSING_PARAMETER
    END_TRANSITION_MAP(pdata)
}

void Config::EV_EncoderLeft()
{
	index--;
	display.Write(Parameter, index);
}

void Config::EV_EncoderRight()
{
	index++;
	display.Write(Parameter, index);
}

void Config::ST_Idle(ConfigData* pdata)
{

}

void Config::ST_ChoosingParameter(ConfigData* pdata)
{
	timer.Disable(TIMER_INIT_COUNTDOWN);
	timer.Assign(TIMER_ENCODER_POLL, 5, EncoderPoll);
	display.Write(Parameter, index);
}
