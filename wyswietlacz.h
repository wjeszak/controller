/*
 * wyswietlacz.h
 *
 *  Created on: 15 cze 2017
 *      Author: tomek
 */

#ifndef WYSWIETLACZ_H_
#define WYSWIETLACZ_H_
#include <avr/io.h>
#include <inttypes.h>

// Numeracja wg. rozmieszczenia na plytce
#define WYSW1_DDR 				DDRD
#define WYSW2_DDR 				DDRC
#define WYSW3_DDR 				DDRB
#define WYSW4_DDR 				DDRB

#define WYSW1_PORT	 			PORTD
#define WYSW2_PORT 				PORTC
#define WYSW3_PORT 				PORTB
#define WYSW4_PORT 				PORTB

#define WYSW1_PIN				(1 << 4)
#define WYSW2_PIN				(1 << 0)
#define WYSW3_PIN 				(1 << 3)
#define WYSW4_PIN				(1 << 4)

#define WYSW_SEGMENTY_DDR 		DDRA
#define WYSW_SEGMENTY_PORT 		PORTA

#define WYSW_ZNAK_ZERO 			0x40
#define WYSW_ZNAK_JEDEN			0x79
#define WYSW_ZNAK_DWA			0x24
#define WYSW__ZNAK_TRZY			0x30
#define WYSW_ZNAK_CZTERY		0x19
#define WYSW_ZNAK_PIEC			0x12
#define WYSW_ZNAK_SZESC			0x02
#define WYSW_ZNAK_SIEDEM		0x78
#define WYSW_ZNAK_OSIEM			0x00
#define WYSW_ZNAK_DZIEWIEC		0x10
#define WYSW_ZNAK_WYL			0x7F


class Wyswietlacz
{
public:
	Wyswietlacz();
	void Odswiez();
	void Wypisz(uint16_t liczba);
private:
	struct Wysw
	{
		volatile uint8_t *ddr;
		volatile uint8_t *port;
		uint8_t pin;
	};
	uint8_t nr_wysw;
	Wysw tab[4];
	uint8_t tab_cyfry[11];
	uint8_t cyfra[4];
};

#endif /* WYSWIETLACZ_H_ */
