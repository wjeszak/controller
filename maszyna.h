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
	void ZmienStan(uint16_t nowy_stan);
	void Zdarzenie(uint16_t NowyStan, Param *Dane = NULL);
	void ZdarzenieWewn(uint16_t NowyStan, Param *Dane);
	virtual uint16_t PrzedstawSie() { return 10; }
protected:
	uint16_t StanBiezacy;
private:
	bool JestZdarzenie;

	void SilnikStanow();
	void (Maszyna::*wsk_f[])(Param *Dane);
	enum Stany {ST_NIEDOZWOLONY = 0xFF};
};

enum TypMaszyny {TLockerbox, TDynabox};


#endif /* MASZYNA_H_ */
