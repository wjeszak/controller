/*
 * typ_maszyny.h
 *
 *  Created on: 18 cze 2017
 *      Author: tomek
 */

#ifndef TYP_MASZYNY_H_
#define TYP_MASZYNY_H_

#include "maszyna.h"
#include "dynabox.h"
#include "lockerbox.h"

Maszyna * WybierzTypMaszyny(TypMaszyny typ)
{
	Maszyna *wsk = NULL;
	switch (typ)
	{
	case TLockerbox:
		wsk = &lockerbox;
		break;
	case TDynabox:
		wsk = &dynabox;
		break;
	default:
		wsk = NULL;
	}
	return wsk;
}



#endif /* TYP_MASZYNY_H_ */
