/*
 * modbus_rtu.c
 *
 *  Created on: 27 cze 2017
 *      Author: tomek
 */

#include "comm_prot.h"
#include "timer.h"
#include "usart.h"
#include "modbus_tcp.h"

Comm_prot::Comm_prot()
{
	addr_start = 1;
	addr_stop = 13;
	addr_curr = 1;
}

void Comm_prot::Prepare(uint8_t addr, uint8_t command)
{
	usart_data.frame[0] = addr;
	usart_data.frame[1] = command;
	usart_data.frame[2] = Crc8(usart_data.frame, 2);
	usart_data.frame[3] = 0x0A;
	usart_data.len = FRAME_LENGTH;
	usart.SendFrame(&usart_data);
}

void Comm_prot::Poll()
{
	Prepare(addr_curr++, 0x01);
	if(addr_curr == addr_stop + 1) timer.Disable(TIMER_DOORS_POLL);//addr_curr = addr_start;
}

void Comm_prot::Parse(uint8_t* frame)
{
	uint8_t crc = Crc8(frame, 2);
	if((frame[0] == addr_curr - 1) && (frame[2] == crc))
	{
		switch(frame[1])
		{
		case 0x00:
			modbus_tcp.UpdateHoldingRegisters(addr_curr, 0x05 << 8);
		break;
		case 0x01:
			modbus_tcp.UpdateHoldingRegisters(addr_curr, 0);
		break;
		case 0x02:

		break;
		case 0x03:

		break;

		}
	}
}

uint8_t Comm_prot::Crc8(uint8_t *frame, uint8_t len)
{
	uint8_t crc = 0x00;
	for (uint8_t pos = 0; pos < len; pos++)
	{
		crc ^= frame[pos];
		for (uint8_t i = 8; i != 0; i--)
		{
			if ((crc & 0x80) != 0)
			{
				crc <<= 1;
				crc ^= 0x07;
			}
			else
				crc <<= 1;
		}
	}
	return crc;
}
