/*
 * modbus_rtu.c
 *
 *  Created on: 27 cze 2017
 *      Author: tomek
 */

#include "modbus_rtu.h"
#include "usart.h"
#include "display.h"

ModbusRTU::ModbusRTU()
{
	SlaveAddr = 5;
	HoldingRegisters[0] = 45;
	HoldingRegisters[1] = 23;
}

void ModbusRTU::Poll()
{
	static uint16_t addr = 1;
	display.Write(addr);
	usart_data.frame[0] = (uint8_t)addr++;
	usart_data.frame[1] = 3;
	usart_data.frame[2] = 0;
	usart_data.frame[3] = 0;
	usart_data.frame[4] = 0;
	usart_data.frame[5] = 1;
	uint16_t crc = Checksum(usart_data.frame, 6);
	usart_data.frame[6] = crc & 0xFF;
	usart_data.frame[7] = crc >> 8;
	usart_data.len = 8;
	usart.SendFrame(&usart_data);
	if(addr == 10) addr = 1;
}

// to jest do kitu, trzeba podzielic na 2 funkcje tak jak w slave!!!!!!!!!!!!!!!!!!!!!!!
void ModbusRTU::ParseFrame(uint8_t* frame, uint8_t len)
{
	uint16_t crc = Checksum(frame, len - 2);
	if((frame[0] == SlaveAddr) && ((uint8_t) crc == frame[len - 2]) && ((uint8_t) (crc >> 8)) == frame[len - 1])
	{
		switch(frame[1])
		{
			case 3:
				ReadHoldingRegisters(frame);
			break;
			default:
				FunctionNotSupported(frame);
		}
	}
}

void ModbusRTU::ReadHoldingRegisters(uint8_t* frame)
{
	uint16_t starting_address = frame[2] << 8 | frame[3];
	uint16_t quantity 		  = frame[4] << 8 | frame[5];
	uint8_t error_code = 0;
	uint8_t byte_count;

	if((quantity > 125) || (quantity < 1)) error_code = 3;
	if((starting_address + quantity) > NUMBER_OF_HOLDING_REGISTERS) error_code = 2;

	if(error_code)
	{
		frame[0] = SlaveAddr;
		frame[1] = frame[1] + 0x80;
		frame[2] = error_code;
		uint16_t crc = Checksum(frame, 3);
		frame[3] = (uint8_t) crc;
		frame[4] = (uint8_t) (crc >> 8);
		usart_data.len = 5;
		usart.SendFrame(&usart_data);
		return;
	}

	byte_count = (uint8_t)(quantity * 2);
	frame[0] = SlaveAddr;
	//frame[1] =
	frame[2] = byte_count;

	for(uint8_t i = 0; i < quantity; i++)
	{
		frame[3 + 2 * i] 	 = (uint8_t)(HoldingRegisters[starting_address + i] >> 8);
		frame[3 + 2 * i + 1] = (uint8_t)(HoldingRegisters[starting_address + i]);
	}
	uint16_t crc = Checksum(frame, (3 + byte_count));
	frame[3 + byte_count] = (uint8_t) crc;
	frame[4 + byte_count] = (uint8_t) (crc >> 8);
	usart_data.len = 5 + byte_count;
	usart.SendFrame(&usart_data);
}

void ModbusRTU::FunctionNotSupported(uint8_t *frame)
{
	frame[0] = SlaveAddr;
	frame[1] = frame[1] + 0x80;
	frame[2] = 1; 									// Illegal function
	uint16_t crc = Checksum(frame, 3);
	frame[3] = (uint8_t) crc;
	frame[4] = (uint8_t) (crc >> 8);
	usart_data.len = 5;
	usart.SendFrame(&usart_data);
}

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
