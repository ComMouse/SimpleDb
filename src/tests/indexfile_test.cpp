/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * tests/indexfile_test.cpp
 * ----------------------------
 * IndexFile test cases.
 * =============================
 */

// Header files
#include "test.h"
#include "../storage/IndexFile.h"

size_t hash(const std::string& str)
{
	size_t hash = 0;
	for (std::string::size_type i = 0; i < str.size(); ++i)
	{
		hash = hash * 31 + str[i];	// or 131, 1313, ...
	}
	return hash & 0x3F;
}

std::string getRandomStr(int size)
{
	std::string str;
	for (int i = 0; i < size; ++i)
	{
		str.push_back(rand() % 26 + 'a');
	}
	return str;
}

void test_indexfile()
{
#ifdef INDEXFILE_TEST
	remove("test.idx");
	clock_t time = clock();
	srand(static_cast<unsigned>(time));
	IndexFile file("test.idx", 1024);
	//file.add(0x05, "good", 3, 0x19);
	for (int i = 0; i < 40000; ++i)
	{
		std::string random = "xxxxxx";
		random += ('a' + i % 26);
		file.add(hash(random), random.c_str(), i, i % 256);
		file.get(hash(random), random.c_str());
		if (i % 100 == 0)
			file.del(hash(random), random.c_str());
	}
	debugInfo("Testing get");
	file.get(hash("xxxxxxc"), "xxxxxxc");
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
	file.sync();
	clock_t over = clock();
	std::cout << (over - time) * 1000 / CLOCKS_PER_SEC << "ms" << std::endl;
#endif
}
