/*
 * maszyna.h
 *
 *  Created on: 3 cze 2017
 *      Author: tomek
 */

#ifndef MASZYNA_H_
#define MASZYNA_H_
#include <inttypes.h>
#include <stdio.h>			// NULL
#include <stdlib.h>
#include <assert.h>
void * operator new(size_t size);
void operator delete(void * ptr);

// Dane zdarzenia
class EventData
{
public:
	virtual ~EventData() {};
};
typedef EventData NoEventData;

class StateMachine;
// Klasa bazowa abstrakcyjna dla kazdego innego stanu
/// @brief Abstract state base class that all states inherit from.
class StateBase
{
public:
	/// Called by the state machine engine to execute a state action. If a guard condition
	/// exists and it evaluates to false, the state action will not execute.
	/// @param[in] sm - A state machine instance.
	/// @param[in] data - The event data.
	virtual void InvokeStateAction(StateMachine* sm, const EventData* data) const = 0;
};

// Szablon do tworzenia nowych klas stanu
/// @brief StateAction takes three template arguments: A state machine class,
/// a state function event data type (derived from EventData) and a state machine
/// member function pointer.


/// @brief A structure to hold a single row within the state map.
struct StateMapRow
{
	const StateBase* const State;
};

class MaszynaStanow
{
public:
	enum { EVENT_IGNORED = 0xFE, CANNOT_HAPPEN };
	MaszynaStanow(uint8_t maxStates, uint8_t initialState = 0);
	uint8_t GetCurrentState() { return m_currentState; }
	uint8_t GetMaxStates() { return MAX_STATES; }
	virtual ~MaszynaStanow() {}
protected:
	void ExternalEvent(uint8_t newState);
	void InternalEvent(uint8_t newState);

private:
	const uint8_t MAX_STATES;
	uint8_t m_currentState;
	uint8_t m_newState;
	bool m_eventGenerated;
	virtual void InvokeStateAction(StateMachine* sm);
	virtual const StateMapRow* GetStateMap() = 0;
	void SetCurrentState(uint8_t newState) { m_currentState = newState; }
	void StateEngine(void);
	void StateEngine(const StateMapRow* const pStateMap);
};



#define STATE_DEFINE(stateMachine, stateName, eventData) \
	void stateMachine::ST_##stateName(const eventData* data)

#define BEGIN_TRANSITION_MAP \
    static const uint8_t TRANSITIONS[] = {\

#define TRANSITION_MAP_ENTRY(entry)\
    entry,

#define END_TRANSITION_MAP(data) \
    };\
	assert(GetCurrentState() < ST_MAX_STATES); \
    ExternalEvent(TRANSITIONS[GetCurrentState()], data); \
	assert((sizeof(TRANSITIONS)/sizeof(uint8_t)) == ST_MAX_STATES);

#define PARENT_TRANSITION(state) \
	if (GetCurrentState() >= ST_MAX_STATES && \
		GetCurrentState() < GetMaxStates()) { \
		ExternalEvent(state); \
		return; }


#endif /* MASZYNA_H_ */
