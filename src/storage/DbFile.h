/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * storage/DbFile.h
 * ----------------------------
 * DbFile declaration.
 * =============================
 */

// Header Protection
#ifndef STORAGE_DBFILE_H
#define STORAGE_DBFILE_H

// Header files
#include "../common.h"
#include "../container/Vector.h"
#include "DbPage.h"
#include "File.h"

// Class declarations
class DbPage;
class DbRecord;

/**
 * Class DbFile
 *
 * Database file entity.
 */
class DbFile
{
public:
	// Unit Type def
	typedef DbPage unitType;

	// Constructor
	DbFile(const char* path);
	DbFile(const DbFile& file) = delete;

	// Destructor
	~DbFile();

	// Get a record
	DbRecord getRecord(int pageId, size_t cursor);

	// Delete a record
	void delRecord(DbRecord& record);

	// Create a new record from the given info
	DbRecord newRecord(const char* key, const char* value, RecordType type = Normal);

	// Shrink a record to save space
	void shrinkRecord(DbRecord& record, const char* key, const char* value);

	// Check if the data needs a long record to store
	bool needLongRecord(const char* key, const char* value);


	// Get file pointer
	File* getFile();

	// Get a page at the given pos
	DbPage& get(size_t pos);
	DbPage& getPage(size_t pos);

	// Get a page pointer at the given pos
	DbPage* getPtr(size_t pos);


	// Check if the file needs syncronization
	bool needSync() const;

	// Sync the file
	void sync();

	// Disable copy assignment operator
	DbFile& operator=(const DbFile& file) = delete;

	// Friends
	friend DbPage;
private:
	// File pointer
	File* file;

	// Pages vector
	Vector<DbPage*> pages;

	// In-memory page count
	int pageCount;

	// Append a new page
	DbPage* appendPage();


	// Check page count for recycle
	void checkPage();

	// Shrink pages
	void shrinkPage();

	// Create a new record without checking
	DbRecord getNewRecord(const char* key, const char* value, RecordType type);
};
// End of Class DbFile

#endif
// End of Header Protection
