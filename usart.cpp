/*
 * usart.cpp
 *
 *  Created on: 26 maj 2017
 *      Author: tomek
 */

#include "../usart.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdarg.h>
#include <stdlib.h>

#define USART_KIERUNEK_DDR 		DDRC
#define USART_KIERUNEK_PORT 	PORTC
#define USART_KIERUNEK	 		2

#define USART_KIERUNEK_NAD 	USART_KIERUNEK_PORT |=  (1 << USART_KIERUNEK)
#define USART_KIERUNEK_ODB 	USART_KIERUNEK_PORT &= ~(1 << USART_KIERUNEK)
// halt motor external event
char usart_buf_nad[USART_ROZMIAR_BUFORA];
uint8_t	poz_buf, index, zajety, flaga = 0;

Usart::Usart() :
	StateMachine(ST_MAX_STATES),
	m_currentSpeed(0)
{
}

void Usart::OdebranoZnak(void)
{
    // given the Halt event, transition to a new state based upon
    // the current state of the state machine
    BEGIN_TRANSITION_MAP                      // - Current State -
        TRANSITION_MAP_ENTRY (ST_ODEBRANY_ZNAK)  // ST_Idle
        TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)  // ST_OdebranyZnak
    END_TRANSITION_MAP(NULL)
}
/*
// set motor speed external event
void Usart::SetSpeed(UsartData* pData)
{
    BEGIN_TRANSITION_MAP                      // - Current State -
        TRANSITION_MAP_ENTRY (ST_START)       // ST_Idle
        TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)  // ST_Stop
        TRANSITION_MAP_ENTRY (ST_CHANGE_SPEED)// ST_Start
        TRANSITION_MAP_ENTRY (ST_CHANGE_SPEED)// ST_ChangeSpeed
    END_TRANSITION_MAP(pData)
}
*/
// state machine sits here when motor is not running
void Usart::ST_Idle(const NoEventData*)
{
	WyslijRamke("STAN IDLE");
	//cout << "Motor::ST_Idle" << endl;
}
/*
// stop the motor
void Usart::ST_Stop(EventData* pData)
{
	//cout << "Motor::ST_Stop" << endl;

    // perform the stop motor processing here
    // transition to ST_Idle via an internal event
    InternalEvent(ST_IDLE);
}

// start the motor going
void Usart::ST_Start(MotorData* pData)
{
	//cout << "Motor::ST_Start" << endl;
    // set initial motor speed processing here
}
*/
// changes the motor speed once the motor is moving
//void Usart::ST_ChangeSpeed(const UsartData* pData)
//{
	//cout << "Motor::ST_ChangeSpeed" << endl;
    // perform the change motor speed to pData->speed here
//}
//Usart us(9600);
//UsartDane *wUsartDane;
//UsartDane dane;

//void (*WskPustyBufor)() = PustyBufor;

void Kom_Init(uint16_t Predkosc)
{
	uint8_t ubrr = F_CPU / 16 / Predkosc - 1;
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;

	UCSR0B = (1 << RXEN0) | (1 << RXCIE0) | (1 << TXEN0) | (1 << TXCIE0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

	USART_KIERUNEK_DDR |= (1 << USART_KIERUNEK);
	USART_KIERUNEK_ODB;

	//WskPustyBufor = PustyBufor;
	//wUsartDane = &dane;
}

void PustyBufor()
{
	if(poz_buf > 0)
	{
		UDR0 = usart_buf_nad[index++];
		poz_buf--;
	}
	else
	{
		UCSR0B &= ~(1 << UDRIE0);
		index = 0;
	}
}

void KoniecNadawania()
{
	USART_KIERUNEK_ODB;
	zajety = 0;
}



void WyslijRamke(const char *ramka)
{
	while(zajety);
	zajety = 1;
	while(*ramka)
	{
		usart_buf_nad[poz_buf++] = *ramka++;
	}

	USART_KIERUNEK_NAD;
	UCSR0B |= (1 << UDRIE0);
}

void USART_WyslijLiczbe(uint16_t liczba, uint16_t podstawa)
{
	char buf[10];
	itoa(liczba, buf, podstawa);
	//USART_WyslijRamke(buf);
}

void USART_Debug(const char *txt, uint16_t liczba, uint16_t podstawa)
{
	//USART_WyslijRamke(txt);
	USART_WyslijLiczbe(liczba, podstawa);
}

/*
void USART_WyslijString(const char *buf)
{
	while(*buf)
	{
		USART_WyslijZnak(*buf++);
	}
}
*/
/*
void USART_DodajDoBufora(const char znak)
{
	usart_buf_nad[poz_buf++] = znak;
	if(poz_buf == 16)
	{
		USART_KIERUNEK_NAD;
		UCSR0B |= (1 << UDRIE0);
	}
}
*/

// --------- Debugowanie
// http://mckmragowo.pl/mck/pliki/programming/clib/?f=va_start


ISR(USART0_RX_vect)
{

	char znak = UDR0;
//	us.OdebranoZnak(wUsartDane);
}

ISR(USART0_UDRE_vect)
{
	PustyBufor();
}

ISR(USART0_TX_vect)
{
	KoniecNadawania();
}

/*
void USART_WyslijZnak(uint8_t znak)
{
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = znak;
}
*/
