/**
* Programming II Project
* SE106 - 2015 Spring
*
* =============================
* storage/CacheManager.cpp
* ----------------------------
* CacheManager implementation.
* =============================
*/

// Header files
#include "CacheManager.h"

// Singleton pattern
CacheManager* CacheManager::instance()
{
	static CacheManager instance;
	return &instance;
}

CacheManager::CacheManager()
	:_maxSpace(0)
{
	_maxSpace = MAX_CACHE_SPACE;
}

size_t CacheManager::maxSpace()
{
	return _maxSpace;
}

void CacheManager::maxSpace(size_t space)
{
	_maxSpace = space;
	checkSpace();
}

void CacheManager::add(Cache* cache)
{
	cacheList.pushBack(cache);
}

void CacheManager::del(Cache* cache)
{
	for (int i = 0; i < cacheList.size(); ++i)
		if (cacheList[i] == cache)
			cacheList[i] = nullptr;
}

void CacheManager::checkSpace()
{
	size_t size = 0;
	for (int i = 0; i < cacheList.size(); ++i)
		if (cacheList[i])
			size += cacheList[i]->size();

	if (size > _maxSpace)
	{
		shrinkSpace();
	}
}

void CacheManager::shrinkSpace()
{
	Cache* freeCache = nullptr;
	int minUseCount = INT_MAX;
	for (int i = 0; i < cacheList.size(); ++i)
	{
		if (!cacheList[i]->isWeak() || cacheList[i]->isEmpty())
			continue;

		if (cacheList[i]->_useCount < minUseCount)
		{
			freeCache = cacheList[i];
			minUseCount = freeCache->_useCount;
		}
	}

	if (!freeCache)
		error("No more space for cache");

	freeCache->recycle();
	checkSpace();
}
