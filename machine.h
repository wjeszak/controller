/*
 * machine.h
 *
 *  Created on: 6 sie 2017
 *      Author: tomek
 */

#ifndef MACHINE_H_
#define MACHINE_H_

#include "state_machine.h"
#include "comm_prot.h"

#define MACHINE_TYPE_DYNABOX 					0
#define MACHINE_TYPE_LOCKERBOX 					1
#define MACHINE_MAX_NUMBER_OF_DOORS				13

class MachineData : public EventData
{
public:
};

class Machine : public StateMachine
{
public:
	Machine();
	bool CrcOk(uint8_t* frame);
	bool CurrentAddress();
	bool LastAddress();
	void SetOrderStatus(uint8_t status);
	uint8_t GetOrderStatus();
	void SetIOInfo(uint8_t info);
	void ClearIOInfo(uint8_t info);
	virtual void LoadParameters() {}
	virtual void SaveParameters() {}
	virtual void Init() {}
	virtual void EV_EnterToConfig() {}
	virtual void Scheduler() {}
	virtual void EV_Parse(uint8_t* frame) {}
	virtual void EV_Timeout() {}
	virtual void EV_UserAction(MachineData* pdata) {}
	uint8_t current_address;
private:
	enum States {ST_MAX_STATES};
};

extern Machine* m;
extern MachineData* d;

extern void GetPointerTypeOfMachine(uint8_t type);

#endif /* MACHINE_H_ */
