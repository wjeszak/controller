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
	virtual uint8_t StartupTest() { return 0xFF; }
	virtual void LoadSupportedFunctions() {}
private:
	enum States {ST_INIT = 0, ST_MAX_STATES};
};

extern Machine *m;

#endif /* MACHINE_H_ */
