/*
 * stack.cpp
 *
 *  Created on: 15 gru 2017
 *      Author: tomek
 */

#include "stack.h"

Stack::Stack()
{
	ptr = -1;
}

int8_t Stack::GetPtr()
{
	return ptr;
}

bool Stack::IsEmpty()
{
    if (ptr < 0)
        return true;
    return false;
}

bool Stack::IsFull()
{
    if (ptr >= STACK_CAPACITY - 1)
        return true;
    return false;
}

void Stack::Push(uint8_t el)
{
    if (IsFull() == true) { }
    else
        stack[++ptr] = el;
}

uint8_t Stack::Pop()
{
    if (IsEmpty() == true)
        return 0xFF;
    return stack[ptr--];
}
