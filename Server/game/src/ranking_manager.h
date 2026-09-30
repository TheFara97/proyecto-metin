#pragma once
#include "../../common/tables.h"

#ifdef RANKING_SYSTEM

#define WEEKLY_RANK_MAX_NUMS 7

class CRankingManager : public singleton<CRankingManager>
{
	public:
		CRankingManager();
		void LoadRanking(LPCHARACTER ch, uint8_t catIdx);
		void LoadMyRankingPos(LPCHARACTER ch, uint8_t catIdx);
		void OnLoadRankingFromDB(LPCHARACTER ch, const char* c_pData);
		void OnLoadRankingMyPosFromDB(LPCHARACTER ch, const char* c_pData);
		void SendRankingToPlayer(LPCHARACTER ch, uint8_t catIdx);
		void SendRankingMyPositionToPlayer(LPCHARACTER ch, uint8_t catIdx);

		void ResetRefreshTimes();

		void FirstLoadPlayerRankingData(LPCHARACTER ch);
		void IncRankValue(LPCHARACTER ch, uint8_t catIdx, uint32_t extraData=0);

	private:
		std::vector<TRankTable>				m_vec_rankCache[WEEKLY_RANK_MAX_NUMS];
		time_t								nextRankRefreshTime[WEEKLY_RANK_MAX_NUMS];
		std::map<uint32_t, TRankMyData>		m_rankPlayerPoses[WEEKLY_RANK_MAX_NUMS];

		uint32_t							sundayEndTime;
};

#endif