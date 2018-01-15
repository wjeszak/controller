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

// Global faults from bit 1 to n
// Door's faults from bit 0 to n

Fault::Fault()
{
	global_faults = 0;
}

void Fault::SetGlobal(FaultType fault)
{
	global_faults |= (1UL << fault);
	mb.Write((uint8_t)GENERAL_ERROR_STATUS, fault);
}

void Fault::ClearGlobal(FaultType fault)
{
	global_faults &= ~(1UL << fault);
	// tutaj dopisac "rekalkulacje" bledow, jesli jeden zniknie to
	// nie jest powiedziane, ze wiecej nie bedzie
	mb.Write((uint8_t)GENERAL_ERROR_STATUS, 0);
}

bool Fault::IsGlobal(FaultType fault)
{
	if(global_faults & (1UL << fault))
		return true;
	else
		return false;
}

bool Fault::IsGlobal()
{
	if(global_faults != 0UL)
		return true;
	else
		return false;
}

void Fault::Show()
{
	static uint8_t i = 1;
	if(!IsGlobal())
	{
		display.Write(TNoFault, 0);
		i = 1;
		return;
	}
	while(i <= NUMBER_OF_FAULTS + 1)
	{
		if(i == NUMBER_OF_FAULTS + 1) i = 1;
		if(global_faults & (1UL << i))
		{
			display.Write(TFault, i);
			i++;
			break;
		}
		i++;
	}
}

/*
void Fault::Show()
{
	static uint8_t i = 1;
	if(!IsGlobal()) { display.Write(TNoFault, 0); return; }
	if(i == NUMBER_OF_FAULTS) i = 1;
	for(uint8_t fault = i; fault <= NUMBER_OF_FAULTS; fault++)
	{
		if(global_faults & (1ULL << fault))
		{
			display.Write(TFault, fault);
			break;
		}
	}
	i++;
}
*/
// -------------------------- door's faults --------------------------
void Fault::SetLocal(uint8_t address, FaultType fault)
{
	doors_faults[address - 1] |= (1 << (fault - 1));
}

void Fault::ClearLocal(uint8_t address, FaultType fault)
{
	doors_faults[address - 1] &= ~(1 << (fault - 1));
	mb.Write((uint8_t)(address + 1), 0 << 8);
}

bool Fault::IsLocal(uint8_t address, FaultType fault)
{
	if(doors_faults[address - 1] & (1 << (fault - 1)))
		return true;
	return false;
}

bool Fault::IsLocal(uint8_t address)
{
	if(doors_faults[address - 1] != 0)
		return true;
	return false;

}
