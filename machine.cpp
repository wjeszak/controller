/*
 * machine.cpp
 *
 *  Created on: 6 sie 2017
 *      Author: tomek
 */

#include <avr/eeprom.h>
#include "state_machine.h"
#include "machine_type.h"
#include "machine.h"



Machine::Machine() : StateMachine(ST_MAX_STATES)
{

}



