/*
 * machine_type.cpp
 *
 *  Created on: 21 cze 2017
 *      Author: tomek
 */

#include "machine_type.h"
#include "machine.h"
#include "lockerbox.h"
#include "dynabox.h"

Machine* GetTypeOfMachine(MachineType type)
{
	Machine* p = NULL;
	switch (type)
	{
	case TLockerbox:
		p = &lockerbox;
		break;
	case TDynabox:
		p = &dynabox;
		break;
	default:
		p = NULL;
	}
	return p;
}


