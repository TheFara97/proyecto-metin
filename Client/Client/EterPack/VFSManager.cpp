#include "stdafx.h"
#include "VFSManager.hpp"
#include "VFSArchive.hpp"
#include "VFSFilename.hpp"

#include "MappedFile.hpp"

#include <cassert>
#include <io.h>

bool CanOpenExternalFile = false;

bool VFSManager::RegisterVirtualPackage(const std::string& path)
{
	std::unique_ptr<VFSArchive> pack(new VFSArchive());

	if (!pack->Create(path, m_files)) 
	{
		return false;
	}

	m_packages.push_back(std::move(pack));
	return true;
}

bool VFSManager::DoesFileExist(const FilenameWrapper& path) const
{
	if (m_files.find(path.GetHash()) != m_files.end())
		return true;
	return 0 == _access(path.GetPath().c_str(), 0);
}

bool VFSManager::OpenFile(const FilenameWrapper& path, CVirtualFile& fp, bool silent_failure) const
{
	if (CanOpenExternalFile)
	{
		// In this mode we can try to load locally first before packs
		if (fp.Create(path.GetPath()))
			return true;

		const auto it = m_files.find(path.GetHash());
		if (it != m_files.end())
		{
			const auto& entry = it->second;
			return entry.first->Get(path.GetPath(), entry.second, fp);
		}
		if (!silent_failure)
		{
		}
		return false;
	}
	else
	{
		const auto it = m_files.find(path.GetHash());
		if (it != m_files.end())
		{
			const auto& entry = it->second;
			return entry.first->Get(path.GetPath(), entry.second, fp);
		}
		if (fp.Create(path.GetPath()))
			return true;

		if (!silent_failure)
		{
			return false;
		}
	}
	return false;
}

void VFSManager::AbleToOpenExternal(bool canDo)
{
	CanOpenExternalFile = canDo;
}

void VFSManager::SetBlacklistedItem(const std::string& extension)
{
	m_diskExtBlacklist.insert(extension);
}


