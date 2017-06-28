/*
 * lockerbox.cpp
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#include "lockerbox.h"

Lockerbox::Lockerbox() : Machine(ST_MAX_STATES)
{

}

uint16_t Lockerbox::StartupTest()
{
	return 5;
}

