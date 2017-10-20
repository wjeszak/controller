/*
 * dynabox_commands.cpp
 *
 *  Created on: 4 paŸ 2017
 *      Author: tomek
 */

#include "dynabox.h"
#include "timer.h"
#include "comm_prot.h"
#include "modbus_tcp.h"
/*

void Dynabox::ParseCheckLed(uint8_t* frame)
{
	if(frame[0] == current_address + LED_ADDRESS_OFFSET)
	{
		if(current_address == last_address)
		{
			SLAVES_POLL_STOP;
			EV_TestedLed(NULL);
		}
		else
			current_address++;
	}
}
*/
void Dynabox::CommandCheckElectromagnet()
{
	if(current_address == last_address + 1)
	{
		SLAVES_POLL_STOP;
		EV_TestedElm(NULL);
	}
	else
	{
		SLAVES_POLL_TIMEOUT_SET;
//		comm.Prepare(current_address, current_command);
		comm.Prepare(current_address + LED_ADDRESS_OFFSET, COMM_GREEN_ON_FOR_TIME);
	}
}

void Dynabox::ParseCheckElectromagnet(uint8_t* frame)
{
	if(frame[0] == current_address)
	{
		if(frame[1] == COMM_F05_ELECTROMAGNET)
		{
//			m->SetFault(F05_ELECTROMAGNET);
			mb.UpdateHoldingRegister(current_address + 1, F05_ELECTROMAGNET << 8);
		}
		current_address++;
	}
}

void Dynabox::CommandGetSetState()
{
	if(current_address == last_address + 1)
	{
//		SetCurrentCommand(COMM_SHOW_STATUS_ON_LED);
		current_address = first_address;
		return;
	}

	SLAVES_POLL_TIMEOUT_SET;
	uint8_t position = mb.GetHoldingRegister(LOCATIONS_NUMBER + current_address);
	if(position > 0)
	{
		comm.Prepare(current_address, 0xC0 + position);
	}
	else
	{
		comm.Prepare(current_address, 0x80);
	}
}

void Dynabox::ParseGetSetState(uint8_t* frame)
{
	if(frame[0] == current_address)
	{
		if(frame[1] == 0xF0)
		{
//			m->SetFault(F03_OPTICAL_SWITCHES);
			mb.UpdateHoldingRegister(current_address + 1, F03_OPTICAL_SWITCHES << 8);
		}
		else
		{
			mb.UpdateHoldingRegister(current_address + 1, frame[1]);
		}
		current_address++;
	}
}

void Dynabox::CommandShowStatusOnLed()
{
	// clear command
	static bool need_send_to_led = false;
	if(led_same_for_all == 0xFF)
	{
		uint16_t status = mb.GetHoldingRegister(current_address + 1);
//		if((status >> 8) != last_fault[current_address])
//		{
//			uint8_t command = faults_to_led_map[status >> 8];
//			comm.Prepare(current_address + LED_ADDRESS_OFFSET, command + 0x80);
//			last_fault[current_address] = status >> 8;
//			need_send_to_led = true;
//		}

		if((status & 0xFF) == 0x80)
		{
			comm.Prepare(current_address + LED_ADDRESS_OFFSET, 0x01 + 0x80);
			need_send_to_led = true;
		}

		if((status & 0xFF) == mb.GetHoldingRegister(LOCATIONS_NUMBER + current_address + 1))
		{
			comm.Prepare(current_address + LED_ADDRESS_OFFSET, 0x00 + 0x80);
			need_send_to_led = true;
		}
	}
	else
	{
		comm.Prepare(current_address + LED_ADDRESS_OFFSET, led_same_for_all + 0x80);
		need_send_to_led = true;
	}
	mb.UpdateHoldingRegister(LOCATIONS_NUMBER + current_address + 1, 0);
	if(current_address == last_address)
	{
		if(need_send_to_led)
		{
			comm.LedTrigger();
			need_send_to_led = false;
		}
		led_same_for_all = 0xFF;
//		SetCurrentCommand(COMM_CHECK_TRANSOPTORS_GET_SET_STATUS);
	}
	else
		current_address++;
}
