/*
 * enc28j60.cpp
 *
 * Created on: 25 maj 2017
 * Author: tomek
 */

#include <avr/io.h>
#include <util/delay.h>
#include "enc28j60.h"
#include "eeprom.h"

#define ENC28J60_CS_DDR 		DDRC
#define ENC28J60_CS_PORT		PORTC
#define ENC28J60_CS				3

#define ENC28J60_SPI_DDR 		DDRB
#define ENC28J60_SPI_PORT 		PORTB
#define ENC28J60_SPI_MISO 		6
#define ENC28J60_SPI_MOSI 		5
#define ENC28J60_SPI_SCK 		7

#define ENC28J60_CS_ACTIVE	 	ENC28J60_CS_PORT &= ~(1 << ENC28J60_CS)
#define ENC28J60_CS_PASSIVE 	ENC28J60_CS_PORT |=  (1 << ENC28J60_CS)

#define spi_wait() 				while(!(SPSR & (1 << SPIF)))

Enc28j60::Enc28j60()
{

}

void Enc28j60::SPI_Init()
{
	ENC28J60_SPI_DDR |= ((1 << ENC28J60_SPI_MOSI) | (1 << ENC28J60_SPI_SCK));
	ENC28J60_SPI_DDR &= ~(1 << ENC28J60_SPI_MISO);
	ENC28J60_SPI_PORT |= (1 << ENC28J60_SPI_SCK);

	ENC28J60_CS_DDR |= (1 << ENC28J60_CS);
	ENC28J60_CS_PASSIVE;
	SPCR |= (1 << SPE) | (1 << MSTR);
	_delay_ms(100);
}

void Enc28j60::SPI_Send(uint8_t byte)
{
	SPDR = byte;
	spi_wait();
}

uint8_t Enc28j60::SPI_Recv()
{
	uint8_t byte = SPDR;
	return byte;
}

void Enc28j60::Reset()
{
	ENC28J60_CS_ACTIVE;
	SPI_Send(ENC28J60_SRC);
	ENC28J60_CS_PASSIVE;
	// errata #2
	_delay_us(300);
}

void Enc28j60::RegClearBits(uint8_t reg, uint8_t bits)
{
	uint8_t addr = reg & ADDR_MASK;
	ENC28J60_CS_ACTIVE;
	SPI_Send(ENC28J60_BFC | addr);
	SPI_Send(bits);
	ENC28J60_CS_PASSIVE;
}

void Enc28j60::RegSetBits(uint8_t reg, uint8_t bits)
{
	uint8_t addr = reg & ADDR_MASK;
	ENC28J60_CS_ACTIVE;
	SPI_Send(ENC28J60_BFS | addr);
	SPI_Send(bits);
	ENC28J60_CS_PASSIVE;
}

uint8_t Enc28j60::RegRead(uint8_t reg)
{
	uint8_t reg_val;
	uint8_t addr = reg & ADDR_MASK;
	if(addr < 0x1A)
	{
		uint8_t bank = (reg & BANK_MASK) >> 5;
		if(bank != current_bank)
		{
			RegClearBits(ENC_REG_ECON1, (0x03 << ENC_BIT_BSEL0));

			if(bank)
			{
				RegSetBits(ENC_REG_ECON1, bank << ENC_BIT_BSEL0);
			}
			current_bank = bank;
		}
	}

	ENC28J60_CS_ACTIVE;
	SPI_Send(ENC28J60_RCR | addr);
	SPI_Send(0x00);

	if(reg & MAC_MII_MASK) SPI_Send(0x00);
	reg_val = SPI_Recv();
	ENC28J60_CS_PASSIVE;
	return reg_val;
}

void Enc28j60::RegWrite(uint8_t reg, uint8_t val)
{
	uint8_t addr = reg & ADDR_MASK;
	if(addr < 0x1A)
	{
		uint8_t bank = (reg & BANK_MASK) >> 5;
		if(bank != current_bank)
		{
			RegClearBits(ENC_REG_ECON1, (0x03 << ENC_BIT_BSEL0));
			if(bank)
			{
				RegSetBits(ENC_REG_ECON1, bank << ENC_BIT_BSEL0);
			}
			current_bank = bank;
		}
	}
	ENC28J60_CS_ACTIVE;
	SPI_Send(ENC28J60_WCR | addr);
	SPI_Send(val);
	ENC28J60_CS_PASSIVE;
}

void Enc28j60::PhyWrite(uint8_t reg, uint16_t val)
{
	RegWrite(ENC_REG_MIREGADR, reg);
	RegWrite(ENC_REG_MIWRL, LO8(val));
	RegWrite(ENC_REG_MIWRH, HI8(val));
	_delay_us(10);
	while(RegRead(ENC_REG_MISTAT) & (1 << ENC_BIT_BUSY));
}

void Enc28j60::BufRead(uint8_t* buf, uint16_t len)
{
	ENC28J60_CS_ACTIVE;
	SPI_Send(ENC28J60_RBM);
	for(; len > 0; len--, buf++)
	{
		SPI_Send(0x00);
		*buf = SPI_Recv();
	}
	ENC28J60_CS_PASSIVE;
}

void Enc28j60::BufWrite(uint8_t* buf, uint16_t len)
{
	ENC28J60_CS_ACTIVE;
	SPI_Send(ENC28J60_WBM);
	for(; len > 0; len--, buf++)
	{
		SPI_Send(*buf);
	}
	ENC28J60_CS_PASSIVE;
}

// ------------------------------------------------------------------

void Enc28j60::SendPacket(uint8_t* buf, uint16_t len)
{
	uint8_t ctrl = 0;
	uint8_t ms = 100;
	while(ms--)
	{
		if(!(RegRead(ENC_REG_ECON1) & (1 << ENC_BIT_TXRTS))) break;
		_delay_ms(1);
	}
	#ifdef FULL_DUPLEX
	//errata #12: Reset TX Logic
	if(RegRead(ENC_REG_ECON1) & (1 << ENC_BIT_TXRTS))
	{
	#else
	if(1)
	{
	#endif
		RegSetBits(ENC_REG_ECON1, (1 << ENC_BIT_TXRST));
		RegClearBits(ENC_REG_ECON1, (1 << ENC_BIT_TXRST));
	}
	RegWrite(ENC_REG_EWRPTL, LO8(ENC_TX_BUFFER_START));
	RegWrite(ENC_REG_EWRPTH, HI8(ENC_TX_BUFFER_START));

	RegWrite(ENC_REG_ETXNDL, LO8(ENC_TX_BUFFER_START + len));
	RegWrite(ENC_REG_ETXNDH, HI8(ENC_TX_BUFFER_START + len));

	BufWrite(&ctrl, 1);
	BufWrite(buf, len);
	RegClearBits(ENC_REG_EIR, (1 << ENC_BIT_TXIF));
	RegSetBits(ENC_REG_ECON1, (1 << ENC_BIT_TXRTS));
}

uint16_t Enc28j60::ReceivePacket(uint8_t* buf, uint16_t buf_size)
{
	uint8_t rx_header[6];
	uint16_t len, status;
	uint8_t u;

	u = RegRead(ENC_REG_EPKTCNT);

	if(u == 0) return 0;

	RegWrite(ENC_REG_ERDPTL, LO8(next_packet));
	RegWrite(ENC_REG_ERDPTH, HI8(next_packet));

	BufRead(rx_header, sizeof(rx_header));

	next_packet  =            rx_header[0];
	next_packet |= ((uint8_t)(rx_header[1])) << 8;
	len          =            rx_header[2];
	len         |= ((uint8_t)(rx_header[3])) << 8;
	status       = 			  rx_header[4];
	status      |= ((uint8_t)(rx_header[5])) << 8;
	// If the receive OK bit is not 1 or the zero bit is not zero, or the packet is larger then the buffer, reset the enc chip and SPI
	if ((!(status & (1 << 7))) || (status & 0x8000) || (len > buf_size))
	{
		Init();
	}
	len -= 4;

	if( len > buf_size) len = buf_size;
	BufRead(buf, len);
	if(next_packet - 1 > ENC_RX_BUFFER_END  || next_packet - 1 < ENC_RX_BUFFER_START)
	{
		RegWrite(ENC_REG_ERXRDPTL, LO8(ENC_RX_BUFFER_END));
		RegWrite(ENC_REG_ERXRDPTH, HI8(ENC_RX_BUFFER_END));
	}
	else
	{
		RegWrite(ENC_REG_ERXRDPTL, LO8(next_packet - 1));
		RegWrite(ENC_REG_ERXRDPTH, HI8(next_packet - 1));
	}
	RegSetBits(ENC_REG_ECON2, (1 << ENC_BIT_PKTDEC));
	return len;
}

void Enc28j60::Init()
{
	SPI_Init();
	Reset();
	while(!(RegRead(ENC_REG_ESTAT) & (1 << ENC_BIT_CLKRDY)));
	_delay_ms(20);

	RegWrite(ENC_REG_ETXSTL, LO8(ENC_TX_BUFFER_START));
	RegWrite(ENC_REG_ETXSTH, HI8(ENC_TX_BUFFER_START));
	RegWrite(ENC_REG_ETXNDL, LO8(ENC_TX_BUFFER_END));
	RegWrite(ENC_REG_ETXNDH, HI8(ENC_TX_BUFFER_END));

	RegWrite(ENC_REG_ERXSTL, LO8(ENC_RX_BUFFER_START));
	RegWrite(ENC_REG_ERXSTH, HI8(ENC_RX_BUFFER_START));
	RegWrite(ENC_REG_ERXNDL, LO8(ENC_RX_BUFFER_END));
	RegWrite(ENC_REG_ERXNDH, HI8(ENC_RX_BUFFER_END));

	RegWrite(ENC_REG_MACON2, 0x00);

	RegWrite(ENC_REG_MACON1, (1<<ENC_BIT_MARXEN) | (1<<ENC_BIT_RXPAUS) | (1<<ENC_BIT_TXPAUS));

	#ifdef FULL_DUPLEX
	RegWrite(ENC_REG_MACON3, (1<<ENC_BIT_PADCFG0) | (1<<ENC_BIT_TXCRCEN) | (1<<ENC_BIT_FRMLNEN) | (1<<ENC_BIT_FULDPX));
	#else
	// mac auto padding of small packets, add crc, frame length check, half duplex
	RegWrite(ENC_REG_MACON3, (1<<ENC_BIT_PADCFG0) | (1<<ENC_BIT_TXCRCEN) | (1<<ENC_BIT_FRMLNEN));
	#endif

	RegWrite(ENC_REG_MAMXFLL, LO8(1518));
	RegWrite(ENC_REG_MAMXFLH, HI8(1518));

	#ifdef FULL_DUPLEX
	// back-to-back inter packet gap delay time (0x15 for full duplex)
	RegWrite(ENC_REG_MABBIPG, 0x15);
	#else
	// back-to-back inter packet gap delay time (0x12 for half duplex)
	RegWrite(ENC_REG_MABBIPG, 0x12);
	#endif
	// non back-to-back inter packet gap delay time (should be 0x12)
	RegWrite(ENC_REG_MAIPGL, 0x12);
	#ifndef FULL_DUPLEX
	// non back-to-back inter packet gap delay time high (should be 0x0C for half duplex)
	RegWrite(ENC_REG_MAIPGH, 0x0C);
	#endif

	RegWrite(ENC_REG_MAADR5, cfg.mac_addr[0]);
	RegWrite(ENC_REG_MAADR4, cfg.mac_addr[1]);
	RegWrite(ENC_REG_MAADR3, cfg.mac_addr[2]);
	RegWrite(ENC_REG_MAADR2, cfg.mac_addr[3]);
	RegWrite(ENC_REG_MAADR1, cfg.mac_addr[4]);
	RegWrite(ENC_REG_MAADR0, cfg.mac_addr[5]);

	RegWrite(ENC_REG_ECOCON, 0x00);
	#ifdef FULL_DUPLEX
	// set the PDPXMD full duplex mode bit on the phy
	#define ENC_REG_PHCON1_VALUE (0x0000 | (1 << ENC_BIT_PDPXMD))
	PhyWrite(ENC_REG_PHCON1, HI8(ENC_REG_PHCON1_VALUE)| LO8(ENC_REG_PHCON1_VALUE));
	#endif

	#ifndef FULL_DUPLEX
	// in half duplex do not loop back transmitted data
	#define ENC_REG_PHCON2_VALUE (0x0000 | (1 << ENC_BIT_HDLDIS))
	PhyWrite(ENC_REG_PHCON2, HI8(ENC_REG_PHCON2_VALUE)| LO8(ENC_REG_PHCON2_VALUE));
	#endif

	// leds: ledA (yellow) rx and tx activity
	//       ledB (green)  link status
	#define ENC_REG_PHCON_VALUE (0x0000 | (1 << ENC_BIT_STRCH) | (7 << ENC_BIT_LACFG0) | (4 << ENC_BIT_LBCFG0))

	next_packet = ENC_RX_BUFFER_START;

	// configure the interrupt sources
	RegWrite( ENC_REG_EIE, (1 << ENC_BIT_INTIE)  | (1 << ENC_BIT_PKTIE)
						  | (0 << ENC_BIT_DMAIE)  | (0 << ENC_BIT_LINKIE)
						  | (0 << ENC_BIT_TXIE)   | (0 << ENC_BIT_WOLIE)
						  | (0 << ENC_BIT_TXERIE) | (0 << ENC_BIT_RXERIE));

	RegSetBits(ENC_REG_ECON1, (1 << ENC_BIT_RXEN));
}
