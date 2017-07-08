/*
 * enc28j60.h
 *
 * Created on: 25 maj 2017
 * Author: tomek
 */

#ifndef ENC28J60_H_
#define ENC28J60_H_

#include <inttypes.h>

class Enc28j60
{
public:
	Enc28j60();
	void Init();
	uint16_t ReceivePacket(uint8_t* buf, uint16_t buf_size);
	void SendPacket(uint8_t* buf, uint16_t len);
private:
	void SPI_Init();
	void SPI_Send(uint8_t byte);
	uint8_t SPI_Recv();
	void Reset();
	void RegClearBits(uint8_t reg, uint8_t bits);
	void RegSetBits(uint8_t reg, uint8_t bits);
	uint8_t RegRead(uint8_t reg);
	void RegWrite(uint8_t reg, uint8_t val);
	void PhyWrite(uint8_t reg, uint16_t val);
	void BufRead(uint8_t* buf, uint16_t len);
	void BufWrite(uint8_t* buf, uint16_t len);
	uint8_t current_bank;
	uint16_t next_packet;
};

#define FULL_DUPLEX

#define LO8(x) 						((uint8_t) ((x) & 0xFF))
#define HI8(x) 						((uint8_t) (((x) >> 8) & 0xFF))

// ENC28J60 Packet Control Byte Bit Definitions
#define PKTCTRL_PHUGEEN 			0x08
#define PKTCTRL_PPADEN 				0x04
#define PKTCTRL_PCRCEN 				0x02
#define PKTCTRL_POVERRIDE 			0x01

// (DT p. 26)
#define ENC28J60_RCR 				0x00
#define ENC28J60_RBM 				0x3A
#define ENC28J60_WCR 				0x40
#define ENC28J60_WBM 				0x7A
#define ENC28J60_BFS 				0x80
#define ENC28J60_BFC 				0xA0
#define ENC28J60_SRC 				0xFF

// Reg address: 					 bits 0-4
// No of bank:	   				     bits 5-6
// type marker (ETH or MAC and MII): bit 7

#define ADDR_MASK 					0x1F		// 0b00011111
#define BANK_MASK 					0x60		// 0b01100000
// (DT p. 27)
#define MAC_MII_MASK 				0x80		// 0b10000000

// -------------------------------------------------

// registers bank 0
#define ENC_REG_ERDPTL 				(0x00 | 0x00)
#define ENC_REG_ERDPTH 				(0x00 | 0x01)
#define ENC_REG_EWRPTL 				(0x00 | 0x02)
#define ENC_REG_EWRPTH 				(0x00 | 0x03)
#define ENC_REG_ETXSTL 				(0x00 | 0x04)
#define ENC_REG_ETXSTH 				(0x00 | 0x05)
#define ENC_REG_ETXNDL 				(0x00 | 0x06)
#define ENC_REG_ETXNDH 				(0x00 | 0x07)
#define ENC_REG_ERXSTL 				(0x00 | 0x08)
#define ENC_REG_ERXSTH 				(0x00 | 0x09)
#define ENC_REG_ERXNDL 				(0x00 | 0x0A)
#define ENC_REG_ERXNDH 				(0x00 | 0x0B)
#define ENC_REG_ERXRDPTL 			(0x00 | 0x0C)
#define ENC_REG_ERXRDPTH 			(0x00 | 0x0D)
#define ENC_REG_ERXWRPTL 			(0x00 | 0x0E)
#define ENC_REG_ERXWRPTH 			(0x00 | 0x0F)
#define ENC_REG_EDMASTL 			(0x00 | 0x10)
#define ENC_REG_EDMASTH 			(0x00 | 0x11)
#define ENC_REG_EDMANDL 			(0x00 | 0x12)
#define ENC_REG_EDMANDH 			(0x00 | 0x13)
#define ENC_REG_EDMADSTL 			(0x00 | 0x14)
#define ENC_REG_EDMADSTH 			(0x00 | 0x15)
#define ENC_REG_EDMACSL 			(0x00 | 0x16)
#define ENC_REG_EDMACSH 			(0x00 | 0x17)

// registers bank 1
#define ENC_REG_EHT0 				(0x20 | 0x00)
#define ENC_REG_EHT1 				(0x20 | 0x01)
#define ENC_REG_EHT2 				(0x20 | 0x02)
#define ENC_REG_EHT3 				(0x20 | 0x03)
#define ENC_REG_EHT4 				(0x20 | 0x04)
#define ENC_REG_EHT5 				(0x20 | 0x05)
#define ENC_REG_EHT6 				(0x20 | 0x06)
#define ENC_REG_EHT7 				(0x20 | 0x07)
#define ENC_REG_EPMM0 				(0x20 | 0x08)
#define ENC_REG_EPMM1 				(0x20 | 0x09)
#define ENC_REG_EPMM2 				(0x20 | 0x0A)
#define ENC_REG_EPMM3 				(0x20 | 0x0B)
#define ENC_REG_EPMM4 				(0x20 | 0x0C)
#define ENC_REG_EPMM5 				(0x20 | 0x0D)
#define ENC_REG_EPMM6 				(0x20 | 0x0E)
#define ENC_REG_EPMM7 				(0x20 | 0x0F)
#define ENC_REG_EPMCSL 				(0x20 | 0x10)
#define ENC_REG_EPMCSH 				(0x20 | 0x11)
#define ENC_REG_EPMOL 				(0x20 | 0x14)
#define ENC_REG_EPMOH 				(0x20 | 0x15)
#define ENC_REG_EWOLIE 				(0x20 | 0x16)
	 #define ENC_BIT_UCWOLIE 		7
	 #define ENC_BIT_AWOLIE  		6
	 #define ENC_BIT_PMWOLIE 		4
	 #define ENC_BIT_MPWOLIE 		3
	 #define ENC_BIT_HTWOLIE 		2
	 #define ENC_BIT_MCWOLIE 		1
	 #define ENC_BIT_BCWOLIE 		0
#define ENC_REG_EWOLIR 				(0x20 | 0x17)
	 #define ENC_BIT_UCWOLIF 		7
	 #define ENC_BIT_AWOLIF 		6
	 #define ENC_BIT_PMWOLIF 		4
	 #define ENC_BIT_MPWOLIF 		3
	 #define ENC_BIT_HTWOLIF 		2
	 #define ENC_BIT_MCWOLIF 		1
	 #define ENC_BIT_BCWOLIF 		0
#define ENC_REG_ERXFCON 			(0x20 | 0x18)
	 #define ENC_BIT_UCEN 			7
	 #define ENC_BIT_ANDOR 			6
	 #define ENC_BIT_CRCEN 			5
	 #define ENC_BIT_PMEN 			4
	 #define ENC_BIT_MPEN 			3
	 #define ENC_BIT_HTEN 			2
	 #define ENC_BIT_MCEN 			1
	 #define ENC_BIT_BCEN 			0
#define ENC_REG_EPKTCNT 			(0x20 | 0x19)

// registers bank 2
#define ENC_REG_MACON1   			(0x80 | 0x40 | 0x00)
	 #define ENC_BIT_LOOPBK  		4
 	 #define ENC_BIT_TXPAUS  		3
 	 #define ENC_BIT_RXPAUS  		2
 	 #define ENC_BIT_PASSALL 		1
 	 #define ENC_BIT_MARXEN  		0
#define ENC_REG_MACON2   			(0x80 | 0x40 | 0x01)
 	 #define ENC_BIT_MARST   		7
 	 #define ENC_BIT_RNDRST  		6
 	 #define ENC_BIT_MARXRST 		3
 	 #define ENC_BIT_RFUNRST 		2
 	 #define ENC_BIT_MATXRST 		1
 	 #define ENC_BIT_TFUNRST 		0
#define ENC_REG_MACON3   			(0x80 | 0x40 | 0x02)
 	 #define ENC_BIT_PADCFG2 		7
 	 #define ENC_BIT_PADCFG1 		6
 	 #define ENC_BIT_PADCFG0 		5
 	 #define ENC_BIT_TXCRCEN 		4
 	 #define ENC_BIT_PHDRLEN 		3
 	 #define ENC_BIT_HFRMEN  		2
 	 #define ENC_BIT_FRMLNEN 		1
 	 #define ENC_BIT_FULDPX  		0
#define ENC_REG_MACON4   			(0x80 | 0x40 | 0x03)
 	 #define ENC_BIT_DEFER   		6
 	 #define ENC_BIT_BPEN    		5
 	 #define ENC_BIT_NOBKOFF 		4
 	 #define ENC_BIT_LONGPRE 		1
 	 #define ENC_BIT_PUREPRE 		0
#define ENC_REG_MABBIPG  			(0x80 | 0x40 | 0x04)
#define ENC_REG_MAIPGL   			(0x80 | 0x40 | 0x06)
#define ENC_REG_MAIPGH   			(0x80 | 0x40 | 0x07)
#define ENC_REG_MACLCON1 			(0x80 | 0x40 | 0x08)
#define ENC_REG_MACLCON2 			(0x80 | 0x40 | 0x09)
#define ENC_REG_MAMXFLL 			(0x80 | 0x40 | 0x0A)
#define ENC_REG_MAMXFLH  			(0x80 | 0x40 | 0x0B)
#define ENC_REG_MAPHSUP  			(0x80 | 0x40 | 0x0D)
 	 #define ENC_BIT_RSTINTFC 		7
 	 #define ENC_BIT_RSTRMII  		3
#define ENC_REG_MICON    			(0x80 | 0x40 | 0x11)
 	 #define ENC_BIT_RSTMII  		7
#define ENC_REG_MICMD    			(0x80 | 0x40 | 0x12)
 	 #define ENC_BIT_MIISCAN 		1
 	 #define ENC_BIT_MIIRD   		0
#define ENC_REG_MIREGADR 			(0x80 | 0x40 | 0x14)
#define ENC_REG_MIWRL    			(0x80 | 0x40 | 0x16)
#define ENC_REG_MIWRH    			(0x80 | 0x40 | 0x17)
#define ENC_REG_MIRDL    			(0x80 | 0x40 | 0x18)
#define ENC_REG_MIRDH    			(0x80 | 0x40 | 0x19)

// registers bank 3
#define ENC_REG_MAADR1   			(0x80 | 0x60 | 0x00)
#define ENC_REG_MAADR0   			(0x80 | 0x60 | 0x01)
#define ENC_REG_MAADR3   			(0x80 | 0x60 | 0x02)
#define ENC_REG_MAADR2   			(0x80 | 0x60 | 0x03)
#define ENC_REG_MAADR5   			(0x80 | 0x60 | 0x04)
#define ENC_REG_MAADR4   			(0x80 | 0x60 | 0x05)
#define ENC_REG_EBSTSD   			(0x60 | 0x06)
#define ENC_REG_EBSTCON  			(0x60 | 0x07)
 	 #define ENC_BIT_PSV2    		7
 	 #define ENC_BIT_PSV1    		6
 	 #define ENC_BIT_PSV0    		5
 	 #define ENC_BIT_PSEL    		4
 	 #define ENC_BIT_TMSEL1  		3
 	 #define ENC_BIT_TMSEL0  		2
 	 #define ENC_BIT_TME     		1
 	 #define ENC_BIT_BISTST  		0
#define ENC_REG_EBSTCSL  			(0x60 | 0x08)
#define ENC_REG_EBSTCSH  			(0x60 | 0x09)
#define ENC_REG_MISTAT   			(0x80 | 0x60 | 0x0A)
 	 #define ENC_BIT_NVALID  		2
 	 #define ENC_BIT_SCAN    		1
 	 #define ENC_BIT_BUSY    		0
#define ENC_REG_EREVID   			(0x60 | 0x12)
#define ENC_REG_ECOCON   			(0x60 | 0x15)
 	 #define ENC_BIT_COCON2  		2
 	 #define ENC_BIT_COCON1  		1
 	 #define ENC_BIT_COCON0  		0
#define ENC_REG_EFLOCON  			(0x60 | 0x17)
 	 #define ENC_BIT_FULDPXS 		2
	 #define ENC_BIT_FCEN1   		1
 	 #define ENC_BIT_FCEN0  		0
#define ENC_REG_EPAUSL   			(0x60 | 0x18)
#define ENC_REG_EPAUSH   			(0x60 | 0x19)

// registers common in all banks
#define ENC_REG_EIE      			(0x00 | 0x1B)
 	 #define ENC_BIT_INTIE  		7
 	 #define ENC_BIT_PKTIE   		6
 	 #define ENC_BIT_DMAIE   		5
 	 #define ENC_BIT_LINKIE  		4
 	 #define ENC_BIT_TXIE    		3
 	 #define ENC_BIT_WOLIE   		2
 	 #define ENC_BIT_TXERIE  		1
 	 #define ENC_BIT_RXERIE  		0
#define ENC_REG_EIR      			(0x00 | 0x1C)
 	 #define ENC_BIT_PKTIF   		6
 	 #define ENC_BIT_DMAIF   		5
 	 #define ENC_BIT_LINKIF  		4
 	 #define ENC_BIT_TXIF    		3
 	 #define ENC_BIT_WOLIF   		2
 	 #define ENC_BIT_TXERIF  		1
 	 #define ENC_BIT_RXERIF  		0
#define ENC_REG_ESTAT    			(0x00 | 0x1D)
 	 #define ENC_BIT_INT     		7
 	 #define ENC_BIT_LATECOL 		4
 	 #define ENC_BIT_RXBUSY  		2
 	 #define ENC_BIT_TXABRT  		1
 	 #define ENC_BIT_CLKRDY  		0
#define ENC_REG_ECON2    			(0x00 | 0x1E)
 	 #define ENC_BIT_AUTOINC 		7
 	 #define ENC_BIT_PKTDEC  		6
 	 #define ENC_BIT_PWRSV   		5
 	 #define ENC_BIT_VRPS    		3
#define ENC_REG_ECON1    			(0x00 | 0x1F)
 	 #define ENC_BIT_TXRST   		7
 	 #define ENC_BIT_RXRST   		6
 	 #define ENC_BIT_DMAST   		5
 	 #define ENC_BIT_CSUMEN  		4
	 #define ENC_BIT_TXRTS   		3
 	 #define ENC_BIT_RXEN    		2
 	 #define ENC_BIT_BSEL1   		1
 	 #define ENC_BIT_BSEL0   		0

// phy registers
#define ENC_REG_PHCON1   			0x00
 	 #define ENC_BIT_PRST    		15
 	 #define ENC_BIT_PLOOPBK 		14
 	 #define ENC_BIT_PPWRSV  		11
 	 #define ENC_BIT_PDPXMD 		8
#define ENC_REG_PHSTAT1  			0x01
 	 #define ENC_BIT_PFDPX   		12
 	 #define ENC_BIT_PHDPX   		11
 	 #define ENC_BIT_LLSTAT  		2
 	 #define ENC_BIT_JBSTAT  		1
#define ENC_REG_PHID1    			0x02
#define ENC_REG_PHID2    			0x03
#define ENC_REG_PHCON2   			0x10
 	 #define ENC_BIT_FRCLNK  		14
 	 #define ENC_BIT_TXDIS   		13
 	 #define ENC_BIT_JABBER  		10
 	 #define ENC_BIT_HDLDIS  		8
#define ENC_REG_PHSTAT2  			0x11
 	 #define ENC_BIT_TXSTAT  		13
	 #define ENC_BIT_RXSTAT  		12
 	 #define ENC_BIT_COLSTAT 		11
 	 #define ENC_BIT_LSTAT   		10
	 #define ENC_BIT_DPXSTAT 		9
 	 #define ENC_BIT_PLRITY  		4
#define ENC_REG_PHIE     			0x12
 	 #define ENC_BIT_PLNKIE  		4
 	 #define ENC_BIT_PGEIE   		1
#define ENC_REG_PHIR     			0x13
 	 #define ENC_BIT_PLNKIF  		4
 	 #define ENC_BIT_PGIF    		2
#define ENC_REG_PHLCON   			0x14
 	 #define ENC_BIT_LACFG0  		8
 	 #define ENC_BIT_LBCFG0  		4
 	 #define ENC_BIT_LFRQ0   		2
 	 #define ENC_BIT_STRCH   		1


#define ENC_RX_BUFFER_START  		0x0000
#define ENC_RX_BUFFER_END    		0x19FF
// tx buffer 0x0600 = 1536 bytes
#define ENC_TX_BUFFER_START  		0x1A00
#define ENC_TX_BUFFER_END    		0x1FFF

#define MAX_PACKET_SIZE 			1518

extern Enc28j60 enc28j60;

#endif /* ENC28J60_H_ */
