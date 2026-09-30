#pragma once
// Not strictly necessary (fwd. decl. would suffice),
// but we want to avoid having client-code #including this directly.
#include "VFSFilename.hpp"

#include "VFSArchive.hpp"

#include <vector>
#include <memory>
#include <string>
#include <unordered_set>

class CVirtualFile;

class VFSManager
{
	public:
		bool RegisterVirtualPackage(const std::string& path);
		bool DoesFileExist(const FilenameWrapper& path) const;
		bool OpenFile(const FilenameWrapper& path, CVirtualFile& fp, bool silent_failure = false) const;
		void SetBlacklistedItem(const std::string& extension);
		void AbleToOpenExternal(bool canDo);

	private:
		VirtualFileDict m_files;
		std::vector<std::unique_ptr<VFSArchive> > m_packages;
		std::unordered_set<std::string> m_diskExtBlacklist;
};

extern bool CanOpenExternalFile;