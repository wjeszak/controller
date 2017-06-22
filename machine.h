/*
 * maszyna.h
 *
 *  Created on: 3 cze 2017
 *      Author: tomek
 */

#ifndef MACHINE_H_
#define MACHINE_H_

#include <inttypes.h>
#include <stdio.h>			// NULL

class EventData
{
public:
};

class Machine
{
public:
	Machine();
	uint16_t GetState();
	void ChangeState(uint16_t new_state);
	void Event(uint16_t new_state, EventData* pdata = NULL);
	void InternalEvent(uint16_t new_state, EventData* pdata);
	virtual uint16_t Who() { return 10; }
	//virtual ~Machine();
protected:
	uint16_t current_state;
private:
	bool _event_generated;
	void StateEngine();
	void (Machine::*pfun[])(EventData* pdata);
	enum States {NOT_ALLOWED = 0xFF};
};

#endif /* MACHINE_H_ */
