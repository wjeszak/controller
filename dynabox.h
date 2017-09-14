/*
 * dynabox.h
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#ifndef DYNABOX_H_
#define DYNABOX_H_

#include "machine.h"

class DynaboxData : public EventData
{
public:
};


class Dynabox : public Machine
{
public:
	Dynabox();
	void EV_LEDChecked(DynaboxData* pdata);
	void LoadSupportedFunctions();
	void SaveParameters();
	void SendToDoor();
	void Parse(uint8_t* frame);
	void ReplyTimeout();
private:
	void ST_Init(DynaboxData* pdata);
	void ST_CheckLED(DynaboxData* pdata);
	void ST_Homing(DynaboxData* pdata);
	enum States {ST_INIT = 0, ST_CHECK_LED, ST_HOMING, ST_MAX_STATES};
	BEGIN_STATE_MAP
		STATE_MAP_ENTRY(&Dynabox::ST_Init)
		STATE_MAP_ENTRY(&Dynabox::ST_CheckLED)
		STATE_MAP_ENTRY(&Dynabox::ST_Homing)
	END_STATE_MAP
};

extern Dynabox dynabox;
extern DynaboxData dynabox_data;

#endif /* DYNABOX_H_ */
