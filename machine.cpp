/*
 * machine.cpp
 *
 *  Created on: 6 sie 2017
 *      Author: tomek
 */

#include "machine.h"
#include "dynabox.h"
#include "lockerbox.h"

void GetPointerTypeOfMachine(uint8_t type)
{
	switch (type)
	{
	case MACHINE_TYPE_DYNABOX:
		m = &dynabox;
	break;
	case MACHINE_TYPE_LOCKERBOX:
		m = &lockerbox;
	break;
	default:
		m = NULL;
	break;
	}
}

Machine::Machine() : StateMachine(ST_MAX_STATES)
{
	current_address =  1;
}
