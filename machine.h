/*
 * maszyna.h
 *
 *  Created on: 3 cze 2017
 *      Author: tomek
 */

#ifndef MACHINE_H_
#define MACHINE_H_

#include <inttypes.h>
#include <stdio.h>

class EventData
{
public:
};

struct StateStruct;

class Machine
{
public:
	Machine(uint8_t max_states);
	uint16_t GetState();
	void Event(uint16_t new_state, EventData* pdata = NULL);
	virtual uint16_t StartupTest() { return 0xFF; }
//	virtual ~Machine();
protected:
	uint16_t current_state;
	enum States {ST_NOT_ALLOWED = 0xFF};
private:
	const uint8_t _max_states;
	bool _event_generated;
	void ChangeState(uint16_t new_state);
	virtual const StateStruct* GetStateMap() { return NULL; }
};

typedef void (Machine::*StateFunc)(EventData *);

struct StateStruct
{
    StateFunc pStateFunc;
};

extern Machine *m;

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

#endif /* MACHINE_H_ */
