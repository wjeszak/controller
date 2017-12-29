/*
 * fault.cpp
 *
 *  Created on: 20 paü 2017
 *      Author: tomek
 */

#include "dynabox_commands_faults.h"
#include "fault.h"
#include "modbus_tcp.h"
#include "display.h"

Fault::Fault()
{
	global_faults = 0;
}

void Fault::SetGlobal(uint8_t fault)
{
	global_faults |= (1ULL << fault);
	mb.Write(GENERAL_ERROR_STATUS, fault);
}

void Fault::ClearGlobal(uint8_t fault)
{
	global_faults &= ~(1ULL << fault);
}

bool Fault::CheckGlobal(uint8_t fault)
{
	if(global_faults & (1ULL << fault)) return true;
	return false;
}

bool Fault::CheckGlobal()
{
	for(uint8_t i = 0; i < NUMBER_OF_FAULTS; i++)
	{
		if(global_faults & (1ULL << i)) return true;
	}
	return false;
}

void Fault::ShowGlobal()
{
	static uint8_t i = 1;
	if(global_faults == 0) { display.Write(TNoFault, 0); return; }
	while(i <= NUMBER_OF_FAULTS + 1)
	{
		if(i == NUMBER_OF_FAULTS + 1) i = 1;
		if(global_faults & (1ULL << i))
		{
			display.Write(TFault, i);
			i++;
			break;
		}
		i++;
	}
}

void Fault::Set(uint8_t fault, uint8_t address)
{
	doors_faults[address - 1] |= (1 << (fault - 1));
}

void Fault::Clear(uint8_t fault, uint8_t address)
{
	doors_faults[address - 1] &= ~(1 << (fault - 1));
}

bool Fault::Check(uint8_t fault, uint8_t address)
{
	if(doors_faults[address - 1] & (1 << (fault - 1)))
		return true;
	return false;
}
