/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * util/error.h
 * ----------------------------
 * Error handling functions.
 * =============================
 */

// Header Protection
#ifndef UTIL_ERROR_H
#define UTIL_ERROR_H

// Header files
#include "../common.h"

inline void error(const char* msg)
{
	debugInfo(msg, Error);
	throw std::runtime_error(msg);
}

#endif
// End of Header Protection
