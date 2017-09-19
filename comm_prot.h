/*
 * comm_prot.h
 *
 *  Created on: 27 cze 2017
 *      Author: tomek
 */

#ifndef COMM_PROT_H_
#define COMM_PROT_H_

#include <inttypes.h>

#define FRAME_LENGTH_REQUEST 				4
#define FRAME_LENGTH_RESPONSE 				4

#define COMM_CHECK_ELECTROMAGNET 			0x01
#define COMM_CHECK_TRANSOPTORS_GET_STATUS 	0x02
#define COMM_SET_STATE 						0x03

#define LED_ADDRESS_OFFSET 					100
#define LED_ADDRESS_TRIGGER 				0xFF

#define COMM_GREEN_RED_OFF 					0x00
#define COMM_GREEN_ON 						0x01
#define COMM_RED_ON 						0x02
#define COMM_GREEN_BLINK 					0x03
#define COMM_RED_BLINK 						0x04
#define COMM_GREEN_RED_BLINK 				0x05
#define COMM_GREEN_1PULSE 					0x06
#define COMM_RED_1PULSE 					0x07
#define COMM_GREEN_2PULSES 					0x08
#define COMM_RED_2PULSES 					0x09
#define COMM_GREEN_3PULSES 					0x0A
#define COMM_RED_3PULSES 					0x0B
#define COMM_GREEN_ON_FOR_TIME 				0x0C
#define COMM_LED_DIAG						0x0D
#define COMM_NEED_QUEUE_BIT					7

#define NO_FAULT 							0x00
#define F01_LED_FAULT	 					0x01
#define F02_DOOR_FAULT 						0x02
#define F03_OPTICAL_SWITCHES_FAULT			0x03
#define F04_DOOR_OPENED_TOO_FAR 			0x04
#define F05_ELECTROMAGNET_FAULT				0x05
#define F06_CLOSE_THE_DOOR 					0x06
#define F07_DOOR_NOT_OPEN 					0x07
#define F08_ILLEGAL_OPENING 				0x08

enum DestType {TDoor, TLed};

class Comm_prot
{
public:
	Comm_prot();
	uint8_t Crc8(uint8_t *frame, uint8_t len);
	void Prepare(DestType t, uint8_t addr, uint8_t command);
	void LedTrigger();
	void ParseGeneral(uint8_t* frame);
	bool repeat;
	DestType dest;
	uint8_t curr_command;
};

extern Comm_prot comm;

#endif /* COMM_PROT_H_ */
