/*
 * main.cpp
 *
 *  Created on: 25 maj 2017
 *      Author: tomek
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "display.h"
#include "timer.h"
#include "usart.h"
#include "enc28j60.h"
#include "machine_type.h"
#include "motor.h"
#include "stack.h"

Timer timer(T0_PS_1024, 17);
//Enc28j60 enc28j60;
Stack stack;
Motor motor;
Display display;
Usart usart;
UsartData usart_data;

int main()
{
	_delay_ms(1000);
	timer.Assign(0, 1, DisplayRefresh);
	motor.Enable(Forward, 20);
	sei();
//	ethernet.Init();
	//Machine *m = GetTypeOfMachine(Lockerbox);
	//uint16_t stan = m->Who();
	//wysw.Wypisz(0);
	while(1)
	{
		stack.StackPoll();
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
*/

/*
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
