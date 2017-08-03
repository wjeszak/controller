/*
 * encoder.cpp
 *
 *  Created on: 30 cze 2017
 *      Author: tomek
 */

#include "encoder.h"
#include "display.h"
#include "config.h"

Encoder::Encoder()
{
	ENCODER_DDR &= ~((1 << ENCODER_PA) | (1 << ENCODER_PB));
	ENCODER_BUTTON_DDR &= ~(1 << ENCODER_BUTTONP);
	wait = 0;
	debounce_click = 0;
	uint8_t val=0, val_tmp =0;
	ReadGray();
}

uint8_t Encoder::ReadGray()
{
	uint8_t val = 0;
	if(!bit_is_clear(ENCODER_PIN, ENCODER_PA))
	val |= (1 << 1);

	if(!bit_is_clear(ENCODER_PIN, ENCODER_PB))
	val |= (1 << 0);
	return val;
}
/*
void Encoder::Poll()
{
	if(ROTA & (!wait))
		wait = 1;
	if(ROTB & ROTA & (wait))
	{
		config.EV_EncoderLeft(&config_data);
		wait = 2;
	}
	else if(ROTA & (!ROTB) & wait)
	{
		config.EV_EncoderRight(&config_data);
		wait = 2;
	}
	if((!ROTA) & !(ROTB) & (wait == 2))
		wait = 0;

	if(ROTCLICK)
	{
		debounce_click++;
		// debounce 50 ms
		if (ROTCLICK && debounce_click == 10)
		{
			debounce_click = 0;
			config.EV_EncoderClick();
		};
	}
} */
void Encoder::Poll()
{
	val_tmp = ReadGray();
	if(val != val_tmp)
	{
		if( /*(val==2 && val_tmp==3) || */
			   (val==3 && val_tmp==1) ||
			 /*  (val==1 && val_tmp==0) || */
			   (val==0 && val_tmp==2)
			 )
		   {
			config.EV_EncoderRight(&config_data);
		   }
		   else if(/* (val==3 && val_tmp==2) || */
			   (val==2 && val_tmp==0) ||
			/*   (val==0 && val_tmp==1) || */
			   (val==1 && val_tmp==3)
			 )
		   {
			   config.EV_EncoderLeft(&config_data);
		   }

		   val = val_tmp;
	   }
}
