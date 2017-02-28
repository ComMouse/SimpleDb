/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * storage/Page.cpp
 * ----------------------------
 * DbFile page implementation.
 * =============================
 */

// Header files
#include "DbPage.h"

void DbPage::cbSyncCache(void* page)
{
	static_cast<DbPage*>(page)->sync();
}

DbPage::DbPage(DbFile& file, int id)
	:file(&file), id(id), freeList(nullptr), syncFlag(true)
{
	// Init free record list
	freeList = new DbFreeList(*this);
	// Allow recycle
	cache.weak(true);
	// Register cache sync callback
	cache.callback(DbPage::cbSyncCache, this);
	// Register strategy
	cache.syncStrategy(SyncStrategy(SyncStrategy::WriteInterval, 1000));
}

DbPage::~DbPage()
{
	if (!syncFlag)
		sync();
	delete freeList;
}

void DbPage::init()
{
	//initFile();
	initCache(true);
	setRecordCount(0);
	setRecordSpace(0);
	setFreeCursor(0);
	setFreeSpace(pageSize - headerSize);
	//sync();
}

int DbPage::getId() const
{
	return id;
}

void DbPage::initFile()
{
	// Use unique ptr to ensure exception safety
	std::unique_ptr<char> emptyBytes(new char[pageSize]);
	memset(emptyBytes.get(), 0, sizeof(char) * pageSize);
	file->getFile()->writeBytes(id * pageSize, pageSize, emptyBytes.get());
	file->getFile()->flush();
	emptyBytes.release();
}

void DbPage::initCache(bool newPage)
{
	if (!cache.isEmpty())
		return;

	if (cache.size() > pageSize)
	{
		error("");
	}

	if (!newPage)
	{
		cache.set(file->getFile()->readBytes(id * pageSize, pageSize), pageSize);
	}
	else
	{
		char* emptyBytes = new char[pageSize];
		memset(emptyBytes, 0, sizeof(char) * pageSize);
		cache.set(emptyBytes, pageSize);
	}
}

int DbPage::recordCount()
{
	initCache();
	int count = cache.readInt(0, 2);
	return count;
}

size_t DbPage::recordSpace()
{
	initCache();
	size_t space = cache.readInt(2, 2);
	return space;
}

size_t DbPage::freeSpace()
{
	initCache();
	size_t space = cache.readInt(4, 2);
	return space;
}

size_t DbPage::freeCursor()
{
	initCache();
	size_t cursor = cache.readInt(6, 2);
	return cursor;
}

void DbPage::setRecordCount(int val)
{
	initCache();
	cache.writeInt(0, val, 2);
	syncFlag = false;
}

void DbPage::setRecordSpace(size_t space)
{
	initCache();
	cache.writeInt(2, space, 2);
	syncFlag = false;
}

void DbPage::setFreeSpace(size_t space)
{
	initCache();
	cache.writeInt(4, space, 2);
	syncFlag = false;
}

void DbPage::setFreeCursor(size_t cursor)
{
	initCache();
	cache.writeInt(6, cursor, 2);
	syncFlag = false;
}

bool DbPage::hasSpace(const char* key, const char* value)
{
	return hasSpace(DbRecord::getSize(key, value));
}

bool DbPage::hasSpace(size_t size)
{
	if (freeSpace() >= size)
		return true;

	return freeList->hasSpace(size);
}

DbRecord DbPage::getRecord(size_t cursor)
{
	return DbRecord(*this, cursor);
}

DbRecord DbPage::getRecordAt(int index)
{
	DbRecord rec = firstRecord();
	for (int i = 0; i < index; ++i)
	{
		rec = nextRecord(rec);
	}
	return rec;
}

void DbPage::delRecord(size_t cursor)
{
	DbRecord rec = getRecord(cursor);
	if (rec.type() == Long || rec.type() == LongExtended)
	{
		if (rec.hasNextExtendedRecord())
		{
			DbRecord nextRec = rec.nextExtendedRecord();
			file->delRecord(nextRec);
		}
	}
	rec.type(Deleted);
	freeList->addRecord(rec);
}

DbRecord DbPage::newRecord(const char* key, const char* value, RecordType type)
{
	syncFlag = false;
	size_t size = DbRecord::getSize(key, value, type);

	// Create from free space
	if (freeSpace() >= size)
	{
		DbRecord rec(*this, recordStartPos + recordSpace());
		rec.init(key, value, type);
		rec.setSize(size);

		setRecordCount(recordCount() + 1);
		setRecordSpace(recordSpace() + size);
		setFreeSpace(freeSpace() - size);

		return rec;
	}

	// Check free node list
	try
	{
		DbRecord rec = freeList->findRecord(size);
		freeList->shrinkRecord(rec, size);

		// Init record
		rec.init(key, value, type);
		rec.setSize(size);
		return rec;
	}
	catch (ReachLastRecord)
	{
		error("No space for new record");
		return DbRecord(*this, 0);		// Aviod Warning
	}
}

void DbPage::shrinkRecord(DbRecord record, const char* key, const char* value)
{
	if (!record.canShrink(key, value))
	{
		error("Record type or size unsuitable for shrinking");
	}

	size_t oldSize = record.getSize();
	size_t newSize = DbRecord::getSize(key, value, record.type());

	DbRecord freeRec(*this, record.getCursor() + newSize);
	freeRec.init(nullptr, nullptr, Deleted);
	freeRec.setSize(oldSize - newSize);
	delRecord(freeRec.getCursor());

	record.init(key, value, record.type());
	record.setSize(newSize);
}

DbRecord DbPage::firstRecord()
{
	if (!recordCount())
		throw ReachLastRecord();
	size_t pos = recordStartPos;
	DbRecord current(*this, pos);
	while (current.isDeleted())
		current = nextRecord(current);
	return current;
}

DbRecord DbPage::nextRecord(DbRecord & rec)
{
	size_t usedSpace = recordSpace();
	do
	{
		if (rec.getCursor() >= headerSize + usedSpace || rec.nextOffset() == 0)
			throw ReachLastRecord();
		rec = DbRecord(*this, rec.getCursor() + rec.nextOffset());
	} while (rec.isDeleted());
	return rec;
}

DbRecord DbPage::prevRecord(DbRecord& rec)
{
	DbRecord itr = firstRecord();
	if (rec == itr)
		throw ReachFirstRecord();

	while (rec.nextOffset() != 0)
	{
		if (itr.getCursor() + itr.getSize() == rec.getCursor())
			return itr;
		itr = nextRecord(itr);
	}
	error("Previous record not found");
	return itr;		// Avoid warning
}

DbRecord DbPage::findRecord(const char* key)
{
	DbRecord itr = firstRecord();
	while (itr.nextOffset() != 0)
	{
		if (itr.type() != LongExtended && itr.key() == key)
			return itr;
		itr = nextRecord(itr);
	}
	throw ReachLastRecord();
}

bool DbPage::needSync() const
{
	return !syncFlag;
}

void DbPage::sync()
{
	if (syncFlag || cache.isEmpty())
		return;

	file->getFile()->writeBytes(id * pageSize, pageSize, cache.get());
	syncFlag = true;
}

Cache* DbPage::getCache()
{
	initCache();
	return &cache;
}

DbFile* DbPage::getFile()
{
	return file;
}
