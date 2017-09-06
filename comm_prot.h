/*
 * comm_prot.h
 *
 *  Created on: 27 cze 2017
 *      Author: tomek
 */

#ifndef COMM_PROT_H_
#define COMM_PROT_H_

#include <inttypes.h>

#define FRAME_LENGTH 		4

class Comm_prot
{
public:
	Comm_prot();
	void Parse(uint8_t* frame);
	void Prepare(uint8_t res);
private:
	uint8_t Crc8(uint8_t *frame, uint8_t len);
	uint8_t slave_addr;
};

extern Comm_prot comm;

#endif /* COMM_PROT_H_ */
