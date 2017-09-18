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
	enum FaultsType {NoFault, F01_SlaveLed, F02_SlaveDoor, F03_OpticalSwitches, F04_DoorOpenedTooFar,
			F05_ElectromagnetFault, F06_CloseTheDoor, F07_DoorNotOpen, F08_IllegalOpening, F09_None,
			F10_MechanicalWarning, F11_MechanicalFault, F12_Positioning, F13_MainDoor, F14_HomingFailed,
			F15_IllegalRotation, F16_OrderRefused, F17_24VMissing};
	Machine();
//	void SetFault(FaultsType fault);
//	void ClearFault(FaultsType fault);
	void SetFault(uint8_t fault);
	void ClearFault(uint8_t fault);
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

extern void GetPointerTypeOfMachine(uint8_t type);

#endif /* MACHINE_H_ */
