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
} lockerbox;

#endif /* LOCKERBOX_H_ */
