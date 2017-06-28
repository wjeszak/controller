/*
 * modbus_tcp.h
 *
 *  Created on: 28 cze 2017
 *      Author: tomek
 */

#ifndef MODBUS_TCP_H_
#define MODBUS_TCP_H_

#include <inttypes.h>
// 7 bytes MBAP
// REQ
#define TRANS_ID_H 				0
#define TRANS_ID_L 				1
#define PROT_ID_H 				2
#define PROT_ID_L 				3
#define LENGTH_H				4
#define LENGTH_L				5
#define UNIT_ID					6
// PDU
#define FUNCTION_CODE			7
#define ADDR_FIRST_H 			8
#define ADDR_FIRST_L 			9
#define QUANTITY_H				10
#define QUANTITY_L	 			11
// RES
#define BYTE_COUNT 				8
#define START_DATA 				9

#define NUMBER_OF_HOLDING_REGISTERS_TCP 	20

class ModbusTcp
{
public:
	void ParseFrame(uint8_t* frame);
	uint8_t ReadHoldingRegisters(uint8_t* frame);
private:
	uint16_t starting_address;
	uint16_t quantity;
	uint16_t trans_id;
	uint8_t byte_count;
};

extern ModbusTcp modbus_tcp;

#endif /* MODBUS_TCP_H_ */
