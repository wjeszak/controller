/*
 * enc28j60.cpp
 *
 *  Created on: 25 maj 2017
 *      Author: tomek
 */
#include <avr/io.h>
#include "enc28j60.h"
// definicje pinów interfejsu SPI dla Atmega8
#if defined(__AVR_ATmega644PA__) || defined(__AVR_ATmega8__)
#define ENC28J60_CS_DDR 		DDRB
#define ENC28J60_CS_PORT		PORTB
#define ENC28J60_CS				2

#define ENC28J60_SPI_DDR 		DDRB
#define ENC28J60_SPI_PORT 		PORTB
#define ENC28J60_SPI_MISO 		4
#define ENC28J60_SPI_MOSI 		3
#define ENC28J60_SPI_SCK 		5
#endif

#define ENC28J60_CS_AKTYWNY 	ENC28J60_CS_PORT &= ~(1 << ENC28J60_CS)
#define ENC28J60_CS_PASYWNY 	ENC28J60_CS_PORT |=  (1 << ENC28J60_CS)

#define spi_czekaj() 			while(!(SPSR & (1 << SPIF)))

static unsigned char ENC28J60Bank;
static int WskNastepnyPakiet;

uint8_t enc28j60_CzytajKod(uint8_t rozkaz, uint8_t adres)
{
	ENC28J60_CS_AKTYWNY;
	// patrz s. 26
	SPDR = rozkaz | (adres & ADR_MASKA);
	spi_czekaj();
	SPDR = 0x00;
	spi_czekaj();
	// pierwszy bajt pusty -> dostêp do rejestrów MAC lub MII
	if(adres & MAC_MII_MASKA)
	{
		SPDR = 0x00;
		spi_czekaj();
	}
	ENC28J60_CS_PASYWNY;
	return SPDR;
}

void enc28j60_ZapiszKod(uint8_t rozkaz, uint8_t adres, uint8_t dane)
{
	ENC28J60_CS_AKTYWNY;
	SPDR = rozkaz | (adres & ADR_MASKA);
	spi_czekaj();
	SPDR = dane;
	spi_czekaj();
	ENC28J60_CS_PASYWNY;
}



