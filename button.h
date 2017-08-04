/*
 * button.h
 *
 *  Created on: 1 sie 2017
 *      Author: tomek
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include <inttypes.h>
#include "machine.h"

#define BUTTON_DDR 					DDRC
#define BUTTON_PORT 				PORTC
#define BUTTON_PIN					PINC
#define BUTTON_PIN_NUMBER			7

class ButtonData : public EventData
{
public:
};

class Button : public Machine
{
public:
	Button();
	// Events
	void EV_Press(ButtonData* pdata = NULL);
	// Help function
	bool CheckVal();
private:
	// States functions
	void ST_Idle(ButtonData* pdata);
	void ST_Debounce(ButtonData* pdata);
	void ST_Down(ButtonData* pdata);
	void ST_Action(ButtonData* pdata);
	enum States {ST_IDLE = 0, ST_DEBOUNCE, ST_DOWN, ST_ACTION, ST_MAX_STATES};
	BEGIN_STATE_MAP
		STATE_MAP_ENTRY(&Button::ST_Idle)
		STATE_MAP_ENTRY(&Button::ST_Debounce)
		STATE_MAP_ENTRY(&Button::ST_Down)
		STATE_MAP_ENTRY(&Button::ST_Action)
	END_STATE_MAP
};

extern Button button;
extern ButtonData button_data;

#endif /* BUTTON_H_ */
