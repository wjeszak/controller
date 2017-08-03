/*
 * encoder1.cpp
 *
 *  Created on: 3 sie 2017
 *      Author: tomek
 */

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "encoder1.h"
#include "config.h"

volatile int enco_cnt;
volatile uint8_t enco_dir;
volatile uint8_t enco_flag;

#if HALF_STEP == 1
const uint8_t enc_tab[6][4] PROGMEM =
{
	{0x03, 0x02, 0x01, 0x00}, {0x23, 0x00, 0x01, 0x00},
	{0x13, 0x02, 0x00, 0x00}, {0x03, 0x05, 0x04, 0x00},
	{0x03, 0x03, 0x04, 0x10}, {0x03, 0x05, 0x04, 0x20}
};
#else
const uint8_t enc_tab[7][4] PROGMEM =
{
	{0x00, 0x02, 0x04, 0x00}, {0x03, 0x00, 0x01, 0x10},
	{0x03, 0x02, 0x00, 0x00}, {0x03, 0x02, 0x01, 0x00},
	{0x06, 0x00, 0x04, 0x00}, {0x06, 0x05, 0x00, 0x20},
	{0x06, 0x05, 0x04, 0x00},
};
#endif

int get_encoder()
{
	uint8_t sreg = SREG;
	cli();
	int res = enco_cnt;
	SREG = sreg;
	return res;
}

void set_encoder(int val)
{
	uint8_t sreg = SREG;
	cli();
	enco_cnt = val;
	SREG = sreg;
}

void encoder_proc()
{
	static uint8_t enc_stat;
	register uint8_t pin = ENC_AB_PIN;
	register uint8_t ABstate = ((pin & ENC_B)?2:0) | ((pin & ENC_A)?1:0);
	enc_stat = pgm_read_byte(&enc_tab[enc_stat & 0x0F][ABstate]);
	ABstate = (enc_stat & 0x30);
	if(ABstate)
	{
		enco_dir = ABstate;
		if(ABstate == ENC_RIGHT) enco_cnt++;
		else enco_cnt--;
		enco_flag = 1;
	}
}

void mk_encoder_init()
{
#if USE_ENC_SWITCH == 1
	ENC_SW_PORT |= ENC_SW;
#endif
	ENC_AB_PORT |= ENC_A | ENC_B;
	encoder_proc();
#if USE_INT_IRQ == 1

#if ENC_INT == -1

#ifndef GICR
	EIMSK |= (1 << INT0);
	EICRA |= (1 << ISC00);
#else
	GICR |= (1 << INT0);
	MCUCR |= (1 << ISC00);
#endif

#endif

#if ENC_INT == -1

#ifndef GICR
	EIMSK |= (1 << INT1);
	EICRA |= (1 << ISC10);
#else
	GICR |= (1 << INT1);
	MCUCR |= (1 << ISC10);
#endif

#endif

#if ENC_INT == -2
#ifndef GICR
	PCICR |= (1 << PCIE2);
#else
	GICR |= (1 << PCIE);
#endif
	PCMSK_REG |= (1 << PCINT_B) | (1 << PCINT_A);
#endif

#endif
}

void ENCODER_EVENT()
{
#if USE_INT_IRQ == 0
	encoder_proc();
#endif
	if(enco_flag)
	{
		enco_flag = 0;
		if(enco_dir == ENC_RIGHT) config.EV_EncoderRight(&config_data);
		else config.EV_EncoderLeft(&config_data);
		// tutaj co ma robic
	}
#if USE_ENC_SWITCH == 1
	static uint16_t enc_key_lock;
	if(!enc_key_lock && !(ENC_SW_PIN & ENC_SW))
	{
		enc_key_lock = 65000;
		// tutaj akcja od przycisku
	}
	else if(enc_key_lock && (ENC_SW_PIN & ENC_SW)) enc_key_lock++;
#endif
}

