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

void ModbusTCP::PrepareMBAPHeader(uint8_t* frame)	// without length
{
	frame[MODBUS_TCP_TRANSACTION_ID_HI] = hi(trans_id);
	frame[MODBUS_TCP_TRANSACTION_ID_LO] = lo(trans_id);
	// always 0 for ModbusTCP
	frame[MODBUS_TCP_PROTOCOL_ID_HI] = 0;
	frame[MODBUS_TCP_PROTOCOL_ID_LO] = 0;
	frame[MODBUS_TCP_UNIT_ID] = unit_id;
}

// for ReadHoldingRegisters
void ModbusTCP::ReadHoldingRegistersReply(uint8_t* frame)
{
	PrepareMBAPHeader(frame);
	length = (quantity * 2) + UNIT_ID_FUNCTION_BYTE_COUNT_LEN;

	frame[MODBUS_TCP_LENGTH_HI] = hi(length);
	frame[MODBUS_TCP_LENGTH_LO] = lo(length);
	frame[MODBUS_TCP_FUNCTION]  = function_code;
	frame[MODBUS_RES_TCP_BYTE_COUNT] = lo(quantity * 2);

	ReturnHoldingRegisters(frame, starting_address, quantity);
	stack_data.len = MBAP_FUNCTION_BYTE_COUNT_LEN + (quantity * 2);
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


		frame[LENGTH_H] = 0;
		frame[LENGTH_L] = 6;

		frame[FUNCTION_CODE] = function_code;
		frame[START_ADDR_H] = (starting_address >> 8);
		frame[START_ADDR_L] = (starting_address & 0xFF);
		frame[QUANTITY_H] = (quantity >> 8);
		frame[QUANTITY_L] = (quantity & 0xFF);
		stack_data.len = 12;
		for(uint8_t i = 2; i < NUMBER_OF_MULTIPLE_REG_TCP; i ++)
		{
			if(MultipleRegisters[i] != 0)
			{
				// to juz jest TOTALNIE na chama
				usart_data.frame[0] = i - 1;
				usart_data.frame[1] = 6; 		// function
				usart_data.frame[2] = 0;		// reg addr hi
				usart_data.frame[3] = 0;		// reg addr lo
				usart_data.frame[4] = MultipleRegisters[i] >> 8;		// val hi
				usart_data.frame[5] = MultipleRegisters[i] & 0xFF;
				uint16_t crc = modbus_rtu.Checksum(usart_data.frame, 6);
				usart_data.frame[6] = crc & 0xFF;
				usart_data.frame[7] = crc >> 8;
				usart_data.len = 8;
				usart.SendFrame(&usart_data);
				break;
			}
		}

		if(MultipleRegisters[0] > 0) m->StartupTest();
		//if(MultipleRegisters[0] > 0) motor.EV_Homing();		// homing

		if(MultipleRegisters[1] > 0)
		{
			motor_data.pos = MultipleRegisters[1] * 100;
			motor.EV_RunToPosition(&motor_data);
		}
		//if(MultipleRegisters[1] > 0) //timer.Assign(2, 500, ModbusPoll);
		//if(MultipleRegisters[1] == 0) timer.Disable(2);
	}
	return 0;
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

void ModbusTCP::UpdateHoldingRegisters(uint8_t address, uint16_t value)
{
	HoldingRegisters[address] = value;
}

void ModbusTCP::UpdateMultipleRegisters(uint8_t address, uint16_t value)
{
	MultipleRegisters[address - MODBUS_TCP_ADDR_OFFSET_MULTIPLE_REG] = (frame[13 + 2 * i] << 8) | frame[14 + 2 * i];
}

void ModbusTCP::ReturnHoldingRegisters(uint8_t* frame, uint8_t starting_address, uint16_t quantity)
{
	for(uint8_t i = 0; i < quantity; i++)
	{
		frame[MODBUS_RES_TCP_DATA + 2 * i]       = hi(HoldingRegisters[starting_address - MODBUS_TCP_ADDR_OFFSET_HOLDING_REG + i]);
		frame[MODBUS_RES_TCP_DATA + (2 * i) + 1] = lo(HoldingRegisters[starting_address - MODBUS_TCP_ADDR_OFFSET_HOLDING_REG + i]);
	}
}

void ModbusTCP::WriteMultipleRegistersReply(uint8_t *frame)
{
	PrepareMBAPHeader(frame);
}
