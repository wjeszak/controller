/*
 * machine.cpp
 *
 *  Created on: 6 sie 2017
 *      Author: tomek
 */

#include "machine.h"
#include "dynabox.h"
#include "lockerbox.h"
#include "config.h"
#include "modbus_tcp.h"

void GetPointerTypeOfMachine(uint8_t type)
{
	switch (type)
	{
	case MACHINE_TYPE_DYNABOX:
		m = &dynabox;
		d = &dynabox_data;
	break;
	case MACHINE_TYPE_LOCKERBOX:
		m = &lockerbox;
		d = &lockerbox_data;
	break;
	default:
		m = NULL;
	break;
	}
}

Machine::Machine() : StateMachine(ST_MAX_STATES)
{
	current_address =  1;
}

bool Machine::LastAddress()
{
	if(current_address == functions[1].param + 1)
		return true;
	return false;
}

void Machine::SetOrderStatus(uint8_t status)
{
	mb.UpdateHoldingRegister(ORDER_STATUS, status);
}

uint8_t Machine::GetOrderStatus()
{
	return mb.GetHoldingRegister(ORDER_STATUS);
}

void Machine::SetIOInfo(uint8_t info)
{
	mb.SetBitHoldingRegister(IO_INFORMATIONS, info);
}

void Machine::ClearIOInfo(uint8_t info)
{
	mb.ClearBitHoldingRegister(IO_INFORMATIONS, info);
}
