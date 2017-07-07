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

#define USART_DE_INIT 			USART_DE_DDR  |=  (1 << USART_DE_PIN)
#define USART_DE_RECEIVE 		USART_DE_PORT &= ~(1 << USART_DE_PIN)
#define USART_DE_SEND 			USART_DE_PORT |=  (1 << USART_DE_PIN)

#define UART_BUF_SIZE 		64
#define UART_BUF_MASK 		(UART_BUF_SIZE - 1)

class UsartData : public EventData
{
public:
	uint8_t c;
	uint8_t frame[UART_BUF_SIZE];
	uint8_t len;
};

class Usart : public Machine
{
public:
	Usart(uint16_t baud = 19400);
	// Events
	void EV_NewByte(UsartData* pdata);							// RX_vect callback
	void EV_TXBufferEmpty(UsartData* pdata = NULL);				// UDRE_vect callback
	void EV_TXComplete(UsartData* pdata = NULL);				// TX_vect callback
	void RTU35T(UsartData* pdata = NULL);
	void SendFrame(UsartData* pdata);
private:
	void RxEnable();
	void RxDisable();
	void TxEnable();
	void TxDisable();
	// States functions
	void ST_Init(UsartData* pdata);
	void ST_Idle(UsartData* pdata);
	void ST_ByteReceived(UsartData* pdata);
	void ST_FrameReceived(UsartData* pdata);
	enum States {ST_INIT = 0, ST_IDLE, ST_BYTE_RECEIVED, ST_FRAME_RECEIVED, ST_MAX_STATES};
	BEGIN_STATE_MAP
		STATE_MAP_ENTRY(&Usart::ST_Init)
		STATE_MAP_ENTRY(&Usart::ST_Idle)
		STATE_MAP_ENTRY(&Usart::ST_ByteReceived)
		STATE_MAP_ENTRY(&Usart::ST_FrameReceived)
	END_STATE_MAP

	volatile uint8_t rx_buf[UART_BUF_SIZE];
	volatile uint8_t tx_buf[UART_BUF_SIZE];
	volatile uint8_t rx_head, rx_tail, tx_head, tx_tail;
};

extern Usart usart;
extern UsartData usart_data;

#endif /* USART_H_ */
