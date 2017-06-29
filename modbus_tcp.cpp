/*
 * modbus_tcp.cpp
 *
 *  Created on: 28 cze 2017
 *      Author: tomek
 */

#include "modbus_tcp.h"
#include "display.h"
#include "stack.h"
#include "motor.h"
ModbusTCP::ModbusTCP()
{
	HoldingRegisters[12] = 45;
}

void ModbusTCP::Process(uint8_t* frame)
{
	trans_id 		 = frame[TRANSACTION_ID_H] << 8 | frame[TRANSACTION_ID_L];
	prot_id 		 = frame[PROTOCOL_ID_H] << 8 | frame[PROTOCOL_ID_L];
	unit_id 		 = frame[UNIT_ID];
	function_code 	 = frame[FUNCTION_CODE];
	starting_address = frame[START_ADDR_H] << 8 | frame[START_ADDR_L];
	quantity 		 = frame[QUANTITY_H] << 8 | frame[QUANTITY_L];
	// ModbusTCP frame ?
	if(prot_id == 0)
	{
		switch(function_code)
		{
			case FUNCTION_CODE_READ_HOLDING:
				if(ReadHoldingRegisters(frame) == 0)
					PrepareFrame(frame);
			break;

			case FUNCTION_CODE_WRITE_MULTIPLE:
				WriteMultipleRegisters(frame);
			break;

			default:
			//FunctionNotSupported(frame);
			break;

		}
	}
}

uint8_t ModbusTCP::ReadHoldingRegisters(uint8_t* frame)
{
	uint8_t error_code = 0;

	if((quantity > 125) || (quantity < 1)) error_code = 3;
	if((starting_address + quantity - ADDR_OFFSET_READ_HOLDING_REG) > NUMBER_OF_HOLDING_REG_TCP) error_code = 2;
	if(starting_address < ADDR_OFFSET_READ_HOLDING_REG) error_code = 2;
	if(error_code)
	{
		frame[TRANSACTION_ID_H] = (trans_id >> 8);
		frame[TRANSACTION_ID_L] = (trans_id & 0xFF);
		//frame[PROT_ID_H] = 0;
		//frame[PROT_ID_L] = 0;
		frame[LENGTH_H] = 0;
		frame[LENGTH_L] = 3;
		frame[UNIT_ID] = unit_id;
		frame[FUNCTION_CODE] = function_code + 0x80;
		frame[BYTE_COUNT] = error_code;
		stack_data.len = 9;
		return error_code;
	}
	return 0;
}

uint8_t ModbusTCP::WriteMultipleRegisters(uint8_t* frame)
{
	uint8_t error_code = 0;

	if((quantity > 123) || (quantity < 1)) error_code = 3;
	if((starting_address + quantity - ADDR_OFFSET_WRITE_MULTIPLE_REG) > NUMBER_OF_WRITE_MULTIPLE_REG_TCP) error_code = 2;
	if(starting_address < ADDR_OFFSET_WRITE_MULTIPLE_REG) error_code = 2;

	if(error_code)
	{
		frame[TRANSACTION_ID_H] = (trans_id >> 8);
		frame[TRANSACTION_ID_L] = (trans_id & 0xFF);
		//frame[PROT_ID_H] = 0;
		//frame[PROT_ID_L] = 0;
		frame[LENGTH_H] = 0;
		frame[LENGTH_L] = 3;
		frame[UNIT_ID] = unit_id;
		frame[FUNCTION_CODE] = function_code + 0x80;
		frame[BYTE_COUNT] = error_code;
		stack_data.len = 9;
		return error_code;
	}
	else
	{
		for(uint8_t i = 0; i < (uint8_t)quantity; i++)
		{
			MultipleRegisters[starting_address - ADDR_OFFSET_WRITE_MULTIPLE_REG + i] = (frame[13 + 2 * i] << 8) | frame[14 + 2 * i];
		}
		frame[TRANSACTION_ID_H] = (trans_id >> 8);
		frame[TRANSACTION_ID_L] = (trans_id & 0xFF);
		//frame[PROT_ID_H] = 0;
		//frame[PROT_ID_L] = 0;
		frame[LENGTH_H] = 0;
		frame[LENGTH_L] = 6;
		frame[UNIT_ID] = unit_id;
		frame[FUNCTION_CODE] = function_code;
		frame[START_ADDR_H] = (starting_address >> 8);
		frame[START_ADDR_L] = (starting_address & 0xFF);
		frame[QUANTITY_H] = (quantity >> 8);
		frame[QUANTITY_L] = (quantity & 0xFF);
		stack_data.len = 12;
		display.Write(MultipleRegisters[3]);
		if(MultipleRegisters[0] == 1) motor.Test();
		//motor.Enable(Forward, MultipleRegisters[3]);
	}
	return 0;
}

// for ReadHoldingRegisters
void ModbusTCP::PrepareFrame(uint8_t* frame)
{
	length = (quantity * 2) + UNIT_ID_FUNCTION_CODE_BYTE_COUNT_LEN;
	frame[TRANSACTION_ID_H] = (trans_id >> 8);
	frame[TRANSACTION_ID_L] = (trans_id & 0xFF);
	//frame[PROT_ID_H] = 0;
	//frame[PROT_ID_L] = 0;
	frame[LENGTH_H] = (length >> 8);
	frame[LENGTH_L] = (length & 0xFF);
	frame[UNIT_ID] = unit_id;
	frame[FUNCTION_CODE] = function_code;
	frame[BYTE_COUNT] = ((quantity * 2) & 0xFF);

	for(uint8_t i = 0; i < (uint8_t)quantity; i++)
	{
		frame[DATA + 2 * i] 	  = (uint8_t)(HoldingRegisters[starting_address - ADDR_OFFSET_READ_HOLDING_REG + i] >> 8);
		frame[DATA + (2 * i) + 1] = (uint8_t)(HoldingRegisters[starting_address - ADDR_OFFSET_READ_HOLDING_REG + i]);
	}
	stack_data.len = MBAP_FUNCTION_CODE_BYTE_COUNT_LEN + (quantity * 2);
}

