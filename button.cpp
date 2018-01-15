/*
 * button.cpp
 *
 *  Created on: 1 sie 2017
 *      Author: tomek
 */

#include <avr/io.h>
#include "button.h"
#include "timer.h"
//#include "display.h"

Button::Button(volatile uint8_t *ddr, volatile uint8_t *port, volatile uint8_t *pin, uint8_t pin_number, uint16_t time_to_action, void (Config::*f)(ConfigData*)) : StateMachine(ST_MAX_STATES)
{
	_ddr = ddr;
	_port = port;
	_pin = pin;
	_pin_number = pin_number;
	_time_to_action = time_to_action;
	_f = f;
	_time_to_action_tmp = 0;
	*_ddr &= ~(1 << _pin_number);
	*_port |= (1 << _pin_number);

	timer.Assign(TButtonPoll, 100, ButtonPoll);
	ST_Idle(&button_data);
}

bool Button::Pressed()
{
	if(!(*_pin & (1 << _pin_number))) return true;
	return false;
}

void Button::EV_Pressed(ButtonData* pdata)
{
    BEGIN_TRANSITION_MAP								// current state
        TRANSITION_MAP_ENTRY(ST_DEBOUNCE)				// ST_IDLE
    	TRANSITION_MAP_ENTRY(ST_ACTION)					// ST_DEBOUNCE
        TRANSITION_MAP_ENTRY(ST_ACTION)					// ST_ACTION
    END_TRANSITION_MAP(pdata)
}

void Button::EV_Released(ButtonData* pdata)
{
	if(_time_to_action_tmp == _time_to_action)
	{
		(config.*_f)(&config_data);
		InternalEvent(ST_IDLE, NULL);
		_time_to_action_tmp = 0;
	}
}

void Button::ST_Idle(ButtonData* pdata)
{

}

void Button::ST_Debounce(ButtonData* pdata)
{

}

void Button::ST_Action(ButtonData* pdata)
{
	if(!(_time_to_action_tmp == _time_to_action)) _time_to_action_tmp++;
}

