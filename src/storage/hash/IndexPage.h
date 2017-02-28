/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * storage/hash/IndexPage.h
 * ----------------------------
 * IndexPage declaration.
 * =============================
 */

// Header Protection
#ifndef STORAGE_HASH_INDEXPAGE_H
#define STORAGE_HASH_INDEXPAGE_H

// Header files
#include "../../common.h"
#include "../../memory/Cache.h"
#include "../common.h"
#include "IndexFile.h"
#include "IndexNode.h"
#include "IndexPage.h"
#include "IndexFreeList.h"

// Class declarations
class IndexFile;
class IndexNode;
class IndexFreeList;

/**
* Class IndexPage
*
* Database file entity.
*/
class IndexPage
{
public:
	// Exception
	class ReachFirstIndex : std::runtime_error
	{
	public:
		ReachFirstIndex()
			:runtime_error("Already reaches the first index") {}
	};
	class ReachLastIndex : std::runtime_error
	{
	public:
		ReachLastIndex()
			:runtime_error("Already reaches the last index") {}
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
	IndexPage(IndexFile& file, int id);
	IndexPage(const IndexPage& page) = delete;

	// Destructor
	~IndexPage();


	// Init page
	void init();

	// Get page id
	int getId() const;

	// Count of indexs in this page
	int indexCount();

	// Size of indexs in this page
	size_t indexSpace();

	// Size of free space (deleted indexs not included) in this page
	size_t freeSpace();

	// Cursor of first node of the list of deleted indexs
	size_t freeCursor();

	// Check if this page can create a new index of the given size
	bool hasSpace(const char* key);
	bool hasSpace(size_t size);


	// Get the index at the given cursor
	IndexNode getIndex(size_t cursor);

	// Delete the index at the given cursor
	void delIndex(size_t cursor);

	// Create a new index from the given info
	IndexNode newIndex(const char* key, int pageId, size_t cursor);

	// Shrink an index
	void shrinkIndex(IndexNode index, const char* key, const char* value);


	// Check if the page needs syncronization
	bool needSync() const;

	// Sync the page with disk file
	void sync();


	// Get the page cache ptr
	Cache* getCache();

	// Get the file ptr of the page
	IndexFile* getFile();

	// Disable copy assignment operator
	IndexPage& operator=(const IndexPage& page) = delete;

	// Friends
	friend IndexNode;
	friend IndexFreeList;
private:
	// Starting position of indexs
	const static size_t indexStartPos = headerSize;

	// File pointer
	IndexFile* file;

	// IndexPage id
	int id;

	// Cached page
	Cache cache;

	// Free index list
	IndexFreeList* freeList;

	// Sync flag
	bool syncFlag;


	// Init file data
	void initFile();

	// Init cache from file
	void initCache(bool newPage = false);

	// Set count of indexs in this page
	void setIndexCount(int val);

	// Set size of indexs in this page
	void setIndexSpace(size_t space);

	// Set size of free space (deleted indexs not included) in this page
	void setFreeSpace(size_t space);

	// Set cursor of first node of the list of deleted indexs
	void setFreeCursor(size_t cursor);
};
// End of Class IndexPage

#endif
// End of Header Protection
