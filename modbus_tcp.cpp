/*
 * modbus_tcp.cpp
 *
 *  Created on: 28 cze 2017
 *      Author: tomek
 */

#include "modbus_tcp.h"
#include "display.h"
#include "stack.h"

void ModbusTcp::ParseFrame(uint8_t* frame)
{
	// Modbus TCP frame ?
	if((frame[PROTOCOL_ID_H] == 0) && (frame[PROTOCOL_ID_L] == 0))
	{
		switch(frame[FUNCTION_CODE])
		{
			case 3:
				if(ReadHoldingRegisters(frame) == 0)
					PrepareFrame(frame);
			break;
			//default:
				//FunctionNotSupported(frame);
		}
	}
}

uint8_t ModbusTcp::ReadHoldingRegisters(uint8_t* frame)
{
	trans_id 		 = frame[TRANSACTION_ID_H] << 8 | frame[TRANSACTION_ID_L];
	unit_id 		 = frame[UNIT_ID];
	function_code 	 = frame[FUNCTION_CODE];
	starting_address = frame[START_ADDR_H] << 8 | frame[START_ADDR_L];
	quantity 		 = frame[QUANTITY_H] << 8 | frame[QUANTITY_L];
	length = (quantity * 2) + UNIT_ID_FUNCTION_CODE_BYTE_COUNT_LEN;
	uint8_t error_code = 0;

	if((quantity > 125) || (quantity < 1)) error_code = 3;
	if((starting_address + quantity - ADDR_OFFSET) > NUMBER_OF_HOLDING_REGISTERS_TCP) error_code = 2;
	if(starting_address < ADDR_OFFSET) error_code = 2;
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
void ModbusTcp::PrepareFrame(uint8_t* frame)
{
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
		frame[DATA + 2 * i] 	  = (uint8_t)(HoldingRegisters[starting_address - ADDR_OFFSET + i] >> 8);
		frame[DATA + (2 * i) + 1] = (uint8_t)(HoldingRegisters[starting_address - ADDR_OFFSET + i]);
	}
	stack_data.len = MBAP_FUNCTION_CODE_BYTE_COUNT_LEN + (quantity * 2);
}

