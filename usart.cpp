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

#define USART_KIERUNEK_DDR 		DDRC
#define USART_KIERUNEK_PORT 	PORTC
#define USART_KIERUNEK	 		2

#define USART_KIERUNEK_NAD 	USART_KIERUNEK_PORT |=  (1 << USART_KIERUNEK)
#define USART_KIERUNEK_ODB 	USART_KIERUNEK_PORT &= ~(1 << USART_KIERUNEK)

Uart_Param uart_dane;
Uart uart(9600);
uint8_t buf[UART_ROZMIAR_BUFORA];
uint8_t	volatile poz_buf, index, zajety = 0;
Uart::Uart(uint16_t Predkosc)
{
	uint8_t ubrr = F_CPU / 16 / Predkosc - 1;
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;

	UCSR0B = (1 << RXEN0) | (1 << RXCIE0) | (1 << TXEN0) | (1 << TXCIE0);
	//UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

	USART_KIERUNEK_DDR |= (1 << USART_KIERUNEK);
	USART_KIERUNEK_ODB;

	//wsk_f[0] = &Uart::ST_Gotowy;
	//wsk_f[1] = &Uart::ST_OdebranyZnak;
	//wsk_f[2] = &Uart::ST_Wysylanie;
	poz_buf = 0;
	index = 0;
	zajety = 0;
	i = 0;
	//Zdarzenie(ST_GOTOWY);
	//WskPustyBufor = PustyBufor;
	//wUsartDane = &dane;
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
void Uart::ZD_PustyBufor(Uart_Param *Dane)
{

}

void Uart::ZD_KoniecNadawania(Uart_Param *Dane)
{
	USART_KIERUNEK_ODB;
	//zajety = 0;
}

void Uart::ZD_WyslijRamke(Uart_Param *Dane)
{
	//while(zajety);
	//zajety = 1;
	//cli();
	const char *txt = "Dupas";
	const char *w = txt;
	while(*w)
	{
		buf[poz_buf++] = *w++;
	}
	//sei();
	USART_KIERUNEK_NAD;
	UCSR0B |= (1 << UDRIE0);
}

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

// --------- Debugowanie
// http://mckmragowo.pl/mck/pliki/programming/clib/?f=va_start


ISR(USART0_RX_vect)
{
	char znak = UDR0;
	//uart_dane.znak = znak;
	UDR0 = znak;
	//uart_dane.flaga = 1;
	//uart.ZD_NowyZnak(&uart_dane);
}

ISR(USART0_UDRE_vect)
{
	if(poz_buf > 0)
	{
		UDR0 = buf[index++];
		poz_buf--;
	}
	else
	{
		UCSR0B &= ~(1 << UDRIE0);
		index = 0;
	}
	//uart.ZD_PustyBufor();
}

ISR(USART0_TX_vect)
{
	USART_KIERUNEK_ODB;
	//uart.ZD_KoniecNadawania();
}

