/*
 * dynabox_commands.h
 *
 *  Created on: 14 gru 2017
 *      Author: tomek
 */

#ifndef DYNABOX_COMMANDS_FAULTS_H_
#define DYNABOX_COMMANDS_FAULTS_H_

// door's commands
#define COMM_DOOR_CHECK_ELM			 				0x01
#define COMM_DOOR_REPLY_ELM_OK 						0x00
#define COMM_DOOR_REPLY_ELM_FAULT 					0x01

#define COMM_DOOR_GET_STATUS 						0x80
#define COMM_DOOR_REPLY_TRANSOPT_FAULT 				0xF0
#define COMM_DOOR_SET_POSITION 						0xC0

// led's commands
#define LED_ADDRESS_OFFSET 							100
#define LED_ADDRESS_TRIGGER 						0xFF

#define COMM_LED_GREEN_RED_OFF 						0x00
#define COMM_LED_GREEN_ON 							0x01
#define COMM_LED_RED_ON 							0x02
#define COMM_LED_GREEN_BLINK 						0x03
#define COMM_LED_RED_BLINK 							0x04
#define COMM_LED_GREEN_RED_BLINK 					0x05
#define COMM_LED_GREEN_1PULSE 						0x06
#define COMM_LED_RED_1PULSE 						0x07
#define COMM_LED_GREEN_2PULSES 						0x08
#define COMM_LED_RED_2PULSES 						0x09
#define COMM_LED_GREEN_3PULSES 						0x0A
#define COMM_LED_RED_3PULSES 						0x0B
#define COMM_LED_GREEN_ON_FOR_TIME 					0x0C
#define COMM_LED_DIAG								0x0D
#define COMM_LED_NEED_QUEUE_BIT						7

#define COMM_F05_ELECTROMAGNET 						0x01

// faults
#define NUMBER_OF_FAULTS 							17
#define NO_FAULT									0x00
#define F01_LED										0x01
#define F02_DOOR									0x02
#define F03_OPTICAL_SWITCHES						0x03
#define F04_DOOR_OPENED_TOO_FAR						0x04
#define F05_ELECTROMAGNET							0x05
#define F06_CLOSE_THE_DOOR							0x06
#define F07_DOOR_NOT_OPEN							0x07
#define F08_ILLEGAL_OPENING							0x08
#define F10_MECHANICAL_WARNING						0x0A
#define F11_MECHANICAL_FAULT						0x0B
#define F12_POSITIONING								0x0C
#define F13_MAIN_DOOR								0x0D
#define F14_HOMING_FAILED							0x0E
#define F15_ILLEGAL_ROTATION						0x0F
#define F16_ORDER_REFUSED							0x10
#define F17_24V_MISSSING							0x11

#endif /* DYNABOX_COMMANDS_FAULTS_H_ */
