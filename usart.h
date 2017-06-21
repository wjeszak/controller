/*
 * usart.h
 *
 *  Created on: 26 maj 2017
 *      Author: tomek
 */

#ifndef USART_H_
#define USART_H_

#include <inttypes.h>
#include "machine.h"

#define USART_DE_DDR 			DDRC
#define USART_DE_PORT 			PORTC
#define USART_DE_PIN	 		2

#define USART_DE_RECEIVE 		USART_DE_PORT &= ~(1 << USART_DE_PIN)
#define USART_DE_SEND 			USART_DE_PORT |=  (1 << USART_DE_PIN)
#define USART_DE_INIT 			USART_DE_DDR  |=  (1 << USART_DE_PIN); USART_DE_RECEIVE

#define UART_RX_BUF_SIZE 		32
#define UART_TX_BUF_SIZE 		32

class UsartData : public EventData
{
public:
	uint8_t c;
	const char *frame;
};

class Usart : public Machine
{
public:
	Usart(uint16_t baud = 9600);
	void Idle(UsartData* pdata);
	void NewChar(UsartData* pdata);									// RX_vect callback
	void SendFrame(UsartData* pdata);
	void TXBufferEmpty(UsartData* pdata = NULL);					// UDRE_vect callback
	void TXComplete(UsartData* pdata = NULL);						// TX_vect callback
	//virtual ~Usart();

private:
	void RxEnable();
	void RxDisable();
	void TxEnable();
	void TxDisable();
	void (Usart::*p_fun[10])(UsartData* pdata);
	enum States {IDLE, NEW_CHAR, SEND_FRAME, SENDING_FRAME};
	volatile uint8_t buf_rx[UART_RX_BUF_SIZE];
	volatile uint8_t buf_tx[UART_TX_BUF_SIZE];
	volatile uint8_t rx_head, rx_tail, tx_head, tx_tail;
};
extern Usart usart;

#endif /* USART_H_ */
