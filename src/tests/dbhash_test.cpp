/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * tests/hashdb_test.cpp
 * ----------------------------
 * HashTable databasetest cases.
 * =============================
 */

// Header files
#include "test.h"
#include "../db/HashTableDb.h"

void test_dbhash()
{
#ifdef DBHASH_TEST
	//remove("mydata.db");
	//remove("mydata.idx");
	HashTableDb test("mydata");
	debugInfo("Testing get and set");
	test.set("hello2", "ok!");
	debugInfo(test.get("hello2"));
	test.set("hello", "another test");
	debugInfo("Testing del");
	test.del("hello");
	try
	{
		debugInfo(test.get("hello").c_str());
		debugInfo("del test failed");
	}
	catch (HashTableDb::NotFound)
	{
		debugInfo("del test success");
	}
	test.set("hello", "another test");

	clock_t time = clock();
	srand(static_cast<unsigned>(time));
	std::string random;
	for (int i = 0; i < 40000; ++i)
	{
		random += ('a' + (rand() % 26));
	}
	debugInfo("Testing long key");
	test.set("long key test", random.c_str());
	
	//debugInfo("Raw: " + random);
	std::cerr << random.size() << std::endl;
	//debugInfo("Result: " + test.get("long key test"));
	std::cerr << test.get("long key test").size() << std::endl;
	debugInfo(test.get("long key test") == random ? "Success" : "Failed");

	debugInfo("Hash conflict test");
	test.set("ZPQ", "Hello, world!");
	test.set("YoQ", "Goodbye, world!");

	debugInfo(test.get("YoQ"));
	debugInfo(test.get("ZPQ"));
	test.del("YoQ");
	debugInfo(test.get("ZPQ"));
#endif
}
