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
#include "config.h"

// default value in EEPROM
uint8_t ee_machine_type EEMEM = MACHINE_TYPE_DYNABOX;
uint8_t machine_type;

void Boot()
{
	uint8_t mt = eeprom_read_byte(&ee_machine_type);
	GetPointerTypeOfMachine(mt);
	m->Init();
	m->LoadParameters();
	sei();
	timer.Assign(TIMER_DISPLAY_REFRESH, 4, DisplayRefresh);
	timer.Assign(TIMER_ENCODER_POLL, 1, EncoderPoll);
	timer.Assign(TIMER_MOTOR_SPEED_MEAS, MOTOR_SPEED_MEAS_INTERVAL, MotorSpeedMeas);
//	timer.Assign(TIMER_DEBUG, DEBUG_INTERVAL, Debug);
}
