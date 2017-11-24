/*
 * modbus_tcp.cpp
 *
 * Created on: 28 cze 2017
 * Author: tomek
 */

#include "modbus_tcp.h"
#include "stack.h"
#include "common.h"
#include "dynabox.h"
#include "display.h"

ModbusTCP::ModbusTCP()
{

}

void ModbusTCP::Process(uint8_t* frame)
{
	trans_id 		 = hi(frame[MODBUS_TCP_TRANSACTION_ID_HI]) | frame[MODBUS_TCP_TRANSACTION_ID_LO];
	prot_id 		 = hi(frame[MODBUS_TCP_PROTOCOL_ID_HI])    | frame[MODBUS_TCP_PROTOCOL_ID_LO];
	unit_id 		 = frame[MODBUS_TCP_UNIT_ID];
	function_code 	 = frame[MODBUS_TCP_FUNCTION];
	starting_address = hi(frame[MODBUS_TCP_START_ADDR_HI])     | frame[MODBUS_TCP_START_ADDR_LO];
	quantity 		 = hi(frame[MODBUS_TCP_QUANTITY_HI])       | frame[MODBUS_TCP_QUANTITY_LO];

	if(prot_id == PROTOCOL_ID_MODBUS_TCP)
	{
		switch(function_code)
		{
		case MODBUS_TCP_HOLDING_FUNCTION:
			Read(frame);
			break;
		case MODBUS_TCP_WRITE_MULTIPLE_FUNCTION:
			Write(frame);
			break;
		default:
			//FunctionNotSupported(frame);
			break;
		}
	}
}

uint16_t ModbusTCP::Read(uint8_t address)
{
	return Registers[address];
}

void ModbusTCP::Write(uint8_t address, uint16_t value)
{
	Registers[address] = value;
}

void ModbusTCP::Read(uint8_t* frame)
{
	uint8_t error_code = 0;

	if((quantity > MODBUS_TCP_HOLDING_MAX_QUANTITY) || (quantity < 1)) error_code = MODBUS_TCP_ERROR_ILL_DATA_VAL;
	if((starting_address + quantity - MODBUS_TCP_ADDR_OFFSET) > MODBUS_TCP_NUMBER_OF_REG) error_code = MODBUS_TCP_ERROR_ILL_DATA_ADDR;
	if(starting_address < MODBUS_TCP_ADDR_OFFSET) error_code = MODBUS_TCP_ERROR_ILL_DATA_ADDR;
	if(error_code)
	{
		ErrorReply(frame, error_code);
	}
	else
	{
		ReadReply(frame);
	}
}

void ModbusTCP::Write(uint8_t* frame)
{
	uint8_t error_code = 0;

	if((quantity > MODBUS_TCP_MULTIPLE_MAX_QUANTITY) || (quantity < 1)) error_code = MODBUS_TCP_ERROR_ILL_DATA_VAL;
	if((starting_address + quantity - MODBUS_TCP_ADDR_OFFSET) > MODBUS_TCP_NUMBER_OF_REG) error_code = MODBUS_TCP_ERROR_ILL_DATA_ADDR;
	if(starting_address < MODBUS_TCP_ADDR_OFFSET) error_code = MODBUS_TCP_ERROR_ILL_DATA_ADDR;
	if(error_code)
	{
		ErrorReply(frame, error_code);
	}
	else
	{
		WriteReply(frame);
		// ----------------------------- User action ----------------------------------
		m->EV_UserAction(d);
		//display.Write(5555);
		// ----------------------------- User action ----------------------------------
	}
}

void ModbusTCP::SetBit(uint8_t address, uint8_t bit)
{
	Registers[address] |= (1 << bit);
}

void ModbusTCP::ClearBit(uint8_t address, uint8_t bit)
{
	Registers[address] &= ~(1 << bit);
}

void ModbusTCP::MakeMBAPHeader(uint8_t* frame)	// without length
{
	frame[MODBUS_TCP_TRANSACTION_ID_HI] = hi(trans_id);
	frame[MODBUS_TCP_TRANSACTION_ID_LO] = lo(trans_id);
	frame[MODBUS_TCP_PROTOCOL_ID_HI] = 0;						// 0 for ModbusTCP
	frame[MODBUS_TCP_PROTOCOL_ID_LO] = 0;						// 0 for ModbusTCP
	frame[MODBUS_TCP_UNIT_ID] = unit_id;
}

void ModbusTCP::ReadReply(uint8_t* frame)
{
	MakeMBAPHeader(frame);
	uint16_t length = (quantity * 2) + UNIT_ID_FUNCTION_BYTE_COUNT_LEN;
	frame[MODBUS_TCP_LENGTH_HI] = hi(length);
	frame[MODBUS_TCP_LENGTH_LO] = lo(length);
	frame[MODBUS_TCP_FUNCTION]  = function_code;
	frame[MODBUS_RES_TCP_BYTE_COUNT] = lo(quantity * 2);

	for(uint8_t i = 0; i < quantity; i++)
	{
		frame[MODBUS_RES_TCP_DATA + 2 * i]       = hi(Registers[starting_address - MODBUS_TCP_ADDR_OFFSET + i]);
		frame[MODBUS_RES_TCP_DATA + (2 * i) + 1] = lo(Registers[starting_address - MODBUS_TCP_ADDR_OFFSET + i]);
	}
	stack_data.len = MBAP_FUNCTION_BYTE_COUNT_LEN + (quantity * 2);
}

void ModbusTCP::WriteReply(uint8_t *frame)
{
	MakeMBAPHeader(frame);
	frame[MODBUS_TCP_LENGTH_HI] = 0;
	frame[MODBUS_TCP_LENGTH_LO] = 6;
	frame[MODBUS_TCP_FUNCTION] = function_code;
	frame[MODBUS_TCP_START_ADDR_HI] = hi(starting_address);
	frame[MODBUS_TCP_START_ADDR_LO] = lo(starting_address);
	frame[MODBUS_TCP_QUANTITY_HI] = hi(quantity);
	frame[MODBUS_TCP_QUANTITY_LO] = lo(quantity);

	for(uint8_t i = 0; i < quantity; i++)
	{
		Registers[starting_address - MODBUS_TCP_ADDR_OFFSET + i] = (hi(frame[MODBUS_REQ_TCP_REG_VAL_HI + 2 * i])) | (lo(frame[MODBUS_REQ_TCP_REG_VAL_LO + 2 * i]));
	}
	stack_data.len = 12;
}

void ModbusTCP::ErrorReply(uint8_t* frame, uint8_t error_code)
{
	MakeMBAPHeader(frame);
	frame[MODBUS_TCP_LENGTH_HI] = 0;
	frame[MODBUS_TCP_LENGTH_LO] = 3;
	frame[MODBUS_TCP_FUNCTION] = function_code + 0x80;
	frame[MODBUS_RES_TCP_BYTE_COUNT] = error_code;
	stack_data.len = 9;
}

//void ModbusTCP::AnalizeMultipleRegisters()
//{

	//for(uint8_t i = 52; i < 65; i++)
	//{
//		if(Registers[52] > 0) comm.Prepare(TDoor, 1, Registers[52] + 0x80);
	//}
	//	if(Registers[0] > 0) m->SetFault(Registers[0]);
	//	if(Registers[1] > 0) m->ClearFault(Registers[1]);
//}
