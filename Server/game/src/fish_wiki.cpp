#include "stdafx.h"
#include "fish_wiki.h"

#include "buffer_manager.h"
#include "char.h"
#include "char_manager.h"
#include "db.h"
#include "desc.h"
#include "desc_client.h"
#include "item.h"
#include "item_manager.h"
#include "protocol.h"
#include "questmanager.h"
#include "utils.h"

CFishWikiManager::CFishWikiManager()
{
	// KOLEJNOSC MUSI BYC ZACHOWANA IDENTYCZNA JAK W KLIENCIE!
	// DODAJAC NOWE RYBKI JUZ PO URUCHOMIENIU SERWERA MUSISZ DODAWAC ZAWSZE NA KONIEC, NIGDY NIE ZMIENIASZ KOLEJNOSCI!
	// MAX 62 RYBKI INACZEJ TRZEBA PRZEROBIC SYSTEMIK!
	fishVnums = {
		27802, 27803, 27804,
		27805, 27806, 27807,
		27808, 27809, 27810,
		27811, 27812, 27813,
		27814, 27815, 27816,
		27817, 27818, 27819,
		27820, 27821, 27822,
		27823, 27824, 27825,
		27826, 27827, 27828,
		27829, 27830, 27831,
		27832,
	};
// vnum, {ilosc, minimalne_cm, szansa, type_bonusu, wartosc_bonusu}
    fishMissionsData = {
		{27802, {25, 18, 80, 53, 20}},        //"Hodnota utoku +20"], #EASY
		{27803, {25, 40, 80, 5, 5}},          //"Síla +5"],    #EASY
		{27804, {25, 9, 80, 4, 5}},       //"Inteligence +5"],    #EASY
		{27805, {25, 50, 80, 6, 5}},       //"Pohyblivost +5"],    #EASY
		{27806, {25, 105, 80, 66, 25}},     //"Bonus Doświadczenia 25%"],    #MEDIUM
		{27807, {25, 180, 70, 15, 20}},      //"Szansa na cios Krytyczny 20%"],    #EASY
		{27808, {25, 180, 70, 16, 20}},      //"Szansa na Przeszywające Uderzenie 20%"],    #EASY
		{27809, {25, 95, 70, 19, 20}},      //"Silny proti orkum +20%"],    #EASY
		{27810, {25, 85, 70, 71, 5}},      //"Poskozeni schopnosti +5%"],    #EASY
		{27811, {25, 60, 70, 73, 5}},      //"Odolnost proti poskozeni schopnosti +5%"],    #EASY
		{27812, {20, 100, 60, 78, 3}},    //"Odolnost proti valecnikum +3%"],    #MEDIUM
		{27813, {20, 50, 60, 79, 3}},    //"Odolnost proti surum +3%"],    #MEDIUM
		{27814, {20, 100, 60, 80, 3}},    //"Odolnost proti ninjum +3%"],    #MEDIUM
		{27815, {20, 65, 60, 81, 3}},      //"Odolnost proti samanum +3%"],    #MEDIUM
		{27816, {20, 313, 60, 53, 30}},      //"Hodnota utoku +30"],    #MEDIUM
		{27817, {20, 50, 60, 53, 50}},      //"Hodnota utoku +50"],    #MEDIUM
		{27818, {20, 82, 50, 1, 2000}},      //"Max. ZB: +2000"],    #MEDIUM
		{27819, {20, 66, 50, 1, 3000}},      //"Max. ZB: +3000"],    #MEDIUM
		{27820, {20, 34, 50, 1, 4000}},     //"Max. ZB: +4000"],    #MEDIUM
		{27821, {15, 175, 50, 72, 3}},     //"Prumerna skoda +3%"],        #MEDIUM
		{27822, {15, 106, 50, 114, 15}},     //"Silný proti legendam +15%"],    #HARD
		{27823, {15, 106, 50, 108, 10}},     //"Silny proti balvanum +10%"],    #HARD end
		{27824, {15, 18, 50, 1, 15000}},     //"Max PŻ +15000"],    #HARD
		{27825, {10, 37, 50, 78, 3}},     //"Odporność na Wojowników 3%"],    #HARD
		{27826, {10, 58, 50, 80, 3}},     //"Odporność na Sury 3%"],    #HARD
		{27827, {10, 18, 50, 79, 3}},    //"Odporność na Ninje 3%"],    #HARD
		{27828, {10, 135, 50, 81, 3}},     //"Odporność na Szamanów 3%"],    #HARD
		{27829, {10, 37, 50, 17, 20}},      //"Silny przeciwko Ludziom 20%"],    #HARD
		{27830, {10, 230, 50, 103, 3}},      //"Zwiększenie ataku Krytycznego 3%"],    #HARD
		{27831, {10, 17, 50, 102, 5}},      //"Odporność na ludzi 5%"],    #HARD
		{27832, {10, 17, 50, 125, 5}},     //"Przebicie odporności na Ludzi +5%"],    #HARD
		// {27833, {10, 200, 35, 63, 10}},
		// {27844, {10, 200, 35, 63, 10}},
		// {27845, {10, 200, 35, 63, 10}},
		// {27846, {10, 200, 35, 63, 10}},
		// {27847, {10, 200, 35, 63, 10}},
		// {27848, {10, 200, 35, 63, 10}},
		// {27849, {10, 200, 35, 63, 10}},
		// {27850, {10, 200, 35, 63, 10}},
		// {27851, {10, 200, 35, 63, 10}},
	};
}

CFishWikiManager::~CFishWikiManager()
{
	fishVnums.clear();
	fishMissionsData.clear();
	nextFishVnumRankRefreshTime.clear();
}

uint8_t CFishWikiManager::GetFishRealIdx(uint32_t fishVnum)
{
	const auto it = std::find(fishVnums.begin(), fishVnums.end(), fishVnum);
	if (it == fishVnums.end())
		return UINT8_MAX;

	return static_cast<uint8_t>(std::distance(fishVnums.begin(), it));
}

void CFishWikiManager::LoadFishRanking(CFishWiki* fishWiki, uint32_t fishVnum)
{
	LPCHARACTER ch = fishWiki->GetCharacter();
	if (!ch)
		return;

	if (fishWiki->GetActualFishRankLoadingVnum() != 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Pouze jeden typ zebricek ryb muze byt nacten najednou!");
		ch->ChatPacket(CHAT_TYPE_COMMAND, "fishw_loader cant");
		return;
	}

	if (fishWiki->GetNextFishVnumRankRefreshTime(fishVnum) >= time(0))
	{
		if (test_server)
			ch->ChatPacket(CHAT_TYPE_INFO, "[TEST_SERVER] Tvuj zebricek je jiz aktualni!");
		ch->ChatPacket(CHAT_TYPE_COMMAND, "fishw_loader cant");
		return;
	}

	if (nextFishVnumRankRefreshTime[fishVnum] <= time(0))
	{
		fishWiki->SetActualFishRankLoadingVnum(fishVnum);
		TLoadFishRanking p;
		p.fishVnum = fishVnum;
		p.playerID = ch->GetPlayerID();
		db_clientdesc->DBPacket(HEADER_GD_FISH_VNUM_RANKING_LOAD, ch->GetDesc()->GetHandle(), &p, sizeof(p));
	}
	else
	{
		SendFishVnumRankingToPlayer(ch, fishVnum);
	}
}

void CFishWikiManager::OnLoadFishVnumRankingFromDB(LPCHARACTER ch, const char* c_pData)
{
	const uint32_t nextRefreshTime = decode_4bytes(c_pData);
	c_pData += sizeof(uint32_t);

	CFishWiki* fishWiki = ch->GetFishWiki();
	if (!fishWiki)
		return;

	const uint32_t loadingFishVnum = fishWiki->GetActualFishRankLoadingVnum();
	fishWiki->SetNextFishVnumRankRefreshTime(loadingFishVnum, time(0) + 60 * 15 + 5);		// nextRefreshTime

	fishWiki->SetActualFishRankLoadingVnum(0);
	fishVnumRankCache[loadingFishVnum].clear();
	nextFishVnumRankRefreshTime[loadingFishVnum] = nextRefreshTime + 4;

	const uint32_t dwCount = decode_4bytes(c_pData);
	c_pData += sizeof(uint32_t);
	if (dwCount == 0)
	{
		if (test_server)
			ch->ChatPacket(CHAT_TYPE_INFO, "[TEST_SERVER] V tomto zebricku neni zadna postava!");
		ch->ChatPacket(CHAT_TYPE_COMMAND, "fishw_loader empty");
	}
	else
	{
		TFishVnumRankTable* p = (TFishVnumRankTable*)c_pData;
		for (uint32_t i = 0; i < dwCount; ++i, ++p)
			fishVnumRankCache[loadingFishVnum].push_back(*p);

		SendFishVnumRankingToPlayer(ch, loadingFishVnum);
	}
}

void CFishWikiManager::SendFishVnumRankingToPlayer(LPCHARACTER ch, uint32_t fishVnum)
{
	if (!ch->GetDesc())
		return;

	const uint32_t rankPlayersCount = fishVnumRankCache[fishVnum].size();
	if (rankPlayersCount <= 0)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "fishw_ranks");
		ch->ChatPacket(CHAT_TYPE_COMMAND, "fishw_ranke");
		return;
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, "fishw_ranks");
	for (uint32_t i = 0; i < rankPlayersCount; ++i)
		ch->ChatPacket(CHAT_TYPE_COMMAND, "fishw_rankd %u %s %u %u", i, fishVnumRankCache[fishVnum][i].szName, fishVnumRankCache[fishVnum][i].fishLength, fishVnumRankCache[fishVnum][i].rodLevel);
	ch->ChatPacket(CHAT_TYPE_COMMAND, "fishw_ranke");
}

CFishWiki::CFishWiki(LPCHARACTER _ch) : ch(_ch), actualFishRankLoadingVnum(0), isWindowOpened(false), isFirstDataLoaded(false)
{
	LoadPlayerData();
}

CFishWiki::~CFishWiki()
{
	ch = NULL;
	nextFishVnumRankRefreshTime.clear();
	myFishData.clear();
	needUpdateVnum.clear();
}

void CFishWiki::LoadPlayerData()
{
	std::unique_ptr<SQLMsg> caughtFishDataMsg(DBManager::instance().DirectQuery("SELECT vnum,best_length,best_price FROM fish_rank WHERE pid=%u;", ch->GetPlayerID()));
	if (caughtFishDataMsg->Get()->uiNumRows > 0)
	{
		MYSQL_ROW row = NULL;
		while ((row = mysql_fetch_row(caughtFishDataMsg->Get()->pSQLResult)))
		{
			uint32_t fishVnum = 0;
			str_to_number(fishVnum, row[0]);
			str_to_number(myFishData[fishVnum].first, row[1]);
			str_to_number(myFishData[fishVnum].second, row[2]);
		}
	}
}

void CFishWiki::OpenWindow()
{
	isWindowOpened = true;

	if (!isFirstDataLoaded)
	{
		isFirstDataLoaded = true;
		
		SendBaseInfo();
		needUpdateVnum.clear();

		return;
	}

	SendBaseInfo();

	SendAllNeededUpdateFishData();
}

void CFishWiki::CloseWindow()
{
	isWindowOpened = false;
}

void CFishWiki::SendBaseInfo()
{
	if (!ch->GetDesc())
		return;

	if (!isWindowOpened)
		return;

	quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
	if (!pPC)
		return;

	ch->ChatPacket(CHAT_TYPE_COMMAND, "fishw_ustat %d %d", pPC->GetFlag("fish_wiki.unlocks0"), pPC->GetFlag("fish_wiki.unlocks1"));
}

void CFishWiki::SendFishData(uint32_t fishVnum)
{
	if (CFishWikiManager::instance().GetFishRealIdx(fishVnum) == UINT8_MAX)
		return;

	quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
	if (!pPC)
		return;

	const auto it = myFishData.find(fishVnum);
	if (it != myFishData.end())
		MyFishDataPacket(fishVnum, pPC->GetFlag("fish_wiki.count" + std::to_string(fishVnum)), it->second.first, it->second.second, pPC->GetFlag("fish_wiki.missionc" + std::to_string(fishVnum)));
	else
		MyFishDataPacket(fishVnum, 0, 0, 0, 0);
}

void CFishWiki::MyFishDataPacket(uint32_t fishVnum, int caughtCount, int bestLength, int bestPrice, int missionProgress)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "fishw_fdata %u %d %d %d %d", fishVnum, caughtCount, bestLength, bestPrice, missionProgress);
}

void CFishWiki::SendAllNeededUpdateFishData()
{
	if (needUpdateVnum.empty())
		return;

	quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
	if (!pPC)
		return;

	for (const auto& fishVnum : needUpdateVnum)
	{
		const auto it = myFishData.find(fishVnum);
		if (it != myFishData.end())
			MyFishDataPacket(fishVnum, pPC->GetFlag("fish_wiki.count" + std::to_string(fishVnum)), it->second.first, it->second.second, pPC->GetFlag("fish_wiki.missionc" + std::to_string(fishVnum)));
		else
			MyFishDataPacket(fishVnum, 0, 0, 0, 0);
	}
}

void CFishWiki::GiveFishMission(uint32_t fishVnum)
{
	if (CFishWikiManager::instance().GetFishRealIdx(fishVnum) == UINT8_MAX)
		return;

	quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
	if (!pPC)
		return;

	const auto it = myFishData.find(fishVnum);
	if (it == myFishData.end())
	{
		if (test_server)
			ch->ChatPacket(CHAT_TYPE_INFO, "[TEST_SERVER] Nejprve musis sam vylovit rybu k uspesnemu odevzdani!");
		return;
	}

	auto missionData = CFishWikiManager::instance().GetFishMissionData(fishVnum);
	if (!missionData)
		return;

	int givedCount = pPC->GetFlag("fish_wiki.missionc" + std::to_string(fishVnum));
	if (givedCount >= missionData->neededCount)
		return;

	const int needSize = missionData->neededLength * 100;

	bool isSuccess = false;
	const uint16_t totalInventorySlots = INVENTORY_MAX_NUM + 
										SKILL_BOOK_INVENTORY_MAX_NUM + 
										UPGRADE_ITEMS_INVENTORY_MAX_NUM + 
										STONE_INVENTORY_MAX_NUM + 
										BOX_INVENTORY_MAX_NUM + 
										EFSUN_INVENTORY_MAX_NUM + 
										CICEK_INVENTORY_MAX_NUM;
	
	for (uint16_t i = 0; i < totalInventorySlots; i++)
	{
		LPITEM tempItem = ch->GetInventoryItem(i);
		if (!tempItem)
			continue;
		if (tempItem->GetVnum() == fishVnum && tempItem->GetSocket(0) >= needSize)
		{
			isSuccess = true;
			tempItem->SetCount(tempItem->GetCount() - 1);
			break;
		}
	}

	if (!isSuccess)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2442]");
		return;
	}

	if (number(1, 100) <= missionData->successChance)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2443]");

		givedCount++;
		pPC->SetFlag("fish_wiki.missionc" + std::to_string(fishVnum), givedCount);

		MyFishDataPacket(fishVnum, pPC->GetFlag("fish_wiki.count" + std::to_string(fishVnum)), it->second.first, it->second.second, givedCount);

		if (givedCount >= missionData->neededCount)
		{
			int value = missionData->rewardAffectValue;
			CAffect* pkAff = ch->FindAffect(AFFECT_FISH_WIKI_MISSION, aApplyInfo[missionData->rewardAffectType].bPointType);
			if (pkAff)
			{
				value += pkAff->lApplyValue;
				ch->RemoveAffect(pkAff);
			}

			ch->AddAffect(AFFECT_FISH_WIKI_MISSION, aApplyInfo[missionData->rewardAffectType].bPointType, value, 0, INFINITE_AFFECT_DURATION, 0, false);
		}
	}
	else
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2444]");
}

void CFishWiki::LoadFishRanking(uint32_t fishVnum)
{
	if (CFishWikiManager::instance().GetFishRealIdx(fishVnum) == UINT8_MAX)
		return;

	CFishWikiManager::instance().LoadFishRanking(this, fishVnum);
}

void CFishWiki::CatchFish(uint32_t fishVnum, int fishLength, int fishPrice, int rodLevel)
{
	uint8_t fishRealIdx = CFishWikiManager::instance().GetFishRealIdx(fishVnum);
	if (fishRealIdx == UINT8_MAX)
		return;

	quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
	if (!pPC)
		return;

	pPC->SetFlag("fish_wiki.count" + std::to_string(fishVnum), pPC->GetFlag("fish_wiki.count" + std::to_string(fishVnum))+1);

	const auto it = myFishData.find(fishVnum);
	if (it != myFishData.end())
	{
		std::string updateQr = "";
		if (fishLength > it->second.first)
		{
			it->second.first = fishLength;

			updateQr = "best_length=" + std::to_string(fishLength) + ",";
			updateQr += "best_length_rod_level=" + std::to_string(rodLevel);
		}

		if (fishPrice > it->second.second)
		{
			it->second.second = fishPrice;
			if (!updateQr.empty())
				updateQr += ",";
			updateQr += "best_price=" + std::to_string(fishPrice);
		}

		if (!updateQr.empty())
			std::unique_ptr<SQLMsg> fishUpdateQr(DBManager::instance().DirectQuery("UPDATE fish_rank SET %s WHERE pid=%u AND vnum=%u;", updateQr.c_str(), ch->GetPlayerID(), fishVnum));
	}
	else
	{
		std::unique_ptr<SQLMsg> fishUpdateQr(DBManager::instance().DirectQuery("INSERT INTO fish_rank(pid,vnum,best_length,best_length_rod_level,best_price) VALUES(%u,%u,%d,%d,%d);", ch->GetPlayerID(), fishVnum, fishLength, rodLevel, fishPrice));

		ch->ChatPacket(CHAT_TYPE_COMMAND, "fishw_new %u", fishVnum);

		const int subIdx = fishRealIdx / 31;
		fishRealIdx = fishRealIdx % 31;
		int unlockStatus = pPC->GetFlag("fish_wiki.unlocks" + std::to_string(subIdx));
		SET_BIT(unlockStatus, (1 << fishRealIdx));
		pPC->SetFlag("fish_wiki.unlocks" + std::to_string(subIdx), unlockStatus);

		myFishData[fishVnum].first = fishLength;
		myFishData[fishVnum].second = fishPrice;

		SendBaseInfo();
	}

	UpdateOneFishData(fishVnum);
}

void CFishWiki::UpdateOneFishData(uint32_t fishVnum)
{
	if (isWindowOpened)
		SendFishData(fishVnum);
	else if (std::find(needUpdateVnum.begin(), needUpdateVnum.end(), fishVnum) == needUpdateVnum.end())
		needUpdateVnum.push_back(fishVnum);
}
