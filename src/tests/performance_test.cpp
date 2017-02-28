/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * tests/performance_test.cpp
 * ----------------------------
 * Database performance test cases.
 * =============================
 */

// Header files
#include "test.h"
#include "../db/HashTableDb.h"
#include <vector>

void test_db_performance()
{
#ifdef DB_PERFORMANCE_TEST
	// Clear previous test files
	remove("performance_test.db");
	remove("performance_test.idx");

	// Settings
	int tablePow = 5;	// Table size (2 ** tablePow)
	int nrec = 10000;
	int recKeyLen = 20;
	int recValueLen = 40;
	clock_t time = clock();
	srand((unsigned)time);
	int writeCursor = 0;
	int readCursor = 0;
	int recSize = 0;

	// Init test data
	std::vector<std::string> keys(nrec * 8);
	std::vector<std::string> values(nrec * 8);
	for (int i = 0; i < nrec * 8; ++i)
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
	// Step 1
	for (int i = 0; i < nrec; ++i)
	{
		db.set(keys[i], values[i]);
		++writeCursor;
	}
	recSize = writeCursor;
	
	// Step 2
	for (int i = 0; i < nrec; ++i)
	{
		try
		{
			++readCursor;
			db.get(keys[i]);
		}
		catch (HashTableDb::NotFound)
		{
			//debugInfo("Not found when getting");
		}
		
	}

	// Step 3
	for (int i = 0; i < nrec * 5; ++i)
	{
		try
		{
			++readCursor;
			db.get(keys[rand() % nrec]);
		}
		catch (HashTableDb::NotFound)
		{
			//debugInfo("Not found when getting");
		}
		if (i % 37 == 1)
		{
			try
			{
				++readCursor;
				db.del(keys[rand() % nrec]);
			}
			catch (HashTableDb::NotFound)
			{
				//debugInfo("Not found when deleting");
			}
			--recSize;
		}
		if (i % 11 == 1)
		{
			++readCursor;
			int recIndex = writeCursor++;
			db.set(keys[recIndex], values[recIndex]);
			db.get(keys[recIndex]);
			++recSize;
		}
		if (i % 17 == 1)
		{
			// TODO
			int recIndex = rand() % writeCursor;
			db.set(keys[recIndex], values[rand() % nrec * 6]);
		}
	}

	// Step 4
	for (int i = 0; i < writeCursor; ++i)
	{
		try
		{
			++readCursor;
			db.del(keys[i]);
		}
		catch (HashTableDb::NotFound)
		{
			//debugInfo("Not found when deleting:");
			//debugInfo(keys[i]);
		}
		for (int j = 0; j < 10; j++)
		{
			try
			{
				++readCursor;
				db.get(keys[rand() % writeCursor]);
			}
			catch (HashTableDb::NotFound)
			{
				//debugInfo("Not found when getting");
			}
		}
	}

	// Info
	clock_t endTime = clock();
	debugInfo("Finished");
	debugInfo("Time (ms):");
	debugInfo((endTime - startTime) * 1000 / CLOCKS_PER_SEC);
#endif
}
