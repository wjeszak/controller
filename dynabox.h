/*
 * dynabox.h
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#ifndef DYNABOX_H_
#define DYNABOX_H_

class Dynabox : public Machine
{
public:
	Dynabox() : Machine(5) {}
	uint16_t Who() { return 2; }
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
} dynabox;

#endif /* DYNABOX_H_ */
