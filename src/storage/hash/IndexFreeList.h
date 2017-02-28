/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * storage/hash/IndexFreeList.h
 * ----------------------------
 * Deleted index list declaration.
 * =============================
 */

// Header Protection
#ifndef STORAGE_HASH_INDEXFREELIST_H
#define STORAGE_HASH_INDEXFREELIST_H

// Header files
#include "../../common.h"
#include "../common.h"
#include "IndexPage.h"
#include "IndexNode.h"

// Class declarations
class IndexPage;
class IndexNode;

/**
* Class IndexFreeList
*
* List of deleted indexes in IndexFile.
*/
class IndexFreeList
{
public:
	// Constructor
	IndexFreeList(IndexPage& page);
	IndexFreeList(const IndexFreeList& list) = delete;

	// Destructor
	~IndexFreeList();

	// Cursor of first node of the list of deleted records
	size_t freeCursor();

	// Check if this list can create a new record of the given size
	bool hasSpace(size_t size);

	// Get the first index of free node list
	IndexNode firstIndex();

	// Get the next index of free node list
	IndexNode nextIndex(IndexNode& rec);

	// Get the previous index of free node list
	IndexNode prevIndex(IndexNode& rec);

	// Get a deleted index of the given size
	IndexNode findIndex(size_t size);

	// Add a new index to free node list
	void addIndex(IndexNode& rec);

	// Remove the given size of space from the head of the index
	void shrinkIndex(IndexNode& rec, size_t space);

	// Remove the index from free node list
	void removeIndex(IndexNode& rec);

	// Disable copy assignment operator
	IndexNode& operator=(const IndexFreeList& list) = delete;
private:
	IndexPage* page;

	// Set cursor of first node of the list of deleted records
	void setFreeCursor(size_t cursor);
};
// End of Class IndexFreeList

#endif
// End of Header Protection
