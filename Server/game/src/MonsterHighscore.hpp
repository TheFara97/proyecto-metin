#pragma once

#include <vector>
#include <string>
#include <optional>
#include <unordered_set>
#include <unordered_map>

struct MonsterDamageHighscoreEntry {
    uint32_t player_id;
    std::string player_name;
    int64_t damage;
    time_t timestamp;

    bool operator<(const MonsterDamageHighscoreEntry& other) const {
        return damage > other.damage; // sort descending
    }
};

class MonsterHighscoreManager {
public:
    void AddToWhitelist(uint32_t boss_vnum);

    // Called during fight - accumulates damage in quest flag
    void RegisterDamage(uint32_t boss_vnum, CHARACTER* ch, int64_t damage);

    // Called when boss spawns - clears current fight data
    void ClearCurrentFight(uint32_t boss_vnum);

    // Called when boss dies - saves quest flag data to database
    void SaveCurrentFightToDatabase(uint32_t boss_vnum);

    // Get ranking data from DATABASE (for UI display)
    std::vector<MonsterDamageHighscoreEntry> GetTopFromDatabase(uint32_t boss_vnum, size_t max_entries = 10) const;
    std::optional<MonsterDamageHighscoreEntry> GetPersonalFromDatabase(uint32_t boss_vnum, uint32_t player_id) const;

    bool IsHighscoreEnabled(uint32_t boss_vnum) const;

private:
    std::unordered_set<uint32_t> m_whitelist;

    // Track attackers during current fight (player IDs who dealt damage)
    std::unordered_map<uint32_t, std::unordered_set<uint32_t>> m_currentFightAttackers;

    std::optional<std::string> GetPlayerNameByID(uint32_t pid) const;
    std::string GetQuestFlagName(uint32_t boss_vnum) const;
};
