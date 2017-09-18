/*
 * machine.h
 *
 *  Created on: 6 sie 2017
 *      Author: tomek
 */

#ifndef MACHINE_H_
#define MACHINE_H_

#include "state_machine.h"

class MachineData : public EventData
{
public:
	uint32_t errors;
};

class Machine : public StateMachine
{
public:
	enum FaultsType	{ };
	Machine();
	void SetFault(FaultsType fault);
	void ClearFault(FaultsType fault);
	virtual void LoadSupportedFunctions() {}
	virtual void SaveParameters() {}
	virtual void Parse(uint8_t* frame) {}
	virtual void ReplyTimeout() {}
	uint8_t first_addr, last_addr, curr_addr;

private:
	enum States {ST_MAX_STATES};
};

extern Machine* m;
extern MachineData* d;

extern void GetPointerTypeOfMachine(uint8_t type, Machine* mach, MachineData* mach_data);

#endif /* MACHINE_H_ */
