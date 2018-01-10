/*
 * fault.h
 *
 *  Created on: 20 paü 2017
 *      Author: tomek
 */

#ifndef FAULT_H_
#define FAULT_H_

#include <inttypes.h>
#include "machine.h"

enum FaultType { None, F01_Led, F02_Door, F03_Transoptors, F04_DoorOpenedSoFar, F05_Elm,
			  	 F06_CloseDoor, F07_DoorNotOpen, F08_IllegalOpening,
				 F10_MechanicalWarning = 0x0A, F11_MechanicalFault, F12_Positioning,
				 F13_MainDoor, F14_HomingFailed, F15_IllegalRotation, F16_OrderRefused,
				 F17_24VMissing };

class Fault
{
public:
	Fault();
	void SetGlobal(FaultType fault);
	void ClearGlobal(FaultType fault);
	bool IsGlobal(FaultType fault);
	bool IsGlobal();
	void SetLocal(uint8_t address, FaultType fault);
	void ClearLocal(uint8_t address, FaultType fault);
	bool IsLocal(uint8_t address, FaultType fault);
	bool IsLocal(uint8_t address);
	void Show();
private:
	uint32_t global_faults;
	uint8_t  doors_faults[MACHINE_MAX_NUMBER_OF_DOORS];
};

extern Fault fault;

#endif /* FAULT_H_ */
