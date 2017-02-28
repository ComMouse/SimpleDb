/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * tests/write_test.cpp
 * ----------------------------
 * Database performance test cases.
 * =============================
 */

// Header files
#include "test.h"
#include "../db/HashTableDb.h"
#include <vector>

void test_db_write()
{
#ifdef DB_WRITE_TEST
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
	int writeCursor = 0;
	int readCursor = 0;
	int recSize = 0;

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

	// Init database
	HashTableDb db("performance_test", tablePow);
	clock_t startTime = clock();

	// Testing start
	for (int i = 0; i < nrec; ++i)
	{
		db.set(keys[i], values[i]);
		++writeCursor;
	}
	recSize = writeCursor;

	// Info
	clock_t endTime = clock();
	debugInfo("Finished");
	debugInfo("Time (ms):");
	debugInfo((endTime - startTime) * 1000 / CLOCKS_PER_SEC);
#endif
}
