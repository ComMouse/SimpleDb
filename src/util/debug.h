/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * util/debug.h
 * ----------------------------
 * Debug-related functions implementation.
 * =============================
 */

// Header Protection
#ifndef UTIL_DEBUG_H
#define UTIL_DEBUG_H

// Header files
#include "../common.h"

#if DEBUG_MODE == 1
#include <cassert>
#include <sstream>
#endif

// Debug marcos
#if DEBUG_MODE == 1
#define DEBUG_THROW_EXCEPTION throw
#else
#define DEBUG_THROW_EXCEPTION
#endif

// Debug severity enum
enum Severity
{
	Info, Warning, Error
};

// Set debug stream
inline void debugStream(std::ostream& stream)
{
	std::cerr.rdbuf(stream.rdbuf());
}

// Get debug severity
inline char* debugSeverity(Severity severity)
{
	switch (severity)
	{
	case Info:
		return "[INFO]";
	case Warning:
		return "[WARNING]";
	case Error:
		return "[ERROR]";
	default:
		return "";
	}
}

// Print debug info
inline void debugInfo(const char* str, Severity severity = Info)
{
#if DEBUG_MODE == 1
	std::cerr << debugSeverity(severity) << ' ' << str << std::endl;
#endif
}

inline void debugInfo(const std::string& str, Severity severity = Info)
{
#if DEBUG_MODE == 1
	debugInfo(str.c_str(), severity);
#endif
}

inline void debugInfo(int str, Severity severity = Info)
{
#if DEBUG_MODE == 1
	std::cerr << debugSeverity(severity) << ' ' << str << std::endl;
#endif
}

// Assert macros
#if DEBUG_MODE == 1
// Assertion
#define ASSERT(val) assert(val)
// Equal assertion
#define ASSERT_EUQALS(a, b) assert(a == b)
// Unequal assertion
#define ASSERT_NOT_EQUALS(a, b) assert(a != b)
#else
// Empty definitions
#define ASSERT(val)
#define ASSERT_EUQALS(a, b)
#define ASSERT_NOT_EQUALS(a, b)
#endif

#endif
// End of Header Protection
