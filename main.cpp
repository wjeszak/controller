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
Eeprom eeprom;
UsartData usart_data;
Usart usart;
ModbusRTU modbus_rtu;
ModbusTCP modbus_tcp;
MotorData motor_data;
Motor motor;
Encoder encoder;
ButtonData button_data;
Button button_enter_config(&BUTTON_ENTER_CONFIG_DDR, &BUTTON_ENTER_CONFIG_PORT, &BUTTON_ENTER_CONFIG_PIN, BUTTON_ENTER_CONFIG_PIN_NUMBER, 20, &Config::EV_EnterConfig);
Button button_encoder_sw(&BUTTON_ENCODER_SW_DDR, &BUTTON_ENCODER_SW_PORT, &BUTTON_ENCODER_SW_PIN, BUTTON_ENCODER_SW_PIN_NUMBER, 1, &Config::EV_EncoderClick);
Lockerbox lockerbox;
Dynabox dynabox;
ConfigData config_data;
Config config;

StackData stack_data;
Stack stack;

Machine *m;

int main()
{
	while(1)
	{
		stack.Poll();
	}
}
