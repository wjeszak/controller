/*
 * modbus_tcp.cpp
 *
 *  Created on: 28 cze 2017
 *      Author: tomek
 */

#include "modbus_tcp.h"
#include "display.h"
#include "stack.h"
#include "modbus_rtu.h"
#include "timer.h"
#include "machine.h"
#include "usart.h"
#include "common.h"

ModbusTCP::ModbusTCP()
{

}

void ModbusTCP::Process(uint8_t* frame)
{
	trans_id 		 = frame[MODBUS_TCP_TRANSACTION_ID_HI] << 8 | frame[MODBUS_TCP_TRANSACTION_ID_LO];
	prot_id 		 = frame[MODBUS_TCP_PROTOCOL_ID_HI] << 8 | frame[MODBUS_TCP_PROTOCOL_ID_LO];
	unit_id 		 = frame[MODBUS_TCP_UNIT_ID];
	function_code 	 = frame[MODBUS_TCP_FUNCTION];
	starting_address = frame[MODBUS_TCP_START_ADDR_HI] << 8 | frame[MODBUS_TCP_START_ADDR_LO];
	quantity 		 = frame[MODBUS_TCP_QUANTITY_HI] << 8 | frame[MODBUS_TCP_QUANTITY_LO];

	if(prot_id == PROTOCOL_ID_MODBUS_TCP)
	{
		switch(function_code)
		{
		case MODBUS_TCP_HOLDING_FUNCTION:
			ReadHoldingRegisters(frame);
			break;
		case MODBUS_TCP_WRITE_MULTIPLE_FUNCTION:
			WriteMultipleRegisters(frame);
			break;
		default:
			//FunctionNotSupported(frame);
			break;
		}
	}
}

void ModbusTCP::ReadHoldingRegisters(uint8_t* frame)
{
	uint8_t error_code = 0;

	if((quantity > MODBUS_TCP_HOLDING_MAX_QUANTITY) || (quantity < 1)) error_code = 3;
	if((starting_address + quantity - MODBUS_TCP_ADDR_OFFSET_HOLDING_REG) > MODBUS_TCP_NUMBER_OF_HOLDING_REG) error_code = 2;
	if(starting_address < MODBUS_TCP_ADDR_OFFSET_HOLDING_REG) error_code = 2;
	if(error_code)
	{
		SendErrorFrame(frame, error_code);
	}
	else
	{
		ReadHoldingRegistersReply(frame);
	}
}

void ModbusTCP::WriteMultipleRegisters(uint8_t* frame)
{
	uint8_t error_code = 0;

	if((quantity > MODBUS_TCP_MULTIPLE_MAX_QUANTITY) || (quantity < 1)) error_code = 3;
	if((starting_address + quantity - MODBUS_TCP_ADDR_OFFSET_MULTIPLE_REG) > MODBUS_TCP_NUMBER_OF_MULTIPLE_REG) error_code = 2;
	if(starting_address < MODBUS_TCP_ADDR_OFFSET_MULTIPLE_REG) error_code = 2;

	if(error_code)
	{
		SendErrorFrame(frame, error_code);
	}
	else
	{
		//MultipleRegisters[1] = 23;
		UpdateMultipleRegisters(frame, starting_address, quantity);
		WriteMultipleRegistersReply(frame);
		AnalizeMultipleRegisters();
	}
}

void ModbusTCP::UpdateHoldingRegisters(uint8_t address, uint16_t value)
{
	HoldingRegisters[address] = value;
}

void ModbusTCP::PrepareMBAPHeader(uint8_t* frame)	// without length
{
	frame[MODBUS_TCP_TRANSACTION_ID_HI] = hi(trans_id);
	frame[MODBUS_TCP_TRANSACTION_ID_LO] = lo(trans_id);
	// always 0 for ModbusTCP
	frame[MODBUS_TCP_PROTOCOL_ID_HI] = 0;
	frame[MODBUS_TCP_PROTOCOL_ID_LO] = 0;
	frame[MODBUS_TCP_UNIT_ID] = unit_id;
}

void ModbusTCP::ReturnHoldingRegisters(uint8_t* frame, uint16_t starting_address, uint16_t quantity)
{
	for(uint8_t i = 0; i < quantity; i++)
	{
		frame[MODBUS_RES_TCP_DATA + 2 * i]       = hi(HoldingRegisters[starting_address - MODBUS_TCP_ADDR_OFFSET_HOLDING_REG + i]);
		frame[MODBUS_RES_TCP_DATA + (2 * i) + 1] = lo(HoldingRegisters[starting_address - MODBUS_TCP_ADDR_OFFSET_HOLDING_REG + i]);
	}
}

void ModbusTCP::UpdateMultipleRegisters(uint8_t* frame, uint16_t starting_address, uint16_t quantity)
{
	for(uint8_t i = 0; i < quantity; i++)
	{
		MultipleRegisters[starting_address - MODBUS_TCP_ADDR_OFFSET_MULTIPLE_REG + i] = (hi(frame[MODBUS_REQ_TCP_REG_VAL_HI + 2 * i])) | (lo(frame[MODBUS_REQ_TCP_REG_VAL_LO + 2 * i]));
	}
}

void ModbusTCP::ReadHoldingRegistersReply(uint8_t* frame)
{
	PrepareMBAPHeader(frame);
	uint16_t length = (quantity * 2) + UNIT_ID_FUNCTION_BYTE_COUNT_LEN;
	frame[MODBUS_TCP_LENGTH_HI] = hi(length);
	frame[MODBUS_TCP_LENGTH_LO] = lo(length);
	frame[MODBUS_TCP_FUNCTION]  = function_code;
	frame[MODBUS_RES_TCP_BYTE_COUNT] = lo(quantity * 2);

	ReturnHoldingRegisters(frame, starting_address, quantity);
	stack_data.len = MBAP_FUNCTION_BYTE_COUNT_LEN + (quantity * 2);
}

void ModbusTCP::WriteMultipleRegistersReply(uint8_t *frame)
{
	PrepareMBAPHeader(frame);
	frame[MODBUS_TCP_LENGTH_HI] = 0;
	frame[MODBUS_TCP_LENGTH_LO] = 6;
	frame[MODBUS_TCP_FUNCTION] = function_code;
	frame[MODBUS_TCP_START_ADDR_HI] = hi(starting_address);
	frame[MODBUS_TCP_START_ADDR_LO] = lo(starting_address);
	frame[MODBUS_TCP_QUANTITY_HI] = hi(quantity);
	frame[MODBUS_TCP_QUANTITY_LO] = lo(quantity);
	stack_data.len = 12;
}

void ModbusTCP::SendErrorFrame(uint8_t* frame, uint8_t error_code)
{
	PrepareMBAPHeader(frame);
	frame[MODBUS_TCP_LENGTH_HI] = 0;
	frame[MODBUS_TCP_LENGTH_LO] = 3;
	frame[MODBUS_TCP_FUNCTION] = function_code + 0x80;
	frame[MODBUS_RES_TCP_BYTE_COUNT] = error_code;
	stack_data.len = 9;
}

void ModbusTCP::AnalizeMultipleRegisters()
{
	// ---------------- tutaj dziala polimorfizm ----------------
	// testowo, dla pokazania idei
	if(MultipleRegisters[MULTIPLE_LOCATIONS_NUMBER] > 0) m->StartupTest();
}
