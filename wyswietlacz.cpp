/*
 * wyswietlacz.cpp
 *
 *  Created on: 15 cze 2017
 *      Author: tomek
 */

#include "wyswietlacz.h"
Wyswietlacz::Wyswietlacz()
{
	 //tab[0] = {&WYSW1_DDR, &WYSW1_PORT, WYSW1_PIN};
	 //tab[1] = {&WYSW2_DDR, &WYSW2_PORT, WYSW2_PIN};
	 //tab[2] = {&WYSW3_DDR, &WYSW3_PORT, WYSW3_PIN};
	 //tab[3] = {&WYSW4_DDR, &WYSW4_PORT, WYSW4_PIN};
	tab[0].ddr = &WYSW1_DDR;
	tab[0].port = &WYSW1_PORT;
	tab[0].pin = WYSW1_PIN;
	tab[1].ddr = &WYSW2_DDR;
	tab[1].port = &WYSW2_PORT;
	tab[1].pin = WYSW2_PIN;
	tab[2].ddr = &WYSW3_DDR;
	tab[2].port = &WYSW3_PORT;
	tab[2].pin = WYSW3_PIN;
	tab[3].ddr = &WYSW4_DDR;
	tab[3].port = &WYSW4_PORT;
	tab[3].pin = WYSW4_PIN;
	for(uint8_t i = 0; i < 4; i++)
		{
			*(tab[i].ddr) |= tab[i].pin;		// jako wyjscia
			*(tab[i].port) |= tab[i].pin; 		// gaszenie wszystkich
		}
		// jako wyjscia port (za wyjatkiem najstarszego bitu) z segmentami a, b, c...
		WYSW_SEGMENTY_DDR = 0b01111111;
		nr_wysw = 0;
		tab_cyfry[0] = WYSW_ZNAK_ZERO;
		tab_cyfry[1] = WYSW_ZNAK_JEDEN;
		tab_cyfry[2] = WYSW_ZNAK_DWA;
		tab_cyfry[3] = WYSW__ZNAK_TRZY;
		tab_cyfry[4] = WYSW_ZNAK_CZTERY;
		tab_cyfry[5] = WYSW_ZNAK_PIEC;
		tab_cyfry[6] = WYSW_ZNAK_SZESC;
		tab_cyfry[7] = WYSW_ZNAK_SIEDEM;
		tab_cyfry[8] = WYSW_ZNAK_OSIEM;
		tab_cyfry[9] = WYSW_ZNAK_DZIEWIEC;
		tab_cyfry[10] = WYSW_ZNAK_WYL;

		cyfra[0] = tab_cyfry[1];
		cyfra[1] = tab_cyfry[2];
		cyfra[2] = tab_cyfry[3];
		cyfra[3] = tab_cyfry[4];
}

/*
uint8_t tab_cyfry[11] = {WYSW_ZNAK_ZERO, WYSW_ZNAK_JEDEN, WYSW_ZNAK_DWA,
		WYSW__ZNAK_TRZY, WYSW_ZNAK_CZTERY, WYSW_ZNAK_PIEC, WYSW_ZNAK_SZESC,
		WYSW_ZNAK_SIEDEM, WYSW_ZNAK_OSIEM, WYSW_ZNAK_DZIEWIEC, WYSW_ZNAK_WYL};
*/

/*
void Wysw_Init()
{
	for(uint8_t i = 0; i < 4; i++)
		{
			*(tab[i].ddr) |= tab[i].pin;		// jako wyjscia
			*(tab[i].port) |= tab[i].pin; 		// gaszenie wszystkich
		}
		// jako wyjscia port (za wyjatkiem najstarszego bitu) z segmentami a, b, c...
		WYSW_SEGMENTY_DDR = 0b01111111;
}
*/
void Wyswietlacz::Odswiez()
{
	WYSW_SEGMENTY_PORT = cyfra[nr_wysw];				// laduj cyfre do portu
	*(tab[nr_wysw].port) &= ~tab[nr_wysw].pin;			// zaswiec modul
	if(nr_wysw == 0)
		*(tab[3].port) |= tab[3].pin;					// zgas modul
	else
		*(tab[nr_wysw-1].port) |= tab[nr_wysw-1].pin;	// zgas modul
	nr_wysw++;
	if(nr_wysw == 4) nr_wysw = 0;
}

void Wyswietlacz::Wypisz(uint16_t liczba)
{
	cyfra[0] = tab_cyfry[liczba / 1000]; 			// tys
	cyfra[1] = tab_cyfry[(liczba / 100) % 10]; 		// setki
	cyfra[2] = tab_cyfry[(liczba % 100) / 10]; 		// dzies
	cyfra[3] = tab_cyfry[liczba % 10]; 				// jedn

}

