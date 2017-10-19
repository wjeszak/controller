/*
 * boot.cpp
 *
 *  Created on: 19 pa� 2017
 *      Author: tomek
 */

#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "machine.h"
#include "timer.h"

// default value in EEPROM
uint8_t machine_type EEMEM = MACHINE_TYPE_DYNABOX;

void Boot()
{
	uint8_t mt = eeprom_read_byte(&machine_type);
	GetPointerTypeOfMachine(mt);
	m->Init();
	m->LoadSupportedFunctions();
	sei();
	timer.Assign(TIMER_DISPLAY_REFRESH, 4, DisplayRefresh);
	timer.Assign(TIMER_ENCODER_POLL, 1, EncoderPoll);
	timer.Assign(TIMER_FAULTS_HANDLING, 1000, ShowFault);
}

