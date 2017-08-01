/*
 * system.h
 *
 *  Created on: 8 lip 2017
 *      Author: tomek
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <inttypes.h>
#include "machine.h"

class StartupConfigData : public EventData
{
public:
};

class StartupConfig : public Machine
{
public:
	StartupConfig();
	// Events
	void EV_ButtonPress(StartupConfigData* pdata = NULL);
private:
	// States functions
	void ST_Start(StartupConfigData* pdata);
	void ST_ChoosingParameter(StartupConfigData* pdata);
	enum States {ST_START = 0, ST_CHOOSING_PARAMETER, ST_MAX_STATES};
	BEGIN_STATE_MAP
		STATE_MAP_ENTRY(&StartupConfig::ST_Start)
		STATE_MAP_ENTRY(&StartupConfig::ST_ChoosingParameter)
	END_STATE_MAP
};

extern StartupConfig startup_config;
extern StartupConfigData startup_config_data;

#endif /* SYSTEM_H_ */
