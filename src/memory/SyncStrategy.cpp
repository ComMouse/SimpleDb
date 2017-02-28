/**
* Programming II Project
* SE106 - 2015 Spring
*
* =============================
* storage/SyncStrategy.cpp
* ----------------------------
* SyncStrategy implementation.
* =============================
*/

// Header files
#include "SyncStrategy.h"

SyncStrategy::SyncStrategy(Type type, int arg)
	:type(type), arg(arg), count(0), clockTime(clock())
{
}

bool SyncStrategy::needSync()
{
	switch (type)
	{
	case TimeInterval:
		if (clock() - clockTime > arg * CLOCKS_PER_SEC)
		{
			clockTime = clock();
			return true;
		}
		return false;
	case WriteInterval:
		if (++count >= arg)
		{
			count = 0;
			return true;
		}
		return false;
	default:	// Manual
		return false;
	}
}
