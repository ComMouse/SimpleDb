/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * storage/hash/IndexFile.cpp
 * ----------------------------
 * IndexFile implementation.
 * =============================
 */

// Header files
#include "IndexFile.h"

void IndexFile::cbSyncCache(void* file)
{
	static_cast<IndexFile*>(file)->syncCache();
}

IndexFile::IndexFile(const char* path, int tableSize)
	:file(nullptr), syncFlag(true), pageCount(0), htSize(tableSize)
{
	file = new File(path);

	// Cache settings
	cache.weak(false);
	cache.callback(IndexFile::cbSyncCache, this);
	cache.syncStrategy(SyncStrategy(SyncStrategy::WriteInterval, 1000));

	// Init index file
	if (file->size() == 0)
		initFile();

	// Read page size
	pages.resize((file->size() - pageStartPos()) / IndexPage::pageSize);
}

IndexFile::~IndexFile()
{
	sync();
	for (int i = 0; i < pages.size(); ++i)
	{
		if (pages[i])
			delete pages[i];
	}
	delete file;
}

int IndexFile::tableSize()
{
	return htSize;
}

bool IndexFile::exists(size_t hash, const char* key)
{
	try
	{
		get(hash, key);
		return true;
	}
	catch (IndexNotFound)
	{
		return false;
	}
}

IndexNode IndexFile::get(size_t hash, const char* key)
{
	initCache();

	wsize_t cursor = getPosAt(hash);
	if (!cursor)
		throw IndexNotFound();

	IndexNode node = getIndexAt(cursor);
	if (node.key() == key)
		return node;
	while (node.hasNext())
	{
		node = node.next();
		if (node.key() == key)
			return node;
	}

	throw IndexNotFound();
}

void IndexFile::set(size_t hash, const char* key, int pageId, size_t cursor)
{
	get(hash, key).setRecord(pageId, cursor);
}

void IndexFile::add(size_t hash, const char* key, int pageId, size_t cursor)
{
	initCache();

	wsize_t hashCursor = getPosAt(hash);
	IndexNode node = getNewIndex(key, pageId, cursor);
	node.nextNodePos(hashCursor);
	setPosAt(hash, node.getFullCursor());
}

void IndexFile::del(size_t hash, const char* key)
{
	initCache();

	IndexNode node = get(hash, key);
	IndexNode firstNode = getFirstIndex(hash);
	if (node == firstNode)
		setPosAt(hash, node.nextNodePos());
	else
		node.prev(firstNode).nextNodePos(node.nextNodePos());
	node.getPage()->delIndex(node.getCursor());
}

bool IndexFile::needSync() const
{
	if (!syncFlag)
		return true;
	for (int i = 0; i < pages.size(); ++i)
	{
		if (pages[i] && pages[i]->needSync())
			return true;
	}
	return false;
}

void IndexFile::sync()
{
	syncCache();
	for (int i = 0; i < pages.size(); ++i)
	{
		if (pages[i])
			pages[i]->sync();
	}
	file->flush();
}

Cache* IndexFile::getCache()
{
	return &cache;
}

File* IndexFile::getFile()
{
	return file;
}

IndexPage& IndexFile::get(size_t pos)
{
	return getPage(pos);
}

IndexPage& IndexFile::getPage(size_t pos)
{
	return *getPtr(pos);
}

IndexPage* IndexFile::getPtr(size_t pos)
{
	if (pages[pos])
		return pages[pos];
	pages[pos] = new IndexPage(*this, pos);
	++pageCount;
	return pages[pos];
}

IndexPage* IndexFile::appendPage()
{
	IndexPage* page = new IndexPage(*this, pages.size());
	page->init();
	//page->sync();
	pages.pushBack(page);
	++pageCount;
	return page;
}

size_t IndexFile::tableSpace()
{
	return tableSize() * tableItemSize;
}

size_t IndexFile::pageStartPos()
{
	if (pagePos == 0)
		pagePos = (static_cast<size_t>(ceil(
		double(tableSpace()) / IndexPage::pageSize)) * IndexPage::pageSize);
	return pagePos;
}

void IndexFile::initFile()
{
	// Use unique ptr to ensure exception safety
	size_t tableSize = pageStartPos();
	std::unique_ptr<char> emptyBytes(new char[tableSize]);
	memset(emptyBytes.get(), 0, sizeof(char) * tableSize);
	file->writeBytes(0, tableSize, emptyBytes.get());
	file->flush();
	emptyBytes.release();
}

void IndexFile::initCache()
{
	if (!cache.isEmpty())
		return;

	size_t tableSize = tableSpace();
	if (cache.size() > tableSize)
		error("Invalid cache size");

	cache.set(file->readBytes(0, tableSize), tableSize);
}

void IndexFile::syncCache()
{
	if (syncFlag || cache.isEmpty())
		return;

	size_t tableSize = tableSpace();
	file->writeBytes(0, tableSize, cache.get());
	syncFlag = true;
}

wsize_t IndexFile::getPosAt(size_t hash)
{
	initCache();
	return cache.readLong(hash * tableItemSize, tableItemSize);
}

void IndexFile::setPosAt(size_t hash, wsize_t cursor)
{
	initCache();
	cache.writeLong(hash * tableItemSize, cursor, tableItemSize);
	syncFlag = false;
}

IndexNode IndexFile::getFirstIndex(size_t hash)
{
	initCache();
	return getIndexAt(getPosAt(hash));
}

IndexNode IndexFile::getIndexAt(wsize_t pos)
{
	int pageId = static_cast<int>(pos >> (IndexNode::nextNodeCursorSize * 8));
	size_t offset = pos & 0xFFFF;
	return getPage(pageId).getIndex(offset);
}

IndexNode IndexFile::getNewIndex(const char* key, int pageId, size_t cursor)
{
	size_t size = IndexNode::getSize(key);

	// Try creating a record in one of pages
	for (int i = pages.size() - 1; i >= 0; --i)
	{
		if (getPage(i).hasSpace(size))
			return getPage(i).newIndex(key, pageId, cursor);
	}

	// Create the record in a new page
	return appendPage()->newIndex(key, pageId, cursor);
}
