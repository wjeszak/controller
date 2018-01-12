/*
 * lockerbox_states_events.cpp
 *
 *  Created on: 12 sty 2018
 *      Author: wjeszak
 */

#include "lockerbox.h"

// ---------------------------------- States ----------------------------------
void Lockerbox::ST0_TestingElm(LockerboxData* pdata)
{

}

void Lockerbox::ST1_Ready(LockerboxData* pdata)
{
	//comm.EV_Send(current_address, , true);
}

void Lockerbox::ST2_NotReady(LockerboxData* pdata)
{

}
