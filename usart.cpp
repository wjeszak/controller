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
#include "usart.h"

Usart::Usart(uint16_t baud) : rx_head(0), rx_tail(0), tx_head(0), tx_tail(0)
{
	uint8_t ubrr = F_CPU / 16 / baud - 1;
	UBRR0H = (uint8_t)(ubrr >> 8);
	UBRR0L = (uint8_t)ubrr;

	UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << TXCIE0);
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);

	USART_DE_INIT;
	//wsk_f[0] = &Uart::ST_Gotowy;
	//wsk_f[1] = &Uart::ST_OdebranyZnak;
	//wsk_f[2] = &Uart::ST_Wysylanie;

	//Zdarzenie(ST_GOTOWY);
	//WskPustyBufor = PustyBufor;
	//wUsartDane = &dane;
}

void Usart::RxEnable()
{
	USART_DE_RECEIVE;
	UCSR0B |= (1 << RXCIE0);
}

void Usart::RxDisable()
{
	UCSR0B &= ~(1 << RXCIE0);
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
/*
void Uart::ZD_NowyZnak(Uart_Param *Dane)
{
	const uint16_t TAB_PRZEJSC[] =
	{
		ST_ODEBRANY_ZNAK,				// ST_GOTOWY
		ST_ODEBRANY_ZNAK,				// ST_ODEBRANY_ZNAK
		//ST_ODEBRANY_ZNAK,				// ST_ODEBRANY_STRING
		//ST_GOTOWY 						// ST_BLAD
	};
	Zdarzenie(TAB_PRZEJSC[StanBiezacy], Dane);
}
void Uart::ST_Gotowy(Uart_Param *Dane)
{
	//cout << "Stan: Gotowy" << endl;
}

void Uart::ST_OdebranyZnak(Uart_Param *Dane)
{
	//strcpy(Dane->ramka, "Tomeczek");
	Dane->ramka = "Testowa";
	ZD_WyslijRamke(Dane);
	//Zdarzenie(ST_);
}
*/
void Usart::NewChar(UsartData* pdata)
{

}

void Usart::TXBufferEmpty(UsartData* pdata)
{
	if(tx_head > 0)
	{
		UDR0 = buf_tx[tx_tail++];
		tx_head--;
	}
	else
	{
		TxDisable();
		tx_tail = 0;
	}
}

void Usart::TXComplete(UsartData* pdata)
{
	RxEnable();
}

void Usart::SendFrame(UsartData* pdata)
{
	RxDisable();
	const char *w = pdata->frame;
	while(*w)
	{
		buf_tx[tx_head++] = *w++;
	}
	TxEnable();
}
/*
void USART_WyslijLiczbe(uint16_t liczba, uint16_t podstawa)
{
	char buf[10];
	itoa(liczba, buf, podstawa);
	//USART_WyslijRamke(buf);
}

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
	uint8_t c = UDR0;
	//uart_dane.znak = znak;
	//UDR0 = znak;
//	flaga = 1;
//	tmp_licznik++;
	//uart.ZD_NowyZnak(&uart_dane);
}

ISR(USART0_UDRE_vect)
{
	usart.TXBufferEmpty();
}

ISR(USART0_TX_vect)
{
	usart.TXComplete();
}

void tmp_wyslij()
{
//	uart.ZD_WyslijRamke(&uart_dane);
}
