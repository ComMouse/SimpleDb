/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * storage/File.h
 * ----------------------------
 * File declaration.
 * =============================
 */

// Header Protection
#ifndef STORAGE_FILE_H
#define STORAGE_FILE_H

// Header files
#include "../common.h"

#if FILE_STREAM_IMPLEMENT == 1
/**
 * Class File
 *
 * File wrapper.
 */
class File
{
public:
	// Constructor
	File(const char* path);
	File(const File& file) = delete;

	// Destructor
	~File();

	// Whether opening succeeds
	bool isOpen() const;

	// Get file size
	size_t size();


	// Read a byte
	char read(size_t pos);

	// Read bytes
	char* readBytes(size_t start, size_t size);
	void readBytes(size_t start, size_t size, char* buf);


	// Write a byte
	void write(size_t pos, char val);

	// Write bytes
	void writeBytes(size_t start, size_t size, char* data);

	// Append bytes to end of file
	void appendBytes(size_t size, char* data);


	// Clear stream states
	void clearState();

	// Flush buffer
	void flush();


	// Get file stream
	std::fstream* getStream();

	// Disable copy assignment operator
	File& operator=(const File& file) = delete;
private:
	// File stream
	std::fstream* file;

	// Cached file size
	size_t _size;

	// Switch of size update
	bool _sizeUpdated;

	// Create file
	void createFile(const char* path);

	// Prepare data for pos
	void preparePos(size_t pos);
};

#else
/**
* Class File
*
* File wrapper.
*/
class File
{
public:
	// Constructor
	File(const char* path);
	File(const File& file) = delete;

	// Destructor
	~File();

	// Whether opening succeeds
	bool isOpen() const;

	// Get file size
	size_t size();


	// Read a byte
	char read(size_t pos);

	// Read bytes
	char* readBytes(size_t start, size_t size);
	void readBytes(size_t start, size_t size, char* buf);


	// Write a byte
	void write(size_t pos, char val);

	// Write bytes
	void writeBytes(size_t start, size_t size, char* data);

	// Append bytes to end of file
	void appendBytes(size_t size, char* data);


	// Clear stream states
	void clearState();

	// Flush buffer
	void flush();


	// Get file stream
	FILE* getFile();

	// Disable copy assignment operator
	File& operator=(const File& file) = delete;
private:
	// File ptr
	FILE* file;

	// Cached file size
	size_t _size;

	// Switch of size update
	bool _sizeUpdated;

	// Create file
	void createFile(const char* path);

	// Prepare data for pos
	void preparePos(size_t pos);
};
#endif

#endif
// End of Header Protection
