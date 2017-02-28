/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * storage/File.cpp
 * ----------------------------
 * File implementation.
 * =============================
 */

// Header files
#include "File.h"

#if FILE_STREAM_IMPLEMENT == 1
File::File(const char* path)
	:file(nullptr), _size(0), _sizeUpdated(false)
{
	// Existence check
	std::ifstream in_file(path, std::ios::binary);
	if (!in_file.is_open())
		createFile(path);
	in_file.close();

	file = new std::fstream(path, std::ios::in | std::ios::out | std::ios::binary);
	file->sync_with_stdio(false);
	file->exceptions(std::fstream::failbit | std::fstream::badbit);
}

File::~File()
{
	if (file)
		delete file;
}

bool File::isOpen() const
{
	return file->is_open();
}

size_t File::size()
{
	if (_sizeUpdated)
		return _size;

	file->seekg(0, std::ios::end);
	std::streamoff len = file->tellg();
	if (len == -1)
		error("Read file size failed");
	_size = static_cast<size_t>(len);
	_sizeUpdated = true;
	return _size;
}

char File::read(size_t pos)
{
	file->seekg(pos, std::ios::beg);
	char c = file->get();
	if (!file)
		error("Read char failed");
	return c;
}

char* File::readBytes(size_t start, size_t size)
{
	static int i = 0;
	++i;
	std::cout << "Reading: " << i << std::endl;
	file->seekg(start, std::ios::beg);
	char* buf = new char[size];
	try
	{
		file->read(buf, size);
		if (!file)
			error("Read bytes failed");
		return buf;
	}
	catch (...)
	{
		delete[] buf;
		throw;
	}
}

void File::readBytes(size_t start, size_t size, char* buf)
{
	file->seekg(start, std::ios::beg);
	file->read(buf, size);
	if (!file)
		error("Read bytes failed");
}

void File::write(size_t pos, char val)
{
	preparePos(pos);
	if (pos == size())
		_sizeUpdated = false;
	file->seekp(pos, std::ios::beg);
	file->put(val);
	if (!file)
		error("Write char failed");
}

void File::writeBytes(size_t start, size_t size, char* data)
{
	static int i = 0;
	++i;
	std::cout << "Writing: " << i << std::endl;
	preparePos(start);
	if (start >= this->size())
		_sizeUpdated = false;
	file->seekp(start, std::ios::beg);
	file->write(data, size);
	file->flush();
	if (!file)
		error("Write bytes failed");
}

void File::appendBytes(size_t size, char* data)
{
	_sizeUpdated = false;
	file->seekp(0, std::ios::end);
	file->write(data, size);
	file->flush();
	if (!file)
		error("Write bytes failed");
}

void File::clearState()
{
	file->clear();
}

void File::flush(){
	file->flush();
}

std::fstream* File::getStream()
{
	return file;
}

void File::createFile(const char* path)
{
	std::ofstream out_file(path, std::ios::binary);
	if (!out_file)
		error("Create file failed");
}

void File::preparePos(size_t pos)
{
	if (pos <= size())
		return;
	int appendLen = size() - pos;
	std::unique_ptr<char> data(new char[appendLen]);
	appendBytes(appendLen, data.get());
	data.release();
}
#else
File::File(const char* path)
	:file(nullptr)
{
	// Existence check
	FILE* in_file = nullptr;
	fopen_s(&in_file, path, "rb+");
	if (in_file == nullptr)
		createFile(path);
	else
		fclose(in_file);

	fopen_s(&file, path, "rb+");
}

File::~File()
{
	if (file)
		fclose(file);
}

bool File::isOpen() const
{
	return file != nullptr;
}

size_t File::size()
{
	if (_sizeUpdated)
		return _size;

	fseek(file, 0, SEEK_END);
	long len = ftell(file);
	if (len < 0)
		error("Read file size failed");
	_size = static_cast<size_t>(len);
	_sizeUpdated = true;
	return _size;
}

char File::read(size_t pos)
{
	//debugInfo("Reading char");
	if (!file)
		error("Read char failed");
	fseek(file, pos, SEEK_SET);
	char c = fgetc(file);
	return c;
}

char* File::readBytes(size_t start, size_t size)
{
	//static int i = 0;
	//debugInfo("Reading bytes");
	//debugInfo(++i);
	if (!file)
		error("Read bytes failed");
	fseek(file, start, SEEK_SET);
	char* buf = new char[size];
	try
	{
		if (fread(buf, sizeof(char), size, file) != size)
			error("Read bytes failed");
		return buf;
	}
	catch (...)
	{
		delete[] buf;
		throw;
	}
}

void File::readBytes(size_t start, size_t size, char* buf)
{
	//debugInfo("Reading bytes");
	if (!file)
		error("Read bytes failed");
	fseek(file, start, SEEK_SET);
	fread(buf, sizeof(char), size, file);
}

void File::write(size_t pos, char val)
{
	//debugInfo("Writing char");
	preparePos(pos);
	if (pos == size())
		_sizeUpdated = false;
	if (!file)
		error("Write char failed");
	fseek(file, pos, SEEK_SET);
	fputc(val, file);
}

void File::writeBytes(size_t start, size_t size, char* data)
{
	//static int i = 0;
	//debugInfo("Writing bytes");
	//debugInfo(++i);
	preparePos(start);
	if (start >= this->size())
		_sizeUpdated = false;
	if (!file)
		error("Write char failed");
	fseek(file, start, SEEK_SET);
	fwrite(data, sizeof(char), size, file);
}

void File::appendBytes(size_t size, char* data)
{
	//debugInfo("Appending bytes");
	_sizeUpdated = false;
	if (!file)
		error("Write bytes failed");
	fseek(file, 0, SEEK_END);
	fwrite(data, sizeof(char), size, file);
}

void File::clearState()
{
}

void File::flush(){
	fflush(file);
}

FILE* File::getFile()
{
	return file;
}

void File::createFile(const char* path)
{
	FILE* out_file = nullptr;
	fopen_s(&out_file, path, "wb");
	if (!out_file)
		error("Create file failed");
	fclose(out_file);
}

void File::preparePos(size_t pos)
{
	if (pos <= size())
		return;
	int appendLen = size() - pos;
	std::unique_ptr<char> data(new char[appendLen]);
	appendBytes(appendLen, data.get());
	data.release();
}
#endif
