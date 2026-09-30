#pragma once
#include "../../common/tables.h"

#ifdef RANKING_SYSTEM
class CServerRankingManager : public singleton<CServerRankingManager>
{
	public:
		CServerRankingManager();
		void LoadServerRanking(LPCHARACTER ch, uint8_t catIdx);
		void LoadMyServerRankingPos(LPCHARACTER ch, uint8_t catIdx);
		void OnLoadServerRankingFromDB(LPCHARACTER ch, const char* c_pData);
		void OnLoadServerRankingMyPosFromDB(LPCHARACTER ch, const char* c_pData);
		void SendServerRankingToPlayer(LPCHARACTER ch, uint8_t catIdx);
		void SendServerRankingMyPositionToPlayer(LPCHARACTER ch, uint8_t catIdx);

		void UpdateLevel(LPCHARACTER ch);
		void IncServerRankValue(LPCHARACTER ch, uint8_t catIdx);

	private:
		std::vector<TServerRankTable>			m_vec_serverRankCache[6];
		time_t									nextServerRankRefreshTime[6];
		std::map<uint32_t, TServerRankMyData>	m_serverRankPlayerPoses[6];
};

#endif