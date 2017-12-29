/*
 * usart.h
 *
 * Created on: 26 maj 2017
 * Author: tomek
 *
 * Usart char: 1 bit start + 8 bits data + 1 bit stop = 10 bits
 * @19200 bps: t_char = (1 / (19200 bps / 10 bits)) * 1000 =~ 0.5 ms
 */

#ifndef USART_H_
#define USART_H_

#include "comm.h"
#include "state_machine.h"

#define USART_DE_DDR 			DDRC
#define USART_DE_PORT 			PORTC
#define USART_DE_PIN	 		2

#define USART_DE_RECEIVE 		USART_DE_PORT &= ~(1 << USART_DE_PIN)
#define USART_DE_SEND 			USART_DE_PORT |=  (1 << USART_DE_PIN)
#define USART_DE_INIT 			USART_DE_DDR  |=  (1 << USART_DE_PIN)

#define USART_BUF_SIZE 			16
#define USART_BUF_MASK 			(USART_BUF_SIZE - 1)

#define USART_FRAME_END_CHAR 	0x0A

class UsartData : public EventData
{
public:
	uint8_t c;
	uint8_t frame[USART_BUF_SIZE];
	uint8_t len;
};

class Usart : public StateMachine
{
public:
	Usart(uint16_t baud = 19200);
	// Events
	void EV_NewByte(UsartData* pdata);							// RX_vect callback
	void EV_TXBufferEmpty(UsartData* pdata = NULL);				// UDRE_vect callback
	void EV_TXComplete(UsartData* pdata = NULL);				// TX_vect callback
	void SendFrame(UsartData* pdata);
private:
	void RxEnable();
	void RxDisable();
	void TxEnable();
	void TxDisable();
	// States functions
	void ST_Idle(UsartData* pdata);
	void ST_ByteReceived(UsartData* pdata);
	void ST_FrameReceived(UsartData* pdata);
	enum States {ST_IDLE = 0, ST_BYTE_RECEIVED, ST_FRAME_RECEIVED, ST_MAX_STATES};
	BEGIN_STATE_MAP
		STATE_MAP_ENTRY(&Usart::ST_Idle)
		STATE_MAP_ENTRY(&Usart::ST_ByteReceived)
		STATE_MAP_ENTRY(&Usart::ST_FrameReceived)
	END_STATE_MAP

	volatile uint8_t rx_buf[USART_BUF_SIZE];
	volatile uint8_t tx_buf[USART_BUF_SIZE];
	volatile uint8_t rx_head, rx_tail, tx_head, tx_tail;
};

extern Usart usart;
extern UsartData usart_data;

#endif /* USART_H_ */
