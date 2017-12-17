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
#include "usart.h"

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

Machine::Machine() : StateMachineEx(ST_MAX_STATES)
{
	current_address =  1;
}

uint8_t Machine::LastAddress()
{
	return functions[1].param;
}

void Machine::SetOrderStatus(OrderStatus status)
{
	mb.Write(ORDER_STATUS, status);
}

OrderStatus Machine::GetOrderStatus()
{
	uint8_t status = mb.Read((uint8_t)ORDER_STATUS);
	return (OrderStatus)status;
}

void Machine::SetIOInfo(IO_InfoType info)
{
	mb.SetBit(IO_INFORMATIONS, info);
}

void Machine::ClearIOInfo(IO_InfoType info)
{
	mb.ClearBit(IO_INFORMATIONS, info);
}

