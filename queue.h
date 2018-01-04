/*
 * queue.h
 *
 *  Created on: 4 sty 2018
 *      Author: wjeszak
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include <inttypes.h>
#include "machine.h"

#define QUEUE_LENGTH 	MACHINE_MAX_NUMBER_OF_DOORS - 1

class Queue
{
public:
	Queue();
	void Add(uint8_t el);
	uint8_t Get();
	uint8_t GetNumberOfElements();
private:
	uint8_t head;
	uint8_t tail;
	uint8_t elements[MACHINE_MAX_NUMBER_OF_DOORS];
};

extern Queue q;

#endif /* QUEUE_H_ */
