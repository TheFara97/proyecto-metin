#include "stdafx.h"
#include "VFSArchive.hpp"
#include "VFSBuffer.hpp"

#include "MappedFile.hpp"
#include "XTea.hpp"

#include "Modules/xxhash.h"
#include "Modules/lz4.h"
#include "../EterBase/Debug.h"

bool VFSArchive::Create(const std::string& filename, VirtualFileDict& dict)
{
	if (!m_file.Create(filename, CFileBase::FILEMODE_READ))
		return false;

	VirtualHeader hdr;
	if (!m_file.Read(&hdr, sizeof(hdr)))
		return false;

	if (hdr.fourCc != VirtualHeader::kFourCc)
		return false;

	if (hdr.version != VirtualHeader::kVersion)
		return false;

	m_file.Seek(hdr.fileInfoOffset);

	for (uint32_t i = 0; i != hdr.fileCount; ++i) 
	{
		VirtualEntry e;
		if (!m_file.Read(&e, sizeof(e)))
			return false;

		const auto it = dict.find(e.filenameHash);
		if (it != dict.end())
		{
			it->second = std::make_pair(this, e);
		}
		else
		{
			dict.emplace(e.filenameHash, std::make_pair(this, e));
		}
	}

	return true;
}

bool VFSArchive::Get(const std::string& path, const VirtualEntry& entry, CVirtualFile& fp)
{
	fp.Create(entry.size);

	TempOrOutputBuffer diskData(fp.GetCurrentSeekPoint());

	// If the file is compressed, we'll always need a temporary buffer.
	const bool needsTempBuffer = !!(entry.flags & kFileFlagLz4);
	if (needsTempBuffer)
		diskData.MakeTemporaryBuffer(entry.diskSize);

	m_file.Seek(entry.offset);

	if (!m_file.Read(diskData.ptr, entry.diskSize)) 
	{
		TraceError("Failed to read %s", path.c_str());
		return false;
	}

	if (entry.flags & kFileFlagLz4) 
	{
		const auto r = LZ4_decompress_fast(reinterpret_cast<const char*>(diskData.ptr), reinterpret_cast<char*>(fp.GetCurrentSeekPoint()), entry.size);

		if (r < 0 || r != entry.diskSize) 
		{
			TraceError("Failed to decompress %s with %s", path.c_str(), r);
			return false;
		}
	}

	return true;
}
