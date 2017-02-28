/**
* Programming II Project
* SE106 - 2015 Spring
*
* =============================
* db/HashTableDb.h
* ----------------------------
* Hash table based database declaration.
* =============================
*/

// Header Protection
#ifndef DB_HASHTABLE_H
#define DB_HASHTABLE_H

#include "../common.h"
#include "../storage/common.h"
#include "../storage/DbFile.h"
#include "../storage/DbRecord.h"
#include "../storage/IndexFile.h"
#include "../storage/IndexNode.h"
#include "../container/HashTable.h"

/**
* Class HashTableDb
*s
* Implementation of hash table based database.
*/
class HashTableDb
{
public:
	// Exception
	class NotFound : std::runtime_error
	{
	public:
		NotFound()
			:runtime_error("Key not found") {}
	};

	// Constructor
	explicit HashTableDb(const std::string& path, int spacePow = 4);

	explicit HashTableDb(const HashTableDb& ht) = delete;
	  
	// Destructor
	~HashTableDb();


	// CRUD functions
	bool exists(const std::string& key) const;

	std::string get(const std::string& key);

	void set(const std::string& key, const std::string& value);

	void del(const std::string& key);

	std::string operator[](const std::string& key);

	// Disable copy assignment operator
	HashTableDb& operator=(const HashTableDb& ht) = delete;
private:
	// Index file
	IndexFile* ht;

	// Database file
	DbFile* db;

	// Space pow
	int space;

	// Hash mask
	size_t hashMask;

	// Hash algorithm
	size_t getHash(const std::string& str) const;
};
// End of Class HashTable

#endif
// End of Header Protection
