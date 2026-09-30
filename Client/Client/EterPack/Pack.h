#pragma once
#include "config.h"
#include <CompileTimeValue.hpp>
#include <cstdio>
#include <runtime_xorstr.hpp>
#include <string>
#include <vector>
#include <memory>

#if WIN_X64
#include <mio/mmap.hpp>
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

class CPack : public std::enable_shared_from_this<CPack> {
public:
	CPack() = default;
	~CPack();

	RX_FORCEINLINE bool Open(const char* path, TPackFileMap& entries) {
		rx::runtime_xorstr<MAX_PATH> encrypted_path(path);
		return Open_real(encrypted_path, entries);
	}

	RX_FORCEINLINE bool GetFile(const TPackFileEntry& entry, std::shared_ptr<const TPackFile>& result) {
		return GetFile_real(entry, result);
	}

private:
	bool Open_real(rx::runtime_xorstr<MAX_PATH>& path, TPackFileMap& entries);
	bool GetFile_real(const TPackFileEntry& entry, std::shared_ptr<const TPackFile>& result);

private:
	TPackFileHeader m_header;
	std::string m_file_path;

	static std::vector<uint8_t> s_scratch_buffer;

#if WIN_X64
	mio::mmap_source m_file;
#else
	FILE* m_file{ nullptr };
	std::vector<uint8_t> m_read_buffer;
	TPackSize m_buffer_start_offset{ 0 };
	static constexpr TPackSize READ_BUFFER_SIZE = 4 * 1024 * 1024;
#endif
};