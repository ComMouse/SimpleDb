/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * storage/DbFreeList.h
 * ----------------------------
 * Deleted record list declaration.
 * =============================
 */

// Header Protection
#ifndef STORAGE_DBFREELIST_H
#define STORAGE_DBFREELIST_H

// Header files
#include "../common.h"
#include "common.h"
#include "DbPage.h"
#include "DbRecord.h"

// Class declarations
class DbPage;
class DbRecord;

/**
* Class DbFreeList
*
* List of deleted records in DbFile.
*/
class DbFreeList
{
public:
	// Constructor
	DbFreeList(DbPage& page);
	DbFreeList(const DbFreeList& list) = delete;

	// Destructor
	~DbFreeList();

	// Cursor of first node of the list of deleted records
	size_t freeCursor();

	// Check if this list can create a new record of the given size
	bool hasSpace(size_t size);

	// Get the first record of free node list
	DbRecord firstRecord();

	// Get the next record of free node list
	DbRecord nextRecord(DbRecord& rec);

	// Get the previous record of free node list
	DbRecord prevRecord(DbRecord& rec);

	// Get a deleted record of the given size
	DbRecord findRecord(size_t size);

	// Add a new record to free node list
	void addRecord(DbRecord& rec);

	// Remove the given size of space from the head of the record
	void shrinkRecord(DbRecord& rec, size_t space);

	// Remove the record from free node list
	void removeRecord(DbRecord& rec);

	// Disable copy assignment operator
	DbRecord& operator=(const DbFreeList& list) = delete;
private:
	DbPage* page;

	// Set cursor of first node of the list of deleted records
	void setFreeCursor(size_t cursor);
};
// End of Class DbFreeList

#endif
// End of Header Protection
