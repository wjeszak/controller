/*
 * fault.h
 *
 *  Created on: 20 paü 2017
 *      Author: tomek
 */

#ifndef FAULT_H_
#define FAULT_H_

#include <inttypes.h>

class Fault
{
public:
	Fault();
	void Set(uint8_t fault);
	void Clear(uint8_t fault);
	bool Check(uint8_t fault);
	void Show();
private:
	uint32_t faults;
};

extern Fault fault;

#endif /* FAULT_H_ */
