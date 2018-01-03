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

class Fault
{
public:
	Fault();
	void SetGlobal(uint8_t fault);
	void ClearGlobal(uint8_t fault);
	bool CheckGlobal(uint8_t fault);
	bool CheckGlobal();
	void ShowGlobal();
	void Set(uint8_t fault, uint8_t address);
	void Clear(uint8_t fault, uint8_t address);
	bool Check(uint8_t fault, uint8_t address);
	bool CheckAll(uint8_t address);
private:
	uint32_t global_faults;
	uint8_t  doors_faults[MACHINE_MAX_NUMBER_OF_DOORS];
};

extern Fault fault;

#endif /* FAULT_H_ */
