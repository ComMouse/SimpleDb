/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * storage/DbFile.cpp
 * ----------------------------
 * DbFile implementation.
 * =============================
 */

// Header files
#include "DbFile.h"

DbFile::DbFile(const char* path)
	:file(nullptr), pageCount(0)
{
	file = new File(path);
	pages.resize(file->size() / DbPage::pageSize);
}

DbFile::~DbFile()
{
	sync();
	for (int i = 0; i < pages.size(); ++i)
	{
		if (pages[i])
			delete pages[i];
	}
	delete file;
}

DbRecord DbFile::getRecord(int pageId, size_t cursor)
{
	return getPage(pageId).getRecord(cursor);
}

void DbFile::delRecord(DbRecord& record)
{
	record.getPage()->delRecord(record.getCursor());
}

DbRecord DbFile::newRecord(const char* key, const char* value, RecordType type)
{
	switch (type)
	{
	case Normal:
	{
		size_t size = DbRecord::getSize(key, value, type);
		// Record size too large
		if (size > DbPage::maxFreeSpace)
			return newRecord(key, value, Long);

		return getNewRecord(key, value, type);
	}
	case Long:
	{
		// Check value length
		size_t size = DbRecord::getSize(key, value, type);
		int valueLen = strlen(value);
		int leftValueLen = size - DbPage::maxFreeSpace;
		// Record size too small
		if (leftValueLen <= 0)
			return newRecord(key, value, Normal);

		// Split the first value str
		std::string currentStr(value, valueLen - leftValueLen);

		// Init iterators
		DbRecord firstRecord(getPage(0), 0);
		DbRecord currentRecord(getPage(0), 0);
		DbRecord lastRecord(getPage(0), 0);

		// Create records
		while (leftValueLen > 0)
		{
			lastRecord = currentRecord;
			currentRecord = getNewRecord(key, currentStr.c_str(), type);

			// Update record iterator and next record cursor
			if (type != Long)
			{
				lastRecord.nextExtendedRecord(currentRecord.getPage()->getId(), currentRecord.getCursor());
				leftValueLen -= currentStr.size();
			}
			else
			{
				firstRecord = currentRecord;
				type = LongExtended;
			}

			// Get the size of next value in the new record
			if (leftValueLen > 0)
			{
				size = DbRecord::getSize(key, value + (valueLen - leftValueLen), type);
				if (size > DbPage::maxFreeSpace)
					currentStr = std::string(value + (valueLen - leftValueLen), leftValueLen - (size - DbPage::maxFreeSpace));
				else
					currentStr = std::string(value + (valueLen - leftValueLen), leftValueLen);
			}
		}

		currentRecord.nextExtendedRecord(0, 0);
		return firstRecord;
	}
	default:
		error("Not known record type");
		return DbRecord(getPage(0), 0);		// Avoid warning
	}
}

DbRecord DbFile::getNewRecord(const char* key, const char* value, RecordType type)
{
	size_t size = DbRecord::getSize(key, value, type);

	// Try creating a record in one of pages
	for (int i = pages.size() - 1; i >= 0; --i)
	{
		if (getPage(i).hasSpace(size))
			return getPage(i).newRecord(key, value, type);
	}

	// Create the record in a new page
	return appendPage()->newRecord(key, value, type);
}

void DbFile::shrinkRecord(DbRecord& record, const char* key, const char* value)
{
	record.getPage()->shrinkRecord(record, key, value);
}

bool DbFile::needLongRecord(const char* key, const char* value)
{
	size_t size = DbRecord::getSize(key, value, Long);
	int leftValueLen = size - DbPage::maxFreeSpace;
	return (leftValueLen > 0);
}

File* DbFile::getFile()
{
	return file;
}

DbPage& DbFile::get(size_t pos)
{
	return getPage(pos);
}

DbPage& DbFile::getPage(size_t pos)
{
	return *getPtr(pos);
}

DbPage* DbFile::getPtr(size_t pos)
{
	if (pages[pos])
		return pages[pos];
	checkPage();
	pages[pos] = new DbPage(*this, pos);
	++pageCount;
	return pages[pos];
}

bool DbFile::needSync() const
{
	for (int i = 0; i < pages.size(); ++i)
	{
		if (pages[i] && pages[i]->needSync())
			return true;
	}
	return false;
}

void DbFile::sync()
{
	for (int i = 0; i < pages.size(); ++i)
	{
		if (pages[i])
			pages[i]->sync();
	}
	file->flush();
}

DbPage* DbFile::appendPage()
{
	checkPage();
	DbPage* page = new DbPage(*this, pages.size());
	page->init();
	//page->sync();
	pages.pushBack(page);
	++pageCount;
	return page;
}

void DbFile::checkPage()
{
#if MAX_CACHE_PAGE_SWITCH == 1
	if (pageCount > MAX_CACHE_PAGE)
		shrinkPage();
#endif
}

void DbFile::shrinkPage()
{
	// Delete pages with empty cache
	for (int i = 0; i < pages.size(); ++i)
	{
		if (pages[i] && pages[i]->getCache()->isEmpty())
		{
			delete pages[i];
			--pageCount;

			if (pageCount <= MAX_CACHE_PAGE)
				return;
		}
	}

	// Delete pages with index order
	for (int i = 0; i < pages.size(); ++i)
	{
		if (pages[i])
		{
			delete pages[i];
			pages[i] = nullptr;
			--pageCount;

			if (pageCount <= MAX_CACHE_PAGE)
				return;
		}
	}

	error("No pages found to be shrunk");
}
