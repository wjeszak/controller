/*
 * boot.h
 *
 *  Created on: 19 paü 2017
 *      Author: tomek
 */

#ifndef BOOT_H_
#define BOOT_H_
#include <avr/eeprom.h>

extern uint8_t ee_machine_type EEMEM;
//extern uint8_t machine_type;

extern void Boot();

#endif /* BOOT_H_ */
