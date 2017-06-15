/*
 * main.cpp
 *
 *  Created on: 25 maj 2017
 *      Author: tomek
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "enc28j60.h"
#include "stos.h"
#include "usart.h"
#include "wyswietlacz.h"
#include "timery.h"
//static uint8_t port_L = 80;
//static uint8_t port_H = 0;

extern Uart uart;
extern Uart_Param uart_dane;
int main()
{
	Wysw_Init();
	Timer_Init();
	sei();
	//uart_dane.ramka = "Kupsko piekne";
	//strcpy(uart_dane.ramka, txt);
	//uart.ZD_WyslijRamke(&uart_dane);


	//us.OdebranoZnak();
	//enc28j60_Init();
	//us.WyslijRamke("ENC OK\n");
	//_delay_ms(100);
	//enc28j60_ZrzutRejestrow();
	//uint8_t buf_eth[1500];
	//uint16_t dl;
	uart_dane.flaga = 0;
	//uart_dane.ramka = "Jest ramka\n";
	uint8_t nr_wysw = 0;
	uint16_t l = 0;
	while(1)
	{
		if(t_flaga_licz)
				{
					t_flaga_licz = 0;
					IntToLed(l++);
					if(l == 5000) l = 0;
				}
				if(t_flaga)
				{
					t_flaga = 0;
					WYSW_SEGMENTY_PORT = cyfra[nr_wysw];				// laduj cyfre do portu
					*(tab[nr_wysw].port) &= ~tab[nr_wysw].pin;			// zaswiec modul
					if(nr_wysw == 0)
						*(tab[3].port) |= tab[3].pin;					// zgas modul
					else
						*(tab[nr_wysw-1].port) |= tab[nr_wysw-1].pin;	// zgas modul
					nr_wysw++;
					if(nr_wysw == 4) nr_wysw = 0;
				}
		//if(uart_dane.flaga == 1)
		//{
			//_delay_ms(100);
			//uart_dane.flaga = 0;
		//_delay_ms(100);
		//uart.ZD_WyslijRamke(&uart_dane);
			//uart.ZD_NowyZnak(&uart_dane);

		//}
		/*	dl = enc28j60_OdbierzPakiet(1500, buf_eth);
		if(eth_type_is_arp_and_my_ip(buf_eth, dl))
		{
			// doprecyzowac typ pakietu
			//USART_WyslijRamke("Nasz pakiet ARP!\n");
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
				//USART_WyslijRamke("Nasz pakiet TCP [SYN]\n");
				make_tcp_synack_from_syn(buf_eth);
			}
		}
		*/
	}

}
