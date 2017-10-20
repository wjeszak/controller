/*
 * machine.h
 *
 *  Created on: 6 sie 2017
 *      Author: tomek
 */

#ifndef MACHINE_H_
#define MACHINE_H_

#include "state_machine.h"
#include "comm_prot.h"

#define MACHINE_TYPE_DYNABOX 					0
#define MACHINE_TYPE_LOCKERBOX 					1

class MachineData : public EventData
{
public:
	CommStatus comm_status;
};

class Machine : public StateMachine
{
public:
	Machine();
	virtual void Init() {}
	virtual void EV_EnterToConfig() {}
	virtual void LoadSupportedFunctions() {}
	virtual void SaveParameters() {}
	virtual void Parse(uint8_t* frame) {}
	virtual void SlavesPoll() {}
	virtual void ReplyTimeout() {}
	uint8_t first_address, last_address, current_address;
private:
	enum States {ST_MAX_STATES};
};

extern Machine* m;
extern MachineData* d;

extern void GetPointerTypeOfMachine(uint8_t type);

#endif /* MACHINE_H_ */
