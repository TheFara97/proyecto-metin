#pragma once

#include "MonsterConfig.hpp"
#include "MonsterHighscore.hpp"
#include <chrono>
#include <ctime>
#include <memory>
#include <unordered_map>

// Forward declarations
class SimpleEventManager;

class MonsterSpawner : public singleton<MonsterSpawner> {
public:
	MonsterSpawner();
	~MonsterSpawner();

	// Initialize the spawner system
	bool Initialize();

	// Shutdown and cleanup
	void Destroy();

	// Process spawners (called periodically)
	void Process();

	// Handle monster death
	void OnMonsterDeath(uint32_t vid);

	// Handle monster damage
	void OnDamage(LPCHARACTER attacker, LPCHARACTER victim, int64_t damage);

	// Reload configuration
	void Reload();

	// Manual spawn trigger
	bool TriggerSpawn(uint32_t vnum);

	// Get spawner statistics
	size_t GetActiveSpawnCount() const;
	
	// Teleport to boss
	bool TeleportToBoss(CHARACTER* ch, uint32_t boss_vnum);

	// Special monster management
	std::unordered_map<uint8_t, MonsterSpawnEntry> CollectSpecialMonsters();
	void SendPacket(CHARACTER* ch, int day_index = -1);
	void SendWorldBossInfo(CHARACTER* ch);
	void SendHighscoreData(CHARACTER* ch, uint32_t boss_vnum);
	void SendPersonalStats(CHARACTER* ch, uint32_t boss_vnum);

	// Cross-core boss alive sync
	void BroadcastBossAlive(uint32_t vnum, bool is_alive, time_t next_spawn);
	void BroadcastAllBossStatusToDesc(LPDESC d);
	void UpdateBossAliveStatus(uint32_t vnum, bool is_alive, time_t next_spawn);

	// DB persistence for cross-channel status sync
	void SaveAliveStatusToDB(uint32_t vnum, bool is_alive, time_t next_spawn);
	void LoadAliveStatusFromDB(uint32_t vnum, bool& is_alive, time_t& next_spawn);

	// Configuration management
	bool SaveConfig() const { return m_config ? m_config->SaveToFile(m_configPath) : false; }
	MonsterConfig* GetConfig() const { return m_config.get(); }
	const std::string& GetConfigPath() const { return m_configPath; }
	void SetConfigPath(const std::string& path) { m_configPath = path; }

private:
	// Initialize a single spawner entry
	void InitializeSpawner(MonsterSpawnEntry& entry);

	// Schedule next spawn for interval/scheduled types
	void ScheduleNextSpawn(MonsterSpawnEntry& entry);

	// Spawn monsters according to entry configuration
	void SpawnMonster(MonsterSpawnEntry& entry);

	// Show spawn notice if configured
	bool ShowSpawnNotice(const MonsterSpawnEntry& entry);

	// Clean up dead monsters from entry
	void CleanDeadMonsters(MonsterSpawnEntry& entry);

	// Calculate next scheduled time
	std::chrono::system_clock::time_point GetNextScheduledTime(const MonsterSpawnEntry& entry);

	// Generate unique event name for spawner
	std::string GenerateEventName(const MonsterSpawnEntry& entry);

private:
	std::unique_ptr<MonsterConfig> m_config;
	std::unique_ptr<SimpleEventManager> m_eventManager;
	MonsterHighscoreManager m_highscoreManager;
	bool m_bInitialized;
	std::string m_configPath;
};