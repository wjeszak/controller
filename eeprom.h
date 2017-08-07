/*
 * eeprom.h
 *
 *  Created on: 3 cze 2017
 *      Author: tomek
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#include "machine_type.h"

#define MAX_NUMBER_OF_FUNCTIONS 			28

typedef void (*fp)();

struct Function
{
	uint8_t id;
	uint8_t param;
	fp f;
};

struct ConfigurationParams
{
	uint8_t mac_addr[6];
	uint8_t ip_addr[4];
};

class Eeprom
{
public:
	Eeprom();
	void Read();
	void Write();
};
extern Function functions[MAX_NUMBER_OF_FUNCTIONS];
extern ConfigurationParams cfg;
extern Eeprom eeprom;

#endif /* EEPROM_H_ */
