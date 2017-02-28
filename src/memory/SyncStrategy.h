/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * storage/SyncStrategy.h
 * ----------------------------
 * SyncStrategy declaration.
 * =============================
 */

// Header Protection
#ifndef STORAGE_SYNCSTRATEGY_H
#define STORAGE_SYNCSTRATEGY_H

// Header files
#include "../common.h"

/**
* Class SyncStrategy
*
* Handles strategy of cache syncronization.
*/
class SyncStrategy
{
public:
	enum Type
	{
		TimeInterval, WriteInterval, Manual
	};

	// Constructor
	SyncStrategy(Type type, int arg = 0);

	// Check whether cache needs syncronizing.
	bool needSync();
private:
	// Type
	Type type;

	// Argument
	int arg;

	// Counter
	int count;

	// Clock time
	clock_t clockTime;
};
// End of Class SyncStrategy

#endif
// End of Header Protection
