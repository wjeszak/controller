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
	void EV_EnterToConfig();
	void EV_TestLed(DynaboxData* pdata);				// 0
	void EV_TestElm();									// 1
	void EV_PreparedToHoming();
	void EV_NeedHoming();
	void EV_LedTrigger();
	void EV_HomingDone(DynaboxData* pdata);
	void EV_UserAction(MachineData* pdata);
	void EV_PositionAchieved(DynaboxData* pdata);
	void EV_ReplyOK(MachineData* pdata);
	void EV_Timeout(MachineData* pdata);
	void EV_OnF8(DynaboxData* pdata);
private:
	void ST_TestingLed(DynaboxData* pdata);				// 0
	void ST_TestingElm(DynaboxData* pdata);				// 1
	void ST_PreparingToMovement(DynaboxData* pdata);	// 2
	void ST_ShowingOnLed(DynaboxData* pdata);			// 3
	void ST_Homing(DynaboxData* pdata);					// 4
	void ST_Ready(DynaboxData* pdata);					// 5
	void ST_Movement(DynaboxData* pdata);				// 6
	void ST_EndMovement(DynaboxData* pdata);			// 7
	void ST_NotReady(DynaboxData* pdata);				// 8

	void Entry_Homing();

	void ST_Config(DynaboxData* pdata);
	enum States {ST_TESTING_LED, ST_TESTING_ELM, ST_PREPARING_TO_HOMING, ST_SHOWING_ON_LED, ST_HOMING, ST_READY, ST_MOVEMENT, ST_END_MOVEMENT, ST_NOT_READY, ST_MAX_STATES};
	BEGIN_STATE_MAP_EX
		STATE_MAP_ENTRY_EX(&Dynabox::ST_TestingLed)
		STATE_MAP_ENTRY_EX(&Dynabox::ST_TestingElm)
		STATE_MAP_ENTRY_EX(&Dynabox::ST_PreparingToMovement)
		STATE_MAP_ENTRY_EX(&Dynabox::ST_ShowingOnLed)
		STATE_MAP_ENTRY_EX(&Dynabox::ST_Homing)
		STATE_MAP_ENTRY_EX(&Dynabox::ST_Ready)
//		STATE_MAP_ENTRY_EX(&Dynabox::ST_NotReady)
		STATE_MAP_ENTRY_EX(&Dynabox::ST_Config)
	END_STATE_MAP_EX
	// ---------------------------------------------------------
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

	struct StateProperties
	{
		Destination dest;
		bool need_timeout;
		void (Dynabox::*on_entry)();
		void (Dynabox::*on_exit)();
	};

	StateProperties state_properties[ST_MAX_STATES] =
	{
		{Dest_Led,  true,  NULL, &Dynabox::EV_TestElm},				// ST_TESTING_LED
		{Dest_Door, true,  NULL, &Dynabox::EV_NeedHoming},			// ST_TESTING_ELM
		{Dest_Door, true,  NULL, &Dynabox::EV_PreparedToHoming},	// ST_PREPARING_TO_HOMING
		{Dest_Led,  false, NULL, &Dynabox::EV_LedTrigger},			// ST_SHOWING_ON_LED
		{Dest_Door, true,  &Dynabox::Entry_Homing, NULL},			// ST_HOMING
//		{Dest_Door, true,  NULL, NULL}
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
//		state 						reply fp 					fault						negation
		{ST_TESTING_ELM, 			0x01, NULL, 				F05_ELECTROMAGNET, 			false},
		{ST_PREPARING_TO_HOMING, 	0xC0, NULL, 				F06_CLOSE_THE_DOOR, 		true },
		{ST_HOMING, 				0xC0, &Dynabox::EV_OnF8, 	F08_ILLEGAL_OPENING, 		true }
	};

	StateFault reply_fault_clear[10] =
	{
		{ST_TESTING_ELM, 			0x01, NULL, 				F05_ELECTROMAGNET, 			false},
		{ST_PREPARING_TO_HOMING, 	0xC0, NULL, 				F06_CLOSE_THE_DOOR, 		true },
		{ST_HOMING, 				0xC0, &Dynabox::EV_OnF8, 	F08_ILLEGAL_OPENING, 		true }
	};
};

extern Dynabox dynabox;
extern DynaboxData dynabox_data;

#endif /* DYNABOX_H_ */
