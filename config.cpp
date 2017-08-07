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

void test()
{
	display.Write(4683);
}

Config::Config() : StateMachine(ST_MAX_STATES)
{
	ST_Init(&config_data);
}

void Config::CountDown(ConfigData* pdata)
{
	static uint8_t i = CONFIG_INIT_TIME;
	display.Write(i);
	if(i == 0)
	{
		timer.Disable(TIMER_INIT_COUNTDOWN);
		InternalEvent(ST_DONE);
	}
	i--;
}

void Config::SetSupportedFunctions(uint8_t number_of_functions)
{
	_number_of_functions = number_of_functions;
}

void Config::EV_EnterConfig(ConfigData* pdata)
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
			pdata->val = functions[index].param;
			encoder.SetCounter(functions[index].param);
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
	m = GetPointerTypeOfMachine(0);
	//	m = GetPointerTypeOfMachine(functions[27].param);
	m->LoadSupportedFunctions();
	sei();
	timer.Assign(TIMER_DISPLAY_REFRESH, 4, DisplayRefresh);
	timer.Assign(TIMER_INIT_COUNTDOWN, 1000, InitCountDown);
	timer.Assign(TIMER_ENCODER_POLL, 1, EncoderPoll);
	pdata->val = 0;
}

void Config::ST_ChoosingFunction(ConfigData* pdata)
{
	index = pdata->val;
	if(index == 0xFF)
	{
		index = 0;
		encoder.SetCounter(0);
	}
	if(index >= (_number_of_functions - 1))
	{
		index = _number_of_functions - 1;
		encoder.SetCounter(_number_of_functions - 1);
	}

	if(functions[index].id != 0)
	//if(_supported_functions & (1 << functions[index].id))
		display.Write(TFunction, functions[index].id);
//	else
//		display.Write(TFunctionNotSupported, functions[index].id);
}

void Config::ST_ExecutingFunction(ConfigData* pdata)
{
	if(GetTypeOfFunction(index) == 1) display.Write(TParameterValue, pdata->val);
	if(GetTypeOfFunction(index) == 2)
	{
		functions[index].f();
	}
}

void Config::ST_Done(ConfigData* pdata)
{
	timer.Disable(TIMER_BUTTON_DEBOUNCE);
	timer.Disable(TIMER_ENCODER_POLL);
//	m->StartupTest();

}

uint8_t Config::GetTypeOfFunction(uint8_t id)
{
	if(functions[index].param == 0xFF && functions[index].f == NULL) return 0;
	if(functions[index].param != 0xFF && functions[index].f == NULL) return 1;
	if(functions[index].param == 0xFF && functions[index].f != NULL) return 2;
	return 0; 	// if(functions[index].param != 0xFF && functions[index].f != NULL)
}
