/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * storage/CacheManager.h
 * ----------------------------
 * CacheManager declaration.
 * =============================
 */

// Header Protection
#ifndef STORAGE_CACHEMANEGER_H
#define STORAGE_CACHEMANEGER_H

// Header files
#include "../common.h"
#include "../container/Vector.h"
#include "Cache.h"

// Class declarations
class Cache;

/**
 * Static Class CacheManager
 *
 * Manages cache space usage as a heap.
 * The class uses singleton pattern
 */
class CacheManager
{
public:
	// Get the instance
	static CacheManager* instance();

	// Set maximum space
	size_t maxSpace();

	// Get maximum space
	void maxSpace(size_t space);

	// Add cache into manager
	void add(Cache* cache);

	// Remove cache from manager
	void del(Cache* cache);

	// Check cache space
	void checkSpace();
private:
	// Disable constructor
	CacheManager();

	// Max space size
	size_t _maxSpace;

	// Cache list
	Vector<Cache*> cacheList;	// Check cache space

	// Shrink cache space
	void shrinkSpace();
};
// End of Class CacheManager

#endif
// End of Header Protection
