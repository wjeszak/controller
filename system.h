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
//	void ST_ButtonDebounce(StartupConfigData* pdata);
//	void ST_ButtonDown(StartupConfigData* pdata);
//	void ST_Configuration(StartupConfigData* pdata);
	enum States {ST_START = 0, ST_BUTTON_DEBOUNCE, ST_BUTTON_DOWN, ST_CONFIGURATION, ST_MAX_STATES};
	BEGIN_STATE_MAP
		STATE_MAP_ENTRY(&StartupConfig::ST_Start)
//		STATE_MAP_ENTRY(&StartupConfig::ST_ButtonDebounce)
//		STATE_MAP_ENTRY(&StartupConfig::ST_ButtonDown)
//		STATE_MAP_ENTRY(&StartupConfig::ST_Configuration)
	END_STATE_MAP
};

extern StartupConfig startup_config;
extern StartupConfigData startup_config_data;

#endif /* SYSTEM_H_ */
