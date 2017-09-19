/*
 * usart.cpp
 *
 *  Created on: 26 maj 2017
 *      Author: tomek
 */

#include <avr/interrupt.h>
#include "usart.h"
#include "comm_prot.h"
#include "machine.h"

Usart::Usart(uint16_t baud) : StateMachine(ST_MAX_STATES)
{
	uint16_t ubrr = F_CPU / 16 / baud - 1;
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

void Usart::ST_Idle(UsartData* pdata)
{

}

void Usart::ST_ByteReceived(UsartData* pdata)
{
	uint8_t tmp_head;
	tmp_head = (rx_head + 1) & UART_BUF_MASK;
	if(tmp_head == rx_tail)
	{
		// buffer overwrite
	}
	else
	{
		rx_head = tmp_head;
		rx_buf[tmp_head] = pdata->c;
		pdata->len++;
	}
}

void Usart::ST_FrameReceived(UsartData* pdata)
{
	uint8_t i = 0;
	while(rx_tail != rx_head)
	{
		rx_tail = (rx_tail + 1) & UART_BUF_MASK;
		pdata->frame[i] = rx_buf[rx_tail];
		i++;
	}
	pdata->len = 0;
	InternalEvent(ST_IDLE, pdata);
	m->Parse(pdata->frame);
}

void Usart::EV_NewByte(UsartData* pdata)
{
	BEGIN_TRANSITION_MAP								// current state
        TRANSITION_MAP_ENTRY(ST_BYTE_RECEIVED)			// ST_IDLE
		TRANSITION_MAP_ENTRY(ST_BYTE_RECEIVED)			// ST_BYTE_RECEIVED
    END_TRANSITION_MAP(pdata)

	if((pdata->c == 0x0A) && (pdata->len == FRAME_LENGTH_RESPONSE)) InternalEvent(ST_FRAME_RECEIVED, pdata);
}

void Usart::EV_TXBufferEmpty(UsartData* pdata)
{
	if(tx_head != tx_tail)
	{
		tx_tail = (tx_tail + 1) & UART_BUF_MASK;
		UDR0 = tx_buf[tx_tail];
	}
	else
	{
		TxDisable();
	}
}

void Usart::EV_TXComplete(UsartData* pdata)
{
	RxEnable();
}

void Usart::SendFrame(UsartData* pdata)
{
	RxDisable();
	uint8_t tmp_tx_head;
	uint8_t *w = pdata->frame;
	while(pdata->len)
	{
		tmp_tx_head = (tx_head  + 1) & UART_BUF_MASK;
		while(tmp_tx_head == tx_tail) {}
		tx_buf[tmp_tx_head] = *w++;
		tx_head = tmp_tx_head;
		pdata->len--;
	}
	TxEnable();
}

ISR(USART0_RX_vect)
{
	usart_data.c = UDR0;
	usart.EV_NewByte(&usart_data);
}

ISR(USART0_UDRE_vect)
{
	usart.EV_TXBufferEmpty();
}

ISR(USART0_TX_vect)
{
	usart.EV_TXComplete();
}
