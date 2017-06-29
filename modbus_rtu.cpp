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

void ModbusRTU::Poll()
{
	display.Write(slave_addr);
	usart_data.frame[0] = slave_addr++;
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
	if(slave_addr == slave_addr_max) slave_addr = 1;
}

void ModbusRTU::ParseFrame(uint8_t* frame, uint8_t len)
{
	uint16_t crc = Checksum(frame, len - 2);
	if((frame[0] == (slave_addr - 1)) && ((uint8_t) crc == frame[len - 2]) && ((uint8_t) (crc >> 8)) == frame[len - 1])
	{
		//display.Write(4444);
		switch(frame[1])
		{
			case 3:
				ReadHoldingRegisters(frame);
			break;
			default:
			break;
				//FunctionNotSupported(frame);
		}
	}
}

void ModbusRTU::ReadHoldingRegisters(uint8_t* frame)
{
	//uint8_t byte_count = frame[2];
	// obsluga bledow !!!!!!!!!!!!
	modbus_tcp.HoldingRegisters[slave_addr - 1] = (frame[3] << 8) | frame[4];
}

/*
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
