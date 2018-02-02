/*
 * machine.h
 *
 *  Created on: 6 sie 2017
 *      Author: tomek
 */

#ifndef MACHINE_H_
#define MACHINE_H_

#include "comm.h"
#include "state_machine_ex.h"

#define MACHINE_TYPE_DYNABOX 					0
#define MACHINE_TYPE_LOCKERBOX 					1
#define MACHINE_MAX_NUMBER_OF_DOORS				60

enum OrderStatus { Ready, GoAck, Processing, EndOfMovement, NotReady = 0xFFFF };
enum IO_InfoType { Moving, MovingDirection, HomingInProgress, HomingDone, ManualOperation,
				   NormalOperation, SwitchDoor };

enum DoorCommand { CheckElm = 0x01, GetStatusBeforeMovement = 0x02, ElmOff = 0x03,
				   ElmOffOn = 0x04, CheckElmGetStatusLockerbox = 0x05,
				   GetStatusLockerbox = 0x06, OpenLockerbox = 0x07, GetStatus = 0x80, SetPosition = 0xC0 };

class MachineData : public EventDataEx
{
public:
	uint8_t addr;
	uint8_t data;
};

class Machine : public StateMachineEx
{
public:
	Machine();
	void SetOrderStatus(OrderStatus status);
	OrderStatus GetOrderStatus();
	void SetIOInfo(IO_InfoType info);
	void ClearIOInfo(IO_InfoType info);
	bool GetIOInfo(IO_InfoType info);
	uint8_t LastAddress();
	virtual void LoadParameters() {}
	virtual void SaveParameters() {}
	virtual void Init() {}
	virtual void Maintenance() {}
	virtual void EV_EnterToConfig() {}
	virtual void StateManager() {}
	virtual void EV_Reply(MachineData* pdata) {}
	virtual void EV_Timeout(MachineData* pdata) {}
	virtual void EV_UserAction(MachineData* pdata) {}
	uint8_t current_command[MACHINE_MAX_NUMBER_OF_DOORS];
	uint8_t current_address;
	uint64_t door_need_open;
private:
	enum States {ST_MAX_STATES};
};

extern Machine* m;
extern MachineData* d;

extern void GetPointerTypeOfMachine(uint8_t type);

#endif /* MACHINE_H_ */
