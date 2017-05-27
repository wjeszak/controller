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

extern void USART_Init(unsigned int ubrr);
extern void USART_DodajDoBufora(const char znak);
void USART_WyslijRamke(const char *ramka);
//void USART_Transmit( unsigned char data );

#endif /* USART_USART_H_ */
