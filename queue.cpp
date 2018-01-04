/*
 * queue.cpp
 *
 *  Created on: 4 sty 2018
 *      Author: wjeszak
 */

#include "queue.h"

Queue::Queue()
{
	head = 0;
	tail = 0;
}

void Queue::Add(uint8_t el)
{
    uint8_t tmp_head = (head + 1) & QUEUE_LENGTH;
    if (tmp_head == tail)
    {
        // overwrite
    }
    else
        head = tmp_head;
    elements[tmp_head] = el;
}

uint8_t Queue::Get()
{
    if(head != tail)
    {
        // something in queue
        tail = (tail + 1) & QUEUE_LENGTH;
        return elements[tail];
    }
    // empty
    return 0xFF;
}

uint8_t Queue::GetNumberOfElements()
{
	return (head - tail) & QUEUE_LENGTH;
}
