/*
 * stos.cpp
 *
 *  Created on: 26 maj 2017
 *      Author: tomek
 */
#include "enc28j60.h"
#include "stos.h"

static uint8_t Adres_MAC[6] = {ADR_MAC1, ADR_MAC2, ADR_MAC3, ADR_MAC4, ADR_MAC5, ADR_MAC6};
static uint8_t Adres_IP[4]  = {ADR_IP1, ADR_IP2, ADR_IP3, ADR_IP4};
static uint8_t seqnum=0xa; // my initial tcp sequence number

// Doprecyzowac typ pakietu przychodzacego, zeby ARP odpowiadal tylko na ARP Req
// a ping na ICMP -> patrz koniec pliku ip_arp_udp_tcp.c

uint8_t eth_type_is_arp_and_my_ip(uint8_t *buf, uint16_t len)
{
	uint8_t i=0;
        //
	if(len < 41)
	{
		return(0);
	}
	if(buf[ETH_TYPE_H_P] != ETHTYPE_ARP_H_V || buf[ETH_TYPE_L_P] != ETHTYPE_ARP_L_V)
	{
		return 0;
	}
	while(i < 4)
	{
		if(buf[ETH_ARP_DST_IP_P + i] != Adres_IP[i])
		{
			return 0;
		}
		i++;
	}
	return 1;
}

void make_eth(uint8_t *buf)
{
	uint8_t i = 0;
	//copy the destination mac from the source and fill my mac into src
	while(i < 6)
	{
		buf[ETH_DST_MAC + i] = buf[ETH_SRC_MAC + i];
		buf[ETH_SRC_MAC + i] = Adres_MAC[i];
		i++;
	}
}

void make_arp_answer_from_request(uint8_t *buf)
{
	uint8_t i = 0;
	make_eth(buf);
	buf[ETH_ARP_OPCODE_H_P] = ETH_ARP_OPCODE_REPLY_H_V;
	buf[ETH_ARP_OPCODE_L_P]=ETH_ARP_OPCODE_REPLY_L_V;
        // fill the mac addresses:
        while(i<6){
                buf[ETH_ARP_DST_MAC_P+i]=buf[ETH_ARP_SRC_MAC_P+i];
                buf[ETH_ARP_SRC_MAC_P+i]=Adres_MAC[i];
                i++;
        }
        i=0;
        while(i<4){
                buf[ETH_ARP_DST_IP_P+i]=buf[ETH_ARP_SRC_IP_P+i];
                buf[ETH_ARP_SRC_IP_P+i]=Adres_IP[i];
                i++;
        }
        // eth+arp is 42 bytes:
        enc28j60_WyslijPakiet(42,buf);
}

uint8_t eth_type_is_ip_and_my_ip(uint8_t *buf, uint16_t len)
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
		if(buf[IP_DST_P + i] != Adres_IP[i])
		{
			return 0;
		}
		i++;
	}
	return 1;
}

uint16_t checksum(uint8_t *buf, uint16_t len,uint8_t type){
        // type 0=ip , icmp
        //      1=udp
        //      2=tcp
        uint32_t sum = 0;

        //if(type==0){
        //        // do not add anything, standard IP checksum as described above
        //        // Usable for ICMP and IP header
        //}
        if(type==1){
                sum+=IP_PROTO_UDP_V; // protocol udp
                // the length here is the length of udp (data+header len)
                // =length given to this function - (IP.scr+IP.dst length)
                sum+=len-8; // = real udp len
        }
        if(type==2){
                sum+=IP_PROTO_TCP_V;
                // the length here is the length of tcp (data+header len)
                // =length given to this function - (IP.scr+IP.dst length)
                sum+=len-8; // = real tcp len
        }
        // build the sum of 16bit words
        while(len >1){
                sum += 0xFFFF & (((uint32_t)*buf<<8)|*(buf+1));
                buf+=2;
                len-=2;
        }
        // if there is a byte left then add it (padded with zero)
        if (len){
                sum += ((uint32_t)(0xFF & *buf))<<8;
        }
        // now calculate the sum over the bytes in the sum
        // until the result is only 16bit long
        while (sum>>16){
                sum = (sum & 0xFFFF)+(sum >> 16);
        }
        // build 1's complement:
        return( (uint16_t) sum ^ 0xFFFF);
}

void fill_ip_hdr_checksum(uint8_t *buf)
{
        uint16_t ck;
        // clear the 2 byte checksum
        buf[IP_CHECKSUM_P]=0;
        buf[IP_CHECKSUM_P+1]=0;
        buf[IP_FLAGS_P]=0x40; // don't fragment
        buf[IP_FLAGS_P+1]=0;  // fragement offset
        buf[IP_TTL_P]=64; // ttl
        // calculate the checksum:
        ck=checksum(&buf[IP_P], IP_HEADER_LEN,0);
        buf[IP_CHECKSUM_P]=ck>>8;
        buf[IP_CHECKSUM_P+1]=ck& 0xff;
}

void make_ip(uint8_t *buf)
{
	uint8_t i = 0;
	while(i < 4)
	{
		buf[IP_DST_P + i] = buf[IP_SRC_P + i];
		buf[IP_SRC_P + i] = Adres_IP[i];
		i++;
	}
	fill_ip_hdr_checksum(buf);
}

void make_echo_reply_from_request(uint8_t *buf, uint16_t len)
{
	make_eth(buf);
	make_ip(buf);
	buf[ICMP_TYPE_P] = ICMP_TYPE_ECHOREPLY_V;
	// we changed only the icmp.type field from request(=8) to reply(=0).
	// we can therefore easily correct the checksum:
	if (buf[ICMP_CHECKSUM_P] > (0xff-0x08))
	{
		buf[ICMP_CHECKSUM_P + 1]++;
	}
	buf[ICMP_CHECKSUM_P]+=0x08;
	enc28j60_WyslijPakiet(len,buf);
}

// ------------------------ TCP --------------------------
void step_seq(uint8_t *buf,uint16_t rel_ack_num,uint8_t cp_seq)
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

void make_tcphead(uint8_t *buf,uint16_t rel_ack_num,uint8_t cp_seq)
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

void make_tcp_synack_from_syn(uint8_t *buf)
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
        enc28j60_WyslijPakiet(IP_HEADER_LEN+TCP_HEADER_LEN_PLAIN+4+ETH_HEADER_LEN,buf);
}
void LAN_Start()
{
//	Stos_Init(adres_MAC, adres_IP, 502);
}



