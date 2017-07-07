/*
 * lockerbox.cpp
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#include <avr/io.h> // ze wzgledu na preskaler timera
#include "lockerbox.h"
#include "timer.h"
Lockerbox::Lockerbox() : Machine(ST_MAX_STATES)
{

}

uint16_t Lockerbox::StartupTest()
{
	timer.Assign(2, 500, ModbusPoll);
	return 1;
}

