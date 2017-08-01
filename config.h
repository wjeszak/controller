/*
 * config.h
 *
 *  Created on: 8 lip 2017
 *      Author: tomek
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <inttypes.h>
#include "machine.h"

class ConfigData : public EventData
{
public:
};

class Config : public Machine
{
public:
	Config();
	// Events
	void EV_ButtonPress(ConfigData* pdata = NULL);
private:
	// States functions
	void ST_Start(ConfigData* pdata);
	void ST_ChoosingParameter(ConfigData* pdata);
	enum States {ST_START = 0, ST_CHOOSING_PARAMETER, ST_MAX_STATES};
	BEGIN_STATE_MAP
		STATE_MAP_ENTRY(&Config::ST_Start)
		STATE_MAP_ENTRY(&Config::ST_ChoosingParameter)
	END_STATE_MAP
};

extern Config config;
extern ConfigData config_data;

#endif /* CONFIG_H_ */
