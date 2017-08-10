/*
 * machine.cpp
 *
 *  Created on: 6 sie 2017
 *      Author: tomek
 */

#include "dynabox.h"
#include "lockerbox.h"
#include "config.h"

Machine* GetPointerTypeOfMachine(uint8_t type)
{
	Machine* p = NULL;
	switch (type)
	{
	case MACHINE_DYNABOX:
		p = &dynabox;
		break;
	case MACHINE_LOCKERBOX:
		p = &lockerbox;
		break;
	default:
		p = NULL;
	}
	return p;
}

Machine::Machine() : StateMachine(ST_MAX_STATES)
{

}
