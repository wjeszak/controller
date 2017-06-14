/*
 * usart.h
 *
 *  Created on: 26 maj 2017
 *      Author: tomek
 */

#ifndef USART_H_
#define USART_H_

#include "maszyna.h"
#define USART_ROZMIAR_BUFORA 50
void Kom_Init(uint16_t Predkosc);
void WyslijRamke(const char *ramka);
/*
__extension__ typedef int __guard __attribute__((mode (__DI__)));

extern "C" int __cxa_guard_acquire(__guard *);
extern "C" void __cxa_guard_release (__guard *);
extern "C" void __cxa_guard_abort (__guard *);

int __cxa_guard_acquire(__guard *g) {return !*(char *)(g);};
void __cxa_guard_release (__guard *g) {*(char *)g = 1;};
void __cxa_guard_abort (__guard *) {};
*/

class Usart : public MaszynaStanow
{
public:
	Usart();
	// ZDARZENIA
	// External events taken by this state machine
	//void SetSpeed(UsartData* data);
	//void Halt();
	void OdebranoZnak();
private:
	uint8_t m_currentSpeed;

	// State enumeration order must match the order of state method entries
	// in the state map.
	enum States
	{
		ST_GOTOWY,
		ST_ODEBRANY_ZNAK,
		ST_MAX_STATES
	};

	// Define the state machine state functions with event data type
	// Stany
	void ST_Gotowy();
	void ST_OdebranyZnak();

	// State map to define state object order. Each state map entry defines a
	// state object.

	private:
	//virtual const StateMapRowEx* GetStateMapEx() { return NULL; }
	virtual const StateMapRow* GetStateMap()
	{
		// tablica z obiektami klasy StateAction zdefiniowanymi za pomoca szablonu wyzej
		static const StateMapRow STATE_MAP[] =
		{
			&Idle,
			&OdebranyZnak
		};
		assert((sizeof(STATE_MAP)/sizeof(StateMapRow)) == ST_MAX_STATES);
		return &STATE_MAP[0];
	}
};
/*
class Usart : public Maszyna
{
public:
	//void (*WskPustyBufor)();
	void PustyBufor();
	void KoniecNadawania();
	void OdebranoZnak(UsartDane*);
	// UWAGA! Tylko do 65535 !!
	void WyslijRamke(const char *ramka);
	Usart(uint16_t Predkosc);			// konstruktor z klasy bazowej, patrz Grebosz 510
private:
	char usart_buf_nad[USART_ROZMIAR_BUFORA];
	volatile uint8_t poz_buf, index, zajety, flaga;
	// Funkcje stanu
	void ST_Wolny();
	void ST_OdebranyZnak(UsartDane*);
public:
	const StanStruct* PobierzMapeStanow()
	{
		static const StanStruct MapaStanow[] =
		{
			//{ reinterpret_cast<StanFunkcja>(&Usart::ST_Wolny) },
			//{ reinterpret_cast<StanFunkcja>(&Usart::ST_OdebranyZnak) }
			//{ reinterpret_cast<StanFunkcja>(NULL) }
				{&Usart::ST_Wolny},
				{&Usart::ST_OdebranyZnak}
		};
	return &MapaStanow[0];
	}
// --------------------------------------------------

	#define START_MAPA_PRZEJSC \
	static const uint8_t PRZEJSCIA[] = \
	{ \

#define MAPA_PRZEJSC(przejscie) \
	przejscie,

#define STOP_MAPA_PRZEJSC(dane) \
	0 }; \
	ZdarzenieZewn(PRZEJSCIA[stan], dane);

	//	START_MAPA_STANOW
//		MAPA_STANOW(ST_Wolny)
//		MAPA_STANOW(ST_OdebranoZnak)
//	STOP_MAPA_STANOW

	enum E_Stany {ST_WOLNY = 0, ST_ODEBRANO_ZNAK, ST_MAX_STAN};
};
*/
//extern void USART_DodajDoBufora(const char znak);

//void USART_WyslijString(const char *buf);
//void USART_WyslijLiczbe(uint16_t liczba, uint16_t podstawa);
//void USART_Debug(const char *txt, uint16_t liczba, uint16_t podstawa);
//void USART_Transmit( unsigned char data );
//void USART_WyslijDebug(const char *buf, ...);

#endif /* USART_H_ */
