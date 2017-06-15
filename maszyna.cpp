/*
 * maszyna.cpp
 *
 *  Created on: 3 cze 2017
 *      Author: tomek
 */

#include "maszyna.h"

Maszyna::Maszyna() : StanBiezacy(0)
{
	PobierzStan();
}

void Maszyna::ZmienStan(uint16_t NowyStan)
{
	StanBiezacy = NowyStan;
}

uint16_t Maszyna::PobierzStan()
{
	return StanBiezacy;
}
void Maszyna::Zdarzenie(uint16_t NowyStan, Param *Dane)
{
	JestZdarzenie = true;
	ZmienStan(NowyStan);
	while(JestZdarzenie)
	{
		(this->*wsk_f[StanBiezacy])(Dane);
		JestZdarzenie = false;
	}
}

void Maszyna::ZdarzenieWewn(uint16_t NowyStan, Param *Dane)
{
	ZmienStan(NowyStan);
	(this->*wsk_f[StanBiezacy])(Dane);
}
