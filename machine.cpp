/*
 * maszyna.cpp
 *
 *  Created on: 3 cze 2017
 *      Author: tomek
 */

#include "machine.h"

Machine::Machine(uint8_t max_states) : current_state(0), _max_states(max_states), _event_generated(false)
{
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
	if(new_state == ST_NOT_ALLOWED) {}
	else
		InternalEvent(new_state, pdata);
}

void Machine::InternalEvent(uint16_t new_state, EventData* pdata)
{
	_event_generated = true;
	ChangeState(new_state);
	while(_event_generated)
	{
		const StateStruct* pStateMap = GetStateMap();
		// run state function
		(this->*pStateMap[current_state].pStateFunc)(pdata);
		_event_generated = false;
	}
}
