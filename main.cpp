/*
 * main.cpp
 *
 *  Created on: 25 maj 2017
 *      Author: tomek
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "enc28j60/stos.h"
#include <util/delay.h>
#include "usart/usart.h"
#include "enc28j60/enc28j60.h"

int main()
{
	USART_Init(WART_UBRR);
	sei();
	USART_WyslijRamke("Inicjowanie...\n");
	_delay_ms(10000);
	enc28j60_Init();
	USART_WyslijRamke("ENC OK\n");
	_delay_ms(100);
	enc28j60_ZrzutRejestrow();
	//_delay_ms(100);
	uint8_t buf_eth[1500];

	while(1)
	{
		enc28j60_OdbierzPakiet(1500, buf_eth);
	}

}
