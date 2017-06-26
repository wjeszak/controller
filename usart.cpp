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
	usart_data.frame = "Idle\n";
	usart.SendFrame(&usart_data);
	static uint16_t i;
	display.Write(i++);
}

void Usart::ST_ByteReceived(UsartData* pdata)
{
	usart_data.frame = "Byte received\n";
	usart.SendFrame(&usart_data);
	if(usart_data.c == 'f') Event(ST_FRAME_RECEIVED, NULL);
}

void Usart::ST_FrameReceived(UsartData* pdata)
{
	usart_data.frame = "Frame received!\n";
	usart.SendFrame(&usart_data);
}

void Usart::CharReceived(UsartData* pdata)
{
	const uint8_t Transitions[] =
	{
		ST_BYTE_RECEIVED,			// ST_IDLE
		ST_BYTE_RECEIVED, 			// ST_BYTE_RECEIVED
		ST_BYTE_RECEIVED			// ST_FRAME_RECEIVED
	};
	Event(Transitions[current_state], pdata);
}

void Usart::TXBufferEmpty(UsartData* pdata)
{
	if(tx_head != tx_tail)
	{
		UDR0 = buf_tx[tx_tail];
		tx_tail = (tx_tail + 1) & UART_TX_BUF_MASK;
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
	const char *w = pdata->frame;
	while(*w)
	{
		tmp_tx_head = (tx_head  + 1) & UART_TX_BUF_MASK;
		while(tmp_tx_head == tx_tail) {}
		buf_tx[tmp_tx_head] = *w++;
		tx_head = tmp_tx_head;
	}
	TxEnable();
}

void Usart::SendInt(UsartData *pdata)
{
	char buf[10];
	itoa(pdata->c, buf, 10);
	pdata->frame = buf;
	SendFrame(pdata);
}
/*
void USART_Debug(const char *txt, uint16_t liczba, uint16_t podstawa)
{
	//USART_WyslijRamke(txt);
	USART_WyslijLiczbe(liczba, podstawa);
}
*/
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
