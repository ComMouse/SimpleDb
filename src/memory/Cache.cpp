/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * storage/Cache.cpp
 * ----------------------------
 * Cache implementation.
 * =============================
 */

// Header files
#include "Cache.h"

Cache::Cache()
	: _data(nullptr), _size(0), _useCount(0),
	_weak(false), _callback(nullptr), _cbArg(nullptr),
	_strategy(SyncStrategy::Manual)
{
#if MAX_CACHE_SPACE_SWITCH == 1
	CacheManager::instance()->add(this);
#endif
}

Cache::Cache(size_t size)
	: _data(nullptr), _size(0), _useCount(0),
	_weak(false), _callback(nullptr), _cbArg(nullptr),
	_strategy(SyncStrategy::Manual)
{
#if MAX_CACHE_SPACE_SWITCH == 1
	CacheManager::instance()->add(this);
#endif
	create(size);
}

Cache::Cache(char* data, size_t size)
	: _data(data), _size(size), _useCount(0),
	_weak(false), _callback(nullptr), _cbArg(nullptr),
	_strategy(SyncStrategy::Manual)
{
#if MAX_CACHE_SPACE_SWITCH == 1
	CacheManager::instance()->add(this);
#endif
}

Cache::~Cache()
{
#if MAX_CACHE_SPACE_SWITCH == 1
	CacheManager::instance()->del(this);
#endif
	clear();
}

bool Cache::isWeak()
{
	return _weak;
}

void Cache::weak(bool val)
{
	_weak = val;
}

void Cache::callback(CallBack ptr, void* arg)
{
	_callback = ptr;
	_cbArg = arg;
}

void Cache::syncStrategy(SyncStrategy strategy)
{
	_strategy = strategy;
}

void Cache::create(size_t size)
{
	if (_data)
		clear();

	if (size == 0)
		return;

	char* space;
	try
	{
		space = new char[size];
	}
	catch (std::bad_alloc&)
	{
		debugInfo("Cache allocation failed", Error);
		throw;
	}

	set(space, size);
}

void Cache::set(char* data, size_t size)
{
	if (_data)
		clear();
	if (size == 0)
		return;

	_data = data;
	_size = size;
	_useCount = 0;

#if MAX_CACHE_SPACE_SWITCH == 1
	// Avoid self being recycled
	bool tmpWeak = _weak;
	_weak = false;
	CacheManager::instance()->checkSpace();
	_weak = tmpWeak;
#endif
}

char* Cache::get()
{
	return _data;
}

void Cache::clear()
{
	if (_data)
		delete[] _data;
	_data = nullptr;
	_size = 0;
	_useCount = 0;
}

bool Cache::isEmpty()
{
	return _size == 0;
}

size_t Cache::size()
{
	return _size;
}

void Cache::write(size_t pos, char byte)
{
	if (pos >= _size)
		error("Invalid position in cache");

	_data[pos] = byte;
	++_useCount;
	checkSync();
}

void Cache::writeBytes(size_t pos, size_t size, const char* bytes)
{
	if (pos >= _size || pos + size > _size)
		error("Invalid position in cache");

	for (size_t i = 0; i < size; ++i)
		_data[pos + i] = bytes[i];
	++_useCount;
	checkSync();
}

void Cache::writeInt(size_t pos, int val, size_t size)
{
	if (pos >= _size || pos + size > _size)
		error("Invalid position in cache");

	for (size_t i = 0; i < size; ++i)
	{
		char ch = (val >> ((size - i - 1) * 8)) & 0xFF;
		_data[pos + i] = (val >> ((size - i - 1) * 8)) & 0xFF;
	}
	++_useCount;
	checkSync();
}

void Cache::writeLong(size_t pos, long long val, size_t size)
{
	if (pos >= _size || pos + size > _size)
		error("Invalid position in cache");

	for (size_t i = 0; i < size; ++i)
	{
		char ch = (val >> ((size - i - 1) * 8)) & 0xFF;
		_data[pos + i] = (val >> ((size - i - 1) * 8)) & 0xFF;
	}
	++_useCount;
	checkSync();
}

void Cache::writeString(size_t pos, const char* str, size_t lenSize)
{
	int len = strlen(str);
	if (pos >= _size || pos + lenSize + len > _size)
		error("Invalid position in cache");

	if (lenSize)
		writeInt(pos, len, lenSize);
	writeBytes(pos + lenSize, len, str);
	checkSync();
}

char Cache::read(size_t pos)
{
	if (pos >= _size)
		error("Invalid position in cache");

	++_useCount;
	return _data[pos];
}

char* Cache::readBytes(size_t pos)
{
	if (pos >= _size)
		error("Invalid position in cache");

	++_useCount;
	return _data + pos;
}

void Cache::readBytes(size_t pos, char* buf, size_t size)
{
	if (pos >= _size || pos + size > _size)
		error("Invalid position in cache");

	++_useCount;
	for (size_t i = pos; i < pos + size; ++i)
	{
		buf[i] = _data[i];
	}
}

long long Cache::readLong(size_t pos, size_t size)
{
	if (pos >= _size || pos + size > _size)
		error("Invalid position in cache");

	++_useCount;
	unsigned long long val = 0;
	for (size_t i = 0; i < size; ++i)
	{
		char ch = static_cast<unsigned char>(_data[pos + i]);
		val = (val << 8) | static_cast<unsigned char>(_data[pos + i]);
	}
	return static_cast<long long>(val);
}

int Cache::readInt(size_t pos, size_t size)
{
	if (pos >= _size || pos + size > _size)
		error("Invalid position in cache");

	++_useCount;
	unsigned int val = 0;
	for (size_t i = 0; i < size; ++i)
	{
		char ch = static_cast<unsigned char>(_data[pos + i]);
		val = (val << 8) | static_cast<unsigned char>(_data[pos + i]);
	}
	return static_cast<int>(val);
}

std::string Cache::readString(size_t pos, size_t lenSize)
{
	if (pos >= _size || pos + lenSize > _size)
		error("Invalid position in cache");

	if (lenSize)
	{
		int len = readInt(pos, lenSize);
		return std::string(_data + pos + lenSize, len);
	}
	else
	{
		return std::string(_data + pos);
	}
}

size_t Cache::readStringSize(size_t pos, size_t lenSize)
{
	if (pos >= _size || pos + lenSize > _size)
		error("Invalid position in cache");

	if (lenSize)
		return readInt(pos, lenSize);
	else
		return strlen(_data + pos);
}

void Cache::recycle()
{
	if (!_data)
		return;

	_callback(_cbArg);
	clear();
}

void Cache::checkSync()
{
	if (!_data)
		return;

	if (_strategy.needSync())
	{
		//debugInfo("Disk sync ok");
		_callback(_cbArg);
	}
}

char& Cache::operator[](size_t pos)
{
	if (pos >= _size)
		error("Invalid position in cache");

	return _data[pos];
}
