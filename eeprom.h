/*
 * eeprom.h
 *
 *  Created on: 3 cze 2017
 *      Author: tomek
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#include "machine.h"

struct Config
{
	MachineType machine_type;
};
class Eeprom
{
public:
	void Read();
	void Write();
};




#endif /* EEPROM_H_ */
