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
#include "encoder.h"

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
	void EV_EncoderLeft(ConfigData* pdata);
	void EV_EncoderRight(ConfigData* pdata);
	void EV_EncoderClick(ConfigData* pdata = NULL);
private:
	// States functions
	void ST_Init(ConfigData* pdata);
	void ST_Idle(ConfigData* pdata);
	void ST_ChoosingFunction(ConfigData* pdata);
	void ST_ExecutingFunction(ConfigData* pdata);
	enum States {ST_INIT = 0, ST_IDLE, ST_CHOOSING_FUNCTION, ST_EXECUTING_FUNCTION, ST_MAX_STATES};
	BEGIN_STATE_MAP
		STATE_MAP_ENTRY(&Config::ST_Init)
		STATE_MAP_ENTRY(&Config::ST_Idle)
		STATE_MAP_ENTRY(&Config::ST_ChoosingFunction)
		STATE_MAP_ENTRY(&Config::ST_ExecutingFunction)
	END_STATE_MAP
	uint8_t index;
};

extern Config config;
extern ConfigData config_data;

#endif /* CONFIG_H_ */
