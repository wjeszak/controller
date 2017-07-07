/*
 * modbus_tcp.h
 *
 *  Created on: 28 cze 2017
 *      Author: tomek
 */

#ifndef MODBUS_TCP_H_
#define MODBUS_TCP_H_

#include <inttypes.h>
// Modbus TCP frame format
// 7 bytes MBAP
// REQ, RES
#define TRANSACTION_ID_H 						0
#define TRANSACTION_ID_L 						1
#define PROTOCOL_ID_H 							2
#define PROTOCOL_ID_L 							3
#define LENGTH_H								4
#define LENGTH_L								5
#define UNIT_ID									6
// PDU
#define FUNCTION_CODE							7
#define START_ADDR_H 							8
#define START_ADDR_L 							9
#define QUANTITY_H								10
#define QUANTITY_L	 							11
// RES
#define BYTE_COUNT 								8
#define DATA 									9

#define UNIT_ID_FUNCTION_CODE_BYTE_COUNT_LEN 	3
#define MBAP_FUNCTION_CODE_BYTE_COUNT_LEN 		9

#define FUNCTION_CODE_READ_HOLDING 				3
#define FUNCTION_CODE_WRITE_MULTIPLE 			16

#define ADDR_OFFSET_READ_HOLDING_REG			100
#define NUMBER_OF_HOLDING_REG_TCP		 		49

#define ADDR_OFFSET_WRITE_MULTIPLE_REG 			150
#define NUMBER_OF_MULTIPLE_REG_TCP				37

#define HOLDING_ORDER_STATUS 					0
#define HOLDING_GENERAL_ERROR_STATUS 			1
#define HOLDING_ACTUAL_SPEED 					44
#define HOLDING_TYPE_OF_MACHINE 				45
#define HOLDING_SERIAL_NUMBER 					46
#define HOLDING_IO_INFORMATIONS					47
#define HOLDING_ENCODER_CURRENT_VALUE_HI 		48
#define HOLDING_ENCODER_CURRENT_VALUE_LO 		49

#define MULTIPLE_FIRST_DOOR_NUMBER 				0
#define MULTIPLE_LOCATIONS_NUMBER 				1

class ModbusTCP
{
public:
	ModbusTCP();
	void Process(uint8_t* frame);
	void WhatKindOfFunction(uint8_t* frame);
	uint8_t ReadHoldingRegisters(uint8_t* frame);
	uint8_t WriteMultipleRegisters(uint8_t* frame);
	void PrepareFrame(uint8_t* frame);
	void UpdateMultiple(uint8_t address, uint16_t value);
private:
	uint16_t HoldingRegisters[NUMBER_OF_HOLDING_REG_TCP];
	uint16_t MultipleRegisters[NUMBER_OF_MULTIPLE_REG_TCP];
	uint16_t trans_id;
	uint16_t prot_id;
	uint8_t unit_id;
	uint8_t function_code;
	uint16_t starting_address;
	uint16_t quantity;
	uint16_t length;
};

extern ModbusTCP modbus_tcp;

#endif /* MODBUS_TCP_H_ */
