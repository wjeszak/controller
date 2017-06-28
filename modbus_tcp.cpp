/*
 * modbus_tcp.cpp
 *
 *  Created on: 28 cze 2017
 *      Author: tomek
 */

#include "modbus_tcp.h"
/*
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
*/

