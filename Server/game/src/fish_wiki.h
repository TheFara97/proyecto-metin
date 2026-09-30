#pragma once
#include "../../common/tables.h"
#include "packet.h"

class CFishWiki;

class CFishWikiManager : public singleton<CFishWikiManager>
{
	struct SFishMissionData
	{
		uint8_t neededCount;
		int		neededLength;
		uint8_t	successChance;
		uint8_t	rewardAffectType;
		int		rewardAffectValue;
	};

	public:
		CFishWikiManager();
		~CFishWikiManager();

		uint8_t GetFishRealIdx(uint32_t fishVnum);
		const SFishMissionData* GetFishMissionData(uint32_t fishVnum) { return &fishMissionsData.at(fishVnum); }
		
		void LoadFishRanking(CFishWiki* fishWiki, uint32_t fishVnum);
		void OnLoadFishVnumRankingFromDB(LPCHARACTER ch, const char* c_pData);
		void SendFishVnumRankingToPlayer(LPCHARACTER ch, uint32_t fishVnum);

	private:
		std::vector<uint32_t> fishVnums;
		std::map<uint32_t, SFishMissionData> fishMissionsData;

		std::map<uint32_t, std::vector<TFishVnumRankTable>>	fishVnumRankCache;
		std::map<uint32_t, time_t> nextFishVnumRankRefreshTime;
};

class CFishWiki
{
	public:
		CFishWiki(LPCHARACTER _ch);
		~CFishWiki();
		void LoadPlayerData();

		LPCHARACTER GetCharacter() { return ch; }

		uint32_t	GetActualFishRankLoadingVnum() { return actualFishRankLoadingVnum; }
		void		SetActualFishRankLoadingVnum(uint32_t fishVnum) { actualFishRankLoadingVnum = fishVnum; }

		time_t		GetNextFishVnumRankRefreshTime(uint32_t fishVnum) { return nextFishVnumRankRefreshTime[fishVnum]; }
		void		SetNextFishVnumRankRefreshTime(uint32_t fishVnum, time_t newTime) { nextFishVnumRankRefreshTime[fishVnum] = newTime; }
		
		void OpenWindow();
		void CloseWindow();

		void SendBaseInfo();
		void SendFishData(uint32_t fishVnum);
		void MyFishDataPacket(uint32_t fishVnum, int caughtCount, int bestLength, int bestPrice, int missionProgress);
		void SendAllNeededUpdateFishData();
		void GiveFishMission(uint32_t fishVnum);
		void LoadFishRanking(uint32_t fishVnum);
		void CatchFish(uint32_t fishVnum, int fishLength, int fishPrice, int rodLevel);
		void UpdateOneFishData(uint32_t fishVnum);

	private:
		LPCHARACTER ch;

		uint32_t actualFishRankLoadingVnum;
		std::map<uint32_t, time_t> nextFishVnumRankRefreshTime;

		bool isWindowOpened;
		bool isFirstDataLoaded;

		std::map<uint32_t, std::pair<int,int>> myFishData;	// vnum, <best_length, best_price>
		std::vector<uint32_t> needUpdateVnum;
};
