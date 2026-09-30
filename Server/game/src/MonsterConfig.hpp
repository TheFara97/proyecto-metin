#pragma once

#include "stdafx.h"
#include <vector>
#include <string>
#include <unordered_set>
#include <filesystem>
#include <fstream>
#include <compare>  // Required for C++20 comparison features
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

enum class SpawnType : uint8_t {
    CYCLIC = 0,     // Spawn -> Death -> Wait -> Respawn
    INTERVAL = 1,   // Fixed interval spawning
    SCHEDULED = 2   // Specific time-based spawning
};

enum class SpawnPosition : uint8_t {
    FIXED = 0,
    RANDOM = 1
};

struct ScheduleEntry {
    int day_of_week = -1;  // -1 = every day, 0 = Sunday, 1 = Monday, etc.
    int hour = 0;
    int minute = 0;
    int second = 0;
};

struct MonsterSpawnEntry {
    // Basic spawn info
    uint32_t vnum = 0;
    uint32_t map_index = 0;
    int32_t x = 0;
    int32_t y = 0;
    int32_t count = 1;

    // Spawn behavior
    SpawnType type = SpawnType::CYCLIC;
    SpawnPosition position = SpawnPosition::FIXED;
    uint32_t radius = 0;
    uint32_t respawn_time = 300;  // seconds
    uint32_t interval = 3600;     // seconds for interval spawning

    // Schedule (for SCHEDULED type)
    std::vector<ScheduleEntry> schedule;

    // Configuration
    bool enabled = true;
    bool show_notice = false;
    bool is_special = false;
    std::string notice_text;

    // Runtime data (not serialized)
    std::vector<uint32_t> mob_vids;
    std::string event_name;
    time_t next_spawn_time = 0;
    bool is_alive = false;
};

class MonsterConfig {
public:
    MonsterConfig() = default;
    ~MonsterConfig() = default;

    bool LoadFromFile(const std::string& filename) {
        try {
            if (!std::filesystem::exists(filename)) {
                sys_err("MonsterConfig: File does not exist: %s", filename.c_str());
                return false;
            }

            std::ifstream file(filename);
            if (!file.is_open()) {
                sys_err("MonsterConfig: Failed to open file: %s", filename.c_str());
                return false;
            }

            std::string content((std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());
            file.close();

            // Clear existing data
            m_spawns.clear();

            // Parse JSON
            if (!ParseConfigJson(content)) {
                sys_err("MonsterConfig: Failed to parse JSON config");
                return false;
            }

            sys_log(0, "MonsterConfig: Loaded %zu spawn entries from %s",
                m_spawns.size(), filename.c_str());
            return true;

        }
        catch (const std::exception& e) {
            sys_err("MonsterConfig: Exception loading config: %s", e.what());
            return false;
        }
    }

    bool SaveToFile(const std::string& filename) const {
        try {
            std::string json_str = GenerateConfigJson();

            if (json_str.empty()) {
                sys_err("MonsterConfig: Failed to serialize to JSON - empty result");
                return false;
            }

            // Ensure directory exists
            std::filesystem::create_directories(std::filesystem::path(filename).parent_path());

            std::ofstream file(filename);
            if (!file.is_open()) {
                sys_err("MonsterConfig: Failed to create file: %s", filename.c_str());
                return false;
            }

            file << json_str;
            file.close();

            sys_log(0, "MonsterConfig: Saved config to %s", filename.c_str());
            return true;

        }
        catch (const std::exception& e) {
            sys_err("MonsterConfig: Exception saving config: %s", e.what());
            return false;
        }
    }

    bool Reload(const std::string& filename) {
        m_spawns.clear();
        return LoadFromFile(filename);
    }

    // Accessors
    std::vector<MonsterSpawnEntry>& GetSpawns() { return m_spawns; }
    const std::vector<MonsterSpawnEntry>& GetSpawns() const { return m_spawns; }
    size_t GetSpawnCount() const { return m_spawns.size(); }

    // Add/Remove entries
    void AddSpawn(const MonsterSpawnEntry& entry) {
        m_spawns.push_back(entry);
    }

    bool RemoveSpawn(uint32_t vnum) {
        auto it = std::find_if(m_spawns.begin(), m_spawns.end(),
            [vnum](const MonsterSpawnEntry& entry) { return entry.vnum == vnum; });

        if (it != m_spawns.end()) {
            m_spawns.erase(it);
            return true;
        }
        return false;
    }

private:
    bool ParseConfigJson(const std::string& json_content) {
        try {
            rapidjson::Document document;
            document.Parse(json_content.c_str());

            if (document.HasParseError()) {
                sys_err("MonsterConfig: JSON parse error at offset %zu: %d",
                    document.GetErrorOffset(), document.GetParseError());
                return false;
            }

            if (!document.IsArray()) {
                sys_err("MonsterConfig: Expected JSON array at root level");
                return false;
            }

            for (rapidjson::SizeType i = 0; i < document.Size(); ++i) {
                const auto& item = document[i];
                if (!item.IsObject()) continue;

                MonsterSpawnEntry entry;

                // Parse basic fields
                if (item.HasMember("vnum") && item["vnum"].IsUint())
                    entry.vnum = item["vnum"].GetUint();
                if (item.HasMember("map_index") && item["map_index"].IsUint())
                    entry.map_index = item["map_index"].GetUint();
                if (item.HasMember("x") && item["x"].IsInt())
                    entry.x = item["x"].GetInt();
                if (item.HasMember("y") && item["y"].IsInt())
                    entry.y = item["y"].GetInt();
                if (item.HasMember("count") && item["count"].IsInt())
                    entry.count = item["count"].GetInt();
                if (item.HasMember("radius") && item["radius"].IsUint())
                    entry.radius = item["radius"].GetUint();
                if (item.HasMember("respawn_time") && item["respawn_time"].IsUint())
                    entry.respawn_time = item["respawn_time"].GetUint();
                if (item.HasMember("interval") && item["interval"].IsUint())
                    entry.interval = item["interval"].GetUint();

                // Parse enums
                if (item.HasMember("type") && item["type"].IsInt()) {
                    int type_val = item["type"].GetInt();
                    entry.type = static_cast<SpawnType>(type_val);
                }
                if (item.HasMember("position") && item["position"].IsInt()) {
                    int pos_val = item["position"].GetInt();
                    entry.position = static_cast<SpawnPosition>(pos_val);
                }

                // Parse booleans
                if (item.HasMember("enabled") && item["enabled"].IsBool())
                    entry.enabled = item["enabled"].GetBool();
                if (item.HasMember("show_notice") && item["show_notice"].IsBool())
                    entry.show_notice = item["show_notice"].GetBool();
                if (item.HasMember("is_special") && item["is_special"].IsBool())
                    entry.is_special = item["is_special"].GetBool();

                // Parse strings
                if (item.HasMember("notice_text") && item["notice_text"].IsString())
                    entry.notice_text = item["notice_text"].GetString();

                // Parse schedule array
                if (item.HasMember("schedule") && item["schedule"].IsArray()) {
                    const auto& schedule_array = item["schedule"];
                    for (rapidjson::SizeType j = 0; j < schedule_array.Size(); ++j) {
                        const auto& sched_item = schedule_array[j];
                        if (!sched_item.IsObject()) continue;

                        ScheduleEntry sched;
                        if (sched_item.HasMember("day_of_week") && sched_item["day_of_week"].IsInt())
                            sched.day_of_week = sched_item["day_of_week"].GetInt();
                        if (sched_item.HasMember("hour") && sched_item["hour"].IsInt())
                            sched.hour = sched_item["hour"].GetInt();
                        if (sched_item.HasMember("minute") && sched_item["minute"].IsInt())
                            sched.minute = sched_item["minute"].GetInt();
                        if (sched_item.HasMember("second") && sched_item["second"].IsInt())
                            sched.second = sched_item["second"].GetInt();

                        entry.schedule.push_back(sched);
                    }
                }

                m_spawns.push_back(entry);
            }

            return true;
        }
        catch (const std::exception& e) {
            sys_err("MonsterConfig: Exception parsing JSON: %s", e.what());
            return false;
        }
    }

    std::string GenerateConfigJson() const {
        try {
            rapidjson::Document document;
            document.SetArray();
            auto& allocator = document.GetAllocator();

            for (const auto& entry : m_spawns) {
                rapidjson::Value spawn_obj(rapidjson::kObjectType);

                // Basic fields
                spawn_obj.AddMember("vnum", entry.vnum, allocator);
                spawn_obj.AddMember("map_index", entry.map_index, allocator);
                spawn_obj.AddMember("x", entry.x, allocator);
                spawn_obj.AddMember("y", entry.y, allocator);
                spawn_obj.AddMember("count", entry.count, allocator);
                spawn_obj.AddMember("type", static_cast<int>(entry.type), allocator);
                spawn_obj.AddMember("position", static_cast<int>(entry.position), allocator);
                spawn_obj.AddMember("radius", entry.radius, allocator);
                spawn_obj.AddMember("respawn_time", entry.respawn_time, allocator);
                spawn_obj.AddMember("interval", entry.interval, allocator);
                spawn_obj.AddMember("enabled", entry.enabled, allocator);
                spawn_obj.AddMember("show_notice", entry.show_notice, allocator);
                spawn_obj.AddMember("is_special", entry.is_special, allocator);

                // String field
                rapidjson::Value notice_text(entry.notice_text.c_str(), allocator);
                spawn_obj.AddMember("notice_text", notice_text, allocator);

                // Schedule array
                rapidjson::Value schedule_array(rapidjson::kArrayType);
                for (const auto& sched : entry.schedule) {
                    rapidjson::Value sched_obj(rapidjson::kObjectType);
                    sched_obj.AddMember("day_of_week", sched.day_of_week, allocator);
                    sched_obj.AddMember("hour", sched.hour, allocator);
                    sched_obj.AddMember("minute", sched.minute, allocator);
                    sched_obj.AddMember("second", sched.second, allocator);
                    schedule_array.PushBack(sched_obj, allocator);
                }
                spawn_obj.AddMember("schedule", schedule_array, allocator);

                document.PushBack(spawn_obj, allocator);
            }

            // Convert to string with pretty formatting
            rapidjson::StringBuffer buffer;
            rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
            document.Accept(writer);

            return buffer.GetString();
        }
        catch (const std::exception& e) {
            sys_err("MonsterConfig: Exception generating JSON: %s", e.what());
            return "";
        }
    }

private:
    std::vector<MonsterSpawnEntry> m_spawns;
};