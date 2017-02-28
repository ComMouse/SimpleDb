/**
* Programming II Project
* SE106 - 2015 Spring
*
* =============================
* db/HashTableDb.cpp
* ----------------------------
* Hash table based database implementation.
* =============================
*/

#include "HashTableDb.h"

HashTableDb::HashTableDb(const std::string& path, int spacePow)
	:ht(nullptr), space(0x01 << spacePow), hashMask(0x00)
{
	ht = new IndexFile((path + ".idx").c_str(), space);
	db = new DbFile((path + ".db").c_str());
	hashMask = space - 1;
}

HashTableDb::~HashTableDb()
{
	delete db;
	delete ht;
}

bool HashTableDb::exists(const std::string& key) const
{
	size_t hashCode = getHash(key) & hashMask;
	return ht->exists(hashCode, key.c_str());
}

std::string HashTableDb::get(const std::string& key)
{
	size_t hashCode = getHash(key) & hashMask;
	try
	{
		IndexNode node = ht->get(hashCode, key.c_str());
		DbRecord rec = node.getRecord(*db);
		if (rec.type() == Long)
		{
			std::string value = rec.value();
			while (rec.hasNextExtendedRecord())
			{
				rec = rec.nextExtendedRecord();
				value += rec.value();
			}
			return value;
		}
		else
		{
			return rec.value();
		}
	}
	catch (IndexFile::IndexNotFound)
	{
		throw NotFound();
	}
}

void HashTableDb::set(const std::string& key, const std::string& value)
{
	size_t hashCode = getHash(key) & hashMask;
	try
	{
		DbRecord rec = ht->get(hashCode, key.c_str()).getRecord(*db);
		if (rec.canShrink(key.c_str(), value.c_str()))
		{
			db->shrinkRecord(rec, key.c_str(), value.c_str());
		}
		else
		{
			db->delRecord(rec);
			ht->del(hashCode, key.c_str());

			DbRecord newRec = db->newRecord(key.c_str(), value.c_str());
			ht->add(hashCode, key.c_str(), newRec.getPage()->getId(), newRec.getCursor());
		}
	}
	catch (IndexFile::IndexNotFound)
	{
		DbRecord newRec = db->newRecord(key.c_str(), value.c_str());
		ht->add(hashCode, key.c_str(), newRec.getPage()->getId(), newRec.getCursor());
	}
}

void HashTableDb::del(const std::string& key)
{
	size_t hashCode = getHash(key) & hashMask;
	try
	{
		IndexNode node = ht->get(hashCode, key.c_str());
		db->delRecord(node.getRecord(*db));
		ht->del(hashCode, key.c_str());
	}
	catch (IndexFile::IndexNotFound)
	{
		throw NotFound();
	}
}

std::string HashTableDb::operator[](const std::string& key)
{
	return get(key);
}


// BKDR Hash Algorithm
size_t HashTableDb::getHash(const std::string& str) const
{
	size_t hash = 0;
	for (std::string::size_type i = 0; i < str.size(); ++i)
	{
		hash = hash * 31 + str[i];	// or 131, 1313, ...
	}
	return hash;
}
