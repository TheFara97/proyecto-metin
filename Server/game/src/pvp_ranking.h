#pragma once

#include <map>
#include <set>
#include <memory>
#include <atomic>
#include <mutex>

class CPvPRanking : public singleton<CPvPRanking>
{
public:
    struct TPvPInfo
    {
        int iKill = 0;
        int iDead = 0;
        
        TPvPInfo() = default;
        TPvPInfo(int kill_count, int death_count) 
            : iKill(kill_count), iDead(death_count) {}
    };

private:
    using TPvPRankMap = std::map<VID, TPvPInfo>;
#ifdef ENABLE_PVP_RANKING_WITH_EMPIRE
    using TEmpireRankMap = std::map<BYTE, long long>;
#endif

    // Thread-safe atomic members
    std::atomic<BYTE> m_status{0};
    std::atomic<int> m_max_kill{0};
    std::atomic<bool> m_is_ended{false};
    std::atomic<bool> m_time_event_active{false};
    
    // Thread-safe containers with mutex protection
    mutable std::mutex m_data_mutex;
    TPvPRankMap m_player_stats;
    std::set<VID> m_participants;
    
#ifdef ENABLE_PVP_RANKING_WITH_EMPIRE
    TEmpireRankMap m_empire_stats;
#endif

public:
    CPvPRanking();
    virtual ~CPvPRanking();
    
    // Delete copy operations for singleton
    CPvPRanking(const CPvPRanking&) = delete;
    CPvPRanking& operator=(const CPvPRanking&) = delete;
    
    // Main interface methods
    void AddKill(LPCHARACTER ch);
    void AddDead(LPCHARACTER ch);
    void AddMember(LPCHARACTER ch);
    void Check();
    void Destroy();
    void End();
    void Start(BYTE min_level, BYTE max_level, int max_kill, DWORD map_index);
    
    // Getters - providing both const and non-const versions
    BYTE GetStatus() const noexcept { return m_status.load(); }
    BYTE GetStatus() noexcept;
    
    BYTE GetMaxLevel() const;
    BYTE GetMaxLevel();
    
    BYTE GetMinLevel() const;
    BYTE GetMinLevel();
    
    DWORD GetMapIndex() const;
    DWORD GetMapIndex();
    
    bool CheckPlayerLevel(LPCHARACTER ch) const;
    bool CheckPlayerLevel(LPCHARACTER ch);
    
#ifdef ENABLE_PVP_RANKING_WITH_EMPIRE
    BYTE IsRankEmpire() const;
#endif
    
    // Event methods
    void GoHome();
    void TimeEvent();
    void WarpToBase();
    void Reset();

private:
    // Helper methods
    void BroadcastWinMessage(LPCHARACTER winner);
    void SendRankingPacket();
    bool IsEventActive() const noexcept { return !m_is_ended.load(); }
};