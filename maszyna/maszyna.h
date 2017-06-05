/*
 * maszyna.h
 *
 *  Created on: 3 cze 2017
 *      Author: tomek
 */

#ifndef MASZYNA_MASZYNA_H_
#define MASZYNA_MASZYNA_H_
#include <inttypes.h>
#include <stdio.h>			// NULL

class ParamZdarzenia
{
public:
	virtual ~ParamZdarzenia() {};
};

struct StanStruct;

class Maszyna			// abstrakcyjna
{
//private:
//	enum TypyMaszyn {LockerBox, Dynabox};
//	uint8_t TypMaszyny;

public:
	Maszyna(uint8_t MaxIloscStanow);
	virtual ~Maszyna() {};

protected:
	enum {ZD_ZIGNOROWANE = 0xFE, ZD_NIEMOZLIWE};
	uint8_t biezacy_stan;
	void ZdarzenieZewn(uint8_t, ParamZdarzenia* = NULL);
	void ZdarzenieWewn(uint8_t, ParamZdarzenia* = NULL);
	virtual const StanStruct* PobierzMapeStanow() = 0;

private:
	const uint8_t _MaxIloscStanow;
	bool _ByloZdarzenie;
	ParamZdarzenia* _wParamZdarzenia;
	void SilnikStanow(void);
};

typedef void (Maszyna::*StanFunkcja)(ParamZdarzenia *);

struct StanStruct
{
	StanFunkcja wStanFunkcja;
};

#define START_MAPA_STANOW \
	public:\
	const StanStruct* PobierzMapeStanow() \
	{ \
		static const StanStruct MapaStanow[] =

#define MAPA_STANOW(stan) \
{ reinterpret_cast<StanFunkcja>(stan) },

#define STOP_MAPA_STANOW \
{ reinterpret_cast<StanFunkcja>(NULL) } \
	}; \
	return &MapaStanow[0]; }
// --------------------------------------------------
#define START_MAPA_PRZEJSC \
	static const uint8_t PRZEJSCIA[] = \
	{ \

#define MAPA_PRZEJSC(przejscie) \
	przejscie,

#define STOP_MAPA_PRZEJSC(dane) \
	0 }; \
	ZdarzenieZewn(PRZEJSCIA[stan], dane);

/*
				class LockerBox : public Maszyna
{
	uint8_t PobierzTypMaszyny();
};

class DynaboxBox : public Maszyna
{
	uint8_t PobierzTypMaszyny();
};
*/


#endif /* MASZYNA_MASZYNA_H_ */
