/*
 * stack.h
 *
 *  Created on: 26 maj 2017
 *      Author: tomek
 */

#ifndef STACK_H_
#define STACK_H_
#include <inttypes.h>

class Stack
{
public:
	Stack();
	void StackPoll();
private:
	uint8_t EthTypeIsArpMyIP(uint8_t *buf, uint16_t len);
	uint8_t EthTypeIsIPMyIP(uint8_t *buf, uint16_t len);
	void MakeArpReply(uint8_t *buf);
	void MakeIcmpReply(uint8_t *buf, uint16_t len);
	void MakeEthHeader(uint8_t *buf);
	void MakeIpHeader(uint8_t *buf);
	void MakeTcpHeader(uint8_t *buf,uint16_t rel_ack_num,uint8_t cp_seq);
	void MakeTcpSynAckFromSyn(uint8_t *buf);
	uint16_t Checksum(uint8_t *buf, uint16_t len,uint8_t type);
	void FillIpHeaderChecksum(uint8_t *buf);
	void step_seq(uint8_t *buf,uint16_t rel_ack_num,uint8_t cp_seq);
	uint8_t buf[1500];
	uint16_t packet_len;
	uint8_t mac_address[6];
	uint8_t ip_address[4];
};

// ******* ETH *******
#define ETH_HEADER_LEN						14
// type of packet
#define ETHTYPE_ARP_H_V 					0x08
#define ETHTYPE_ARP_L_V 					0x06
#define ETHTYPE_IP_H_V  					0x08
#define ETHTYPE_IP_L_V  					0x00
// Byte positions in the ethernet frame:
// Ethernet type field (2 bytes):
#define ETH_DST_MAC 						0
#define ETH_SRC_MAC 						6
#define ETH_TYPE_H_P 						12
#define ETH_TYPE_L_P 						13

// ******* ARP *******
#define ETH_ARP_OPCODE_REPLY_H_V 			0x00
#define ETH_ARP_OPCODE_REPLY_L_V 			0x02
#define ETH_ARP_OPCODE_REQ_H_V 				0x00
#define ETH_ARP_OPCODE_REQ_L_V 				0x01
// start of arp header:
#define ETH_ARP_P 							0xE
//
#define ETHTYPE_ARP_L_V 					0x06
// arp.dst.ip
#define ETH_ARP_DST_IP_P 					0x26
// arp.opcode
#define ETH_ARP_OPCODE_H_P 					0x14
#define ETH_ARP_OPCODE_L_P 					0x15
// arp.src.mac
#define ETH_ARP_SRC_MAC_P 					0x16
#define ETH_ARP_SRC_IP_P 					0x1C
#define ETH_ARP_DST_MAC_P 					0x20
#define ETH_ARP_DST_IP_P 					0x26

// ******* IP *******
#define IP_HEADER_LEN						20
// ip.src
#define IP_SRC_P 							0x1A
#define IP_DST_P 							0x1E
#define IP_HEADER_LEN_VER_P 				0xE
#define IP_CHECKSUM_P 						0x18
#define IP_TTL_P 							0x16
#define IP_FLAGS_P 							0x14
#define IP_P 								0xE
#define IP_TOTLEN_H_P 						0x10
#define IP_TOTLEN_L_P						0x11
#define IP_ID_H_P 							0x12
#define IP_ID_L_P 							0x13

#define IP_PROTO_P 							0x17

#define IP_PROTO_ICMP_V 					1
#define IP_PROTO_TCP_V 						6
// 17=0x11
#define IP_PROTO_UDP_V 						17
// ******* ICMP *******
#define ICMP_TYPE_ECHOREPLY_V 				0
#define ICMP_TYPE_ECHOREQUEST_V 			8
//
#define ICMP_TYPE_P 						0x22
#define ICMP_CHECKSUM_P 					0x24
#define ICMP_CHECKSUM_H_P 					0x24
#define ICMP_CHECKSUM_L_P 					0x25
#define ICMP_IDENT_H_P 						0x26
#define ICMP_IDENT_L_P 						0x27
#define ICMP_DATA_P 						0x2A

// ******* TCP *******
#define TCP_SRC_PORT_H_P 					0x22
#define TCP_SRC_PORT_L_P 					0x23
#define TCP_DST_PORT_H_P 					0x24
#define TCP_DST_PORT_L_P 					0x25
// the tcp seq number is 4 bytes 0x26-0x29
#define TCP_SEQ_H_P 						0x26
#define TCP_SEQACK_H_P 						0x2A
// flags: SYN=2
#define TCP_FLAGS_P 						0x2F
#define TCP_FLAGS_SYN_V 					2
#define TCP_FLAGS_FIN_V 					1
#define TCP_FLAGS_RST_V 					4
#define TCP_FLAGS_PUSH_V 					8
#define TCP_FLAGS_SYNACK_V 					0x12
#define TCP_FLAGS_ACK_V 					0x10
#define TCP_FLAGS_PSHACK_V					0x18
//  plain len without the options:
#define TCP_HEADER_LEN_PLAIN 				20
#define TCP_HEADER_LEN_P 					0x2E
#define TCP_WIN_SIZE 						0x30
#define TCP_CHECKSUM_H_P 					0x32
#define TCP_CHECKSUM_L_P 					0x33
#define TCP_OPTIONS_P 						0x36

extern Stack stack;

#endif /* STACK_H_ */
