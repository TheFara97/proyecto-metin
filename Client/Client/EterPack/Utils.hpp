#pragma once
#include <cstdint>

#define _4CC(b0, b1, b2, b3) \
	(uint32_t(uint8_t(b0)) | (uint32_t(uint8_t(b1)) << 8) | \
	(uint32_t(uint8_t(b2)) << 16) | (uint32_t(uint8_t(b3)) << 24))

enum FileFlags
{
	kFileFlagLz4 = 1 << 0,
};

// generated using: random.randint(0, 1 << 64)
static const uint64_t kFilenameMagic = 7420643061387133436ull;
static const uint64_t kFilenameKeyMagic1 = 8373488899364292976ull;
static const uint64_t kFilenameKeyMagic2 = 9627464534385349114ull;

#pragma pack(push, 1)
struct VirtualHeader
{
	static const uint32_t kFourCc = _4CC('¦', '§', '@', '¯');
	static const uint32_t kVersion = 1;

	uint32_t fourCc;
	uint32_t version;

	// VirtualEntry array
	uint32_t fileInfoOffset;
	uint32_t fileCount;
};

struct VirtualEntry
{
	// XXH64 hash of the "normalized" filename
	// see: VFSFilename::Set
	uint64_t filenameHash;

	// see FileFlags
	uint32_t flags;

	// Offset of the file - relative to the archive's beginning.
	uint32_t offset;

	// Size of the file - on disk (i.e compressed, etc.)
	uint32_t diskSize;

	// Size of the unpacked file
	uint32_t size;
};
#pragma pack(pop)