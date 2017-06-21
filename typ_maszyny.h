/*
 * typ_maszyny.h
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#ifndef TYP_MASZYNY_H_
#define TYP_MASZYNY_H_

#include "dynabox.h"
#include "lockerbox.h"
#include "machine.h"

Machine * GetTypeOfMachine(MachineType type)
{
	Machine *p = NULL;
	switch (type)
	{
	case Lockerbox:
		p = &lockerbox;
		break;
	case Dynabox:
		p = &dynabox;
		break;
	default:
		p = NULL;
	}
	return p;
}

#endif /* TYP_MASZYNY_H_ */
