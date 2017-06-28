/*
 * modbus_tcp.h
 *
 *  Created on: 28 cze 2017
 *      Author: tomek
 */

#ifndef MODBUS_TCP_H_
#define MODBUS_TCP_H_
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
#define NUMBER_OF_REG_H			10
#define NUMBER_OF_REG_L 		11
// RES
#define BYTE_COUNT 				8
#define START_DATA 				9




#endif /* MODBUS_TCP_H_ */
