/*
 * maszyna.cpp
 *
 *  Created on: 3 cze 2017
 *      Author: tomek
 */

#include "maszyna.h"

#include <assert.h>
extern "C" void __cxa_pure_virtual(void);
void __cxa_pure_virtual(void) {};
void * operator new(size_t size)
{
  return malloc(size);
}

void operator delete(void * ptr)
{
  free(ptr);
}
//----------------------------------------------------------------------------
// StateMachine
//----------------------------------------------------------------------------
MaszynaStanow::MaszynaStanow(uint8_t maxStates, uint8_t initialState) :
	MAX_STATES(maxStates),
	m_currentState(initialState),
	m_newState(false),
	m_eventGenerated(false)
{
	assert(MAX_STATES < EVENT_IGNORED);
}

//----------------------------------------------------------------------------
// ExternalEvent
//----------------------------------------------------------------------------
void MaszynaStanow::ExternalEvent(uint8_t newState)
{
	// If we are supposed to ignore this event
	if (newState == EVENT_IGNORED)
	{

	}
	else
	{
		// TODO - capture software lock here for thread-safety if necessary

		// Generate the event
		InternalEvent(newState);

		// Execute the state engine. This function call will only return
		// when all state machine events are processed.
		StateEngine();

		// TODO - release software lock here
	}
}

//----------------------------------------------------------------------------
// InternalEvent
//----------------------------------------------------------------------------
void MaszynaStanow::InternalEvent(uint8_t newState)
{
	m_eventGenerated = true;
	m_newState = newState;
}

//----------------------------------------------------------------------------
// StateEngine
//----------------------------------------------------------------------------
void MaszynaStanow::StateEngine(const StateMapRow* const pStateMap)
{
#if EXTERNAL_EVENT_NO_HEAP_DATA
	BOOL externalEvent = TRUE;
#endif

	// While events are being generated keep executing states
	while (m_eventGenerated)
	{
		// Error check that the new state is valid before proceeding
		assert(m_newState < MAX_STATES);

		// Get the pointer from the state map
		const StateBase* state = pStateMap[m_newState].State;

		// Event used up, reset the flag
		m_eventGenerated = false;

		// Switch to the new current state
		SetCurrentState(m_newState);

		// Execute the state action passing in event data
		assert(state != NULL);
		state->InvokeStateAction(this);
	}
}
