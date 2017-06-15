/*
 * maszyna.h
 *
 *  Created on: 3 cze 2017
 *      Author: tomek
 */

#ifndef MASZYNA_H_
#define MASZYNA_H_
#include <inttypes.h>
#include <stdio.h>			// NULL

class Param
{
public:

};

class Maszyna
{
public:
	Maszyna();
	uint16_t PobierzStan();
	void Zdarzenie(uint16_t NowyStan, Param *Dane = NULL);
	void ZdarzenieWewn(uint16_t NowyStan, Param *Dane);
	const char * PrzedstawSie() {return "Maszyna";}
protected:
	uint16_t StanBiezacy;
private:
	bool JestZdarzenie;
	void ZmienStan(uint16_t nowy_stan);
	void SilnikStanow();
	void (Maszyna::*wsk_f[])(Param *Dane);
	enum Stany {ST_NIEDOZWOLONY = 0xFF};
};

#endif /* MASZYNA_H_ */
