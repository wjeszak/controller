/*
 * fault.cpp
 *
 *  Created on: 20 paü 2017
 *      Author: tomek
 */

#include "fault.h"
#include "modbus_tcp.h"
#include "display.h"

Fault::Fault()
{
	faults = 0;
}

void Fault::Set(uint8_t fault)
{
	faults |= (1ULL << fault);
	mb.UpdateHoldingRegister(GENERAL_ERROR_STATUS, fault);
}

void Fault::Clear(uint8_t fault)
{
	faults &= ~(1ULL << fault);
}

bool Fault::Check(uint8_t fault)
{
	if(faults & (1ULL << fault)) return true;
	return false;
}

void Fault::Show()
{
	static uint8_t i = 1;
	if(faults == 0) { display.Write(TNoFault, 0); return; }
	while(i <= 18)
	{
		if(i == 18) i = 1;
		if(faults & (1ULL << i))
		{
			display.Write(TFault, i);
			i++;
			break;
		}
		i++;
	}
}
