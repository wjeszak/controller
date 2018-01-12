/*
 * lockerbox.h
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#ifndef LOCKERBOX_H_
#define LOCKERBOX_H_

#include "machine.h"

#define LOCKERBOX_NUMBER_OF_FUNCTIONS 		6

class LockerboxData : public MachineData
{
public:
};

class Lockerbox : public Machine
{
public:
	Lockerbox();
	void LoadParameters();
	void SaveParameters();
	void Init();
// ---------------------------------- Public events ---------------------------
//	void EV_EnterToConfig();
//	void EV_UserAction(MachineData* pdata);
//	void EV_UserActionGo(MachineData* pdata);
//	void EV_UserActionClearFaults(MachineData* pdata);
//	void EV_Reply(MachineData* pdata);
//	void EV_Timeout(MachineData* pdata);
private:
//	void ST0_TestingElm(LockerboxData* pdata);				// 0
//	void ST1_Ready(LockerboxData* pdata);					// 1
//	void ST2_NotReady(LockerboxData* pdata);				// 2

	enum States {ST_INIT = 0, ST_MAX_STATES};
//	BEGIN_STATE_MAP_EX
//		STATE_MAP_ENTRY_EX(&Lockerbox::ST0_TestingElm)
//		STATE_MAP_ENTRY_EX(&Lockerbox::ST1_Ready)
//		STATE_MAP_ENTRY_EX(&Lockerbox::ST2_NotReady)
//	END_STATE_MAP_EX
// ---------------------------------- Entry, exit ----------------------------------
	void ENTRY_TestingElm();
	void EXIT_TestingElm();
	void ENTRY_Ready();
	void EXIT_Ready();
	void ENTRY_NotReady();
	void EXIT_NotReady();
// ---------------------------------------------------------------------------------
	uint8_t GetDestAddr(uint8_t st);
	void SetDoorCommand();
	void SetFaults(uint8_t st, uint8_t reply);
};

extern Lockerbox lockerbox;
extern LockerboxData lockerbox_data;

#endif /* LOCKERBOX_H_ */
