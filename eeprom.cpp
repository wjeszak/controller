/*
 * eeprom.c
 *
 *  Created on: 3 cze 2017
 *      Author: tomek
 */

#include <avr/eeprom.h>
#include "eeprom.h"
#include "machine_type.h"

#define MAC_ADDR1 		0x00
#define MAC_ADDR2 		0x20
#define MAC_ADDR3 		0x18
#define MAC_ADDR4 		0xB1
#define MAC_ADDR5		0x15
#define MAC_ADDR6 		0x6F

#define IP_ADDR1 		192
#define IP_ADDR2 		168
#define IP_ADDR3 		1
#define IP_ADDR4 		170

// default values
ConfigurationParams EEMEM eem_cfg =
{
		TLockerbox,
		{MAC_ADDR1, MAC_ADDR2, MAC_ADDR3, MAC_ADDR4, MAC_ADDR5, MAC_ADDR6},
		{IP_ADDR1, IP_ADDR2, IP_ADDR3, IP_ADDR4}
};

ConfigurationParams cfg;

Eeprom::Eeprom()
{
	Read();
}

void Eeprom::Read()
{
	eeprom_read_block(&cfg, &eem_cfg, sizeof(ConfigurationParams));
}

void Eeprom::Write()
{
	eeprom_update_block(&eem_cfg, &cfg, sizeof(ConfigurationParams));
}
