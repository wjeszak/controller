/*
 * modbus_tcp.cpp
 *
 *  Created on: 28 cze 2017
 *      Author: tomek
 */

#include "modbus_tcp.h"
#include "display.h"

void ModbusTcp::ParseFrame(uint8_t* frame)
{
	// Modbus TCP frame ?
	if((frame[PROT_ID_H] == 0) && (frame[PROT_ID_L] == 0))
	{
		switch(frame[FUNCTION_CODE])
		{
			case 3:
				display.Write(333);
				ReadHoldingRegisters(frame);
			break;
			//default:
				//FunctionNotSupported(frame);
		}
	}
}

uint8_t ModbusTcp::ReadHoldingRegisters(uint8_t* frame)
{
	starting_address = frame[ADDR_FIRST_H] << 8 | frame[ADDR_FIRST_L];
	quantity 		 = frame[QUANTITY_H] << 8 | frame[QUANTITY_L];
	trans_id 		 = frame[TRANS_ID_L]; 		// zmienic
	uint8_t error_code = 0;

	//if((quantity > 125) || (quantity < 1)) error_code = 3;
	//if((starting_address + quantity) > NUMBER_OF_HOLDING_REGISTERS_TCP) error_code = 2;

	//if(error_code)
	//{
		frame[TRANS_ID_H] = 0;
		frame[TRANS_ID_L] = trans_id;
		frame[PROT_ID_H] = 0;
		frame[PROT_ID_L] = 0;
		frame[LENGTH_H] = 0; 		// to jest do przerobienia
		frame[LENGTH_L] = 5;//(uint8_t)(quantity * 2) + 3;
		frame[UNIT_ID] = 1;
		frame[FUNCTION_CODE] = 3;
		frame[BYTE_COUNT] = 2;//(uint8_t)(quantity * 2);

		for(uint8_t i = 0; i < 1; i++)
		//for(uint8_t i = 0; i < (uint8_t)(quantity); i++)
		{
			frame[START_DATA + 2 * i] = 0;//Msb(i);
			frame[START_DATA + (2 * i) + 1] = 73;//Lsb(i);
		}

		//return error_code;
	//}
	return 0;
}
/*
void ModbusTcp::ParseFrame(uint8_t* buf)
{
	uint8_t frame[100];
	frame[TRANS_ID_H] = 0;
	frame[TRANS_ID_L] = 1;
	frame[PROT_ID_H] = 0;
	frame[PROT_ID_L] = 0;
	frame[LENGTH_H] = 0;
	frame[LENGTH_L] = (buf[TCP_CHECKSUM_L_P + 3 + NUMBER_OF_REG_L] * 2) + 3; // 4b
	frame[UNIT_ID] = 1;
	frame[FUNCTION_CODE] = 3;
	frame[BYTE_COUNT] = buf[TCP_CHECKSUM_L_P + 3 + NUMBER_OF_REG_L] * 2;
	//uint16_t wart =
	for(uint8_t i = 0; i < buf[TCP_CHECKSUM_L_P + 3 + NUMBER_OF_REG_L]; i++)
	{
		frame[START_DATA + 2 * i] = Msb(i);
		frame[START_DATA + (2 * i) + 1] = Lsb(i);
	}
	uint8_t j = 0;
	j = (1 << 1);
	frame[START_DATA] = Msb(j);
	frame[START_DATA +1] = Lsb(j);
	FillTcpData(buf, 0, frame, frame[LENGTH_L] + 6);
	buf[TCP_FLAGS_P] = TCP_FLAGS_PUSH_V | TCP_FLAGS_ACK_V; //| TCP_FLAGS_FIN_V;
	MakeTcpAckWithDataNoFlags(buf, frame[LENGTH_L] + 6);
}
*/
