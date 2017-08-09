/*
 * state_machine.h
 *
 *  Created on: 3 cze 2017
 *      Author: tomek
 */

#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_

#include <inttypes.h>
#include <stdio.h>

class EventData
{
public:
};

struct StateStruct;

class StateMachine
{
public:
	StateMachine(uint8_t max_states);
	uint8_t GetState();
	void InternalEvent(uint8_t new_state, EventData* pdata = NULL);
	void Event(uint8_t new_state, EventData* pdata = NULL);
protected:
	uint8_t current_state;
	enum States {ST_NOT_ALLOWED = 0xFF};
private:
	const uint8_t _max_states;
	bool _event_generated;
	void ChangeState(uint8_t new_state);
	virtual const StateStruct* GetStateMap() { return NULL; }
};

typedef void (StateMachine::*StateFunc)(EventData *);

struct StateStruct
{
    StateFunc pStateFunc;
};

// State map
#define BEGIN_STATE_MAP \
const StateStruct* GetStateMap() {\
    static const StateStruct StateMap[] = {

#define STATE_MAP_ENTRY(stateFunc)\
    { reinterpret_cast<StateFunc>(stateFunc) },

#define END_STATE_MAP \
    }; \
    return &StateMap[0]; }

// Transition map
#define BEGIN_TRANSITION_MAP \
	const uint8_t Transitions[] = {\

#define TRANSITION_MAP_ENTRY(entry)\
    entry,

#define END_TRANSITION_MAP(data) \
    0 };\
    Event(Transitions[current_state], pdata);

#endif /* STATE_MACHINE_H_ */
