/*
 * stos.h
 *
 *  Created on: 26 maj 2017
 *      Author: tomek
 */

#ifndef STOS_H_
#define STOS_H_
#include <inttypes.h>

class Stos
{
public:
	uint8_t eth_type_is_arp_and_my_ip(uint8_t *buf, uint16_t len);
	uint8_t eth_type_is_ip_and_my_ip(uint8_t *buf, uint16_t len);
	void make_arp_answer_from_request(uint8_t *buf);
	void make_echo_reply_from_request(uint8_t *buf, uint16_t len);
	void make_tcp_synack_from_syn(uint8_t *buf);
private:
	void make_eth(uint8_t *buf);
	void make_ip(uint8_t *buf);
	uint16_t checksum(uint8_t *buf, uint16_t len,uint8_t type);
	void fill_ip_hdr_checksum(uint8_t *buf);
	void step_seq(uint8_t *buf,uint16_t rel_ack_num,uint8_t cp_seq);
	void make_tcphead(uint8_t *buf,uint16_t rel_ack_num,uint8_t cp_seq);

};
#define ADR_MAC1	0x00
#define ADR_MAC2	0x20
#define ADR_MAC3	0x18
#define ADR_MAC4	0xB1
#define ADR_MAC5	0x15
#define ADR_MAC6	0x6F

#define ADR_IP1		192
#define ADR_IP2 	168
#define ADR_IP3 	1
#define ADR_IP4 	170

// ******* ETH *******
#define ETH_HEADER_LEN	14
// values of certain bytes:
#define ETHTYPE_ARP_H_V 0x08
#define ETHTYPE_ARP_L_V 0x06
#define ETHTYPE_IP_H_V  0x08
#define ETHTYPE_IP_L_V  0x00
// byte positions in the ethernet frame:
//
// Ethernet type field (2bytes):
#define ETH_TYPE_H_P 12
#define ETH_TYPE_L_P 13
//
#define ETH_DST_MAC 0
#define ETH_SRC_MAC 6


// ******* ARP *******
#define ETH_ARP_OPCODE_REPLY_H_V 0x0
#define ETH_ARP_OPCODE_REPLY_L_V 0x02
#define ETH_ARP_OPCODE_REQ_H_V 0x0
#define ETH_ARP_OPCODE_REQ_L_V 0x01
// start of arp header:
#define ETH_ARP_P 0xe
//
#define ETHTYPE_ARP_L_V 0x06
// arp.dst.ip
#define ETH_ARP_DST_IP_P 0x26
// arp.opcode
#define ETH_ARP_OPCODE_H_P 0x14
#define ETH_ARP_OPCODE_L_P 0x15
// arp.src.mac
#define ETH_ARP_SRC_MAC_P 0x16
#define ETH_ARP_SRC_IP_P 0x1c
#define ETH_ARP_DST_MAC_P 0x20
#define ETH_ARP_DST_IP_P 0x26

// ******* IP *******
#define IP_HEADER_LEN	20
// ip.src
#define IP_SRC_P 0x1a
#define IP_DST_P 0x1e
#define IP_HEADER_LEN_VER_P 0xe
#define IP_CHECKSUM_P 0x18
#define IP_TTL_P 0x16
#define IP_FLAGS_P 0x14
#define IP_P 0xe
#define IP_TOTLEN_H_P 0x10
#define IP_TOTLEN_L_P 0x11
#define IP_ID_H_P 0x12
#define IP_ID_L_P 0x13

#define IP_PROTO_P 0x17

#define IP_PROTO_ICMP_V 1
#define IP_PROTO_TCP_V 6
// 17=0x11
#define IP_PROTO_UDP_V 17
// ******* ICMP *******
#define ICMP_TYPE_ECHOREPLY_V 0
#define ICMP_TYPE_ECHOREQUEST_V 8
//
#define ICMP_TYPE_P 0x22
#define ICMP_CHECKSUM_P 0x24
#define ICMP_CHECKSUM_H_P 0x24
#define ICMP_CHECKSUM_L_P 0x25
#define ICMP_IDENT_H_P 0x26
#define ICMP_IDENT_L_P 0x27
#define ICMP_DATA_P 0x2a

// ******* TCP *******
#define TCP_SRC_PORT_H_P 0x22
#define TCP_SRC_PORT_L_P 0x23
#define TCP_DST_PORT_H_P 0x24
#define TCP_DST_PORT_L_P 0x25
// the tcp seq number is 4 bytes 0x26-0x29
#define TCP_SEQ_H_P 0x26
#define TCP_SEQACK_H_P 0x2a
// flags: SYN=2
#define TCP_FLAGS_P 0x2f
#define TCP_FLAGS_SYN_V 2
#define TCP_FLAGS_FIN_V 1
#define TCP_FLAGS_RST_V 4
#define TCP_FLAGS_PUSH_V 8
#define TCP_FLAGS_SYNACK_V 0x12
#define TCP_FLAGS_ACK_V 0x10
#define TCP_FLAGS_PSHACK_V 0x18
//  plain len without the options:
#define TCP_HEADER_LEN_PLAIN 20
#define TCP_HEADER_LEN_P 0x2e
#define TCP_WIN_SIZE 0x30
#define TCP_CHECKSUM_H_P 0x32
#define TCP_CHECKSUM_L_P 0x33
#define TCP_OPTIONS_P 0x36

//extern void Stos_Init(uint8_t *moj_adres_MAC, uint8_t *moj_adres_IP, uint16_t port);
//extern void LAN_Start();

#endif /* STOS_H_ */
