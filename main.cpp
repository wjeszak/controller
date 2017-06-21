/*
 * main.cpp
 *
 *  Created on: 25 maj 2017
 *      Author: tomek
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "timer.h"
#include "usart.h"
#include "wyswietlacz.h"
#include "typ_maszyny.h"
#include "enc28j60.h"
#include "stos.h"
#include "motor.h"

Enc28j60 ethernet;
//Wyswietlacz wysw;
Usart usart;
int main()
{
	Motor motor;
	motor.Enable(Forward, 50);

	//Uart uart(9600);
	//Uart_Param uart_dane;
	//uart_dane.ramka = "Jest ramka\n";
	//ethernet.Init();
	//Stos stos;
	//Timer_Init();
//	uint8_t buf_eth[1500];
//	uint16_t dl;
//	uint16_t licznik_pakietow = 1;
	sei();
//	Machine *m = GetTypeOfMachine(Lockerbox);
//	uint16_t stan = m->Who();
	//wysw.Wypisz(0);
	//TDirection d;
	while(1)
	{

		//_delay_ms(1);
		//MOTOR_PORT |= (1 << MOTOR_EN_PIN);
		//_delay_ms(10);
		//MOTOR_PORT &= ~ (1 << MOTOR_EN_PIN);
		//if(d == TForward) d = TBackward;
		//if(d == TBackward) d = TForward;
		//motor.SetDirection(d);

		/*
		if(t_flaga_dluga)
		{
			m = WybierzTypMaszyny(TDynabox);
			m->ZmienStan(2);
			stan = m->PrzedstawSie();
			wysw.Wypisz(stan);
			t_flaga_dluga = 0;
			wysw.Wypisz(licznik++);
			if(licznik == 9999) licznik = 0;
		}


		dl = ethernet.OdbierzPakiet(1500, buf_eth);
		if(dl != 0)
		{
			//wysw.Wypisz(licznik_pakietow++);
			//uart.ZD_WyslijRamke(&uart_dane);
		}
		if(stos.eth_type_is_arp_and_my_ip(buf_eth, dl))
		{
			// doprecyzowac typ pakietu
			//USART_WyslijRamke("Nasz pakiet ARP!\n");
			stos.make_arp_answer_from_request(buf_eth);
		}
		if(stos.eth_type_is_ip_and_my_ip(buf_eth, dl))
		{
			// doprecyzowac typ pakietu
		//	USART_WyslijRamke("Nasz pakiet PING!\n");
			stos.make_echo_reply_from_request(buf_eth, dl);
		}

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
