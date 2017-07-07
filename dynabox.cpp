/*
 * dynabox.cpp
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#include "dynabox.h"
#include "motor.h"
#include "timer.h"

Dynabox::Dynabox() : Machine(ST_MAX_STATES)
{

}

uint16_t Dynabox::StartupTest()
{
	motor.EV_Homing();
	timer.Assign(TIMER_MODBUS_RTU_POLL, 500, ModbusPoll);
	return 2;
}



