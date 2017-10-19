/*
 * config.cpp
 *
 * Created on: 8 lip 2017
 * Author: tomek
 */

#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "config.h"
#include "machine.h"
#include "timer.h"
#include "display.h"
#include "dynabox.h"

Function functions[MAX_NUMBER_OF_FUNCTIONS];

void test()
{
	display.Write(4683);
}

Config::Config() : StateMachine(ST_MAX_STATES)
{
	need_stack_poll = true;
}

void Config::EV_ButtonClick(ConfigData* pdata)
{
	m->EV_EnterToConfig();
	if(current_state == ST_CHOOSING_FUNCTION)
	{
		// exit from configuration
		m->SaveParameters();
		timer.Assign(TIMER_SHOW_FAULT, 1000, ShowFault);
	}
	BEGIN_TRANSITION_MAP								// current state
        TRANSITION_MAP_ENTRY(ST_CHOOSING_FUNCTION)		// ST_INIT
        TRANSITION_MAP_ENTRY(ST_DONE)					// ST_CHOOSING_FUNCTION
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_EXECUTING_FUNCTION
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
	if(functions[index].id != 0)
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
			timer.Assign(TIMER_ENCODER_POLL, 1, EncoderPoll);
		}
		BEGIN_TRANSITION_MAP								// current state
        	TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_INIT
        	TRANSITION_MAP_ENTRY(ST_EXECUTING_FUNCTION)		// ST_CHOOSING_FUNCTION
			TRANSITION_MAP_ENTRY(ST_CHOOSING_FUNCTION)		// ST_EXECUTING_FUNCTION
			TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_DONE
		END_TRANSITION_MAP(pdata)
	}
}

void Config::ST_Idle(ConfigData* pdata)
{

}

void Config::ST_ChoosingFunction(ConfigData* pdata)
{
	index = pdata->val;
	if(index == 0xFF)
	{
		index = 0;
		encoder.SetCounter(0);
	}
	if(index >= (number_of_functions - 1))
	{
		index = number_of_functions - 1;
		encoder.SetCounter(number_of_functions - 1);
	}
	if(functions[index].id != 0)
		display.Write(TFunction, functions[index].id);
}

void Config::ST_ExecutingFunction(ConfigData* pdata)
{
	if(functions[index].f)
	{
		timer.Disable(TIMER_ENCODER_POLL);
		functions[index].f();
	}
	else
	{
		display.Write(TParameterValue, pdata->val);
		functions[index].param = pdata->val;
	}
}

void Config::ST_Done(ConfigData* pdata)
{
//	timer.Disable(TIMER_BUTTON_POLL);
//	timer.Disable(TIMER_ENCODER_POLL);
	//timer.Assign(TIMER_SHOW_FAULT, 1000, ShowFault);
	// odpalenie maszyny
	//m->InternalEvent(ST_INIT, NULL);
}
