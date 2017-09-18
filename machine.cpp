/*
 * machine.cpp
 *
 *  Created on: 6 sie 2017
 *      Author: tomek
 */

#include "dynabox.h"
#include "lockerbox.h"
#include "config.h"

void GetPointerTypeOfMachine(uint8_t type)
{
	switch (type)
	{
	case MACHINE_DYNABOX:
		m = &dynabox;
		d = &dynabox_data;
	break;
	case MACHINE_LOCKERBOX:
		m = &lockerbox;
		d = &lockerbox_data;
	break;
	default:
		m = NULL;
		d = NULL;
	break;
	}
}

Machine::Machine() : StateMachine(ST_MAX_STATES)
{

}

void Machine::SetFault(Machine::FaultsType fault)
{
	d->faults |= (1ULL << fault);
}

void Machine::ClearFault(FaultsType fault)
{
	d->faults &= ~(1ULL << fault);
}

bool Machine::CheckFault(FaultsType fault)
{
	if(d->faults & (1ULL << fault))	return true;
	return false;
}
/*
void Machine::SetFault(uint8_t fault)
{
	d->faults |= (1ULL << fault);
}

void Machine::ClearFault(uint8_t fault)
{
	d->faults &= ~(1ULL << fault);
}
*/
