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
	if(current_state == ST_INIT) timer.Disable(TIMER_INIT_COUNTDOWN);
	BEGIN_TRANSITION_MAP								// current state
        TRANSITION_MAP_ENTRY(ST_CHOOSING_FUNCTION)		// ST_INIT
        TRANSITION_MAP_ENTRY(ST_DONE)					// ST_CHOOSING_FUNCTION
        TRANSITION_MAP_ENTRY(ST_DONE)					// ST_EXECUTING_FUNCTION
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_DONE
    END_TRANSITION_MAP(pdata)
}

void Config::EV_Encoder(ConfigData* pdata)
{
    BEGIN_TRANSITION_MAP								// current state
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_INIT
    	TRANSITION_MAP_ENTRY(ST_CHOOSING_FUNCTION)		// ST_CHOOSING_FUNCTION
        TRANSITION_MAP_ENTRY(ST_EXECUTING_FUNCTION)		// ST_EXECUTING_FUNCTION
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_DONE
    END_TRANSITION_MAP(pdata)
}

void Config::EV_EncoderClick(ConfigData* pdata)
{
	if(GetTypeOfFunction(index))
	{
		static uint8_t index_cache;
		if(current_state == ST_CHOOSING_FUNCTION)
		{
			index_cache = pdata->val;
			display.Write(TParameterValue, functions[index].param);
		}
		if(current_state == ST_EXECUTING_FUNCTION)
		{
			encoder.SetCounter(index_cache);
			pdata->val = index_cache;
		}
		BEGIN_TRANSITION_MAP								// current state
        	TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_INIT
        	TRANSITION_MAP_ENTRY(ST_EXECUTING_FUNCTION)		// ST_CHOOSING_FUNCTION
			TRANSITION_MAP_ENTRY(ST_CHOOSING_FUNCTION)		// ST_EXECUTING_FUNCTION
			TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_DONE
		END_TRANSITION_MAP(pdata)
	}
}

void Config::ST_Init(ConfigData* pdata)
{
	eeprom.Read();
	sei();
	timer.Assign(TIMER_DISPLAY_REFRESH, 4, DisplayRefresh);
	timer.Assign(TIMER_INIT_COUNTDOWN, 1000, InitCountDown);
	timer.Assign(TIMER_ENCODER_POLL, 1, EncoderPoll);
	index = 0;
}

void Config::ST_ChoosingFunction(ConfigData* pdata)
{
	index = pdata->val;
	if(!GetTypeOfFunction(index)) display.Write(TFunctionNotSupported, index + 1);
	else
		display.Write(TFunction, index + 1);
}

void Config::ST_ExecutingFunction(ConfigData* pdata)
{
//	display.Write(1234);
}

void Config::ST_Done(ConfigData* pdata)
{
	display.Write(0);
}

uint8_t Config::GetTypeOfFunction(uint8_t id)
{
	if(functions[index].param == 0xFF && functions[index].f == NULL) return 0;
	if(functions[index].param != 0xFF && functions[index].f == NULL) return 1;
	if(functions[index].param == 0xFF && functions[index].f != NULL) return 2;
	return 0; 	// if(functions[index].param != 0xFF && functions[index].f != NULL)
}
