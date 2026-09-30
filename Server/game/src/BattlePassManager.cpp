#include "stdafx.h"

#ifdef _ENABLE_BATTLEPASS_
#include "BattlePassManager.h"
#include "locale_service.h"
#include "char.h"

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/error/en.h>
#include <fstream>
#include <random>

uint8_t BattlePassManager::ParseTrigger(const std::string& s)
{
    static const std::unordered_map<std::string, uint8_t> tbl = {
        {"kill",            MISSION_TYPE_KILL},
        {"sell",            MISSION_TYPE_SELL},
        {"destroy",         MISSION_TYPE_ITEM_DESTROY},
        {"drop",            MISSION_TYPE_ITEM_DROP},
        {"craft",           MISSION_TYPE_CRAFT},
        {"chest",           MISSION_TYPE_CHEST_OPEN},
        {"use_item",        MISSION_TYPE_USE_ITEM},
        {"dungeon",         MISSION_TYPE_FINISH_DUNGEON},
        {"polymorph",       MISSION_TYPE_POLYMORPH},
        {"shout",           MISSION_TYPE_MESSAGES},
        {"playtime",        MISSION_TYPE_PLAYTIME},
        {"refine",          MISSION_TYPE_REFINE},
        {"switchbot",       MISSION_TYPE_SWITCH_ITEMS},
        {"avg_damage",      MISSION_TYPE_AVERAGE_DAMAGE},
        {"kill_stone",      MISSION_TYPE_KILL_STONES},
        {"kill_boss",       MISSION_TYPE_KILL_BOSS},
        {"sash",            MISSION_TYPE_SASH},
        {"xmas_raffle",     MISSION_TYPE_CHRISTMAS_RAFFLE},
        {"xmas_map",        MISSION_TYPE_CHRISTMAS_MAP},
    };
    auto it = tbl.find(s);
    return (it != tbl.end()) ? it->second : MISSION_TYPE_KILL;
}

uint8_t BattlePassManager::ParseTrack(const std::string& s)
{
    if (s == "weekly") return MISSION_TIME_WEEKLY;
    if (s == "event")  return MISSION_TIME_EVENT;
    return MISSION_TIME_DAILY;
}

void BattlePassManager::LoadRewardList(const rapidjson::Value& arr,
    std::vector<std::pair<uint32_t, uint32_t>>& out)
{
    if (!arr.IsArray()) return;
    out.reserve(arr.Size());
    for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
    {
        const auto& e = arr[i];
        if (e.IsArray() && e.Size() == 2 && e[0].IsUint() && e[1].IsUint())
            out.emplace_back(e[0].GetUint(), e[1].GetUint());
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Boot
// ─────────────────────────────────────────────────────────────────────────────
void BattlePassManager::Boot()
{
    if (g_bAuthServer) return;

    m_defs.clear();
    for (auto& p : m_pool) p.clear();

    char path[PATH_MAX] = {};
    snprintf(path, sizeof(path), "%s/battlepass.json", LocaleService_GetBasePath().c_str());

    std::ifstream ifs(path);
    if (ifs.fail()) { sys_err("BattlePassManager: cannot open %s", path); return; }

    rapidjson::Document doc;
    rapidjson::IStreamWrapper isw(ifs);
    doc.ParseStream(isw);

    if (doc.HasParseError())
    {
        sys_err("BattlePassManager: JSON parse error in %s: %s (offset %u)",
                path, GetParseError_En(doc.GetParseError()), doc.GetErrorOffset());
        return;
    }

    // ── season ───────────────────────────────────────────────────────────────
    if (doc.HasMember("season") && doc["season"].IsObject())
    {
        const auto& s = doc["season"];
        if (s.HasMember("endsAt")       && s["endsAt"].IsUint64())  m_conf.seasonEnd      = s["endsAt"].GetUint64();
        if (s.HasMember("maxLevel")     && s["maxLevel"].IsUint())   m_conf.maxLevel       = static_cast<uint8_t>(s["maxLevel"].GetUint());
        if (s.HasMember("pointsPerLevel") && s["pointsPerLevel"].IsUint()) m_conf.pointsPerLevel = s["pointsPerLevel"].GetUint();
        if (s.HasMember("premiumItem")  && s["premiumItem"].IsUint()) m_conf.premiumItem   = s["premiumItem"].GetUint();
        if (s.HasMember("daily")        && s["daily"].IsUint())       m_conf.dailySlots    = static_cast<uint8_t>(s["daily"].GetUint());
        if (s.HasMember("weekly")       && s["weekly"].IsUint())      m_conf.weeklySlots   = static_cast<uint8_t>(s["weekly"].GetUint());
    }

    // ── event ─────────────────────────────────────────────────────────────────
    if (doc.HasMember("event") && doc["event"].IsObject())
    {
        const auto& e = doc["event"];
        if (e.HasMember("active")    && e["active"].IsBool())   m_conf.eventActive   = e["active"].GetBool();
        if (e.HasMember("endsAt")    && e["endsAt"].IsUint64()) m_conf.eventEnd      = e["endsAt"].GetUint64();
        if (e.HasMember("maxLevel")  && e["maxLevel"].IsUint()) m_conf.eventMaxLevel = static_cast<uint8_t>(e["maxLevel"].GetUint());
        if (e.HasMember("slots")     && e["slots"].IsUint())    m_conf.eventSlots    = static_cast<uint8_t>(e["slots"].GetUint());
    }

    // ── rewards ───────────────────────────────────────────────────────────────
    if (doc.HasMember("rewards") && doc["rewards"].IsObject())
    {
        const auto& r = doc["rewards"];
        if (r.HasMember("free"))    LoadRewardList(r["free"],    m_conf.rewardsFree);
        if (r.HasMember("premium")) LoadRewardList(r["premium"], m_conf.rewardsPremium);
        if (r.HasMember("event"))   LoadRewardList(r["event"],   m_conf.rewardsEvent);
    }

    // ── missions ──────────────────────────────────────────────────────────────
    if (doc.HasMember("missions") && doc["missions"].IsArray())
    {
        const auto& arr = doc["missions"];
        m_defs.reserve(arr.Size());

        for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
        {
            const auto& node = arr[i];
            if (!node.IsObject()) continue;

            MissionDef def;
            def.id = static_cast<uint16_t>(i);

            def.name    = (node.HasMember("name")    && node["name"].IsString())    ? node["name"].GetString()    : "Unknown";
            def.trigger = (node.HasMember("trigger") && node["trigger"].IsString()) ? ParseTrigger(node["trigger"].GetString()) : MISSION_TYPE_KILL;
            def.track   = (node.HasMember("track")   && node["track"].IsString())   ? ParseTrack(node["track"].GetString())     : MISSION_TIME_DAILY;
            def.goal    = (node.HasMember("goal")    && node["goal"].IsUint())      ? node["goal"].GetUint()    : 1;
            def.pts     = (node.HasMember("pts")     && node["pts"].IsUint())       ? static_cast<uint16_t>(node["pts"].GetUint()) : 0;

            // "target": "any"  |  [vnum, vnum, ...]
            def.anyTarget = true;
            if (node.HasMember("target"))
            {
                const auto& tgt = node["target"];
                if (tgt.IsArray())
                {
                    def.anyTarget = false;
                    def.targets.reserve(tgt.Size());
                    for (rapidjson::SizeType j = 0; j < tgt.Size(); ++j)
                        if (tgt[j].IsUint()) def.targets.push_back(tgt[j].GetUint());
                }
                // if it's a string ("any") we keep anyTarget = true
            }

            m_pool[def.track].push_back(m_defs.size());
            m_defs.push_back(std::move(def));

            sys_log(0, "BattlePassManager: def[%u] '%s' trigger=%d track=%d goal=%u pts=%u",
                    i, m_defs.back().name.c_str(), m_defs.back().trigger,
                    m_defs.back().track, m_defs.back().goal, m_defs.back().pts);
        }
    }

    sys_log(0, "BattlePassManager: boot done – %zu defs  pool[D=%zu W=%zu E=%zu]",
            m_defs.size(), m_pool[0].size(), m_pool[1].size(), m_pool[2].size());
}

std::vector<const MissionDef*> BattlePassManager::DrawMissions(uint8_t track) const
{
    const auto& pool = m_pool[track];

    // Event: hand back all definitions in the order they appear in the file
    if (track == MISSION_TIME_EVENT)
    {
        std::vector<const MissionDef*> out;
        out.reserve(pool.size());
        for (size_t idx : pool) out.push_back(&m_defs[idx]);
        return out;
    }

    const uint8_t cap = (track == MISSION_TIME_DAILY) ? m_conf.dailySlots : m_conf.weeklySlots;

    // Reservoir sampling (Algorithm R) – O(n), no duplicate ids possible
    std::vector<const MissionDef*> reservoir;
    reservoir.reserve(cap);

    std::mt19937 rng(static_cast<uint32_t>(get_global_time())
                     ^ static_cast<uint32_t>(reinterpret_cast<uintptr_t>(this) >> 4));

    for (size_t i = 0; i < pool.size(); ++i)
    {
        if (i < cap)
        {
            reservoir.push_back(&m_defs[pool[i]]);
        }
        else
        {
            std::uniform_int_distribution<size_t> pick(0, i);
            size_t j = pick(rng);
            if (j < cap) reservoir[j] = &m_defs[pool[i]];
        }
    }

    return reservoir;
}

// ─────────────────────────────────────────────────────────────────────────────
//  GetDef
// ─────────────────────────────────────────────────────────────────────────────
const MissionDef* BattlePassManager::GetDef(uint16_t id) const
{
    if (id < m_defs.size()) return &m_defs[id];
    return nullptr;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Notify – single dispatch point for all mission triggers
// ─────────────────────────────────────────────────────────────────────────────
void BattlePassManager::Notify(uint8_t trigger, LPCHARACTER ch, uint32_t subject, uint32_t amount)
{
    if (!ch || !ch->IsPC() || ch->GetLevel() < 30) return;
    ch->HandleBPTrigger(trigger, subject, amount);
}

#endif
