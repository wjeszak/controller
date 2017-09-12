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
#include "comm_prot.h"
#include "config.h"
#include "encoder.h"
#include "lockerbox.h"
#include "dynabox.h"
#include "encoder.h"
#include "usart.h"
#include "stack.h"
#include "modbus_tcp.h"
#include "motor.h"

Timer timer(T2_PS_1);
Display display;
ButtonData button_data;
Button button_enter_config(&BUTTON_ENTER_CONFIG_DDR, &BUTTON_ENTER_CONFIG_PORT, &BUTTON_ENTER_CONFIG_PIN, BUTTON_ENTER_CONFIG_PIN_NUMBER, 20, &Config::EV_ButtonClick);
Button button_encoder_sw(&BUTTON_ENCODER_SW_DDR, &BUTTON_ENCODER_SW_PORT, &BUTTON_ENCODER_SW_PIN, BUTTON_ENCODER_SW_PIN_NUMBER, 1, &Config::EV_EncoderClick);
Encoder encoder;
MotorData motor_data;
Motor motor;
DynaboxData dynabox_data;
Dynabox dynabox;
Lockerbox lockerbox;
ConfigData config_data;
Config config;
UsartData usart_data;
Usart usart;
StackData stack_data;
Stack stack;
Comm_prot comm;
ModbusTCP modbus_tcp;

Machine *m;

int main()
{
	while(1)
	{
	stack.Poll();
	}
}
