/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * main.cpp
 * ----------------------------
 * Bootstrap file
 * =============================
 */

// Header files
#include "common.h"
#include "tests/test.h"

void do_main()
{
	//test_hashtable();
	//test_btree();
	//test_dbfile();
	//test_indexfile();
	//test_dbhash();
	test_db_performance();
	//test_db_write();
	//test_db_delete();
}

int main()
{
	try
	{
		do_main();
	}
	catch (std::exception& e)
	{
		debugInfo(std::string("Program exited with an exception: ") + e.what(), Error);
		DEBUG_THROW_EXCEPTION;
	}

	system("pause");
	return 0;
}