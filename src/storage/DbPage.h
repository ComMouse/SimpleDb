/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * storage/DbPage.h
 * ----------------------------
 * DbPage declaration.
 * =============================
 */

// Header Protection
#ifndef STORAGE_DBPAGE_H
#define STORAGE_DBPAGE_H

// Header files
#include "../common.h"
#include "../memory/Cache.h"
#include "common.h"
#include "DbFile.h"
#include "DbRecord.h"
#include "DbFreeList.h"

// Class declarations
class DbFile;
class DbRecord;
class DbFreeList;

/**
 * Class DbPage
 *
 * Database file entity.
 */
class DbPage
{
public:
	// Exception
	class ReachFirstRecord : std::runtime_error
	{
	public:
		ReachFirstRecord()
			:runtime_error("Already reaches the first record") {}
	};
	class ReachLastRecord : std::runtime_error
	{
	public:
		ReachLastRecord()
			:runtime_error("Already reaches the last record") {}
	};

	// Default page size (16 KB)
	const static size_t pageSize = PAGE_SIZE;

	// Header size
	const static size_t headerSize = 8;

	// Maximum free space
	const static size_t maxFreeSpace = pageSize - headerSize;

	// Cache sync callback
	static void cbSyncCache(void* page);

	// Constructor
	DbPage(DbFile& file, int id);
	DbPage(const DbPage& page) = delete;

	// Destructor
	~DbPage();


	// Init page
	void init();

	// Get page id
	int getId() const;

	// Count of records in this page
	int recordCount();

	// Size of records in this page
	size_t recordSpace();

	// Size of free space (deleted records not included) in this page
	size_t freeSpace();

	// Cursor of first node of the list of deleted records
	size_t freeCursor();

	// Check if this page can create a new record of the given size
	bool hasSpace(const char* key, const char* value);
	bool hasSpace(size_t size);


	// Get the record at the given cursor
	DbRecord getRecord(size_t cursor);

	// Get the record at the given index
	DbRecord getRecordAt(int index);

	// Delete the record at the given cursor
	void delRecord(size_t cursor);

	// Create a new record from the given info
	DbRecord newRecord(const char* key, const char* value, RecordType type = Normal);

	// Shrink a record
	void shrinkRecord(DbRecord record, const char* key, const char* value);


	// Get the first record of the current record
	DbRecord firstRecord();

	// Get the next record of the current record
	DbRecord nextRecord(DbRecord& record);

	// Get the previous record of the current record
	DbRecord prevRecord(DbRecord& record);

	// Find the first record of given key
	DbRecord findRecord(const char* key);


	// Check if the page needs syncronization
	bool needSync() const;

	// Sync the page with disk file
	void sync();


	// Get the page cache ptr
	Cache* getCache();

	// Get the file ptr of the page
	DbFile* getFile();

	// Disable copy assignment operator
	DbPage& operator=(const DbPage& page) = delete;

	// Friends
	friend DbRecord;
	friend DbFreeList;
private:
	// Starting position of records
	const static size_t recordStartPos = 8;

	// File pointer
	DbFile* file;

	// DbPage id
	int id;

	// Cached page
	Cache cache;

	// Free record list
	DbFreeList* freeList;

	// Sync flag
	bool syncFlag;


	// Init file data
	void initFile();

	// Init cache from file
	void initCache(bool newPage = false);

	// Set count of records in this page
	void setRecordCount(int val);

	// Set size of records in this page
	void setRecordSpace(size_t space);

	// Set size of free space (deleted records not included) in this page
	void setFreeSpace(size_t space);

	// Set cursor of first node of the list of deleted records
	void setFreeCursor(size_t cursor);
};
// End of Class DbPage

#endif
// End of Header Protection
