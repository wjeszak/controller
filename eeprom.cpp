/*
 * eeprom.c
 *
 *  Created on: 3 cze 2017
 *      Author: tomek
 */

#include <avr/eeprom.h>
#include "eeprom.h"
#include "machine_type.h"
#include "timer.h"

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

Function EEMEM eem_functions[MAX_FUNCTIONS] =
{
	{0xFF, NULL},	// 1
	{0xFF, NULL},	// 2
	{0xFF, NULL},	// 3
	{0xFF, NULL},	// 4
	{0xFF, NULL},	// 5
	{0xFF, NULL},	// 6
	{0xFF, NULL},	// 7
	{0xFF, NULL},	// 8
	{0xFF, NULL},	// 9
	{0xFF, NULL}, 	// 10
	{0xFF, NULL},	// 11
	{0xFF, NULL},	// 12
	{0xFF, NULL},	// 13
	{0xFF, NULL},	// 14
	{0xFF, NULL},	// 15
	{0xFF, NULL},	// 16
	{0xFF, NULL},	// 17
	{0xFF, NULL},	// 18
	{0xFF, NULL},	// 19
	{0xFF, NULL},	// 20
	{0xFF, NULL},	// 21
	{0xFF, NULL},	// 22
	{0xFF, NULL},	// 23
	{0xFF, NULL},	// 24
	{0xFF, NULL},	// 25
	{0xFF, NULL},	// 26
	{0xFF, NULL},	// 27
	{0x00, NULL},	// 28
	{0xFF, NULL},	// 29
	{0xFF, NULL},	// 30
};

Function functions[MAX_FUNCTIONS];

// default values
ConfigurationParams EEMEM eem_cfg =
{
	{MAC_ADDR1, MAC_ADDR2, MAC_ADDR3, MAC_ADDR4, MAC_ADDR5, MAC_ADDR6},
	{IP_ADDR1, IP_ADDR2, IP_ADDR3, IP_ADDR4}
};

ConfigurationParams cfg;

Eeprom::Eeprom()
{

}

void Eeprom::Read()
{
	eeprom_read_block(&cfg, &eem_cfg, sizeof(ConfigurationParams));
	eeprom_read_block(&functions, &eem_functions, sizeof(functions));
}

void Eeprom::Write()
{
	eeprom_update_block(&eem_cfg, &cfg, sizeof(ConfigurationParams));
}
