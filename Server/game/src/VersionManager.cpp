#include "stdafx.h"
#include "VersionManager.hpp"

#include <fstream>
#include "locale_service.h"
#include <fmt/format.h>

namespace client {
	std::unique_ptr<VersionManager> g_versionManager;

	void VersionManager::Load() {
	    sys_log(0, "Load client version.");

	    try {
	        std::ifstream file(fmt::format("{}/version.txt", LocaleService_GetBasePath().c_str()).c_str());

	        std::string fileContent((std::istreambuf_iterator<char>(file)),
	                                std::istreambuf_iterator<char>());

	        uint32_t versionId = 0;
	        str_to_number(versionId, fileContent.c_str());

	        if (versionId == 0)
	            sys_log(0, "Client version is 0, check's disabled.");

	        SetVersionId(versionId);
	    } catch (std::exception& e) {
	        sys_err("Failed to load version with error: %s", e.what());
	    }
	}

	void VersionManager::Reload() {
	    sys_log(0, "Send client version reload packet.");

	}

	bool VersionManager::IsValidVersion(uint32_t versionId) const {
	    return versionId >= GetVersionId();
	}

	void VersionManager::Create() {
	    g_versionManager.reset(new VersionManager());
	    g_versionManager->Load();
	}
};  // namespace client
