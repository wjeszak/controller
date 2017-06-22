/*
 * maszyna.cpp
 *
 *  Created on: 3 cze 2017
 *      Author: tomek
 */

#include "machine.h"
#include "usart.h"

Machine::Machine(uint8_t max_states) : current_state(0), _max_states(max_states), _event_generated(false)
{
}

/*
const StateStruct* Machine::GetStateMap()
{
	// to jest sprytne bo StateMap jest tworzone nie na stosie dzieki temu mozna zwrocic adres
	static const StateStruct StateMap[] =
	{
		//{&Machine::ST_Init},
		//{&Machine::ST_Idle}
	};
	return &StateMap[0];
}
*/
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
		//usart.ST_Idle(NULL);
		//(this->*fp[0])(pdata);			// run state function
		const StateStruct* pStateMap = GetStateMap();
		(this->*pStateMap[current_state].pStateFunc)(pdata);
		_event_generated = false;
	}
}
