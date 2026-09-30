#include "stdafx.h"

#ifdef ENABLE_SUMMER_HWID_LIMIT
#include "summer_hwid_manager.h"
#include "char.h"
#include "db.h"
#include <fmt/format.h>
#include <ctime>

namespace SummerHwidManager {
    
    // Singleton instance
    HwidAccessManager& HwidAccessManager::instance() {
        static HwidAccessManager instance_;
        return instance_;
    }
    
    // Generate cache key
    std::string HwidAccessManager::GetCacheKey(const std::string& hwid, AccessType type, uint32_t index) const {
        return fmt::format("{}_{}_{}", hwid, static_cast<int>(type), index);
    }
    
    // Get access type name for database
    const char* HwidAccessManager::GetAccessTypeName(AccessType type) const {
        return (type == ACCESS_TYPE_DUNGEON) ? "dungeon" : "map";
    }
    
    // Clean expired cache entries
    void HwidAccessManager::CleanExpiredCache() {
        std::lock_guard<std::mutex> lock(cache_mutex_);
        
        auto it = cache_.begin();
        while (it != cache_.end()) {
            if (it->second.IsExpired()) {
                it = cache_.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    // Load access data from database
    HwidAccessCache HwidAccessManager::LoadFromDatabase(const std::string& hwid, AccessType type, uint32_t index) {
        if (hwid.empty()) {
            sys_err("LoadFromDatabase: Empty HWID");
            return HwidAccessCache();
        }
        
        char escaped_hwid[513]; // Increased size for composite HWID
        DBManager::instance().EscapeString(escaped_hwid, sizeof(escaped_hwid), hwid.c_str(), hwid.length());
        
        char query[1024]; // Increased query size
        snprintf(query, sizeof(query),
            "SELECT last_entry_time, cooldown_seconds, max_duration_seconds FROM srv1_account.summer_hwid_cooldowns "
            "WHERE hwid_composite='%s' AND access_type='%s' AND access_index=%u",
            escaped_hwid, GetAccessTypeName(type), index);
        
        std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(query));
        if (!pMsg || !pMsg->Get()) {
            return HwidAccessCache(); // No entry found
        }
        
        MYSQL_RES* pRes = pMsg->Get()->pSQLResult;
        if (!pRes) {
            return HwidAccessCache();
        }
        
        MYSQL_ROW row = mysql_fetch_row(pRes);
        if (!row || !row[0]) {
            return HwidAccessCache();
        }
        
        time_t last_entry_time = static_cast<time_t>(strtoul(row[0], nullptr, 10));
        uint32_t cooldown_seconds = row[1] ? static_cast<uint32_t>(strtoul(row[1], nullptr, 10)) : 10800;
        uint32_t max_duration_seconds = row[2] ? static_cast<uint32_t>(strtoul(row[2], nullptr, 10)) : 0;
        
        sys_log(0, "LoadFromDatabase: HWID %s, %s %u, last_entry: %ld, cooldown: %u", 
                hwid.substr(0, 20).c_str(), GetAccessTypeName(type), index, last_entry_time, cooldown_seconds);
        
        return HwidAccessCache(last_entry_time, cooldown_seconds, max_duration_seconds);
    }
    
    // Save access data to database
    bool HwidAccessManager::SaveToDatabase(const std::string& hwid, AccessType type, uint32_t index,
                                          time_t entry_time, uint32_t cooldown_seconds, uint32_t max_duration_seconds, uint32_t account_id) {
        if (hwid.empty()) {
            sys_err("SaveToDatabase: Empty HWID");
            return false;
        }

        char escaped_hwid[513]; // Increased size for composite HWID
        DBManager::instance().EscapeString(escaped_hwid, sizeof(escaped_hwid), hwid.c_str(), hwid.length());

        char query[2048]; // Increased query size
        if (max_duration_seconds > 0) {
            snprintf(query, sizeof(query),
                "INSERT INTO srv1_account.summer_hwid_cooldowns (account_id, hwid_composite, access_type, access_index, last_entry_time, cooldown_seconds, max_duration_seconds) "
                "VALUES (%u, '%s', '%s', %u, %ld, %u, %u) "
                "ON DUPLICATE KEY UPDATE account_id=%u, last_entry_time=%ld, cooldown_seconds=%u, max_duration_seconds=%u, updated_at=NOW()",
                account_id, escaped_hwid, GetAccessTypeName(type), index, entry_time, cooldown_seconds, max_duration_seconds,
                account_id, entry_time, cooldown_seconds, max_duration_seconds);
        } else {
            snprintf(query, sizeof(query),
                "INSERT INTO srv1_account.summer_hwid_cooldowns (account_id, hwid_composite, access_type, access_index, last_entry_time, cooldown_seconds) "
                "VALUES (%u, '%s', '%s', %u, %ld, %u) "
                "ON DUPLICATE KEY UPDATE account_id=%u, last_entry_time=%ld, cooldown_seconds=%u, updated_at=NOW()",
                account_id, escaped_hwid, GetAccessTypeName(type), index, entry_time, cooldown_seconds,
                account_id, entry_time, cooldown_seconds);
        }
        
        std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(query));
        if (!pMsg) {
            sys_err("SaveToDatabase: Query failed for HWID %s, %s %u", 
                    hwid.substr(0, 20).c_str(), GetAccessTypeName(type), index);
            return false;
        }
        
        sys_log(0, "SaveToDatabase: HWID %s, %s %u, entry_time: %ld, cooldown: %u", 
                hwid.substr(0, 20).c_str(), GetAccessTypeName(type), index, entry_time, cooldown_seconds);
        
        return true;
    }
    
    // Delete access data from database
    bool HwidAccessManager::DeleteFromDatabase(const std::string& hwid, AccessType type, uint32_t index) {
        if (hwid.empty()) {
            sys_err("DeleteFromDatabase: Empty HWID");
            return false;
        }
        
        char escaped_hwid[513]; // Increased size for composite HWID
        DBManager::instance().EscapeString(escaped_hwid, sizeof(escaped_hwid), hwid.c_str(), hwid.length());
        
        char query[1024];
        snprintf(query, sizeof(query),
            "DELETE FROM srv1_account.summer_hwid_cooldowns WHERE hwid_composite='%s' AND access_type='%s' AND access_index=%u",
            escaped_hwid, GetAccessTypeName(type), index);
        
        std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(query));
        if (!pMsg) {
            sys_err("DeleteFromDatabase: Query failed for HWID %s, %s %u", 
                    hwid.substr(0, 20).c_str(), GetAccessTypeName(type), index);
            return false;
        }
        
        sys_log(0, "DeleteFromDatabase: HWID %s, %s %u", hwid.substr(0, 20).c_str(), GetAccessTypeName(type), index);
        return true;
    }
    
    // Check if can access (with cache)
    bool HwidAccessManager::CanAccess(const std::string& hwid, AccessType type, uint32_t index, uint32_t cooldown_seconds) {
        if (hwid.empty()) {
            sys_err("CanAccess: Empty HWID");
            return false;
        }
        
        CleanExpiredCache();
        
        std::string cache_key = GetCacheKey(hwid, type, index);
        HwidAccessCache cache_entry;
        
        // Check cache first
        {
            std::lock_guard<std::mutex> lock(cache_mutex_);
            auto it = cache_.find(cache_key);
            if (it != cache_.end() && !it->second.IsExpired()) {
                cache_entry = it->second;
                sys_log(0, "CanAccess: Cache hit for HWID %s, %s %u", 
                        hwid.substr(0, 20).c_str(), GetAccessTypeName(type), index);
            } else {
                // Cache miss, load from database
                cache_entry = LoadFromDatabase(hwid, type, index);
                cache_[cache_key] = cache_entry;
                sys_log(0, "CanAccess: Cache miss, loaded from DB for HWID %s, %s %u", 
                        hwid.substr(0, 20).c_str(), GetAccessTypeName(type), index);
            }
        }
        
        // Check cooldown
        if (cache_entry.last_entry_time == 0) {
            return true; // Never accessed
        }
        
        time_t current_time = time(nullptr); // Use standard C time function
        time_t elapsed = current_time - cache_entry.last_entry_time;
        
        // Use cached cooldown if available, otherwise use provided cooldown
        uint32_t effective_cooldown = cache_entry.cooldown_seconds > 0 ? cache_entry.cooldown_seconds : cooldown_seconds;
        bool can_access = elapsed >= effective_cooldown;
        
        sys_log(0, "CanAccess: HWID %s, %s %u, elapsed: %ld, cooldown: %u, can_access: %s",
                hwid.substr(0, 20).c_str(), GetAccessTypeName(type), index, elapsed, effective_cooldown, can_access ? "YES" : "NO");
        
        return can_access;
    }
    
    // Set access cooldown
    bool HwidAccessManager::SetAccessCooldown(const std::string& hwid, AccessType type, uint32_t index,
                                             uint32_t cooldown_seconds, uint32_t max_duration_seconds, uint32_t account_id) {
        if (hwid.empty()) {
            sys_err("SetAccessCooldown: Empty HWID");
            return false;
        }

        time_t current_time = time(nullptr); // Use standard C time function

        // Save to database
        if (!SaveToDatabase(hwid, type, index, current_time, cooldown_seconds, max_duration_seconds, account_id)) {
            sys_err("SetAccessCooldown: Failed to save to database");
            return false;
        }
        
        // Update cache
        {
            std::lock_guard<std::mutex> lock(cache_mutex_);
            std::string cache_key = GetCacheKey(hwid, type, index);
            cache_[cache_key] = HwidAccessCache(current_time, cooldown_seconds, max_duration_seconds);
        }
        
        sys_log(0, "SetAccessCooldown: HWID %s, %s %u, time: %ld, cooldown: %u", 
                hwid.substr(0, 20).c_str(), GetAccessTypeName(type), index, current_time, cooldown_seconds);
        
        return true;
    }
    
    // Get remaining cooldown time
    time_t HwidAccessManager::GetAccessCooldownRemaining(const std::string& hwid, AccessType type, uint32_t index) {
        if (hwid.empty()) {
            return 0;
        }
        
        std::string cache_key = GetCacheKey(hwid, type, index);
        HwidAccessCache cache_entry;
        
        // Check cache first
        {
            std::lock_guard<std::mutex> lock(cache_mutex_);
            auto it = cache_.find(cache_key);
            if (it != cache_.end() && !it->second.IsExpired()) {
                cache_entry = it->second;
            } else {
                // Load from database and update cache
                cache_entry = LoadFromDatabase(hwid, type, index);
                cache_[cache_key] = cache_entry;
            }
        }
        
        if (cache_entry.last_entry_time == 0) {
            return 0; // Never accessed
        }
        
        time_t current_time = time(nullptr); // Use standard C time function
        time_t elapsed = current_time - cache_entry.last_entry_time;
        
        if (elapsed >= cache_entry.cooldown_seconds) {
            return 0; // Cooldown expired
        }
        
        return cache_entry.cooldown_seconds - elapsed;
    }
    
    // Clear access cooldown (admin function)
    void HwidAccessManager::ClearAccessCooldown(const std::string& hwid, AccessType type, uint32_t index) {
        if (hwid.empty()) {
            return;
        }
        
        // Delete from database
        DeleteFromDatabase(hwid, type, index);
        
        // Remove from cache
        {
            std::lock_guard<std::mutex> lock(cache_mutex_);
            std::string cache_key = GetCacheKey(hwid, type, index);
            cache_.erase(cache_key);
        }
        
        sys_log(0, "ClearAccessCooldown: HWID %s, %s %u", hwid.substr(0, 20).c_str(), GetAccessTypeName(type), index);
    }
    
    // Public convenience functions for maps (updated to use member function)
    bool CanAccessSummerMap(LPCHARACTER ch, uint32_t map_index, uint32_t cooldown_hours) {
        if (!ch) {
            sys_err("CanAccessSummerMap: NULL character");
            return false;
        }
        
        std::string hwid = ch->GetCompositeHWID(); // Use member function
        if (hwid.empty()) {
            sys_err("CanAccessSummerMap: Character %s has empty composite HWID", ch->GetName());
            return false;
        }
        
        return HwidAccessManager::instance().CanAccess(hwid, ACCESS_TYPE_MAP, map_index, cooldown_hours * 3600);
    }
    
    bool SetSummerMapCooldown(LPCHARACTER ch, uint32_t map_index, uint32_t cooldown_hours, uint32_t max_duration_minutes) {
        if (!ch) {
            sys_err("SetSummerMapCooldown: NULL character");
            return false;
        }

        std::string hwid = ch->GetCompositeHWID(); // Use member function
        if (hwid.empty()) {
            sys_err("SetSummerMapCooldown: Character %s has empty composite HWID", ch->GetName());
            return false;
        }

        uint32_t account_id = ch->GetDesc() ? ch->GetDesc()->GetAccountTable().id : 0;
        return HwidAccessManager::instance().SetAccessCooldown(hwid, ACCESS_TYPE_MAP, map_index,
                                                               cooldown_hours * 3600, max_duration_minutes * 60, account_id);
    }
    
    time_t GetSummerMapCooldownRemaining(LPCHARACTER ch, uint32_t map_index) {
        if (!ch) {
            return 0;
        }
        
        std::string hwid = ch->GetCompositeHWID(); // Use member function
        if (hwid.empty()) {
            return 0;
        }
        
        return HwidAccessManager::instance().GetAccessCooldownRemaining(hwid, ACCESS_TYPE_MAP, map_index);
    }
    
    void ClearSummerMapCooldown(LPCHARACTER ch, uint32_t map_index) {
        if (!ch) {
            return;
        }
        
        std::string hwid = ch->GetCompositeHWID(); // Use member function
        if (hwid.empty()) {
            return;
        }
        
        HwidAccessManager::instance().ClearAccessCooldown(hwid, ACCESS_TYPE_MAP, map_index);
    }
    
    // Public convenience functions for dungeons (updated to use member function)
    bool CanAccessSummerDungeon(LPCHARACTER ch, uint32_t dungeon_index, uint32_t cooldown_hours) {
        if (!ch) {
            sys_err("CanAccessSummerDungeon: NULL character");
            return false;
        }
        
        std::string hwid = ch->GetCompositeHWID(); // Use member function
        if (hwid.empty()) {
            sys_err("CanAccessSummerDungeon: Character %s has empty composite HWID", ch->GetName());
            return false;
        }
        
        return HwidAccessManager::instance().CanAccess(hwid, ACCESS_TYPE_DUNGEON, dungeon_index, cooldown_hours * 3600);
    }
    
    bool SetSummerDungeonCooldown(LPCHARACTER ch, uint32_t dungeon_index, uint32_t cooldown_hours) {
        if (!ch) {
            sys_err("SetSummerDungeonCooldown: NULL character");
            return false;
        }

        std::string hwid = ch->GetCompositeHWID(); // Use member function
        if (hwid.empty()) {
            sys_err("SetSummerDungeonCooldown: Character %s has empty composite HWID", ch->GetName());
            return false;
        }

        uint32_t account_id = ch->GetDesc() ? ch->GetDesc()->GetAccountTable().id : 0;
        return HwidAccessManager::instance().SetAccessCooldown(hwid, ACCESS_TYPE_DUNGEON, dungeon_index, cooldown_hours * 3600, 0, account_id);
    }
    
    time_t GetSummerDungeonCooldownRemaining(LPCHARACTER ch, uint32_t dungeon_index) {
        if (!ch) {
            return 0;
        }
        
        std::string hwid = ch->GetCompositeHWID(); // Use member function
        if (hwid.empty()) {
            return 0;
        }
        
        return HwidAccessManager::instance().GetAccessCooldownRemaining(hwid, ACCESS_TYPE_DUNGEON, dungeon_index);
    }
    
    void ClearSummerDungeonCooldown(LPCHARACTER ch, uint32_t dungeon_index) {
        if (!ch) {
            return;
        }
        
        std::string hwid = ch->GetCompositeHWID(); // Use member function
        if (hwid.empty()) {
            return;
        }
        
        HwidAccessManager::instance().ClearAccessCooldown(hwid, ACCESS_TYPE_DUNGEON, dungeon_index);
    }
}
#endif