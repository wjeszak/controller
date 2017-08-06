/*
 * machine_type.h
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#ifndef MACHINE_TYPE_H_
#define MACHINE_TYPE_H_

#include "machine.h"

#define MACHINE_DYNABOX 			0
#define MACHINE_LOCKERBOX 			1

extern Machine* GetPointerTypeOfMachine(uint8_t type);

#endif /* MACHINE_TYPE_H_ */
