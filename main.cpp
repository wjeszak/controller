/*
 * main.cpp
 *
 *  Created on: 25 maj 2017
 *      Author: tomek
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "lockerbox.h"
#include "dynabox.h"
#include "display.h"
#include "timer.h"
#include "usart.h"
#include "enc28j60.h"
#include "machine_type.h"
#include "motor.h"
#include "stack.h"
#include "eeprom.h"
#include "modbus_rtu.h"
#include "modbus_tcp.h"

Lockerbox lockerbox;
Dynabox dynabox;
Eeprom eprom;
Timer timer(T0_PS_1024, 17);
Motor motor;
Display display;
Stack stack;
StackData stack_data;
Usart usart;
UsartData usart_data;
ModbusRTU modbus_rtu;
ModbusTcp modbus_tcp;

int main()
{
	_delay_ms(1000);
	timer.Assign(0, 1, DisplayRefresh);
	timer.Assign(2, 500, ModbusPoll);
	sei();
	Machine *m = GetTypeOfMachine(TLockerbox);
	uint16_t kto = m->StartupTest();
	display.Write(kto);
	while(1)
	{
	//	stack.StackPoll();
	}

}
