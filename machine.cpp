/*
 * machine.cpp
 *
 *  Created on: 6 sie 2017
 *      Author: tomek
 */

#include "dynabox.h"
#include "lockerbox.h"
#include "config.h"

void GetPointerTypeOfMachine(uint8_t type, Machine* mach, MachineData* mach_data)
{
	switch (type)
	{
	case MACHINE_DYNABOX:
		mach = &dynabox;
		mach_data = &dynabox_data;
		break;
	case MACHINE_LOCKERBOX:
		mach = &lockerbox;
		mach_data = &lockerbox_data;
		break;
	default:
		mach = NULL;
		mach_data = NULL;
	}
}

Machine::Machine() : StateMachine(ST_MAX_STATES)
{

}

void Machine::SetFault(FaultsType fault)
{
	d->errors |= (1 << fault);
}

void Machine::ClearFault(FaultsType fault)
{
	d->errors &= ~(1 << fault);
}
