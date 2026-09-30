#include "stdafx.h"
#include "../EterPack/VirtualFileSystem"

namespace
{
	VFSManager* VirtualFileSystemManager = nullptr;
}

void SetVirtualFileSystem(VFSManager* yit)
{
	VirtualFileSystemManager = yit;
}

VFSManager& CallVFS()
{
	// Drop me an assert if ->
	assert(VirtualFileSystemManager != nullptr);
	return *VirtualFileSystemManager;
}