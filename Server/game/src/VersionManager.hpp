#pragma once

#include <memory>

namespace client {
	class VersionManager {
		public:
		    VersionManager() = default;
		    virtual ~VersionManager() = default;

		    void Load();
		    void Reload();

		    // TODO: Add this so we are able to notify currently playing players about a new client version.
		    // void VerifyAllPlayers();

		    bool IsValidVersion(uint32_t versionId) const;

		protected:
		    void SetVersionId(uint32_t versionId) { versionId_ = versionId; }

		public:
		    const uint32_t GetVersionId() const { return versionId_; }

		protected:
		    uint32_t versionId_;

		public:
		    static void Create();
		};

		extern std::unique_ptr<VersionManager> g_versionManager;
};  // namespace client
