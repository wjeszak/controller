/*
 * stack.cpp
 *
 *  Created on: 26 maj 2017
 *      Author: tomek
 */

#include "stack.h"
#include "enc28j60.h"
#include "usart.h"
#include "eeprom.h"
//static uint8_t seqnum=0xa; // my initial tcp sequence number
Enc28j60 enc28j60;

Stack::Stack()
{
	enc28j60.Init();
	packet_len = 0;
}
void Stack::StackPoll()
{
	packet_len = enc28j60.ReceivePacket(1500, buf);
	if(packet_len != 0)
	{
		usart_data.frame = "Pakiet\n";
		usart.SendFrame(&usart_data);
		// ARP
		if(EthTypeIsArpMyIP(buf, packet_len))
		{
			MakeArpReply(buf);
		}
		// IP
		if(EthTypeIsIPMyIP(buf, packet_len))
		{
			// ICMP
			if(buf[IP_PROTO_P] == IP_PROTO_ICMP_V && buf[ICMP_TYPE_P] == ICMP_TYPE_ECHOREQUEST_V)
			{
				MakeIcmpReply(buf, packet_len);
			}
		}
	}
}

uint8_t Stack::EthTypeIsArpMyIP(uint8_t *buf, uint16_t len)
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
		if(buf[ETH_ARP_DST_IP_P + i] != cfg.ip_addr[i])
		{
			return 0;
		}
		i++;
	}
	return 1;
}

void Stack::MakeEthHeader(uint8_t *buf)
{
	uint8_t i = 0;
	//copy the destination MAC from the source and fill my mac into src
	while(i < 6)
	{
		buf[ETH_DST_MAC + i] = buf[ETH_SRC_MAC + i];
		buf[ETH_SRC_MAC + i] = cfg.mac_addr[i];
		i++;
	}
}

void Stack::MakeArpReply(uint8_t *buf)
{
	uint8_t i = 0;
	MakeEthHeader(buf);
	buf[ETH_ARP_OPCODE_H_P] = ETH_ARP_OPCODE_REPLY_H_V;
	buf[ETH_ARP_OPCODE_L_P] = ETH_ARP_OPCODE_REPLY_L_V;
	while(i < 6)
	{
		buf[ETH_ARP_DST_MAC_P + i] = buf[ETH_ARP_SRC_MAC_P + i];
		buf[ETH_ARP_SRC_MAC_P +i ] = cfg.mac_addr[i];
		i++;
	}
	i = 0;
	while(i < 4)
	{
		buf[ETH_ARP_DST_IP_P + i] = buf[ETH_ARP_SRC_IP_P + i];
		buf[ETH_ARP_SRC_IP_P + i] = cfg.ip_addr[i];
		i++;
	}
	// Eth + Arp is 42 bytes:
	enc28j60.SendPacket(42, buf);
}


void Stack::MakeIpHeader(uint8_t *buf)
{
	uint8_t i = 0;
	while(i < 4)
	{
		buf[IP_DST_P + i] = buf[IP_SRC_P + i];
		buf[IP_SRC_P + i] = cfg.ip_addr[i];
		i++;
	}
	FillIpHeaderChecksum(buf);
}


uint8_t Stack::EthTypeIsIPMyIP(uint8_t *buf, uint16_t len)
{
	uint8_t i = 0;
	//eth+ip+udp header is 42
	if(len < 42)
	{
		return 0;
	}
	if(buf[ETH_TYPE_H_P] != ETHTYPE_IP_H_V || buf[ETH_TYPE_L_P]!=ETHTYPE_IP_L_V)
	{
		return 0;
	}
	if(buf[IP_HEADER_LEN_VER_P] != 0x45)
	{
    // must be IP V4 and 20 byte header
		return 0;
	}
	while(i < 4)
	{
		if(buf[IP_DST_P + i] != cfg.ip_addr[i])
		{
			return 0;
		}
		i++;
	}
	return 1;
}

uint16_t Stack::Checksum(uint8_t *buf, uint16_t len, uint8_t type)
{
	// type 0 = ip, icmp
	//      1 = udp
	//      2 = tcp
	uint32_t sum = 0;

	if(type == 1)
	{
		sum+=IP_PROTO_UDP_V;
                // the length here is the length of udp (data+header len)
                // =length given to this function - (IP.scr+IP.dst length)
		sum+=len-8; // = real udp len
	}
	if(type==2)
	{
		sum+=IP_PROTO_TCP_V;
                // the length here is the length of tcp (data+header len)
                // =length given to this function - (IP.scr+IP.dst length)
		sum+=len-8; // = real tcp len
	}
        // build the sum of 16bit words
	while(len > 1)
	{
		sum += 0xFFFF & (((uint32_t)*buf<<8)|*(buf+1));
		buf+=2;
		len-=2;
	}
	// if there is a byte left then add it (padded with zero)
	if(len)
	{
		sum += ((uint32_t)(0xFF & *buf)) << 8;
	}
	// now calculate the sum over the bytes in the sum
	// until the result is only 16bit long
	while (sum >> 16)
	{
		sum = (sum & 0xFFFF) + (sum >> 16);
	}
	// build 1's complement:
	return ((uint16_t) sum ^ 0xFFFF);
}

void Stack::FillIpHeaderChecksum(uint8_t *buf)
{
	uint16_t ck;
	// clear the 2 byte checksum
	buf[IP_CHECKSUM_P] = 0;
	buf[IP_CHECKSUM_P + 1] = 0;
	buf[IP_FLAGS_P] = 0x40; 			// don't fragment
	buf[IP_FLAGS_P + 1] = 0;  			// fragement offset
	buf[IP_TTL_P] = 64; 				// TTL
	// calculate the checksum:
	ck = Checksum(&buf[IP_P], IP_HEADER_LEN, 0);
	buf[IP_CHECKSUM_P] = ck >> 8;
	buf[IP_CHECKSUM_P + 1] = ck & 0xFF;
}

void Stack::MakeIcmpReply(uint8_t *buf, uint16_t len)
{
	MakeEthHeader(buf);
	MakeIpHeader(buf);
	buf[ICMP_TYPE_P] = ICMP_TYPE_ECHOREPLY_V;
	// we changed only the icmp.type field from request(=8) to reply(=0).
	// we can therefore easily correct the checksum:
	if(buf[ICMP_CHECKSUM_P] > (0xff - 0x08))
	{
		buf[ICMP_CHECKSUM_P + 1]++;
	}
	buf[ICMP_CHECKSUM_P]+= 0x08;
	enc28j60.SendPacket(len,buf);
}

// ------------------------ TCP --------------------------
/*
void Stos::step_seq(uint8_t *buf,uint16_t rel_ack_num,uint8_t cp_seq)
{
        uint8_t i;
        uint8_t tseq;
        i=4;
        // sequence numbers:
        // add the rel ack num to SEQACK
        while(i>0){
                rel_ack_num=buf[TCP_SEQ_H_P+i-1]+rel_ack_num;
                tseq=buf[TCP_SEQACK_H_P+i-1];
                buf[TCP_SEQACK_H_P+i-1]=0xff&rel_ack_num;
                if (cp_seq){
                        // copy the acknum sent to us into the sequence number
                        buf[TCP_SEQ_H_P+i-1]=tseq;
                }else{
                        buf[TCP_SEQ_H_P+i-1]= 0; // some preset value
                }
                rel_ack_num=rel_ack_num>>8;
                i--;
        }
}

void Stos::make_tcphead(uint8_t *buf,uint16_t rel_ack_num,uint8_t cp_seq)
{
        uint8_t i;
        // copy ports:
        i=buf[TCP_DST_PORT_H_P];
        buf[TCP_DST_PORT_H_P]=buf[TCP_SRC_PORT_H_P];
        buf[TCP_SRC_PORT_H_P]=i;
        //
        i=buf[TCP_DST_PORT_L_P];
        buf[TCP_DST_PORT_L_P]=buf[TCP_SRC_PORT_L_P];
        buf[TCP_SRC_PORT_L_P]=i;
        step_seq(buf,rel_ack_num,cp_seq);
        // zero the checksum
        buf[TCP_CHECKSUM_H_P]=0;
        buf[TCP_CHECKSUM_L_P]=0;
        // no options:
        // 20 bytes:
        // The tcp header length is only a 4 bit field (the upper 4 bits).
        // It is calculated in units of 4 bytes.
        // E.g 20 bytes: 20/4=6 => 0x50=header len field
        buf[TCP_HEADER_LEN_P]=0x50;
}

void Stos::make_tcp_synack_from_syn(uint8_t *buf)
{
        uint16_t ck;
        make_eth(buf);
        // total length field in the IP header must be set:
        // 20 bytes IP + 24 bytes (20tcp+4tcp options)
        buf[IP_TOTLEN_H_P]=0;
        buf[IP_TOTLEN_L_P]=IP_HEADER_LEN+TCP_HEADER_LEN_PLAIN+4;
        make_ip(buf);
        buf[TCP_FLAGS_P]=TCP_FLAGS_SYNACK_V;
        make_tcphead(buf,1,0);
        // put an inital seq number
        buf[TCP_SEQ_H_P+0]= 0;
        buf[TCP_SEQ_H_P+1]= 0;
        // we step only the second byte, this allows us to send packts
        // with 255 bytes, 512  or 765 (step by 3) without generating
        // overlapping numbers.
        buf[TCP_SEQ_H_P+2]= seqnum;
        buf[TCP_SEQ_H_P+3]= 0;
        // step the inititial seq num by something we will not use
        // during this tcp session:
        seqnum+=3;
        // add an mss options field with MSS to 1280:
        // 1280 in hex is 0x500
        buf[TCP_OPTIONS_P]=2;
        buf[TCP_OPTIONS_P+1]=4;
        buf[TCP_OPTIONS_P+2]=0x05;
        buf[TCP_OPTIONS_P+3]=0x0;
        // The tcp header length is only a 4 bit field (the upper 4 bits).
        // It is calculated in units of 4 bytes.
        // E.g 24 bytes: 24/4=6 => 0x60=header len field
        buf[TCP_HEADER_LEN_P]=0x60;
        // here we must just be sure that the web browser contacting us
        // will send only one get packet
        buf[TCP_WIN_SIZE]=0x0a; // was 1400=0x578, 2560=0xa00 suggested by Andras Tucsni to be able to receive bigger packets
        buf[TCP_WIN_SIZE+1]=0; //
        // calculate the checksum, len=8 (start from ip.src) + TCP_HEADER_LEN_PLAIN + 4 (one option: mss)
        ck=checksum(&buf[IP_SRC_P], 8+TCP_HEADER_LEN_PLAIN+4,2);
        buf[TCP_CHECKSUM_H_P]=ck>>8;
        buf[TCP_CHECKSUM_L_P]=ck& 0xff;
        // add 4 for option mss:
        ethernet.WyslijPakiet(IP_HEADER_LEN+TCP_HEADER_LEN_PLAIN+4+ETH_HEADER_LEN,buf);
}
*/


