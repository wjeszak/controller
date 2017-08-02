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
	// without state machine
	void EV_EncoderLeft();
	void EV_EncoderRight();
private:
	// States functions
	void ST_Idle(ConfigData* pdata);
	void ST_ChoosingParameter(ConfigData* pdata);
	enum States {ST_START = 0, ST_CHOOSING_PARAMETER, ST_MAX_STATES};
	BEGIN_STATE_MAP
		STATE_MAP_ENTRY(&Config::ST_Idle)
		STATE_MAP_ENTRY(&Config::ST_ChoosingParameter)
	END_STATE_MAP
	uint8_t index;
};

extern Config config;
extern ConfigData config_data;

#endif /* CONFIG_H_ */
