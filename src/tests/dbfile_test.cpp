/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * tests/dbfile_test.cpp
 * ----------------------------
 * DbFile test cases.
 * =============================
 */

// Header files
#include "test.h"
#include "../storage/DbFile.h"

void test_dbfile()
{
#ifdef DBFILE_TEST
	remove("test.db");
	clock_t time = clock();
	DbFile file("test.db");
	DbRecord rec1 = file.newRecord("Hello", "World");
	DbRecord rec2 = file.newRecord("Hello1", "World1");
	DbRecord rec3 = file.newRecord("Hello2", "World2");
	DbRecord rec4 = file.newRecord("Hello3", "World3");
	DbRecord rec5 = file.newRecord("Hello4", "World4");
	file.delRecord(rec4);
	file.delRecord(rec2);
	/*std::string str;
	srand(static_cast<unsigned>(time));
	for (int i = 0; i < 32 * 1024; ++i)
	{
		str.push_back(rand() % 26 + 'a');
	}
	DbRecord recHuge = file.newRecord("huge", str.c_str(), Long);*/
	/*for (int i = 0; i < 81600; ++i)
	{
		DbRecord rec = file.newRecord("shortt", "testt9");
		//file.delRecord(rec);
	}*/
	DbRecord rec6 = file.newRecord("aaa", "bbb");
	file.shrinkRecord(rec6, "a", "");
	file.sync();
	clock_t over = clock();
	std::cout << (over - time) * 1000 / CLOCKS_PER_SEC << "ms" << std::endl;
#endif
}
