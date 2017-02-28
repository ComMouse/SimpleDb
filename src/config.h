/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * config.h
 * ----------------------------
 * Configuration handler.
 * =============================
 */

// Header Protection
#ifndef CONFIG_H
#define CONFIG_H

// Header files
#include "common.h"

/**
 * Project config
 */

// Maximum cache space
#define MAX_CACHE_SPACE 256 * 1024 * 1024

// Maximum cache space limit switch
#define MAX_CACHE_SPACE_SWITCH 0

// Maximum in-memory page count
#define MAX_CACHE_PAGE 16 * 1024

// Maximum in-memory page limit switch
#define MAX_CACHE_PAGE_SWITCH 0

// Page size
#define PAGE_SIZE 64 * 1024

// Debug mode switch
#define DEBUG_MODE 1

// fstream/stdio switch
#define FILE_STREAM_IMPLEMENT 0

// End of project config

#endif
// End of Header Protection
