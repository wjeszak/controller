/*
 * dynabox.h
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#ifndef DYNABOX_H_
#define DYNABOX_H_

#include "dynabox_commands_faults.h"
#include "machine.h"
#include "motor.h"

#define IO_MOVING 						0
#define IO_MOVING_DIRECTION 			1
#define IO_HOMING_IN_PROGRESS 			2
#define IO_HOMING_DONE 					3
#define IO_MANUAL_OPERATION 			4
#define IO_NORMAL_OPERATION 			5
#define IO_SWITCH_DOOR 					6

#define MAX_PWM_HOMING 					173

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
	void EV_PositionAchieved(DynaboxData* pdata);
	void EV_LedTrigger();
	void EV_Reply(MachineData* pdata);
	void EV_Timeout(MachineData* pdata);
	void EV_OnF8(DynaboxData* pdata);
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
	void SetCommand(uint8_t command); 	// common command for all slaves
	void SetFaults(uint8_t st, uint8_t reply);
	uint8_t fault_to_led[NUMBER_OF_FAULTS + 1] =
	{
		0,										// not used
		0,										// F01, impossible
		COMM_LED_RED_1PULSE,					// F02
		COMM_LED_RED_2PULSES,					// F03
		COMM_LED_RED_ON,						// F04
		COMM_LED_RED_3PULSES,					// F05
		COMM_LED_GREEN_RED_BLINK, 				// F06
		COMM_LED_GREEN_RED_BLINK,				// F07
		COMM_LED_GREEN_RED_BLINK,				// F08
		0,										// not used
		0, 										// F10, not used
		COMM_LED_RED_BLINK,						// F11
		COMM_LED_RED_BLINK, 					// F12
		COMM_LED_RED_ON, 						// F13
		COMM_LED_RED_BLINK, 					// F14
		0,										// F15, not used
		0, 										// F16, not used
		COMM_LED_RED_BLINK	 					// F17
	};

	enum Destination {Dest_Door, Dest_Led};

	uint8_t current_command[MACHINE_MAX_NUMBER_OF_DOORS];
	uint8_t desired_doors_position[MACHINE_MAX_NUMBER_OF_DOORS];

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
		{Dest_Led,  false, 	NULL, 									&Dynabox::EV_LedTrigger				},	// ST_SHOWING_ON_LED
		{Dest_Door, true,  &Dynabox::ENTRY_Homing, 					NULL								},	// ST_HOMING
	};

	struct StateFault
	{
		States state;
		uint8_t reply;
		void (Dynabox::*fp)(DynaboxData* pdata);
		uint8_t fault;
		bool neg;
	};
	StateFault reply_fault_set[10] =
	{
//		state 						reply 						fp 					fault						negation
		{ST_TESTING_ELM, 			COMM_DOOR_REPLY_ELM_FAULT,	NULL, 				F05_ELM, 					false},
		{ST_PREPARING_TO_MOVEMENT, 	COMM_DOOR_REPLY_CLOSED, 	NULL, 				F06_CLOSE_THE_DOOR, 		true },
		{ST_HOMING, 				COMM_DOOR_REPLY_CLOSED, 	&Dynabox::EV_OnF8, 	F08_ILLEGAL_OPENING, 		true }
	};

	StateFault reply_fault_clear[10] =
	{
		{ST_TESTING_ELM, 			0x01, NULL, 				F05_ELM,		 			false},
		{ST_PREPARING_TO_MOVEMENT, 	0xC0, NULL, 				F06_CLOSE_THE_DOOR, 		true },
		{ST_HOMING, 				0xC0, &Dynabox::EV_OnF8, 	F08_ILLEGAL_OPENING, 		true }
	};
};

extern Dynabox dynabox;
extern DynaboxData dynabox_data;

#endif /* DYNABOX_H_ */
