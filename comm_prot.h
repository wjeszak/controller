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

#define LED_ADDRESS_OFFSET 					100

#define COMM_CHECK_ELECTROMAGNET 			0x01
#define COMM_CHECK_TRANSOPTORS_GET_STATE 	0x02
#define COMM_SET_STATE 						0x03

class Comm_prot
{
public:
	Comm_prot();
	void Prepare(uint8_t addr, uint8_t command);
	void Parse(uint8_t addr, uint8_t* frame);
private:
	uint8_t Crc8(uint8_t *frame, uint8_t len);
};

extern Comm_prot comm;

#endif /* COMM_PROT_H_ */
