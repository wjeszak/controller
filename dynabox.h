/*
 * dynabox.h
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#ifndef DYNABOX_H_
#define DYNABOX_H_
#include "machine.h"

class Dynabox : public Machine
{
public:
	Dynabox();
	uint16_t StartupTest();
private:
	enum States {ST_INIT = 0, ST_MAX_STATES};
	const StateStruct* GetStateMap()
	{
		// to jest sprytne bo StateMap jest tworzone nie na stosie dzieki temu mozna zwrocic adres
		static const StateStruct StateMap[] =
		{
			//{reinterpret_cast<StateFunc>(&Usart::ST_Idle)},
			//{reinterpret_cast<StateFunc>(&Usart::ST_ByteReceived)},
			//{reinterpret_cast<StateFunc>(&Usart::ST_FrameReceived)}
		};
		return &StateMap[0];
	}
};

extern Dynabox dynabox;

#endif /* DYNABOX_H_ */
