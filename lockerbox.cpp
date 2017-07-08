/*
 * lockerbox.cpp
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#include "lockerbox.h"
#include "timer.h"

Lockerbox::Lockerbox() : Machine(ST_MAX_STATES)
{

}

uint16_t Lockerbox::StartupTest()
{
	timer.Assign(TIMER_MODBUS_RTU_POLL, 1000, ModbusPoll);
	return 1;
}
