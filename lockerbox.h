/*
 * lockerbox.h
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#ifndef LOCKERBOX_H_
#define LOCKERBOX_H_

#include <inttypes.h>
#include "machine.h"

class Lockerbox : public Machine
{
public:
	Lockerbox();
	uint16_t StartupTest();

private:
	enum States {ST_INIT = 0, ST_MAX_STATES};
	const StateStruct* GetStateMap()
	{
		static const StateStruct StateMap[] =
		{
				//{reinterpret_cast<StateFunc>(&Usart::ST_Idle)},
				//{reinterpret_cast<StateFunc>(&Usart::ST_ByteReceived)},
				//{reinterpret_cast<StateFunc>(&Usart::ST_FrameReceived)}
		};
		return &StateMap[0];
	}
};

extern Lockerbox lockerbox;

#endif /* LOCKERBOX_H_ */
