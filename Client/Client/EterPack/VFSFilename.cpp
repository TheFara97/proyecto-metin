#include "stdafx.h"
#include "VFSFilename.hpp"
#include "Utils.hpp"

#include "Modules/xxhash.h"
#include <cstring>

namespace
{
	char ToLowerAscii(char ch)
	{
		if (ch <= 'Z' && ch >= 'A')
			return ch - ('Z' - 'z');

		return ch;
	}
}

FilenameWrapper::FilenameWrapper(const std::string& path)
{
	Set(path.data(), path.length());
}

FilenameWrapper::FilenameWrapper(std::string_view path)
{
	Set(path.data(), path.length());
}

FilenameWrapper::FilenameWrapper(const char* path)
{
	Set(path, std::strlen(path));
}

FilenameWrapper& FilenameWrapper::operator=(const std::string& path)
{
	Set(path.data(), path.length());
	return *this;
}

FilenameWrapper& FilenameWrapper::operator=(std::string_view path)
{
	Set(path.data(), path.length());
	return *this;
}

FilenameWrapper& FilenameWrapper::operator=(const char* path)
{
	Set(path, (uint32_t)std::strlen(path));
	Set(path, std::strlen(path));
	return *this;
}

void FilenameWrapper::Set(const char* path, uint32_t length)
{
	m_path.resize(length);
	for (uint32_t i = 0; i < length; ++i) 
	{
		uint8_t ch = path[i];
		if (ch == '\\')
			ch = '/';
		else
			ch = ToLowerAscii(ch);

		m_path[i] = ch;
	}
	m_hash = XXH64(m_path.data(), length, kFilenameMagic);
}
uint64_t FilenameWrapper::GetHash() const
{
	return m_hash;
}
const std::string& FilenameWrapper::GetPath() const
{
	return m_path;
}
