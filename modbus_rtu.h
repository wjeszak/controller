/*
 * modbus_rtu.h
 *
 *  Created on: 27 cze 2017
 *      Author: tomek
 */

#ifndef MODBUS_RTU_H_
#define MODBUS_RTU_H_

#include <inttypes.h>

class ModbusRTU
{
public:
	ModbusRTU();
	void Poll();
	void ParseFrame(uint8_t* frame, uint8_t len);
	uint16_t Checksum(uint8_t* frame, uint8_t len);
private:
	void ReadHoldingRegisters(uint8_t* frame);
	void WriteSingleRegister(uint8_t* frame);
	void WriteSingleRegisterToElectrom(uint8_t* frame);
//	void FunctionNotSupported(uint8_t *frame);
	uint8_t slave_addr, slave_addr_max;
};

extern ModbusRTU modbus_rtu;

#endif /* MODBUS_RTU_H_ */
