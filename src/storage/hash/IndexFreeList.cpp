/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * storage/hash/IndexFreeList.cpp
 * ----------------------------
 * IndexFreeList implementation.
 * =============================
 */

// Header files
#include "IndexFreeList.h"


IndexFreeList::IndexFreeList(IndexPage& page)
	:page(&page)
{
}

IndexFreeList::~IndexFreeList()
{
}

size_t IndexFreeList::freeCursor()
{
	return page->freeCursor();
}

bool IndexFreeList::hasSpace(size_t size)
{
	try
	{
		findIndex(size);
		return true;
	}
	catch (IndexPage::ReachLastIndex)
	{
		return false;
	}
}

IndexNode IndexFreeList::firstIndex()
{
	size_t cursor = freeCursor();
	if (!cursor)
		throw IndexPage::ReachLastIndex();
	return IndexNode(*page, cursor);
}

IndexNode IndexFreeList::nextIndex(IndexNode& rec)
{
	size_t cursor = rec.nextDelCursor();
	if (!cursor)
		throw IndexPage::ReachLastIndex();
	rec = IndexNode(*page, cursor);
	return rec;
}

IndexNode IndexFreeList::prevIndex(IndexNode& rec)
{
	size_t cursor = freeCursor();
	if (!cursor || cursor == rec.getCursor())
		throw IndexPage::ReachLastIndex();

	IndexNode itr = IndexNode(*page, cursor);
	while (itr.nextDelCursor() != 0)
	{
		if (itr.nextDelCursor() == rec.getCursor())
			return itr;
		itr = nextIndex(itr);
	}
	throw IndexPage::ReachLastIndex();
}

IndexNode IndexFreeList::findIndex(size_t size)
{
	try
	{
		for (IndexNode freeItr = firstIndex();; freeItr = nextIndex(freeItr))
		{
			size_t recSize = freeItr.getSize();
			if (size + IndexNode::minDeletedSize <= recSize || size == recSize)
				return freeItr;
		}
	}
	catch (IndexPage::ReachLastIndex)
	{
		throw;
	}
}

void IndexFreeList::addIndex(IndexNode& rec)
{
	size_t freeCur = freeCursor();
	if (freeCur)
		rec.nextDelCursor(freeCur);
	setFreeCursor(rec.getCursor());
}

void IndexFreeList::shrinkIndex(IndexNode& rec, size_t space)
{
	// Do not shrink if the size is just ok
	if (rec.getSize() == space)
	{
		removeIndex(rec);
		return;
	}

	// Check shrink size
	if (rec.getSize() - space < IndexNode::minDeletedSize)
		error("Shrunk space not enough for a free record");

	// Create a record to take the place of the old one in the free list
	IndexNode splitRec(*page, rec.getCursor() + space);
	splitRec.initDel(rec.getSize() - space);
	splitRec.nextDelCursor(rec.nextDelCursor());

	// Update linked list of free records
	if (freeCursor() == rec.getCursor())
	{
		setFreeCursor(splitRec.getCursor());
	}
	else
	{
		IndexNode prevRec = prevIndex(rec);
		prevRec.nextDelCursor(splitRec.getCursor());
	}
}

void IndexFreeList::removeIndex(IndexNode& rec)
{
	// Update linked list of free records
	if (freeCursor() == rec.getCursor())
	{
		setFreeCursor(rec.nextDelCursor());
	}
	else
	{
		IndexNode prevRec = prevIndex(rec);
		prevRec.nextDelCursor(rec.nextDelCursor());
	}
}

void IndexFreeList::setFreeCursor(size_t cursor)
{
	page->setFreeCursor(cursor);
}
