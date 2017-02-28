/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * storage/DbFreeList.cpp
 * ----------------------------
 * DbFreeList implementation.
 * =============================
 */

// Header files
#include "DbFreeList.h"

DbFreeList::DbFreeList(DbPage& page)
	:page(&page)
{
}

DbFreeList::~DbFreeList()
{
}

size_t DbFreeList::freeCursor()
{
	return page->freeCursor();
}

bool DbFreeList::hasSpace(size_t size)
{
	try
	{
		findRecord(size);
		return true;
	}
	catch (DbPage::ReachLastRecord)
	{
		return false;
	}
}

DbRecord DbFreeList::firstRecord()
{
	size_t cursor = freeCursor();
	if (!cursor)
		throw DbPage::ReachLastRecord();
	return DbRecord(*page, cursor);
}

DbRecord DbFreeList::nextRecord(DbRecord& rec)
{
	size_t cursor = rec.nextDelCursor();
	if (!cursor)
		throw DbPage::ReachLastRecord();
	rec = DbRecord(*page, cursor);
	return rec;
}

DbRecord DbFreeList::prevRecord(DbRecord& rec)
{
	size_t cursor = freeCursor();
	if (!cursor || cursor == rec.getCursor())
		throw DbPage::ReachLastRecord();

	DbRecord itr = DbRecord(*page, cursor);
	while (itr.nextDelCursor() != 0)
	{
		if (itr.nextDelCursor() == rec.getCursor())
			return itr;
		itr = nextRecord(itr);
	}
	throw DbPage::ReachLastRecord();
}

DbRecord DbFreeList::findRecord(size_t size)
{
	try
	{

		for (DbRecord freeItr = firstRecord();; freeItr = nextRecord(freeItr))
		{
			size_t recSize = freeItr.getSize();
			if (size + DbRecord::minDeletedSize <= recSize || size == recSize)
				return freeItr;
		}
	}
	catch (DbPage::ReachLastRecord)
	{
		throw;
	}
}

void DbFreeList::addRecord(DbRecord& rec)
{
	size_t freeCur = freeCursor();
	if (freeCur)
		rec.nextDelCursor(freeCur);
	setFreeCursor(rec.getCursor());
}

void DbFreeList::shrinkRecord(DbRecord& rec, size_t space)
{
	// Do not shrink if the size is just ok
	if (rec.getSize() == space)
	{
		removeRecord(rec);
		return;
	}

	// Check shrink size
	if (rec.getSize() - space < DbRecord::minDeletedSize)
		error("Shrunk space not enough for a free record");

	// Create a record to take the place of the old one in the free list
	DbRecord splitRec(*page, rec.getCursor() + space);
	splitRec.init(nullptr, nullptr, Deleted);
	splitRec.setSize(rec.getSize() - space);
	splitRec.nextDelCursor(rec.nextDelCursor());

	// Update linked list of free records
	if (freeCursor() == rec.getCursor())
	{
		setFreeCursor(splitRec.getCursor());
	}
	else
	{
		DbRecord prevRec = prevRecord(rec);
		prevRec.nextDelCursor(splitRec.getCursor());
	}
}

void DbFreeList::removeRecord(DbRecord& rec)
{
	// Update linked list of free records
	if (freeCursor() == rec.getCursor())
	{
		setFreeCursor(rec.nextDelCursor());
	}
	else
	{
		DbRecord prevRec = prevRecord(rec);
		prevRec.nextDelCursor(rec.nextDelCursor());
	}
}

void DbFreeList::setFreeCursor(size_t cursor)
{
	page->setFreeCursor(cursor);
}