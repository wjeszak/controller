/*
 * dynabox.cpp
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#include <avr/io.h> 		// to jest ze wzgledu na preskalery silnika, zmienic!
#include "dynabox.h"
#include "motor.h"

Dynabox::Dynabox() : Machine(ST_MAX_STATES)
{

}

uint16_t Dynabox::StartupTest()
{
	motor.Enable(Forward, 20);
	return 10;
}



