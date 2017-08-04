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

#define FUNCTION_NOT_SUPPORTED 			0xFF

class ConfigData : public EventData
{
public:
	uint8_t val;
};

class Config : public Machine
{
public:
	Config();
	// Events
	void EV_ButtonPress(ConfigData* pdata = NULL);
	void EV_Encoder(ConfigData* pdata);
	void EV_EncoderClick(ConfigData* pdata = NULL);
private:
	// States functions
	void ST_Init(ConfigData* pdata);
	void ST_ChoosingFunction(ConfigData* pdata);
	void ST_ExecutingFunction(ConfigData* pdata);
	void ST_Done(ConfigData* pdata);
	enum States {ST_INIT = 0, ST_CHOOSING_FUNCTION, ST_EXECUTING_FUNCTION, ST_DONE, ST_MAX_STATES};
	BEGIN_STATE_MAP
		STATE_MAP_ENTRY(&Config::ST_Init)
		STATE_MAP_ENTRY(&Config::ST_ChoosingFunction)
		STATE_MAP_ENTRY(&Config::ST_ExecutingFunction)
		STATE_MAP_ENTRY(&Config::ST_Done)
	END_STATE_MAP
	uint8_t GetTypeOfFunction(uint8_t id);
	uint8_t index;
};

extern Config config;
extern ConfigData config_data;
extern void test();
#endif /* CONFIG_H_ */
