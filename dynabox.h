/*
 * dynabox.h
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#ifndef DYNABOX_H_
#define DYNABOX_H_

#include "machine.h"
#include "motor.h"

#define NUMBER_OF_FAULTS 				17
#define NO_FAULT						0x00
#define F01_LED							0x01
#define F02_DOOR						0x02
#define F03_OPTICAL_SWITCHES			0x03
#define F04_DOOR_OPENED_TOO_FAR			0x04
#define F05_ELECTROMAGNET				0x05
#define F06_CLOSE_THE_DOOR				0x06
#define F07_DOOR_NOT_OPEN				0x07
#define F08_ILLEGAL_OPENING				0x08
#define F10_MECHANICAL_WARNING			0x0A
#define F11_MECHANICAL_FAULT			0x0B
#define F12_POSITIONING					0x0C
#define F13_MAIN_DOOR					0x0D
#define F14_HOMING_FAILED				0x0E
#define F15_ILLEGAL_ROTATION			0x0F
#define F16_ORDER_REFUSED				0x10
#define F17_24V_MISSSING				0x11

#define IO_MOVING 						0
#define IO_MOVING_DIRECTION 			1
#define IO_HOMING_IN_PROGRESS 			2
#define IO_HOMING_DONE 					3
#define IO_MANUAL_OPERATION 			4
#define IO_NORMAL_OPERATION 			5
#define IO_SWITCH_DOOR 					6

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
	void Poll();
	void EV_EnterToConfig();
	void EV_TestLed(DynaboxData* pdata);
	void EV_TestElm();
	void EV_PreparedToHoming();
	void EV_NeedHoming();
	void EV_HomingDone(DynaboxData* pdata);
	void EV_ShowOnLed(DynaboxData* pdata);
	void EV_UserAction(MachineData* pdata);
	void EV_ReplyOK(MachineData* pdata);
	void EV_Timeout(MachineData* pdata);
	void EV_OnF8(DynaboxData* pdata);
private:
	void ST_TestingLed(DynaboxData* pdata);
	void ST_TestingElm(DynaboxData* pdata);
	void ST_PreparingToHoming(DynaboxData* pdata);
	void ST_ShowingOnLed(DynaboxData* pdata);
	void ST_ShowingOnLedOnExit();
	void ST_HomingOnEntry();
	void ST_Homing(DynaboxData* pdata);
	void ST_Ready(DynaboxData* pdata);

	void ST_NotReady(DynaboxData* pdata);
	void ST_Config(DynaboxData* pdata);
	enum States {ST_TESTING_LED, ST_TESTING_ELM, ST_PREPARING_TO_HOMING, ST_SHOWING_ON_LED, ST_HOMING, ST_READY, ST_MAX_STATES};
	BEGIN_STATE_MAP
		STATE_MAP_ENTRY(&Dynabox::ST_TestingLed)
		STATE_MAP_ENTRY(&Dynabox::ST_TestingElm)
		STATE_MAP_ENTRY(&Dynabox::ST_PreparingToHoming)
		STATE_MAP_ENTRY(&Dynabox::ST_ShowingOnLed)
		STATE_MAP_ENTRY(&Dynabox::ST_Homing)
		STATE_MAP_ENTRY(&Dynabox::ST_Ready)
//		STATE_MAP_ENTRY(&Dynabox::ST_NotReady)
		STATE_MAP_ENTRY(&Dynabox::ST_Config)
	END_STATE_MAP
	// ---------------------------------------------------------
	uint8_t GetDestAddr(uint8_t st);
	void SetDestAddr(uint8_t addr);
	void SetFaults(uint8_t st, uint8_t reply);
	uint8_t faults_to_led_map[NUMBER_OF_FAULTS + 1];

	enum Destination {Dest_Door, Dest_Led};

	bool end_state = false;

	uint8_t addr_command[13];

	struct StateProperties
	{
		Destination dest;
		uint8_t command;
		bool need_timeout;
		bool repeat;
		void (Dynabox::*on_entry)();
		void (Dynabox::*on_exit)();
		uint8_t next_state;
	};

	StateProperties state_prop[ST_MAX_STATES] =
	{
		{Dest_Led,  COMM_LED_DIAG, true, false, NULL, &Dynabox::EV_TestElm, ST_TESTING_ELM},			// ST_TESTING_LED
		{Dest_Door, COMM_DOOR_CHECK_ELECTROMAGNET, true, false, NULL, &Dynabox::EV_NeedHoming, ST_PREPARING_TO_HOMING},	// ST_TESTING_ELM
		{Dest_Door, 0x80, true, false, NULL, &Dynabox::EV_PreparedToHoming, ST_SHOWING_ON_LED},											// ST_PREPARING_TO_HOMING
		{Dest_Led, 0, false, false, NULL, NULL, ST_HOMING},												// ST_SHOWING_ON_LED
		{Dest_Door, 0x80, true, true, &Dynabox::ST_HomingOnEntry, NULL, ST_READY},						// ST_HOMING
		{Dest_Door, 0x80, true, true, NULL, NULL, 0}
	};

	struct StateFault
	{
		States state;
		uint8_t reply;
		void (Dynabox::*fp)(DynaboxData* pdata);
		uint8_t fault;
		bool neg;
	};
	StateFault set_state_fault[3] =
	{
		{ST_TESTING_ELM, 0x01, NULL, F05_ELECTROMAGNET, false},
		{ST_PREPARING_TO_HOMING, 0xC0, NULL, F06_CLOSE_THE_DOOR, true},
		{ST_HOMING, 0xC0, &Dynabox::EV_OnF8, F08_ILLEGAL_OPENING, true}
	};
	StateFault clear_state_fault[10];
};

extern Dynabox dynabox;
extern DynaboxData dynabox_data;

#endif /* DYNABOX_H_ */
