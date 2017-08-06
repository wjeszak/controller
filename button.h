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
#include "config.h"

#define BUTTON_ENTER_CONFIG_DDR					DDRC
#define BUTTON_ENTER_CONFIG_PORT				PORTC
#define BUTTON_ENTER_CONFIG_PIN					PINC
#define BUTTON_ENTER_CONFIG_PIN_NUMBER			7

#define BUTTON_ENCODER_SW_DDR 					DDRD
#define BUTTON_ENCODER_SW_PORT					PORTD
#define BUTTON_ENCODER_SW_PIN 					PIND
#define BUTTON_ENCODER_SW_PIN_NUMBER			2

class ButtonData : public EventData
{
public:
};

class Button : public Machine
{
public:
	Button(volatile uint8_t *ddr, volatile uint8_t *port, volatile uint8_t *pin, uint8_t pin_number, uint16_t time_to_action, void (Config::*f)(ConfigData*));
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
	volatile uint8_t *_ddr;
	volatile uint8_t *_port;
	volatile uint8_t *_pin;
	uint8_t _pin_number;
	uint16_t _time_to_action;
	void (Config::*_f)(ConfigData*);
};

extern Button button_enter_config;
extern Button button_encoder_sw;
extern ButtonData button_data;

#endif /* BUTTON_H_ */
