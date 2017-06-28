/*
 * machine_type.h
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#ifndef MACHINE_TYPE_H_
#define MACHINE_TYPE_H_

#include "machine.h"

enum MachineType {TLockerbox, TDynabox};
extern Machine* GetTypeOfMachine(MachineType type);

#endif /* MACHINE_TYPE_H_ */
