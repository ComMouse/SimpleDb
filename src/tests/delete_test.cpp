/**
* Programming II Project
* SE106 - 2015 Spring
*
* =============================
* tests/delete_test.cpp
* ----------------------------
* Database performance test cases.
* =============================
*/

// Header files
#include "test.h"
#include "../db/HashTableDb.h"
#include <vector>

void test_db_delete()
{
#ifdef DB_DELETE_TEST
	// Clear previous test files
	remove("performance_test.db");
	remove("performance_test.idx");

	// Settings
	int tablePow = 5;	// Table size (2 ** tablePow)
	int nrec = 100000;
	int recKeyLen = 20;
	int recValueLen = 40;
	clock_t time = clock();
	srand((unsigned)time);

	// Init test data
	std::vector<std::string> keys(nrec);
	std::vector<std::string> values(nrec);
	std::vector<int> delOrder(nrec);
	std::vector<int> temp;
	for (int i = 0; i < nrec; ++i)
	{
		temp.push_back(i);
	}

	for (int i = 0; i < nrec; ++i)
	{
		int j = rand() % temp.size();
		delOrder[i] = temp[j];
		temp.erase(temp.begin() + j);
	}

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

	// Init database
	HashTableDb db("performance_test", tablePow);

	// Testing start
	// Step 1
	for (int i = 0; i < nrec; ++i)
	{
		db.set(keys[i], values[i]);
	}

	clock_t startTime = clock();
	for (int i = 0; i < nrec; ++i)
	{
		db.del(keys[delOrder[i]]);
	}

	// Info
	clock_t endTime = clock();
	debugInfo("Finished");
	debugInfo("Time (ms):");
	debugInfo((endTime - startTime) * 1000 / CLOCKS_PER_SEC);
#endif
}
