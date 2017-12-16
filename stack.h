/*
 * stack.h
 *
 *  Created on: 15 gru 2017
 *      Author: tomek
 */

#ifndef STACK_H_
#define STACK_H_

#include <inttypes.h>

#define STACK_CAPACITY 		2

class Stack
{
public:
	Stack();
	int8_t GetPtr();
	bool IsEmpty();
	bool IsFull();
	void Push(uint8_t el);
	uint8_t Pop();
private:
	int8_t ptr;
	uint8_t stack[STACK_CAPACITY];
};

extern Stack s;

#endif /* STACK_H_ */
