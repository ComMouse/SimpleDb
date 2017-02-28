/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * storage/Cache.h
 * ----------------------------
 * Cache mapping interface.
 * =============================
 */

// Header Protection
#ifndef STORAGE_CACHE_H
#define STORAGE_CACHE_H

// Header files
#include "../common.h"
#include "CacheManager.h"
#include "SyncStrategy.h"

// Class declarations
class CacheManager;

/**
 * Class Cache
 *
 * Memory cache wrapper.
 */
class Cache
{
public:
	// Callback typedef
	typedef void (*CallBack)(void*);

	// Constructor
	Cache();
	Cache(size_t size);
	Cache(char* data, size_t size);
	Cache(const Cache& cache) = delete;

	// Destructor
	~Cache();

	// Get whether the cache memory can be recycled
	bool isWeak();

	// Set weak switch
	void weak(bool val);

	// Set recycle callback function 
	void callback(CallBack ptr, void* arg = nullptr);

	// Set sync strategy
	void syncStrategy(SyncStrategy strategy);

	// Create cache of the given size
	void create(size_t size);

	// Set cache data
	void set(char* data, size_t size);

	// Get cache data
	char* get();

	// Clear cache
	void clear();

	// Get whether cache is empty
	bool isEmpty();
	
	// Get size of cache
	size_t size();

	// Write a byte
	void write(size_t pos, char byte);

	// Write bytes
	void writeBytes(size_t pos, size_t size, const char* bytes);

	// Write an integer
	void writeInt(size_t pos, int val, size_t size = 4);

	// Writa a long integer
	void writeLong(size_t pos, long long val, size_t size = 8);

	// Write a string
	void writeString(size_t pos, const char* str, size_t lenSize = 0);


	// Read a byte
	char read(size_t pos);

	// Read bytes
	char* readBytes(size_t pos);
	void readBytes(size_t pos, char* buf, size_t size);

	// Read a long integer
	long long readLong(size_t pos, size_t size = 8);

	// Read an integer
	int readInt(size_t pos, size_t size = 4);

	// Read a string
	std::string readString(size_t pos, size_t lenSize = 0);

	// Read the size of the string
	size_t readStringSize(size_t pos, size_t lenSize = 0);

	// Byte Accessor
	char& operator[](size_t pos);

	// Disable copy assignment operator
	Cache& operator=(const Cache& cache) = delete;

	// Friends
	friend CacheManager;
private:
	// Cache data
	char* _data;

	// Cache size
	size_t _size;

	// Counter of using frequency
	int _useCount;

	// Sync strategy
	SyncStrategy _strategy;

	// Recycle switch
	bool _weak;

	// Recycle callback function ptr
	CallBack _callback;

	// Callback argument
	void* _cbArg;

	// Recycle cache memory
	void recycle();

	// Check sync strategy
	void checkSync();
};
// End of Class Cache

#endif
// End of Header Protection
