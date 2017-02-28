/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * tests/hashtable_test.cpp
 * ----------------------------
 * HashTable test cases.
 * =============================
 */

// Header files
#include "test.h"
#include "../container/HashTable.h"

void test_hashtable()
{
#ifdef HASHTABLE_TEST
	HashTable<std::string, StrAlgo> test;
	test.set("hello", "ok!");
	puts(test.get("hello").c_str());
	test.del("hello");
	test.set("hello", "another test");
	puts(test.get("hello").c_str());
#endif
}
