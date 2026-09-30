#include "stdafx.h"
#include "char.h"
#include "char_manager.h"
#include "config.h"
#include "db.h"
#include "desc_manager.h"
#include "questmanager.h"
#include <fmt/format.h>
#include "MonsterHighscore.hpp"

std::string MonsterHighscoreManager::GetQuestFlagName(uint32_t boss_vnum) const {
    return fmt::format("MSC.BOSS_{}_DAMAGE", boss_vnum);
}

void MonsterHighscoreManager::RegisterDamage(uint32_t boss_vnum, CHARACTER* ch, int64_t damage) {
    if (!ch || !ch->IsPC()) return;

    // Track this player as an attacker for this boss
    m_currentFightAttackers[boss_vnum].insert(ch->GetPlayerID());

    // Accumulate damage in quest flag (survives teleport)
    std::string flag = GetQuestFlagName(boss_vnum);
    int64_t current = ch->GetQuestFlag(flag);
    ch->SetQuestFlag(flag, current + damage);
}

void MonsterHighscoreManager::ClearCurrentFight(uint32_t boss_vnum) {
    // Clear quest flags for all attackers
    auto it = m_currentFightAttackers.find(boss_vnum);
    if (it != m_currentFightAttackers.end()) {
        std::string flag = GetQuestFlagName(boss_vnum);

        for (uint32_t player_id : it->second) {
            LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(player_id);
            if (ch) {
                ch->SetQuestFlag(flag, 0);
            }
        }

        it->second.clear();
    }
}

void MonsterHighscoreManager::SaveCurrentFightToDatabase(uint32_t boss_vnum) {
    auto it = m_currentFightAttackers.find(boss_vnum);
    if (it == m_currentFightAttackers.end() || it->second.empty()) return;

    std::string flag = GetQuestFlagName(boss_vnum);
    std::vector<MonsterDamageHighscoreEntry> entries;

    // Collect damage from quest flags for all attackers
    for (uint32_t player_id : it->second) {
        LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(player_id);
        if (!ch) continue;

        int64_t damage = ch->GetQuestFlag(flag);
        if (damage <= 0) continue;

        MonsterDamageHighscoreEntry entry;
        entry.player_id = player_id;
        entry.player_name = ch->GetName();
        entry.damage = damage;
        entry.timestamp = get_global_time();
        entries.push_back(entry);
    }

    if (entries.empty()) return;

    // Sort by damage descending and keep top 10
    std::sort(entries.begin(), entries.end());
    if (entries.size() > 10) entries.resize(10);

    // Save to database (replace existing data for this boss)
    DBManager::instance().DirectQuery(
        fmt::format("DELETE FROM monster_special_ranking WHERE MS_MonsterID = {}", boss_vnum).c_str()
    );

    for (const auto& e : entries) {
        DBManager::instance().DirectQuery(
            fmt::format(
                "INSERT INTO monster_special_ranking "
                "(MS_MonsterID, MS_PlayerID, MS_DamageValue, MS_Time) "
                "VALUES ({}, {}, {}, {})",
                boss_vnum, e.player_id, e.damage, e.timestamp
            ).c_str()
        );
    }

    // Clear quest flags after saving
    for (uint32_t player_id : it->second) {
        LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(player_id);
        if (ch) {
            ch->SetQuestFlag(flag, 0);
        }
    }

    // Clear attacker list
    it->second.clear();
}

std::vector<MonsterDamageHighscoreEntry> MonsterHighscoreManager::GetTopFromDatabase(uint32_t boss_vnum, size_t max_entries) const {
    auto pkMsg = std::unique_ptr<SQLMsg>(DBManager::instance().DirectQuery(
        fmt::format(
            "SELECT MS_PlayerID, MS_DamageValue, MS_Time "
            "FROM monster_special_ranking "
            "WHERE MS_MonsterID = {} "
            "ORDER BY MS_DamageValue DESC "
            "LIMIT {}",
            boss_vnum, max_entries
        ).c_str()
    ));

    std::vector<MonsterDamageHighscoreEntry> result;
    SQLResult* pRes = pkMsg->Get();
    if (!pRes || pRes->uiNumRows == 0) return result;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(pRes->pSQLResult))) {
        MonsterDamageHighscoreEntry entry;
        entry.player_id = std::atoi(row[0]);
        entry.damage = std::atoll(row[1]);
        entry.timestamp = std::atoi(row[2]);
        entry.player_name = GetPlayerNameByID(entry.player_id).value_or("Unknown");
        result.push_back(entry);
    }

    return result;
}

std::optional<MonsterDamageHighscoreEntry> MonsterHighscoreManager::GetPersonalFromDatabase(uint32_t boss_vnum, uint32_t player_id) const {
    auto pkMsg = std::unique_ptr<SQLMsg>(DBManager::instance().DirectQuery(
        fmt::format(
            "SELECT MS_DamageValue, MS_Time "
            "FROM monster_special_ranking "
            "WHERE MS_MonsterID = {} AND MS_PlayerID = {}",
            boss_vnum, player_id
        ).c_str()
    ));

    SQLResult* pRes = pkMsg->Get();
    if (!pRes || pRes->uiNumRows == 0) return std::nullopt;

    MYSQL_ROW row = mysql_fetch_row(pRes->pSQLResult);
    if (!row) return std::nullopt;

    MonsterDamageHighscoreEntry entry;
    entry.player_id = player_id;
    entry.damage = std::atoll(row[0]);
    entry.timestamp = std::atoi(row[1]);
    entry.player_name = GetPlayerNameByID(player_id).value_or("Unknown");

    return entry;
}

bool MonsterHighscoreManager::IsHighscoreEnabled(uint32_t boss_vnum) const {
    return m_whitelist.count(boss_vnum) > 0;
}

void MonsterHighscoreManager::AddToWhitelist(uint32_t boss_vnum) {
    m_whitelist.insert(boss_vnum);
}

std::optional<std::string> MonsterHighscoreManager::GetPlayerNameByID(uint32_t pid) const {
    auto pkMsg = std::unique_ptr<SQLMsg>(DBManager::instance().DirectQuery(
        fmt::format("SELECT name FROM player WHERE id = {}", pid).c_str()
    ));

    SQLResult* pRes = pkMsg->Get();
    if (!pRes || !pRes->uiNumRows) return std::nullopt;

    MYSQL_ROW row = mysql_fetch_row(pRes->pSQLResult);
    return row ? std::optional<std::string>(row[0]) : std::nullopt;
}
