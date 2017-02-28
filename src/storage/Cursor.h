/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * storage/Cusror.h
 * ----------------------------
 * Cusror declaration.
 * =============================
 */

// Header Protection
#ifndef STORAGE_CURSOR_H
#define STORAGE_CURSOR_H

// Header files
#include "../common.h"

/**
 * Template Class Cursor
 *
 * Cursor is a pointer mapping on the real position in a disk file.
 * Using cursors can help hide the details of data storage.
 */
template <class File, class DataType = File::unitType>
class Cursor
{
public:
	Cursor(File* file = nullptr, size_t pos = 0)
		:file(file), _pos(pos) {}
	Cursor(File& file, size_t pos = 0)
		:file(&file), _pos(pos) {}

	size_t pos() const { return _pos; }

	bool operator==(const Cursor& cursor) const { return _pos == cursor._pos; }
	bool operator!=(const Cursor& cursor) const { return !(*this == cursor); }
	bool operator<(const Cursor& cursor) const { return _pos < cursor._pos; }
	bool operator<=(const Cursor& cursor) const { return _pos <= cursor._pos; }
	bool operator>(const Cursor& cursor) const { return !(*this <= cursor); }
	bool operator>=(const Cursor& cursor) const { return !(*this < cursor); }

	DataType& operator*()
	{
		return file->get(_pos);
	}
	DataType* operator->()
	{
		return file->getPtr(_pos);
	}
	DataType* ptr()
	{
		return file->getPtr(_pos);
	}

	Cursor operator+(size_t offset) { return Cursor(_pos + offset); }
	Cursor& operator+=(size_t offset) { _pos += offset; return *this; }

	Cursor operator-(size_t offset) { return Cursor(_pos - offset); }
	Cursor& operator-=(size_t offset) { _pos -= offset; return *this; }

	Cursor& operator++() { *this += 1; return *this; }
	Cursor operator++(int i) { Cursor tmp(_pos); *this += 1; return tmp; }

	Cursor& operator--() { *this -= 1; return *this; }
	Cursor operator--(int i) { Cursor tmp(_pos); *this -= 1; return tmp; }
private:
	size_t _pos;

	File* file;
};
// End of Class Cursor

#endif
// End of Header Protection
