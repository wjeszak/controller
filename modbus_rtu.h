/*
 * modbus_rtu.h
 *
 *  Created on: 27 cze 2017
 *      Author: tomek
 */

#ifndef MODBUS_RTU_H_
#define MODBUS_RTU_H_

#include <inttypes.h>

#define MODBUS_RTU_ADDR_ID							0
#define MODBUS_RTU_FUNCTION_ID 						1

// read from slave - request
#define MODBUS_RTU_REQ_HOLDING_STARTING_ADDR_HI 	2
#define MODBUS_RTU_REQ_HOLDING_STARTING_ADDR_LO 	3
#define MODBUS_RTU_REQ_HOLDING_QUANTITY_HI 			4
#define MODBUS_RTU_REQ_HOLDING_QUANTITY_LO 			5

// read from slave - response
#define MODBUS_RTU_RES_HOLDING_BYTE_COUNT	 		2
#define MODBUS_RTU_RES_HOLDING_REG_VAL_HI 			3
#define MODBUS_RTU_RES_HOLDING_REG_VAL_LO 			4

// write to slave - request & response
#define MODBUS_RTU_SINGLE_REG_ADDR_HI 				2
#define MODBUS_RTU_SINGLE_REG_ADDR_LO 				3
#define MODBUS_RTU_SINGLE_REG_VAL_HI 				4
#define MODBUS_RTU_SINGLE_REG_VAL_LO 				5

#define MODBUS_RTU_REQ_CRC_LO 						6
#define MODBUS_RTU_REQ_CRC_HI 						7

#define MODBUS_RTU_HOLDING_FUNCTION					3
#define MODBUS_RTU_SINGLE_FUNCTION 					6

#define MODBUS_RTU_REQ_LEN 							8

#define MODBUS_RTU_LED_OFFSET 						100
class ModbusRTU
{
public:
	ModbusRTU();
	void PrepareFrameHolding(uint8_t* frame, uint8_t address, uint8_t function);
	void PrepareFrameSingle(uint8_t* frame, uint8_t address, uint8_t function, uint16_t value);
	void PrepareFrameCRC(uint8_t* frame);
	void PollDoors();
	void ParseFrame(uint8_t* frame, uint8_t len);
	uint16_t Checksum(uint8_t* frame, uint8_t len);
private:
	void ReadHolding(uint8_t* frame);
	void WriteSingle(uint8_t* frame);
//	void FunctionNotSupported(uint8_t *frame);
	uint8_t slave_addr, slave_addr_max;
};

extern ModbusRTU modbus_rtu;

#endif /* MODBUS_RTU_H_ */
