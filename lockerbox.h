/*
 * lockerbox.h
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#ifndef LOCKERBOX_H_
#define LOCKERBOX_H_

#include "machine.h"
#include "fault.h"

#define LOCKERBOX_NUMBER_OF_FUNCTIONS 		7

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
	void Maintenance();
	void StateManager();
// ---------------------------------- Public events ---------------------------
	void EV_EnterToConfig();
	void EV_UserAction(MachineData* pdata);
	void EV_UserActionGo(MachineData* pdata);
	void EV_UserActionClearFaults(MachineData* pdata);
	void EV_Reply(MachineData* pdata);
	void EV_Timeout(MachineData* pdata);
private:
	void ST0_TestingElm(LockerboxData* pdata);				// 0
	void ST1_Ready(LockerboxData* pdata);					// 1
	void ST2_Processing(LockerboxData* pdata);				// 2
	void ST3_NotReady(LockerboxData* pdata);				// 3

	enum States {ST_TESTING_ELM, ST_READY, ST_PROCESSING, ST_NOT_READY, ST_MAX_STATES};
	BEGIN_STATE_MAP_EX
		STATE_MAP_ENTRY_EX(&Lockerbox::ST0_TestingElm)
		STATE_MAP_ENTRY_EX(&Lockerbox::ST1_Ready)
		STATE_MAP_ENTRY_EX(&Lockerbox::ST2_Processing)
		STATE_MAP_ENTRY_EX(&Lockerbox::ST3_NotReady)
	END_STATE_MAP_EX
// ---------------------------------- Entry, exit ----------------------------------
	void ENTRY_TestingElm(LockerboxData* pdata);
	void EXIT_TestingElm();
	void ENTRY_Ready();
	void EXIT_Ready();
	void ENTRY_Processing();
	void EXIT_Processing();
	void ENTRY_NotReady();
	void EXIT_NotReady();
// ---------------------------------------------------------------------------------
	uint8_t GetDestAddr(uint8_t st);
	void SetDestAddr(uint8_t addr);
	void SetDoorCommand();
	void SetDoorCommand(DoorCommand command);
	void SetFaults(uint8_t st, uint8_t reply);
	uint8_t number_of_elm_faults;
	uint64_t door_need_open;
	struct StateProperties
	{
		void (Lockerbox::*on_entry)();
		void (Lockerbox::*on_exit)();
	};

	StateProperties state_properties[ST_MAX_STATES] =
	{
//		entry 									exit
		{NULL, 									&Lockerbox::EXIT_TestingElm	},	// ST_TESTING_ELM
		{&Lockerbox::ENTRY_Ready,	 			&Lockerbox::EXIT_Ready		},	// ST_READY
		{&Lockerbox::ENTRY_Processing, 			&Lockerbox::EXIT_Processing	},	// ST_PROCESSING
		{&Lockerbox::ENTRY_NotReady, 			&Lockerbox::EXIT_NotReady	},	// ST_NOT_READY

	};

	struct StateFault
	{
		uint8_t state;
		uint8_t reply;
		void (Lockerbox::*fp)(LockerboxData* pdata);
		FaultType fault;
		bool neg;
	};
	StateFault reply_fault_set[ST_MAX_STATES] =
	{
//		state 						reply 			fp 					fault						negation
		{ST_TESTING_ELM, 			0x01,			NULL, 				F05_Elm, 					false},
	};
};

extern Lockerbox lockerbox;
extern LockerboxData lockerbox_data;

#endif /* LOCKERBOX_H_ */
