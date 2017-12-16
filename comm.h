/*
 * comm.h
 *
 *  Created on: 27 cze 2017
 *      Author: tomek
 */

#ifndef COMM_H_
#define COMM_H_

#include <inttypes.h>
#include "usart.h"

#define FRAME_LENGTH_REQUEST 		4
#define FRAME_LENGTH_RESPONSE 		4

class Comm
{
public:
	Comm();
	// Events
	void EV_Send(uint8_t addr, uint8_t command, bool need_timeout);
//	void EV_LedTrigger();
	void EV_Reply();
	void EV_Timeout();
private:
	uint8_t Crc(uint8_t* frame, uint8_t len);
};

extern Comm comm;

#endif /* COMM_H_ */
