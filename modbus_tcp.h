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

#define MODBUS_TCP_ADDR_OFFSET_HOLDING_REG		100
#define MODBUS_TCP_NUMBER_OF_HOLDING_REG 		49

#define MODBUS_TCP_ADDR_OFFSET_MULTIPLE_REG		150
#define MODBUS_TCP_NUMBER_OF_MULTIPLE_REG		37

#define MODBUS_TCP_HOLDING_MAX_QUANTITY 		125
#define MODBUS_TCP_MULTIPLE_MAX_QUANTITY	 	123
// ------------------------------------------------
#define HOLDING_ORDER_STATUS 					0
	#define ORDER_STATUS_READY					0x0000
	#define ORDER_STATUS_GO_ACK 				0x0001
	#define ORDER_STATUS_PROCESSING 			0x0002
	#define ORDER_STATUS_END_OF_MOVEMENT 		0x0003
	#define ORDER_STATUS_NOT_READY 				0xFFFF
#define HOLDING_GENERAL_ERROR_STATUS 			1
	#define GENERAL_ERROR_STATUS_F10			0
	#define GENERAL_ERROR_STATUS_F11			1
	#define GENERAL_ERROR_STATUS_F12			2
	#define GENERAL_ERROR_STATUS_F13			3
	#define GENERAL_ERROR_STATUS_F14			4
	#define GENERAL_ERROR_STATUS_F15 			5
	#define GENERAL_ERROR_STATUS_F16 			6
	#define GENERAL_ERROR_STATUS_F17 			7
#define HOLDING_ACTUAL_SPEED 					44
#define HOLDING_TYPE_OF_MACHINE 				45
	#define TYPE_OF_MACHINE_NOT_DEFINED 		0
	#define TYPE_OF_MACHINE_LOCKERBOX			1
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
	void ReadHoldingRegisters(uint8_t* frame);
	void WriteMultipleRegisters(uint8_t* frame);
	void UpdateHoldingRegisters(uint8_t address, uint16_t value);
private:
	void PrepareMBAPHeader(uint8_t* frame);
	void ReturnHoldingRegisters(uint8_t* frame, uint16_t starting_address, uint16_t quantity);
	void UpdateMultipleRegisters(uint8_t* frame, uint16_t starting_address, uint16_t quantity);
	void ReadHoldingRegistersReply(uint8_t* frame);
	void WriteMultipleRegistersReply(uint8_t* frame);
	void SendErrorFrame(uint8_t* frame, uint8_t error_code);
	void AnalizeMultipleRegisters();
	uint16_t HoldingRegisters[MODBUS_TCP_NUMBER_OF_HOLDING_REG];
	uint16_t MultipleRegisters[MODBUS_TCP_NUMBER_OF_MULTIPLE_REG];
	uint16_t trans_id;
	uint16_t prot_id;
	uint8_t  unit_id;
	uint8_t  function_code;
	uint16_t starting_address;
	uint16_t quantity;
};

extern ModbusTCP modbus_tcp;

#endif /* MODBUS_TCP_H_ */
