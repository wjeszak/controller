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
        TRANSITION_MAP_ENTRY(ST_CHOOSING_FUNCTION)		// ST_INIT
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
//	timer.Assign(TIMER_ENCODER_POLL, 1, EncoderPoll);
}

void Config::ST_ChoosingFunction(ConfigData* pdata)
{
	timer.Disable(TIMER_INIT_COUNTDOWN);
	if(!GetTypeOfFunction(index)) display.Write(TFunctionNotSupported, index);
	else
		display.Write(TFunction, index);
}

void Config::ST_ExecutingFunction(ConfigData* pdata)
{

}

uint8_t Config::GetTypeOfFunction(uint8_t id)
{
	if(functions[index].param == 0xFF && functions[index].f == NULL) return 0;
	if(functions[index].param != 0xFF && functions[index].f == NULL) return 1;
	if(functions[index].param == 0xFF && functions[index].f != NULL) return 2;
	return 0; 	// if(functions[index].param != 0xFF && functions[index].f != NULL)
}
