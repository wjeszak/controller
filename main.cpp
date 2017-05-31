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
#include "enc28j60/stos.h"
static uint8_t port_L = 80;
static uint8_t port_H = 0;
int main()
{
	USART_Init(WART_UBRR);
	sei();
	USART_WyslijRamke("Inicjowanie...\n");
	//_delay_ms(10000);
	enc28j60_Init();
	USART_WyslijRamke("ENC OK\n");
	//_delay_ms(100);
	enc28j60_ZrzutRejestrow();
	uint8_t buf_eth[1500];
	uint16_t dl;
	while(1)
	{
		dl = enc28j60_OdbierzPakiet(1500, buf_eth);
		if(eth_type_is_arp_and_my_ip(buf_eth, dl))
		{
			// doprecyzowac typ pakietu
			USART_WyslijRamke("Nasz pakiet ARP!\n");
			make_arp_answer_from_request(buf_eth);
		}
		//if(eth_type_is_ip_and_my_ip(buf_eth, dl))
		//{
			// doprecyzowac typ pakietu
		//	USART_WyslijRamke("Nasz pakiet PING!\n");
		//	make_echo_reply_from_request(buf_eth, dl);
		//}
		if(buf_eth[TCP_DST_PORT_H_P]== port_H && buf_eth[TCP_DST_PORT_L_P] == port_L)
		{
			if (buf_eth[TCP_FLAGS_P] & TCP_FLAGS_SYN_V)
			{
				USART_WyslijRamke("Nasz pakiet TCP [SYN]\n");
				make_tcp_synack_from_syn(buf_eth);
			}
		}
	}

}
