/*
 * main.cpp
 *
 * Created on: 25 maj 2017
 * Author: tomek
 *
 */

#include "system.h"
#include "timer.h"
#include "display.h"
#include "encoder.h"
#include "eeprom.h"
#include "machine_type.h"
#include "lockerbox.h"
#include "dynabox.h"
#include "usart.h"
#include "modbus_rtu.h"
#include "stack.h"
#include "modbus_tcp.h"
#include "motor.h"

Timer timer(T2_PS_1);
Display display;
Encoder encoder;
Eeprom eprom;
Lockerbox lockerbox;
Dynabox dynabox;
Usart usart;
ModbusRTU modbus_rtu;
Stack stack;
ModbusTCP modbus_tcp;
Motor motor;

Timer0 timer0;
Timer1 timer1;

UsartData usart_data;
StackData stack_data;
MotorData motor_data;
Machine *m;

int main()
{
	SystemInit();
	m = GetTypeOfMachine(TDynabox);
	while(1)
	{
		stack.StackPoll();
	}
}
