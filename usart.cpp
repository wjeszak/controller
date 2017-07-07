/*
 * usart.cpp
 *
 *  Created on: 26 maj 2017
 *      Author: tomek
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart.h"
#include "modbus_rtu.h"
#include "timer.h"

Usart::Usart(uint16_t baud) : Machine(ST_MAX_STATES)
{
	uint8_t ubrr = F_CPU / 16 / baud - 1;
	UBRR0H = (uint8_t)(ubrr >> 8);
	UBRR0L = (uint8_t)ubrr;

	UCSR0B |= (1 << RXEN0) | (1 << RXCIE0) | (1 << TXEN0) | (1 << TXCIE0);
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);

	USART_DE_INIT;
	rx_head = 0;
	rx_tail = 0;
	tx_head = 0;
	tx_tail = 0;
	timer.Assign(TIMER_MODBUS_RTU_35T, 2, ModbusRTU35T);
}

void Usart::EV_NewByte(UsartData* pdata)
{
    BEGIN_TRANSITION_MAP							// current state
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_INIT
        TRANSITION_MAP_ENTRY(ST_BYTE_RECEIVED)		// ST_IDLE
        TRANSITION_MAP_ENTRY(ST_BYTE_RECEIVED)		// ST_BYTE_RECEIVED
        TRANSITION_MAP_ENTRY(ST_BYTE_RECEIVED)		// ST_FRAME_RECEIVED
    END_TRANSITION_MAP(pdata)
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

void Usart::RTU35T(UsartData* pdata)
{
    BEGIN_TRANSITION_MAP							// current state
        TRANSITION_MAP_ENTRY(ST_IDLE)				// ST_INIT
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_IDLE
        TRANSITION_MAP_ENTRY(ST_FRAME_RECEIVED)		// ST_BYTE_RECEIVED
    END_TRANSITION_MAP(pdata)
}

void Usart::SendFrame(UsartData* pdata)
{
	RxDisable();
	uint8_t tmp_tx_head;
	uint8_t *w = pdata->frame;
	uint8_t len = pdata->len;

	while(len)
	{
		tmp_tx_head = (tx_head  + 1) & UART_BUF_MASK;
		while(tmp_tx_head == tx_tail) {}
		tx_buf[tmp_tx_head] = *w++;
		tx_head = tmp_tx_head;
		len--;
	}
	TxEnable();
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

void Usart::ST_Init(UsartData* pdata)
{

}

void Usart::ST_Idle(UsartData* pdata)
{
	timer.Disable(TIMER_MODBUS_RTU_35T);
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
	}
	timer.Enable(TIMER_MODBUS_RTU_35T);
}

void Usart::ST_FrameReceived(UsartData* pdata)
{
	pdata->len = 7;
	uint8_t i = 0;
	while(rx_tail != rx_head)
	{
		rx_tail = (rx_tail + 1) & UART_BUF_MASK;
		usart_data.frame[i] = rx_buf[rx_tail];
		i++;
	}
	modbus_rtu.ParseFrame(pdata->frame, pdata->len);
	timer.Disable(TIMER_MODBUS_RTU_35T);
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
