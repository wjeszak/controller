/*
 * wyswietlacz.cpp
 *
 *  Created on: 15 cze 2017
 *      Author: tomek
 */
#include "wyswietlacz.h"
Wysw tab[] =
{
	{&WYSW1_DDR, &WYSW1_PORT, WYSW1_PIN},
	{&WYSW2_DDR, &WYSW2_PORT, WYSW2_PIN},
	{&WYSW3_DDR, &WYSW3_PORT, WYSW3_PIN},
	{&WYSW4_DDR, &WYSW4_PORT, WYSW4_PIN}
};

uint8_t tab_cyfry[11] = {WYSW_ZNAK_ZERO, WYSW_ZNAK_JEDEN, WYSW_ZNAK_DWA,
		WYSW__ZNAK_TRZY, WYSW_ZNAK_CZTERY, WYSW_ZNAK_PIEC, WYSW_ZNAK_SZESC,
		WYSW_ZNAK_SIEDEM, WYSW_ZNAK_OSIEM, WYSW_ZNAK_DZIEWIEC, WYSW_ZNAK_WYL};

uint8_t cyfra[4] = {tab_cyfry[1], tab_cyfry[2], tab_cyfry[3], tab_cyfry[4]};

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

void IntToLed(uint16_t liczba)
{
	cyfra[0] = tab_cyfry[liczba / 1000]; 			// tys
	cyfra[1] = tab_cyfry[(liczba / 100) % 10]; 		// setki
	cyfra[2] = tab_cyfry[(liczba % 100) / 10]; 		// dzies
	cyfra[3] = tab_cyfry[liczba % 10]; 				// jedn

}
uint8_t volatile t_flaga, t_flaga_licz = 0;
uint8_t volatile licznik = 0;
