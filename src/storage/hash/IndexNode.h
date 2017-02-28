/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * storage/hash/IndexNode.h
 * ----------------------------
 * Index file node declaration.
 * =============================
 */

// Header Protection
#ifndef STORAGE_HASH_INDEXNODE_H
#define STORAGE_HASH_INDEXNODE_H

// Header files
#include "../../common.h"
#include "../common.h"
#include "../DbFile.h"
#include "../DbRecord.h"
#include "IndexFile.h"
#include "IndexPage.h"

// Class Declarations
class IndexFile;
class IndexPage;
class DbFile;
class DbRecord;

/**
* Class IndexNode
*
* Index file node entity.
*/
class IndexNode
{
public:
	// Exception
	class ReachFirst : std::runtime_error
	{
	public:
		ReachFirst()
			:runtime_error("Already reaches the first node") {}
	};
	class ReachLast : std::runtime_error
	{
	public:
		ReachLast()
			:runtime_error("Already reaches the last node") {}
	};

	// Size estimation
	static size_t getSize(const char* key);

	// Position constants
	const static size_t pageIdSize = 4;
	const static size_t nextNodeCursorSize = 2;
	const static size_t nextPos = 0;
	const static size_t nextSize = pageIdSize + nextNodeCursorSize;
	const static size_t delPos = nextSize;
	const static size_t delSize = 2;
	const static size_t keyPos = delPos + delSize;
	const static size_t keyLenSize = 2;
	const static size_t recordCursorSize = 2;
	const static size_t minDeletedSize = nextSize + delSize + keyLenSize;

	// Construtor
	IndexNode(IndexPage& page, size_t cursor);
	IndexNode(const IndexNode& rec);

	// Check whether this is a null node
	bool isNull() const;

	// Init index info
	void init(const char* key, int pageId, size_t cursor);

	// Init deleted node info
	void initDel(size_t size);

	// Get cursor of next index node
	wsize_t nextNodePos();
	int nextNodePageId();
	size_t nextNodePageCursor();

	// Set cursor of next index node
	void nextNodePos(int pageId, size_t cursor);
	void nextNodePos(wsize_t pos);

	// Next node cursor of free list
	size_t nextDelCursor();

	// Set node cursor of free list
	void nextDelCursor(size_t cursor);


	// Get if the record is deleted
	bool isDeleted();


	// Get cursor
	size_t getCursor() const;

	// Get cursor with page id
	wsize_t getFullCursor();

	// Get size
	size_t getSize();

	// Get file pointer
	IndexPage* getPage();


	// Get key
	std::string key();

	// Check if the node can be shrunk
	bool canShrink(const char* key);

	// Check whether the next node exists
	bool hasNext();

	// Get next node
	IndexNode next();

	// Get previous node
	IndexNode prev(IndexNode& startNode);


	// Get the record
	DbRecord getRecord(DbFile& file);

	// Get record page id
	int getRecordPageId();

	// Get record page cursor
	size_t getRecordPageCursor();

	// Set the record
	void setRecord(wsize_t pos);
	void setRecord(int pageId, size_t cursor);

	// Compare operator
	bool operator==(const IndexNode& rec) const;
	bool operator!=(const IndexNode& rec) const;

	// Disable copy assignment operator
	IndexNode& operator=(const IndexNode& rec);
private:
	// File pointer
	IndexPage* page;

	// Record start pos
	size_t start;

	// Record size
	size_t size;

	// Get page cache
	Cache* cache();

	// Set key
	void setKey(const char* key);
};
// End of Class IndexNode

#endif
// End of Header Protection
