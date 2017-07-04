/*
 * usart.cpp
 *
 *  Created on: 26 maj 2017
 *      Author: tomek
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "machine_type.h"
#include "usart.h"
#include "motor.h"
#include "machine.h"
#include "display.h"
#include "timer.h"
#include "modbus_rtu.h"

Usart::Usart(uint16_t baud) : Machine(ST_MAX_STATES)
{
	uint8_t ubrr = F_CPU / 16 / baud - 1;
	UBRR0H = (uint8_t)(ubrr >> 8);
	UBRR0L = (uint8_t)ubrr;

	UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << TXCIE0);
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);

	USART_DE_INIT;
	rx_head = 0;
	rx_tail = 0;
	tx_head = 0;
	tx_tail = 0;
	RxEnable();
	timer2.Assign(1, 2, ModbusRTU35T);
}

void Usart::RxEnable()
{
	USART_DE_RECEIVE;
	UCSR0B |= (1 << RXEN0) | (1 << RXCIE0);
}

void Usart::RxDisable()
{
	UCSR0B &= ~((1 << RXEN0) | (1 << RXCIE0));
}

void Usart::TxEnable()
{
	USART_DE_SEND;
	UCSR0B |= (1 << UDRIE0);
}

void Usart::TxDisable()
{
	UCSR0B &= ~(1 << UDRIE0);
}

void Usart::ST_Init(UsartData* pdata) {}

void Usart::ST_Idle(UsartData* pdata)
{
//	display.Write(GetState());
	timer2.Disable(1);
}

void Usart::ST_ByteReceived(UsartData* pdata)
{
//	display.Write(GetState());
	uint8_t tmp_head;
	tmp_head = (rx_head + 1) & UART_RX_BUF_MASK;
	if(tmp_head == rx_tail)
	{
		// nadpisanie bufora
	}
	else
	{
		rx_head = tmp_head;
		buf_rx[tmp_head] = pdata->c;
	}
	timer2.Enable(1);
}

void Usart::ST_FrameReceived(UsartData* pdata)
{
	usart_data.len = 7;
	uint8_t i = 0;
	while(rx_tail != rx_head)
	{
		rx_tail = (rx_tail + 1) & UART_RX_BUF_MASK;
		usart_data.frame[i] = buf_rx[rx_tail];
		i++;
	}
//	display.Write(GetState());
	modbus_rtu.ParseFrame(usart_data.frame, 7);
	timer2.Disable(1);
}

void Usart::CharReceived(UsartData* pdata)
{
	const uint8_t Transitions[] =
	{
		// musi byc obsluga jesli znak przyjdzie w stanie INIT
		ST_BYTE_RECEIVED,			// ST_INIT
		ST_BYTE_RECEIVED, 			// ST_IDLE
		ST_BYTE_RECEIVED,			// ST_BYTE_RECEIVED
		ST_BYTE_RECEIVED			// ST_FRAME_RECEIVED
	};
	Event(Transitions[current_state], pdata);
}

void Usart::RTU35T(UsartData* pdata)
{
	const uint8_t Transitions[] =
	{
		ST_IDLE,						// ST_INIT
		ST_NOT_ALLOWED, 				// ST_IDLE
		ST_FRAME_RECEIVED				// ST_BYTE_RECEIVED
	};
	Event(Transitions[current_state], pdata);
}

void Usart::TXBufferEmpty(UsartData* pdata)
{
	if(tx_head != tx_tail)
	{
		tx_tail = (tx_tail + 1) & UART_TX_BUF_MASK;
		UDR0 = buf_tx[tx_tail];
	}
	else
	{
		TxDisable();
	}
}

void Usart::TXComplete(UsartData* pdata)
{
	RxEnable();
}

void Usart::SendFrame(UsartData* pdata)
{
	RxDisable();
	uint8_t tmp_tx_head;
	uint8_t *w = pdata->frame;
	uint16_t len = pdata->len;
//	display.Write(len);
	while(len)
	{
		tmp_tx_head = (tx_head  + 1) & UART_TX_BUF_MASK;
		while(tmp_tx_head == tx_tail) {}
		buf_tx[tmp_tx_head] = *w++;
		tx_head = tmp_tx_head;
		len--;
	}

	TxEnable();
}

void Usart::SendInt(UsartData *pdata)
{
	char buf[10];
	itoa(pdata->c, buf, 10);
	//pdata->frame = buf;
	SendFrame(pdata);
}

// --------- Debugowanie
// http://mckmragowo.pl/mck/pliki/programming/clib/?f=va_start

ISR(USART0_RX_vect)
{
	usart_data.c = UDR0;
	usart.CharReceived(&usart_data);
}

ISR(USART0_UDRE_vect)
{
	usart.TXBufferEmpty();
}

ISR(USART0_TX_vect)
{
	usart.TXComplete();
}
