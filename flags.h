/*
 * flags.h
 *
 *  Created on: 8 sty 2018
 *      Author: wjeszak
 */

#ifndef FLAGS_H_
#define FLAGS_H_

#include <inttypes.h>

enum FlagsType { EncoderIrq, NeedFaultsClear };

class Flags
{
public:
	Flags();
	void Set(FlagsType f);
	void Clear(FlagsType f);
	bool Get(FlagsType f);
private:
	uint8_t flags;
};

extern Flags f;

#endif /* FLAGS_H_ */
