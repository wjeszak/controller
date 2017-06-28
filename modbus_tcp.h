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
#define NUMBER_OF_HOLDING_REGISTERS_TCP 		20
#define ADDR_OFFSET 							100

class ModbusTcp
{
public:
	void ParseFrame(uint8_t* frame);
	uint8_t ReadHoldingRegisters(uint8_t* frame);
	void PrepareFrame(uint8_t* frame);
private:
	uint16_t HoldingRegisters[NUMBER_OF_HOLDING_REGISTERS_TCP];
	uint16_t trans_id;
	uint8_t unit_id;
	uint8_t function_code;
	uint16_t starting_address;
	uint16_t quantity;
	//uint8_t byte_count;
	uint16_t length;
};

extern ModbusTcp modbus_tcp;

#endif /* MODBUS_TCP_H_ */
