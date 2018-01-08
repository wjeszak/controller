/*
 * flags.cpp
 *
 *  Created on: 8 sty 2018
 *      Author: wjeszak
 */

#include "flags.h"

Flags::Flags()
{
	flags = 0;
}

void Flags::Set(FlagsType f)
{
	flags |= (1 << f);
}

void Flags::Clear(FlagsType f)
{
	flags &= ~(1 << f);
}

bool Flags::Get(FlagsType f)
{
	if(flags & (1 << f))
		return true;
	else
		return false;
}
