/*
 * modbus_rtu.c
 *
 *  Created on: 27 cze 2017
 *      Author: tomek
 */

#include "modbus_rtu.h"
#include "usart.h"
#include "display.h"
#include "modbus_tcp.h"

ModbusRTU::ModbusRTU()
{
	slave_addr = 1;
	slave_addr_max = 10;
}

void ModbusRTU::PrepareFrameHolding(uint8_t* frame, uint8_t address, uint8_t function)
{
	frame[MODBUS_RTU_ADDR_ID] = address;
	frame[MODBUS_RTU_FUNCTION_ID] = MODBUS_RTU_HOLDING_FUNCTION;
	frame[MODBUS_RTU_REQ_HOLDING_STARTING_ADDR_HI] = 0;
	frame[MODBUS_RTU_REQ_HOLDING_STARTING_ADDR_LO] = 0;
	frame[MODBUS_RTU_REQ_HOLDING_QUANTITY_HI] = 0;
	frame[MODBUS_RTU_REQ_HOLDING_QUANTITY_LO] = 1;
}

void ModbusRTU::PrepareFrameSingle(uint8_t* frame, uint8_t address, uint8_t function, uint16_t value)
{
	frame[MODBUS_RTU_ADDR_ID] = address;
	frame[MODBUS_RTU_FUNCTION_ID] = MODBUS_RTU_SINGLE_FUNCTION;
	frame[MODBUS_RTU_SINGLE_REG_ADDR_HI] = 0;
	frame[MODBUS_RTU_SINGLE_REG_ADDR_LO] = 0;
	frame[MODBUS_RTU_SINGLE_REG_VAL_HI] = value >> 8;
	frame[MODBUS_RTU_SINGLE_REG_VAL_LO] = value & 0xFF;
}

void ModbusRTU::PrepareFrameCRC(uint8_t* frame)
{
	uint16_t crc = Checksum(frame, MODBUS_RTU_REQ_LEN - 2);
	frame[MODBUS_RTU_REQ_CRC_LO] = crc & 0xFF;
	frame[MODBUS_RTU_REQ_CRC_HI] = crc >> 8;
}

void ModbusRTU::PollDoors()
{
	PrepareFrameHolding(usart_data.frame, slave_addr, MODBUS_RTU_HOLDING_FUNCTION);
	PrepareFrameCRC(usart_data.frame);
	usart_data.len = MODBUS_RTU_REQ_LEN;
	usart.SendFrame(&usart_data);
	slave_addr++;
	if(slave_addr == slave_addr_max + 1) slave_addr = 1;
}

void ModbusRTU::ParseFrame(uint8_t* frame, uint8_t len)
{
	uint16_t crc = Checksum(frame, len - 2);
	if((frame[MODBUS_RTU_ADDR_ID] == (slave_addr - 1)) &&
			((uint8_t) crc == frame[len - 2]) &&
			((uint8_t) (crc >> 8)) == frame[len - 1])
	{
		switch(frame[MODBUS_RTU_FUNCTION_ID])
		{
		case MODBUS_RTU_HOLDING_FUNCTION:
			ReadHolding(frame);
		break;
		default:
		break;
		}
	}
}

void ModbusRTU::ReadHolding(uint8_t* frame)
{
	uint8_t address = MULTIPLE_LOCATIONS_NUMBER + slave_addr - 1;
	uint16_t value = (frame[MODBUS_RTU_RES_HOLDING_REG_VAL_HI] << 8) | frame[MODBUS_RTU_RES_HOLDING_REG_VAL_LO];
	// update master's holding registers
	modbus_tcp.UpdateMultiple(address, value);
	// send command to led module
	PrepareFrameSingle(frame, MODBUS_RTU_LED_OFFSET + slave_addr - 1, MODBUS_RTU_SINGLE_FUNCTION, 5);
	PrepareFrameCRC(usart_data.frame);
	usart_data.len = MODBUS_RTU_REQ_LEN;
	usart.SendFrame(&usart_data);
}
/*
void ModbusRTU::WriteSingle(uint8_t* frame)
{
	frame[0] = slave_addr - 1 + 100;
	frame[1] = 6; 		// function
	frame[2] = 0;		// reg addr hi
	frame[3] = 0;		// reg addr lo
	frame[4] = 0;		// val hi
	if(modbus_tcp.HoldingRegisters[1 + slave_addr - 1] == 5)
	{
		frame[5] = 25;			// val lo
	}
	else
	{
		frame[5] = 200;
	}
	uint16_t crc = Checksum(frame, 6);
	frame[6] = crc & 0xFF;
	frame[7] = crc >> 8;
	usart_data.len = 8;
	usart.SendFrame(&usart_data);
}
*/
uint16_t ModbusRTU::Checksum(uint8_t *frame, uint8_t len)
{
	uint16_t crc = 0xFFFF;
	int pos = 0;
	for (pos = 0; pos < len; pos++)
	{
		crc ^= (uint16_t)frame[pos];
		int i = 0;
		for (i = 8; i != 0; i--)
		{
			if ((crc & 0x0001) != 0)
			{
				crc >>= 1;
				crc ^= 0xA001;
			}
			else
			crc >>= 1;
		}
	}
	return crc;
}
