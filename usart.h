/*
 * usart.h
 *
 *  Created on: 26 maj 2017
 *      Author: tomek
 */

#ifndef USART_H_
#define USART_H_

#include <inttypes.h>
#include "maszyna.h"
#define UART_ROZMIAR_BUFORA 			30

class Uart_Param : public Param
{
public:
	char znak;
	volatile int flaga;
	const char *ramka;
};

class Uart : public Maszyna
{
public:
	Uart(uint16_t Predkosc);
	void ZD_NowyZnak(Uart_Param *Dane);
	void ZD_WyslijRamke(Uart_Param *Dane);
	void ZD_PustyBufor(Uart_Param *Dane = NULL);
	void ZD_KoniecNadawania(Uart_Param *Dane = NULL);

	void ST_Gotowy(Uart_Param *Dane);
	void ST_OdebranyZnak(Uart_Param *Dane);

private:
	void (Uart::*wsk_f[10])(Uart_Param *Dane);
	enum Stany {ST_GOTOWY = 0, ST_ODEBRANY_ZNAK, ST_ODEBRANY_STRING, ST_WYSYLANIE};
	uint8_t i;
	//uint8_t buf[UART_ROZMIAR_BUFORA];
	//uint8_t	poz_buf, index, zajety;
};

#endif /* USART_H_ */
