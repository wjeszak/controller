/*
 * comm.c
 *
 *  Created on: 27 cze 2017
 *      Author: tomek
 */

#include "comm.h"

#include "dynabox_commands_faults.h"
#include "timer.h"
#include "usart.h"
#include "machine.h"

Comm::Comm()
{

}

void Comm::EV_Send(uint8_t addr, uint8_t command, bool need_timeout)
{
	usart_data.frame[0] = addr;
	usart_data.frame[1] = command;
	usart_data.frame[2] = Crc(usart_data.frame, 2);
	usart_data.frame[3] = USART_FRAME_END_CHAR;
	usart_data.len = FRAME_LENGTH_REQUEST;
	usart.SendFrame(&usart_data);
	if(need_timeout) SLAVE_POLL_TIMEOUT_SET;
}

void Comm::EV_LedTrigger()
{
	EV_Send(LED_ADDRESS_TRIGGER, 0x00, false);
}

void Comm::EV_Reply()
{
	SLAVE_POLL_TIMEOUT_OFF;
	if(Crc(usart_data.frame, 2) == usart_data.frame[2])
	{
		d->addr = usart_data.frame[0];
		d->data = usart_data.frame[1];
		m->EV_ReplyOK(d);
	}
}

void Comm::EV_Timeout()
{
	SLAVE_POLL_TIMEOUT_OFF;
	d->addr = usart_data.frame[0];
	m->EV_Timeout(d);
}

uint8_t Comm::Crc(uint8_t* frame, uint8_t len)
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
