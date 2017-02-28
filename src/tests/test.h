/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * tests/test.h
 * ----------------------------
 * Testing common header.
 * =============================
 */

// Header Protection
#ifndef TEST_TEST_H
#define TEST_TEST_H

// Header files
#include "../common.h"

// Testing switch
#define DBFILE_TEST
#define INDEXFILE_TEST
#define HASHTABLE_TEST
#define BTREE_TEST
#define DBHASH_TEST
#define DB_PERFORMANCE_TEST
#define DB_WRITE_TEST
#define DB_DELETE_TEST

// Testing function declaration
void test_dbfile();
void test_indexfile();
void test_hashtable();
void test_btree();
void test_dbhash();
void test_db_performance();
void test_db_write();
void test_db_delete();

#endif
// End of Header Protection
