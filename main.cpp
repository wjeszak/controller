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

#include "boot.h"
#include "fault.h"
#include "timer.h"
#include "display.h"
#include "button.h"
#include "comm.h"
#include "config.h"
#include "encoder.h"
#include "dynabox.h"
#include "lockerbox.h"
#include "encoder.h"
#include "usart.h"
#include "modbus_tcp.h"
#include "motor.h"
#include "tcp.h"
#include "stack.h"
#include "queue.h"
#include "flags.h"

Machine* m;
MachineData* d;
Fault fault;
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
LockerboxData lockerbox_data;
Lockerbox lockerbox;
ConfigData config_data;
Config config;
UsartData usart_data;
Usart usart;
TcpData tcp_data;
Tcp tcp;
Comm comm;
ModbusTCP mb;
Stack s;
Queue q;
Flags f;

int main()
{
	Boot();
	while(1)
	{
		m->Maintenance();
		tcp.Poll();
	}
}
