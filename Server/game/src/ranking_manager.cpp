#include "stdafx.h"
#include "ranking_manager.h"

#include "buffer_manager.h"
#include "../../common/tables.h"
#include "char.h"
#include "config.h"
#include "db.h"
#include "desc_client.h"
#include "protocol.h"

#ifdef RANKING_SYSTEM
CRankingManager::CRankingManager()
{
	memset(&nextRankRefreshTime, 0, sizeof(nextRankRefreshTime));

	sundayEndTime = 0;
}

void CRankingManager::LoadRanking(LPCHARACTER ch, uint8_t catIdx)
{
	if (ch->GetActualRankLoadingType() != UINT8_MAX)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2425]");
		ch->ChatPacket(CHAT_TYPE_COMMAND, "load_rank no_clear %u", sundayEndTime);
		return;
	}

	if (ch->GetNextRankRefreshTime(catIdx) >= time(0))
	{
		if (test_server)
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2426]");
		ch->ChatPacket(CHAT_TYPE_COMMAND, "load_rank no_clear %u", sundayEndTime);
		return;
	}

	if (nextRankRefreshTime[catIdx] <= time(0))
	{
		ch->SetActualRankLoadingType(catIdx);
		ch->ChatPacket(CHAT_TYPE_INFO, "ch->SetActualRankLoadingType(catIdx)");
		TLoadRanking p;
		p.catIdx = catIdx;
		p.playerID = ch->GetPlayerID();
		db_clientdesc->DBPacket(HEADER_GD_RANKING_LOAD, ch->GetDesc()->GetHandle(), &p, sizeof(p));
		ch->ChatPacket(CHAT_TYPE_INFO, "HEADER_GD_RANKING_LOAD");
	}
	else
	{
		SendRankingToPlayer(ch, catIdx);
		ch->ChatPacket(CHAT_TYPE_INFO, "SendRankingToPlayer");
		LoadMyRankingPos(ch, catIdx);
		ch->ChatPacket(CHAT_TYPE_INFO, "LoadMyRankingPos");
	}
}

void CRankingManager::LoadMyRankingPos(LPCHARACTER ch, uint8_t catIdx)
{
	const auto& i = m_rankPlayerPoses[catIdx].find(ch->GetPlayerID());
	if (i == m_rankPlayerPoses[catIdx].end())
	{
		ch->SetActualRankLoadingType(catIdx);
		TLoadRanking p;
		p.catIdx = catIdx;
		p.playerID = ch->GetPlayerID();
		db_clientdesc->DBPacket(HEADER_GD_RANKING_LOAD_MY_POS, ch->GetDesc()->GetHandle(), &p, sizeof(p));

		m_rankPlayerPoses[catIdx][ch->GetPlayerID()] = {0, 0};
	}
	else
	{
		SendRankingMyPositionToPlayer(ch, catIdx);
	}
}

void CRankingManager::OnLoadRankingFromDB(LPCHARACTER ch, const char* c_pData)
{
	const uint32_t nextRefreshTime = decode_4bytes(c_pData);
	c_pData += sizeof(uint32_t);
	sundayEndTime = decode_4bytes(c_pData);
	c_pData += sizeof(uint32_t);

	const uint8_t loadingRankType = ch->GetActualRankLoadingType();

	ch->SetActualRankLoadingType(UINT8_MAX);
	m_vec_rankCache[loadingRankType].clear();
	nextRankRefreshTime[loadingRankType] = nextRefreshTime + 3;
	m_rankPlayerPoses[loadingRankType].clear();

	const uint32_t dwCount = decode_4bytes(c_pData);
	c_pData += sizeof(uint32_t);
	if (dwCount == 0)
	{
		ch->SetNextRankRefreshTime(loadingRankType, nextRefreshTime + 3);

		if (test_server)
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2427]");
		ch->ChatPacket(CHAT_TYPE_COMMAND, "load_rank clear %u", sundayEndTime);
	}
	else
	{
		const uint32_t myRankPos = decode_4bytes(c_pData);
		c_pData += sizeof(uint32_t);
		const uint32_t myRankValue = decode_4bytes(c_pData);
		c_pData += sizeof(uint32_t);
		m_rankPlayerPoses[loadingRankType][ch->GetPlayerID()] = { myRankPos, myRankValue };

		TRankTable* p = (TRankTable*)c_pData;
		for (uint32_t i = 0; i < dwCount; ++i, ++p)
			m_vec_rankCache[loadingRankType].push_back(*p);

		SendRankingToPlayer(ch, loadingRankType);
		SendRankingMyPositionToPlayer(ch, loadingRankType);
	}
}

void CRankingManager::OnLoadRankingMyPosFromDB(LPCHARACTER ch, const char* c_pData)
{
	const uint32_t myPos = decode_4bytes(c_pData);
	c_pData += sizeof(uint32_t);
	const uint32_t myValue = decode_4bytes(c_pData);
	//c_pData += sizeof(uint32_t);

	const uint8_t loadingRankType = ch->GetActualRankLoadingType();
	ch->SetActualRankLoadingType(UINT8_MAX);

	m_rankPlayerPoses[loadingRankType][ch->GetPlayerID()] = { myPos, myValue };

	SendRankingMyPositionToPlayer(ch, loadingRankType);
}

void CRankingManager::SendRankingToPlayer(LPCHARACTER ch, uint8_t catIdx)
{
	if (!ch->GetDesc())
		return;

	uint32_t nextRefreshTime = time(0) + 60 * 15 + 3;
	ch->SetNextRankRefreshTime(catIdx, nextRefreshTime);

	const uint32_t rankPlayersCount = m_vec_rankCache[catIdx].size();
	if (rankPlayersCount <= 0)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "load_rank clear %u", sundayEndTime);
		return;
	}
	
	TPacketGCRankingLoad pack;
	pack.header = HEADER_GC_RANKING_LOAD;
	pack.size = sizeof(pack) + rankPlayersCount * sizeof(TRankTable);
	pack.nextRefreshTime = nextRefreshTime;
	pack.sundayEndTime = sundayEndTime;

	TEMP_BUFFER buf;
	buf.write(&pack, sizeof(pack));
	for (uint32_t i = 0; i < rankPlayersCount; ++i)
		buf.write(&m_vec_rankCache[catIdx][i], sizeof(TRankTable));

	ch->GetDesc()->Packet(buf.read_peek(), buf.size());
}

void CRankingManager::SendRankingMyPositionToPlayer(LPCHARACTER ch, uint8_t catIdx)
{
	if (!ch->GetDesc())
		return;

	ch->SetNextRankRefreshTime(catIdx, time(0) + 60 * 15 + 5);		// nextRefreshTime

	const auto& i = m_rankPlayerPoses[catIdx].find(ch->GetPlayerID());
	if (i != m_rankPlayerPoses[catIdx].end())
		ch->ChatPacket(CHAT_TYPE_COMMAND, "load_rank_pos %u %u %u", catIdx, i->second.position, i->second.value);
	else
		ch->ChatPacket(CHAT_TYPE_COMMAND, "load_rank_pos %u 0 0", catIdx);
}

void CRankingManager::ResetRefreshTimes()
{
	memset(&nextRankRefreshTime, 0, sizeof(nextRankRefreshTime));
}

static const std::string rankTypeNames[RANK_TYPE_MAX_NUM] = { "stones", "bosses", "dungeons", "monsters", "alchemy", "refine", "dragon_coins" };
void CRankingManager::FirstLoadPlayerRankingData(LPCHARACTER ch)
{
	std::unique_ptr<SQLMsg> pkMsg(DBManager::instance().DirectQuery("SELECT pid FROM player_week_rank WHERE pid=%lu;", ch->GetPlayerID()));
	SQLResult* pRes = pkMsg->Get();
	if (pRes->uiNumRows < 1)
	{
		std::unique_ptr<SQLMsg> a(DBManager::instance().DirectQuery("INSERT INTO player_week_rank VALUES(%lu,0,0,0,0,0,0,0);", ch->GetPlayerID()));
	}
	ch->SetRankDataLoaded(true);
}

static const std::map<uint32_t, uint16_t> RANK_BOSSES_POINTS = {
	{2092, 1},
	{1093, 1},
	{9682, 2},
	{9694, 2},
	{9712, 3},
	{29754, 3},
	{16103, 4},
	{691, 1},
	{2091, 1},
	{3913, 1},
	{1901, 1},
	{3190, 2},
	{3191, 2},
	{8213, 2},
	{8214, 2},
	{6775, 3},
	{6783, 3},
	{6789, 3},
	{34600, 3},
	{34601, 3},
	{3390, 4},
	{3391, 4},
};
static const std::map<uint32_t, uint16_t> RANK_STONES_POINTS = {
	{8008, 1},
	{8009, 1},
	{2095, 1},
	{8013, 1},
	{8027, 1},
	{8052, 2},
	{8053, 2},
	{8210, 2},
	{8207, 2},
	{6798, 3},
	{8203, 3},
	{6801, 3},
	{6799, 3},
	{6802, 4},
	{6803, 4},
};
void CRankingManager::IncRankValue(LPCHARACTER ch, uint8_t catIdx, uint32_t extraData)
{
	if (!ch->GetDesc())
		return;

	if (!isRankingSendEnabled)	// Ranking zablokowany przez 5 minut (w niedziele o 23:55 az do wylosowania zwyciezcow o 23:59)
		return;

	if (!ch->IsRankDataLoaded())
		FirstLoadPlayerRankingData(ch);

	if (ch->GetMapIndex() >= 10000 && catIdx != RANK_TYPE_DUNGEONS)
		return;

	uint16_t addPoints = 1;
	if (catIdx == RANK_TYPE_BOSSES)
	{
		const auto& it = RANK_BOSSES_POINTS.find(extraData);
		if (it != RANK_BOSSES_POINTS.end())
			addPoints = it->second;
		else
			return;
	}
	else if (catIdx == RANK_TYPE_STONES)
	{
		const auto& it = RANK_STONES_POINTS.find(extraData);
		if (it != RANK_STONES_POINTS.end())
			addPoints = it->second;
		else
			return;
	}
	else if (catIdx == RANK_TYPE_DUNGEONS)
	{
		addPoints = static_cast<uint16_t>(extraData);
	}
	else if (catIdx == RANK_TYPE_DRAGON_COINS)
	{
		addPoints = static_cast<uint16_t>(extraData);
	}

	ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2428;%u;%u]", catIdx, addPoints);

	if (!ch->IsGM() || (ch->IsGM() && test_server))	// We dont want to insert GM to ranking
		DBManager::instance().Query("UPDATE player_week_rank SET %s=%s+%u WHERE pid=%u;", rankTypeNames[catIdx].c_str(), rankTypeNames[catIdx].c_str(), addPoints, ch->GetPlayerID());
}
#endif