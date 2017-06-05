/*
 * maszyna.cpp
 *
 *  Created on: 3 cze 2017
 *      Author: tomek
 */

#include <assert.h>
#include "maszyna.h"

Maszyna::Maszyna(uint8_t MaxIloscStanow) :
	biezacy_stan(0),
	_MaxIloscStanow(MaxIloscStanow),
	_ByloZdarzenie(false),
	_wParamZdarzenia(NULL)
{
}

void Maszyna::ZdarzenieZewn(uint8_t NowyStan, ParamZdarzenia* wDane)
{
	if(NowyStan == ZD_ZIGNOROWANE)
	{
		if(wDane) delete wDane;
	}
	else
	{
		ZdarzenieWewn(NowyStan, wDane);
		SilnikStanow();
	}
}

void Maszyna::ZdarzenieWewn(uint8_t NowyStan, ParamZdarzenia* wDane)
{
	_wParamZdarzenia = wDane;
	_ByloZdarzenie = true;
	biezacy_stan = NowyStan;
}

void Maszyna::SilnikStanow(void)
{
	ParamZdarzenia* wDaneTmp = NULL;
	while(_ByloZdarzenie)
	{
		wDaneTmp = _wParamZdarzenia;
		_wParamZdarzenia = NULL;
		_ByloZdarzenie = false;

		assert(biezacy_stan < _MaxIloscStanow);

		const StanStruct* wMapaStanow = PobierzMapeStanow();
		(this->*wMapaStanow[biezacy_stan].wStanFunkcja)(wDaneTmp);

		if(wDaneTmp)
		{
			delete wDaneTmp;
			wDaneTmp = NULL;
		}

	}
}
