#ifdef ENABLE_REAL_TIME_REGEN
#pragma once

#include "char.h"

#include <unordered_map>
#include <ctime>
#include <iostream>
typedef std::tuple<BYTE, DWORD, int32_t, int32_t, int32_t, int32_t, int32_t, std::vector<int32_t>> TRegen;
typedef std::tuple<int32_t, int32_t> TRegenTime;

static const std::string file_name = "real_time_regen.txt";
static const bool bRefreshRegen = true;
static const bool bRefreshAllMobs = false;
static const BYTE bRefreshAllMobsHour = 5;

enum ERegen : BYTE
{
	REGEN_TYPE,
	REGEN_VNUM,
	REGEN_MAP_INDEX,
	REGEN_X,
	REGEN_Y,
	REGEN_RANGE_X,
	REGEN_RANGE_Y,
	REGEN_TIMES,
};

enum ERegenType : BYTE
{
	REAL_TIME_REGEN_STANDARD,
	REAL_TIME_REGEN_PRECISE,
	REAL_TIME_REGEN_DEATH,	// per-mob timer starts on death (for stones)
};

class CRealTimeRegen : public singleton<CRealTimeRegen>
{
	public:
		CRealTimeRegen();
		~CRealTimeRegen() {};
		
	public:
		void Initialize();
		void Destroy();
		void Process();
		void OnMobDeath(const uint32_t wNum);

#ifdef ENABLE_RESP_SYSTEM
		static const std::tm GetTime();
		int GetNextSpawnTime(const uint32_t& wNum);
		int SecondsToSpawn(const int32_t hour, const int32_t minute);
#endif
	private:
		bool LoadRegen();
		void SpawnMonster(const uint32_t & wNum, const DWORD & dwVnum, const int32_t & lMapIndex, const int32_t & lX, const int32_t & lY, const int32_t & lRangeX, const int32_t & lRangeY);
		void SendRegenNotice(const std::string & szNotice);
		
	private:
		std::unordered_map<uint32_t, TRegen> mapRespawnTimes;
		std::unordered_map<uint32_t, TRegenTime> mapNextRespawn;
		std::unordered_map<uint32_t, int64_t> mapDeathTimes;	// wNum -> unix timestamp when to respawn (0 = alive)
		
		bool m_bIsInitialized;
		DWORD m_dwLastTick;
		DWORD m_dwLastMobsRefresh;
};
#endif
