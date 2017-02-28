/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * storage/DbRecord.h
 * ----------------------------
 * Db file record declaration.
 * =============================
 */

// Header Protection
#ifndef STORAGE_DBRECORD_H
#define STORAGE_DBRECORD_H

// Header files
#include "../common.h"
#include "common.h"
#include "Cursor.h"
#include "DbFile.h"
#include "DbPage.h"

// Class Declarations
class DbFile;
class DbPage;

// Class Aliases
typedef RecordType Type;

/**
 * Class DbRecord
 *
 * Db file record entity.
 */
class DbRecord
{
public:
	// Exception
	class LastExtendedRecord : std::runtime_error{
	public:
		LastExtendedRecord()
			:runtime_error("Already reaches the last extended record") {}
	};

	// Size estimation
	static size_t getSize(const char* key, const char* value, Type type = Normal);

	// Position constants
	const static size_t nextPos = 0;
	const static size_t nextSize = 2;
	const static size_t delPos = 2;
	const static size_t delSize = 2;
	const static size_t typePos = 4;
	const static size_t typeSize = 1;
	const static size_t keyPos = 5;
	const static size_t keyLenSize = 2;
	const static size_t valueLenSize = 4;
	const static size_t pageIdSize = 4;
	const static size_t recordCursorSize = 2;
	const static size_t minDeletedSize = nextSize + delSize + typeSize;

	// Construtor
	DbRecord(DbPage& page, size_t start);
	DbRecord(const DbRecord& rec);

	// Init record info
	void init(const char* key, const char* value, Type type);

	// Get offset of next record
	size_t nextOffset();

	// Set offset of next record
	void nextOffset(size_t offset);

	// Next node offset of free list
	size_t nextDelCursor();

	// Set node offset of free list
	void nextDelCursor(size_t offset);


	// Get if the record is deleted
	bool isDeleted();

	// Get record type
	Type type();

	// Set record type
	void type(Type type);


	// Get cursor
	size_t getCursor() const;

	// Get size
	size_t getSize();

	// Get page pointer
	DbPage* getPage();

	// Set Size
	void setSize(size_t size);


	// Get key
	std::string key();

	// Get value
	std::string value();

	// Judge whether record can be shrunk, if not shrinking can be also operated through deletion and creation
	bool canShrink(const char* key, const char* value);


	// Get next record
	DbRecord next();

	// Get previous record
	DbRecord prev();


	// Check whether next record for the rest of data exists (long type only)
	bool hasNextExtendedRecord();

	// Get next record for the rest of data (long type only)
	DbRecord nextExtendedRecord();

	// Set next record for the rest of data (long type only)
	void nextExtendedRecord(int pageId, size_t cursor);

	// Compare operator
	bool operator==(const DbRecord& rec) const;
	bool operator!=(const DbRecord& rec) const;

	// Disable copy assignment operator
	DbRecord& operator=(const DbRecord& rec);
private:
	// DbPage pointer
#if MAX_CACHE_PAGE_SWITCH == 1
	Cursor<DbFile, DbPage> page;
#else
	DbPage* page;
#endif

	// Record start pos
	size_t start;

	// Record size
	size_t size;

	// Get page cache
	Cache* cache();

	// Set key
	void setKey(const char* key);

	// Set value
	void setValue(const char* value);
};
// End of Class DbRecord

#endif
// End of Header Protection
