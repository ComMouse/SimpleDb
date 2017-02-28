/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * storage/hash/Page.cpp
 * ----------------------------
 * IndexFile page implementation.
 * =============================
 */

// Header files
#include "IndexPage.h"
#include <locale>

void IndexPage::cbSyncCache(void* page)
{
	static_cast<IndexPage*>(page)->sync();
}

IndexPage::IndexPage(IndexFile& file, int id)
	:file(&file), id(id), syncFlag(true)
{
	// Init free record list
	freeList = new IndexFreeList(*this);
	// Disallow recycle
	cache.weak(false);
	// Register cache sync callback
	cache.callback(IndexPage::cbSyncCache, this);
	// Register strategy
	cache.syncStrategy(SyncStrategy(SyncStrategy::WriteInterval, 100));
}

IndexPage::~IndexPage()
{
	if (!syncFlag)
		sync();
	delete freeList;
}

void IndexPage::init()
{
	//initFile();
	initCache(true);
	setIndexCount(0);
	setIndexSpace(0);
	setFreeCursor(0);
	setFreeSpace(pageSize - headerSize);
	//sync();
}

int IndexPage::getId() const
{
	return id;
}

int IndexPage::indexCount()
{
	initCache();
	int count = cache.readInt(0, 2);
	return count;
}

size_t IndexPage::indexSpace()
{
	initCache();
	size_t space = cache.readInt(2, 2);
	return space;
}

size_t IndexPage::freeSpace()
{
	initCache();
	size_t space = cache.readInt(4, 2);
	return space;
}

size_t IndexPage::freeCursor()
{
	initCache();
	size_t cursor = cache.readInt(6, 2);
	return cursor;
}

bool IndexPage::hasSpace(const char* key)
{
	return hasSpace(IndexNode::getSize(key));
}

bool IndexPage::hasSpace(size_t size)
{
	if (freeSpace() >= size)
		return true;

	return freeList->hasSpace(size);
}

IndexNode IndexPage::getIndex(size_t cursor)
{
	return IndexNode(*this, cursor);
}

void IndexPage::delIndex(size_t cursor)
{
	IndexNode rec = getIndex(cursor);
	if (!rec.isDeleted())
		setIndexCount(indexCount() - 1);
	rec.initDel(rec.getSize());
	freeList->addIndex(rec);
}

IndexNode IndexPage::newIndex(const char* key, int pageId, size_t cursor)
{
	syncFlag = false;
	size_t size = IndexNode::getSize(key);

	// Create from free space
	if (freeSpace() >= size)
	{
		IndexNode rec(*this, indexStartPos + indexSpace());
		rec.init(key, pageId, cursor);

		setIndexCount(indexCount() + 1);
		setIndexSpace(indexSpace() + size);
		setFreeSpace(freeSpace() - size);

		return rec;
	}

	// Check free node list
	try
	{
		IndexNode rec = freeList->findIndex(size);
		freeList->shrinkIndex(rec, size);

		// Init record
		rec.init(key, pageId, cursor);
		return rec;
	}
	catch (ReachLastIndex)
	{
		error("No space for new record");
		return IndexNode(*this, 0);		// Aviod Warning
	}
}

void IndexPage::shrinkIndex(IndexNode index, const char* key, const char* value)
{
	if (!index.canShrink(key))
	{
		error("Record size unsuitable for shrinking");
	}

	size_t oldSize = index.getSize();
	size_t newSize = IndexNode::getSize(key);

	int pageId = index.getRecordPageId();
	size_t cursor = index.getRecordPageCursor();

	IndexNode freeRec(*this, index.getCursor() + newSize);
	freeRec.initDel(oldSize - newSize);
	delIndex(freeRec.getCursor());

	index.init(key, pageId, cursor);
}

bool IndexPage::needSync() const
{
	return !syncFlag;
}

void IndexPage::sync()
{
	if (syncFlag || cache.isEmpty())
		return;

	file->getFile()->writeBytes(file->pageStartPos() + id * pageSize, pageSize, cache.get());
	syncFlag = true;
}

Cache* IndexPage::getCache()
{
	return &cache;
}

IndexFile* IndexPage::getFile()
{
	return file;
}

void IndexPage::initFile()
{
	// Use unique ptr to ensure exception safety
	std::unique_ptr<char> emptyBytes(new char[pageSize]);
	memset(emptyBytes.get(), 0, sizeof(char) * pageSize);
	file->getFile()->writeBytes(file->pageStartPos() + id * pageSize, pageSize, emptyBytes.get());
	file->getFile()->flush();
	emptyBytes.release();
}

void IndexPage::initCache(bool newPage)
{
	if (!cache.isEmpty())
		return;

	if (cache.size() > pageSize)
	{
		error("");
	}

	if (!newPage)
	{
		cache.set(file->getFile()->readBytes(getFile()->pageStartPos() + id * pageSize, pageSize), pageSize);
	}
	else
	{
		char* emptyBytes = new char[pageSize];
		memset(emptyBytes, 0, sizeof(char) * pageSize);
		cache.set(emptyBytes, pageSize);
	}
}

void IndexPage::setIndexCount(int val)
{
	initCache();
	cache.writeInt(0, val, 2);
	syncFlag = false;
}

void IndexPage::setIndexSpace(size_t space)
{
	initCache();
	cache.writeInt(2, space, 2);
	syncFlag = false;
}

void IndexPage::setFreeSpace(size_t space)
{
	initCache();
	cache.writeInt(4, space, 2);
	syncFlag = false;
}

void IndexPage::setFreeCursor(size_t cursor)
{
	initCache();
	cache.writeInt(6, cursor, 2);
	syncFlag = false;
}
