/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * storage/hash/IndexNode.cpp
 * ----------------------------
 * IndexFile node implementation.
 * =============================
 */

// Header files
#include "IndexNode.h"

size_t IndexNode::getSize(const char* key)
{
	return nextSize + delSize + keyLenSize + strlen(key) + pageIdSize + recordCursorSize;
}

IndexNode::IndexNode(IndexPage& page, size_t cursor)
	:page(&page), start(cursor), size(0)
{
}

IndexNode::IndexNode(const IndexNode& rec)
{
	page = rec.page;
	start = rec.start;
	size = rec.size;
}

bool IndexNode::isNull() const
{
	return (start == 0);
}

void IndexNode::init(const char* key, int pageId, size_t cursor)
{
	this->nextNodePos(0);
	this->nextDelCursor(0);
	setKey(key);
	this->setRecord(pageId, cursor);
}

void IndexNode::initDel(size_t size)
{
	this->nextNodePos(0);
	this->nextDelCursor(0);
	std::string str(size - minDeletedSize, '0');
	setKey(str.c_str());
}

wsize_t IndexNode::nextNodePos()
{
	wsize_t cursor = cache()->readLong(start + nextPos, nextSize);
	return cursor;
}

int IndexNode::nextNodePageId()
{
	return static_cast<int>(nextNodePos() >> (nextNodeCursorSize * 8));
}

size_t IndexNode::nextNodePageCursor()
{
	return nextNodePos() & 0xFFFF;
}

void IndexNode::nextNodePos(int pageId, size_t cursor)
{
	wsize_t pos = ((wsize_t)(pageId) << (nextNodeCursorSize * 8)) | cursor;
	nextNodePos(pos);
}

void IndexNode::nextNodePos(wsize_t pos)
{
	cache()->writeLong(start + nextPos, pos, nextSize);
	page->syncFlag = false;
}

size_t IndexNode::nextDelCursor()
{
	size_t cursor = cache()->readInt(start + delPos, delSize);
	return cursor;
}

void IndexNode::nextDelCursor(size_t cursor)
{
	cache()->writeInt(start + delPos, cursor, delSize);
	size = 0;
	page->syncFlag = false;
}

bool IndexNode::isDeleted()
{
	return nextDelCursor() != 0;
}

size_t IndexNode::getCursor() const
{
	return start;
}

wsize_t IndexNode::getFullCursor()
{
	return ((wsize_t)(getPage()->getId()) <<
		(nextNodeCursorSize * 8)) |
		getCursor();
}

size_t IndexNode::getSize()
{
	if (size)
		return size;

	size = minDeletedSize;
	size += cache()->readStringSize(start + keyPos, keyLenSize);
	if (!isDeleted())
		size += pageIdSize + recordCursorSize;
	return size;
}

IndexPage* IndexNode::getPage()
{
	return page;
}

std::string IndexNode::key()
{
	return cache()->readString(start + keyPos, keyLenSize);
}

bool IndexNode::canShrink(const char* key)
{
	size_t oldSize = getSize();
	size_t newSize = getSize(key);
	if (newSize > oldSize)
		return false;
	if (oldSize - newSize < minDeletedSize)
		return false;

	return true;
}

bool IndexNode::hasNext()
{
	return nextNodePos() != 0;
}

IndexNode IndexNode::next()
{
	int pageId = nextNodePageId();
	size_t pageCursor = nextNodePageCursor();

	if (pageId == 0 && pageCursor == 0)
		throw ReachLast();
	return IndexNode(getPage()->getFile()->getPage(pageId), pageCursor);
}

IndexNode IndexNode::prev(IndexNode& startNode)
{
	IndexNode itr = startNode;
	if (*this == itr)
		throw ReachFirst();

	while (itr.hasNext())
	{
		IndexNode next = itr.next();
		if (next == *this)
			return itr;
		itr = next;
	}
	error("Previous record not found");
	return itr;		// Avoid warning
}

DbRecord IndexNode::getRecord(DbFile& file)
{
	size_t pos = start;
	pos += keyPos + keyLenSize + cache()->readStringSize(keyPos + pos, keyLenSize);

	int pageId = cache()->readInt(pos, pageIdSize);
	size_t cursor = cache()->readInt(pageIdSize + pos, recordCursorSize);

	return file.getRecord(pageId, cursor);
}

int IndexNode::getRecordPageId()
{
	size_t pos = start;
	pos += keyPos + keyLenSize + cache()->readStringSize(keyPos + pos, keyLenSize);

	int pageId = cache()->readInt(pos, pageIdSize);
	return pageId;
}

size_t IndexNode::getRecordPageCursor()
{
	size_t pos = start;
	pos += keyPos + keyLenSize + cache()->readStringSize(keyPos + pos, keyLenSize);

	size_t cursor = cache()->readInt(pageIdSize + pos, recordCursorSize);
	return cursor;
}

void IndexNode::setRecord(wsize_t recPos)
{
	size_t pos = start;
	pos += keyPos + keyLenSize + cache()->readStringSize(keyPos + pos, keyLenSize);
	cache()->writeLong(pos, recPos, pageIdSize + recordCursorSize);
}

void IndexNode::setRecord(int pageId, size_t cursor)
{
	size_t pos = start;
	pos += keyPos + keyLenSize + cache()->readStringSize(keyPos + pos, keyLenSize);

	cache()->writeInt(pos, pageId, pageIdSize);
	cache()->writeInt(pageIdSize + pos, cursor, recordCursorSize);
}

bool IndexNode::operator==(const IndexNode& rec) const
{
	return (page == rec.page) && (start == rec.start);
}

bool IndexNode::operator!=(const IndexNode& rec) const
{
	return !(*this == rec);
}

IndexNode& IndexNode::operator=(const IndexNode& rec)
{
	page = rec.page;
	start = rec.start;
	size = rec.size;
	return *this;
}

Cache* IndexNode::cache()
{
	page->initCache();
	return page->getCache();
}

void IndexNode::setKey(const char* key)
{
	cache()->writeString(start + keyPos, key, keyLenSize);
	page->syncFlag = false;
}

