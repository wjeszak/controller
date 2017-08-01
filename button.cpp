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

#include "config.h"

Button::Button() : Machine(ST_MAX_STATES)
{
	Init();
	ST_Idle(&button_data);
}

bool Button::CheckVal()
{
	if(!(BUTTON_PIN & (1 << BUTTON_PIN_NUMBER))) return true;
	return false;
}

void Button::Init()
{
	BUTTON_DDR &= ~(1 << BUTTON_PIN_NUMBER);
	BUTTON_PORT |= (1 << BUTTON_PIN_NUMBER);
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
	timer.Assign(TIMER_BUTTON_ACTION, 2000, ButtonAction);
}

void Button::ST_Action(ButtonData* pdata)
{
	config.EV_ButtonPress(&config_data);
	InternalEvent(ST_IDLE, NULL);
}
