/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * storage/common.h
 * ----------------------------
 * Header for common libraries for storage.
 * =============================
 */

// Header Protection
#ifndef STORAGE_COMMON_H
#define STORAGE_COMMON_H

// wsize_t
typedef unsigned long long wsize_t;

// RecordType Enumeration
enum RecordType
{
	Normal, Long, LongExtended, Deleted
};

#endif
// End of Header Protection