#ifndef __SUMMER_HWID_MANAGER_H__
#define __SUMMER_HWID_MANAGER_H__

#ifdef ENABLE_SUMMER_HWID_LIMIT
#include "stdafx.h"
#include <unordered_map>
#include <memory>
#include <mutex>
#include <string>
#include <ctime>

namespace SummerHwidManager {
    // Access types
    enum AccessType {
        ACCESS_TYPE_DUNGEON = 0,
        ACCESS_TYPE_MAP = 1
    };
    
    // Cache entry structure
    struct HwidAccessCache {
        time_t last_entry_time;
        time_t cache_time;
        uint32_t cooldown_seconds;
        uint32_t max_duration_seconds;
        bool is_valid;
        
        HwidAccessCache() : last_entry_time(0), cache_time(0), cooldown_seconds(0), max_duration_seconds(0), is_valid(false) {}
        HwidAccessCache(time_t entry_time, uint32_t cooldown, uint32_t max_duration = 0) 
            : last_entry_time(entry_time), cache_time(time(nullptr)), cooldown_seconds(cooldown), 
              max_duration_seconds(max_duration), is_valid(true) {}
        
        bool IsExpired() const {
            return (time(nullptr) - cache_time) > 300; // 5 minutes cache TTL
        }
    };
    
    // Main manager class
    class HwidAccessManager {
    private:
        std::unordered_map<std::string, HwidAccessCache> cache_;
        std::mutex cache_mutex_;
        
        std::string GetCacheKey(const std::string& hwid, AccessType type, uint32_t index) const;
        void CleanExpiredCache();
        const char* GetAccessTypeName(AccessType type) const;
        
    public:
        static HwidAccessManager& instance();
        
        // Core functions
        bool CanAccess(const std::string& hwid, AccessType type, uint32_t index, uint32_t cooldown_seconds = 10800);
        bool SetAccessCooldown(const std::string& hwid, AccessType type, uint32_t index, uint32_t cooldown_seconds = 10800, uint32_t max_duration_seconds = 0, uint32_t account_id = 0);
        time_t GetAccessCooldownRemaining(const std::string& hwid, AccessType type, uint32_t index);
        void ClearAccessCooldown(const std::string& hwid, AccessType type, uint32_t index);
        
        // Database operations
        HwidAccessCache LoadFromDatabase(const std::string& hwid, AccessType type, uint32_t index);
        bool SaveToDatabase(const std::string& hwid, AccessType type, uint32_t index, time_t entry_time, uint32_t cooldown_seconds, uint32_t max_duration_seconds = 0, uint32_t account_id = 0);
        bool DeleteFromDatabase(const std::string& hwid, AccessType type, uint32_t index);
    };
    
    // Public convenience functions for maps
    bool CanAccessSummerMap(LPCHARACTER ch, uint32_t map_index, uint32_t cooldown_hours = 3);
    bool SetSummerMapCooldown(LPCHARACTER ch, uint32_t map_index, uint32_t cooldown_hours = 3, uint32_t max_duration_minutes = 30);
    time_t GetSummerMapCooldownRemaining(LPCHARACTER ch, uint32_t map_index);
    void ClearSummerMapCooldown(LPCHARACTER ch, uint32_t map_index);
    
    // Public convenience functions for dungeons
    bool CanAccessSummerDungeon(LPCHARACTER ch, uint32_t dungeon_index, uint32_t cooldown_hours = 3);
    bool SetSummerDungeonCooldown(LPCHARACTER ch, uint32_t dungeon_index, uint32_t cooldown_hours = 3);
    time_t GetSummerDungeonCooldownRemaining(LPCHARACTER ch, uint32_t dungeon_index);
    void ClearSummerDungeonCooldown(LPCHARACTER ch, uint32_t dungeon_index);
}

#endif // ENABLE_SUMMER_HWID_LIMIT
#endif // __SUMMER_HWID_MANAGER_H__