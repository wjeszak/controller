/*
 * lockerbox.cpp
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#include "lockerbox.h"
#include "timer.h"

Lockerbox::Lockerbox()
{

}

uint8_t Lockerbox::StartupTest()
{
	timer.Assign(TIMER_MODBUS_RTU_POLL, 1000, ModbusPoll);
	return 1;
}

