/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * storage/DbRecord.cpp
 * ----------------------------
 * DbFile record implementation.
 * =============================
 */

// Header files
#include "DbRecord.h"

size_t DbRecord::getSize(const char* key, const char* value, Type type)
{
	switch (type)
	{
	case Normal:
		return nextSize + delSize + typeSize + keyLenSize + valueLenSize + strlen(key) + strlen(value);
	case Long:
		return nextSize + delSize + typeSize + keyLenSize + valueLenSize + pageIdSize + recordCursorSize + strlen(key) + strlen(value);
	case LongExtended:
		return nextSize + delSize + typeSize + valueLenSize + pageIdSize + recordCursorSize + strlen(value);
	default:
		error("Invalid record type");
		return 0;
	}
}

DbRecord::DbRecord(DbPage& page, size_t start)
#if MAX_CACHE_PAGE_SWITCH == 1
	:page(*page.getFile(), page.getId()), start(start), size(0)
#else
	:page(&page), start(start), size(0)
#endif
{
}


DbRecord::DbRecord(const DbRecord& rec)
{
	page = rec.page;
	start = rec.start;
	size = rec.size;
}

void DbRecord::init(const char* key, const char* value, Type type)
{
	this->nextOffset(0);
	this->nextDelCursor(0);
	this->type(type);

	switch (type)
	{
	case Normal:
		setKey(key);
		setValue(value);
		break;
	case Long:
		setKey(key);
		setValue(value);
		nextExtendedRecord(0, 0);
		break;
	case LongExtended:
		setValue(value);
		break;
	case Deleted:
		break;
	default:
		error("Unknown record type");
	}
}

size_t DbRecord::nextOffset()
{
	size_t offset = cache()->readInt(start + nextPos, nextSize);
	return offset;
}

void DbRecord::nextOffset(size_t offset)
{
	cache()->writeInt(start + nextPos, offset, nextSize);
	size = 0;	// Clear size cache
	page->syncFlag = false;
}

size_t DbRecord::nextDelCursor()
{
	size_t offset = cache()->readInt(start + delPos, delSize);
	return offset;
}

void DbRecord::nextDelCursor(size_t offset)
{
	cache()->writeInt(start + delPos, offset, delSize);
	page->syncFlag = false;
}

bool DbRecord::isDeleted()
{
	return type() == Deleted;
}

Type DbRecord::type()
{
	return Type(cache()->read(start + typePos));
}

void DbRecord::type(Type type)
{
	cache()->write(start + typePos, type);
	page->syncFlag = false;
}

size_t DbRecord::getCursor() const
{
	return start;
}

size_t DbRecord::getSize()
{
	if (size)
		return size;

	size = nextOffset();
	if (!size)
	{
		size = page->recordSpace() - start;
	}
	return size;
}

void DbRecord::setSize(size_t size)
{
	nextOffset(size);
	page->syncFlag = false;
}

DbPage* DbRecord::getPage()
{
#if MAX_CACHE_PAGE_SWITCH == 1
	return page.ptr();
#else
	return page;
#endif
}

std::string DbRecord::key()
{
	if (type() == LongExtended || type() == Deleted)
		return "";

	return cache()->readString(start + keyPos, keyLenSize);
}

void DbRecord::setKey(const char* key)
{
	if (type() == LongExtended || type() == Deleted)
		return;
	cache()->writeString(start + keyPos, key, keyLenSize);
	page->syncFlag = false;
}

std::string DbRecord::value()
{
	size_t keyLen = 0;
	if (type() != LongExtended)
		keyLen += keyLenSize + cache()->readStringSize(start + keyPos, keyLenSize);

	return cache()->readString(start + keyPos + keyLen, valueLenSize);
}

void DbRecord::setValue(const char* value)
{
	size_t keyLen = 0;
	if (type() != LongExtended)
		keyLen += keyLenSize + cache()->readStringSize(start + keyPos, keyLenSize);

	cache()->writeString(start + keyPos + keyLen, value, valueLenSize);
	page->syncFlag = false;
}

bool DbRecord::canShrink(const char* key, const char* value)
{
	if (type() != Normal)
		return false;

	size_t oldSize = getSize();
	size_t newSize = getSize(key, value, type());
	if (newSize > oldSize)
		return false;
	if (oldSize - newSize < minDeletedSize)
		return false;

	return true;
}

DbRecord DbRecord::next()
{
	return page->nextRecord(*this);
}

DbRecord DbRecord::prev()
{
	return page->prevRecord(*this);
}

bool DbRecord::hasNextExtendedRecord()
{
	if (type() == Normal || type() == Deleted)
		return false;

	size_t pos = start;
	if (type() != LongExtended)
		pos += keyLenSize + cache()->readStringSize(keyPos + pos, keyLenSize);

	pos += keyPos + valueLenSize + cache()->readStringSize(keyPos + pos, valueLenSize);
	int pageId = cache()->readInt(pos, pageIdSize);
	size_t cursor = cache()->readInt(pageIdSize + pos, recordCursorSize);

	return (pageId != 0 || cursor != 0);
}

DbRecord DbRecord::nextExtendedRecord()
{
	if (type() == Normal || type() == Deleted)
		error("This method is only available to long records");

	size_t pos = start;
	if (type() != LongExtended)
		pos += keyLenSize + cache()->readStringSize(keyPos + pos, keyLenSize);

	pos += keyPos + valueLenSize + cache()->readStringSize(keyPos + pos, valueLenSize);
	int pageId = cache()->readInt(pos, pageIdSize);
	size_t cursor = cache()->readInt(pageIdSize + pos, recordCursorSize);

	if (pageId == 0 && cursor == 0)
		throw LastExtendedRecord();
	return page->getFile()->getRecord(pageId, cursor);
}

void DbRecord::nextExtendedRecord(int pageId, size_t cursor)
{
	if (type() == Normal || type() == Deleted)
		error("This method is only available to long records");

	size_t pos = start;
	if (type() != LongExtended)
		pos += keyLenSize + cache()->readStringSize(keyPos + pos, keyLenSize);

	pos += keyPos + valueLenSize + cache()->readStringSize(keyPos + pos, valueLenSize);
	cache()->writeInt(pos, pageId, pageIdSize);
	cache()->writeInt(pageIdSize + pos, cursor, recordCursorSize);
}

DbRecord& DbRecord::operator=(const DbRecord& rec)
{
	page = rec.page;
	start = rec.start;
	size = rec.size;
	return *this;
}

bool DbRecord::operator==(const DbRecord& rec) const
{
	return (page == rec.page) && (start == rec.start);
}

bool DbRecord::operator!=(const DbRecord& rec) const
{
	return !(*this == rec);
}

Cache* DbRecord::cache()
{
	return page->getCache();
}
