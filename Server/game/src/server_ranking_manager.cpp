#include "stdafx.h"
#include "server_ranking_manager.h"

#include "buffer_manager.h"
#include "../../common/tables.h"
#include "char.h"
#include "config.h"
#include "db.h"
#include "desc_client.h"
#include "protocol.h"

#ifdef RANKING_SYSTEM
CServerRankingManager::CServerRankingManager()
{
	memset(&nextServerRankRefreshTime, 0, sizeof(nextServerRankRefreshTime));
}

void CServerRankingManager::LoadServerRanking(LPCHARACTER ch, uint8_t catIdx)
{
	if (ch->GetActualServerRankLoadingType() != UINT8_MAX)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2425]");
		ch->ChatPacket(CHAT_TYPE_COMMAND, "load_server_rank no_clear");
		return;
	}

	if (ch->GetNextServerRankRefreshTime(catIdx) >= time(0))
	{
		if (test_server)
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2426]");
		ch->ChatPacket(CHAT_TYPE_COMMAND, "load_server_rank no_clear");
		return;
	}

	if (nextServerRankRefreshTime[catIdx] <= time(0))
	{
		ch->SetActualServerRankLoadingType(catIdx);
		TLoadServerRanking p;
		p.catIdx = catIdx;
		p.playerID = ch->GetPlayerID();
		db_clientdesc->DBPacket(HEADER_GD_SERVER_RANKING_LOAD, ch->GetDesc()->GetHandle(), &p, sizeof(p));
	}
	else
	{
		SendServerRankingToPlayer(ch, catIdx);
		LoadMyServerRankingPos(ch, catIdx);
	}
}

void CServerRankingManager::LoadMyServerRankingPos(LPCHARACTER ch, uint8_t catIdx)
{
	const auto& i = m_serverRankPlayerPoses[catIdx].find(ch->GetPlayerID());
	if (i == m_serverRankPlayerPoses[catIdx].end())
	{
		ch->SetActualServerRankLoadingType(catIdx);
		TLoadServerRanking p;
		p.catIdx = catIdx;
		p.playerID = ch->GetPlayerID();
		db_clientdesc->DBPacket(HEADER_GD_SERVER_RANKING_LOAD_MY_POS, ch->GetDesc()->GetHandle(), &p, sizeof(p));

		m_serverRankPlayerPoses[catIdx][ch->GetPlayerID()] = {0, 0};
	}
	else
	{
		SendServerRankingMyPositionToPlayer(ch, catIdx);
	}
}

void CServerRankingManager::OnLoadServerRankingFromDB(LPCHARACTER ch, const char* c_pData)
{
	const uint32_t nextRefreshTime = decode_4bytes(c_pData);
	c_pData += sizeof(uint32_t);

	const uint8_t loadingRankType = ch->GetActualServerRankLoadingType();
	ch->SetNextServerRankRefreshTime(loadingRankType, time(0) + 60 * 15 + 5);		// nextRefreshTime

	ch->SetActualServerRankLoadingType(UINT8_MAX);
	m_vec_serverRankCache[loadingRankType].clear();
	nextServerRankRefreshTime[loadingRankType] = nextRefreshTime + 4;
	m_serverRankPlayerPoses[loadingRankType].clear();

	const uint32_t dwCount = decode_4bytes(c_pData);
	c_pData += sizeof(uint32_t);
	if (dwCount == 0)
	{
		if (test_server)
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2427]");
		ch->ChatPacket(CHAT_TYPE_COMMAND, "load_server_rank clear");
	}
	else
	{
		const uint32_t myRankPos = decode_4bytes(c_pData);
		c_pData += sizeof(uint32_t);
		const uint32_t myRankValue = decode_4bytes(c_pData);
		c_pData += sizeof(uint32_t);
		m_serverRankPlayerPoses[loadingRankType][ch->GetPlayerID()] = { myRankPos, myRankValue };

		TServerRankTable* p = (TServerRankTable*)c_pData;
		for (uint32_t i = 0; i < dwCount; ++i, ++p)
			m_vec_serverRankCache[loadingRankType].push_back(*p);

		SendServerRankingToPlayer(ch, loadingRankType);
		SendServerRankingMyPositionToPlayer(ch, loadingRankType);
	}
}

void CServerRankingManager::OnLoadServerRankingMyPosFromDB(LPCHARACTER ch, const char* c_pData)
{
	const uint32_t myPos = decode_4bytes(c_pData);
	c_pData += sizeof(uint32_t);
	const uint32_t myValue = decode_4bytes(c_pData);
	c_pData += sizeof(uint32_t);

	const uint8_t loadingRankType = ch->GetActualServerRankLoadingType();
	ch->SetActualServerRankLoadingType(UINT8_MAX);

	m_serverRankPlayerPoses[loadingRankType][ch->GetPlayerID()] = { myPos, myValue };
	ch->SetNextServerRankRefreshTime(loadingRankType, time(0) + 60 * 15 + 5);		// nextRefreshTime

	SendServerRankingMyPositionToPlayer(ch, loadingRankType);
}

void CServerRankingManager::SendServerRankingToPlayer(LPCHARACTER ch, uint8_t catIdx)
{
	if (!ch->GetDesc())
		return;

	const uint32_t rankPlayersCount = m_vec_serverRankCache[catIdx].size();
	if (rankPlayersCount <= 0)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "load_server_rank clear");
		return;
	}
	
	TPacketGCServerRankingLoad pack;
	pack.header = HEADER_GC_SERVER_RANKING_LOAD;
	pack.size = sizeof(pack) + rankPlayersCount * sizeof(TServerRankTable);

	TEMP_BUFFER buf;
	buf.write(&pack, sizeof(pack));
	for (uint32_t i = 0; i < rankPlayersCount; ++i)
		buf.write(&m_vec_serverRankCache[catIdx][i], sizeof(TServerRankTable));

	ch->GetDesc()->Packet(buf.read_peek(), buf.size());
}

void CServerRankingManager::SendServerRankingMyPositionToPlayer(LPCHARACTER ch, uint8_t catIdx)
{
	if (!ch->GetDesc())
		return;
	
	const auto& i = m_serverRankPlayerPoses[catIdx].find(ch->GetPlayerID());
	if (i != m_serverRankPlayerPoses[catIdx].end())
		ch->ChatPacket(CHAT_TYPE_COMMAND, "load_server_rank_pos %u %u %u", catIdx, i->second.position, i->second.value);
	else
		ch->ChatPacket(CHAT_TYPE_COMMAND, "load_server_rank_pos %u 0 0", catIdx);
}

static const std::string serverTankTypeNames[7] = { "stones", "bosses", "dungeons", "monsters", "alchemy", "refine", "dragon_coins" };
void CServerRankingManager::UpdateLevel(LPCHARACTER ch)
{
	if (!ch->GetDesc())
		return;

	if (ch->GetMapIndex() >= 10000 || ch->GetGMLevel() > GM_PLAYER)
		return;

	if (ch->GetLevel() >= 75)
		DBManager::instance().Query("REPLACE INTO player.player_ranking_levels(pid,`level`,levelup_time) VALUES(%u,%d,%d)", ch->GetPlayerID(), ch->GetLevel(), time(0));
}

void CServerRankingManager::IncServerRankValue(LPCHARACTER ch, uint8_t catIdx)
{
	if (!ch->GetDesc())
		return;
	
	if (ch->GetGMLevel() > GM_PLAYER)
		return;
	
	if (ch->GetMapIndex() >= 10000 && catIdx != SERVER_RANK_TYPE_DUNGEONS)
		return;

	DBManager::instance().Query("INSERT INTO player.player_ranking(pid,%s) VALUES(%u,1) ON DUPLICATE KEY UPDATE %s=%s+1", serverTankTypeNames[catIdx].c_str(), ch->GetPlayerID(), serverTankTypeNames[catIdx].c_str(), serverTankTypeNames[catIdx].c_str());
}
#endif