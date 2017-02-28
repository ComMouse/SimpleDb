/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * storage/hash/IndexFile.h
 * ----------------------------
 * IndexFile declaration.
 * =============================
 */

// Header Protection
#ifndef STORAGE_HASH_INDEXFILE_H
#define STORAGE_HASH_INDEXFILE_H

// Header files
#include "../../common.h"
#include "../../memory/Cache.h"
#include "../common.h"
#include "../File.h"
#include "IndexPage.h"
#include "IndexNode.h"

// Class declarations
class File;
class IndexPage;
class IndexNode;

/**
* Class IndexFile
*
* Database index file entity.
*/
class IndexFile
{
public:
	// Exception
	class IndexNotFound : std::runtime_error
	{
	public:
		IndexNotFound()
			:runtime_error("Index not found") {}
	};

	// Cache sync callback
	static void cbSyncCache(void* file);

	// Unit Type def
	typedef IndexPage unitType;

	// Constructor
	IndexFile(const char* path, int tableSize);
	IndexFile(const IndexFile& file) = delete;

	// Destructor
	~IndexFile();

	// Count of records in this page
	int tableSize();


	// Check whether the key exists
	bool exists(size_t hash, const char* key);

	// Get the index of the key
	IndexNode get(size_t hash, const char* key);

	// Set the index of the key
	void set(size_t hash, const char* key, int pageId, size_t cursor);

	// Add an index to the file
	void add(size_t hash, const char* key, int pageId, size_t cursor);

	// Delete the index of the key
	void del(size_t hash, const char* key);


	// Check if the file needs syncronization
	bool needSync() const;

	// Sync the page with disk file
	void sync();


	// Get the table cache ptr
	Cache* getCache();

	// Get the file ptr of the page
	File* getFile();

	// Get a page at the given pos
	IndexPage& get(size_t pos);
	IndexPage& getPage(size_t pos);

	// Get a page pointer at the given pos
	IndexPage* getPtr(size_t pos);


	// Disable copy assignment operator
	IndexFile& operator=(const IndexFile& page) = delete;
	
	// Friends
	friend IndexNode;
	friend IndexPage;
private:
	// Size of each item in the hash table
	const static size_t tableItemSize = 6;

	// File pointer
	File* file;

	// Cached table
	Cache cache;

	// Pages vector
	Vector<IndexPage*> pages;

	// Cache sync flag
	bool syncFlag;

	// In-memory page count
	int pageCount;

	// Size of hash table
	int htSize;

	// Page start pos
	size_t pagePos;


	// Append a new page
	IndexPage* appendPage();


	// Get the size of the hash table
	size_t tableSpace();

	// Get the start pos of pages
	size_t pageStartPos();


	// Init file data
	void initFile();

	// Init cache from file
	void initCache();

	// Sync table cache
	void syncCache();

	// Get the pos in the hash table
	wsize_t getPosAt(size_t hash);

	// Set the pos in the hash table
	void setPosAt(size_t hash, wsize_t cursor);

	// Get the first index in the hash table at the hash
	IndexNode getFirstIndex(size_t hash);

	// Get the index at the pos
	IndexNode getIndexAt(wsize_t pos);


	IndexNode getNewIndex(const char* key, int pageId, size_t cursor);
};
// End of Class DbPage

#endif
// End of Header Protection
