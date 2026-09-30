#include "stdafx.h"

#ifdef ENABLE_PVP_RANKING
#include "char.h"  
#include "char_manager.h"
#include "pvp_ranking.h"
#include "utils.h"
#include "log.h"
#include "db.h"
#include "config.h"
#include "desc.h"
#include "desc_manager.h"
#include "buffer_manager.h"
#include "packet.h"
#include "desc_client.h"
#include "p2p.h"
#include "questmanager.h"
#include "event.h"
#include "party.h"
#include "start_position.h"
#include "sectree_manager.h"

#include <algorithm>
#include <vector>
#include <queue>
#include <memory>

CPvPRanking::CPvPRanking()
{
}

CPvPRanking::~CPvPRanking()
{
    Destroy();
}

void CPvPRanking::Destroy()
{
    std::lock_guard<std::mutex> lock(m_data_mutex);
    
    m_player_stats.clear();
    m_participants.clear();
    
#ifdef ENABLE_PVP_RANKING_WITH_EMPIRE
    m_empire_stats.clear();
#endif
    
    m_status.store(0);
    m_is_ended.store(false);
    m_time_event_active.store(false);
}

void CPvPRanking::AddKill(LPCHARACTER ch)
{
    if (!ch || !IsEventActive()) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_data_mutex);
    
    const VID player_vid = ch->GetVID();
    auto it = m_player_stats.find(player_vid);
    
    if (it == m_player_stats.end()) {
        m_player_stats[player_vid] = TPvPInfo(1, 0);
        ch->ChatPacket(CHAT_TYPE_NOTICE, LC_TEXT("Zabil jsi hrace."));
        ch->SetRankKill(1);
    } else {
        it->second.iKill++;
        ch->SetRankKill(it->second.iKill);
        ch->ChatPacket(CHAT_TYPE_NOTICE, 
            LC_TEXT("Zabil jsi hrace - Celkem zabiti: %d - Celkem smrti: %d"), 
            it->second.iKill, it->second.iDead);
    }
    
#ifdef ENABLE_PVP_RANKING_WITH_EMPIRE
    // Update empire stats
    const BYTE empire = ch->GetEmpire();
    auto emp_it = m_empire_stats.find(empire);
    if (emp_it == m_empire_stats.end()) {
        m_empire_stats[empire] = 1;
    } else {
        emp_it->second++;
    }
#endif
    
    // Check win condition
    const int max_kills = m_max_kill.load();
    if (m_player_stats[player_vid].iKill >= max_kills) {
        BroadcastWinMessage(ch);
        m_is_ended.store(true);
    }
}

void CPvPRanking::AddDead(LPCHARACTER ch)
{
    if (!ch || !IsEventActive()) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_data_mutex);
    
    const VID player_vid = ch->GetVID();
    auto it = m_player_stats.find(player_vid);
    
    if (it == m_player_stats.end()) {
        m_player_stats[player_vid] = TPvPInfo(0, 1);
        ch->SetRankDead(1);
    } else {
        it->second.iDead++;
        ch->SetRankDead(it->second.iDead);
    }
    
    ch->ChatPacket(CHAT_TYPE_NOTICE, 
        LC_TEXT("Celkem zabiti: %d - Celkem smrti: %d"), 
        m_player_stats[player_vid].iKill, m_player_stats[player_vid].iDead);
}

void CPvPRanking::AddMember(LPCHARACTER ch)
{
    if (!ch) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_data_mutex);
    
    const VID player_vid = ch->GetVID();
    
    // Add to participants
    m_participants.insert(player_vid);
    
    // Initialize stats if player has previous data
    const int prev_kills = ch->GetRankKill();
    const int prev_deaths = ch->GetRankDead();
    
    if (prev_kills > 0 || prev_deaths > 0) {
        m_player_stats[player_vid] = TPvPInfo(prev_kills, prev_deaths);
    }
    
    // Set PK mode
    ch->SetPKMode(PK_MODE_BATTLE);
    
    // Handle party dissolution
    if (LPPARTY party = ch->GetParty()) {
        if (party->GetMemberCount() <= 2) {
            CPartyManager::instance().DeleteParty(party);
        } else {
            party->Quit(ch->GetPlayerID());
        }
    }
    
    ch->ChatPacket(CHAT_TYPE_COMMAND, "pvp_empty_window");
}

void CPvPRanking::Check()
{
    std::lock_guard<std::mutex> lock(m_data_mutex);
    
    if (m_player_stats.empty() || m_participants.empty() || m_time_event_active.load()) {
        return;
    }
    
    SendRankingPacket();
    
    // Check if event should end
    if (m_is_ended.load() && m_player_stats.size() >= 2) {
        m_time_event_active.store(true);
        TimeEvent();
    }
}

void CPvPRanking::SendRankingPacket()
{
    // Create priority queue for sorting by kills (descending)
    using RankEntry = std::pair<int, VID>; // kills, player_vid
    std::priority_queue<RankEntry> ranking_queue;
    
    // Populate queue with player data
    for (const auto& entry : m_player_stats) {
        if (entry.second.iKill > 0 || entry.second.iDead > 0) {
            ranking_queue.emplace(entry.second.iKill, entry.first);
        }
    }
    
    // Prepare packet
    TPacketGCPvPRanking packet{};
    packet.bHeader = HEADER_GC_PVP_RANKING;
    packet.iMaxKill = m_max_kill.load();
    
#ifdef ENABLE_PVP_RANKING_WITH_EMPIRE
    packet.bIsRankEmpire = IsRankEmpire();
    
    // Set empire stats
    auto it1 = m_empire_stats.find(1);
    auto it2 = m_empire_stats.find(2);
    auto it3 = m_empire_stats.find(3);
    
    packet.llEmpire1 = (it1 != m_empire_stats.end()) ? it1->second : 0;
    packet.llEmpire2 = (it2 != m_empire_stats.end()) ? it2->second : 0;
    packet.llEmpire3 = (it3 != m_empire_stats.end()) ? it3->second : 0;
#endif
    
    // Fill packet with top 10 players
    BYTE count = 0;
    while (!ranking_queue.empty() && count < 10) {
        const auto rank_entry = ranking_queue.top();
        ranking_queue.pop();
        
        const VID vid = rank_entry.second;
        
        if (LPCHARACTER ch = CHARACTER_MANAGER::instance().Find(vid)) {
            if (ch->IsPC()) {
                const auto& stats = m_player_stats[vid];
                
                strlcpy(packet.szNames[count], ch->GetName(), sizeof(packet.szNames[count]));
                packet.bJobs[count] = ch->GetRaceNum();
                packet.bEmpire[count] = ch->GetEmpire();
                packet.bLevels[count] = ch->GetLevel();
                packet.iKill[count] = stats.iKill;
                packet.iDead[count] = stats.iDead;
                packet.bStatus[count] = 1;
                
                // Save to database if event ended
                if (m_is_ended.load()) {
                    char query[QUERY_MAX_LEN];
                    snprintf(query, sizeof(query), 
                        "INSERT INTO srv1_player.pvp_ranking_top (name, value) VALUES ('%s', %d)",
                        ch->GetName(), stats.iKill);
                    
                    std::unique_ptr<SQLMsg> db_result(DBManager::instance().DirectQuery(query));
                }
                
                ++count;
            }
        }
    }
    
    // Send packet to all participants
    for (const VID& member_vid : m_participants) {
        if (LPCHARACTER ch = CHARACTER_MANAGER::instance().Find(member_vid)) {
            if (ch->GetDesc()) {
                ch->GetDesc()->Packet(&packet, sizeof(packet));
            }
        }
    }
}

void CPvPRanking::End()
{
    m_status.store(0);
    quest::CQuestManager::instance().RequestSetEventFlag("pvp_ranking", 0);
    
    // Send end packet to all participants
    TPacketGCPvPRanking end_packet{};
    end_packet.bHeader = HEADER_GC_PVP_RANKING;
    end_packet.bStatus[0] = 0;
    
    {
        std::lock_guard<std::mutex> lock(m_data_mutex);
        for (const VID& member_vid : m_participants) {
            if (LPCHARACTER ch = CHARACTER_MANAGER::instance().Find(member_vid)) {
                if (ch->GetDesc()) {
                    ch->GetDesc()->Packet(&end_packet, sizeof(end_packet));
                }
            }
        }
    }
    
    GoHome();
    Reset();
}

void CPvPRanking::Start(BYTE min_level, BYTE max_level, int max_kill, DWORD map_index)
{
    m_status.store(1);
    m_max_kill.store(max_kill);
    m_is_ended.store(false);
    m_time_event_active.store(false);
    
    auto& quest_mgr = quest::CQuestManager::instance();
    quest_mgr.RequestSetEventFlag("pvp_ranking_minLevel", min_level);
    quest_mgr.RequestSetEventFlag("pvp_ranking_maxLevel", max_level);
    quest_mgr.RequestSetEventFlag("pvp_ranking_maxKill", max_kill);
    quest_mgr.RequestSetEventFlag("pvp_ranking_mapIndex", map_index);
}

// Const versions
BYTE CPvPRanking::GetMaxLevel() const
{
    return static_cast<BYTE>(quest::CQuestManager::instance().GetEventFlag("pvp_ranking_maxLevel"));
}

BYTE CPvPRanking::GetMinLevel() const
{
    return static_cast<BYTE>(quest::CQuestManager::instance().GetEventFlag("pvp_ranking_minLevel"));
}

DWORD CPvPRanking::GetMapIndex() const
{
    return 40;
}

bool CPvPRanking::CheckPlayerLevel(LPCHARACTER ch) const
{
    if (!ch) {
        return false;
    }
    
    const BYTE level = ch->GetLevel();
    return level >= GetMinLevel() && level <= GetMaxLevel();
}

// Non-const versions
BYTE CPvPRanking::GetStatus() noexcept
{
    return m_status.load();
}

BYTE CPvPRanking::GetMaxLevel()
{
    return static_cast<BYTE>(quest::CQuestManager::instance().GetEventFlag("pvp_ranking_maxLevel"));
}

BYTE CPvPRanking::GetMinLevel()
{
    return static_cast<BYTE>(quest::CQuestManager::instance().GetEventFlag("pvp_ranking_minLevel"));
}

DWORD CPvPRanking::GetMapIndex()
{
    return 40;
}

bool CPvPRanking::CheckPlayerLevel(LPCHARACTER ch)
{
    if (!ch) {
        return false;
    }
    
    const BYTE level = ch->GetLevel();
    return level >= GetMinLevel() && level <= GetMaxLevel();
}

#ifdef ENABLE_PVP_RANKING_WITH_EMPIRE
BYTE CPvPRanking::IsRankEmpire() const
{
    return static_cast<BYTE>(quest::CQuestManager::instance().GetEventFlag("empire_war_deathmatch"));
}
#endif

void CPvPRanking::BroadcastWinMessage(LPCHARACTER winner)
{
    if (!winner) return;
    
    char message[256];
    snprintf(message, sizeof(message), 
        "Hrac %s dosahl maximalniho poctu zabiti %d!",
        winner->GetName(), m_max_kill.load());
    
    BroadcastNotice(LC_TEXT(message));
}

void CPvPRanking::Reset()
{
    constexpr const char* reset_query = 
        "UPDATE srv1_player.player SET rank_Kill = 0, rank_Dead = 0";
    
    std::unique_ptr<SQLMsg> result(DBManager::instance().DirectQuery(reset_query));
    if (result->uiSQLErrno != 0) {
        sys_err("Failed to reset player ranking stats, error code: %u", result->uiSQLErrno);
    }
}

// Warp event structures and functions
struct FWarpAllToVillage
{
    FWarpAllToVillage() = default;
    void operator()(LPENTITY ent)
    {
        if (ent->IsType(ENTITY_CHARACTER))
        {
            LPCHARACTER ch = static_cast<LPCHARACTER>(ent);
            if (ch->IsPC())
            {
                const BYTE empire = ch->GetEmpire();
                if (empire == 0)
                {
                    sys_err("Unknown Empire %s %d", ch->GetName(), ch->GetPlayerID());
                    return;
                }

                ch->WarpSet(g_start_position[empire][0], g_start_position[empire][1]);
            }
        }
    }
};

EVENTINFO(warp_all_to_village_event_info)
{
    DWORD dwWarpMapIndex;

    warp_all_to_village_event_info() 
        : dwWarpMapIndex(0)
    {
    }
};

EVENTFUNC(warp_all_to_village_event)
{
    const auto* info = dynamic_cast<warp_all_to_village_event_info*>(event->info);

    if (!info)
    {
        sys_err("warp_all_to_village_event> <Factor> Null pointer");
        return 0;
    }

    LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(info->dwWarpMapIndex);

    if (pSecMap)
    {
        FWarpAllToVillage f;
        pSecMap->for_each(f);
    }

    return 0;
}

void CPvPRanking::GoHome()
{
    auto* info = AllocEventInfo<warp_all_to_village_event_info>();
    info->dwWarpMapIndex = GetMapIndex();

    event_create(warp_all_to_village_event, info, PASSES_PER_SEC(10));

    SendNoticeMap(LC_TEXT("Vsichni hraci budou teleportovani pryc z areny za 10s."), GetMapIndex(), false);
}

// Time event structures and functions
EVENTINFO(warp_all_to_time_event_info)
{
    DWORD dwWarpMapIndex;

    warp_all_to_time_event_info() 
        : dwWarpMapIndex(0)
    {
    }
};

EVENTFUNC(warp_all_to_time_event)
{
    const auto* info = dynamic_cast<warp_all_to_time_event_info*>(event->info);

    if (!info)
    {
        sys_err("warp_all_to_time_event> <Factor> Null pointer");
        return 0;
    }

    LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(info->dwWarpMapIndex);

    if (pSecMap)
    {
        CPvPRanking::Instance().End();
    }

    return 0;
}

void CPvPRanking::TimeEvent()
{
    auto* info = AllocEventInfo<warp_all_to_time_event_info>();
    info->dwWarpMapIndex = GetMapIndex();

    event_create(warp_all_to_time_event, info, PASSES_PER_SEC(30));

    SendNoticeMap(LC_TEXT("Vsichni budou teleportovani pryc z areny za 30 sekund."), GetMapIndex(), false);
}

// Base warp structures and functions
struct FWarpAllToBase
{
    FWarpAllToBase() = default;
    void operator()(LPENTITY ent)
    {
        if (ent->IsType(ENTITY_CHARACTER))
        {
            LPCHARACTER ch = static_cast<LPCHARACTER>(ent);
            if (ch->IsPC())
            {
                ch->WarpSet(716800, 1203200);
            }
        }
    }
};

EVENTINFO(warp_all_to_base_event_info)
{
    DWORD dwWarpMapIndex;

    warp_all_to_base_event_info() 
        : dwWarpMapIndex(0)
    {
    }
};

EVENTFUNC(warp_all_to_base_event)
{
    const auto* info = dynamic_cast<warp_all_to_base_event_info*>(event->info);

    if (!info)
    {
        sys_err("warp_all_to_base_event> <Factor> Null pointer");
        return 0;
    }

    LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(info->dwWarpMapIndex);

    if (pSecMap)
    {
        FWarpAllToBase f;
        pSecMap->for_each(f);
    }

    return 0;
}

void CPvPRanking::WarpToBase()
{
    auto* info = AllocEventInfo<warp_all_to_base_event_info>();
    info->dwWarpMapIndex = GetMapIndex();

    event_create(warp_all_to_base_event, info, PASSES_PER_SEC(1));
}

#endif // ENABLE_PVP_RANKING