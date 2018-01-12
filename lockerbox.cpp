/*
 * lockerbox.cpp
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#include "lockerbox.h"
#include "timer.h"
#include "display.h"

Lockerbox::Lockerbox()
{
	SetOrderStatus(Ready);
}

void Lockerbox::Init()
{
	//ENTRY_TestingLed(&dynabox_data);
	//SLAVE_POLL_START;
	display.Write(6667);
}

void Lockerbox::Maintenance()
{

}
