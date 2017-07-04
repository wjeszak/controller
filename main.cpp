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
Motor motor;
Timer0 timer0;
Timer1 timer1;
Timer2 timer2(T2_PS_64, 17);

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
	Irq_Init();
	timer2.Assign(0, 1, DisplayRefresh);
	timer2.Assign(4, 1, EncoderStatus);
	sei();
	// wskaznik do typu maszyny
	m = GetTypeOfMachine(TDynabox);
	//uint16_t kto = m->StartupTest();

	uint16_t param = 0;
	display.Write(param);
	while(1)
	{
	/*
		//EncoderStatus();
		if (encoder.GetStatus() == 1)
		{
			param++;
			if(param > 1000) param = 0;
			display.Write(param);
			encoder.ResetStatus();
		}
		if (encoder.GetStatus() == 2)
		{
			param--;
			if((param < 0) || (param > 1000)) param = 0;
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
		*/
		stack.StackPoll();
	}

}
