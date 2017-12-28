/*
 * state_machine_ex.h
 *
 *  Created on: 20 lis 2017
 *      Author: tomek
 */

#ifndef STATE_MACHINE_EX_H_
#define STATE_MACHINE_EX_H_

#include <inttypes.h>
#include <stdio.h>

class EventDataEx
{
public:
};

struct StateStructEx;

class StateMachineEx
{
protected:
	uint8_t current_state;
	enum States {ST_EMPTY = 0xFE, ST_NOT_ALLOWED = 0xFF};
public:
	StateMachineEx(uint8_t max_states);
	States GetState();
	void ChangeState(States new_state);
	void InternalEventEx(States new_state, EventDataEx* pdata = NULL);
	void EventEx(States new_state, EventDataEx* pdata = NULL);
	void AddToQueue(States state);
	uint8_t GetFromQueue();
private:
	const uint8_t _max_states;
	bool _event_generated;
	virtual const StateStructEx* GetStateMapEx() { return NULL; }
	uint8_t queued_state;
};

typedef void (StateMachineEx::*StateFuncEx)(EventDataEx*);

struct StateStructEx
{
    StateFuncEx pStateFunc;
};

// State map
#define BEGIN_STATE_MAP_EX \
const StateStructEx* GetStateMapEx() {\
    static const StateStructEx StateMapEx[] = {

#define STATE_MAP_ENTRY_EX(stateFunc)\
    { reinterpret_cast<StateFuncEx>(stateFunc) },

#define END_STATE_MAP_EX \
    }; \
    return &StateMapEx[0]; }
// ------------------------------------------------------------
// Transition map
#define BEGIN_TRANSITION_MAP_EX \
	const uint8_t Transitions[] = {\

#define TRANSITION_MAP_ENTRY_EX(entry)\
    entry,

#define END_TRANSITION_MAP_EX(data) \
    0 };\
    EventEx(Transitions[current_state], pdata);

#endif /* STATE_MACHINE_EX_H_ */
