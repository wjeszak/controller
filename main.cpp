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
#include "encoder.h"

Encoder encoder;
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
ModbusTCP modbus_tcp;
Machine *m;
int main()
{
	_delay_ms(1000);
	timer.Assign(0, 1, DisplayRefresh);
	timer.Assign(4, 1, EncoderStatus);
	sei();
	// wskaznik do typu maszyny
	m = NULL;
	//uint16_t kto = m->StartupTest();

	uint16_t param = 0;
	display.Write(param);
	while(1)
	{
		if (encoder.GetStatus() == 1)
		{
			param++;
			if(param > 10) param = 0;
			display.Write(param);
			encoder.ResetStatus();
		}
		if (encoder.GetStatus() == 2)
		{
			param--;
			if((param < 0) || (param > 10)) param = 0;
			display.Write(param);
			encoder.ResetStatus();
		}
		if (encoder.GetStatus() == 3)
		{
			if(param == 1) m = GetTypeOfMachine(TLockerbox);
			if(param == 2) m = GetTypeOfMachine(TDynabox);
			//display.Write(50);
			encoder.ResetStatus();
		}
		stack.StackPoll();
	}

}
