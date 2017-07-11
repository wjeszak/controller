/*
 * main.cpp
 *
 * Created on: 25 maj 2017
 * Author: tomek
 * Add compiler option: -std=c++11 (extended constructor list, display):
 * Project's properties->C/C++ Build->Settings->AVR C++ Compiler->Miscellaneous->Other flags
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

UsartData usart_data;
StackData stack_data;
MotorData motor_data;
Machine *m;

int main()
{
	SystemInit();
	m = GetTypeOfMachine(TDynabox);
	display.Write(4764);
	while(1)
	{
		stack.Poll();
	}
}
