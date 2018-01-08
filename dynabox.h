/*
 * dynabox.h
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#ifndef DYNABOX_H_
#define DYNABOX_H_

#define DEBUG

#include "dynabox_commands_faults.h"
#include "machine.h"
#include "motor.h"

#define DYNABOX_NUMBER_OF_FUNCTIONS 		16
#define MAX_PWM_HOMING 						173

enum DoorCommand { CheckElm = 0x01, GetStatusBeforeMovement = 0x02, ElmOff = 0x03, ElmOffOn = 0x04, GetStatus = 0x80, SetPosition = 0xC0 };
enum DoorReply 	 { ElmOk, ElmFault, TransoptFault = 0xF0, OpenedElmOff = 0x40, Opened15StopsElmOff = 0x4F, Closed = 0xC0, OnOff = 0xD0 };

enum LedCommand  { GreenRedOff, GreenOn, RedOn, GreenBlink, RedBlink, GreenRedBlink,
				   Green1Pulse, Red1Pulse, Green2Pulses, Red2Pulses, Green3Pulses, Red3Pulses,
				   GreenOnForTime, Diag, NeedQueue = 0x80 };

enum Faults 	 { None, F01_Led, F02_Door, F03_Transoptors, F04_DoorOpenedSoFar, F05_Elm,
				   F06_CloseDoor, F07_DoorNotOpen, F08_IllegalOpening,
				   F10_MechanicalWarning = 0x0A, F11_MechanicalFault, F12_Positioning,
				   F13_MainDoor, F14_HomingFailed, F15_IllegalRotation, F16_OrderRefused,
				   F17_24VMissing };

class DynaboxData : public MachineData
{
public:
};

class Dynabox : public Machine
{
public:
	Dynabox();
	void LoadParameters();
	void SaveParameters();
	void Init();
	void StateManager();
// ---------------------------------- Public events ---------------------------
	void EV_EnterToConfig();
	void EV_HomingDone(DynaboxData* pdata);
	void EV_UserAction(MachineData* pdata);
	void EV_UserActionGo(MachineData* pdata);
	void EV_UserActionClearFaults(MachineData* pdata);
	void EV_PositionAchieved(DynaboxData* pdata);
	void EV_LedTrigger();
	void EV_Reply(MachineData* pdata);
	void EV_Timeout(MachineData* pdata);
	void EV_OnF8(DynaboxData* pdata);

	volatile uint8_t encoder_irq_flag;

private:

// ---------------------------------- States ----------------------------------
	void ST0_TestingLed(DynaboxData* pdata);				// 0
	void ST1_TestingElm(DynaboxData* pdata);				// 1
	void ST2_PreparingToMovement(DynaboxData* pdata);		// 2
	void ST3_ShowingOnLed(DynaboxData* pdata);				// 3
	void ST4_Homing(DynaboxData* pdata);					// 4
	void ST5_Ready(DynaboxData* pdata);						// 5
	void ST6_Movement(DynaboxData* pdata);					// 6
	void ST7_EndMovement(DynaboxData* pdata);				// 7
	void ST8_NotReady(DynaboxData* pdata);					// 8
	void ST9_Config(DynaboxData* pdata);					// 9

	enum States {ST_TESTING_LED, ST_TESTING_ELM, ST_PREPARING_TO_MOVEMENT, ST_SHOWING_ON_LED, ST_HOMING, ST_READY, ST_MOVEMENT, ST_END_MOVEMENT, ST_NOT_READY, ST_MAX_STATES};
	BEGIN_STATE_MAP_EX
		STATE_MAP_ENTRY_EX(&Dynabox::ST0_TestingLed)
		STATE_MAP_ENTRY_EX(&Dynabox::ST1_TestingElm)
		STATE_MAP_ENTRY_EX(&Dynabox::ST2_PreparingToMovement)
		STATE_MAP_ENTRY_EX(&Dynabox::ST3_ShowingOnLed)
		STATE_MAP_ENTRY_EX(&Dynabox::ST4_Homing)
		STATE_MAP_ENTRY_EX(&Dynabox::ST5_Ready)
		STATE_MAP_ENTRY_EX(&Dynabox::ST6_Movement)
		STATE_MAP_ENTRY_EX(&Dynabox::ST7_EndMovement)
		STATE_MAP_ENTRY_EX(&Dynabox::ST8_NotReady)
		STATE_MAP_ENTRY_EX(&Dynabox::ST9_Config)
	END_STATE_MAP_EX
// ---------------------------------- Entry, exit ----------------------------------
	void ENTRY_TestingLed(DynaboxData* pdata); 	// call from Dynabox::Init() (dynabox.cpp)
	void EXIT_TestingLed();
	void ENTRY_TestingElm();
	void EXIT_TestingElm();
	void ENTRY_PreparingToMovement();
	void EXIT_PreparingToMovement();
	void ENTRY_ShowingOnLed();
	void EXIT_ShowingOnLed();
	void ENTRY_Homing();
	void EXIT_Homing();
	void ENTRY_Ready();
	void EXIT_Ready();
	void ENTRY_Movement();
	void EXIT_Movement();
	void ENTRY_EndMovement();
	void EXIT_EndMovement();
	void ENTRY_NotReady();
	void EXIT_NotReady();
// ---------------------------------------------------------------------------------
	void SetDestAddr(uint8_t addr);
	uint8_t GetDestAddr(uint8_t st);
	void SetDoorCommand();
	void SetDoorCommand(DoorCommand command);
	void SetLedCommand(bool queued);
	void SetLedCommand(LedCommand command, bool queued);
	bool DoorPositionAchieved();
	void DoorOpenTimeoutManager();
	void SetFaults(uint8_t st, uint8_t reply);
	uint8_t last_position;
	uint8_t door_open_timeout_val;

	LedCommand fault_to_led[NUMBER_OF_FAULTS + 1] =
	{
		GreenRedOff,	// not used
		GreenRedOff,	// F01, impossible
		Red1Pulse,		// F02
		Red2Pulses,		// F03
		RedOn,			// F04
		Red3Pulses,		// F05
		GreenRedBlink, 	// F06
		GreenRedBlink,	// F07
		GreenRedBlink,	// F08
		GreenRedOff,	// not used
		GreenRedOff,	// F10, not used
		RedBlink,		// F11
		RedBlink, 		// F12
		RedOn, 			// F13
		RedBlink, 		// F14
		GreenRedOff,	// F15, not used
		GreenRedOff,	// F16, not used
		RedBlink	 	// F17
	};

	enum Destination {Dest_Door, Dest_Led};

	uint8_t current_command[MACHINE_MAX_NUMBER_OF_DOORS];
	uint8_t desired_doors_position[MACHINE_MAX_NUMBER_OF_DOORS];
	uint8_t door_open_timeout[MACHINE_MAX_NUMBER_OF_DOORS];

	struct StateProperties
	{
		Destination dest;
		bool need_timeout;
		void (Dynabox::*on_entry)();
		void (Dynabox::*on_exit)();
	};

	StateProperties state_properties[ST_MAX_STATES] =
	{
//		dest 		tout 	entry 									exit
		{Dest_Led,  true,  	NULL, 									&Dynabox::EXIT_TestingLed			},	// ST_TESTING_LED
		{Dest_Door, true,  	&Dynabox::ENTRY_TestingElm, 			&Dynabox::EXIT_TestingElm			},	// ST_TESTING_ELM
		{Dest_Door, true,  	&Dynabox::ENTRY_PreparingToMovement, 	&Dynabox::EXIT_PreparingToMovement	},	// ST_PREPARING_TO_MOVEMENT
		{Dest_Led,  false, 	&Dynabox::ENTRY_ShowingOnLed, 			&Dynabox::EXIT_ShowingOnLed			},	// ST_SHOWING_ON_LED
		{Dest_Door, true, 	&Dynabox::ENTRY_Homing, 				&Dynabox::EXIT_Homing				},	// ST_HOMING
		{Dest_Door, true, 	&Dynabox::ENTRY_Ready, 					&Dynabox::EXIT_Ready 				},	// ST_READY
		{Dest_Door, true, 	&Dynabox::ENTRY_Movement,				&Dynabox::EXIT_Movement				},	// ST_MOVEMENT
		{Dest_Door, true, 	&Dynabox::ENTRY_EndMovement,			&Dynabox::EXIT_EndMovement			},	// ST_END_MOVEMENT
		{Dest_Door, true, 	&Dynabox::ENTRY_NotReady,				&Dynabox::EXIT_NotReady				},	// ST_NOT_READY
	};
// -----------------------------need by SetFaults() --------------------------------
	struct StateFault
	{
		uint8_t state;
		uint8_t reply;
		void (Dynabox::*fp)(DynaboxData* pdata);
		Faults fault;
		bool neg;
	};
	StateFault reply_fault_set[ST_MAX_STATES] =
	{
//		state 						reply 			fp 					fault						negation
		{ST_TESTING_ELM, 			ElmFault,		NULL, 				F05_Elm, 					false},
		{ST_PREPARING_TO_MOVEMENT, 	Closed, 		NULL, 				F06_CloseDoor, 				true },
		{ST_HOMING, 				Closed, 		&Dynabox::EV_OnF8, 	F08_IllegalOpening, 		true },
		{ST_MOVEMENT, 				Closed, 		&Dynabox::EV_OnF8,	F08_IllegalOpening, 		true },
	};

	StateFault reply_fault_clear[ST_MAX_STATES] =
	{
		{ST_TESTING_ELM, 			ElmOk, 			NULL, 				F05_Elm,		 			false},
		{ST_PREPARING_TO_MOVEMENT, 	ElmOk, 			NULL, 				F06_CloseDoor, 				true },
		{ST_HOMING, 				ElmOk, 			&Dynabox::EV_OnF8, 	F08_IllegalOpening, 		true },
		//{ST_NOT_READY, 				Closed,
	};
};

extern Dynabox dynabox;
extern DynaboxData dynabox_data;

#endif /* DYNABOX_H_ */
