/*
 * comm_prot.h
 *
 *  Created on: 27 cze 2017
 *      Author: tomek
 */

#ifndef COMM_PROT_H_
#define COMM_PROT_H_

#include <inttypes.h>

#define FRAME_LENGTH_REQUEST 						4
#define FRAME_LENGTH_RESPONSE 						4

// door's commands
#define COMM_DOOR_CHECK_ELECTROMAGNET 				0x01
#define COMM_DOOR_CHECK_TRANSOPTORS_GET_SET_STATUS 	0x02


// led's commands
#define LED_ADDRESS_OFFSET 							100
#define LED_ADDRESS_TRIGGER 						0xFF

#define COMM_LED_GREEN_RED_OFF 						0x00
#define COMM_LED_GREEN_ON 							0x01
#define COMM_LED_RED_ON 							0x02
#define COMM_LED_GREEN_BLINK 						0x03
#define COMM_LED_RED_BLINK 							0x04
#define COMM_LED_GREEN_RED_BLINK 					0x05
#define COMM_LED_GREEN_1PULSE 						0x06
#define COMM_LED_RED_1PULSE 						0x07
#define COMM_LED_GREEN_2PULSES 						0x08
#define COMM_LED_RED_2PULSES 						0x09
#define COMM_LED_GREEN_3PULSES 						0x0A
#define COMM_LED_RED_3PULSES 						0x0B
#define COMM_LED_GREEN_ON_FOR_TIME 					0x0C
#define COMM_LED_DIAG								0x0D
#define COMM_LED_NEED_QUEUE_BIT						7

#define COMM_F05_ELECTROMAGNET 						0x01

enum CommStatus {CommStatusRequest, CommStatusReply, CommStatusTimeout};

class Comm_prot
{
public:
	Comm_prot();
	uint8_t Crc8(uint8_t* frame, uint8_t len);
	void EV_Send(uint8_t addr, uint8_t command);
	bool CrcOk(uint8_t* frame);
	bool CurrentAddress();
	bool LastAddress();
	void LedTrigger();
};

extern Comm_prot comm;

#endif /* COMM_PROT_H_ */
