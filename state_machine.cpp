/*
 * machine.cpp
 *
 *  Created on: 3 cze 2017
 *      Author: tomek
 */

#include "state_machine.h"

StateMachine::StateMachine(uint8_t max_states) : current_state(0), _max_states(max_states), _event_generated(false)
{
}

void StateMachine::ChangeState(uint8_t new_state)
{
	current_state = new_state;
}

uint8_t StateMachine::GetState()
{
	return current_state;
}
void StateMachine::Event(uint8_t new_state, EventData *pdata)
{
	if(new_state == ST_NOT_ALLOWED) {}
	else
		InternalEvent(new_state, pdata);
}

void StateMachine::InternalEvent(uint8_t new_state, EventData* pdata)
{
	_event_generated = true;
	ChangeState(new_state);
	while(_event_generated)
	{
		const StateStruct* pStateMap = GetStateMap();
		// call state function
		(this->*pStateMap[current_state].pStateFunc)(pdata);
		_event_generated = false;
	}
}
