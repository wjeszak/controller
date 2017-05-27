/*
 * enc28j60.cpp
 *
 *  Created on: 25 maj 2017
 *      Author: tomek
 */
#include <avr/io.h>
#include <util/delay.h>
#include "enc28j60.h"

// definicje pinów interfejsu SPI dla Atmega644PA
//#if defined(__AVR_ATmega644PA__) || defined(__AVR_ATmega8__)
#define ENC28J60_CS_DDR 		DDRC
#define ENC28J60_CS_PORT		PORTC
#define ENC28J60_CS				3

#define ENC28J60_SPI_DDR 		DDRB
#define ENC28J60_SPI_PORT 		PORTB
#define ENC28J60_SPI_MISO 		6
#define ENC28J60_SPI_MOSI 		5
#define ENC28J60_SPI_SCK 		7
//#endif

#define ENC28J60_CS_AKTYWNY 	ENC28J60_CS_PORT &= ~(1 << ENC28J60_CS)
#define ENC28J60_CS_PASYWNY 	ENC28J60_CS_PORT |=  (1 << ENC28J60_CS)

#define spi_czekaj() 			while(!(SPSR & (1 << SPIF)))

static unsigned char enc28j60_Bank;
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
	// patrz s. 26
	SPDR = rozkaz | (adres & ADR_MASKA);
	spi_czekaj();
	SPDR = dane;
	spi_czekaj();
	ENC28J60_CS_PASYWNY;
}

void enc28j60_CzytajBufor(uint16_t dl, uint8_t* dane)
{
	ENC28J60_CS_AKTYWNY;
	// patrz s. 26, kod rozkazu i argument const
	SPDR = ENC28J60_CZYTAJ_BUFOR;
	spi_czekaj();
	while(dl)
	{
		dl--;
		SPDR = 0x00;
		spi_czekaj();
		*dane = SPDR;
		dane++;
	}
	*dane = '\0';
	ENC28J60_CS_PASYWNY;
}

void enc28j60_ZapiszBufor(uint16_t dl, uint8_t* dane)
{
	ENC28J60_CS_AKTYWNY;
	// patrz s. 26, kod rozkazu i argument const
	SPDR = ENC28J60_ZAPISZ_BUFOR;
	spi_czekaj();
	while(dl)
	{
		dl--;
		SPDR = *dane;
		spi_czekaj();
		dane++;
	}
	ENC28J60_CS_AKTYWNY;
}

void enc28j60_UstawBank(uint8_t adres)
{
	// porównanie bitów 5 i 6 w adresie
	if((adres & BANK_MASKA) != enc28j60_Bank)
	{
		enc28j60_ZapiszKod(ENC28J60_KASUJ_BITY, ECON1, (ECON1_BSEL1 | ECON1_BSEL0));
		enc28j60_ZapiszKod(ENC28J60_USTAW_BITY, ECON1, (adres & BANK_MASKA) >> 5);
		enc28j60_Bank = (adres & BANK_MASKA);
	}
}

uint8_t enc28j60_Czytaj(uint8_t adres)
{
	enc28j60_UstawBank(adres);
	return enc28j60_CzytajKod(ENC28J60_CZYTAJ_REJ_KONTR, adres);
}

void enc28j60_Zapisz(uint8_t adres, uint8_t dane)
{
	enc28j60_UstawBank(adres);
	enc28j60_ZapiszKod(ENC28J60_ZAPISZ_REJ_KONTR, adres, dane);
}

uint16_t enc28j60_CzytajPhy(uint8_t adres)
{
	// patrz s. 19
	// dlaczego w tej funkcji nie ma czekania 10 us ??
	enc28j60_Zapisz(MIREGADR, adres);
	enc28j60_Zapisz(MICMD, MICMD_MIIRD);
	while(enc28j60_Czytaj(MISTAT) &  MISTAT_BUSY);
	enc28j60_Zapisz(MICMD, 0x00);
	return (enc28j60_Czytaj(MIRDH << 8) | enc28j60_Czytaj(MIRDL));
}

void enc28j60_ZapiszPhy(uint8_t adres, uint16_t dane)
{
	enc28j60_Zapisz(MIREGADR, adres);
	enc28j60_Zapisz(MIWRL, dane);
	enc28j60_Zapisz(MIWRH, (dane >> 8));
	while(enc28j60_Czytaj(MISTAT) &  MISTAT_BUSY)
	{
		_delay_us(10);
	}
}

void enc28j60_Init(uint8_t* macadr)
{
	ENC28J60_CS_DDR |= (1 << ENC28J60_CS);
	ENC28J60_CS_PASYWNY;
	ENC28J60_SPI_DDR |= ((1 << ENC28J60_SPI_MOSI) | (1 << ENC28J60_SPI_SCK));
	ENC28J60_SPI_DDR &= ~(1 << ENC28J60_SPI_MISO);

	ENC28J60_SPI_PORT &= ~((1 << ENC28J60_SPI_MOSI) | (1 << ENC28J60_SPI_SCK));
	// SPI
	SPCR |= (1 << SPE) | (1 << MSTR);
	SPSR |= (1<<SPI2X);
	// Miekki reset
	enc28j60_ZapiszKod(ENC28J60_RESET, 0, ENC28J60_RESET);	// jako 3 parametr mo¿e byc cokolwiek
	_delay_ms(20);

	// Inicjalizacja buforow odbiorczych/nadawczych
	WskNastepnyPakiet = RXSTART_INIT;
	// Ustawienie adresu poczatku bufora odbiorczego
	enc28j60_Zapisz(ERXSTL, RXSTART_INIT & 0xFF);
	enc28j60_Zapisz(ERXSTH, RXSTART_INIT >> 8);

	// Ustawienie wskaznika w buforze odbiorczym
	enc28j60_Zapisz(ERXRDPTL, RXSTART_INIT & 0xFF);
	enc28j60_Zapisz(ERXRDPTH, RXSTART_INIT >> 8);

	// Ustawienie adresu konca bufora odbiorczego
	enc28j60_Zapisz(ERXNDL, RXSTOP_INIT & 0xFF);
	enc28j60_Zapisz(ERXNDH, RXSTOP_INIT >> 8);

	// Ustawienie adresu poczatku bufora nadawczego
	enc28j60_Zapisz(ETXSTL, TXSTART_INIT & 0xFF);
	enc28j60_Zapisz(ETXSTH, TXSTART_INIT >> 8);
	// Ustawienie adresu konca bufora nadawczego
	enc28j60_Zapisz(ETXNDL, TXSTOP_INIT & 0xFF);
	enc28j60_Zapisz(ETXNDH, TXSTOP_INIT >> 8);

	// Filtry: przyjmujemy tylko pakiety o naszym MAC i poprawna CRC
	enc28j60_Zapisz(ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN);

	//enc28j60_Zapisz(MACON2, 0x00); Adres zarezerwowany

	// patrz s. 34, dlaczego nie uzyc enc28j60_Zapisz() ?
	enc28j60_ZapiszKod(ENC28J60_USTAW_BITY, MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);

	// Non-Nack-to-Back
	enc28j60_Zapisz(MAIPGL, 0x12);
	enc28j60_Zapisz(MAIPGH, 0x0C);

	// Back-to-Back
	enc28j60_Zapisz(MABBIPG, 0x12);

	// Maksymalny rozmiar pakietu
	enc28j60_Zapisz(MAMXFLL, MAX_ROZMIAR_PAKIETU & 0xFF);
	enc28j60_Zapisz(MAMXFLH, MAX_ROZMIAR_PAKIETU >> 8);

	// Przypisanie adresu MAC, odwrotna kolejnosc
	//
    enc28j60_Zapisz(MAADR5, macadr[0]);
    enc28j60_Zapisz(MAADR4, macadr[1]);
    enc28j60_Zapisz(MAADR3, macadr[2]);
    enc28j60_Zapisz(MAADR2, macadr[3]);
    enc28j60_Zapisz(MAADR1, macadr[4]);
    enc28j60_Zapisz(MAADR0, macadr[5]);

    // patrz s. 37
	enc28j60_ZapiszPhy(PHCON2, PHCON2_HDLDIS);

	// Konfiguracja koncowa, przelaczenie do Banku 0
	// Czy jest potrzeba, skoro ten rejestr jest mapowany na wszystkie banki ?

	enc28j60_UstawBank(ECON1);
	// Po co wlaczac te bity, skoro program z tego nie korzysta ?
	enc28j60_ZapiszKod(ENC28J60_USTAW_BITY, EIE, EIE_INTIE|EIE_PKTIE);

	// Pakiety, ktore przejda przez filtr zostana zapisane do bufora odbiorczego
	enc28j60_ZapiszKod(ENC28J60_USTAW_BITY, ECON1, ECON1_RXEN);
}

void enc28j60_WyslijPakiet(uint16_t dl, uint8_t* pakiet)
{
	// Komendy w tej funkcji nie zgadzaja sie z DTR zupelnie, patrz s. 40
	// Czy inny pakiet nie jest nadawany ?
	while (enc28j60_CzytajKod(ENC28J60_CZYTAJ_REJ_KONTR, ECON1) & ECON1_TXRTS);
	// Reset the transmit logic problem. Unblock stall in the transmit logic.
	// See Rev. B4 Silicon Errata point 12.
	if((enc28j60_Czytaj(EIR) & EIR_TXERIF))
	{
		enc28j60_ZapiszKod(ENC28J60_USTAW_BITY, ECON1, ECON1_TXRST);
		enc28j60_ZapiszKod(ENC28J60_KASUJ_BITY, ECON1, ECON1_TXRST);
		enc28j60_ZapiszKod(ENC28J60_KASUJ_BITY, EIR, EIR_TXERIF);
		_delay_ms(10);
	}
		// Set the write pointer to start of transmit buffer area
		enc28j60_Zapisz(EWRPTL, TXSTART_INIT & 0xFF);
		enc28j60_Zapisz(EWRPTH, TXSTART_INIT >> 8);
		// Set the TXND pointer to correspond to the packet size given
		enc28j60_Zapisz(ETXNDL, (TXSTART_INIT + dl)& 0xFF);
		enc28j60_Zapisz(ETXNDH, (TXSTART_INIT + dl) >> 8);
		// write per-packet control byte (0x00 means use macon3 settings)
		enc28j60_ZapiszKod(ENC28J60_ZAPISZ_BUFOR, 0, 0x00);
		// copy the packet into the transmit buffer
		enc28j60_ZapiszBufor(dl, pakiet);
		// send the contents of the transmit buffer onto the network
		enc28j60_ZapiszKod(ENC28J60_USTAW_BITY, ECON1, ECON1_TXRTS);
}

// Czy jest nowy pakiet w buforze odbiorczym ?
uint8_t enc28j60_JestPakiet()
{
	if(enc28j60_Czytaj(EPKTCNT) == 0)
	{
		return 0;
	}
	return 1;
}

// patrz s. 43
uint16_t enc28j60_OdbierajPakiet(uint16_t maxdl, uint8_t* pakiet)
{
	// Nie ma pakietu
	if(!enc28j60_JestPakiet()) return 0;

	uint16_t status;
	uint16_t dl;

	// Ustaw wskaznik czytania na poczatku pakietu
	// W przypadku pierwszego pakietu WskNastepnyPakiet = RXSTART_INIT
	enc28j60_Zapisz(ERDPTL, (WskNastepnyPakiet & 0xFF));
	enc28j60_Zapisz(ERDPTH, (WskNastepnyPakiet) >> 8);

	// Ustaw wskaznik na nowy pakiet
	WskNastepnyPakiet  = enc28j60_CzytajKod(ENC28J60_CZYTAJ_BUFOR, 0);
	WskNastepnyPakiet |= enc28j60_CzytajKod(ENC28J60_CZYTAJ_BUFOR, 0) << 8;

	// Dlugosc pakietu
	dl  = enc28j60_CzytajKod(ENC28J60_CZYTAJ_BUFOR, 0);
	dl |= enc28j60_CzytajKod(ENC28J60_CZYTAJ_BUFOR, 0) << 8;
	// Odjac dlugosc sumy kontrolnej, 4 bajty
	dl -= 4;

	// Status ramki
	status  = enc28j60_CzytajKod(ENC28J60_CZYTAJ_BUFOR, 0);
	status |= ((uint16_t)enc28j60_CzytajKod(ENC28J60_CZYTAJ_BUFOR, 0)) << 8;

	if(dl > maxdl - 1) dl = maxdl - 1;
	// bit 23 w tabeli na s. 44
	if((status & 0x80) == 0)
	{
		// niepoprawna ramka
		dl = 0;
	}
	else
	{
		enc28j60_CzytajBufor(dl, pakiet);
	}
	// Nie wiem co robi ta czesc kodu
	if(WskNastepnyPakiet - 1 > RXSTOP_INIT)
	{ // RXSTART_INIT is zero, no test for gNextPacketPtr less than RXSTART_INIT.
		enc28j60_Zapisz(ERXRDPTL, (RXSTOP_INIT) & 0xFF);
		enc28j60_Zapisz(ERXRDPTH, (RXSTOP_INIT) >> 8);
	}
	else
	{
		enc28j60_Zapisz(ERXRDPTL, (WskNastepnyPakiet - 1) & 0xFF);
		enc28j60_Zapisz(ERXRDPTH, (WskNastepnyPakiet - 1) >> 8);
	}
		// decrement the packet counter indicate we are done with this packet
	enc28j60_ZapiszKod(ENC28J60_USTAW_BITY, ECON2, ECON2_PKTDEC);
	return dl;
}

