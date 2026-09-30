#pragma once
#include <array>
#include <vector>
#include <string>
#include "../../common/tables.h"
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/error/en.h>

//  In-memory mission definition  (loaded from battlepass.json)
struct MissionDef
{
	uint16_t             id        = 0;
	std::string          name;
	uint8_t              trigger   = 0; // EBattlePassMissionTypes
	bool                 anyTarget = true; // false => only fire if subject is in targets[]
	std::vector<uint32_t> targets; // mob race vnum or item vnum
	uint32_t             goal      = 1;
	uint16_t             pts       = 0;
	uint8_t              track     = 0; // MISSION_TIME_*

	bool MatchTarget(uint32_t subject) const
	{
		if (anyTarget) return true;
		return std::find(targets.begin(), targets.end(), subject) != targets.end();
	}
};

// ─────────────────────────────────────────────────────────────────────────────
//  Global season + event config  (loaded from battlepass.json)
// ─────────────────────────────────────────────────────────────────────────────
struct BPConfig
{
	uint64_t seasonEnd      = 0;
	uint32_t pointsPerLevel = 10000;
	uint8_t  maxLevel       = 30;
	uint8_t  dailySlots     = 8;
	uint8_t  weeklySlots    = 8;
	uint32_t premiumItem    = 80045;

	uint64_t eventEnd       = 0;
	uint8_t  eventMaxLevel  = 32;
	uint8_t  eventSlots     = 15;
	bool     eventActive    = false;

	std::vector<std::pair<uint32_t, uint32_t>> rewardsFree;
	std::vector<std::pair<uint32_t, uint32_t>> rewardsPremium;
	std::vector<std::pair<uint32_t, uint32_t>> rewardsEvent;
};

// ─────────────────────────────────────────────────────────────────────────────
//  BattlePassManager
// ─────────────────────────────────────────────────────────────────────────────
class BattlePassManager : public singleton<BattlePassManager>
{
public:
	BattlePassManager()           = default;
	~BattlePassManager() override = default;

	// Load everything from battlepass.json
	void Boot();

	// subject = mob race num (kills) OR item vnum (items) OR 0 (generic)
	void Notify(uint8_t trigger, LPCHARACTER ch, uint32_t subject, uint32_t amount = 1);

	// Event track: returns all missions in order.
	std::vector<const MissionDef*> DrawMissions(uint8_t track) const;

	const MissionDef* GetDef(uint16_t id) const;
	const BPConfig&   GetConf()           const { return m_conf; }

private:
	BPConfig                           m_conf;
	std::vector<MissionDef>            m_defs; // flat, index == id
	std::array<std::vector<size_t>, 3> m_pool; // [track] -> m_defs indices

	static uint8_t ParseTrigger(const std::string& s);
	static uint8_t ParseTrack  (const std::string& s);
	static void    LoadRewardList(const rapidjson::Value& arr,
								std::vector<std::pair<uint32_t, uint32_t>>& out);
};
