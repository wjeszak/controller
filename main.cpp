/*
 * main.cpp
 *
 * Created on: 25 maj 2017
 * Author: tomek
 * Add compiler option: -std=c++11 (extended constructor list, display):
 * Project's properties->C/C++ Build->Settings->AVR C++ Compiler->Miscellaneous->Other flags
 * ------------------
 * Zeby byl widoczny status drzwi (Control) w Testbox nalezy ustawic typ maszyny - Registers[45]
 * Ustawic takze Registers[50] zeby nie bylo glupot
 * W Testbox bez sensu jest to ze przy typie maszyny Lockerbox (Registers[145]) wyswietla Opened + 12 stops...
 */

#include "timer.h"
#include "display.h"
#include "button.h"
#include "config.h"
#include "encoder.h"
#include "eeprom.h"
#include "machine_type.h"
#include "lockerbox.h"
#include "dynabox.h"
#include "encoder.h"
#include "usart.h"
#include "modbus_rtu.h"
#include "stack.h"
#include "modbus_tcp.h"
#include "motor.h"

Timer timer(T2_PS_1);
Display display;
Button button;
Encoder encoder;
Eeprom eeprom;
Lockerbox lockerbox;
Dynabox dynabox;
Usart usart;
ModbusRTU modbus_rtu;
Stack stack;
ModbusTCP modbus_tcp;
Motor motor;
ButtonData button_data;
Config config;
ConfigData config_data;
UsartData usart_data;
StackData stack_data;
MotorData motor_data;

Machine *m;

int main()
{
	//mk_encoder_init();
	m = GetPointerTypeOfMachine(TDynabox);
	//m->StartupTest();

	while(1)
	{
		encoder.Poll();
		stack.Poll();
	}
}
