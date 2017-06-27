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
#define USART_DE_INIT 			USART_DE_DDR  |=  (1 << USART_DE_PIN)

#define UART_RX_BUF_SIZE 		64
#define UART_TX_BUF_SIZE 		64
#define UART_TX_BUF_MASK 		(UART_TX_BUF_SIZE - 1)
#define UART_RX_BUF_MASK 		(UART_RX_BUF_SIZE - 1)
class UsartData : public EventData
{
public:
	uint8_t c;
	uint8_t frame[64];
	uint16_t len;
};

class Usart : public Machine
{
public:
	Usart(uint16_t baud = 19400);
	void CharReceived(UsartData* pdata);							// RX_vect callback
	void SendFrame(UsartData* pdata);
	void SendInt(UsartData* pdata);
	void TXBufferEmpty(UsartData* pdata = NULL);					// UDRE_vect callback
	void TXComplete(UsartData* pdata = NULL);						// TX_vect callback
	void RTU35T(UsartData* pdata = NULL);
private:
	void RxEnable();
	void RxDisable();
	void TxEnable();
	void TxDisable();
	volatile uint8_t buf_rx[UART_RX_BUF_SIZE];
	volatile uint8_t buf_tx[UART_TX_BUF_SIZE];
	volatile uint8_t rx_head, rx_tail, tx_head, tx_tail;
	// States
	void ST_Init(UsartData* pdata);
	void ST_Idle(UsartData* pdata);
	void ST_ByteReceived(UsartData* pdata);
	void ST_FrameReceived(UsartData* pdata);
	//
	enum States {ST_INIT = 0, ST_IDLE, ST_BYTE_RECEIVED, ST_FRAME_RECEIVED, ST_MAX_STATES};
	const StateStruct* GetStateMap()
	{
		// to jest sprytne bo StateMap jest tworzone nie na stosie dzieki temu mozna zwrocic adres
		static const StateStruct StateMap[] =
		{
			{reinterpret_cast<StateFunc>(&Usart::ST_Init)},
			{reinterpret_cast<StateFunc>(&Usart::ST_Idle)},
			{reinterpret_cast<StateFunc>(&Usart::ST_ByteReceived)},
			{reinterpret_cast<StateFunc>(&Usart::ST_FrameReceived)}
		};
		return &StateMap[0];
	}
};

extern Usart usart;
extern UsartData usart_data;

#endif /* USART_H_ */
