/*
 * modbus_rtu.c
 *
 *  Created on: 27 cze 2017
 *      Author: tomek
 */

#include "comm_prot.h"

#include "usart.h"
#include "modbus_tcp.h"

Comm_prot::Comm_prot()
{
	slave_addr = 1;
}

void Comm_prot::Parse(uint8_t* frame)
{
	uint8_t crc = Crc8(frame, 2);
	if((frame[0] == slave_addr) && (frame[2] == crc))
	{
		switch(frame[1])
		{
		case 0x01:

		break;
		case 0x02:

		break;
		case 0x03:

		break;

		}
	}
}

void Comm_prot::Prepare(uint8_t res)
{
	usart_data.frame[0] = slave_addr;
	usart_data.frame[1] = res;
	usart_data.frame[2] = Crc8(usart_data.frame, 2);
	usart_data.frame[3] = 0x0A;
	usart_data.len = FRAME_LENGTH;
	usart.SendFrame(&usart_data);
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
