/*
 * display.h
 *
 *  Created on: 15 cze 2017
 *      Author: tomek
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <avr/io.h>
#include <inttypes.h>
#include "machine.h"
// Numeracja wg. rozmieszczenia na plytce
#define DISP1_DDR 				DDRD
#define DISP2_DDR 				DDRC
#define DISP3_DDR 				DDRB
#define DISP4_DDR 				DDRB

#define DISP1_PORT	 			PORTD
#define DISP2_PORT 				PORTC
#define DISP3_PORT 				PORTB
#define DISP4_PORT 				PORTB

#define DISP1_PIN				(1 << 4)
#define DISP2_PIN				(1 << 0)
#define DISP3_PIN 				(1 << 3)
#define DISP4_PIN				(1 << 4)

#define DISP_SEGM_DDR 			DDRA
#define DISP_SEGM_PORT 			PORTA

#define DISP_CHAR_0 			0x40
#define DISP_CHAR_1				0x79
#define DISP_CHAR_2				0x24
#define DISP_CHAR_3 			0x30
#define DISP_CHAR_4 			0x19
#define DISP_CHAR_5 			0x12
#define DISP_CHAR_6 			0x02
#define DISP_CHAR_7 			0x78
#define DISP_CHAR_8				0x00
#define DISP_CHAR_9 			0x10
#define DISP_CHAR_ALL_OFF		0x7F

class Display
{
public:
	Display();
	void Refresh();
	void Write(uint16_t value);
private:
	struct Disp
	{
		volatile uint8_t *ddr;
		volatile uint8_t *port;
		uint8_t pin;
	};
	Disp disp_tab[4];
	uint8_t disp_number;
	uint8_t digits[11];
	uint8_t value[4];
};
extern Display display;

#endif /* DISPLAY_H_ */