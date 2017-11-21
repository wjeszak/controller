/*
 * state_machine_ex.cpp
 *
 *  Created on: 20 lis 2017
 *      Author: tomek
 */

#include "state_machine_ex.h"

StateMachineEx::StateMachineEx(uint8_t max_states) : current_state(0), _max_states(max_states), _event_generated(false)
{
	queued_state = ST_EMPTY;
}

void StateMachineEx::ChangeState(uint8_t new_state)
{
	current_state = new_state;
}

uint8_t StateMachineEx::GetState()
{
	return current_state;
}

void StateMachineEx::Event(uint8_t new_state, EventDataEx* pdata)
{
	if(new_state != ST_NOT_ALLOWED)
		AddToQueue(new_state);
}

void StateMachineEx::AddToQueue(uint8_t state)
{
	if(queued_state == ST_EMPTY)
		queued_state = state;
}

uint8_t StateMachineEx::GetFromQueue()
{
	uint8_t tmp_st = queued_state;
	queued_state = ST_EMPTY;
    return tmp_st;
}

void StateMachineEx::InternalEvent(uint8_t new_state, EventDataEx* pdata)
{
	_event_generated = true;
//	ChangeState(new_state);
	while(_event_generated)
	{
		const StateStructEx* pStateMap = GetStateMapEx();
		// call state function
		(this->*pStateMap[current_state].pStateFunc)(pdata);
		_event_generated = false;
	}
}
