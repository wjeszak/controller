/*
 * main.cpp
 *
 *  Created on: 25 maj 2017
 *      Author: tomek
 */
#include <avr/interrupt.h>
#include "enc28j60/stos.h"
#include <util/delay.h>
#include "usart/usart.h"


int main()
{
	USART_Init(WART_UBRR);
	// testowe mruganie LED
	DDRB |= (1 << 6);
	sei();
	USART_WyslijRamke("Paczkomat v.1,8\n");
	USART_WyslijRamke("Uruchamiam enc28j60...\n");
	//LAN_Start();
	while(1)
	{
		_delay_ms(1000);
		PORTB ^= (1 << 6);
		//USART_Transmit('T');
		USART_WyslijRamke("TEST\n");
	}

}



