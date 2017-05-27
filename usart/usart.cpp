/*
 * usart.cpp
 *
 *  Created on: 26 maj 2017
 *      Author: tomek
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#define USART_KIERUNEK_DDR 		DDRC
#define USART_KIERUNEK_PORT 	PORTC
#define USART_KIERUNEK	 		2

#define USART_KIERUNEK_NAD 	USART_KIERUNEK_PORT |=  (1 << USART_KIERUNEK)
#define USART_KIERUNEK_ODB 	USART_KIERUNEK_PORT &= ~(1 << USART_KIERUNEK)

#define USART_ROZMIAR_BUFORA 50

char usart_buf_nad[USART_ROZMIAR_BUFORA];
volatile unsigned int poz_buf = 0, index = 0;

void USART_Init(unsigned int ubrr)
{
	USART_KIERUNEK_DDR |= (1 << USART_KIERUNEK);
	USART_KIERUNEK_ODB;

	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0) | (1 << TXCIE0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}
/*
void USART_Transmit( unsigned char data )
{
	USART_KIERUNEK_NAD;
	_delay_ms(10);

 while ( !( UCSR0A & (1<<UDRE0)) )
 ;

 UDR0 = data;
}
*/
/*
void USART_DodajDoBufora(const char znak)
{
	usart_buf_nad[poz_buf++] = znak;
	if(poz_buf == 16)
	{
		USART_KIERUNEK_NAD;
		UCSR0B |= (1 << UDRIE0);
	}
}
*/
void USART_WyslijRamke(const char *ramka)
{
	//while(poz_buf);
	while(*ramka)
	{
		usart_buf_nad[poz_buf++] = *ramka++;
	}
	USART_KIERUNEK_NAD;
	UCSR0B |= (1 << UDRIE0);
}

ISR(USART0_RX_vect)
{
	char znak = UDR0;
	//USART_DodajDoBufora(znak);
}

ISR(USART0_UDRE_vect)
{
	if(poz_buf != 0)
	{
		UDR0 = usart_buf_nad[index++];
		poz_buf--;
	}
	else
	{
		UCSR0B &= ~(1 << UDRIE0);
		index = 0;
	}
}

ISR(USART0_TX_vect)
{
	USART_KIERUNEK_ODB;
}
