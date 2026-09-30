#include "stdafx.h"
#include "MonsterSpawner.hpp"
#include "MonsterConfig.hpp"
#include "char.h"
#include "char_manager.h"
#include "config.h"
#include "sectree_manager.h"
#include "locale_service.h"
#include "mob_manager.h"
#include "p2p.h"
#include "utils.h"
#include "db.h"
#include <algorithm>
#include <random>
#include <ctime>
#include <fmt/format.h>

// Simple event system replacement
struct MonsterSpawnerEvent {
	std::string name;
	std::function<void()> callback;
	time_t execute_time;
	bool valid = true;

	MonsterSpawnerEvent(const std::string& n, std::function<void()> cb, time_t exec_time)
		: name(n), callback(std::move(cb)), execute_time(exec_time) {
	}
};

class SimpleEventManager {
public:
	void AddEvent(const std::string& name, std::function<void()> callback, time_t delay) {
		RemoveEvent(name);  // Remove existing event with same name
		time_t execute_time = time(nullptr) + delay;
		m_events.emplace_back(name, std::move(callback), execute_time);
	}

	void RemoveEvent(const std::string& name) {
		m_events.erase(
			std::remove_if(m_events.begin(), m_events.end(),
				[&name](MonsterSpawnerEvent& event) {
					if (event.name == name) {
						event.valid = false;
						return true;
					}
					return false;
				}),
			m_events.end()
		);
	}

	void Process() {
		time_t now = time(nullptr);

		for (auto it = m_events.begin(); it != m_events.end();) {
			if (!it->valid) {
				it = m_events.erase(it);
				continue;
			}

			if (now >= it->execute_time) {
				try {
					it->callback();
				}
				catch (const std::exception& e) {
					sys_err("SimpleEventManager: Exception in event %s: %s",
						it->name.c_str(), e.what());
				}
				it = m_events.erase(it);
			}
			else {
				++it;
			}
		}
	}

	void Clear() {
		m_events.clear();
	}

private:
	std::vector<MonsterSpawnerEvent> m_events;
};

MonsterSpawner::MonsterSpawner()
	: m_config(std::make_unique<MonsterConfig>()),
	m_eventManager(std::make_unique<SimpleEventManager>()),
	m_bInitialized(false),
	m_configPath("locale/germany/worldboss-spawner.json") {
}

MonsterSpawner::~MonsterSpawner() {
	Destroy();
}

bool MonsterSpawner::Initialize() {
	if (m_bInitialized) return true;

	sys_log(0, "MonsterSpawner: Initializing system...");

	// Load configuration
	if (!m_config->LoadFromFile(m_configPath)) {
		sys_err("MonsterSpawner: Failed to load configuration from %s", m_configPath.c_str());
		return false;
	}

	sys_log(0, "MonsterSpawner: Loaded %zu spawn entries", m_config->GetSpawnCount());

	// Initialize all spawn entries
	for (auto& entry : m_config->GetSpawns()) {
		if (!entry.enabled) {
			sys_log(0, "MonsterSpawner: Skipping disabled entry for vnum %u", entry.vnum);
			continue;
		}

		// Log map ownership for debugging
		bool ownsMap = map_allow_find(entry.map_index);
		sys_log(0, "MonsterSpawner: Entry vnum %u, map %u, owns_map=%d",
			entry.vnum, entry.map_index, ownsMap ? 1 : 0);

		// Generate unique event name
		entry.event_name = GenerateEventName(entry);

		// Initialize spawner on all cores (for schedule info)
		// Actual spawning is controlled by map_allow_find check in SpawnMonster()
		InitializeSpawner(entry);

		// Adding special monsters to whitelist (all cores need this for data queries)
		if (entry.is_special) {
			m_highscoreManager.AddToWhitelist(entry.vnum);
		}
	}

	m_bInitialized = true;
	sys_log(0, "MonsterSpawner: System initialized successfully");
	return true;
}

void MonsterSpawner::Destroy() {
	if (!m_bInitialized) return;

	sys_log(0, "MonsterSpawner: Shutting down system...");

	// Cancel all events
	if (m_eventManager) {
		m_eventManager->Clear();
	}

	m_bInitialized = false;

	sys_log(0, "MonsterSpawner: System shutdown complete");
}

void MonsterSpawner::InitializeSpawner(MonsterSpawnEntry& entry) {
	sys_log(0, "MonsterSpawner: Initializing spawner for vnum %u, type %u",
		entry.vnum, static_cast<uint32_t>(entry.type));

	switch (entry.type) {
	case SpawnType::CYCLIC:
		// Cyclic spawns wait for monster death, then respawn after delay
		// Initial spawn happens immediately
		SpawnMonster(entry);
		break;

	case SpawnType::INTERVAL:
		// Interval spawns happen at fixed intervals regardless of monster state
		ScheduleNextSpawn(entry);
		break;

	case SpawnType::SCHEDULED:
		// Scheduled spawns happen at specific times of day
		ScheduleNextSpawn(entry);
		break;
	}
}

void MonsterSpawner::ScheduleNextSpawn(MonsterSpawnEntry& entry) {
	using namespace std::chrono;

	time_t delay = 0;

	switch (entry.type) {
	case SpawnType::INTERVAL:
		delay = entry.interval;
		break;

	case SpawnType::SCHEDULED: {
		auto now = std::chrono::system_clock::now();
		auto next_time = GetNextScheduledTime(entry);
		delay = std::chrono::ceil<std::chrono::seconds>(next_time - now).count();
		break;
	}

	case SpawnType::CYCLIC:
		delay = entry.respawn_time;
		break;
	}

	if (delay <= 0)
		delay = 1;

	// Remove existing event
	m_eventManager->RemoveEvent(entry.event_name);

	// Schedule new event
	auto callback = [this, &entry]() {
		SpawnMonster(entry);

		if ((entry.type == SpawnType::INTERVAL) || (entry.type == SpawnType::SCHEDULED)) {
			ScheduleNextSpawn(entry);
		}
		};

	m_eventManager->AddEvent(entry.event_name, callback, delay);
	entry.next_spawn_time = time(nullptr) + delay;

	sys_log(0, "MonsterSpawner: Scheduled spawn for vnum %u in %ld seconds",
		entry.vnum, delay);
}

void MonsterSpawner::SpawnMonster(MonsterSpawnEntry& entry) {
	// Check if this core owns the map
	if (!map_allow_find(entry.map_index)) {
		sys_log(0, "MonsterSpawner: Map %u not owned by this core, skipping spawn for vnum %u",
			entry.map_index, entry.vnum);
		return;
	}

	// Validate map
	LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(entry.map_index);
	if (!pSecMap) {
		sys_err("MonsterSpawner: Map %u is allowed but not loaded for monster %u",
			entry.map_index, entry.vnum);
		return;
	}

	// Clean up dead monsters
	CleanDeadMonsters(entry);

	// Check if we need to spawn more monsters
	int32_t spawnCount = entry.count - entry.mob_vids.size();
	if (spawnCount <= 0) {
		// Already have enough monsters spawned
		return;
	}

	// Show spawn notice if configured
	ShowSpawnNotice(entry);

	// Clear previous fight's damage tracking for special monsters
	if (entry.is_special) {
		m_highscoreManager.ClearCurrentFight(entry.vnum);
	}

	// Spawn the monsters
	for (int32_t i = 0; i < spawnCount; ++i) {
		int32_t x = entry.x;
		int32_t y = entry.y;

		// Handle random position if needed
		if (entry.position == SpawnPosition::RANDOM && entry.radius > 0) {
			static std::random_device rd;
			static std::mt19937 gen(rd());
			std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * M_PI);
			std::uniform_real_distribution<float> radiusDist(0.0f, static_cast<float>(entry.radius));

			float angle = angleDist(gen);
			float radius = radiusDist(gen);

			x = entry.x + static_cast<int32_t>(radius * cos(angle));
			y = entry.y + static_cast<int32_t>(radius * sin(angle));
		}

		// Convert to world coordinates
		int32_t worldX = pSecMap->m_setting.iBaseX + x * 100;
		int32_t worldY = pSecMap->m_setting.iBaseY + y * 100;

		// Create the monster
		LPCHARACTER mob = CHARACTER_MANAGER::instance().SpawnMob(
			entry.vnum,
			entry.map_index,
			worldX, worldY, 0, // x, y, z
			false, // show spawn effect
			-1, // rotation
			true // spawn motion
		);

		if (mob) {
			entry.mob_vids.push_back(mob->GetVID());
			entry.is_alive = true;
			sys_log(0, "MonsterSpawner: Spawned %u (VID: %u) at %d, %d (map: %u)",
				entry.vnum, (DWORD) mob->GetVID(), mob->GetX(), mob->GetY(), entry.map_index);

			if (entry.is_special) {
				BroadcastBossAlive(entry.vnum, true, 0);
				SaveAliveStatusToDB(entry.vnum, true, 0);
			}
		}
		else {
			sys_err("MonsterSpawner: Failed to spawn %u at %d,%d (map: %u)",
				entry.vnum, x, y, entry.map_index);
		}
	}
}

void MonsterSpawner::Process() {
	if (!m_bInitialized) return;

	// Process events
	if (m_eventManager) {
		m_eventManager->Process();
	}

	// Check cyclic spawners for respawn conditions
	for (auto& entry : m_config->GetSpawns()) {
		if (!entry.enabled || entry.type != SpawnType::CYCLIC) continue;

		// Only process entries for maps this core owns
		if (!map_allow_find(entry.map_index)) continue;

		// Clean up dead monsters
		CleanDeadMonsters(entry);

		// Check if we need to respawn and enough time has passed
		if (entry.mob_vids.empty() &&
			entry.next_spawn_time > 0 &&
			time(nullptr) >= entry.next_spawn_time) {

			SpawnMonster(entry);
			entry.next_spawn_time = 0; // Reset spawn time
		}
	}
}

void MonsterSpawner::OnMonsterDeath(uint32_t vid) {
	if (!m_bInitialized) return;

	for (auto& entry : m_config->GetSpawns()) {
		// Check if this monster belongs to this entry
		auto it = std::find(entry.mob_vids.begin(), entry.mob_vids.end(), vid);
		if (it != entry.mob_vids.end()) {
			// Remove the monster from this entry
			entry.mob_vids.erase(it);

			entry.is_alive = false;
			m_highscoreManager.SaveCurrentFightToDatabase(entry.vnum);

			sys_log(0, "MonsterSpawner: Monster VID %u died (vnum: %u)", vid, entry.vnum);

			// If this was a cyclic spawn and all monsters are dead, schedule respawn
			if (entry.type == SpawnType::CYCLIC && entry.mob_vids.empty()) {
				entry.next_spawn_time = time(nullptr) + entry.respawn_time;
				sys_log(0, "MonsterSpawner: Cyclic respawn scheduled for vnum %u in %u seconds",
					entry.vnum, entry.respawn_time);
			}

			if (entry.is_special)
				BroadcastBossAlive(entry.vnum, false, entry.next_spawn_time);

			break;
		}
	}
}

void MonsterSpawner::OnDamage(LPCHARACTER attacker, LPCHARACTER victim, int64_t damage) {
	if (!attacker || !victim || !attacker->IsPC() || damage <= 0)
		return;

	if (m_highscoreManager.IsHighscoreEnabled(victim->GetRaceNum())) {
		m_highscoreManager.RegisterDamage(victim->GetRaceNum(), attacker, damage);
	}
}

void MonsterSpawner::Reload() {
	sys_log(0, "MonsterSpawner: Reloading configuration...");

	// Cancel all existing events
	if (m_eventManager) {
		m_eventManager->Clear();
	}

	// Reload the configuration file
	if (!m_config->Reload(m_configPath)) {
		sys_err("MonsterSpawner: Failed to reload configuration file");
		return;
	}

	// Re-initialize all spawners
	for (auto& entry : m_config->GetSpawns()) {
		if (!entry.enabled) continue;

		// Clean up dead monsters
		CleanDeadMonsters(entry);

		// Generate new event name
		entry.event_name = GenerateEventName(entry);

		// Re-initialize the spawner
		InitializeSpawner(entry);
	}

	sys_log(0, "MonsterSpawner: Configuration reloaded successfully with %zu entries",
		m_config->GetSpawnCount());
}

bool MonsterSpawner::TriggerSpawn(uint32_t vnum) {
	if (!m_bInitialized) return false;

	for (auto& entry : m_config->GetSpawns()) {
		if (entry.vnum == vnum && entry.enabled) {
			SpawnMonster(entry);
			return true;
		}
	}

	return false;
}

size_t MonsterSpawner::GetActiveSpawnCount() const {
	if (!m_config) return 0;

	size_t count = 0;
	for (const auto& entry : m_config->GetSpawns()) {
		if (entry.enabled) {
			count += entry.mob_vids.size();
		}
	}

	return count;
}

bool MonsterSpawner::ShowSpawnNotice(const MonsterSpawnEntry& entry) {
	if (!entry.show_notice) return false;

	std::string noticeText;
	if (!entry.notice_text.empty()) {
		noticeText = entry.notice_text;
	}
	else {
		// Get monster name for default notice
		const CMob* mob_proto = CMobManager::instance().Get(entry.vnum);
		if (!mob_proto) {
			return false;
		}

		noticeText = fmt::format("A {} has appeared!", mob_proto->m_table.szLocaleName);
	}

	// Show notice to all players on the map
	SendNoticeMap(noticeText.c_str(), entry.map_index, false);
	return true;
}

void MonsterSpawner::CleanDeadMonsters(MonsterSpawnEntry& entry) {
	entry.mob_vids.erase(
		std::remove_if(entry.mob_vids.begin(), entry.mob_vids.end(),
			[](uint32_t vid) {
				LPCHARACTER ch = CHARACTER_MANAGER::instance().Find(vid);
				return ch == nullptr || ch->IsDead();
			}
		),
		entry.mob_vids.end()
	);
}

std::chrono::system_clock::time_point MonsterSpawner::GetNextScheduledTime(const MonsterSpawnEntry& entry) {
	using namespace std::chrono;

	const auto now = floor<seconds>(system_clock::now());
	const std::time_t now_c = system_clock::to_time_t(now);
	const std::tm local_tm = *std::localtime(&now_c);
	const int current_wday = local_tm.tm_wday; // Sunday = 0

	system_clock::time_point best_time;

	for (int dayOffset = 0; dayOffset < 7; ++dayOffset) {
		int future_wday = (current_wday + dayOffset) % 7;

		for (const auto& sched : entry.schedule) {
			if (sched.day_of_week != -1 && sched.day_of_week != future_wday)
				continue;

			std::tm scheduled_tm = local_tm;
			scheduled_tm.tm_mday += dayOffset;
			scheduled_tm.tm_hour = sched.hour;
			scheduled_tm.tm_min = sched.minute;
			scheduled_tm.tm_sec = sched.second;

			time_t sched_time_t = std::mktime(&scheduled_tm);
			if (sched_time_t == -1)
				continue;

			auto candidate = system_clock::from_time_t(sched_time_t);

			auto delta = duration_cast<seconds>(candidate - now).count();

			if (delta <= 0) {
				scheduled_tm.tm_mday += 7;
				sched_time_t = std::mktime(&scheduled_tm);
				candidate = system_clock::from_time_t(sched_time_t);
			}

			if (best_time == system_clock::time_point{} || candidate < best_time) {
				best_time = candidate;
			}
		}

		if (best_time != system_clock::time_point{})
			break;
	}

	// Fallback
	if (best_time == system_clock::time_point{}) {
		return now + seconds(entry.interval);
	}

	return best_time;
}

std::string MonsterSpawner::GenerateEventName(const MonsterSpawnEntry& entry) {
	std::string base = fmt::format("{}_{}_{}_{}", entry.vnum, entry.map_index, entry.x, entry.y);

	if (!entry.schedule.empty()) {
		const auto& s = entry.schedule[0];
		base += fmt::format("_{}_{}:{}:{}", s.day_of_week, s.hour, s.minute, s.second);
	}

	std::hash<std::string> hasher;
	size_t hash_val = hasher(base);
	return fmt::format("monster_spawn_{}", hash_val);
}

std::unordered_map<uint8_t, MonsterSpawnEntry> MonsterSpawner::CollectSpecialMonsters() {
	std::unordered_map<uint8_t, MonsterSpawnEntry> m_Entries;

	for (const auto& entry : m_config->GetSpawns()) {
		if (!entry.is_special)
			continue;

		for (const auto& sched : entry.schedule) {
			m_Entries[sched.day_of_week] = entry;
		}
	}

	return m_Entries;
}

void MonsterSpawner::SendPacket(CHARACTER* ch, int day_index) {
	if (!ch || !m_config) {
		sys_err("SendPacket: ch=%p, config=%p", ch, m_config.get());
		return;
	}

	sys_log(0, "MonsterSpawner::SendPacket called for player %s, day_index=%d", 
		ch->GetName(), day_index);

	auto entries = CollectSpecialMonsters();
	sys_log(0, "MonsterSpawner::SendPacket: Found %zu special monsters", entries.size());

	for (const auto& [d, entry] : entries) {
		if (day_index > -1 && d != day_index)
			continue;

		// Convert local coordinates to world coordinates
		// Use GetMapBasePositionByMapIndex which works across cores
		PIXEL_POSITION basePos;
		int32_t worldX = 0;
		int32_t worldY = 0;

		if (SECTREE_MANAGER::instance().GetMapBasePositionByMapIndex(entry.map_index, basePos)) {
			worldX = basePos.x + entry.x * 100;
			worldY = basePos.y + entry.y * 100;
		}

		sys_log(0, "MonsterSpawner: Sending day %d, vnum %u, map %u, coords (%d, %d)", 
			d, entry.vnum, entry.map_index, worldX, worldY);

		// Send with map index and coordinates
		ch->SendEventToClient("WORLD_BOSS_REGISTER_DAY", 
			static_cast<int>(d),
			static_cast<int>(entry.vnum), 
			entry.is_alive ? 1 : 0, 
			static_cast<long>(entry.next_spawn_time),
			static_cast<int>(entry.map_index),
			static_cast<int>(worldX),
			static_cast<int>(worldY));

		if (!entry.is_special || !m_highscoreManager.IsHighscoreEnabled(entry.vnum))
			continue;

		// Send ranking data
		const auto top = m_highscoreManager.GetTopFromDatabase(entry.vnum);
		for (size_t i = 0; i < top.size(); ++i) {
			const auto& e = top[i];
			ch->SendEventToClient("WORLD_BOSS_REGISTER_RANKING", 
				static_cast<int>(d),
				static_cast<int>(entry.vnum),
				static_cast<int>(i + 1),
				e.player_name,
				static_cast<long>(e.damage),
				static_cast<long>(e.timestamp));
		}

		// Send personal best
		auto personal = m_highscoreManager.GetPersonalFromDatabase(entry.vnum, ch->GetPlayerID());
		if (personal.has_value()) {
			const auto& p = personal.value();
			ch->SendEventToClient("WORLD_BOSS_REGISTER_PERSONAL", 
				static_cast<int>(d),
				static_cast<int>(entry.vnum),
				p.player_name,
				static_cast<long>(p.damage),
				static_cast<long>(p.timestamp));
		}
	}
}

void MonsterSpawner::SendWorldBossInfo(CHARACTER* ch) {
	if (!ch || !m_config) return;

	ch->ChatPacket(CHAT_TYPE_COMMAND, "WORLD_BOSS_INFO_START");

	auto entries = CollectSpecialMonsters();
	for (const auto& [day, entry] : entries) {
		if (!entry.is_special) continue;

		// Get monster name
		const CMob* mob_proto = CMobManager::instance().Get(entry.vnum);
		std::string mob_name = mob_proto ? mob_proto->m_table.szLocaleName : "Unknown";

		// Calculate time until next spawn
		time_t now = time(nullptr);
		time_t time_until_spawn = (entry.next_spawn_time > now) ? (entry.next_spawn_time - now) : 0;

		ch->ChatPacket(CHAT_TYPE_COMMAND, "WORLD_BOSS_INFO %d %u %s %d %ld %ld",
			day, entry.vnum, mob_name.c_str(), entry.is_alive ? 1 : 0,
			entry.next_spawn_time, time_until_spawn);
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, "WORLD_BOSS_INFO_END");
}

void MonsterSpawner::SendHighscoreData(CHARACTER* ch, uint32_t boss_vnum) {
	if (!ch || !m_highscoreManager.IsHighscoreEnabled(boss_vnum)) return;

	ch->ChatPacket(CHAT_TYPE_COMMAND, "WORLD_BOSS_HIGHSCORE_START %u", boss_vnum);

	const auto top = m_highscoreManager.GetTopFromDatabase(boss_vnum);
	for (size_t i = 0; i < top.size(); ++i) {
		const auto& entry = top[i];

		// Format damage with commas for readability
		std::string damage_str = fmt::format("{:L}", entry.damage);

		// Calculate days ago
		time_t now = time(nullptr);
		int days_ago = (now - entry.timestamp) / 86400;

		ch->ChatPacket(CHAT_TYPE_COMMAND, "WORLD_BOSS_HIGHSCORE %u %zu %s %s %ld %d",
			boss_vnum, i + 1, entry.player_name.c_str(), damage_str.c_str(),
			entry.timestamp, days_ago);
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, "WORLD_BOSS_HIGHSCORE_END %u", boss_vnum);
}

void MonsterSpawner::SendPersonalStats(CHARACTER* ch, uint32_t boss_vnum) {
	if (!ch || !m_highscoreManager.IsHighscoreEnabled(boss_vnum)) return;

	auto personal = m_highscoreManager.GetPersonalFromDatabase(boss_vnum, ch->GetPlayerID());

	if (personal.has_value()) {
		const auto& p = personal.value();

		// Format damage with commas
		std::string damage_str = fmt::format("{:L}", p.damage);

		// Calculate days ago
		time_t now = time(nullptr);
		int days_ago = (now - p.timestamp) / 86400;

		// Find ranking position
		const auto top = m_highscoreManager.GetTopFromDatabase(boss_vnum);
		size_t rank = 0;
		for (size_t i = 0; i < top.size(); ++i) {
			if (top[i].player_id == ch->GetPlayerID()) {
				rank = i + 1;
				break;
			}
		}

		ch->ChatPacket(CHAT_TYPE_COMMAND, "WORLD_BOSS_PERSONAL %u %s %s %ld %d %zu",
			boss_vnum, p.player_name.c_str(), damage_str.c_str(),
			p.timestamp, days_ago, rank);
	}
	else {
		ch->ChatPacket(CHAT_TYPE_COMMAND, "WORLD_BOSS_PERSONAL %u NONE 0 0 0 0", boss_vnum);
	}
}

bool MonsterSpawner::TeleportToBoss(CHARACTER* ch, uint32_t boss_vnum) {
	if (!ch || !m_config || !m_bInitialized)
		return false;
	
	auto spawns = m_config->GetSpawns();
	sys_log(0, "TeleportToBoss: Total spawns loaded: %zu, looking for vnum %u", 
		spawns.size(), boss_vnum);
	
	// Find the boss entry
	for (const auto& entry : spawns) {
		sys_log(0, "Checking spawn: vnum=%u, is_special=%d, enabled=%d", 
			entry.vnum, entry.is_special, entry.enabled);
		
		if (entry.vnum == boss_vnum) {
			sys_log(0, "Found matching vnum %u: is_special=%d, enabled=%d", 
				boss_vnum, entry.is_special, entry.enabled);
			
			if (!entry.is_special) {
				ch->ChatPacket(CHAT_TYPE_INFO, "This boss is not marked as special.");
				return false;
			}
			
			if (!entry.enabled) {
				ch->ChatPacket(CHAT_TYPE_INFO, "This boss spawn is disabled.");
				return false;
			}

			PIXEL_POSITION basePos;
			if (!SECTREE_MANAGER::instance().GetMapBasePositionByMapIndex(entry.map_index, basePos)) {
				sys_err("TeleportToBoss: Cannot get base position for map %u", entry.map_index);
				ch->ChatPacket(CHAT_TYPE_INFO, "Cannot find map for this boss.");
				return false;
			}

			int32_t worldX = basePos.x + entry.x * 100;
			int32_t worldY = basePos.y + entry.y * 100;
			
			// Teleport the player
			ch->ChatPacket(CHAT_TYPE_INFO, "Teleporting to boss...");
			ch->WarpSet(worldX, worldY, entry.map_index);
			
			sys_log(0, "Player %s teleported to boss %u at (%d, %d) on map %u",
				ch->GetName(), boss_vnum, worldX, worldY, entry.map_index);
			
			return true;
		}
	}
	
	ch->ChatPacket(CHAT_TYPE_INFO, "Boss vnum %u not found in configuration.", boss_vnum);
	sys_log(0, "TeleportToBoss: Boss %u not found in spawns list", boss_vnum);
	return false;
}

void MonsterSpawner::BroadcastBossAlive(uint32_t vnum, bool is_alive, time_t next_spawn) {
	TPacketGGWorldBossAlive p;
	p.bHeader = HEADER_GG_WORLD_BOSS_ALIVE;
	p.dwVnum = vnum;
	p.bAlive = is_alive;
	p.tNextSpawn = next_spawn;

	P2P_MANAGER::instance().Send(&p, sizeof(p));
}

void MonsterSpawner::BroadcastAllBossStatusToDesc(LPDESC d) {
	if (!d || !m_config) return;

	for (const auto& entry : m_config->GetSpawns()) {
		if (!entry.is_special || !entry.enabled) continue;

		TPacketGGWorldBossAlive p;
		p.bHeader = HEADER_GG_WORLD_BOSS_ALIVE;
		p.dwVnum = entry.vnum;
		p.bAlive = entry.is_alive;
		p.tNextSpawn = entry.next_spawn_time;

		d->Packet(&p, sizeof(p));
	}
}

void MonsterSpawner::SaveAliveStatusToDB(uint32_t vnum, bool is_alive, time_t next_spawn) {
	DBManager::instance().Query(
		"INSERT INTO world_boss_status (vnum, is_alive, next_spawn_time) VALUES (%u, %d, %ld) "
		"ON DUPLICATE KEY UPDATE is_alive=%d, next_spawn_time=%ld",
		vnum, is_alive ? 1 : 0, (long)next_spawn,
		is_alive ? 1 : 0, (long)next_spawn);
}

void MonsterSpawner::LoadAliveStatusFromDB(uint32_t vnum, bool& is_alive, time_t& next_spawn) {
	auto pmsg = DBManager::instance().DirectQuery(
		"SELECT is_alive, next_spawn_time FROM world_boss_status WHERE vnum=%u", vnum);
	if (!pmsg || !pmsg->Get() || pmsg->Get()->uiNumRows == 0)
		return;
	MYSQL_ROW row = mysql_fetch_row(pmsg->Get()->pSQLResult);
	if (!row) return;
	is_alive = atoi(row[0]) != 0;
	next_spawn = (time_t)atol(row[1]);
}

void MonsterSpawner::UpdateBossAliveStatus(uint32_t vnum, bool is_alive, time_t next_spawn) {
	if (!m_config) return;

	for (auto& entry : m_config->GetSpawns()) {
		if (entry.vnum == vnum && entry.is_special) {
			entry.is_alive = is_alive;
			entry.next_spawn_time = next_spawn;
			sys_log(0, "MonsterSpawner: Updated boss %u alive=%d next_spawn=%ld (from P2P)",
				vnum, is_alive ? 1 : 0, next_spawn);
			return;
		}
	}
}