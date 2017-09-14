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
#include "machine.h"
#include "display.h"

Comm_prot::Comm_prot()
{

}

void Comm_prot::Prepare(DestType t, uint8_t addr, uint8_t command)
{
	if(t == TLed) addr += LED_ADDRESS_OFFSET;
	usart_data.frame[0] = addr;
	usart_data.frame[1] = command;
	usart_data.frame[2] = Crc8(usart_data.frame, 2);
	usart_data.frame[3] = 0x0A;
	usart_data.len = FRAME_LENGTH_REQUEST;
	usart.SendFrame(&usart_data);
}

void Comm_prot::Parse(uint8_t* frame)
{
	uint8_t crc = Crc8(frame, 2);
	// reply from door
	if((frame[0] == m->curr_door - 1) && (frame[2] == crc))
	{
		uint8_t result = frame[1];
		switch(curr_command)
		{
		case COMM_CHECK_ELECTROMAGNET:
			if(result == 0x00)
			{
				modbus_tcp.UpdateHoldingRegisters(m->curr_door, NO_FAULT << 8);
				Prepare(TLed, m->curr_door - 1, COMM_GREEN_ON);
			}
			if(result == 0x01)
			{
				display.Write(TFault, F05_ELECTROMAGNET_FAULT);
				modbus_tcp.UpdateHoldingRegisters(m->curr_door, F05_ELECTROMAGNET_FAULT << 8);
				Prepare(TLed, m->curr_door - 1, COMM_RED_3PULSES);
			}
			if(m->curr_door == m->last_door + 1) { Prepare(TLed); }
		break;
		timer.Disable(TIMER_REPLY_TIMEOUT);
		}
	}
	// reply from led
	if((frame[0] == m->curr_door - 1 + LED_ADDRESS_OFFSET) && (frame[2] == crc))
	{
		uint8_t result = frame[1];
		if(result == 0x80)
		{
			modbus_tcp.UpdateHoldingRegisters(GENERAL_ERROR_STATUS, NO_FAULT);
			modbus_tcp.UpdateHoldingRegisters(m->curr_door, NO_FAULT << 8);
		}
		timer.Disable(TIMER_REPLY_TIMEOUT);
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
