/*
 * display.cpp
 *
 *  Created on: 15 cze 2017
 *      Author: tomek
 */

#include <avr/pgmspace.h>
#include "display.h"

Display::Display() : digits {DISP_CHAR_0, DISP_CHAR_1, DISP_CHAR_2, DISP_CHAR_3, DISP_CHAR_4,
	DISP_CHAR_5, DISP_CHAR_6, DISP_CHAR_7, DISP_CHAR_8, DISP_CHAR_9, DISP_CHAR_ALL_OFF,
	DISP_CHAR_L, DISP_CHAR_F, DISP_CHAR_P, DISP_CHAR_b, DISP_CHAR_MINUS}
{
	disp_tab[0].ddr =  &DISP1_DDR;
	disp_tab[0].port = &DISP1_PORT;
	disp_tab[0].pin =   (1 << DISP1_PIN);
	disp_tab[1].ddr =  &DISP2_DDR;
	disp_tab[1].port = &DISP2_PORT;
	disp_tab[1].pin =   (1 << DISP2_PIN);
	disp_tab[2].ddr =  &DISP3_DDR;
	disp_tab[2].port = &DISP3_PORT;
	disp_tab[2].pin =   (1 << DISP3_PIN);
	disp_tab[3].ddr =  &DISP4_DDR;
	disp_tab[3].port = &DISP4_PORT;
	disp_tab[3].pin =   (1 << DISP4_PIN);

	for(uint8_t i = 0; i < 4; i++)
	{
		*(disp_tab[i].ddr)  |= disp_tab[i].pin;		// outputs
		*(disp_tab[i].port) |= disp_tab[i].pin; 	// off
	}

	DISP_SEGM_DDR = 0b01111111;
	disp_number = 0;
	// Startup values
	//value[0] = digits[1];
	//value[1] = digits[2];
	//value[2] = digits[3];
	//value[3] = digits[4];
}

void Display::Refresh()
{
	DISP_SEGM_PORT = value[disp_number];									// load to port
	*(disp_tab[disp_number].port) &= ~disp_tab[disp_number].pin;			// on
	if(disp_number == 0)
		*(disp_tab[3].port) |= disp_tab[3].pin;								// off 4
	else
		*(disp_tab[disp_number-1].port) |= disp_tab[disp_number-1].pin;		// off previous
	disp_number++;
	if(disp_number == 4) disp_number = 0;
}


void Display::Write(InfoType type, uint8_t val)
{
	switch(type)
	{
	case State:
		value[0] = digits[11];
		value[1] = digits[10];
		break;
	case Fault:
		value[0] = digits[12];
		value[1] = digits[10];
		break;
	case Parameter:
		value[0] = digits[13];
		value[1] = digits[10];
		break;
	case ParameterNotSupported:
		value[0] = digits[13];
		value[1] = digits[15];
		break;
	case ParameterValue:
		value[0] = digits[14];
		value[1] = digits[10];
		break;
	}
	value[2] = digits[(val % 100) / 10]; 	// dozens
	value[3] = digits[val % 10]; 			// units
}

void Display::Write(uint16_t val)
{
	value[0] = digits[val / 1000]; 			// thousands
	value[1] = digits[(val / 100) % 10]; 	// hundreds
	value[2] = digits[(val % 100) / 10]; 	// dozens
	value[3] = digits[val % 10]; 			// units
}
