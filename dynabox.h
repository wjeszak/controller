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
	void LoadSupportedFunctions();
	void SaveParameters();
	void SendToDoor();
	uint8_t first_door, last_door, curr_door;
private:
	void ST_Init(DynaboxData* pdata);
	void ST_TestDoors(DynaboxData* pdata);
	void ST_Homing(DynaboxData* pdata);
	void StartupTest();
	enum States {ST_INIT = 0, ST_TEST_DOORS, ST_HOMING, ST_MAX_STATES};
	BEGIN_STATE_MAP
		STATE_MAP_ENTRY(&Dynabox::ST_Init)
		STATE_MAP_ENTRY(&Dynabox::ST_TestDoors)
		STATE_MAP_ENTRY(&Dynabox::ST_Homing)
	END_STATE_MAP
};

extern Dynabox dynabox;
extern DynaboxData dynabox_data;

#endif /* DYNABOX_H_ */
