/*
 * enc28j60.cpp
 *
 *  Created on: 25 maj 2017
 *      Author: tomek
 */
#include "enc28j60.h"

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include "../usart.h"
#include "stos.h"

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
unsigned char Adres_MAC[6];
unsigned char enc_revid = 0;

static void SPI_Init()
{
	ENC28J60_SPI_DDR |= ((1 << ENC28J60_SPI_MOSI) | (1 << ENC28J60_SPI_SCK));
	ENC28J60_SPI_DDR &= ~(1 << ENC28J60_SPI_MISO);
	ENC28J60_SPI_PORT |= (1 << ENC28J60_SPI_SCK);

	ENC28J60_CS_DDR |= (1 << ENC28J60_CS);
	ENC28J60_CS_PASYWNY;

	SPCR |= (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);
	//SPSR |= (1<<SPI2X);
	_delay_ms(100);
}

static void SPI_Wyslij(char bajt)
{
	SPDR = bajt;
	spi_czekaj();
}

static uint8_t SPI_Odbierz()
{
	uint8_t bajt = SPDR;
	return bajt;
}

static void enc28j60_Reset()
{
	ENC28J60_CS_AKTYWNY;
	SPI_Wyslij(ENC28J60_RESET);
	ENC28J60_CS_PASYWNY;
	// patrz Errata #2
	_delay_us(300);
}

static void enc28j60_RejWyczyscBity(uint8_t rejestr, uint8_t bity)
{
	// UWAGA!! Trzeba wczesniej ustawic bank !!
	// Jesli nie operuje sie na rejestrach zmapowanych
	uint8_t adres = rejestr & ADR_MASKA;
	ENC28J60_CS_AKTYWNY;
	SPI_Wyslij(ENC28J60_WYCZYSC_BITY | adres);
	SPI_Wyslij(bity);
	ENC28J60_CS_PASYWNY;
}

static void enc28j60_RejUstawBity(uint8_t rejestr, uint8_t bity)
{
	// UWAGA!! Trzeba wczesniej ustawic bank !!
	// Jesli nie operuje sie na rejestrach zmapowanych
	uint8_t adres = rejestr & ADR_MASKA;
	ENC28J60_CS_AKTYWNY;
	SPI_Wyslij(ENC28J60_USTAW_BITY | adres);
	SPI_Wyslij(bity);
	ENC28J60_CS_PASYWNY;
}

static unsigned char enc28j60_RejCzytaj(unsigned char rejestr)
{
	uint8_t wart_rejestru;
	uint8_t adres = rejestr & ADR_MASKA;
	// Jesli czytamy rejestry niewspolne
	if(adres < 0x1A)
	{
		// 2 bity okreslajace numer banku
		unsigned char bank = (rejestr & BANK_MASKA) >> 5;
		if(bank != enc28j60_Bank)
		{
			// Kasowanie bitow BSEL0, BSEL1 poprzez wartosc 0x03
			enc28j60_RejWyczyscBity(ENC_REG_ECON1, (0x03 << ENC_BIT_BSEL0));

			if(bank)
			{
				enc28j60_RejUstawBity(ENC_REG_ECON1, bank << ENC_BIT_BSEL0);
			}
			enc28j60_Bank = bank;
		}
	}
	// Czytamy rejestry zmapowane do wszystkich bankow
	ENC28J60_CS_AKTYWNY;
	SPI_Wyslij(ENC28J60_CZYTAJ_REJ_KONTR | adres);
	SPI_Wyslij(0x00);
	// Rejestr gdzie trzeba odczytac dodatkowy bajt
	if(rejestr & MAC_MII_MASKA) SPI_Wyslij(0x00);
	wart_rejestru = SPI_Odbierz();
	ENC28J60_CS_PASYWNY;
	return wart_rejestru;
}

static void enc28j60_RejZapisz(uint8_t rejestr, uint8_t wart_rejestru)
{
	uint8_t adres = rejestr & ADR_MASKA;
		// Jesli czytamy rejestry niewspolne
		if(adres < 0x1A)
		{
			// 2 bity okreslajace numer banku
			unsigned char bank = (rejestr & BANK_MASKA) >> 5;
			if(bank != enc28j60_Bank)
			{
				// Kasowanie bitow BSEL0, BSEL1 poprzez wartosc 0x03
				enc28j60_RejWyczyscBity(ENC_REG_ECON1, (0x03 << ENC_BIT_BSEL0));
				if(bank)
				{
					enc28j60_RejUstawBity(ENC_REG_ECON1, bank << ENC_BIT_BSEL0);
				}
				enc28j60_Bank = bank;
			}
		}
		ENC28J60_CS_AKTYWNY;
		SPI_Wyslij(ENC28J60_ZAPISZ_REJ_KONTR | adres);
		SPI_Wyslij(wart_rejestru);
		ENC28J60_CS_PASYWNY;
}
/*
static uint16_t enc28j60_CzytajPhy(uint8_t rejPhy)
{
	uint16_t wartosc_rej;

	enc28j60_RejZapisz(ENC_REG_MIREGADR, rejPhy);
	enc28j60_RejZapisz(ENC_REG_MICMD, (1 << ENC_BIT_MIIRD));
	_delay_us(10);
	while(enc28j60_RejCzytaj(ENC_REG_MISTAT) & (1 << ENC_BIT_BUSY));
	enc28j60_RejZapisz(ENC_REG_MICMD, 0x00);
	wartosc_rej = (((uint16_t) enc28j60_RejCzytaj(ENC_REG_MIRDH )) << 8);
	wartosc_rej |= ((uint16_t) enc28j60_RejCzytaj(ENC_REG_MIRDL));

	return wartosc_rej;
}
*/
static void enc28j60_ZapiszPhy(uint8_t rejPhy, uint16_t wartosc_rej)
{
	enc28j60_RejZapisz(ENC_REG_MIREGADR, rejPhy);
	enc28j60_RejZapisz(ENC_REG_MIWRL, LO8(wartosc_rej));
	enc28j60_RejZapisz(ENC_REG_MIWRH, HI8(wartosc_rej));
	_delay_us(10);
	while(enc28j60_RejCzytaj(ENC_REG_MISTAT) & (1 << ENC_BIT_BUSY));
}

static void enc28j60_CzytajBuf(uint8_t *buf, uint16_t dl)
{
	ENC28J60_CS_AKTYWNY;
	SPI_Wyslij(ENC28J60_CZYTAJ_BUFOR);
	for(; dl > 0; dl--, buf++)
	{
		SPI_Wyslij(0x00);
		*buf = SPI_Odbierz();
	}
	ENC28J60_CS_PASYWNY;
}

static void enc28j60_ZapiszBuf(uint8_t *buf, uint16_t dl)
{
	ENC28J60_CS_AKTYWNY;
	SPI_Wyslij(ENC28J60_ZAPISZ_BUFOR);
	for(; dl > 0; dl--, buf++ ) {
		SPI_Wyslij(*buf);
	}
	ENC28J60_CS_PASYWNY;
}

// ------------------------------------------------------------------

void enc28j60_WyslijPakiet(uint16_t dl, uint8_t *buf)
{
	unsigned char ctrl = 0;
	unsigned int ms = 100;

//	ENC_DEBUG("enc_send: %i bytes\n", len);

	// Czekaj do 100 ms na zakonczenie poprzedniej transmisji
	// Jesli transmisja sie zakonczy wczesniej to nie czekaj
	while( ms--)
	{
		if(!(enc28j60_RejCzytaj(ENC_REG_ECON1) & (1 << ENC_BIT_TXRTS))) break;
		_delay_ms(1);
	}
	#ifdef FULL_DUPLEX
	// Reset jesli bit nadal ustawiony, errata #12: Reset TX Logic
	if(enc28j60_RejCzytaj(ENC_REG_ECON1) & (1 << ENC_BIT_TXRTS))
	{
	#else
	if(1)
	{
	#endif
		//ENC_DEBUG("enc_send: reset tx logic\n");
		enc28j60_RejUstawBity(ENC_REG_ECON1, (1 << ENC_BIT_TXRST));
		enc28j60_RejWyczyscBity(ENC_REG_ECON1, (1 << ENC_BIT_TXRST));
	}
	// Ustaw wskaznik zapisu poczatek
	enc28j60_RejZapisz(ENC_REG_EWRPTL, LO8(ENC_TX_BUFFER_START));
	enc28j60_RejZapisz(ENC_REG_EWRPTH, HI8(ENC_TX_BUFFER_START));

	// Ustaw wskaznik zapisu koniec
	enc28j60_RejZapisz(ENC_REG_ETXNDL, LO8(ENC_TX_BUFFER_START + dl));
	enc28j60_RejZapisz(ENC_REG_ETXNDH, HI8(ENC_TX_BUFFER_START + dl));

	// Bajt kontrolny
	enc28j60_ZapiszBuf(&ctrl, 1);

	// Kopiowanie pakietu do bufora
	enc28j60_ZapiszBuf(buf, dl);

	// Czyszczenie flagi
	enc28j60_RejWyczyscBity(ENC_REG_EIR, (1 << ENC_BIT_TXIF));

	// Rozpoczecie transmiji
	enc28j60_RejUstawBity(ENC_REG_ECON1, (1 << ENC_BIT_TXRTS));
}

uint16_t enc28j60_OdbierzPakiet(uint16_t rozmiar_buf, uint8_t *buf)
{
	uint8_t rx_naglowek[6];
	uint16_t dl, status;
	uint8_t u;
	// Licznik pakietow
	u = enc28j60_RejCzytaj(ENC_REG_EPKTCNT);

	//ENC_DEBUG("enc_receive: EPKTCNT=%i\n", (int) u);
	// Nie ma nowego pakietu, opusc funkcje
	if(u == 0) return 0;
	//USART_WyslijRamke("Jest pakiet!\n");
	//Ustaw wskaznik na nastepny pakiet
	enc28j60_RejZapisz(ENC_REG_ERDPTL, LO8(WskNastepnyPakiet));
	enc28j60_RejZapisz(ENC_REG_ERDPTH, HI8(WskNastepnyPakiet));

	// Naglowek
	enc28j60_CzytajBuf(rx_naglowek, sizeof(rx_naglowek));

	WskNastepnyPakiet  =             rx_naglowek[0];
	WskNastepnyPakiet |= ((unsigned)(rx_naglowek[1])) << 8;
	dl                 =             rx_naglowek[2];
	dl                |= ((unsigned)(rx_naglowek[3])) << 8;
	status             =             rx_naglowek[4];
	status            |= ((unsigned)(rx_naglowek[5])) << 8;
	// If the receive OK bit is not 1 or the zero bit is not zero, or the packet is larger then the buffer, reset the enc chip and SPI
	if ((!(status & (1 << 7))) || (status & 0x8000) || (dl > rozmiar_buf))
	{
		enc28j60_Init();
	}
	//ENC_DEBUG("enc_receive: status=%4x\n", (unsigned) status);

	// bez CRC
	dl -= 4;

	// Jesli rozmiar pakietu wiekszy niz bufor to dopasuj
	if( dl > rozmiar_buf ) dl = rozmiar_buf;

		// Kopiuj pakiet do bufora
		enc28j60_CzytajBuf(buf, dl);

		// Zwolnienie adresow dla nastepnego pakietu
		if(WskNastepnyPakiet - 1 > ENC_RX_BUFFER_END
		    || WskNastepnyPakiet - 1 < ENC_RX_BUFFER_START )
		{
			enc28j60_RejZapisz(ENC_REG_ERXRDPTL, LO8(ENC_RX_BUFFER_END));
			enc28j60_RejZapisz(ENC_REG_ERXRDPTH, HI8(ENC_RX_BUFFER_END));
		}
		else
		{
			enc28j60_RejZapisz(ENC_REG_ERXRDPTL, LO8(WskNastepnyPakiet - 1));
			enc28j60_RejZapisz(ENC_REG_ERXRDPTH, HI8(WskNastepnyPakiet - 1));
		}

		// trigger a decrement of the rx packet counter
		// this will clear PKTIF if EPKTCNT reaches 0
		enc28j60_RejUstawBity(ENC_REG_ECON2, (1 << ENC_BIT_PKTDEC));

		// return number of bytes written to the buffer
		//ENC_DEBUG("enc_receive: %i bytes\n", len);
		return dl;
}

void enc28j60_ZrzutRejestrow()
{
	unsigned char v;
//	unsigned int  u;

//	v = enc28j60_RejCzytaj(ENC_REG_EIE);   USART_WyslijLiczbe((unsigned)v, 2);
//	v = enc_read_reg(ENC_REG_EIR);         ENC_DEBUG("EIR %2x\n", (unsigned)v);
//	v = enc_read_reg(ENC_REG_ESTAT);       ENC_DEBUG("ESTAT %2x\n", (unsigned)v);
//	v = enc_read_reg(ENC_REG_ECON2);       ENC_DEBUG("ECON2 %2x\n", (unsigned)v);
	//v = enc_read_reg(ENC_REG_ECON1);       ENC_DEBUG("ECON1 %2x\n", (unsigned)v);

	//v = enc_read_reg(ENC_REG_ERDPTH);      u = (((unsigned)(v & 0x1F)) << 8);
	//u |= enc_read_reg(ENC_REG_ERDPTL);     ENC_DEBUG("ERDPT %4x\n", u);
	//v = enc_read_reg(ENC_REG_EWRPTH);      u = (((unsigned)(v & 0x1F)) << 8);
	//u |= enc_read_reg(ENC_REG_EWRPTL);     ENC_DEBUG("EWRPT %4x\n", u);
	//v = enc_read_reg(ENC_REG_ETXSTH);      u = (((unsigned)(v & 0x1F)) << 8);
	//u |= enc_read_reg(ENC_REG_ETXSTL);     ENC_DEBUG("ETXST %4x\n", u);
	//v = enc_read_reg(ENC_REG_ETXNDH);      u = (((unsigned)(v & 0x1F)) << 8);
	//u |= enc_read_reg(ENC_REG_ETXNDL);     ENC_DEBUG("ETXND %4x\n", u);
	//v = enc_read_reg(ENC_REG_ERXSTH);      u = (((unsigned)(v & 0x1F)) << 8);
	//u |= enc_read_reg(ENC_REG_ERXSTL);     ENC_DEBUG("ERXST %4x\n", u);
	//v = enc_read_reg(ENC_REG_ERXNDH);      u = (((unsigned)(v & 0x1F)) << 8);
	//u |= enc_read_reg(ENC_REG_ERXNDL);     ENC_DEBUG("ERXND %4x\n", u);
	//v = enc_read_reg(ENC_REG_ERXRDPTH);    u = (((unsigned)(v & 0x1F)) << 8);
	//u |= enc_read_reg(ENC_REG_ERXRDPTL);   ENC_DEBUG("ERXRDPT %4x\n", u);
	//v = enc_read_reg(ENC_REG_ERXWRPTH);    u = (((unsigned)(v & 0x1F)) << 8);
	//u |= enc_read_reg(ENC_REG_ERXWRPTL);   ENC_DEBUG("ERXWRPT %4x\n", u);

	//v = enc_read_reg(ENC_REG_EDMASTH);     u = (((unsigned)(v & 0x1F)) << 8);
	//u |= enc_read_reg(ENC_REG_EDMASTL);    ENC_DEBUG("EDMAST %4x\n", u);
	//v = enc_read_reg(ENC_REG_EDMANDH);     u = (((unsigned)(v & 0x1F)) << 8);
	//u |= enc_read_reg(ENC_REG_EDMANDL);    ENC_DEBUG("EDMAND %4x\n", u);
	//v = enc_read_reg(ENC_REG_EDMADSTH);    u = (((unsigned)(v & 0x1F)) << 8);
	//u |= enc_read_reg(ENC_REG_EDMADSTL);   ENC_DEBUG("EDMADST %4x\n", u);
	//v = enc_read_reg(ENC_REG_EDMACSH);     u = (((unsigned)(v & 0x1F)) << 8);
	//u |= enc_read_reg(ENC_REG_EDMACSL);    ENC_DEBUG("EDMACS %4x\n", u);

	//v = enc_read_reg(ENC_REG_EHT0);        ENC_DEBUG("EHT0 %2x\n", (unsigned) v);
	//v = enc_read_reg(ENC_REG_EHT1);        ENC_DEBUG("EHT1 %2x\n", (unsigned) v);
	//v = enc_read_reg(ENC_REG_EHT2);        ENC_DEBUG("EHT2 %2x\n", (unsigned) v);
	//v = enc_read_reg(ENC_REG_EHT3);        ENC_DEBUG("EHT3 %2x\n", (unsigned) v);
	//v = enc_read_reg(ENC_REG_EHT4);        ENC_DEBUG("EHT4 %2x\n", (unsigned) v);
	//v = enc_read_reg(ENC_REG_EHT5);        ENC_DEBUG("EHT5 %2x\n", (unsigned) v);
	//v = enc_read_reg(ENC_REG_EHT6);        ENC_DEBUG("EHT6 %2x\n", (unsigned) v);
	//v = enc_read_reg(ENC_REG_EHT7);        ENC_DEBUG("EHT7 %2x\n", (unsigned) v);

	//v = enc_read_reg(ENC_REG_EPMM0);       ENC_DEBUG("EPMM0 %2x\n", (unsigned) v);
	//v = enc_read_reg(ENC_REG_EPMM1);       ENC_DEBUG("EPMM1 %2x\n", (unsigned) v);
	//v = enc_read_reg(ENC_REG_EPMM2);       ENC_DEBUG("EPMM2 %2x\n", (unsigned) v);
	//v = enc_read_reg(ENC_REG_EPMM3);       ENC_DEBUG("EPMM3 %2x\n", (unsigned) v);
	//v = enc_read_reg(ENC_REG_EPMM4);       ENC_DEBUG("EPMM4 %2x\n", (unsigned) v);
	//v = enc_read_reg(ENC_REG_EPMM5);       ENC_DEBUG("EPMM5 %2x\n", (unsigned) v);
	//v = enc_read_reg(ENC_REG_EPMM6);       ENC_DEBUG("EPMM6 %2x\n", (unsigned) v);
	//v = enc_read_reg(ENC_REG_EPMM7);       ENC_DEBUG("EPMM7 %2x\n", (unsigned) v);

	//v = enc_read_reg(ENC_REG_EPMCSH);      u = (((unsigned)(v & 0x1F)) << 8);
	//u |= enc_read_reg(ENC_REG_EPMCSL);     ENC_DEBUG("EPMCS %4x\n", u);
	//v = enc_read_reg(ENC_REG_EPMOH);       u = (((unsigned)(v & 0x1F)) << 8);
	//u |= enc_read_reg(ENC_REG_EPMOL);      ENC_DEBUG("EPMO %4x\n", u);

	//v = enc_read_reg(ENC_REG_EWOLIE);      ENC_DEBUG("EWOLIE %2x\n", (unsigned) v);
	//v = enc_read_reg(ENC_REG_EWOLIR);      ENC_DEBUG("EWOLIR %2x\n", (unsigned) v);

	//v = enc_read_reg(ENC_REG_ERXFCON);     ENC_DEBUG("ERXFCON %2x\n", (unsigned) v);
	//v = enc_read_reg(ENC_REG_EPKTCNT);     ENC_DEBUG("EPKTCNT %2x\n", (unsigned) v);

	//v = enc_read_reg(ENC_REG_MACON1);      ENC_DEBUG("MACON1 %2x\n", (unsigned) v);
	//v = enc_read_reg(ENC_REG_MACON2);      ENC_DEBUG("MACON2 %2\nx", (unsigned) v);
	//v = enc_read_reg(ENC_REG_MACON3);      ENC_DEBUG("MACON3 %2x\n", (unsigned) v);
	//v = enc_read_reg(ENC_REG_MACON4);      ENC_DEBUG("MACON4 %2x\n", (unsigned) v);
	//v = enc_read_reg(ENC_REG_MABBIPG);     ENC_DEBUG("MABBIPG %2x\n", (unsigned) v);
	//v = enc_read_reg(ENC_REG_MAIPGH);      u = (((unsigned)(v & 0x1F)) << 8);
	//u |= enc_read_reg(ENC_REG_MAIPGL);     ENC_DEBUG("MAIPG %4x\n", u);
	//v = enc_read_reg(ENC_REG_MACLCON1);    ENC_DEBUG("MACLCON1 %2x\n", (unsigned) v);
	//v = enc_read_reg(ENC_REG_MACLCON2);    ENC_DEBUG("MACLCON2 %2x\n", (unsigned) v);

	//v = enc_read_reg(ENC_REG_MAMXFLH);     u = (((unsigned)(v & 0x1F)) << 8);
	//u |= enc_read_reg(ENC_REG_MAMXFLL);    ENC_DEBUG("MAMXFL %4x\n", u);
	//v = enc_read_reg(ENC_REG_MAPHSUP);     ENC_DEBUG("MAPHSUP %2x\n", (unsigned) v);

	//v = enc_read_reg(ENC_REG_MICON);       ENC_DEBUG("MICON %2x\n", (unsigned) v);

	//v = enc_read_reg(ENC_REG_MICMD);       ENC_DEBUG("MICMD %2x\n", (unsigned) v);
	//v = enc_read_reg(ENC_REG_MIREGADR);    ENC_DEBUG("MIREGADR %2x\n", (unsigned) v);
	//v = enc_read_reg(ENC_REG_MIWRH);       u = (((unsigned)(v & 0x1F)) << 8);
	//u |= enc_read_reg(ENC_REG_MIWRL);      ENC_DEBUG("MIWR %4x\n", u);
	//v = enc_read_reg(ENC_REG_MIRDH);       u = (((unsigned)(v & 0x1F)) << 8);
	//u |= enc_read_reg(ENC_REG_MIRDL);      ENC_DEBUG("MIRD %4x\n", u);
//	v = enc28j60_RejCzytaj(ENC_REG_EREVID);      USART_Debug("\nREV: ", v, 16);
//	v = enc28j60_RejCzytaj(ENC_REG_MAADR5);      USART_Debug("\nMAC5: ", v, 16);
//	v = enc28j60_RejCzytaj(ENC_REG_MAADR4);      USART_Debug("\nMAC4: ", v, 16);
//	v = enc28j60_RejCzytaj(ENC_REG_MAADR3);      USART_Debug("\nMAC3: ", v, 16);
//	v = enc28j60_RejCzytaj(ENC_REG_MAADR2);      USART_Debug("\nMAC2: ", v, 16);
//	v = enc28j60_RejCzytaj(ENC_REG_MAADR1);      USART_Debug("\nMAC1: ", v, 16);
//	v = enc28j60_RejCzytaj(ENC_REG_MAADR0);      USART_Debug("\nMAC0: ", v, 16);

	//v = enc_read_reg(ENC_REG_EBSTSD);      ENC_DEBUG("EBSTSD %2x\n", (unsigned) v);
	//v = enc_read_reg(ENC_REG_EBSTCON);     ENC_DEBUG("EBSTCON %2x\n", (unsigned) v);
	//v = enc_read_reg(ENC_REG_EBSTCSH);     u = (((unsigned)(v & 0x1F)) << 8);
	//u |= enc_read_reg(ENC_REG_EBSTCSL);    ENC_DEBUG("EBSTCS %4x\n", u);

	//v = enc_read_reg(ENC_REG_MISTAT);      ENC_DEBUG("MISTAT %2x\n", (unsigned) v);


	//v = enc_read_reg(ENC_REG_ECOCON);      ENC_DEBUG("ECOCON %2x\n", (unsigned) v);
	//v = enc_read_reg(ENC_REG_EFLOCON);     ENC_DEBUG("EFLOCON %2x\n", (unsigned) v);
	//v = enc_read_reg(ENC_REG_EPAUSH);      u = (((unsigned)(v & 0x1F)) << 8);
	//u |= enc_read_reg(ENC_REG_EPAUSL);     ENC_DEBUG("EPAUS %4x\n", u);

	//u = enc_read_phyreg(ENC_REG_PHCON1);   ENC_DEBUG("PHCON1 %4x\n", u);
	//u = enc_read_phyreg(ENC_REG_PHSTAT1);  ENC_DEBUG("PHSTAT1 %4x\n", u);
	//u = enc_read_phyreg(ENC_REG_PHID1);    ENC_DEBUG("PHID1 %4x\n", u);
	//u = enc_read_phyreg(ENC_REG_PHID2);    ENC_DEBUG("PHID2 %4x\n", u);
	//u = enc_read_phyreg(ENC_REG_PHCON2);   ENC_DEBUG("PHCON2 %4x\n", u);
	//u = enc_read_phyreg(ENC_REG_PHSTAT2);  ENC_DEBUG("PHSTAT2 %4x\n", u);
	//u = enc_read_phyreg(ENC_REG_PHIE);     ENC_DEBUG("PHIE %4x\n", u);
	//u = enc_read_phyreg(ENC_REG_PHIR);     ENC_DEBUG("PHIR %4x\n", u);
	//u = enc_read_phyreg(ENC_REG_PHLCON);   ENC_DEBUG("PHLCON %4x\n", u);

}



void enc28j60_Init()
{
	SPI_Init();
	enc28j60_Reset();
	while(!(enc28j60_RejCzytaj(ENC_REG_ESTAT) & (1 << ENC_BIT_CLKRDY)));
	_delay_ms(20);


	enc28j60_RejZapisz(ENC_REG_ETXSTL, LO8(ENC_TX_BUFFER_START));
	enc28j60_RejZapisz(ENC_REG_ETXSTH, HI8(ENC_TX_BUFFER_START));
	enc28j60_RejZapisz(ENC_REG_ETXNDL, LO8(ENC_TX_BUFFER_END));
	enc28j60_RejZapisz(ENC_REG_ETXNDH, HI8(ENC_TX_BUFFER_END));

		// rx buffer
	enc28j60_RejZapisz(ENC_REG_ERXSTL, LO8(ENC_RX_BUFFER_START));
	enc28j60_RejZapisz(ENC_REG_ERXSTH, HI8(ENC_RX_BUFFER_START));
	enc28j60_RejZapisz(ENC_REG_ERXNDL, LO8(ENC_RX_BUFFER_END));
	enc28j60_RejZapisz(ENC_REG_ERXNDH, HI8(ENC_RX_BUFFER_END));

		// push mac out of reset
	enc28j60_RejZapisz(ENC_REG_MACON2, 0x00);

		// mac receive enable, rx and tx pause control frames enable
	enc28j60_RejZapisz(ENC_REG_MACON1, (1<<ENC_BIT_MARXEN) | (1<<ENC_BIT_RXPAUS) | (1<<ENC_BIT_TXPAUS));

		#ifdef FULL_DUPLEX
		// mac auto padding of small packets, add crc, frame length check, full duplex
	enc28j60_RejZapisz(ENC_REG_MACON3, (1<<ENC_BIT_PADCFG0) | (1<<ENC_BIT_TXCRCEN)
				 | (1<<ENC_BIT_FRMLNEN) | (1<<ENC_BIT_FULDPX));
		#else
		// mac auto padding of small packets, add crc, frame length check, half duplex
	enc28j60_RejZapisz(ENC_REG_MACON3, (1<<ENC_BIT_PADCFG0) | (1<<ENC_BIT_TXCRCEN)
				 | (1<<ENC_BIT_FRMLNEN));
		#endif

		// max framelength 1518
	enc28j60_RejZapisz(ENC_REG_MAMXFLL, LO8(1518));
	enc28j60_RejZapisz(ENC_REG_MAMXFLH, HI8(1518));

		#ifdef FULL_DUPLEX
		// back-to-back inter packet gap delay time (0x15 for full duplex)
	enc28j60_RejZapisz(ENC_REG_MABBIPG, 0x15);
		#else
		// back-to-back inter packet gap delay time (0x12 for half duplex)
	enc28j60_RejZapisz(ENC_REG_MABBIPG, 0x12);
		#endif

		// non back-to-back inter packet gap delay time (should be 0x12)
	enc28j60_RejZapisz(ENC_REG_MAIPGL, 0x12);

		#ifndef FULL_DUPLEX
		// non back-to-back inter packet gap delay time high (should be 0x0C for half duplex)
	enc28j60_RejZapisz(ENC_REG_MAIPGH, 0x0C);
		#endif

		// our mac address
	enc28j60_RejZapisz(ENC_REG_MAADR5, ADR_MAC1);
	enc28j60_RejZapisz(ENC_REG_MAADR4, ADR_MAC2);
	enc28j60_RejZapisz(ENC_REG_MAADR3, ADR_MAC3);
	enc28j60_RejZapisz(ENC_REG_MAADR2, ADR_MAC4);
	enc28j60_RejZapisz(ENC_REG_MAADR1, ADR_MAC5);
	enc28j60_RejZapisz(ENC_REG_MAADR0, ADR_MAC6);

		// disable CLKOUT pin
	enc28j60_RejZapisz(ENC_REG_ECOCON, 0x00);

		// end of enc registers marker
		// now the phy registers (with 2 bytes data each)

		#ifdef FULL_DUPLEX
		// set the PDPXMD full duplex mode bit on the phy
		#define ENC_REG_PHCON1_VALUE (0x0000 | (1 << ENC_BIT_PDPXMD))
		enc28j60_ZapiszPhy(ENC_REG_PHCON1, HI8(ENC_REG_PHCON1_VALUE)| LO8(ENC_REG_PHCON1_VALUE));
		#endif

		#ifndef FULL_DUPLEX
		// in half duplex do not loop back transmitted data
		#define ENC_REG_PHCON2_VALUE (0x0000 | (1 << ENC_BIT_HDLDIS))
		enc28j60_ZapiszPhy(ENC_REG_PHCON2, HI8(ENC_REG_PHCON2_VALUE)| LO8(ENC_REG_PHCON2_VALUE));
		#endif

		// leds: leda (yellow) rx and tx activity, stretch to 40ms
		//       ledb (green)  link status
		#define ENC_REG_PHCON_VALUE (0x0000 | (1 << ENC_BIT_STRCH) \
				     | (7 << ENC_BIT_LACFG0)               \
				     | (4 << ENC_BIT_LBCFG0))
		//enc28j60_ZapiszPhy(ENC_REG_PHLCON, HI8(ENC_REG_PHCON_VALUE)| LO8(ENC_REG_PHCON_VALUE));

		WskNastepnyPakiet = ENC_RX_BUFFER_START;

			// configure the enc interrupt sources
			enc28j60_RejZapisz( ENC_REG_EIE, (1 << ENC_BIT_INTIE)  | (1 << ENC_BIT_PKTIE)
						  | (0 << ENC_BIT_DMAIE)  | (0 << ENC_BIT_LINKIE)
						  | (0 << ENC_BIT_TXIE)   | (0 << ENC_BIT_WOLIE)
						  | (0 << ENC_BIT_TXERIE) | (0 << ENC_BIT_RXERIE));

			// enable receive
			enc28j60_RejUstawBity(ENC_REG_ECON1, (1 << ENC_BIT_RXEN));
}
