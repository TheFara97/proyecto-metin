#pragma once
#include "Utils.hpp"

#include "FileBase.hpp"

#include <string>
#include <unordered_map>
#include <utility>

class CVirtualFile;
class VFSArchive;

typedef std::unordered_map<uint64_t, std::pair<VFSArchive*, VirtualEntry>> VirtualFileDict;

class VFSArchive
{
	public:
		bool Create(const std::string& filename, VirtualFileDict& dict);
		bool Get(const std::string& path, const VirtualEntry& entry, CVirtualFile& fp);

	private:
		CFileBase m_file;
};