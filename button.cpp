/*
 * button.cpp
 *
 *  Created on: 1 sie 2017
 *      Author: tomek
 */

#include <avr/io.h>
#include "timer.h"
#include "display.h"
#include "button.h"

Button::Button(volatile uint8_t *ddr, volatile uint8_t *port, volatile uint8_t *pin, uint8_t pin_number, uint64_t time_to_action, void (Config::*f)(ConfigData*)) : Machine(ST_MAX_STATES)
{
	_ddr = ddr;
	_port = port;
	_pin = pin;
	_pin_number = pin_number;
	_time_to_action = time_to_action;
	_f = f;

	*_ddr &= ~(1 << _pin_number);
	*_port |= (1 << _pin_number);

	ST_Idle(&button_data);
}

bool Button::CheckVal()
{
	if(!(*_pin & (1 << _pin_number))) return true;
	return false;
}

void Button::EV_Press(ButtonData* pdata)
{
    BEGIN_TRANSITION_MAP								// current state
        TRANSITION_MAP_ENTRY(ST_DEBOUNCE)				// ST_IDLE
        TRANSITION_MAP_ENTRY(ST_DOWN)					// ST_DEBOUNCE
        TRANSITION_MAP_ENTRY(ST_ACTION)					// ST_DOWN
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)			// ST_ACTION
    END_TRANSITION_MAP(pdata)
}

void Button::ST_Idle(ButtonData* pdata)
{
	timer.Assign(TIMER_BUTTON_DEBOUNCE, 100, ButtonDebounce);
}

void Button::ST_Debounce(ButtonData* pdata)
{

}

void Button::ST_Down(ButtonData* pdata)
{
	timer.Disable(TIMER_BUTTON_DEBOUNCE);
	timer.Assign(TIMER_BUTTON_ACTION, _time_to_action, ButtonAction);
}

void Button::ST_Action(ButtonData* pdata)
{
	timer.Disable(TIMER_BUTTON_ACTION);

	(config.*_f)(NULL);
	//	config.EV_ButtonPress(&config_data);
	InternalEvent(ST_IDLE, NULL);
}
