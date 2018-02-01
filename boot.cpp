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
#include "fault.h"

// default value in EEPROM
uint8_t ee_machine_type EEMEM = MACHINE_TYPE_LOCKERBOX;
uint8_t machine_type;

void Boot()
{
	uint8_t mt = eeprom_read_byte(&ee_machine_type);
	GetPointerTypeOfMachine(mt);
	m->Init();
	m->LoadParameters();
	sei();
	timer.Assign(TDisplayRefresh, DISPLAY_REFRESH_INTERVAL, DisplayRefresh);
	timer.Assign(TFaultShow, FAULT_SHOW_INTERVAL, FaultShow);
	timer.Assign(TEncoderPoll, ENCODER_POLL_INTERVAL, EncoderPoll);
	// przeniesc do funkcji ktory sie wykonuje tylko przy Dynaboxie
	timer.Assign(TMotorSpeedMeas, MOTOR_SPEED_MEAS_INTERVAL, MotorSpeedMeas);
}
