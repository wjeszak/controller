/*
 * boot.cpp
 *
 *  Created on: 19 paü 2017
 *      Author: tomek
 */

#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "machine.h"
#include "timer.h"

uint8_t machine_type EEMEM = MACHINE_TYPE_DYNABOX;

void Boot1()
{
	uint8_t mt = eeprom_read_byte(&machine_type);
	GetPointerTypeOfMachine(mt);
	m->LoadSupportedFunctions();
	sei();
	timer.Assign(TIMER_DISPLAY_REFRESH, 4, DisplayRefresh);
//	timer.Assign(TIMER_INIT_COUNTDOWN, 1000, InitCountDown);
	timer.Assign(TIMER_ENCODER_POLL, 1, EncoderPoll);
}


