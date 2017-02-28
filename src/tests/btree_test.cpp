/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * tests/btree_test.cpp
 * ----------------------------
 * B-Tree test cases.
 * =============================
 */

// Header files
#include "test.h"
#include "../container/BTree.h"
#include <vector>

void test_btree()
{
#ifdef BTREE_TEST
	BTree<std::string> test(3000);
	//test.set("hello", "ok!");
	//puts(test.get("hello").c_str());
	////test.del("hello");
	//test.set("ahellow", "another test");
	//test.set("middle", "middle test1");
	//test.set("middle2", "middle test2");
	//test.set("middle3", "middle test3");
	//test.set("middle4", "middle test4");
	//test.set("middle5", "middle test5");
	//test.set("middle6", "middle test6");
	//puts(test.get("hello").c_str());
	//puts(test.get("ahellow").c_str());
	//puts(test.get("middle").c_str());
	//puts(test.get("middle2").c_str());
	//puts(test.get("middle3").c_str());
	//puts(test.get("middle4").c_str());
	//puts(test.get("middle5").c_str());
	//puts(test.get("middle6").c_str());
	//test.set("middle6", "middle test9");
	//puts(test.get("middle6").c_str());

	int nrec = 10000;
	int recKeyLen = 20;
	int recValueLen = 40;

	// Init test data
	std::vector<std::string> keys(nrec);
	std::vector<std::string> values(nrec);
	for (int i = 0; i < nrec; ++i)
	{
		std::string randomKey;
		int keyLen = recKeyLen + rand() % 5;
		for (int j = 0; j < keyLen; ++j)
			randomKey += ('A' + (rand() % 58));
		keys[i] = randomKey;

		std::string randomValue;
		int valueLen = recValueLen + rand() % 5;
		for (int j = 0; j < valueLen; ++j)
			randomValue += ('A' + (rand() % 58));
		values[i] = randomValue;
	}

	// Testing start
	for (int i = 0; i < nrec; ++i)
	{
		test.set(keys[i], values[i]);
	}

	for (int i = 0; i < nrec; ++i)
	{
		if (test.get(keys[i]) != values[i])
			std::cerr << "Test failed on " << keys[i] << std::endl;
	}

	debugInfo("Test succeed");
#endif
}
