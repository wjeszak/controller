/*
 * encoder1.h
 *
 *  Created on: 3 sie 2017
 *      Author: tomek
 */

#ifndef ENCODER1_H_
#define ENCODER1_H_

#include <avr/io.h>

#define HALF_STEP 		1 		// 0 - fullstep
								// 1 - halfstep
#define USE_ENC_SWITCH 	0		// 0 - bez przycisku
								// 1- z przyciskiem
#define USE_INT_IRQ 	0		// 0 - polling
								// 1 - przerwania
#if USE_INT_IRQ == 1

#define ENC_INT			-1 		//

#if ENC_INT == 2

#define PCINT_IRQ_VECT 	PCINT2_vect
#define PCMSK_REG 		PCMSK2
#define PCINT_A			PCINT18
#define PCINT_B 		PCINT19
#endif

#endif


// ----------------- piny enkoder
#define ENC_A 			(1 << PD4)
#define ENC_B 			(1 << PD5)
#define ENC_AB_PIN 		PINC
#define ENC_AB_PORT 	PORTC

// ----------------- przycisk
#define ENC_SW 			(1 << PD4)
#define ENC_SW_PORT		PORTD
#define ENC_SW_PIN 		PIND
#define ENC_SW_DIR 		DDRD

// ****************************************

#ifdef GIMSK
#define GICR 			GIMSK
#endif

#define ENC_SW_ON 		(ENC_SW_PIN & ENC_SW)

#define enc_A_HI 		(ENC_A_PIN & ENC_A)
#define enc_B_HI 		(ENC_B_PIN & ENC_B)

#define ENC_LEFT 		0x10
#define ENC_RIGHT 		0x20

//extern volatile int enco_cnt;
extern volatile uint8_t enco_dir;
void mk_encoder_init();
void encoder_proc();
int get_encoder();
void set_encoder(int val);
void ENCODER_EVENT();

#endif /* ENCODER1_H_ */
