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
#include "timer.h"
#include "usart.h"
#include "wyswietlacz.h"
#include "typ_maszyny.h"
//static uint8_t port_L = 80;
//static uint8_t port_H = 0;

int main()
{
	Wyswietlacz wysw;
	Timer_Init();

//	uart_dane.ramka = "Kupsko piekne";
	//strcpy(uart_dane.ramka, txt);
	//uart.ZD_WyslijRamke(&uart_dane);


	//us.OdebranoZnak();
	//enc28j60_Init();
	//us.WyslijRamke("ENC OK\n");
	//_delay_ms(100);
	//enc28j60_ZrzutRejestrow();
	//uint8_t buf_eth[1500];
	//uint16_t dl;
	//uart_dane.flaga = 0;
	//uart_dane.ramka = "Jest ramka\n";
	sei();
	Maszyna *m = WybierzTypMaszyny(TLockerbox);
	//uint16_t licznik = 0;

	 uint16_t stan = m->PrzedstawSie();
	wysw.Wypisz(stan);

	while(1)
	{
		if(t_flaga)
		{
			t_flaga = 0;
			wysw.Odswiez();

		}
		if(t_flaga_dluga)
		{
			m = WybierzTypMaszyny(TDynabox);
			//m->ZmienStan(2);
			stan = m->PrzedstawSie();
			wysw.Wypisz(stan);
			t_flaga_dluga = 0;
			//wysw.Wypisz(licznik++);
			//if(licznik == 9999) licznik = 0;

		}

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
