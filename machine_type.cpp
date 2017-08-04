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
