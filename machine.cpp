/*
 * maszyna.cpp
 *
 *  Created on: 3 cze 2017
 *      Author: tomek
 */

#include "machine.h"

Machine::Machine() : current_state(0)
{
	GetState();
}

void Machine::ChangeState(uint16_t new_state)
{
	current_state = new_state;
}

uint16_t Machine::GetState()
{
	return current_state;
}
void Machine::Event(uint16_t new_state, EventData *pdata)
{
	_event_generated = true;
	ChangeState(new_state);
	while(_event_generated)
	{
		(this->*pfun[current_state])(pdata);
		_event_generated = false;
	}
}

void Machine::InternalEvent(uint16_t new_state, EventData *pdata)
{
	ChangeState(new_state);
	(this->*pfun[current_state])(pdata);
}
