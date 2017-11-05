/*
 * machine.h
 *
 *  Created on: 6 sie 2017
 *      Author: tomek
 */

#ifndef MACHINE_H_
#define MACHINE_H_

#include "comm.h"
#include "state_machine.h"

#define MACHINE_TYPE_DYNABOX 					0
#define MACHINE_TYPE_LOCKERBOX 					1
#define MACHINE_MAX_NUMBER_OF_DOORS				13

class MachineData : public EventData
{
public:
	uint8_t addr;
	uint8_t data;
};

class Machine : public StateMachine
{
public:
	Machine();
	void SetOrderStatus(uint8_t status);
	uint8_t GetOrderStatus();
	void SetIOInfo(uint8_t info);
	void ClearIOInfo(uint8_t info);
	virtual void LoadParameters() {}
	virtual void SaveParameters() {}
	virtual void Init() {}
	virtual void EV_EnterToConfig() {}
	virtual void Scheduler() {}
	virtual void EV_ReplyOK(MachineData* pdata) {}
	virtual void EV_Timeout(MachineData* pdata) {}
	virtual void EV_UserAction(MachineData* pdata) {}
	uint8_t current_address;
private:
	enum States {ST_MAX_STATES};
};

extern Machine* m;
extern MachineData* d;

extern void GetPointerTypeOfMachine(uint8_t type);

#endif /* MACHINE_H_ */
