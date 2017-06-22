/*
 * lockerbox.h
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#ifndef LOCKERBOX_H_
#define LOCKERBOX_H_

class Lockerbox : public Machine
{
public:
	Lockerbox() : Machine(4) {}
	uint16_t Who() { return 1; }
} lockerbox;

#endif /* LOCKERBOX_H_ */
