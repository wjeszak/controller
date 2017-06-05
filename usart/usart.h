/*
 * usart.h
 *
 *  Created on: 26 maj 2017
 *      Author: tomek
 */

#ifndef USART_USART_H_
#define USART_USART_H_

#define USART_ROZMIAR_BUFORA 50

class Usart
{
private:
	char usart_buf_nad[USART_ROZMIAR_BUFORA];
	volatile uint8_t poz_buf, index, zajety, flaga;

public:
	//void (*WskPustyBufor)();
	void PustyBufor();
	void KoniecNadawania();
	char OdebranoZnak();
	// UWAGA! Tylko do 65535 !!
	Usart(uint16_t Predkosc);				// konstruktor
	void WyslijRamke(const char *ramka);
};

//extern void USART_DodajDoBufora(const char znak);

//void USART_WyslijString(const char *buf);
//void USART_WyslijLiczbe(uint16_t liczba, uint16_t podstawa);
void USART_Debug(const char *txt, uint16_t liczba, uint16_t podstawa);
//void USART_Transmit( unsigned char data );
//void USART_WyslijDebug(const char *buf, ...);

#endif /* USART_USART_H_ */
