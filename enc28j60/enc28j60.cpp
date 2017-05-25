/*
 * enc28j60.cpp
 *
 *  Created on: 25 maj 2017
 *      Author: tomek
 */
#include <avr/io.h>

// definicje pinów interfejsu SPI
#define ENC28J60_CS_DDR 		DDRB
#define ENC28J60_CS_PORT		PORTB
#define ENC28J60_CS				2

#define ENC28J60_CS_AKTYWNY 	ENC28J60_CS_PORT &= ~(1 << ENC28J60_CS)
#define ENC28J60_CS_PASYWNY 	ENC28J60_CS_PORT |=  (1 << ENC28J60_CS)

#define ENC28J60_SPI_DDR 		DDRB
#define ENC28J60_SPI_PORT 		PORTB
#define ENC28J60_SPI_MISO 		4
#define ENC28J60_SPI_MOSI 		3
#define ENC28J60_SPI_SCK 		5

#define spi_czekaj() 			while(!(SPSR & (1 << SPIF)))

static unsigned char ENC28J60Bank;
static int WskNastepnyPakiet;

unsigned char ENC28J60_CzytajOp(unsigned char op, unsigned char adres)
{
	ENC28J60_CS_AKTYWNY;


}



