/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * container/BTree.h
 * ----------------------------
 * B-Tree implementation.
 * =============================
 */

// Header Protection
#ifndef CONTAINER_BTREE_H
#define CONTAINER_BTREE_H

// Header files
#include "../common.h"

/**
* Template Class BTreeSibling
*
* Implementation of sibling node in a B-Tree.
*/
template <class DataType>
class BTreeSibling
{
public:
	BTreeSibling(const DataType& key, const DataType& value)
		:key(key), value(value), deleteFlag(false)
	{
	}

	bool isDeleted() const
	{
		return deleteFlag;
	}

	DataType key;
	DataType value;

	bool deleteFlag;
};
// End of Class BTreeSibling

/**
* Template Class BTreeNode
*
* Implementation of node in a B-Tree.
*/
template <class DataType>
class BTreeNode
{
public:
	BTreeNode(int order)
		:children(nullptr), leafFlag(true), siblings(nullptr), order(order), count(0)
	{
		siblings = new BTreeSibling<DataType>*[2 * order - 1];
		if (siblings)
			memset(siblings, NULL, sizeof(BTreeSibling<DataType>*) * (2 * order - 1));

		children = new BTreeNode*[2 * order];
		if (children)
			memset(children, NULL, sizeof(BTreeNode*) * (2 * order));
	}

	BTreeNode(const BTreeNode& node) = delete;

	~BTreeNode()
	{
		if (siblings)
		{
			for (int i = 0; i < count; ++i)
				if (siblings[i])
				{
					//printf("Deleted: %x %s\n", siblings[i], siblings[i]->key.c_str());
					delete siblings[i];
				}

			//printf("Deleted Siblings: %x\n", siblings);
			delete[] siblings;
		}

		if (children)
			delete[] children;
	}

	BTreeSibling<DataType>* getSibling(int index)
	{
		return siblings[index];
	}

	BTreeNode* getChildren(int index)
	{
		return children[index];
	}

	void push(BTreeSibling<DataType>* sibling)
	{
		if (isFull())
			throw std::runtime_error("Cannot push siblings to a full node");

		int i = 0;
		for (i = 0; i < count; ++i)
		{
			if (siblings[i]->key > sibling->key)
				break;
		}
		for (int j = count; j > i; --j)
		{
			siblings[j] = siblings[j - 1];
		}
		siblings[i] = sibling;
		++count;
	}

	bool isFull() const
	{
		return (count >= (2 * order - 1));
	}

	bool isLeaf() const
	{
		return leafFlag;
	}

	BTreeNode& operator=(const BTreeNode& node) = delete;

	BTreeNode** children;
	bool leafFlag;

	BTreeSibling<DataType>** siblings;
	int order;
	int count;
};
// End of Class BTreeNode

/**
* Template Class BTreeNode
*
* Implementation of B-Tree.
*/
template <class DataType>
class BTree
{
public:

	typedef BTreeNode<DataType> Node;
	typedef BTreeSibling<DataType> Sibling;

	explicit BTree(int order)
		:order(order)
	{
		root = new Node(order);
	}

	explicit BTree(const DataType& bt) = delete;

	~BTree()
	{
		if (root)
			freeNode(root);
	}

	bool exists(const DataType& key) const
	{
		return getSibling(key) != nullptr;
	}

	DataType& get(const DataType& key)
	{
		Sibling* sibling = getSibling(key);
		if (!sibling)
			throw std::runtime_error("Key not found");
		return sibling->value;
	}

	void set(const DataType& key, const DataType& value)
	{
		Sibling* sibling = getSiblingOrCreate(key);
		sibling->value = value;
	}

	void del(const DataType& key)
	{
		Sibling* sibling = getSibling(key);
		if (!sibling)
			throw std::runtime_error("Key not found");

		sibling->deleteFlag = true;
	}

	DataType& operator[](const DataType& key)
	{
		return get(key);
	}

	DataType& operator=(const DataType& bt) = delete;
private:
	Node* root;
	int order;

	void freeNode(Node* node)
	{
		if (!node)
			return;

		if (node->children)
			for (int i = 0; i < node->count + 1; ++i)
				freeNode(node->children[i]);
		
		delete node;
	}

	Sibling* getSibling(const DataType& key)
	{
		return getSiblingRecursion(key, root);
	}

	Sibling* getSiblingRecursion(const DataType& key, Node* node)
	{
		int i = 0;
		for (; i < node->count; ++i)
			if (node->getSibling(i)->key >= key)
				break;

		Sibling* sibling = nullptr;
		if (i < node->count)
			sibling = node->getSibling(i);
		if (i < node->count && key == sibling->key && !sibling->isDeleted())
			return sibling;
		if (node->isLeaf())
			return nullptr;
		return getSiblingRecursion(key, node->getChildren(i));
	}

	Sibling* getSiblingOrCreate(const DataType& key)
	{
		if (root->isFull())
		{
			Node* oldRoot = root;
			Node* newRoot = new Node(order);
			root = newRoot;
			root->leafFlag = false;
			root->count = 0;
			root->children[0] = oldRoot;
			doSplit(root, 0);
		}
		return getSiblingOrCreateRecursion(key, root);
	}

	Sibling* getSiblingOrCreateRecursion(const DataType& key, Node* node)
	{
		if (!node->isLeaf())
		{
			int i = 0;
			for (; i < node->count; ++i)
				if (node->getSibling(i)->key >= key)
					break;

			Sibling* sibling = nullptr;
			if (i < node->count)
				sibling = node->getSibling(i);
			if (i < node->count && key == sibling->key && !sibling->isDeleted())
				return sibling;

			if (needSplit(node, i))
			{
				doSplit(node, i);
				if (key > node->getSibling(i)->key)
					++i;
			}
			return getSiblingOrCreateRecursion(key, node->getChildren(i));
		}
		else
		{
			int i = 0;
			for (; i < node->count; ++i)
				if (node->getSibling(i)->key == key && !node->getSibling(i)->isDeleted())
					return node->getSibling(i);

			Sibling* newSibling = new Sibling(key, DataType());
			node->push(newSibling);
			return newSibling;
		}
	}

	Sibling* createSibling(const DataType& key, Node* node, Node* parent)
	{
		if (node == nullptr)
		{
			node = new Node(order);
			node->parent = parent;
		}

		Sibling* sibling = new Sibling(key, DataType());
		node->push(sibling);
		return sibling;
	}

	bool needSplit(Node* node, int index)
	{
		return node->getChildren(index)->isFull();
	}

	void doSplit(Node* node, int index)
	{
		Node* newChildNode = new Node(order);
		Node* childNode = node->getChildren(index);
		newChildNode->leafFlag = childNode->leafFlag;
		newChildNode->count = order - 1;

		for (int j = 0; j < order - 1; ++j)
			newChildNode->siblings[j] = childNode->siblings[j + order];

		if (!childNode->isLeaf())
			for (int j = 0; j < order; ++j)
				newChildNode->children[j] = childNode->children[j + order];
		childNode->count = order - 1;
		
		for (int j = node->count; j >= index + 1; --j)
			node->children[j + 1] = node->children[j];
		node->children[index + 1] = newChildNode;

		for (int j = node->count - 1; j >= index; --j)
			node->siblings[j + 1] = node->siblings[j];
		node->siblings[index] = childNode->siblings[order - 1];
		++node->count;
	}
};
// End of Class BTree

#endif
// End of Header Protection
