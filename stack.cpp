/*
 * stack.cpp
 *
 * Created on: 26 maj 2017
 * Author: tomek
 */

#include "stack.h"
#include "usart.h"
#include "modbus_tcp.h"

Enc28j60 enc28j60;

Stack::Stack() : StateMachine(ST_MAX_STATES)
{
	enc28j60.Init();
	packet_len = 0;
	seqnum  = 0xA;
	port = 502;
}

void Stack::Poll()
{
	packet_len = enc28j60.ReceivePacket(buf, MAX_PACKET_SIZE);
	if(packet_len != 0)
	{
		// ARP
		if(EthTypeIsArpMyIP(buf, packet_len))
		{
			MakeArpReply(buf);
		}
		// IP
		if(EthTypeIsIPMyIP(buf, packet_len))
		{
			// ICMP
			if(EthTypeIsIcmp(buf))
			{
				MakeIcmpReply(buf, packet_len);
			}
			// TCP
			if((buf[TCP_DST_PORT_H_P] == (port >> 8)) && (buf[TCP_DST_PORT_L_P] == (port & 0xFF)))
			{
				// SYN
				if(buf[TCP_FLAGS_P] & TCP_FLAGS_SYN_V)
				{
					Syn();
				}
				if(buf[TCP_FLAGS_P] & TCP_FLAGS_ACK_V)
				{
					Ack();
				}
				if(buf[TCP_FLAGS_P] & TCP_FLAGS_PUSH_V)
				{
					Psh();
				}
				if(buf[TCP_FLAGS_P] & TCP_FLAGS_FIN_V)
				{
					MakeTcpAckFromAny(buf, 0, 0);
					InternalEvent(ST_LISTEN, &stack_data);
				}
			}
		}
	}
}
void Stack::ST_Listen(StackData* pdata)
{

}

void Stack::ST_SynReceived(StackData* pdata)
{
	MakeTcpSynAckFromSyn(buf);
}

void Stack::ST_Established(StackData* pdata)
{

}

void Stack::ST_Request(StackData* pdata)
{
	uint16_t len = GetTcpDataLen(buf);
	MakeTcpAckFromAny(buf, len, 0);
	// Requeast from user
	mb.Process(&buf[TCP_OPTIONS_P]);		// no options -> beginning of modbusTCP frame
	buf[TCP_FLAGS_P] =  TCP_FLAGS_ACK_V | TCP_FLAGS_PUSH_V; //| TCP_FLAGS_FIN_V;
	MakeTcpAckWithDataNoFlags(buf, stack_data.len);
}

void Stack::Syn(StackData* pdata)
{
    BEGIN_TRANSITION_MAP							// current state
        TRANSITION_MAP_ENTRY(ST_SYN_RECV)			// ST_LISTEN
    END_TRANSITION_MAP(pdata)
}

void Stack::Ack(StackData* pdata)
{
    BEGIN_TRANSITION_MAP							// current state
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_LISTEN
        TRANSITION_MAP_ENTRY(ST_ESTABLISHED)		// ST_SYN_RECV
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_ESTABLISHED
        TRANSITION_MAP_ENTRY(ST_ESTABLISHED)		// ST_REQUEST
    END_TRANSITION_MAP(pdata)
}

void Stack::Psh(StackData* pdata)
{
    BEGIN_TRANSITION_MAP							// current state
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_LISTEN
        TRANSITION_MAP_ENTRY(ST_NOT_ALLOWED)		// ST_SYN_RECV
        TRANSITION_MAP_ENTRY(ST_REQUEST)			// ST_ESTABLISHED
    END_TRANSITION_MAP(pdata)
}

// ------------------------------------------------------------------
uint8_t Stack::EthTypeIsArpMyIP(uint8_t* buf, uint16_t len)
{
	uint8_t i = 0;
	if(len < 41)
	{
		return 0;
	}
	if(buf[ETH_TYPE_H_P] != ETHTYPE_ARP_H_V || buf[ETH_TYPE_L_P] != ETHTYPE_ARP_L_V)
	{
		return 0;
	}
	if (buf[ETH_ARP_OPCODE_L_P] != ETH_ARP_OPCODE_REQ_L_V)
	{
		return 0;
	}
 	while(i < 4)
	{
		if(buf[ETH_ARP_DST_IP_P + i] != enc28j60.ip_addr[i])
		{
			return 0;
		}
		i++;
	}
	return 1;
}

void Stack::MakeEthHeader(uint8_t* buf)
{
	uint8_t i = 0;
	while(i < 6)
	{
		buf[ETH_DST_MAC + i] = buf[ETH_SRC_MAC + i];
		buf[ETH_SRC_MAC + i] = enc28j60.mac_addr[i];
		i++;
	}
}

void Stack::MakeArpReply(uint8_t* buf)
{
	uint8_t i = 0;
	MakeEthHeader(buf);
	buf[ETH_ARP_OPCODE_H_P] = ETH_ARP_OPCODE_REPLY_H_V;
	buf[ETH_ARP_OPCODE_L_P] = ETH_ARP_OPCODE_REPLY_L_V;
	while(i < 6)
	{
		buf[ETH_ARP_DST_MAC_P + i] = buf[ETH_ARP_SRC_MAC_P + i];
		buf[ETH_ARP_SRC_MAC_P + i] = enc28j60.mac_addr[i];
		i++;
	}
	i = 0;
	while(i < 4)
	{
		buf[ETH_ARP_DST_IP_P + i] = buf[ETH_ARP_SRC_IP_P + i];
		buf[ETH_ARP_SRC_IP_P + i] = enc28j60.ip_addr[i];
		i++;
	}
	enc28j60.SendPacket(buf, 42);
}

void Stack::MakeIpHeader(uint8_t* buf)
{
	uint8_t i = 0;
	while(i < 4)
	{
		buf[IP_DST_P + i] = buf[IP_SRC_P + i];
		buf[IP_SRC_P + i] = enc28j60.ip_addr[i];
		i++;
	}
	FillIpHeaderChecksum(buf);
}


uint8_t Stack::EthTypeIsIPMyIP(uint8_t* buf, uint16_t len)
{
	uint8_t i = 0;
	if(len < 42)
	{
		return 0;
	}
	if(buf[ETH_TYPE_H_P] != ETHTYPE_IP_H_V || buf[ETH_TYPE_L_P] != ETHTYPE_IP_L_V)
	{
		return 0;
	}
	if(buf[IP_HEADER_LEN_VER_P] != 0x45)
	{
		return 0;
	}
	while(i < 4)
	{
		if(buf[IP_DST_P + i] != enc28j60.ip_addr[i])
		{
			return 0;
		}
		i++;
	}
	return 1;
}
uint8_t Stack::EthTypeIsIcmp(uint8_t* buf)
{
	if(buf[IP_PROTO_P] == IP_PROTO_ICMP_V && buf[ICMP_TYPE_P] == ICMP_TYPE_ECHOREQUEST_V)
		return 1;
	else
		return 0;
}

uint16_t Stack::Checksum(uint8_t *buf, uint16_t len, uint8_t type)
{
// type 0 = ip, icmp
//      1 = udp
//      2 = tcp
	uint32_t sum = 0;
	if(type == 1)
	{
		sum += IP_PROTO_UDP_V;
		sum += len - 8;
	}
	if(type == 2)
	{
		sum += IP_PROTO_TCP_V;
		sum += len - 8;
	}
	while(len > 1)
	{
		sum += 0xFFFF & (((uint32_t)*buf << 8) | *(buf + 1));
		buf += 2;
		len -= 2;
	}
	if(len)
	{
		sum += ((uint32_t)(0xFF & *buf)) << 8;
	}
	while (sum >> 16)
	{
		sum = (sum & 0xFFFF) + (sum >> 16);
	}
	return ((uint16_t) sum ^ 0xFFFF);
}

void Stack::FillIpHeaderChecksum(uint8_t* buf)
{
	uint16_t ck;
	buf[IP_CHECKSUM_P] = 0;
	buf[IP_CHECKSUM_P + 1] = 0;
	buf[IP_FLAGS_P] = 0x40; 			// don't fragment
	buf[IP_FLAGS_P + 1] = 0;
	buf[IP_TTL_P] = 64; 				// TTL
	ck = Checksum(&buf[IP_P], IP_HEADER_LEN, 0);
	buf[IP_CHECKSUM_P] = ck >> 8;
	buf[IP_CHECKSUM_P + 1] = ck & 0xFF;
}

void Stack::MakeIcmpReply(uint8_t* buf, uint16_t len)
{
	MakeEthHeader(buf);
	MakeIpHeader(buf);
	buf[ICMP_TYPE_P] = ICMP_TYPE_ECHOREPLY_V;
	if(buf[ICMP_CHECKSUM_P] > (0xff - 0x08))
	{
		buf[ICMP_CHECKSUM_P + 1]++;
	}
	buf[ICMP_CHECKSUM_P] += 0x08;
	enc28j60.SendPacket(buf, len);
}

// ------------------------ TCP --------------------------

void Stack::StepSequence(uint8_t* buf, uint16_t rel_ack_num, uint8_t cp_seq)
{
	uint8_t i = 4;
	uint8_t tseq;
	while(i > 0)
	{
		rel_ack_num = buf[TCP_SEQ_H_P + i - 1] + rel_ack_num;
		tseq = buf[TCP_SEQACK_H_P + i - 1];
		buf[TCP_SEQACK_H_P + i - 1] = 0xFF & rel_ack_num;
		if (cp_seq)
		{
			buf[TCP_SEQ_H_P + i - 1] = tseq;
		}
		else
		{
			buf[TCP_SEQ_H_P + i - 1] = 0;
		}
		rel_ack_num = rel_ack_num >> 8;
		i--;
	}
}

void Stack::MakeTcpHeader(uint8_t* buf, uint16_t rel_ack_num, uint8_t cp_seq)
{
	uint8_t i = buf[TCP_DST_PORT_H_P];
	buf[TCP_DST_PORT_H_P] = buf[TCP_SRC_PORT_H_P];
	buf[TCP_SRC_PORT_H_P] = i;
	i = buf[TCP_DST_PORT_L_P];
	buf[TCP_DST_PORT_L_P] = buf[TCP_SRC_PORT_L_P];
	buf[TCP_SRC_PORT_L_P] = i;

	StepSequence(buf, rel_ack_num, cp_seq);
	buf[TCP_CHECKSUM_H_P] = 0;
	buf[TCP_CHECKSUM_L_P] = 0;
	buf[TCP_HEADER_LEN_P] = 0x50;
}

void Stack::MakeTcpSynAckFromSyn(uint8_t* buf)
{
	uint16_t ck;
	MakeEthHeader(buf);
	buf[IP_TOTLEN_H_P] = 0;
	buf[IP_TOTLEN_L_P] = IP_HEADER_LEN + TCP_HEADER_LEN_PLAIN + 4;
	MakeIpHeader(buf);
	buf[TCP_FLAGS_P] = TCP_FLAGS_SYNACK_V;
	MakeTcpHeader(buf, 1, 0);
	buf[TCP_SEQ_H_P + 0] = 0;
	buf[TCP_SEQ_H_P + 1] = 0;
	buf[TCP_SEQ_H_P + 2] = seqnum;
	buf[TCP_SEQ_H_P + 3] = 0;
	seqnum += 3;
	buf[TCP_OPTIONS_P] = 2;
	buf[TCP_OPTIONS_P + 1] = 4;
	buf[TCP_OPTIONS_P + 2] = 0x05;
	buf[TCP_OPTIONS_P + 3] = 0x0;
	buf[TCP_HEADER_LEN_P] = 0x60;
	buf[TCP_WIN_SIZE] = 0x0a;
	buf[TCP_WIN_SIZE + 1] = 0;
	ck = Checksum(&buf[IP_SRC_P], 8 + TCP_HEADER_LEN_PLAIN + 4, 2);
	buf[TCP_CHECKSUM_H_P] = ck >> 8;
	buf[TCP_CHECKSUM_L_P] = ck & 0xFF;
	// add 4 for option mss:
	enc28j60.SendPacket(buf, IP_HEADER_LEN + TCP_HEADER_LEN_PLAIN + 4 + ETH_HEADER_LEN);
}

uint16_t Stack::FillTcpData(uint8_t* buf, uint16_t pos, uint8_t* pdata, uint8_t len)
{
	// fill in tcp data at position pos
	// with no options the data starts after the checksum + 2 more bytes (urgent pointer)
	uint8_t *w = pdata;
	while (len)
	{
		buf[TCP_CHECKSUM_L_P + 3 + pos] = *w++;
		len--;
		pos++;
	}
	return pos;
}

uint16_t Stack::GetTcpDataLen(uint8_t* buf)
{

	int16_t i = (((int16_t)buf[IP_TOTLEN_H_P]) << 8) | (buf[IP_TOTLEN_L_P] & 0xFF);
	i -= IP_HEADER_LEN;
	i -=(buf[TCP_HEADER_LEN_P] >> 4) * 4; // generate len in bytes;
	if (i <= 0)
	{
		i = 0;
	}
	return((uint16_t)i);
}

void Stack::MakeTcpAckFromAny(uint8_t* buf, int16_t datlentoack, uint8_t addflags)
{
	uint16_t j;
	MakeEthHeader(buf);
	buf[TCP_FLAGS_P] = TCP_FLAGS_ACK_V | addflags;
	if (addflags == TCP_FLAGS_RST_V)
	{
		MakeTcpHeader(buf, datlentoack, 1);
	}
	else
	{
		if (datlentoack == 0)
		{
			// if there is no data then we must still ack one packet
			datlentoack = 1;
		}
		// normal case, ack the data:
		MakeTcpHeader(buf, datlentoack, 1); // no options
	}
	j = IP_HEADER_LEN + TCP_HEADER_LEN_PLAIN;
	buf[IP_TOTLEN_H_P] = j >> 8;
	buf[IP_TOTLEN_L_P] = j & 0xFF;
	MakeIpHeader(buf);
	buf[TCP_WIN_SIZE] = 0x4; // 1024=0x400, 1280=0x500 2048=0x800 768=0x300
	buf[TCP_WIN_SIZE + 1] = 0;
	j = Checksum(&buf[IP_SRC_P], 8 + TCP_HEADER_LEN_PLAIN, 2);
	buf[TCP_CHECKSUM_H_P] = j >> 8;
	buf[TCP_CHECKSUM_L_P] = j & 0xFF;
	enc28j60.SendPacket(buf, IP_HEADER_LEN + TCP_HEADER_LEN_PLAIN + ETH_HEADER_LEN);
}

void Stack::MakeTcpAckWithDataNoFlags(uint8_t* buf, uint16_t dlen)
{
	uint16_t j;
	j = IP_HEADER_LEN + TCP_HEADER_LEN_PLAIN + dlen;
	buf[IP_TOTLEN_H_P] = j >> 8;
	buf[IP_TOTLEN_L_P]= j & 0xFF;
	FillIpHeaderChecksum(buf);
	buf[TCP_CHECKSUM_H_P] = 0;
	buf[TCP_CHECKSUM_L_P] = 0;
	j = Checksum(&buf[IP_SRC_P], 8 + TCP_HEADER_LEN_PLAIN + dlen, 2);
	buf[TCP_CHECKSUM_H_P] = j >> 8;
	buf[TCP_CHECKSUM_L_P] = j & 0xFF;
	enc28j60.SendPacket(buf, IP_HEADER_LEN + TCP_HEADER_LEN_PLAIN + dlen + ETH_HEADER_LEN);
}
