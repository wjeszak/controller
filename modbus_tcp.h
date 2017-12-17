/*
 * modbus_tcp.h
 *
 *  Created on: 28 cze 2017
 *      Author: tomek
 */

#ifndef MODBUS_TCP_H_
#define MODBUS_TCP_H_

#include <inttypes.h>

// ModbusTCP frame format
// 7 bytes MBAP
// REQ, RES
#define MODBUS_TCP_TRANSACTION_ID_HI 			0
#define MODBUS_TCP_TRANSACTION_ID_LO 			1
#define MODBUS_TCP_PROTOCOL_ID_HI 				2
#define MODBUS_TCP_PROTOCOL_ID_LO				3
	#define PROTOCOL_ID_MODBUS_TCP				0x0000
#define MODBUS_TCP_LENGTH_HI					4
#define MODBUS_TCP_LENGTH_LO					5
#define MODBUS_TCP_UNIT_ID						6
// ----------------------------------------------
// PDU
#define MODBUS_TCP_FUNCTION						7
#define MODBUS_TCP_START_ADDR_HI 				8
#define MODBUS_TCP_START_ADDR_LO 				9
#define MODBUS_TCP_QUANTITY_HI					10
#define MODBUS_TCP_QUANTITY_LO	 				11
#define MODBUS_REQ_TCP_BYTE_COUNT				12		// for multiple registers
#define MODBUS_REQ_TCP_REG_VAL_HI 				13		// for multiple registers
#define MODBUS_REQ_TCP_REG_VAL_LO 				14		// for multiple registers
// RES
#define MODBUS_RES_TCP_BYTE_COUNT 				8
#define MODBUS_RES_TCP_DATA 					9
// ----------------------------------------------
#define UNIT_ID_FUNCTION_BYTE_COUNT_LEN 		3
#define MBAP_FUNCTION_BYTE_COUNT_LEN 			9

#define MODBUS_TCP_HOLDING_FUNCTION 			3
#define MODBUS_TCP_WRITE_MULTIPLE_FUNCTION		16
// ----------------------------------------------
#define MODBUS_TCP_ADDR_OFFSET					100
#define MODBUS_TCP_NUMBER_OF_REG 				100
// ----------------------------------------------
#define MODBUS_TCP_HOLDING_MAX_QUANTITY 		125
#define MODBUS_TCP_MULTIPLE_MAX_QUANTITY	 	123

#define MODBUS_TCP_ERROR_ILL_DATA_ADDR 			2
#define MODBUS_TCP_ERROR_ILL_DATA_VAL 			3
// ------------------------------------------------
// Modbus TCP registers names
#define ORDER_STATUS		 					0
#define GENERAL_ERROR_STATUS		 			1
#define ACTUAL_SPEED		 					44
#define TYPE_OF_MACHINE			 				45
	#define TYPE_OF_MACHINE_NOT_DEFINED 		0
	#define TYPE_OF_MACHINE_LOCKERBOX			1
#define SERIAL_NUMBER		 					46
#define IO_INFORMATIONS							47
#define ENCODER_CURRENT_VALUE			 		49
#define FIRST_DOOR_NUMBER 						50
#define LOCATIONS_NUMBER 						51

#define ILLEGAL_ROTATION_DISTANCE 				88
#define OFFSET_FOR_ENCODER 						89
#define SPEED_MANUAL 							90
#define SPEED_NORMAL 							91
#define ACCELERATION_PULSES 					92
#define DECELERATION_PULSES 					93
#define MAX_ELM_ON 								94
#define TIME_FOR_ELM_ON 						95
#define POS_FAULT_DISTANCE 						96
#define LED_BRIGHTNESS 							97
#define TIME_FOR_MECH_WARNING 					98
#define TIME_FOR_MECH_FAULT 					99

class ModbusTCP
{
public:
	ModbusTCP();
	void Process(uint8_t* frame);
	uint16_t Read(uint8_t address);					// from controller
	void Write(uint8_t address, uint16_t value);	// from controller
	void SetBit(uint8_t address, uint8_t bit);		// from controller
	void ClearBit(uint8_t address, uint8_t bit);	// from controller
	void Read(uint8_t* frame);						// from user
	void Write(uint8_t* frame);						// from user
private:
	void MakeMBAPHeader(uint8_t* frame);
	void ReadReply(uint8_t* frame);
	void WriteReply(uint8_t* frame);
	void ErrorReply(uint8_t* frame, uint8_t error_code);
	uint16_t trans_id;
	uint16_t prot_id;
	uint8_t  unit_id;
	uint8_t  function_code;
	uint16_t starting_address;
	uint16_t quantity;
	uint16_t Registers[MODBUS_TCP_NUMBER_OF_REG];
};

extern ModbusTCP mb;

#endif /* MODBUS_TCP_H_ */
