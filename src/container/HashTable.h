/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * container/HashTable.h
 * ----------------------------
 * Hash table implementation.
 * =============================
 */

// Header Protection
#ifndef CONTAINER_HASHTABLE_H
#define CONTAINER_HASHTABLE_H

/**
 * Template Class HashLinkedNode
 *
 * Implementation of node of a linked list in a hash.
 */
template <class DataType>
class HashLinkedNode
{
public:
	HashLinkedNode(const DataType& key, const DataType& value)
		:next(nullptr), key(key), value(value)
	{
	}

	HashLinkedNode* next;
	DataType key;
	DataType value;
};
// End of Class HashLinkedNode

/**
 * Template Class HashTable
 *
 * Implementation of hash table.
 */
template <class DataType, class HashAlgo>
class HashTable
{
public:

	explicit HashTable(int spacePow = 4)
		:ht(nullptr), algo(HashAlgo()), size(0), space(0x01 << spacePow), hashMask(0x00)
	{
		ht = new HashLinkedNode<DataType>*[space];
		hashMask = space - 1;
		for (int i = 0; i < space; ++i)
			ht[i] = nullptr;
	}

	explicit HashTable(const DataType& ht) = delete;

	~HashTable()
	{
		freeTable();
	}

	bool exists(const DataType& key) const
	{
		size_t hashCode = algo(key)& hashMask;
		HashLinkedNode<DataType>* list = ht[hashCode];
		if (!list)
			return false;

		while (list != nullptr)
		{
			if (list->key == key)
				return true;
			list = list->next;
		}
		return false;
	}

	DataType& get(const DataType& key)
	{
		size_t hashCode = algo(key)& hashMask;
		HashLinkedNode<DataType>* list = ht[hashCode];
		if (!list)
			throw std::runtime_error("Key not found");

		while (list != nullptr)
		{
			if (list->key == key)
				return list->value;
			list = list->next;
		}
		throw std::runtime_error("Key not found");
	}

	void set(const DataType& key, const DataType& value)
	{
		size_t hashCode = algo(key)& hashMask;
		HashLinkedNode<DataType>* list = ht[hashCode];

		if (!list)
		{
			list = new HashLinkedNode<DataType>(key, value);
			ht[hashCode] = list;
			++size;
			return;
		}

		HashLinkedNode<DataType>* head = list;
		while (list != nullptr)
		{
			if (list->key == key)
			{
				list->value = value;
				return;
			}
			list = list->next;
		}

		HashLinkedNode<DataType>* node = new HashLinkedNode<DataType>(key, value);
		node->next = head;
		ht[hashCode] = node;
		++size;
	}

	void del(const DataType& key)
	{
		size_t hashCode = HashAlgo()(key)& hashMask;
		HashLinkedNode<DataType>* list = ht[hashCode];
		if (!list)
			throw std::runtime_error("Key not found");

		HashLinkedNode<DataType>* prev = list;
		while (list != nullptr)
		{
			if (list->key == key)
			{
				if (ht[hashCode] == list)
					ht[hashCode] = nullptr;
				else
					prev->next = list->next;
				delete list;
				--size;
				return;
			}
			prev = list;
			list = list->next;
		}
		throw std::runtime_error("Key not found");
	}

	DataType& operator[](const DataType& key)
	{
		return get(key);
	}

	DataType& operator=(const DataType& ht) = delete;
private:
	HashLinkedNode<DataType>** ht;

	HashAlgo algo;

	int size;

	int space;

	size_t hashMask;

	void freeTable()
	{
		for (int i = 0; i < space; ++i)
		{
			if (!ht[i])
				continue;
			freeList(ht[i]);
		}
		delete[] ht;
	}

	void freeList(HashLinkedNode<DataType>* node)
	{
		if (node->next)
			freeList(node->next);
		delete node;
	}
};
// End of Class HashTable

/**
 * StrAlgo Class
 *
 * BKDR Hash Algorithm Implementation.
 */
class StrAlgo
{
public:
	size_t operator()(const std::string& str)
	{
		size_t hash = 0;
		for (std::string::size_type i = 0; i < str.size(); ++i)
		{
			hash = hash * 31 + str[i];	// or 131, 1313, ...
		}
		return hash;
	}
};
// End of Class StrAlgo

#endif
// End of Header Protection
