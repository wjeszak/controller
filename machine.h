/*
 * machine.h
 *
 *  Created on: 6 sie 2017
 *      Author: tomek
 */

#ifndef MACHINE_H_
#define MACHINE_H_

#include "state_machine.h"

class Machine : public StateMachine
{
public:
	Machine();
	virtual void LoadSupportedFunctions() {}
	virtual void SaveParameters() {}
private:
	enum States {ST_MAX_STATES};
};

extern Machine* GetPointerTypeOfMachine(uint8_t type);
extern Machine *m;

#endif /* MACHINE_H_ */
