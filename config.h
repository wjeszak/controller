/*
 * config.h
 *
 *  Created on: 8 lip 2017
 *      Author: tomek
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <inttypes.h>
#include "state_machine.h"
#include "encoder.h"

#define MAX_NUMBER_OF_FUNCTIONS 			28

typedef void (*fp)(uint8_t value);

struct Function
{
	uint8_t id;
	uint16_t param;
	fp f;
};

extern Function functions[MAX_NUMBER_OF_FUNCTIONS];

class ConfigData : public EventData
{
public:
	uint8_t val;
};

class Config : public StateMachine
{
public:
	Config();
//	void SetSupportedFunctions(uint8_t number_of_functions);
	// Events
	void EV_ButtonClick(ConfigData* pdata);
	void EV_Encoder(ConfigData* pdata);
	void EV_EncoderClick(ConfigData* pdata = NULL);
	uint8_t number_of_functions;
	bool need_stack_poll;
private:
	// States functions
	void ST_Idle(ConfigData* pdata);
	void ST_ChoosingFunction(ConfigData* pdata);
	void ST_ExecutingFunction(ConfigData* pdata);
	enum States {ST_IDLE = 0, ST_CHOOSING_FUNCTION, ST_EXECUTING_FUNCTION, ST_MAX_STATES};
	BEGIN_STATE_MAP
		STATE_MAP_ENTRY(&Config::ST_Idle)
		STATE_MAP_ENTRY(&Config::ST_ChoosingFunction)
		STATE_MAP_ENTRY(&Config::ST_ExecutingFunction)
	END_STATE_MAP
	uint8_t index;
};

extern Config config;
extern ConfigData config_data;
extern void test();

#endif /* CONFIG_H_ */
