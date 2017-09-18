/*
 * lockerbox.h
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#ifndef LOCKERBOX_H_
#define LOCKERBOX_H_

#include "machine.h"

class LockerboxData : public MachineData
{
public:
};

class Lockerbox : public Machine
{
public:
	Lockerbox();
//	void LoadSupportedFunctions();
private:
	enum States {ST_INIT = 0, ST_MAX_STATES};
	//	BEGIN_STATE_MAP
	//		STATE_MAP_ENTRY(&Usart::ST_Init)
	//		STATE_MAP_ENTRY(&Usart::ST_Idle)
	//		STATE_MAP_ENTRY(&Usart::ST_ByteReceived)
	//		STATE_MAP_ENTRY(&Usart::ST_FrameReceived)
	//	END_STATE_MAP
};

extern Lockerbox lockerbox;
extern LockerboxData lockerbox_data;

#endif /* LOCKERBOX_H_ */
