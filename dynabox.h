/*
 * dynabox.h
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#ifndef DYNABOX_H_
#define DYNABOX_H_

#include "machine.h"

//#define NUMBER_OF_FAULTS 				17

class DynaboxData : public MachineData
{
public:
};

class Dynabox : public Machine
{
public:
	Dynabox();
	void EV_LEDChecked(DynaboxData* pdata);
	void EV_ElectromagnetChecked(DynaboxData* pdata);
	void LoadSupportedFunctions();
	void SaveParameters();
	void SetCurrentCommand(uint8_t command, bool rep);
	void CommandCheckLed();
	void Parse(uint8_t* frame);
	void ParseCheckLed(uint8_t* frame);
	void TimeoutCheckLed();
	void PCCheckElectromagnet(uint8_t res);
	void PCCheckTransoptorsGetStatus(uint8_t res);
	void SlavesPoll();
	void ReplyTimeout();
private:
	void ST_Init(DynaboxData* pdata);
	void ST_CheckingLED(DynaboxData* pdata);
	void ST_CheckingElectromagnet(DynaboxData* pdata);
	void ST_Homing(DynaboxData* pdata);
	enum States {ST_INIT = 0, ST_CHECKING_LED, ST_CHECKING_ELECTROMAGNET, ST_HOMING, ST_MAX_STATES};
	BEGIN_STATE_MAP
		STATE_MAP_ENTRY(&Dynabox::ST_Init)
		STATE_MAP_ENTRY(&Dynabox::ST_CheckingLED)
		STATE_MAP_ENTRY(&Dynabox::ST_CheckingElectromagnet)
		STATE_MAP_ENTRY(&Dynabox::ST_Homing)
	END_STATE_MAP
	void (Dynabox::*pcommand)();
	void (Dynabox::*pparse)(uint8_t* frame);
	void (Dynabox::*ptimeout)();
};

extern Dynabox dynabox;
extern DynaboxData dynabox_data;

#endif /* DYNABOX_H_ */
