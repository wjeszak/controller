/*
 * timery.h
 *
 *  Created on: 15 cze 2017
 *      Author: tomek
 */

#ifndef TIMER_H_
#define TIMER_H_
/*#if (F_CPU == 1843200) | (F_CPU == 3686400) | (F_CPU == 4915200) | (F_CPU == 7372800) | \
	(F_CPU == 9216000) | (F_CPU == 11059200) | (F_CPU == 14745600) | (F_CPU == 18432000) | (F_CPU == 22118400)
#define TIMER_OCR_WART =
#else
#error "Nieobslugiwana czestotliwosc rezonatora kwarcowego"
#endif
*/
//extern volatile uint8_t
extern volatile uint8_t t_flaga, t_licznik, t_flaga_dluga;
extern void Timer_Init();
#endif /* TIMER_H_ */
