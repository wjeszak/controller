/*
 * usart.h
 *
 *  Created on: 26 maj 2017
 *      Author: tomek
 */

#ifndef USART_USART_H_
#define USART_USART_H_

#define BAUD 			9600
#define WART_UBRR		F_CPU / 16 / BAUD-1
extern volatile unsigned int usart_flaga;
extern void USART_Init(unsigned int ubrr);
extern void USART_DodajDoBufora(const char znak);
void USART_WyslijRamke(const char *ramka);
void USART_WyslijString(const char *buf);
void USART_WyslijLiczbe(uint16_t liczba, uint16_t podstawa);
void USART_Debug(const char *txt, uint16_t liczba, uint16_t podstawa);
//void USART_Transmit( unsigned char data );
void USART_WyslijDebug(const char *buf, ...);
#endif /* USART_USART_H_ */
