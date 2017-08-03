/*
 * encoder.h
 *
 *  Created on: 3 sie 2017
 *      Author: tomek
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include <avr/io.h>

#define HALF_STEP 						1 						// 0 - fullstep, 1 - halfstep
#define USE_ENC_SWITCH 					0						// 0 - without switch, 1 -  with switch
#define USE_INT_IRQ 					0						// 0 - polling, 1 - interrrupts

#if USE_INT_IRQ == 1

#define ENC_INT							-1

#if ENC_INT == 2
#define PCINT_IRQ_VECT 					PCINT2_vect
#define PCMSK_REG 						PCMSK2
#define PCINT_A							PCINT18
#define PCINT_B 						PCINT19
#endif

#endif

#define ENCODER_AB_PORT 				PORTC
#define ENCODER_AB_PIN 					PINC
#define ENCODER_A 						(1 << PC4)
#define ENCODER_B 						(1 << PC5)

#define ENCODER_SW_DIR 					DDRD
#define ENCODER_SW_PORT					PORTD
#define ENCODER_SW_PIN 					PIND
#define ENCODER_SW 						(1 << PD2)

#ifdef GIMSK
#define GICR 							GIMSK
#endif

#define ENCODER_SW_ON 					(ENCODER_SW_PIN & ENCODER_SW)

#define ENCODER_A_HI 					(ENCODER_AB_PIN & ENCODER_A)
#define ENCODER_B_HI 					(ENCODER_AB_PIN & ENCODER_B)

#define ENCODER_LEFT 					0x10
#define ENCODER_RIGHT 					0x20

class Encoder
{
public:
	Encoder();
	void Process();
	void Poll();
};

extern volatile uint8_t enco_dir;
//void encoder_proc();
int get_encoder();
void set_encoder(int val);
//void ENCODER_EVENT();

extern Encoder encoder;

#endif /* ENCODER_H_ */
