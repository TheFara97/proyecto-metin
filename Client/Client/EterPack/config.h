#pragma once
#include <array>
#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <../../Extern/include/xxh3.h>
#include <../../Extern/include/xxhash.h>
#include <CompileTimeValue.hpp>


#if !defined(USE_ZSTD) && !defined(USE_LZ4)
#define USE_LZ4
#endif

#ifdef USE_ZSTD
#include <zstd-1.5.7/lib/zstd.h>
#endif

#ifdef USE_LZ4
#include <lz4.h>
#include <lz4hc.h>
#endif
#include <xorstr.hpp>

#if defined(_M_X64) || defined(__x86_64__)
#define WIN_X64 1
using TPackSize = uint32_t;
#else
#define WIN_X64 0
using TPackSize = uint32_t;
#endif

//#pragma pack(push, 1)
//struct TPackFileHeader {
//	uint32_t entry_num;
//	uint32_t data_begin;
//	uint8_t iv[CryptoPP::AES::BLOCKSIZE];
//};
//
//struct TPackFileEntry {
//	PROTECTED_VALUE_STATIC(uint64_t, file_name_hash, "file_name_hash_SEED261");
//	char file_name[261];
//	PROTECTED_VALUE_STATIC(uint32_t, offset, "offset_SEED161211");
//	PROTECTED_VALUE_STATIC(uint32_t, file_size, "file_size_SEED19045");
//	PROTECTED_VALUE_STATIC(uint32_t, compressed_size, "compressed_size_SEED12617");
//	PROTECTED_VALUE_STATIC(uint8_t, encryption, "encryption_flag_SEED1267111");
//	uint8_t iv[CryptoPP::AES::BLOCKSIZE];
//};
//#pragma pack(pop)
#pragma pack(push, 1)
struct TPackFileHeader {
	PROTECTED_VALUE_STATIC(uint32_t, entry_num, "fentry_num1_SEED261");
	uint32_t data_begin;
	uint8_t iv[16];
};

struct TPackFileEntry {

	PROTECTED_VALUE_STATIC(uint64_t, file_name_hash, "file_name_hash_SEED261");
	char file_name[522];
	PROTECTED_VALUE_STATIC(uint32_t, offset, "offset_SEED161211");
	PROTECTED_VALUE_STATIC(uint32_t, file_size, "file_size_SEED19045");
	PROTECTED_VALUE_STATIC(uint32_t, compressed_size, "compressed_size_SEED12617");
	PROTECTED_VALUE_STATIC(uint8_t, encryption, "encryption_flag_SEED1267111");
	uint8_t iv[16];
};
#pragma pack(pop)

static constexpr const char* PACK_MAGIC_ASCII = ("KoMaR1911");
static constexpr size_t PACK_MAGIC_LEN = 9; 
static constexpr const char* PACK_SERVER_NAME = ("1911");


class CPack;
using TPackFile = std::vector<uint8_t>;
using TPackFileMapEntry = std::pair<std::shared_ptr<CPack>, TPackFileEntry>;
using TPackFileMap = std::unordered_map<uint64_t, TPackFileMapEntry>;