/*
 * stos.cpp
 *
 *  Created on: 26 maj 2017
 *      Author: tomek
 */
#include "enc28j60.h"

static uint8_t adres_MAC[6] = {0x54,0x55,0x58,0x11,0x00,0x29};
static uint8_t adres_IP[4]  = {192,168,1,170};
/*
void Stos_Init(uint8_t *moj_adres_MAC, uint8_t *moj_adres_IP, uint16_t port)
{
	enc28j60_Init(moj_adres_MAC);
	enc28j60_ZapiszPhy(PHLCON, 0x476);
	uint8_t i = 0;
//	wwwport_h=(port>>8)&0xff;
//	wwwport_l=(port&0xff);
	while(i < 6)
	{
		adres_MAC[i] = moj_adres_MAC[i];
		i++;
	}
	i = 0;
	while(i < 4)
	{
		adres_IP[i] = moj_adres_IP[i];
		i++;
	}
}
*/
void LAN_Start()
{
//	Stos_Init(adres_MAC, adres_IP, 502);
}



