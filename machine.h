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
	uint32_t faults;
};

class Machine : public StateMachine
{
public:
	Machine();
	void SetFault(uint8_t fault);
	void ClearFault(uint8_t fault);
	bool CheckFault(uint8_t fault);
	virtual void LoadSupportedFunctions() {}
	virtual void SaveParameters() {}
	virtual void SetCurrentCommand(uint8_t command, bool rep) {}
	virtual void Parse(uint8_t* frame) {}
	virtual void SlavesPoll() {}
	virtual void ReplyTimeout() {}
	uint8_t first_address, last_address, current_address;
	uint8_t current_command;
private:
	enum States {ST_MAX_STATES};
};

extern Machine* m;
extern MachineData* d;

extern void GetPointerTypeOfMachine(uint8_t type);

#endif /* MACHINE_H_ */
