#include "stdafx.h"
#ifdef __FreeBSD__
#include <md5.h>
#else
#include "../../libthecore/include/xmd5.h"
#endif
#ifdef __ENABLE_COLLECTIONS_SYSTEM__
	#include "CollectionsSystem.hpp"
#endif
#ifdef ENABLE_NEWSTUFF
#include "../../common/PulseManager.h"
#endif
#include "utils.h"
#include "config.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "motion.h"
#include "packet.h"
#include "affect.h"
#include "pvp.h"
#include "start_position.h"
#include "party.h"
#include "guild_manager.h"
#include "p2p.h"
#include "dungeon.h"
#include "messenger_manager.h"
#include "war_map.h"
#include "questmanager.h"
#include "item_manager.h"
#include "monarch.h"
#include "mob_manager.h"
#include "item.h"
#include "arena.h"
#include "buffer_manager.h"
#include "unique_item.h"
#include "threeway_war.h"
#include "log.h"
#include "../../common/VnumHelper.h"
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	#include "offlineshop_manager.h"
#endif

#ifdef ENABLE_DRAGON_SOUL_TIME_RENEWAL
#include "DragonSoul.h"
#endif

#include "target.h"
#include "shop.h"

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
#include "MountSystem.h"
#endif
#ifdef __DUNGEON_INFO_ENABLE__
	#include "DungeonInfoManager.hpp"
#endif
#include "fish_wiki.h"

#ifdef __WORLD_BOSS_YUMA__
#include "worldboss.h"
#endif
#ifdef ENABLE_PVP_RANKING
#include "pvp_ranking.h"
#endif
#ifdef _ENABLE_BATTLEPASS_
#include "BattlePassManager.h"
#endif
#include "MonsterSpawner.hpp"
#include "argparser.h"

#include "shop_manager.h"

#ifdef ENABLE_DRAGON_SOUL_TIME_RENEWAL
ACMD(do_renew_alchemy)
{
#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::RenewDS, std::chrono::milliseconds(30000))) {
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2200;%f]", PULSEMANAGER_CLOCK_TO_SEC2(ch->GetPlayerID(), ePulse::RenewDS));
		return;
	}
#endif

	int price = 200000000;
	if (ch->GetGold() < price) {
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2311]");
		return;
	}

	for (int i = DRAGON_SOUL_EQUIP_SLOT_START; i < DRAGON_SOUL_EQUIP_SLOT_END; i++)
	{
		LPITEM item = ch->GetInventoryItem(i);
		if (item != 0 || NULL) {
			item->SetSocket(0, 86400);
		}
	}
	ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2312]");
	ch->PointChange(POINT_GOLD, -price);
}
#endif


ACMD(do_fish_wiki)
{
	if (!ch->CanWarp() || quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID())->IsRunning())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2356]");
		ch->ChatPacket(CHAT_TYPE_COMMAND, "fishw_err cant_now");
		return;
	}

	quest::PC* pPC = quest::CQuestManager::instance().GetPC(ch->GetPlayerID());
	if (!pPC)
		return;

	char arg1[16];
	const char* rest = one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1)
		return;

	uint8_t actionIdx = 0;
	str_to_number(actionIdx, arg1);
	if (actionIdx < 1 || actionIdx > 4)
		return;

	// close window
	if (actionIdx == 1)
	{
		ch->GetFishWiki()->CloseWindow();
	}
	// click fish
	else if (actionIdx == 2)
	{
		char arg2[16];
		rest = one_argument(rest, arg2, sizeof(arg2));
		if (!*arg2)
			return;

		uint32_t fishVnum;
		str_to_number(fishVnum, arg2);
		
		ch->GetFishWiki()->SendFishData(fishVnum);
	}
	// click give fish
	else if (actionIdx == 3)
	{
		char arg2[16];
		rest = one_argument(rest, arg2, sizeof(arg2));
		if (!*arg2)
			return;

		uint32_t fishVnum;
		str_to_number(fishVnum, arg2);

		ch->GetFishWiki()->GiveFishMission(fishVnum);
	}
	// click ranking
	else if (actionIdx == 4)
	{
		char arg2[16];
		rest = one_argument(rest, arg2, sizeof(arg2));
		if (!*arg2)
			return;

		uint32_t fishVnum;
		str_to_number(fishVnum, arg2);

		ch->GetFishWiki()->LoadFishRanking(fishVnum);
	}
}

ACMD(do_dungeons) {
	const DWORD dungeonIdx = ch->GetMapIndex();
	const DWORD mapIdx = dungeonIdx / 10000;

	std::unordered_map<int, std::tuple<int, int, int, int, const char*>> dungeonData = {
		{ 28, {204000, 28, 15514, 3680, "orc_dungeon.join_orc_camp"} },
		{ 18, {204002, 18, 13504, 5265, "deviltower_dungeon.join_deviltower"} },
		{ 300, {204004, 300, 15273, 15225, "sanctum_dungeon.join_sanctum"} },
		{ 51, {204005, 51, 12700, 2172, "hwang_temple_dungeon.join_hwang_temple_dungeon"} },
		{ 21, {204006, 21, 7813, 6280, "emerald_dungeon.join_emerald"} },
		{ 16, {204007, 16, 9594, 6283, "gorgon_dungeon.join_gorgon"} },
		{ 12, {204008, 12, 7821, 5460, "silken_dungeon.join_silken"} },
		{ 14, {204009, 14, 8577, 6555, "hellforge_dungeon.join_hellforge"} },
		{ 27, {204010, 27, 6276, 9058, "crimson_dungeon.join_crimson"} }
	};
	
	auto it = dungeonData.find(mapIdx);
	if (it == dungeonData.end()) {
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2332]");
		return;
	}

	int requiredItem, dungeonId, x, y;
	const char* questFlag;
	std::tie(requiredItem, dungeonId, x, y, questFlag) = it->second;
	
	if (!ch->GetDungeon()) return;
	
	if (ch->GetDungeon()->GetFlag("can_rejoin") <= 0) return;
	
	ch->GetDungeon()->SetFastDestroy(true);

	// Step 1: validate item BEFORE creating dungeon or incrementing limit
	if (ch->GetParty()) {
		if (!ch->GetParty()->GetNearMemberCount()) {
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2334]");
			return;
		}
		if (!ch->GetParty()->GetLeader() || ch->GetParty()->GetLeader()->CountSpecifyItem(requiredItem) < ch->GetParty()->GetMemberCount()) {
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2333]");
			return;
		}
	} else {
		if (ch->CountSpecifyItem(requiredItem) < 1) {
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2335]");
			return;
		}
	}

	// Step 2: create dungeon
	LPDUNGEON pDungeon = CDungeonManager::instance().Create(dungeonId);
	if (!pDungeon)
		return;

	// Step 3: check & increment dungeon limit (item not yet taken)
#ifdef __DUNGEON_LIMIT_RATES__
	auto IncrementDungeonCount = [](LPCHARACTER ch_party, int mapIndex, uint16_t value) -> bool {
		return DungeonRates::AppendCount(ch_party, mapIndex, value);
	};

	bool bMatched = false;

	if (auto* party = ch->GetParty()) {
		bMatched = party->ForEachOnMapMemberBool(
			[&](LPCHARACTER member) {
				return IncrementDungeonCount(member, mapIdx, 1);
			},
			mapIdx,
			false
		);
	} else {
		bMatched = IncrementDungeonCount(ch, mapIdx, 1);
	}

	if (!bMatched) {
		CDungeonManager::instance().Destroy(pDungeon->GetId());
		sys_log(0, "Dungeon join rejected due to rate limits (item NOT consumed).");
		return;
	}
#endif

	// Step 4: point of no return - remove item and jump
	if (ch->GetParty()) {
		ch->GetParty()->GetLeader()->RemoveSpecifyItem(requiredItem, ch->GetParty()->GetMemberCount());
	} else {
		ch->RemoveSpecifyItem(requiredItem, 1);
	}

	pDungeon->JumpAll(dungeonIdx, x, y);
	ch->SetQuestFlag(questFlag, 1);
}

ACMD(do_toggle_antyexp)
{
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::ChangeEXP, std::chrono::milliseconds(20000)))
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2200;%f]", PULSEMANAGER_CLOCK_TO_SEC2(ch->GetPlayerID(), ePulse::ChangeEXP));
        return;
    }

	if (ch->GetLevel() < 30)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2310]");
		return;
	}

	if (!ch->GetDesc())
	{
		return;
	}

	TPacketGCAntyExp pack;
	pack.bHeader = HEADER_GC_ANTYEXP;

	if (ch->GetQuestFlag("antyexp.status") == 1)
	{
		ch->SetQuestFlag("antyexp.status", 0);
		ch->SetToggleAntyExp(false);
		pack.status = 0;
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2313]");
	}
	else
	{
		ch->SetQuestFlag("antyexp.status", 1);
		ch->SetToggleAntyExp(true);
		pack.status = 1;
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2314]");
	}

	ch->GetDesc()->Packet(&pack, sizeof(TPacketGCAntyExp));
}

#ifdef ENABLE_VS_SHOP_SEARCH
ACMD(do_search_in_shops)
{
	char arg1[256], arg2[256], arg3[256], arg4[256];
	four_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2), arg3, sizeof(arg3), arg4, sizeof(arg4));
	// if (!*arg1 || !*arg2 || !*arg3)
	// {
	// 	ch->ChatPacket(CHAT_TYPE_INFO, "Syntax: search_in_shops <search_type> <type> <subtype>");
	// 	return;
	// }
	
	DWORD search_type, vnum, type, subtype;
	str_to_number(search_type, arg1);
	str_to_number(vnum, arg2);
	str_to_number(type, arg3);
	str_to_number(subtype, arg4);

#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::ShopSearch, std::chrono::milliseconds(3000))) {
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Remaining time: %.2f"), PULSEMANAGER_CLOCK_TO_SEC2(ch->GetPlayerID(), ePulse::ShopSearch));
		return;
	}
#endif
	COfflineShopManager::Instance().ClearShopSearch(ch);

	std::string vnumString = "0";

    COfflineShopManager::Instance().SearchInShops(ch, search_type, vnum, vnumString, type, subtype);
}
#endif

#ifdef ENABLE_PROMO_CODE_SYSTEM
ACMD(do_promo_code_system)
{
	const char *line;
	char arg1[256], arg2[256];
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	
	if (!ch || arg1[0] == 0)
		return;
	
	const std::string& strArg1 = std::string(arg1);
	
	if (strArg1 == "check_code")
	{
		if (ch->GetQuestFlag("promo_code.cooltime") && get_global_time() < ch->GetQuestFlag("promo_code.cooltime"))
		{
			return;
		}
	
		if (strlen(arg2) > 10)
			return;
		
		if (ch->check_PromoCodeSystem(arg2))
		{
#ifdef ENABLE_PROMO_CODE_ONE_USE_PER_ACCOUNT
			if (ch->restriction_PromoCodeSystem(arg2))
				ch->reward_PromoCodeSystem(arg2);
			else
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2337]");
#else
			ch->reward_PromoCodeSystem(arg2);
#endif
		}
		else
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2338]");
		
		ch->SetQuestFlag("promo_code.cooltime", get_global_time() + 5);
	}
}

ACMD(do_promo_code_open)
{
	if (!ch)
		return;
	
	ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenCodeWindow");
}
#endif

#ifdef ENABLE_COLLECT_WINDOW
ACMD(do_choose_quest)
{
	const char *line;
	char arg1[256], arg2[256];
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!ch || !*arg1 && !*arg2)
		return;

#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::ChangeQuest, std::chrono::milliseconds(1000))) {
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2200;%f]", PULSEMANAGER_CLOCK_TO_SEC2(ch->GetPlayerID(), ePulse::ChangeQuest));
		return;
	}
#endif

	BYTE id = 0;
	BYTE id1 = 0;
	str_to_number(id, arg1);
	str_to_number(id1, arg2);

	ch->SetQuestFlag("collector.state", id);

	quest::CQuestManager::Instance().QuestButton(ch->GetPlayerID(), id1, 0);
}
ACMD(do_open_collect_window)
{
#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::CollectWindow, std::chrono::milliseconds(700))) {
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2200;%f]", PULSEMANAGER_CLOCK_TO_SEC2(ch->GetPlayerID(), ePulse::CollectWindow));
		return;
	}
#endif
	ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenCollectWindow");
#ifdef ENABLE_NOTIFICATION_SYSTEM
	ch->ChatPacket(CHAT_TYPE_COMMAND, "SetMissionNotifications %d", ch->GetQuestFlag("collector.notifications"));
#endif
	quest::CQuestManager::Instance().QuestButton(ch->GetPlayerID(), 13, 2);
}
#endif

#ifdef ENABLE_HIDE_COSTUME_SYSTEM
ACMD(do_hide_costume)
{
#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::HideCostume, std::chrono::milliseconds(15000))) {
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2200;%f]", PULSEMANAGER_CLOCK_TO_SEC2(ch->GetPlayerID(), ePulse::HideCostume));
		return;
	}
#endif
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
		return;

	bool hidden = true;
	BYTE bPartPos = 0;
	BYTE bHidden = 0;

	str_to_number(bPartPos, arg1);

	if (*arg2)
	{
		str_to_number(bHidden, arg2);

		if (bHidden == 0)
			hidden = false;
	}

	if (bPartPos == 1)
		ch->SetBodyCostumeHidden(hidden);
	else if (bPartPos == 2)
		ch->SetHairCostumeHidden(hidden);
	else if (bPartPos == 3)
		ch->SetAcceCostumeHidden(hidden);
	else if (bPartPos == 4)
		ch->SetWeaponCostumeHidden(hidden);
	else if (bPartPos == 5)
		ch->SetAuraCostumeHidden(hidden);
	else if (bPartPos == 6)
		ch->SetStoleCostumeHidden(hidden);
	else
		return;

	ch->UpdatePacket();
}
#endif

#ifdef ENABLE_NEW_STONE_DETACH
ACMD(do_detach_stone)
{
	if (!ch)
	{
		return;
	}

	if (ch->IsObserverMode() || ch->GetExchange() || ch->IsCubeOpen() || ch->IsOpenSafebox() || ch->GetMyShop() || ch->GetShopOwner())
	{
		return;
	}

	const char* line;
	char arg1[256], arg2[256], arg3[256];
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));

	if (!*arg1 || !*arg2 || !*arg3)
	{
		return;
	}

	BYTE socketIdx;
	WORD scrollSlotPos, targetSlotPos;

	str_to_number(socketIdx, arg1);
	str_to_number(scrollSlotPos, arg2);
	str_to_number(targetSlotPos, arg3);

	const DWORD SCROLL_VNUM = 25100;
	const DWORD ITEM_BROKEN_METIN_VNUM = 28960;

	LPITEM scroll = ch->GetItem(TItemPos(INVENTORY, scrollSlotPos));

	if (!scroll || scroll->GetVnum() != SCROLL_VNUM)
	{
		return;
	}

	LPITEM item = ch->GetItem(TItemPos(INVENTORY, targetSlotPos));

	if (!item || (item->GetType() != ITEM_WEAPON && (item->GetType() != ITEM_ARMOR && item->GetSubType() != ARMOR_BODY)) || item->IsEquipped())
	{
		return;
	}

	int32_t socket = item->GetSocket(socketIdx);

	if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
	{
		char hint[64];
		snprintf(hint, sizeof(hint), "USE_DETACHMENT, STONE: %d", socket);
		LogManager::instance().ItemLog(ch, item, hint, item->GetName());

		item->SetSocket(socketIdx, 1);
		ch->AutoGiveItem(socket);
		scroll->SetCount(scroll->GetCount() - 1);
	}
}
#endif

#ifdef TITLE_SYSTEM_BYLUZER
ACMD(do_detach_title)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	BYTE id = 0;
	str_to_number(id, arg1);

	ch->DetachTitle(id);
}
#endif

#ifdef ENABLE_SORT_INVENTORY_ITEMS
ACMD (do_sort_inventory)
{
#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::SortInventory, std::chrono::milliseconds(10000)))
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2200;%f]", PULSEMANAGER_CLOCK_TO_SEC2(ch->GetPlayerID(), ePulse::SortInventory));
        return;
    }
#endif

    if (ch->IsDead() || ch->GetExchange() || ch->GetShop() || ch->IsOpenSafebox() || ch->IsCubeOpen())
    {
        return;
    }
   
    for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
    {
        LPITEM item = ch->GetInventoryItem(i);
       
        if(!item)
            continue;
       
        if(item->isLocked())
            continue;
       
        if(item->GetCount() == g_bItemCountLimit)
            continue;
       
        if (item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
        {
            for (int j = i; j < INVENTORY_MAX_NUM; ++j)
            {
                LPITEM item2 = ch->GetInventoryItem(j);
               
                if(!item2)
                    continue;
               
                if(item2->isLocked())
                    continue;
   
                if (item2->GetVnum() == item->GetVnum())
                {
                    bool bStopSockets = false;
                   
				   	// if (item->GetType() != ITEM_GACHA && item2->GetType() != ITEM_GACHA) {
					// 	for (int k = 0; k < ITEM_SOCKET_MAX_NUM; ++k)
					// 	{
					// 		if (item2->GetSocket(k) != item->GetSocket(k))
					// 		{
					// 			bStopSockets = true;
					// 			break;
					// 		}
					// 	}
					// }
                   
                    if(bStopSockets)
                        continue;
#ifdef __EXTENDED_ITEM_COUNT__
                    uint16_t bAddCount = MIN(g_bItemCountLimit - item->GetCount(), item2->GetCount());
#else
					BYTE bAddCount = MIN(g_bItemCountLimit - item->GetCount(), item2->GetCount());
#endif
					// if (item->GetType() == ITEM_GACHA && item2->GetType() == ITEM_GACHA) {
					// 	int newSocketValue = MIN(g_bItemCountLimit - item->GetSocket(0), item2->GetSocket(0));
					// 	item->SetSocket(0, item->GetSocket(0) + newSocketValue);
					// 	item2->SetSocket(0, item2->GetSocket(0) - newSocketValue);
					// }

                    item->SetCount(item->GetCount() + bAddCount);
                    item2->SetCount(item2->GetCount() - bAddCount);
                   
                    continue;
                }
            }
        }
    }
   
   	ch->FlushDelayedSaveItem();
}

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
ACMD(do_sort_special_inventory)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	BYTE type = 0;
	str_to_number(type, arg1);

	ch->SortSpecialInventoryItems(type);
}
#endif
#endif

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
ACMD(do_bezposrednie_wpadanie_test)
{
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::Wpadanie, std::chrono::milliseconds(20000)))
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2200;%f]", PULSEMANAGER_CLOCK_TO_SEC2(ch->GetPlayerID(), ePulse::Wpadanie));
        return;
    }

	TPacketGCWpadanie pack;
	pack.bHeader = HEADER_GC_WPADANIE;
	if (ch->IsWpadanie())
	{
		ch->SetToggleWpadanie(false);
		pack.status = 0;
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2315]");
	}
	else
	{
		ch->SetToggleWpadanie(true);
		pack.status = 1;
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2316]");
	}
	ch->GetDesc()->Packet(&pack, sizeof(TPacketGCWpadanie));
}
#endif

#ifdef ENABLE_MOUNT_SYSTEM
ACMD(do_user_horse_ride)
{
	if (ch->IsObserverMode())
		return;

	if (ch->IsDead() || ch->IsStun())
		return;
	
	if (ch->IsIllegalMapForMount())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10018]");
		return;
	}

	if (ch->IsMining() || ch->IsFishing())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10014]");
		return;
	}
	
	//if (thecore_pulse() < ch->GetNextCanMountTime())
	//{
	//	ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10011]");
	//	return;
	//}

	if (ch->IsHorseRiding() == false)
	{
		{
			DWORD dwNow = get_dword_time();
			DWORD dwRemReceived = (dwNow - ch->GetLastPVPHitReceivedTime() < 5000) ? (5000 - (dwNow - ch->GetLastPVPHitReceivedTime())) : 0;
			DWORD dwRemDealt    = (dwNow - ch->GetLastPVPHitDealtTime()    < 5000) ? (5000 - (dwNow - ch->GetLastPVPHitDealtTime()))    : 0;
			DWORD dwRem = std::max(dwRemReceived, dwRemDealt);
			if (dwRem > 0)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10151;%d]", (dwRem + 999) / 1000);
				return;
			}
		}

		if (ch->GetMountVnum())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10019]");
			return;
		}

		if (ch->GetHorse() == NULL)
			ch->HorseSummon(true);

		ch->StartRiding();
	}
	else
		ch->StopRiding();

	ch->SendMountInfoPacket();
}

ACMD(do_user_horse_back)
{
	if (ch->GetHorse() != NULL)
	{
		ch->HorseSummon(false);
		ch->SendMountInfoPacket();
	}
	else if (ch->IsHorseRiding() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10008]");
	}
}

ACMD(do_horse_summon)
{
	if (ch->GetHorse() == NULL)
	{
		ch->HorseSummon(true, true);
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10009]");
		ch->SendMountInfoPacket();
	}
}

ACMD(do_horse_unsummon)
{
	if (ch->GetHorse() != NULL)
	{
		ch->HorseSummon(false, true);
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10010]");
		ch->SendMountInfoPacket();
	}
	else if (ch->IsHorseRiding() == true)
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10008]");
}
#endif

ACMD(do_user_horse_feed)
{
#ifdef __RENEWAL_MOUNT__
	if (ch->MountBlockMap())
		return;
#endif

	if (ch->GetMyShop())
		return;

	if (ch->GetHorse() == NULL)
	{
		if (ch->IsHorseRiding() == false)
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;501]");
		else
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;505]");
		return;
	}

	DWORD dwFood = ch->GetHorseGrade() + 50054 - 1;

	if (ch->CountSpecifyItem(dwFood) > 0)
	{
		ch->RemoveSpecifyItem(dwFood, 1);
		ch->FeedHorse();
	}
}

#ifdef ENABLE_ODLAMKI_SYSTEM
ACMD(do_odlamki)
{
	LPCHARACTER	npc;
	npc = ch->GetQuestNPC();

	if (npc)
	{
		if (NULL == ch || NULL == npc || !npc)
		{
			return;
		}
		ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenOdlamki");
	}
	else {
		return;
	}
}
#endif

#ifdef ENABLE_POLY_SHOP
ACMD(do_buy_marble)
{
	if (ch->IsObserverMode() || ch->GetExchange())
		return;

	char arg1[256], arg2[256], arg3[256];
	three_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2), arg3, sizeof(arg3));

	if (!*arg1 || !*arg2 || !*arg3)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Syntax: buy_marble_item <mobVnum> <itemCount> <marmurGrade>");
		return;
	}

	DWORD mobVnum, marmurGrade;
	int itemCount;
	int64_t price = 0;

	str_to_number(mobVnum, arg1);
	str_to_number(itemCount, arg2);
	str_to_number(marmurGrade, arg3);

	if (!mobVnum || mobVnum <= 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "wrong mobVnum.");
		return;
	}

	if (marmurGrade > 2 || marmurGrade < 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "wrong marmurGrade (must be 0, 1, or 2).");
		return;
	}

	if (itemCount < 1 || itemCount > 20)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "wrong count.");
		return;
	}

	switch (marmurGrade)
	{
		case 0:
			price = (mobVnum == 552) ? 30000000 * itemCount : 50000000 * itemCount;
			break;

		case 1:
			price = 1 * itemCount;
			break;

		case 2:
			price = 3 * itemCount;
			break;

		default:
			ch->ChatPacket(CHAT_TYPE_INFO, "wrong marmurGrade.");
			return;
	}

	switch (mobVnum)
	{
		case 552:
		case 636:
		case 2001:
			break;

		default:
			ch->ChatPacket(CHAT_TYPE_INFO, "marble not exist.");
			return;
	}

	if ((marmurGrade == 1 && ch->GetCheque() < price) || 
	    (marmurGrade == 2 && ch->GetCheque() < price) ||
	    (marmurGrade != 1 && marmurGrade != 2 && ch->GetGold() < price))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2311]");
		return;
	}

	int GRADE_VNUM[3] = {70104, 70105, 70106};
	int REQUIRED_ITEMS[3] = {71001, 71094, 50513};
	int REQUIRED_ITEMS_COUNT[2] = {3, 10};

	LPITEM item = ITEM_MANAGER::instance().CreateItem(GRADE_VNUM[0], itemCount, 0, true);
	LPITEM item_grade1 = ITEM_MANAGER::instance().CreateItem(GRADE_VNUM[1], itemCount, 0, true);
	LPITEM item_grade2 = ITEM_MANAGER::instance().CreateItem(GRADE_VNUM[2], itemCount, 0, true);

	if (item || item_grade1 || item_grade2)
	{
		int iEmptyPos = ch->GetEmptyInventory(item->GetSize());

		if (iEmptyPos != -1)
		{
			switch (marmurGrade)
			{
				case 0:
					ch->PointChange(POINT_GOLD, -price);
					item->SetSocket(0, mobVnum);
					item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
					break;

				case 1:
					if (ch->CountSpecifyItem(REQUIRED_ITEMS[0]) >= REQUIRED_ITEMS_COUNT[0] * itemCount &&
						ch->CountSpecifyItem(REQUIRED_ITEMS[1]) >= REQUIRED_ITEMS_COUNT[0] * itemCount &&
						ch->CountSpecifyItem(REQUIRED_ITEMS[2]) >= REQUIRED_ITEMS_COUNT[0] * itemCount)
					{
						ch->RemoveSpecifyItem(REQUIRED_ITEMS[0], REQUIRED_ITEMS_COUNT[0] * itemCount);
						ch->RemoveSpecifyItem(REQUIRED_ITEMS[1], REQUIRED_ITEMS_COUNT[0] * itemCount);
						ch->RemoveSpecifyItem(REQUIRED_ITEMS[2], REQUIRED_ITEMS_COUNT[0] * itemCount);

						ch->PointChange(POINT_CHEQUE, -price);
						item_grade1->SetSocket(0, mobVnum);
						item_grade1->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
					}
					else
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2291]");
					}
					break;

				case 2:
					if (ch->CountSpecifyItem(REQUIRED_ITEMS[0]) >= REQUIRED_ITEMS_COUNT[1] * itemCount &&
						ch->CountSpecifyItem(REQUIRED_ITEMS[1]) >= REQUIRED_ITEMS_COUNT[1] * itemCount &&
						ch->CountSpecifyItem(REQUIRED_ITEMS[2]) >= REQUIRED_ITEMS_COUNT[1] * itemCount)
					{
						ch->RemoveSpecifyItem(REQUIRED_ITEMS[0], REQUIRED_ITEMS_COUNT[1] * itemCount);
						ch->RemoveSpecifyItem(REQUIRED_ITEMS[1], REQUIRED_ITEMS_COUNT[1] * itemCount);
						ch->RemoveSpecifyItem(REQUIRED_ITEMS[2], REQUIRED_ITEMS_COUNT[1] * itemCount);

						ch->PointChange(POINT_CHEQUE, -price);
						item_grade2->SetSocket(0, mobVnum);
						item_grade2->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
					}
					else
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2291]");
					}
					break;

				default:
					ch->ChatPacket(CHAT_TYPE_INFO, "MARMUR_GRADE: ERROR");
					break;
			}
		}
		else
		{
			M2_DESTROY_ITEM(item);
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1130]");
		}
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Item creation failed.");
	}
}
#endif

#define MAX_REASON_LEN		128

EVENTINFO(TimedEventInfo)
{
	DynamicCharacterPtr ch;
	int		subcmd;
	int         	left_second;
	char		szReason[MAX_REASON_LEN];

	TimedEventInfo()
	: ch()
	, subcmd( 0 )
	, left_second( 0 )
	{
		::memset( szReason, 0, MAX_REASON_LEN );
	}
};

struct SendDisconnectFunc
{
	void operator () (LPDESC d)
	{
		if (d->GetCharacter())
		{
			if (d->GetCharacter()->GetGMLevel() == GM_PLAYER)
				d->GetCharacter()->ChatPacket(CHAT_TYPE_COMMAND, "quit Shutdown(SendDisconnectFunc)");
		}
	}
};

struct DisconnectFunc
{
	void operator () (LPDESC d)
	{
		if (d->GetType() == DESC_TYPE_CONNECTOR)
			return;

		if (d->IsPhase(PHASE_P2P))
			return;

		if (d->GetCharacter())
			d->GetCharacter()->Disconnect("Shutdown(DisconnectFunc)");

		d->SetPhase(PHASE_CLOSE);
	}
};

EVENTINFO(shutdown_event_data)
{
	int seconds;

	shutdown_event_data()
	: seconds( 0 )
	{
	}
};

EVENTFUNC(shutdown_event)
{
	shutdown_event_data* info = dynamic_cast<shutdown_event_data*>( event->info );

	if ( info == NULL )
	{
		sys_err( "shutdown_event> <Factor> Null pointer" );
		return 0;
	}

	int * pSec = & (info->seconds);

	if (*pSec < 0)
	{
		sys_log(0, "shutdown_event sec %d", *pSec);

		if (--*pSec == -10)
		{
			const DESC_MANAGER::DESC_SET & c_set_desc = DESC_MANAGER::instance().GetClientSet();
			std::for_each(c_set_desc.begin(), c_set_desc.end(), DisconnectFunc());
			return passes_per_sec;
		}
		else if (*pSec < -10)
			return 0;

		return passes_per_sec;
	}
	else if (*pSec == 0)
	{
		const DESC_MANAGER::DESC_SET & c_set_desc = DESC_MANAGER::instance().GetClientSet();
		std::for_each(c_set_desc.begin(), c_set_desc.end(), SendDisconnectFunc());
		g_bNoMoreClient = true;
		--*pSec;
		return passes_per_sec;
	}
	else
	{
		char buf[64];
		snprintf(buf, sizeof(buf), "[LS;508;%d]", *pSec);
		SendNotice(buf);

		--*pSec;
		return passes_per_sec;
	}
}

void Shutdown(int iSec)
{
	if (g_bNoMoreClient)
	{
		thecore_shutdown();
		return;
	}

	CWarMapManager::instance().OnShutdown();

	char buf[64];
	snprintf(buf, sizeof(buf), "[LS;509;%d]", iSec);

	SendNotice(buf);

	shutdown_event_data* info = AllocEventInfo<shutdown_event_data>();
	info->seconds = iSec;

	event_create(shutdown_event, info, 1);
}

ACMD(do_shutdown)
{
	if (NULL == ch)
	{
		sys_err("Accept shutdown command from %s.", ch->GetName());
	}
	TPacketGGShutdown p;
	p.bHeader = HEADER_GG_SHUTDOWN;
	P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGShutdown));

	Shutdown(10);
}

EVENTFUNC(timed_event)
{
	TimedEventInfo * info = dynamic_cast<TimedEventInfo *>( event->info );

	if ( info == NULL )
	{
		sys_err( "timed_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;
	if (ch == NULL) { // <Factor>
		return 0;
	}
	LPDESC d = ch->GetDesc();

	if (info->left_second <= 0)
	{
		ch->m_pkTimedEvent = NULL;

		switch (info->subcmd)
		{
			case SCMD_LOGOUT:
			case SCMD_QUIT:
			case SCMD_PHASE_SELECT:
				{
					TPacketNeedLoginLogInfo acc_info;
					acc_info.dwPlayerID = ch->GetDesc()->GetAccountTable().id;

					db_clientdesc->DBPacket( HEADER_GD_VALID_LOGOUT, 0, &acc_info, sizeof(acc_info) );

					LogManager::instance().DetailLoginLog( false, ch );
				}
				break;
		}

		switch (info->subcmd)
		{
			case SCMD_LOGOUT:
				if (d)
					d->SetPhase(PHASE_CLOSE);
				break;

			case SCMD_QUIT:
				ch->ChatPacket(CHAT_TYPE_COMMAND, "quit");
				if (d) // @fixme197
					d->DelayedDisconnect(1);
				break;

			case SCMD_PHASE_SELECT:
 			{
				if (!d)
				{
					sys_err("SECURITY: PHASE_SELECT without descriptor");
					return 0;
				}

				const TAccountTable & account = d->GetAccountTable();
				if (!account.id)
				{
					sys_err("SECURITY: PHASE_SELECT without valid account");
					d->SetPhase(PHASE_CLOSE);
					return 0;
				}
				
				if (DESC_MANAGER::instance().FindByLoginName(account.login) != d)
 				{
					sys_err("SECURITY: PHASE_SELECT session mismatch for %s", account.login);
					d->SetPhase(PHASE_CLOSE);
					return 0;
 				}
				
				ch->Disconnect("timed_event - SCMD_PHASE_SELECT");
				d->SetPhase(PHASE_SELECT);
 			}
				break;
		}

		return 0;
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;510;%d]", info->left_second);
		--info->left_second;
	}

	return PASSES_PER_SEC(1);
}

ACMD(do_cmd)
{
	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;511]");
		event_cancel(&ch->m_pkTimedEvent);
		return;
	}

	switch (subcmd)
	{
		case SCMD_LOGOUT:
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;512]");
			break;

		case SCMD_QUIT:
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;513]");
			break;

		case SCMD_PHASE_SELECT:
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;515]");
			break;
	}

	int nExitLimitTime = 10;

	if (ch->IsHack(false, true, nExitLimitTime) &&
		false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()) &&
	   	(!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
	{
		return;
	}

	switch (subcmd)
	{
		case SCMD_LOGOUT:
		case SCMD_QUIT:
		case SCMD_PHASE_SELECT:
			{
				TimedEventInfo* info = AllocEventInfo<TimedEventInfo>();

				{
					if (ch->IsPosition(POS_FIGHTING))
						info->left_second = 10;
					else
						info->left_second = 3;
				}

				info->ch		= ch;
				info->subcmd		= subcmd;
				strlcpy(info->szReason, argument, sizeof(info->szReason));

				ch->m_pkTimedEvent	= event_create(timed_event, info, 1);
			}
			break;
	}
}

ACMD(do_mount)
{
}

ACMD(do_fishing)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	ch->SetRotation(atof(arg1));
	ch->fishing();
}

ACMD(do_console)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ConsoleEnable");
}

ACMD(do_restart)
{
	if (false == ch->IsDead())
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");
		ch->StartRecoveryEvent();
		return;
	}

	if (NULL == ch->m_pkDeadEvent)
		return;

	int iTimeToDead = 0;
#ifdef ENABLE_PVP_RANKING
	if (ch->GetMapIndex() == CPvPRanking::instance().GetMapIndex() && quest::CQuestManager::instance().GetEventFlag("pvp_ranking") > 0)
		iTimeToDead = 0;
#endif

	if (subcmd != SCMD_RESTART_TOWN && (!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
	{
		if (!test_server)
		{
			if (ch->IsHack())
			{
				if (false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;516;%d]", iTimeToDead - (180 - g_nPortalLimitTime));
					return;
				}
			}
#define eFRS_HERESEC	177
			if (iTimeToDead > eFRS_HERESEC)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;516;%d]", iTimeToDead - eFRS_HERESEC);
				return;
			}
		}
	}

	//PREVENT_HACK

	if (subcmd == SCMD_RESTART_TOWN)
	{
		if (ch->IsHack())
		{
			if ((!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG) ||
			   	false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;516;%d]", iTimeToDead - (180 - g_nPortalLimitTime));
				return;
			}
		}

#define eFRS_TOWNSEC	177
		if (iTimeToDead > eFRS_TOWNSEC)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;519;%d]", iTimeToDead - eFRS_TOWNSEC);
			return;
		}
	}
	//END_PREVENT_HACK

	ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");

	ch->GetDesc()->SetPhase(PHASE_GAME);
	ch->SetPosition(POS_STANDING);
	ch->StartRecoveryEvent();

	//FORKED_LOAD

	if (1 == quest::CQuestManager::instance().GetEventFlag("threeway_war"))
	{
		if (subcmd == SCMD_RESTART_TOWN || subcmd == SCMD_RESTART_HERE)
		{
			if (true == CThreeWayWar::instance().IsThreeWayWarMapIndex(ch->GetMapIndex()) &&
					false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
			{
				ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));

				ch->ReviveInvisible(5);
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
				ch->CheckMount();
#endif
				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());

				return;
			}

			if (true == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
			{
				if (CThreeWayWar::instance().GetReviveTokenForPlayer(ch->GetPlayerID()) <= 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;520]");
					ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));
				}
				else
				{
					ch->Show(ch->GetMapIndex(), GetSungziStartX(ch->GetEmpire()), GetSungziStartY(ch->GetEmpire()));
				}

				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
				ch->ReviveInvisible(5);
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
				ch->CheckMount();
#endif

				return;
			}
		}
	}
	//END_FORKED_LOAD

	if (ch->GetDungeon())
		ch->GetDungeon()->UseRevive(ch);

	if (ch->GetWarMap() && !ch->IsObserverMode())
	{
		CWarMap * pMap = ch->GetWarMap();
		DWORD dwGuildOpponent = pMap ? pMap->GetGuildOpponent(ch) : 0;

		if (dwGuildOpponent)
		{
			switch (subcmd)
			{
				case SCMD_RESTART_TOWN:
					sys_log(0, "do_restart: restart town");
					PIXEL_POSITION pos;

					if (CWarMapManager::instance().GetStartPosition(ch->GetMapIndex(), ch->GetGuild()->GetID() < dwGuildOpponent ? 0 : 1, pos))
						ch->Show(ch->GetMapIndex(), pos.x, pos.y);
					else
						ch->ExitToSavedLocation();

					ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
					ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
					ch->ReviveInvisible(5);
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
					ch->CheckMount();
#endif
					break;

				case SCMD_RESTART_HERE:
					sys_log(0, "do_restart: restart here");
					ch->RestartAtSamePos();
					//ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY());
					ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
					ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
					ch->ReviveInvisible(5);
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
					ch->CheckMount();
#endif
					break;
			}

			return;
		}
	}

#ifdef ENABLE_PVP_RANKING
	if (ch->GetMapIndex() == CPvPRanking::instance().GetMapIndex() && quest::CQuestManager::instance().GetEventFlag("pvp_ranking") > 0)
	{
		switch (subcmd)
		{
			case SCMD_RESTART_TOWN:
				sys_log(0, "do_restart: restart town");
				PIXEL_POSITION pos;

				if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(ch->GetMapIndex(), ch->GetEmpire(), pos))
					ch->WarpSet(pos.x, pos.y);
				else
					ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));

				ch->PointChange(POINT_HP, 50 - ch->GetHP());
				ch->DeathPenalty(1);
				break;
			case SCMD_RESTART_HERE:
				ch->RestartAtSamePos();
				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
				ch->ReviveInvisible(5);
				break;
		}
		return;
	}
#endif
	
	switch (subcmd)
	{
		case SCMD_RESTART_TOWN:
			sys_log(0, "do_restart: restart town");
			PIXEL_POSITION pos;

			if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(ch->GetMapIndex(), ch->GetEmpire(), pos))
				if (ch->GetDungeon())
				{
					ch->WarpSet(1075900, 267800);
				} else {
					ch->WarpSet(pos.x, pos.y);
				}
			else
				ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));

			ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
			ch->DeathPenalty(1);
			break;

		case SCMD_RESTART_HERE:
			sys_log(0, "do_restart: restart here");
			ch->RestartAtSamePos();
			//ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY());
			ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
			ch->DeathPenalty(0);
			ch->ReviveInvisible(5);
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
			ch->CheckMount();
#endif
			break;
	}
}

#ifdef ENABLE_SKILL_SELECT_FEATURE
ACMD(do_selectskill_open)
{
	if (!ch)
		return;
	
	if (ch->GetSkillGroup())
		return;
	
	ch->ChatPacket(CHAT_TYPE_COMMAND, "selectskill_open");
}

ACMD(do_selectskill_select)
{
	if (!ch)
		return;
	
	if (ch->GetSkillGroup())
		return;
	
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;
	
	BYTE skill_group = 0;
	str_to_number(skill_group, arg1);
	if (skill_group)
		ch->SetSkillGroup(skill_group);
		ch->ClearSkill();
		ch->ChatPacket(CHAT_TYPE_INFO, "skill_path_selected");
		ch->UpdatePacket();
}
#endif

#define MAX_STAT g_iStatusPointSetMaxValue

ACMD(do_stat_val)
{
	char	arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	int val = 0;
	str_to_number(val, arg2);
	
	if (!*arg1 || val <= 0)
		return;

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1041]");
		return;
	}

	if (ch->GetPoint(POINT_STAT) <= 0)
		return;

	BYTE idx = 0;
	
	if (!strcmp(arg1, "st"))
		idx = POINT_ST;
	else if (!strcmp(arg1, "dx"))
		idx = POINT_DX;
	else if (!strcmp(arg1, "ht"))
		idx = POINT_HT;
	else if (!strcmp(arg1, "iq"))
		idx = POINT_IQ;
	else
		return;

	if (ch->GetRealPoint(idx) >= MAX_STAT)
		return;
	
	if (val > ch->GetPoint(POINT_STAT))
	{
		val = ch->GetPoint(POINT_STAT);
	}
	
	if (ch->GetRealPoint(idx) + val > MAX_STAT)
	{
		val = MAX_STAT - ch->GetRealPoint(idx);
	}

	ch->SetRealPoint(idx, ch->GetRealPoint(idx) + val);
	ch->SetPoint(idx, ch->GetPoint(idx) + val);
	ch->ComputePoints();
	ch->PointChange(idx, 0);

	if (idx == POINT_IQ)
	{
		ch->PointChange(POINT_MAX_HP, 0);
	}
	else if (idx == POINT_HT)
	{
		ch->PointChange(POINT_MAX_SP, 0);
	}

	ch->PointChange(POINT_STAT, -val);
	ch->ComputePoints();
}


ACMD(do_stat_reset)
{
	ch->PointChange(POINT_STAT_RESET_COUNT, 12 - ch->GetPoint(POINT_STAT_RESET_COUNT));
}

ACMD(do_stat_minus)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;521]");
		return;
	}

	if (ch->GetPoint(POINT_STAT_RESET_COUNT) <= 0)
		return;

	if (!strcmp(arg1, "st"))
	{
		if (ch->GetRealPoint(POINT_ST) <= JobInitialPoints[ch->GetJob()].st)
			return;

		ch->SetRealPoint(POINT_ST, ch->GetRealPoint(POINT_ST) - 1);
		ch->SetPoint(POINT_ST, ch->GetPoint(POINT_ST) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_ST, 0);
	}
	else if (!strcmp(arg1, "dx"))
	{
		if (ch->GetRealPoint(POINT_DX) <= JobInitialPoints[ch->GetJob()].dx)
			return;

		ch->SetRealPoint(POINT_DX, ch->GetRealPoint(POINT_DX) - 1);
		ch->SetPoint(POINT_DX, ch->GetPoint(POINT_DX) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_DX, 0);
	}
	else if (!strcmp(arg1, "ht"))
	{
		if (ch->GetRealPoint(POINT_HT) <= JobInitialPoints[ch->GetJob()].ht)
			return;

		ch->SetRealPoint(POINT_HT, ch->GetRealPoint(POINT_HT) - 1);
		ch->SetPoint(POINT_HT, ch->GetPoint(POINT_HT) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_HT, 0);
		ch->PointChange(POINT_MAX_HP, 0);
	}
	else if (!strcmp(arg1, "iq"))
	{
		if (ch->GetRealPoint(POINT_IQ) <= JobInitialPoints[ch->GetJob()].iq)
			return;

		ch->SetRealPoint(POINT_IQ, ch->GetRealPoint(POINT_IQ) - 1);
		ch->SetPoint(POINT_IQ, ch->GetPoint(POINT_IQ) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_IQ, 0);
		ch->PointChange(POINT_MAX_SP, 0);
	}
	else
		return;

	ch->PointChange(POINT_STAT, +1);
	ch->PointChange(POINT_STAT_RESET_COUNT, -1);
	ch->ComputePoints();
}

ACMD(do_stat)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;521]");
		return;
	}

	if (ch->GetPoint(POINT_STAT) <= 0)
		return;

	BYTE idx = 0;

	if (!strcmp(arg1, "st"))
		idx = POINT_ST;
	else if (!strcmp(arg1, "dx"))
		idx = POINT_DX;
	else if (!strcmp(arg1, "ht"))
		idx = POINT_HT;
	else if (!strcmp(arg1, "iq"))
		idx = POINT_IQ;
	else
		return;

	// ch->ChatPacket(CHAT_TYPE_INFO, "%s GRP(%d) idx(%u), MAX_STAT(%d), expr(%d)", __FUNCTION__, ch->GetRealPoint(idx), idx, MAX_STAT, ch->GetRealPoint(idx) >= MAX_STAT);
	if (ch->GetRealPoint(idx) >= MAX_STAT)
		return;

	ch->SetRealPoint(idx, ch->GetRealPoint(idx) + 1);
	ch->SetPoint(idx, ch->GetPoint(idx) + 1);
	ch->ComputePoints();
	ch->PointChange(idx, 0);

	if (idx == POINT_IQ)
	{
		ch->PointChange(POINT_MAX_HP, 0);
	}
	else if (idx == POINT_HT)
	{
		ch->PointChange(POINT_MAX_SP, 0);
	}

	ch->PointChange(POINT_STAT, -1);
	ch->ComputePoints();
}

ACMD(do_pvp)
{
	if (ch->GetArena() != NULL || CArenaManager::instance().IsArenaMap(ch->GetMapIndex()) == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;403]");
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(vid);

	if (!pkVictim)
		return;

	if (pkVictim->IsNPC())
		return;

	if (pkVictim->GetArena() != NULL)
	{
		pkVictim->ChatPacket(CHAT_TYPE_INFO, "[LS;522]");
		return;
	}

#ifdef ENABLE_MESSENGER_BLOCK
	if (MessengerManager::instance().IsBlocked(ch->GetName(), pkVictim->GetName()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You need to unblock %s do to that.", pkVictim->GetName());
		return;
	}
	if (MessengerManager::instance().IsBlocked(pkVictim->GetName(), ch->GetName()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "%s has blocked you.", pkVictim->GetName());
		return;
	}
#endif
	CPVPManager::instance().Insert(ch, pkVictim);
}

ACMD(do_guildskillup)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (!ch->GetGuild())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;523]");
		return;
	}

	CGuild* g = ch->GetGuild();
	TGuildMember* gm = g->GetMember(ch->GetPlayerID());
	if (gm->grade == GUILD_LEADER_GRADE)
	{
		DWORD vnum = 0;
		str_to_number(vnum, arg1);
		g->SkillLevelUp(vnum);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;524]");
	}
}

ACMD(do_skillup)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vnum = 0;
	str_to_number(vnum, arg1);

	if (true == ch->CanUseSkill(vnum))
	{
		ch->SkillLevelUp(vnum);
	}
	else
	{
		switch(vnum)
		{
			case SKILL_HORSE_WILDATTACK:
			case SKILL_HORSE_CHARGE:
			case SKILL_HORSE_ESCAPE:
			case SKILL_HORSE_WILDATTACK_RANGE:

			case SKILL_7_A_ANTI_TANHWAN:
			case SKILL_7_B_ANTI_AMSEOP:
			case SKILL_7_C_ANTI_SWAERYUNG:
			case SKILL_7_D_ANTI_YONGBI:

			case SKILL_8_A_ANTI_GIGONGCHAM:
			case SKILL_8_B_ANTI_YEONSA:
			case SKILL_8_C_ANTI_MAHWAN:
			case SKILL_8_D_ANTI_BYEURAK:

			case SKILL_ADD_HP:
			case SKILL_RESIST_PENETRATE:
				ch->SkillLevelUp(vnum);
				break;
		}
	}
}

//
//
ACMD(do_safebox_close)
{
	ch->CloseSafebox();
}

//
//
ACMD(do_safebox_password)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	ch->ReqSafeboxLoad(arg1);
}

ACMD(do_safebox_change_password)
{
	char arg1[256];
	char arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || strlen(arg1)>6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;526]");
		return;
	}

	if (!*arg2 || strlen(arg2)>6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;526]");
		return;
	}

	TSafeboxChangePasswordPacket p;

	p.dwID = ch->GetDesc()->GetAccountTable().id;
	strlcpy(p.szOldPassword, arg1, sizeof(p.szOldPassword));
	strlcpy(p.szNewPassword, arg2, sizeof(p.szNewPassword));

	db_clientdesc->DBPacket(HEADER_GD_SAFEBOX_CHANGE_PASSWORD, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}

ACMD(do_mall_password)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1 || strlen(arg1) > 6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;526]");
		return;
	}

	int iPulse = thecore_pulse();

	if (ch->GetMall())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;527]");
		return;
	}

	if (iPulse - ch->GetMallLoadTime() < passes_per_sec * 10)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;528]");
		return;
	}

	ch->SetMallLoadTime(iPulse);

	TSafeboxLoadPacket p;
	p.dwID = ch->GetDesc()->GetAccountTable().id;
	strlcpy(p.szLogin, ch->GetDesc()->GetAccountTable().login, sizeof(p.szLogin));
	strlcpy(p.szPassword, arg1, sizeof(p.szPassword));

	db_clientdesc->DBPacket(HEADER_GD_MALL_LOAD, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}

ACMD(do_mall_close)
{
	if (ch->GetMall())
	{
		ch->SetMallLoadTime(thecore_pulse());
		ch->CloseMall();
		ch->Save();
	}
}

ACMD(do_ungroup)
{
	if (!ch->GetParty())
		return;

	if (!CPartyManager::instance().IsEnablePCParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;530]");
		return;
	}

	if (ch->GetDungeon())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;531]");
		return;
	}

	LPPARTY pParty = ch->GetParty();

	if (pParty->GetMemberCount() == 2)
	{
		// party disband
		CPartyManager::instance().DeleteParty(pParty);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;532]");
		//pParty->SendPartyRemoveOneToAll(ch);
		pParty->Quit(ch->GetPlayerID());
		//pParty->SendPartyRemoveAllToOne(ch);
	}
}

ACMD(do_close_shop)
{
	if (ch->GetMyShop())
	{
		ch->CloseMyShop();
		return;
	}
}

ACMD(do_set_walk_mode)
{
	ch->SetNowWalking(true);
	ch->SetWalking(true);
}

ACMD(do_set_run_mode)
{
	ch->SetNowWalking(false);
	ch->SetWalking(false);
}

#ifdef ENABLE_AFFECT_POLYMORPH_REMOVE
ACMD(do_remove_polymorph)
{
	if (!ch)
		return;

	if (!ch->IsPolymorphed())
		return;

	ch->SetPolymorph(0);
	ch->RemoveAffect(AFFECT_POLYMORPH);
}
#endif

ACMD(do_remove_skill_affect)
{

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (!ch)
		return;

	int affect = 0;
	str_to_number(affect, arg1);
	
	CAffect* pAffect = ch->FindAffect(affect);
	
	if (affect == 3 || affect == 4 || affect == 19 || affect == 34
	|| affect == 49 || affect == 63 || affect == 64 || affect == 65
	|| affect == 79 || affect == 94 || affect == 95 || affect == 96
	|| affect == 110 || affect == 111 || affect == 510)

		if (pAffect)
			ch->RemoveAffect(affect);
	
}

// #ifdef ENABLE_SKILL_AFFECT_REMOVE
// ACMD(do_remove_skill_affect)
// {
	// char arg1[256];
	// one_argument(argument, arg1, sizeof(arg1));

	// if (!*arg1)
		// return;

	// DWORD affectvnum = 0;
	// str_to_number(affectvnum, arg1);

	// if (!ch->IsAffectFlag(affectvnum))
		// return;

	// if (affectvnum != AFF_JEONGWIHON && affectvnum != AFF_GEOMGYEONG 
		// && affectvnum != AFF_CHEONGEUN && affectvnum != AFF_GYEONGGONG 
		// && affectvnum != AFF_JEUNGRYEOK && affectvnum != AFF_GWIGUM
		// && affectvnum != AFF_TERROR && affectvnum != AFF_JUMAGAP
		// && affectvnum != AFF_MUYEONG && affectvnum != AFF_MANASHIELD
		// && affectvnum != AFF_HOSIN && affectvnum != AFF_BOHO
		// && affectvnum != AFF_GICHEON && affectvnum != AFF_KWAESOK
		// && affectvnum != AFF_JEUNGRYEOK && affectvnum != AFFECT_POLYMORPH
		// && affectvnum != AFFECT_MALL_EX && affectvnum != AFFECT_WATER
		// && affectvnum != AFFECT_BLEND_EX
// #ifdef ENABLE_WOLFMAN_CHARACTER
		// && affectvnum != AFF_BLUE_POSSESSION
// #endif
	// )
		// return;

	// CAffect* pAffect = ch->FindAffectByFlag(affectvnum);
	// if (!pAffect)
		// return;

	// ch->RemoveAffect(pAffect);
// }
// #endif

ACMD(do_war)
{
	CGuild * g = ch->GetGuild();

	if (!g)
		return;

	if (g->UnderAnyWar())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;533]");
		return;
	}

	char arg1[256], arg2[256];
	DWORD type = GUILD_WAR_TYPE_FIELD; //fixme102 base int modded uint
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
		return;

	if (*arg2)
	{
		str_to_number(type, arg2);

		if (type >= GUILD_WAR_TYPE_MAX_NUM)
			type = GUILD_WAR_TYPE_FIELD;
	}

	DWORD gm_pid = g->GetMasterPID();

	if (gm_pid != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;534]");
		return;
	}

	CGuild * opp_g = CGuildManager::instance().FindGuildByName(arg1);

	if (!opp_g)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;535]");
		return;
	}

	switch (g->GetGuildWarState(opp_g->GetID()))
	{
		case GUILD_WAR_NONE:
			{
				if (opp_g->UnderAnyWar())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;541]");
					return;
				}

				int iWarPrice = KOR_aGuildWarInfo[type].iWarPrice;

				if (g->GetGuildMoney() < iWarPrice)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;538]");
					return;
				}

				if (opp_g->GetGuildMoney() < iWarPrice)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;539]");
					return;
				}
			}
			break;

		case GUILD_WAR_SEND_DECLARE:
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;537]");
				return;
			}
			break;

		case GUILD_WAR_RECV_DECLARE:
			{
				if (opp_g->UnderAnyWar())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;541]");
					g->RequestRefuseWar(opp_g->GetID());
					return;
				}
			}
			break;

		case GUILD_WAR_RESERVE:
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;540]");
				return;
			}
			break;

		case GUILD_WAR_END:
			return;

		default:
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1050]");
			g->RequestRefuseWar(opp_g->GetID());
			return;
	}

	if (!g->CanStartWar(type))
	{
		if (g->GetLadderPoint() == 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1308]");
			sys_log(0, "GuildWar.StartError.NEED_LADDER_POINT");
		}
		else if (g->GetMemberCount() < GUILD_WAR_MIN_MEMBER_COUNT)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;543;%d]", GUILD_WAR_MIN_MEMBER_COUNT);
			sys_log(0, "GuildWar.StartError.NEED_MINIMUM_MEMBER[%d]", GUILD_WAR_MIN_MEMBER_COUNT);
		}
		else
		{
			sys_log(0, "GuildWar.StartError.UNKNOWN_ERROR");
		}
		return;
	}

	if (!opp_g->CanStartWar(GUILD_WAR_TYPE_FIELD))
	{
		if (opp_g->GetLadderPoint() == 0)
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;544]");
		else if (opp_g->GetMemberCount() < GUILD_WAR_MIN_MEMBER_COUNT)
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;545]");
		return;
	}

	do
	{
		if (g->GetMasterCharacter() != NULL)
			break;

		CCI *pCCI = P2P_MANAGER::instance().FindByPID(g->GetMasterPID());

		if (pCCI != NULL)
			break;

		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1048]");
		g->RequestRefuseWar(opp_g->GetID());
		return;

	} while (false);

	do
	{
		if (opp_g->GetMasterCharacter() != NULL)
			break;

		CCI *pCCI = P2P_MANAGER::instance().FindByPID(opp_g->GetMasterPID());

		if (pCCI != NULL)
			break;

		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1048]");
		g->RequestRefuseWar(opp_g->GetID());
		return;

	} while (false);

	g->RequestDeclareWar(opp_g->GetID(), type);
}

ACMD(do_nowar)
{
	CGuild* g = ch->GetGuild();
	if (!g)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD gm_pid = g->GetMasterPID();

	if (gm_pid != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;534]");
		return;
	}

	CGuild* opp_g = CGuildManager::instance().FindGuildByName(arg1);

	if (!opp_g)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;535]");
		return;
	}

	g->RequestRefuseWar(opp_g->GetID());
}

ACMD(do_detaillog)
{
	ch->DetailLog();
}

ACMD(do_monsterlog)
{
	ch->ToggleMonsterLog();
}

ACMD(do_pkmode)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	BYTE mode = 0;
	str_to_number(mode, arg1);

	if (mode == PK_MODE_PROTECT)
		return;

	if (ch->GetLevel() < PK_PROTECT_LEVEL && mode != 0)
		return;
	
#ifdef ENABLE_PVP_RANKING
	if (ch->GetMapIndex() == CPvPRanking::instance().GetMapIndex() && quest::CQuestManager::instance().GetEventFlag("pvp_ranking") > 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Savas alaninde bu islemi yapamazsin."));
		return;
	}
#endif

	ch->SetPKMode(mode);
}

ACMD(do_messenger_auth)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;403]");
		return;
	}

	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
		return;

	char answer = LOWER(*arg1);
	// @fixme130 AuthToAdd void -> bool
	bool bIsDenied = answer != 'y';
	bool bIsAdded = MessengerManager::instance().AuthToAdd(ch->GetName(), arg2, bIsDenied); // DENY
	if (bIsAdded && bIsDenied)
	{
		LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg2);

		if (tch)
			tch->ChatPacket(CHAT_TYPE_INFO, "[LS;548;%s]", ch->GetName());
	}
}

ACMD(do_setblockmode)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		BYTE flag = 0;
		str_to_number(flag, arg1);
		ch->SetBlockMode(flag);
	}
}

ACMD(do_unmount)
{
	if (true == ch->UnEquipSpecialRideUniqueItem())
	{
		ch->RemoveAffect(AFFECT_MOUNT);
		ch->RemoveAffect(AFFECT_MOUNT_BONUS);

		if (ch->IsHorseRiding())
		{
			ch->StopRiding();
		}
	}

}

ACMD(do_observer_exit)
{
	if (ch->IsObserverMode())
	{
		if (ch->GetWarMap())
			ch->SetWarMap(NULL);

		if (ch->GetArena() != NULL || ch->GetArenaObserverMode() == true)
		{
			ch->SetArenaObserverMode(false);

			if (ch->GetArena() != NULL)
				ch->GetArena()->RemoveObserver(ch->GetPlayerID());

			ch->SetArena(NULL);
			ch->WarpSet(ARENA_RETURN_POINT_X(ch->GetEmpire()), ARENA_RETURN_POINT_Y(ch->GetEmpire()));
		}
		else
		{
			ch->ExitToSavedLocation();
		}
		ch->SetObserverMode(false);
	}
}

ACMD(do_view_equip)
{
	if (ch->GetGMLevel() <= GM_PLAYER)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		DWORD vid = 0;
		str_to_number(vid, arg1);
		LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

		if (!tch)
			return;

		if (!tch->IsPC())
			return;

		tch->SendEquipment(ch);
	}
}

ACMD(do_party_request)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;403]");
		return;
	}

	if (ch->GetParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;549]");
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		if (!ch->RequestToParty(tch))
			ch->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
}

ACMD(do_party_request_accept)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		ch->AcceptToParty(tch);
}

ACMD(do_party_request_deny)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		ch->DenyToParty(tch);
}

ACMD(do_monarch_warpto)
{
	if (!CMonarch::instance().IsMonarch(ch->GetPlayerID(), ch->GetEmpire()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1007]");
		return;
	}

	if (!ch->IsMCOK(CHARACTER::MI_WARP))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;551;%d]", ch->GetMCLTime(CHARACTER::MI_WARP));
		return;
	}

	const int WarpPrice = 10000;

	if (!CMonarch::instance().IsMoneyOk(WarpPrice, ch->GetEmpire()))
	{
		int NationMoney = CMonarch::instance().GetMoney(ch->GetEmpire());
		return;
	}

	int x = 0, y = 0;
	char arg1[256];

	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;553]");
		return;
	}

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);

	if (!tch)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(arg1);

		if (pkCCI)
		{
			if (pkCCI->bEmpire != ch->GetEmpire())
			{
				ch->ChatPacket (CHAT_TYPE_INFO, "[LS;554]");
				return;
			}

			if (pkCCI->bChannel != g_bChannel)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;555;%d;%d]", pkCCI->bChannel, g_bChannel);
				return;
			}
			if (!IsMonarchWarpZone(pkCCI->lMapIndex))
			{
				return;
			}

			PIXEL_POSITION pos;

			if (!SECTREE_MANAGER::instance().GetCenterPositionOfMap(pkCCI->lMapIndex, pos))
				ch->ChatPacket(CHAT_TYPE_INFO, "Cannot find map (index %d)", pkCCI->lMapIndex);
			else
			{
				//ch->ChatPacket(CHAT_TYPE_INFO, "You warp to (%d, %d)", pos.x, pos.y);
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;556;%s]", arg1);
				ch->WarpSet(pos.x, pos.y);

				CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);

				ch->SetMC(CHARACTER::MI_WARP);
			}
		}
		else if (NULL == CHARACTER_MANAGER::instance().FindPC(arg1))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "There is no one by that name");
		}

		return;
	}
	else
	{
		if (tch->GetEmpire() != ch->GetEmpire())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;554]");
			return;
		}
		if (!IsMonarchWarpZone(tch->GetMapIndex()))
		{
			return;
		}
		x = tch->GetX();
		y = tch->GetY();
	}

	ch->ChatPacket(CHAT_TYPE_INFO, "[LS;556;%s]", arg1);
	ch->WarpSet(x, y);
	ch->Stop();

	CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);

	ch->SetMC(CHARACTER::MI_WARP);
}

ACMD(do_monarch_transfer)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1006]");
		return;
	}

	if (!CMonarch::instance().IsMonarch(ch->GetPlayerID(), ch->GetEmpire()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1007]");
		return;
	}

	if (!ch->IsMCOK(CHARACTER::MI_TRANSFER))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;551;%d]", ch->GetMCLTime(CHARACTER::MI_TRANSFER));
		return;
	}

	const int WarpPrice = 10000;

	if (!CMonarch::instance().IsMoneyOk(WarpPrice, ch->GetEmpire()))
	{
		int NationMoney = CMonarch::instance().GetMoney(ch->GetEmpire());
		return;
	}

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);

	if (!tch)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(arg1);

		if (pkCCI)
		{
			if (pkCCI->bEmpire != ch->GetEmpire())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1008]");
				return;
			}
			if (pkCCI->bChannel != g_bChannel)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1009;%s;%d;%d]", arg1, pkCCI->bChannel, g_bChannel);
				return;
			}
			if (!IsMonarchWarpZone(pkCCI->lMapIndex))
			{
				return;
			}
			if (!IsMonarchWarpZone(ch->GetMapIndex()))
			{
				return;
			}

			TPacketGGTransfer pgg;

			pgg.bHeader = HEADER_GG_TRANSFER;
			strlcpy(pgg.szName, arg1, sizeof(pgg.szName));
			pgg.lX = ch->GetX();
			pgg.lY = ch->GetY();

			P2P_MANAGER::instance().Send(&pgg, sizeof(TPacketGGTransfer));
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1010;%s]", arg1);

			CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);

			ch->SetMC(CHARACTER::MI_TRANSFER);
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1011]");
		}

		return;
	}

	if (ch == tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1012]");
		return;
	}

	if (tch->GetEmpire() != ch->GetEmpire())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1008]");
		return;
	}
	if (!IsMonarchWarpZone(tch->GetMapIndex()))
	{
		return;
	}
	if (!IsMonarchWarpZone(ch->GetMapIndex()))
	{
		return;
	}

	//tch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
	tch->WarpSet(ch->GetX(), ch->GetY(), ch->GetMapIndex());

	CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);

	ch->SetMC(CHARACTER::MI_TRANSFER);
}

ACMD(do_monarch_info)
{
	if (CMonarch::instance().IsMonarch(ch->GetPlayerID(), ch->GetEmpire()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;560]");
		TMonarchInfo * p = CMonarch::instance().GetMonarch();
		for (int n = 1; n < 4; ++n)
		{
			if (n == ch->GetEmpire())
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;561;%s;%s;%lld]", EMPIRE_NAME(n), p->name[n], p->money[n]);
			else
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;562;%s;%s]", EMPIRE_NAME(n), p->name[n]);

		}
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;563]");
		TMonarchInfo * p = CMonarch::instance().GetMonarch();
		for (int n = 1; n < 4; ++n)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;562;%s;%s]", EMPIRE_NAME(n), p->name[n]);

		}
	}
}

ACMD(do_elect)
{
	db_clientdesc->DBPacketHeader(HEADER_GD_COME_TO_VOTE, ch->GetDesc()->GetHandle(), 0);
}

// LUA_ADD_GOTO_INFO
struct GotoInfo
{
	std::string 	st_name;

	BYTE 	empire;
	int 	mapIndex;
	DWORD 	x, y;

	GotoInfo()
	{
		st_name 	= "";
		empire 		= 0;
		mapIndex 	= 0;

		x = 0;
		y = 0;
	}

	GotoInfo(const GotoInfo& c_src)
	{
		__copy__(c_src);
	}

	void operator = (const GotoInfo& c_src)
	{
		__copy__(c_src);
	}

	void __copy__(const GotoInfo& c_src)
	{
		st_name 	= c_src.st_name;
		empire 		= c_src.empire;
		mapIndex 	= c_src.mapIndex;

		x = c_src.x;
		y = c_src.y;
	}
};

ACMD(do_monarch_tax)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: monarch_tax <1-50>");
		return;
	}

	if (!ch->IsMonarch())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;564]");
		return;
	}

	int tax = 0;
	str_to_number(tax,  arg1);

	if (tax < 1 || tax > 50)
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;565]");

	quest::CQuestManager::instance().SetEventFlag("trade_tax", tax);

	ch->ChatPacket(CHAT_TYPE_INFO, "[LS;375;%d]");

	char szMsg[1024];

	snprintf(szMsg, sizeof(szMsg), "������ ������ ������ %d %% �� ����Ǿ����ϴ�", tax);
	BroadcastNotice(szMsg);

	snprintf(szMsg, sizeof(szMsg), "�����δ� �ŷ� �ݾ��� %d %% �� ������ ���Ե˴ϴ�.", tax);
	BroadcastNotice(szMsg);

	ch->SetMC(CHARACTER::MI_TAX);
}

static const DWORD cs_dwMonarchMobVnums[] =
{
	191,
	192,
	193,
	194,
	391,
	392,
	393,
	394,
	491,
	492,
	493,
	494,
	591,
	691,
	791,
	1304,
	1901,
	2091,
	2191,
	2206,
	0,
};

ACMD(do_monarch_mob)
{
	char arg1[256];
	LPCHARACTER	tch;

	one_argument(argument, arg1, sizeof(arg1));

	if (!ch->IsMonarch())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;564]");
		return;
	}

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: mmob <mob name>");
		return;
	}

#ifdef ENABLE_MONARCH_MOB_CMD_MAP_CHECK // @warme006
	BYTE pcEmpire = ch->GetEmpire();
	BYTE mapEmpire = SECTREE_MANAGER::instance().GetEmpireFromMapIndex(ch->GetMapIndex());
	if (mapEmpire != pcEmpire && mapEmpire != 0)
	{
		return;
	}
#endif

	const int SummonPrice = 5000000;

	if (!ch->IsMCOK(CHARACTER::MI_SUMMON))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;551;%d]", ch->GetMCLTime(CHARACTER::MI_SUMMON));
		return;
	}

	if (!CMonarch::instance().IsMoneyOk(SummonPrice, ch->GetEmpire()))
	{
		int NationMoney = CMonarch::instance().GetMoney(ch->GetEmpire());
		return;
	}

	const CMob * pkMob;
	DWORD vnum = 0;

	if (isdigit(*arg1))
	{
		str_to_number(vnum, arg1);

		if ((pkMob = CMobManager::instance().Get(vnum)) == NULL)
			vnum = 0;
	}
	else
	{
		pkMob = CMobManager::Instance().Get(arg1, true);

		if (pkMob)
			vnum = pkMob->m_table.dwVnum;
	}

	DWORD count;

	for (count = 0; cs_dwMonarchMobVnums[count] != 0; ++count)
		if (cs_dwMonarchMobVnums[count] == vnum)
			break;

	if (0 == cs_dwMonarchMobVnums[count])
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;567]");
		return;
	}

	tch = CHARACTER_MANAGER::instance().SpawnMobRange(vnum,
			ch->GetMapIndex(),
			ch->GetX() - number(200, 750),
			ch->GetY() - number(200, 750),
			ch->GetX() + number(200, 750),
			ch->GetY() + number(200, 750),
			true,
			pkMob->m_table.bType == CHAR_TYPE_STONE,
			true);

	if (tch)
	{
		CMonarch::instance().SendtoDBDecMoney(SummonPrice, ch->GetEmpire(), ch);

		ch->SetMC(CHARACTER::MI_SUMMON);
	}
}

static const char* FN_point_string(int apply_number)
{
	switch (apply_number)
	{
		case POINT_MAX_HP:	return "[LS;568;%d]";
		case POINT_MAX_SP:	return "[LS;569;%d]";
		case POINT_HT:		return "[LS;571;%d]";
		case POINT_IQ:		return "[LS;572;%d]";
		case POINT_ST:		return "[LS;573;%d]";
		case POINT_DX:		return "[LS;574;%d]";
		case POINT_ATT_SPEED:	return "[LS;575;%d]";
		case POINT_MOV_SPEED:	return "[LS;576;%d]";
		case POINT_CASTING_SPEED:	return "[LS;577;%d]";
		case POINT_HP_REGEN:	return "[LS;578;%d]";
		case POINT_SP_REGEN:	return "[LS;579;%d]";
		case POINT_POISON_PCT:	return "[LS;580;%d]";
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_BLEEDING_PCT:	return "[LS;580;%d]";
#endif
		case POINT_STUN_PCT:	return "[LS;582;%d]";
		case POINT_SLOW_PCT:	return "[LS;583;%d]";
		case POINT_CRITICAL_PCT:	return "[LS;584;%d]";
		case POINT_RESIST_CRITICAL:	return "����� ġ��Ÿ Ȯ�� %d%% ����";
		case POINT_PENETRATE_PCT:	return "[LS;585;%d]";
		case POINT_RESIST_PENETRATE: return "����� ���� ���� Ȯ�� %d%% ����";
		case POINT_ATTBONUS_HUMAN:	return "[LS;586;%d]";
		case POINT_ATTBONUS_ANIMAL:	return "[LS;587;%d]";
		case POINT_ATTBONUS_ORC:	return "[LS;588;%d]";
		case POINT_ATTBONUS_MILGYO:	return "[LS;589;%d]";
		case POINT_ATTBONUS_UNDEAD:	return "[LS;590;%d]";
		case POINT_ATTBONUS_DEVIL:	return "[LS;591;%d]";
		case POINT_STEAL_HP:		return "[LS;593;%d]";
		case POINT_STEAL_SP:		return "[LS;594;%d]";
		case POINT_MANA_BURN_PCT:	return "[LS;595;%d]";
		case POINT_DAMAGE_SP_RECOVER:	return "[LS;596;%d]";
		case POINT_BLOCK:			return "[LS;597;%d]";
		case POINT_DODGE:			return "[LS;598;%d]";
		case POINT_RESIST_SWORD:	return "[LS;599;%d]";
		case POINT_RESIST_TWOHAND:	return "[LS;600;%d]";
		case POINT_RESIST_DAGGER:	return "[LS;601;%d]";
		case POINT_RESIST_BELL:		return "[LS;602;%d]";
		case POINT_RESIST_FAN:		return "[LS;604;%d]";
		case POINT_RESIST_BOW:		return "[LS;605;%d]";
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_RESIST_CLAW:		return "[LS;601;%d]";
#endif
		case POINT_RESIST_FIRE:		return "[LS;606;%d]";
		case POINT_RESIST_ELEC:		return "[LS;607;%d]";
		case POINT_RESIST_MAGIC:	return "[LS;608;%d]";
#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
		case POINT_RESIST_MAGIC_REDUCTION:	return "[LS;608;%d]";
#endif
		case POINT_RESIST_WIND:		return "[LS;609;%d]";
		case POINT_RESIST_ICE:		return "�ñ� ���� %d%%";
		case POINT_RESIST_EARTH:	return "���� ���� %d%%";
		case POINT_RESIST_DARK:		return "��� ���� %d%%";
		case POINT_REFLECT_MELEE:	return "[LS;610;%d]";
		case POINT_REFLECT_CURSE:	return "[LS;611;%d]";
		case POINT_POISON_REDUCE:	return "[LS;612;%d]";
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_BLEEDING_REDUCE:	return "[LS;612;%d]";
#endif
		case POINT_KILL_SP_RECOVER:	return "[LS;613;%d]";
		case POINT_EXP_DOUBLE_BONUS:	return "[LS;615;%d]";
		case POINT_GOLD_DOUBLE_BONUS:	return "[LS;616;%d]";
		case POINT_ITEM_DROP_BONUS:	return "[LS;617;%d]";
		case POINT_POTION_BONUS:	return "[LS;618;%d]";
		case POINT_KILL_HP_RECOVERY:	return "[LS;619;%d]";
		case POINT_ATT_GRADE_BONUS:	return "[LS;623;%d]";
		case POINT_DEF_GRADE_BONUS:	return "[LS;624;%d]";
		case POINT_MAGIC_ATT_GRADE:	return "[LS;626;%d]";
		case POINT_MAGIC_DEF_GRADE:	return "[LS;627;%d]";
		case POINT_MAX_STAMINA:	return "[LS;628;%d]";
		case POINT_ATTBONUS_WARRIOR:	return "[LS;629;%d]";
		case POINT_ATTBONUS_ASSASSIN:	return "[LS;630;%d]";
		case POINT_ATTBONUS_SURA:		return "[LS;631;%d]";
		case POINT_ATTBONUS_SHAMAN:		return "[LS;632;%d]";
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_ATTBONUS_WOLFMAN:	return "[LS;632;%d]";
#endif
		case POINT_ATTBONUS_MONSTER:	return "[LS;633;%d]";
		case POINT_MALL_ATTBONUS:		return "[LS;634;%d]";
		case POINT_MALL_DEFBONUS:		return "[LS;635;%d]";
		case POINT_MALL_EXPBONUS:		return "[LS;637;%d]";
		case POINT_MALL_ITEMBONUS:		return "������ ����� %d��"; // @fixme180 float to int
		case POINT_MALL_GOLDBONUS:		return "�� ����� %d��"; // @fixme180 float to int
		case POINT_MAX_HP_PCT:			return "[LS;640;%d]";
		case POINT_MAX_SP_PCT:			return "[LS;641;%d]";
		case POINT_SKILL_DAMAGE_BONUS:	return "[LS;642;%d]";
		case POINT_NORMAL_HIT_DAMAGE_BONUS:	return "[LS;643;%d]";
		case POINT_SKILL_DEFEND_BONUS:		return "[LS;644;%d]";
		case POINT_NORMAL_HIT_DEFEND_BONUS:	return "[LS;645;%d]";
		case POINT_RESIST_WARRIOR:	return "[LS;646;%d]";
		case POINT_RESIST_ASSASSIN:	return "[LS;648;%d]";
		case POINT_RESIST_SURA:		return "[LS;649;%d]";
		case POINT_RESIST_SHAMAN:	return "[LS;650;%d]";
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_RESIST_WOLFMAN:	return "[LS;650;%d]";
#endif
		default:					return "UNK_ID %d%%"; // @fixme180
	}
}

static bool FN_hair_affect_string(LPCHARACTER ch, char *buf, size_t bufsiz)
{
	if (NULL == ch || NULL == buf)
		return false;

	CAffect* aff = NULL;
	time_t expire = 0;
	struct tm ltm;
	int	year, mon, day;
	int	offset = 0;

	aff = ch->FindAffect(AFFECT_HAIR);

	if (NULL == aff)
		return false;

	expire = ch->GetQuestFlag("hair.limit_time");

	if (expire < get_global_time())
		return false;

	// set apply string
	offset = snprintf(buf, bufsiz, FN_point_string(aff->bApplyOn), aff->lApplyValue);

	if (offset < 0 || offset >= (int) bufsiz)
		offset = bufsiz - 1;

	localtime_r(&expire, &ltm);

	year	= ltm.tm_year + 1900;
	mon		= ltm.tm_mon + 1;
	day		= ltm.tm_mday;

	snprintf(buf + offset, bufsiz - offset, "[LS;651;%d;%d;%d]", year, mon, day);

	return true;
}

ACMD(do_costume)
{
	char buf[1024]; // @warme015
	const size_t bufferSize = sizeof(buf);

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	CItem* pBody = ch->GetWear(WEAR_COSTUME_BODY);
	CItem* pHair = ch->GetWear(WEAR_COSTUME_HAIR);
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	CItem* pMount = ch->GetWear(WEAR_COSTUME_MOUNT);
#endif
#ifdef ENABLE_NEW_MOUNT_SYSTEM
	CItem* pMount = ch->GetWear(WEAR_COSTUME_MOUNT);
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	CItem* pAcce = ch->GetWear(WEAR_COSTUME_ACCE);
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	CItem* pWeapon = ch->GetWear(WEAR_COSTUME_WEAPON);
#endif

	ch->ChatPacket(CHAT_TYPE_INFO, "COSTUME status:");

	if (pHair)
	{
		const char* itemName = pHair->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  HAIR : %s", itemName);

		for (int i = 0; i < pHair->GetAttributeCount(); ++i)
		{
			const TPlayerItemAttribute& attr = pHair->GetAttribute(i);
			if (0 < attr.bType)
			{
				snprintf(buf, bufferSize, FN_point_string(attr.bType), attr.sValue);
				ch->ChatPacket(CHAT_TYPE_INFO, "     %s", buf);
			}
		}

		if (pHair->IsEquipped() && arg1[0] == 'h')
			ch->UnequipItem(pHair);
	}

	if (pBody)
	{
		const char* itemName = pBody->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  BODY : %s", itemName);

		if (pBody->IsEquipped() && arg1[0] == 'b')
			ch->UnequipItem(pBody);
	}

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	if (pMount)
	{
		const char* itemName = pMount->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  MOUNT : %s", itemName);

		if (pMount->IsEquipped() && arg1[0] == 'm')
			ch->UnequipItem(pMount);
	}
#endif

#ifdef ENABLE_NEW_MOUNT_SYSTEM
	if (pMount)
	{
		const char* itemName = pMount->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  MOUNT : %s", itemName);

		if (pMount->IsEquipped() && arg1[0] == 'm')
			ch->UnequipItem(pMount);
	}
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	if (pAcce)
	{
		const char* itemName = pAcce->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  ACCE : %s", itemName);

		if (pAcce->IsEquipped() && arg1[0] == 'a')
			ch->UnequipItem(pAcce);
	}
#endif

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	if (pWeapon)
	{
		const char* itemName = pWeapon->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  WEAPON : %s", itemName);

		if (pWeapon->IsEquipped() && arg1[0] == 'w')
			ch->UnequipItem(pWeapon);
	}
#endif
}

ACMD(do_hair)
{
	char buf[256];

	if (false == FN_hair_affect_string(ch, buf, sizeof(buf)))
		return;

	ch->ChatPacket(CHAT_TYPE_INFO, buf);
}

ACMD(do_inventory)
{
	int	index = 0;
	int	count		= 1;

	char arg1[256];
	char arg2[256];

	LPITEM	item;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: inventory <start_index> <count>");
		return;
	}

	if (!*arg2)
	{
		index = 0;
		str_to_number(count, arg1);
	}
	else
	{
		str_to_number(index, arg1); index = MIN(index, INVENTORY_MAX_NUM);
		str_to_number(count, arg2); count = MIN(count, INVENTORY_MAX_NUM);
	}

	for (int i = 0; i < count; ++i)
	{
		if (index >= INVENTORY_MAX_NUM)
			break;

		item = ch->GetInventoryItem(index);

		ch->ChatPacket(CHAT_TYPE_INFO, "inventory [%d] = %s",
						index, item ? item->GetName() : "<NONE>");
		++index;
	}
}

//gift notify quest command
ACMD(do_gift)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "gift");
}

#ifdef ENABLE_SYSTEMY_KARTY_OKEY
ACMD(do_cards)
{
	const char *line;

	char arg1[256], arg2[256];

	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	switch (LOWER(arg1[0]))
	{
		case 'o':	// open
			if (isdigit(*arg2))
			{
				DWORD safemode;
				str_to_number(safemode, arg2);
				ch->Cards_open(safemode);
			}
			break;
		case 'p':	// open
			ch->Cards_pullout();
			break;
		case 'e':	// open
			ch->CardsEnd();
			break;
		case 'd':	// open
			if (isdigit(*arg2))
			{
				DWORD destroy_index;
				str_to_number(destroy_index, arg2);
				ch->CardsDestroy(destroy_index);
			}
			break;
		case 'a':	// open
			if (isdigit(*arg2))
			{
				DWORD accpet_index;
				str_to_number(accpet_index, arg2);
				ch->CardsAccept(accpet_index);
			}
			break;
		case 'r':	// open
			if (isdigit(*arg2))
			{
				DWORD restore_index;
				str_to_number(restore_index, arg2);
				ch->CardsRestore(restore_index);
			}
			break;
		default:
			return;
	}
}
#endif

ACMD(do_in_game_mall)
{
#ifdef ENABLE_ITEMSHOP
	ch->ChatPacket(CHAT_TYPE_COMMAND, "open_ishop");
#endif
}

ACMD(do_dice)
{


}

#ifdef ENABLE_NEWSTUFF
ACMD(do_click_safebox)
{
	if ((ch->GetGMLevel() <= GM_PLAYER) && (ch->GetDungeon() || ch->GetWarMap()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot open the safebox in dungeon or at war.");
		return;
	}

	ch->SetSafeboxOpenPosition();
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowMeSafeboxPassword");
}
ACMD(do_force_logout)
{
	LPDESC pDesc=DESC_MANAGER::instance().FindByCharacterName(ch->GetName());
	if (!pDesc)
		return;
	pDesc->DelayedDisconnect(0);
}
#endif

ACMD(do_click_mall)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowMeMallPassword");
}

#ifdef ENABLE_NEW_PET_SYSTEM
ACMD(do_pet_skillup)
{
	if (ch->IsObserverMode() || ch->GetExchange())
	{
		return;
	}

	LPITEM pet_item = ch->GetWear(WEAR_NEW_PET);
	if (!pet_item)
	{
		return;
	}

	int skill_id = 0;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1)
	{
		return;
	}

	str_to_number(skill_id, arg1);
	ch->PetSkillUp(skill_id);
}

ACMD(do_pet_name)
{
	if (ch->IsObserverMode() || ch->GetExchange() || !ch->CanWarp())
	{
		return;
	}

	char arg1[CHARACTER_NAME_MAX_LEN + 1];
	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1)
	{
		return;
	}

	ch->PetName(arg1);
}
#endif

ACMD(do_ride)
{
	sys_log(1, "[DO_RIDE] start");
	if (ch->IsDead() || ch->IsStun())
		return;

#ifdef __RENEWAL_MOUNT__
	if (ch->MountBlockMap())
		return;
#endif

	{
		if (ch->IsHorseRiding())
		{
			sys_log(1, "[DO_RIDE] stop riding");
			ch->StopRiding();
			return;
		}

		if (ch->GetMountVnum())
		{
			sys_log(1, "[DO_RIDE] unmount");
			do_unmount(ch, NULL, 0, 0);
			return;
		}
	}

#ifdef __RENEWAL_MOUNT__
	if (ch->GetWear(WEAR_COSTUME_MOUNT))
	{
		DWORD dwNow = get_dword_time();
		DWORD dwRemReceived = (dwNow - ch->GetLastPVPHitReceivedTime() < 5000) ? (5000 - (dwNow - ch->GetLastPVPHitReceivedTime())) : 0;
		DWORD dwRemDealt    = (dwNow - ch->GetLastPVPHitDealtTime()    < 5000) ? (5000 - (dwNow - ch->GetLastPVPHitDealtTime()))    : 0;
		DWORD dwRem = std::max(dwRemReceived, dwRemDealt);
		if (dwRem > 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot mount during PvP. Wait %d second(s).", (dwRem + 999) / 1000);
			return;
		}
		ch->StartRiding();
		return;
	}
	else
#endif
	{
		DWORD dwNow = get_dword_time();
		DWORD dwRemReceived = (dwNow - ch->GetLastPVPHitReceivedTime() < 5000) ? (5000 - (dwNow - ch->GetLastPVPHitReceivedTime())) : 0;
		DWORD dwRemDealt    = (dwNow - ch->GetLastPVPHitDealtTime()    < 5000) ? (5000 - (dwNow - ch->GetLastPVPHitDealtTime()))    : 0;
		DWORD dwRem = std::max(dwRemReceived, dwRemDealt);
		if (dwRem > 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10151;%d]", (dwRem + 999) / 1000);
			return;
		}
		
		if (ch->GetHorse() != NULL)
		{
			sys_log(1, "[DO_RIDE] start riding");
			ch->StartRiding();
			return;
		}

		for (BYTE i=0; i<INVENTORY_MAX_NUM; ++i)
		{
			LPITEM item = ch->GetInventoryItem(i);
			if (NULL == item)
				continue;
#ifdef __RENEWAL_MOUNT__
			else if (item->IsCostumeMountItem())
			{
				ch->EquipItem(item);
				return;
			}
#endif
			else if (item->IsRideItem())
			{
				ch->EquipItem(item);
				return;
			}
		}
	}

	ch->ChatPacket(CHAT_TYPE_INFO, "[LS;501]");
}

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
ACMD(do_open_offline_shop)
{
	// If character is dead, return false
	if (ch->IsDead())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2395]");
		return;
	}

	// If character is exchanging with someone, return false
	if (ch->GetExchange())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2395]");
		return;
	}

	// If character has a private shop, return false
	if (ch->GetMyShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2395]");
		return;
	}

	// If character is look at one offline shop, return false
	if (ch->GetOfflineShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2395]");
		return;
	}

	// If cube window is open, return false
	if (ch->IsCubeOpen())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2395]");
		return;
	}

	// If character is a gm and this server is not test server, return false
	if (ch->GetGMLevel() > GM_LOW_WIZARD && ch->GetGMLevel() < GM_IMPLEMENTOR)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2383]");
		return;
	}

	// If character's safebox is open, return false
	if (ch->IsOpenSafebox())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2395]");
		return;
	}

	// If character's shop window is open, return false
	if (ch->GetShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2395]");
		return;
	}

	// Send the command to client.
	ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenOfflineShop");
}

ACMD(do_offline_shop_open_panel)
{
	// if (ch->GetLastTimePanelOpened() + 2000 > get_dword_time())
	// {
	// 	ch->ChatPacket(CHAT_TYPE_INFO, "You have to wait 2 seconds before opening the pannel.");
	// 	return;
	// }
	
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT time, x, y, mapIndex, channel, sign FROM %soffline_shop_npc WHERE owner_id = %u", get_table_postfix(), ch->GetPlayerID()));
	MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);

	if (pMsg->Get()->uiNumRows)
	{
		DWORD index = 0, pos_x = 0, pos_y = 0, time = 0, channel = 0;
		str_to_number(time, row[0]);
		str_to_number(pos_x, row[1]);
		str_to_number(pos_y, row[2]);
		str_to_number(index, row[3]);
		str_to_number(channel, row[4]);
		
		ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenOfflineShopPanel %d:%d:%d:%d:%d", time, pos_x, pos_y, index, channel);
	}
	else
		ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenOfflineShopPanel 0:0:0:0:0");
	
	// ch->SetLastTimePanelOpened(get_dword_time());
}
ACMD(do_offline_shop_open_logs)
{
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::OfflineLogs, std::chrono::milliseconds(8000))) {
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Remaining time: %.2f"), PULSEMANAGER_CLOCK_TO_SEC2(ch->GetPlayerID(), ePulse::OfflineLogs));
		return;
	}
	ch->SendLogs();
	ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenOfflineShopLogs");
}
#endif

#ifdef __ENABLE_COLLECTIONS_SYSTEM__
ACMD(do_add_collect_item)
{
	if (!ch || !ch->IsPC())
	{
		return;
	}

	char arg1[256];
	char arg2[256];
	char arg3[256];

	const char* line;
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));

	if (!*arg1 || !*arg2 || !*arg3)
	{
		return;
	}

	if (!ch->CanDoAction())
	{
		return;
	}

	BYTE collectID = std::atoi(arg1);
	BYTE itemID = std::atoi(arg2);
	BYTE isAll = std::atoi(arg3);
	CSystemCollections::instance().AddItem(ch, collectID, itemID, (isAll == 1) ? true : false);
}
#endif

#ifdef ENABLE_EVENT_MANAGER
ACMD(do_event_manager)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 2) { return; }
	else if (vecArgs[1] == "info")
	{
		CHARACTER_MANAGER::Instance().SendDataPlayer(ch);
	}
	else if (vecArgs[1] == "remove")
	{
		if (!ch->IsGM())
			return;

		if (vecArgs.size() < 3) { 
			
			ch->ChatPacket(CHAT_TYPE_INFO, "put the event index!!");
			return; 
		}

		BYTE removeIndex;
		str_to_number(removeIndex, vecArgs[2].c_str());

		if(CHARACTER_MANAGER::Instance().CloseEventManuel(removeIndex))
			ch->ChatPacket(CHAT_TYPE_INFO, "successfuly remove!");
		else
			ch->ChatPacket(CHAT_TYPE_INFO, "dont has any event!");
	}
	else if (vecArgs[1] == "update")
	{
		if (!ch->IsGM())
			return;
		const BYTE subHeader = EVENT_MANAGER_UPDATE;
		//db_clientdesc->DBPacketHeader(HEADER_GD_EVENT_MANAGER, 0, sizeof(BYTE));
		//db_clientdesc->Packet(&subHeader, sizeof(BYTE));
		db_clientdesc->DBPacket(HEADER_GD_EVENT_MANAGER, 0, &subHeader, sizeof(BYTE));

		ch->ChatPacket(CHAT_TYPE_INFO, "successfully update!");
	}
}
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__

static const std::set<int> ALLOWED_SHOP_ZONES = { 1, 41 };

bool IsAllowedShopZone(int nMapIndex)
{
    return ALLOWED_SHOP_ZONES.count(nMapIndex) > 0;
}

ACMD(do_register_off_shop)
{
    if (IsAllowedShopZone(ch->GetMapIndex()))
    {
#ifdef __PREMIUM_PRIVATE_SHOP__
		if (ch->IsPrivateShopOwner())
		{
			if (ch->GetPrivateShopTable()->llGold > 0 || ch->GetPrivateShopTable()->dwCheque > 0)
			{
				ch->OpenPrivateShopPanel();
				return;
			}
			else
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Close your current personal shop before opening a new one.");
				return;
			}
		}

		ch->OpenPrivateShopPanel();
#else
		ch->__OpenPrivateShop();
#endif
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "YYou cannot open a private shop in this map.");
	}
}
#endif

ACMD(do_refund_items)
{
	{
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT COUNT(*) FROM %soffline_shop_item WHERE owner_id = %u", get_table_postfix(), ch->GetPlayerID()));
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		
		int bResult = 0;
		str_to_number(bResult, row[0]);
		
		if (bResult)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "SHOP_OFFLINE_BACK_ITEM");
			char szQuery[2048];
			snprintf(szQuery, sizeof(szQuery), "SELECT pos,count,vnum,type,subtype,"
												"socket0,socket1,socket2,socket3,socket4,socket5, attrtype0, attrvalue0, attrtype1, attrvalue1, attrtype2, attrvalue2, attrtype3, attrvalue3, attrtype4, attrvalue4, attrtype5, attrvalue5, attrtype6, attrvalue6 FROM %soffline_shop_item WHERE owner_id = %u", get_table_postfix(), ch->GetPlayerID());

			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
			
			if (pMsg->Get()->uiNumRows == 0)
				return;
			
			MYSQL_ROW row;
			while (NULL != (row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
			{
				int cur = 0;
				
				TPlayerItem item;
				str_to_number(item.pos, row[cur++]);
				str_to_number(item.count, row[cur++]);
				str_to_number(item.vnum, row[cur++]);

				TItemTable* proto = ITEM_MANAGER::instance().GetTable(item.vnum);

				str_to_number(proto->bType, row[cur++]);
				str_to_number(proto->bSubType, row[cur++]);
				
				// Set Sockets
				for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
					str_to_number(item.alSockets[i], row[cur++]);
				// End Of Set Sockets

				// Set Attributes
				for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
				{
					str_to_number(item.aAttr[i].bType, row[cur++]);
					str_to_number(item.aAttr[i].sValue, row[cur++]);
				}
				// End Of Set Attributes

				LPITEM pItem = ITEM_MANAGER::instance().CreateItem(item.vnum, item.count);
				if (pItem)
				{
					int iEmptyPos = 0;
					
					if (pItem->IsDragonSoul())
						iEmptyPos = ch->GetEmptyDragonSoulInventory(pItem);
		#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
					else if (pItem->IsSkillBook())
						iEmptyPos = ch->GetEmptySkillBookInventory(pItem->GetSize());
					else if (pItem->IsUpgradeItem())
						iEmptyPos = ch->GetEmptyUpgradeItemsInventory(pItem->GetSize());
					else if (pItem->IsStone())
						iEmptyPos = ch->GetEmptyStoneInventory(pItem->GetSize());
					else if (pItem->IsBox())
						iEmptyPos = ch->GetEmptyBoxInventory(pItem->GetSize());
					else if (pItem->IsEfsun())
						iEmptyPos = ch->GetEmptyEfsunInventory(pItem->GetSize());
					else if (pItem->IsCicek())
						iEmptyPos = ch->GetEmptyCicekInventory(pItem->GetSize());
		#endif
					else
						iEmptyPos = ch->GetEmptyInventory(pItem->GetSize());

					if (iEmptyPos < 0)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "SHOP_OFFLINE_EQ_FULL");
						return;
					}
					
					pItem->SetSockets(item.alSockets);
					pItem->SetAttributes(item.aAttr);
					if (pItem->IsDragonSoul())
						pItem->AddToCharacter(ch, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyPos));
		#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
					else if (pItem->IsSkillBook())
						pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
					else if (pItem->IsUpgradeItem())
						pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
					else if (pItem->IsStone())
						pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
					else if (pItem->IsBox())
						pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
					else if (pItem->IsEfsun())
						pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
					else if (pItem->IsCicek())
						pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
		#endif
					else
						pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));

					ch->ChatPacket(CHAT_TYPE_INFO, "SHOP_OFFLINE_GIVE_BACK", pItem->GetName());
					DBManager::instance().DirectQuery("DELETE FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), ch->GetPlayerID(), item.pos);
				}
			}	
		}
	}
}


ACMD(do_stack_items)
{
	if (!ch->CanWarp() || quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID())->IsRunning())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2208]");
		return;
	}
	
	if (ch->GetExchange() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->GetMyShop() || ch->IsCubeOpen() || ch->GetShop() || !ch->CanWarp()
#ifdef __ENABLE_POLYMORPH_SYSTEM__
		|| ch->IsPolyShopping()
#endif
	)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2208]");
		return;
	}

	int lastStackTime = ch->GetQuestFlag("systems.stack_last_time");
	if (get_global_time() - lastStackTime < 60)
	{
		return;
	}
	ch->SetQuestFlag("systems.stack_last_time", get_global_time());

	std::map<uint32_t, std::vector<LPITEM>> mapItemsByVnum;

	for (uint16_t i = 0; i < INVENTORY_MAX_NUM; i++)
	{
		LPITEM item = ch->GetInventoryItem(i);
		if (item)
		{
			if (!item->IsStackable() || IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
				continue;

			if (item->IsExchanging() || item->isLocked() || item->IsEquipped())
				continue;

#ifdef __SOULBINDING_SYSTEM__
			if (item->IsBind() || item->IsUntilBind())
				continue;
#endif

			mapItemsByVnum[item->GetOriginalVnum()].push_back(item);
		}
	}

	for (const auto& data : mapItemsByVnum)
	{
		if (data.second.size() <= 1)
			continue;

		auto i = data.second.end();
		while (i != data.second.begin())
		{
			--i;

			LPITEM item = *i;
			if (!item)
				continue;

			uint16_t leftCount = item->GetCount();

			int firstIdx = -1;
			while (leftCount > 0)
			{
				firstIdx++;
				if (firstIdx >= data.second.size())
					break;

				LPITEM firstItem = data.second.at(firstIdx);
				if (!firstItem)
					continue;

				if (firstItem == item)
					break;

				if (!firstItem->IsSameItemData(item))
					continue;

				if (firstItem->GetCount() >= ITEM_MAX_COUNT)
					continue;

				const uint16_t toExchangeCount = MIN(ITEM_MAX_COUNT - firstItem->GetCount(), leftCount);
				leftCount -= toExchangeCount;
				firstItem->SetCount(firstItem->GetCount() + toExchangeCount);
				item->SetCount(leftCount);
			}
		}
	}

	ch->FlushDelayedSaveItem();
}

#ifdef __DUNGEON_INFO_ENABLE__
ACMD(do_dungeon_info_open_panel)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "DungeonInfo_OpenPanel");
}

ACMD(do_dungeon_info_join_dungeon)
{
	if (!ch->CanDoAction())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;373]");
		//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANNOT_DO_THIS_NOW", ch));
		return;
	}
	
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	CDungeonInfoManager::instance().JoinDungeon(ch, arg1);
}

ACMD(do_dungeon_info_reset_cooldown_dungeon)
{
	if (!ch->CanDoAction())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;373]");
		//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANNOT_DO_THIS_NOW", ch));
		return;
	}
	
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	CDungeonInfoManager::instance().ClearDelay(ch, arg1);
}

#ifdef __DUNGEON_RETURN_ENABLE__
ACMD(do_dungeon_info_rejoin_dungeon)
{
	if (!ch->CanDoAction())
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	CDungeonInfoManager::instance().ReJoinDungeon(ch, arg1);
}
#endif
#endif

#ifdef __CASKET_PREVIEW_ENABLE__
ACMD(do_fetch_casket_items)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	DWORD dwVnum = atoi(arg1);
	// Support for special_item_group
	{
		const auto ptrGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(dwVnum);
		if (ptrGroup)
		{
			for (const auto & rItem : ptrGroup->m_vecItems)
			{
				ch->ChatPacket(CHAT_TYPE_COMMAND, "AddCasketItem %u %u %u", dwVnum, rItem.vnum, rItem.count);
			}

			ch->ChatPacket(CHAT_TYPE_COMMAND, "SortCasketWindow");
			return;
		}
	}

	if (m_casket_preview.find(dwVnum) == m_casket_preview.end())
	{
		return;
	}

	for (const auto & v_casket_content : m_casket_preview[dwVnum])
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "AddCasketItem %u %u %u", dwVnum, v_casket_content.first, v_casket_content.second);
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, "SortCasketWindow");
}
#endif

#ifdef __INVENTORY_BUFFERING__
ACMD(do_quick_open)
{
	if (!ch)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	
	if (!*arg1 || !isnhdigit(*arg1))
		return;

	LPITEM item = ch->GetInventoryItem(atoi(arg1));
	if (!item)
		return;

	ch->QuickOpenStack(item);
}
#endif

#ifdef ENABLE_BOT_CONTROL
ACMD(do_bot_control)
{
	if (!ch)
		return;
}

ACMD(do_bot_control_open)
{
	if (!ch)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	bool state = false;
	str_to_number(state, arg1);

	ch->SetBotControl(state);
}
#endif

#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
void BroadcastCmdchat(const char* c_pszBuf)
{
	TPacketGGCmdchat p;
	p.bHeader = HEADER_GG_CMDCHAT;
	p.lSize = strlen(c_pszBuf) + 1;

	TEMP_BUFFER buf;
	buf.write(&p, sizeof(p));
	buf.write(c_pszBuf, p.lSize);

	P2P_MANAGER::instance().Send(buf.read_peek(), buf.size());
	
	SendCmdchatToAll(c_pszBuf);
}

struct cmdchat_to_all_packet_func
{
	const char* m_str;
	cmdchat_to_all_packet_func(const char* str) : m_str(str) {}

	void operator () (LPDESC d)
	{
		if (!d->GetCharacter())
			return;

		d->GetCharacter()->ChatPacket(CHAT_TYPE_COMMAND, "%s", m_str);
	}
};

void SendCmdchatToAll(const char* c_pszBuf)
{
	const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();
	std::for_each(c_ref_set.begin(), c_ref_set.end(), cmdchat_to_all_packet_func(c_pszBuf));
}

struct EnlightNeedItemDataStruct
{
	uint32_t vnum;
	uint16_t count;
};
struct EnlightNeedDataStruct
{
	int64_t yang;
	int64_t won;
	int64_t po;
	std::vector<EnlightNeedItemDataStruct> needItems;
};
ACMD(do_enlight)
{
	static const EnlightNeedDataStruct ENLIGHT_NEED_DATA[ENLIGHT_LEVEL_MAX+1] = {
		{0, 0, 0, {}},	// zerowy poziom, kazdy ma na start
		{0, 0, 0, {}},	// pierwszy poziom, wbija sie misja
		{0, 50, 250000, {{90029,50},{90033,500},{30502,500},{34000,50}}},
		{0, 100, 500000, {{90029,50},{90033,500},{30502,500},{34000,50}}},
		// {600, 0, 100, {{18,1}, {19,1},{27992,30},{27993,30},{27994,30}}},
		// {700, 0, 100, {{18,1}, {19,1},{27992,35},{27993,35},{27994,35}}},
		// {800, 0, 100, {{18,1}, {19,1},{27992,40},{27993,40},{27994,40}}},
		// {900, 0, 100, {{18,1}, {19,1},{27992,45},{27993,45},{27994,45}}},
		// {1000, 0, 100, {{18,1}, {19,1},{27992,50},{27993,50},{27994,50}}},
		// {1500, 0, 500, {{18,1}, {19,1},{27992,100},{27993,100},{27994,010}}},
	};
	if (!ch->CanWarp() || quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID())->IsRunning())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ODCZEKAJ_CHWILE"));
		return;
	}

	if (ch->GetEnlightLevel() == 0 || ch->GetEnlightLevel() >= ENLIGHT_LEVEL_MAX)
		return;

	const EnlightNeedDataStruct& needData = ENLIGHT_NEED_DATA[ch->GetEnlightLevel()+1];

	if (ch->GetGold() < needData.yang)
		return;

	if (ch->GetCheque() < needData.won)
		return;

	if (ch->GetPktOsiag() < needData.po)
		return;

	for (const auto& itemData : needData.needItems)
	{
		if (ch->CountSpecifyItem(itemData.vnum) < itemData.count)
			return;
	}

	if (needData.yang > 0)
		ch->PointChange(POINT_GOLD, -needData.yang, true);

	if (needData.won > 0)
		ch->PointChange(POINT_CHEQUE, -needData.won, true);

	if (needData.po > 0)
		ch->PointChange(POINT_PKT_OSIAG, -needData.po, true);

	for (const auto& itemData : needData.needItems)
	{
		ch->RemoveSpecifyItem(itemData.vnum, itemData.count);
	}

	ch->SetEnlightLevel(ch->GetEnlightLevel() + 1);
	ch->Save();
	ch->ViewReencode();
	ch->ComputePoints();

	ch->ChatPacket(CHAT_TYPE_COMMAND, "enlight_r");
	char buf[128];
	snprintf(buf, sizeof(buf), "enlight_n %s %u", ch->GetName(), ch->GetEnlightLevel());
	BroadcastCmdchat(buf);
	
	char noticeBuff[128];
	snprintf(noticeBuff, sizeof(noticeBuff), LC_TEXT("<Enlight System> Player %s reached enlight level: %d"), ch->GetName(), ch->GetEnlightLevel());
	BroadcastNotice(noticeBuff);
}
#endif

#ifdef __WORLD_BOSS_YUMA__
ACMD(do_boss_debug)
{
	CWorldBossManager::instance().GetWorldbossInfo(ch);
}
#endif

#ifdef DUNGEON_CARDS
ACMD(do_dungeon_cards)
{
	if (!ch->IsPC() || NULL == ch)
		return;

	char szNumber[3];
	one_argument(argument, szNumber, sizeof(szNumber));

	int num = 0;
	str_to_number(num, szNumber);
	
	int randomItem = number(0, 11);
	int randomItemCount = number(1, 5);
	int randomItemM = number(0, 12);
	int randomItemCountM = number(1, 5);
	int randomItemH = number(0, 14);
	int randomItemCountH = number(1, 5);
	
	int dropTableLow[12][6] = {
		{203000, 1, 1, 1, 1, 2},
		{80050, 1, 1, 1, 1, 2},
		{80030, 1, 1, 1, 1, 2},
		{71051, 1, 1, 1, 1, 1},
		{201001, 1, 1, 1, 2, 2},
		{201002, 1, 1, 1, 2, 2},
		{203004, 1, 1, 1, 1, 1},
		{203005, 1, 1, 1, 1, 1},
		{203006, 1, 1, 1, 1, 1},
		{11024, 2, 2, 2, 3, 3},
		{80020, 2, 2, 2, 3, 3},
		{50099, 1, 1, 1, 1, 2},
	};

	int dropTableMedium[13][6] = {
		{203000, 1, 1, 1, 1, 2},
		{80051, 1, 1, 1, 1, 2},
		{80052, 1, 1, 1, 1, 2},
		{80030, 1, 1, 1, 1, 2},
		{71051, 1, 1, 1, 2, 2},
		{82118, 1, 1, 1, 1, 1},
		{82304, 1, 1, 1, 2, 3},
		{82300, 1, 1, 1, 1, 2},
		{82108, 1, 1, 1, 1, 2},
		{82109, 1, 1, 1, 1, 2},
		{82110, 1, 1, 1, 1, 2},
		{82045, 1, 1, 1, 1, 1},
		{71051, 1, 1, 1, 1, 1},
	};

	int dropTableHigh[15][6] = {
		{203000, 1, 1, 1, 2, 2},
		{80053, 1, 1, 1, 1, 2},
		{80054, 1, 1, 1, 1, 2},
		{80030, 1, 1, 1, 1, 2},
		{71051, 1, 1, 2, 2, 2},
		{82118, 1, 1, 1, 1, 1},
		{82304, 1, 1, 1, 2, 3},
		{82300, 1, 1, 1, 1, 2},
		{82301, 1, 1, 1, 1, 2},
		{82108, 1, 1, 1, 1, 2},
		{82109, 1, 1, 1, 1, 2},
		{82110, 1, 1, 1, 1, 2},
		{82111, 1, 1, 1, 1, 2},
		{82045, 1, 1, 1, 1, 1},
		{71051, 1, 1, 1, 1, 1},
	};

	int randomItemLow = dropTableLow[randomItem][0];
	int randomItemCountLow = dropTableLow[randomItem][randomItemCount];

	int randomItemMedium = dropTableMedium[randomItemM][0];
	int randomItemCountMedium = dropTableMedium[randomItemM][randomItemCountM];

	int randomItemHigh = dropTableHigh[randomItemH][0];
	int randomItemCountHigh = dropTableHigh[randomItemH][randomItemCountH];

	int QuestFlagCardLow = ch->GetQuestFlag("dungeon_cards.can_open_low");
	int QuestFlagCardMedium = ch->GetQuestFlag("dungeon_cards.can_open_medium");
	int QuestFlagCardHigh = ch->GetQuestFlag("dungeon_cards.can_open_high");

	if (QuestFlagCardLow > 0)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "REV_CARD_REWARD %i", randomItemLow);
		ch->AutoGiveItem(randomItemLow, randomItemCountLow);
		ch->SetQuestFlag("dungeon_cards.can_open_low", 0);
	}
	else if (QuestFlagCardMedium > 0)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "REV_CARD_REWARD %i", randomItemMedium);
		ch->AutoGiveItem(randomItemMedium, randomItemCountMedium);
		ch->SetQuestFlag("dungeon_cards.can_open_medium", 0);
	}
	else if (QuestFlagCardHigh > 0)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "REV_CARD_REWARD %i", randomItemHigh);
		ch->AutoGiveItem(randomItemHigh, randomItemCountHigh);
		ch->SetQuestFlag("dungeon_cards.can_open_high", 0);
	}
}
#endif

#ifdef __SPIN_WHEEL__
ACMD(do_spin_wheel)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 2) { return; }
	else if (vecArgs[1] == "spin")
	{
		if (ch->IsHack() || !ch->CanHandleItem() || ch->GetQuestFlag("spin_wheel.count") < 15 || quest::CQuestManager::instance().GetEventFlag("spin_wheel") != 1)
			return;
		ch->SetQuestFlag("spin_wheel.count", 0);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "SetSpinWheel 0 1");

		struct spinItem
		{
			BYTE rareType;
			DWORD itemIdx;
			WORD itemCount;
			spinItem(const BYTE _rareType, const DWORD _itemIdx, const WORD _itemCount) : rareType(_rareType), itemIdx(_itemIdx), itemCount(_itemCount) {}
			spinItem() {}
		};
		struct spinStruct
		{
			WORD wStartLevel, wEndLevel;
			std::vector<spinItem> vecItems;
			spinItem jackPotItem;
			spinStruct(const WORD _wStartLevel, const WORD _wEndLevel, const std::vector<spinItem>& _vecItems, const spinItem& _jackPotItem) : wStartLevel(_wStartLevel), wEndLevel(_wEndLevel), vecItems(_vecItems), jackPotItem(){
				std::memcpy(&jackPotItem, &_jackPotItem, sizeof(jackPotItem));
			}
		};

		const static std::vector<spinStruct> vecSpinData = {
			spinStruct(
				1, 
				80, 
				{
					spinItem(0, 25041, 5), spinItem(0, 50255, 70), spinItem(0, 50255, 60), spinItem(0, 50255, 50), spinItem(0, 50255, 40), spinItem(0, 50255, 30), spinItem(0, 50255, 20), spinItem(0, 50255, 10), spinItem(0, 6884, 20), spinItem(0, 30400, 500), spinItem(0, 71084, 500), spinItem(0, 31108, 500), spinItem(0, 31107, 500), spinItem(0, 25041, 10), spinItem(0, 50513, 50), spinItem(0, 30005, 50), spinItem(0, 51001, 100), spinItem(0, 71084, 200), spinItem(0, 49273, 5), spinItem(0, 6884, 15), spinItem(0, 80030, 5), spinItem(0, 51112, 5), spinItem(0, 50186, 5), spinItem(0, 6884, 10), spinItem(0, 50255, 80)
				},
				spinItem(3, 50512, 1)
			),
			spinStruct(
				80,
				90,
				{
					spinItem(0, 25041, 5), spinItem(0, 50255, 70), spinItem(0, 50255, 60), spinItem(0, 50255, 50), spinItem(0, 50255, 40), spinItem(0, 50255, 30), spinItem(0, 50255, 20), spinItem(0, 50255, 10), spinItem(0, 6884, 20), spinItem(0, 30400, 500), spinItem(0, 71084, 500), spinItem(0, 31108, 500), spinItem(0, 31107, 500), spinItem(0, 25041, 10), spinItem(0, 50513, 50), spinItem(0, 30005, 50), spinItem(0, 51001, 100), spinItem(0, 71084, 200), spinItem(0, 51110, 5), spinItem(0, 6884, 15), spinItem(0, 51111, 5), spinItem(0, 51112, 5), spinItem(0, 50186, 5), spinItem(0, 6884, 10), spinItem(0, 50255, 80)
				},
				spinItem(3, 20572, 1)
			),
			spinStruct(
				100,
				110,
				{
					spinItem(0, 25041, 5), spinItem(0, 50255, 70), spinItem(0, 50255, 60), spinItem(0, 50255, 50), spinItem(0, 50255, 40), spinItem(0, 51116, 3), spinItem(0, 54705, 3), spinItem(0, 50266, 3), spinItem(0, 6884, 20), spinItem(0, 30400, 500), spinItem(0, 71084, 500), spinItem(0, 31108, 500), spinItem(0, 31107, 500), spinItem(0, 25041, 10), spinItem(0, 50513, 50), spinItem(0, 30005, 50), spinItem(0, 51001, 100), spinItem(0, 71084, 200), spinItem(0, 49273, 5), spinItem(0, 6884, 15), spinItem(0, 80030, 5), spinItem(0, 51112, 5), spinItem(0, 50186, 5), spinItem(0, 6884, 10), spinItem(0, 50255, 80)
				},
				spinItem(3, 60301, 1)
			),
			spinStruct(
				110,
				120,
				{
					spinItem(0, 25041, 5), spinItem(0, 70429, 1), spinItem(0, 70412, 1), spinItem(0, 70411, 1), spinItem(0, 49274, 3), spinItem(0, 51116, 3), spinItem(0, 54705, 3), spinItem(0, 50266, 3), spinItem(0, 6884, 20), spinItem(0, 30400, 500), spinItem(0, 71084, 500), spinItem(0, 31108, 500), spinItem(0, 31107, 500), spinItem(0, 25041, 10), spinItem(0, 50513, 50), spinItem(0, 30005, 50), spinItem(0, 51001, 100), spinItem(0, 71084, 200), spinItem(0, 49273, 5), spinItem(0, 6884, 15), spinItem(0, 80030, 5), spinItem(0, 51112, 5), spinItem(0, 50186, 5), spinItem(0, 6884, 10), spinItem(0, 50255, 80)
				},
				spinItem(3, 60300, 1)
			),
		};

		for (auto it = vecSpinData.begin(); it != vecSpinData.end(); ++it)
		{
			const spinStruct& spinData = *it;
			if (ch->GetLevel() >= spinData.wStartLevel && ch->GetLevel() <= spinData.wEndLevel)
			{
				DWORD selectedItemIdx = 0, selectedItemCount = 0;

				if (number(0, 5) == 1)
				{
					selectedItemIdx = spinData.jackPotItem.itemIdx;
					selectedItemCount = spinData.jackPotItem.itemCount;
				}
				else
				{
					const WORD idx = spinData.vecItems.size() <= 1 ? 0 : number(0, spinData.vecItems.size() - 1);
					selectedItemIdx = spinData.vecItems[idx].itemIdx;
					selectedItemCount = spinData.vecItems[idx].itemCount;

				}

				std::string cmd("");
				for (BYTE j = 0; j < spinData.vecItems.size(); ++j)
				{
					cmd += std::to_string(spinData.vecItems[j].rareType);
					cmd += "|";
					cmd += std::to_string(spinData.vecItems[j].itemIdx);
					cmd += "|";
					cmd += std::to_string(spinData.vecItems[j].itemCount);
					cmd += "?";
				}

				cmd += std::to_string(spinData.jackPotItem.rareType);
				cmd += "|";
				cmd += std::to_string(spinData.jackPotItem.itemIdx);
				cmd += "|";
				cmd += std::to_string(spinData.jackPotItem.itemCount);

				if (cmd == "")
					cmd = "-";
				ch->ChatPacket(CHAT_TYPE_COMMAND, "SetSpinReward %u %u %s", selectedItemIdx, selectedItemCount, cmd.c_str());
				ch->SetProtectTime("spint_itemidx", selectedItemIdx);
				ch->SetProtectTime("spint_itemcount", selectedItemCount);
				return;
			}
		}
	}
	else if (vecArgs[1] == "ani_done")
	{
		if (!ch->GetProtectTime("spint_itemidx") || !ch->GetProtectTime("spint_itemcount"))
			return;
		ch->AutoGiveItem(ch->GetProtectTime("spint_itemidx"), ch->GetProtectTime("spint_itemcount"));
		ch->SetProtectTime("spint_itemidx", 0);
		ch->SetProtectTime("spint_itemcount", 0);
		ch->ChatPacket(CHAT_TYPE_INFO, "Successfuly gived spin reward.");
	}
}
#endif
ACMD(do_teleport)
{
	// Parse arguments using the argument parser
	auto args = SplitArguments(argument);

	if (args.empty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: teleport <index>");
		return;
	}

	// Check basic requirements
	if (!ch->CanWarp())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10077]");
		return;
	}

	//if (ch->GetQuestFlag("rozpoczecie.rozpoczecie_1") == 0 && ch->GetMapIndex() == 250)
	//{
	//	ch->ChatPacket(CHAT_TYPE_INFO, "Ještě jsi nedokončil začátečnický dungeon.");
	//	return;
	//}

	// Parse index using the argument parser
	int index = 0;
	if (!IsNumber(args[0]) || !TryParseInt(args[0], index))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10076]");
		return;
	}

	// Find teleport location by index
	const std::tuple<int, int, int, int, int, int>* targetLocation = nullptr;

	for (const auto& location : TELEPORT_DATA)
	{
		if (std::get<0>(location) == index && index > 0)
		{
			targetLocation = &location;
			break;
		}
	}

	if (!targetLocation)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10075]");
		return;
	}

	// Extract location data
	int locationIndex = std::get<0>(*targetLocation);
	int mapIndex = std::get<1>(*targetLocation);
	int x = std::get<2>(*targetLocation);
	int y = std::get<3>(*targetLocation);
	int minLevel = std::get<4>(*targetLocation);
	int maxLevel = std::get<5>(*targetLocation);

	// Check level requirements
	int playerLevel = ch->GetLevel();
	if (playerLevel < minLevel)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10072;%d]", minLevel);
		return;
	}

	if (maxLevel < 999 && playerLevel > maxLevel)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10073;%d]", maxLevel);
		return;
	}

	// Execute teleportation
	ch->WarpSet(x, y, mapIndex);
	ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10074]");
}


#ifdef ENABLE_WHEEL_OF_FORTUNE
ACMD(do_wheel_of_fortune)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 2) { return; }
	else if (vecArgs[1] == "start")
	{
		if (ch->CountSpecifyItem(201230) <= 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10091]"); // Not enough wheel tokens
			return;
		}
		else if (ch->GetProtectTime("WheelWorking") != 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10092]"); // Wheel already in progress
			return;
		}
		ch->RemoveSpecifyItem(201230, 1);
		
		std::vector<std::pair<long, long>> m_important_item = {
			{80032,10},
			{201222,20},
			{80040,1},
		};
		
		std::map<std::pair<long, long>, int> m_normal_item = {
			{{40335,1},1},    // 0.1% chance
			{{40336,1},1},    // 0.1% chance
			{{40337,1},1},    // 0.1% chance
			{{40338,1},1},    // 0.1% chance
			{{40339,1},1},    // 0.1% chance
			{{40340,1},1},    // 0.1% chance
			{{40341,1},1},    // 0.1% chance
			{{41738,1},1},    // 0.1% chance
			{{41739,1},1},    // 0.1% chance
			{{46034,1},1},    // 0.1% chance
			{{46035,1},1},    // 0.1% chance
			{{85559,1},1},    // 0.1% chance
			{{201231,1},2},   // 0.2% chance
			{{201229,1},2},   // 0.2% chance
			{{201224,1},2},   // 0.2% chance
			{{201225,1},600}, // 60% chance
			{{201226,1},600}, // 60% chance
			{{201227,1},600}, // 60% chance
			{{201228,1},600}, // 60% chance
			{{201223,5},600}, // 60% chance
			{{201221,1},200}, // 20% chance
			{{52940,1},1},    // 0.1% chance
			{{52941,1},1},    // 0.1% chance
			{{53930,1},1},    // 0.1% chance
			{{53931,1},1},    // 0.1% chance
			{{203000,3},600}, // 60% chance
			{{203001,3},600}, // 60% chance
			{{203002,3},600}, // 60% chance
			{{203003,3},600}  // 60% chance
		};
		
		std::vector<std::pair<long, long>> m_send_items;
		if (m_important_item.size())
		{
			int random = number(0,m_important_item.size()-1);
			m_send_items.emplace_back(m_important_item[random].first, m_important_item[random].second);
		}
		while (true)
		{
			for (auto it = m_normal_item.begin(); it != m_normal_item.end(); ++it)
			{
				int randomEx = number(0,4);
				if (randomEx == 4)
				{
					int random = number(0,1000);
					if (it->second >= random)
					{
						auto itFind = std::find(m_send_items.begin(), m_send_items.end(), it->first);
						if (itFind == m_send_items.end())
						{
							m_send_items.emplace_back(it->first.first, it->first.second);
							if (m_send_items.size() >= 10)
								break;
						}
					}
				}
			}
			if (m_send_items.size() >= 10)
				break;
		}
		std::string cmd_wheel = "";
		if (m_send_items.size())
		{
			for (auto it = m_send_items.begin(); it != m_send_items.end(); ++it)
			{
				cmd_wheel += std::to_string(it->first);
				cmd_wheel += "|";
				cmd_wheel += std::to_string(it->second);
				cmd_wheel += "#";
			}
		}
		int luckyWheel = number(0, 9);
		if(luckyWheel == 0)
			if(number(0,1) == 0)
				luckyWheel = number(0, 9);
		ch->SetProtectTime("WheelLuckyIndex", luckyWheel);
		ch->SetProtectTime("WheelLuckyItemVnum", m_send_items[luckyWheel].first);
		ch->SetProtectTime("WheelLuckyItemCount", m_send_items[luckyWheel].second);
		ch->SetProtectTime("WheelWorking", 1);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "SetItemData %s", cmd_wheel.c_str());
		ch->ChatPacket(CHAT_TYPE_COMMAND, "OnSetWhell %d", luckyWheel);
	}
	else if (vecArgs[1] == "done")
	{
		if (ch->GetProtectTime("WheelWorking") == 0)
			return;
		ch->AutoGiveItem(ch->GetProtectTime("WheelLuckyItemVnum"), ch->GetProtectTime("WheelLuckyItemCount"));
		ch->ChatPacket(CHAT_TYPE_COMMAND, "GetGiftData %d %d", ch->GetProtectTime("WheelLuckyItemVnum"), ch->GetProtectTime("WheelLuckyItemCount"));
		ch->SetProtectTime("WheelLuckyIndex", 0);
		ch->SetProtectTime("WheelLuckyItemVnum", 0);
		ch->SetProtectTime("WheelLuckyItemCount", 0);
		ch->SetProtectTime("WheelWorking", 0);
	}
}

ACMD(do_wheel_of_fortune_gold)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 2) { return; }
	else if (vecArgs[1] == "start")
	{
		//ch->ChatPacket(CHAT_TYPE_INFO, "Kolo stesti je momentalne nedostupne.");
		//return;
		
		// Get player's composite HWID
		std::string playerHwid = ch->GetCompositeHWID();
		if (playerHwid.empty()) {
			ch->ChatPacket(CHAT_TYPE_INFO, "Nebylo mozne overit hwid. Zkus to prosim pozdeji.");
			sys_err("Wheel of Fortune: Player %s has empty composite HWID", ch->GetName());
			return;
		}

		// Check if this HWID has already won any limited costume (50166 or 50167)
		char escaped_hwid[513];
		DBManager::instance().EscapeString(escaped_hwid, sizeof(escaped_hwid), playerHwid.c_str(), playerHwid.length());
		
		std::unique_ptr<SQLMsg> pHwidMsg(DBManager::instance().DirectQuery(
			"SELECT COUNT(*) FROM wheel_fortune_winners WHERE (item_vnum = 50166 OR item_vnum = 50167) AND winner_hwid = '%s'", escaped_hwid));
		
		if (pHwidMsg->Get()->uiNumRows > 0) {
			MYSQL_ROW hwidRow = mysql_fetch_row(pHwidMsg->Get()->pSQLResult);
			int hwidCount = atoi(hwidRow[0]);
			
			if (hwidCount > 0) {
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10099]");
				return;
			}
		}

		// Check individual limits for both items
		std::unique_ptr<SQLMsg> pCountMsg(DBManager::instance().DirectQuery(
			"SELECT item_vnum, COUNT(*) as count FROM wheel_fortune_winners WHERE item_vnum IN (50166, 50167) GROUP BY item_vnum"));
		
		int count50166 = 0, count50167 = 0;
		if (pCountMsg->Get()->uiNumRows > 0) {
			MYSQL_RES* pRes = pCountMsg->Get()->pSQLResult;
			MYSQL_ROW row;
			while ((row = mysql_fetch_row(pRes))) {
				int itemVnum = atoi(row[0]);
				int itemCount = atoi(row[1]);
				if (itemVnum == 50166) count50166 = itemCount;
				else if (itemVnum == 50167) count50167 = itemCount;
			}
		}
		
		// Check if BOTH items have reached their limits (5 each) - WARN BUT ALLOW SPINNING
		bool bothMainRewardsAtLimit = (count50166 >= 5 && count50167 >= 5);
		if (bothMainRewardsAtLimit)
		{
			// Send warning message that main prizes are not available
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10100]"); // Warning: Main prizes not available, alternative rewards will be given
		}

		uint64_t goldCost = 300; // 100k yang cost
		if (ch->GetCheque() < goldCost)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10098;%d]", goldCost); 
			return;
		}
		else if (ch->GetProtectTime("WheelGoldWorking") != 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10092]"); // Wheel already in progress
			return;
		}
		ch->PointChange(POINT_CHEQUE, -goldCost, false);
		
		// Store whether both main rewards are at limit for later use in "done"
		ch->SetProtectTime("WheelGoldMainRewardsAtLimit", bothMainRewardsAtLimit ? 1 : 0);
		
		// Different rewards for gold wheel - more common items, less rare ones
		std::vector<std::pair<long, long>> m_important_item = {
			{80032,30},
			{203000,5},
			{80037,1},
		};
		
		std::map<std::pair<long, long>, int> m_normal_item;
		
		if (bothMainRewardsAtLimit) {
			// Alternative reward pool when main rewards are at limit
			m_normal_item = {
				{{203001,5},400}, // Increased chance
				{{203002,5},300}, // Increased chance
				{{203003,5},200}, // Increased chance
				{{10151,1},50},   // Increased chance
				{{10156,1},50},   // Increased chance
				{{10161,1},50},   // Increased chance
				{{61400,1},50},   // Increased chance
				{{79016,2},150},  // Increased chance
				{{79016,3},125},  // Increased chance
				{{79016,4},100},  // Increased chance
				{{100700,2},150}, // Increased chance
				{{100700,3},125}, // Increased chance
				{{100700,4},100}, // Increased chance
				{{80008,20000},150}, // Increased chance
				{{31150,35},600}, 
				{{31151,35},600}, 
				{{31152,35},600}, 
				{{31153,35},600}, 
				{{31154,35},600}, 
				{{31155,35},600}, 
				{{90029,3},100},  // Increased chance
				{{203034,1},50},  // Increased chance
				{{71180,1},10}    // Increased chance
			};
		} else {
			// Normal reward pool including main rewards
			m_normal_item = {
				{{50166,1},3},    // 0.1% chance
				{{50167,1},3},    // 0.1% chance
				{{203001,5},300}, // 60% chance
				{{203002,5},200}, // 60% chance
				{{203003,5},100}, // 60% chance
				{{10151,1},30},   // 0.1% chance
				{{10156,1},30},   // 0.1% chance
				{{10161,1},30},   // 0.1% chance
				{{61400,1},30},   // 0.1% chance
				{{79016,2},100},  // 0.2% chance
				{{79016,3},75},   // 0.2% chance
				{{79016,4},50},   // 0.2% chance
				{{100700,2},100}, // 0.2% chance
				{{100700,3},75},  // 0.2% chance
				{{100700,4},50},  // 0.2% chance
				{{80008,20000},100}, // 0.2% chance
				{{31150,35},600}, // 60% chance
				{{31151,35},600}, // 60% chance
				{{31152,35},600}, // 60% chance
				{{31153,35},600}, // 60% chance
				{{31154,35},600}, // 60% chance
				{{31155,35},600}, // 20% chance
				{{90029,3},75},   // 0.1% chance
				{{203034,1},30},  // 0.1% chance
				{{71180,1},3}     // 0.1% chance
			};
		}
		
		std::vector<std::pair<long, long>> m_send_items;
		if (m_important_item.size())
		{
			int random = number(0,m_important_item.size()-1);
			m_send_items.emplace_back(m_important_item[random].first, m_important_item[random].second);
		}
		while (true)
		{
			for (auto it = m_normal_item.begin(); it != m_normal_item.end(); ++it)
			{
				int randomEx = number(0,4);
				if (randomEx == 4)
				{
					int random = number(0,1000);
					if (it->second >= random)
					{
						auto itFind = std::find(m_send_items.begin(), m_send_items.end(), it->first);
						if (itFind == m_send_items.end())
						{
							m_send_items.emplace_back(it->first.first, it->first.second);
							if (m_send_items.size() >= 10)
								break;
						}
					}
				}
			}
			if (m_send_items.size() >= 10)
				break;
		}
		std::string cmd_wheel = "";
		if (m_send_items.size())
		{
			for (auto it = m_send_items.begin(); it != m_send_items.end(); ++it)
			{
				cmd_wheel += std::to_string(it->first);
				cmd_wheel += "|";
				cmd_wheel += std::to_string(it->second);
				cmd_wheel += "#";
			}
		}
		int luckyWheel = number(0, 9);
		if(luckyWheel == 0)
			if(number(0,1) == 0)
				luckyWheel = number(0, 9);
		ch->SetProtectTime("WheelGoldLuckyIndex", luckyWheel);
		ch->SetProtectTime("WheelGoldLuckyItemVnum", m_send_items[luckyWheel].first);
		ch->SetProtectTime("WheelGoldLuckyItemCount", m_send_items[luckyWheel].second);
		ch->SetProtectTime("WheelGoldWorking", 1);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "SetItemData %s", cmd_wheel.c_str());
		ch->ChatPacket(CHAT_TYPE_COMMAND, "OnSetWhell %d", luckyWheel);
	}
	else if (vecArgs[1] == "done")
	{
		if (ch->GetProtectTime("WheelGoldWorking") == 0)
			return;
		
		// Check if the won item is a limited costume (vnum 50166 or 50167)
		uint32_t wonItemVnum = ch->GetProtectTime("WheelGoldLuckyItemVnum");
		uint32_t wonItemCount = ch->GetProtectTime("WheelGoldLuckyItemCount");
		bool mainRewardsWereAtLimit = ch->GetProtectTime("WheelGoldMainRewardsAtLimit") == 1;
		
		if (wonItemVnum == 50166 || wonItemVnum == 50167)
		{
			// Get player's composite HWID first
			std::string playerHwid = ch->GetCompositeHWID();
			if (playerHwid.empty()) {
				ch->ChatPacket(CHAT_TYPE_INFO, "Nebylo mozne overit hwid. Zkus to prosim pozdeji.");
				sys_err("Wheel of Fortune Done: Player %s has empty composite HWID when winning item %u", ch->GetName(), wonItemVnum);
				return;
			}

			// Escape HWID for database query
			char escaped_hwid[513];
			DBManager::instance().EscapeString(escaped_hwid, sizeof(escaped_hwid), playerHwid.c_str(), playerHwid.length());
			
			// Double-check if this HWID has already won any limited costume (race condition protection)
			std::unique_ptr<SQLMsg> pDoubleCheckMsg(DBManager::instance().DirectQuery(
				"SELECT COUNT(*) FROM wheel_fortune_winners WHERE (item_vnum = 50166 OR item_vnum = 50167) AND winner_hwid = '%s'", escaped_hwid));
			
			if (pDoubleCheckMsg->Get()->uiNumRows > 0) {
				MYSQL_ROW doubleCheckRow = mysql_fetch_row(pDoubleCheckMsg->Get()->pSQLResult);
				int doubleCheckCount = atoi(doubleCheckRow[0]);
				
				if (doubleCheckCount > 0) {
					ch->ChatPacket(CHAT_TYPE_INFO, "Nebylo mozne overit hwid. Zkus to prosim pozdeji.");
					sys_err("Wheel of Fortune: HWID %s tried to win limited item %u twice for player %s", 
							playerHwid.substr(0, 20).c_str(), wonItemVnum, ch->GetName());
					return;
				}
			}

			// Check current counts for both items
			std::unique_ptr<SQLMsg> pCountMsg(DBManager::instance().DirectQuery(
				"SELECT item_vnum, COUNT(*) as count FROM wheel_fortune_winners WHERE item_vnum IN (50166, 50167) GROUP BY item_vnum"));
			
			int count50166 = 0, count50167 = 0;
			if (pCountMsg->Get()->uiNumRows > 0) {
				MYSQL_RES* pRes = pCountMsg->Get()->pSQLResult;
				MYSQL_ROW row;
				while ((row = mysql_fetch_row(pRes))) {
					int itemVnum = atoi(row[0]);
					int itemCount = atoi(row[1]);
					if (itemVnum == 50166) count50166 = itemCount;
					else if (itemVnum == 50167) count50167 = itemCount;
				}
			}
			
			// Check if the won item has already reached its limit OR if main rewards were at limit when spinning started
			bool itemAtLimit = false;
			if (mainRewardsWereAtLimit || (wonItemVnum == 50166 && count50166 >= 5) || (wonItemVnum == 50167 && count50167 >= 5)) {
				itemAtLimit = true;
			}
			
			if (itemAtLimit) {
				// Item is at limit, give alternative reward
				//ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10101;%u]", wonItemVnum);
				
				// Alternative rewards pool (excluding limited costumes)
				std::vector<std::pair<long, long>> alternative_rewards = {
					{80037, 1},     // Enhanced reward
					{79016, 4},     // Enhanced reward
					{100700, 4},    // Enhanced reward
					{203001, 10},   // Enhanced quantity
					{203002, 10},   // Enhanced quantity
					{203003, 10}    // Enhanced quantity
				};
				
				int altRandom = number(0, alternative_rewards.size() - 1);
				uint32_t altItemVnum = alternative_rewards[altRandom].first;
				uint32_t altItemCount = alternative_rewards[altRandom].second;
				
				ch->AutoGiveItem(altItemVnum, altItemCount);
				ch->ChatPacket(CHAT_TYPE_COMMAND, "GetGiftData %d %d", altItemVnum, altItemCount);
				//ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10102;%u;%u]", altItemVnum, altItemCount);
				
				// Log alternative reward
				sys_log(0, "Wheel of Fortune: Player %s (HWID: %s) got alternative reward %u x%u instead of limited item %u (main rewards at limit: %s)", 
						ch->GetName(), playerHwid.substr(0, 20).c_str(), altItemVnum, altItemCount, wonItemVnum, 
						mainRewardsWereAtLimit ? "true" : "false");
			}
			else {
				// Item is not at limit, proceed normally - LOG TO DATABASE
				DBManager::instance().DirectQuery(
					"INSERT INTO wheel_fortune_winners (item_vnum, item_count, winner_id, winner_name, winner_hwid, server_name) "
					"VALUES (%u, %u, %u, '%s', '%s', 'main')",
					wonItemVnum, wonItemCount, ch->GetPlayerID(), ch->GetName(), escaped_hwid);

				// Update counts after insertion
				if (wonItemVnum == 50166) count50166++;
				else if (wonItemVnum == 50167) count50167++;
				
				// Give the limited item
				ch->AutoGiveItem(wonItemVnum, wonItemCount);
				ch->ChatPacket(CHAT_TYPE_COMMAND, "GetGiftData %d %d", wonItemVnum, wonItemCount);
				
				int remaining50166 = 5 - count50166;
				int remaining50167 = 5 - count50167;
				
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10092]"); // Wheel already in progress
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10103;%d;%d]", remaining50166, remaining50167);

				// Log the HWID win for security tracking
				sys_log(0, "Wheel of Fortune: HWID %s (Player: %s, ID: %u) won limited costume %u. Counts: 50166: %d/5, 50167: %d/5", 
						playerHwid.substr(0, 20).c_str(), ch->GetName(), ch->GetPlayerID(), wonItemVnum, count50166, count50167);
			}
		}
		else {
			// Normal item, give as usual
			ch->AutoGiveItem(wonItemVnum, wonItemCount);
			ch->ChatPacket(CHAT_TYPE_COMMAND, "GetGiftData %d %d", wonItemVnum, wonItemCount);
		}
		
		ch->SetProtectTime("WheelGoldLuckyIndex", 0);
		ch->SetProtectTime("WheelGoldLuckyItemVnum", 0);
		ch->SetProtectTime("WheelGoldLuckyItemCount", 0);
		ch->SetProtectTime("WheelGoldWorking", 0);
		ch->SetProtectTime("WheelGoldMainRewardsAtLimit", 0); // Clear the flag
	}
}
#endif

#ifdef ENABLE_AFFECT_BUFF_REMOVE
ACMD(do_remove_buff)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (!ch)
		return;

	int affect = 0;
	str_to_number(affect, arg1);
	CAffect* pAffect = ch->FindAffect(affect);

	if (pAffect)
		ch->RemoveAffect(affect);
}
#endif

#ifdef __PENDANT__
ACMD(do_pendant)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 2) { return; }
	else if (vecArgs[1] == "set")
	{
		if (vecArgs.size() < 3) { return; }
		WORD uPageIdx;
		if (!str_to_number(uPageIdx, vecArgs[2].c_str()))
			return;
		else if (uPageIdx > 1)
			return;
		else if (ch->GetProtectTime("lastPendantClick") > time(0))
			return;
		else if (ch->GetQuestFlag("pendant.idx") == uPageIdx)
			return;
		ch->SetProtectTime("lastPendantClick", time(0) + 2);
		if (ch->GetQuestFlag("pendant.opened") == 0)
		{
			if (ch->CountSpecifyItem(51501) < 200)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "You don't has enought item");
				return;
			}
			ch->RemoveSpecifyItem(51501, 200);
			ch->SetQuestFlag("pendant.opened", 1);
		}
		ch->SetQuestFlag("pendant.idx", uPageIdx);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "SetPendantPageIdx %d %d", uPageIdx, 1);
		ch->ComputePoints();
	}
}
#endif

#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
#include "AchievementTitle.h"
ACMD(do_load_title_achievement) {
	CTitleAchievementTitle::instance().SendToClient(ch);
}
ACMD(do_change_title_achievement) {
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1) { return; }
	BYTE idx = 0;
	str_to_number(idx, arg1);
	CTitleAchievementTitle::instance().SelectTitle(idx, ch);
}
ACMD(do_change_title_achievement_premium)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	
	if (!*arg1)
		return;
	
	BYTE idx = 0;
	str_to_number(idx, arg1);
	
	CTitleAchievementTitle::instance().SelectTitlePremium(idx, ch);
}
#endif

ACMD(do_guild_dungeon)
{
	if (!ch->CanWarp() || quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID())->IsRunning())
	{
		sys_log(0, "do_guild_dungeon BLOCKED [%s]: CanWarp=%d IsRunning=%d", ch->GetName(), (int)ch->CanWarp(), (int)quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID())->IsRunning());
		ch->ChatPacket(CHAT_TYPE_INFO, "Cannot start dungeon: warp blocked or quest running.");
		return;
	}

	quest::PC* pPC = quest::CQuestManager::instance().GetPC(ch->GetPlayerID());
	if (!pPC)
	{
		sys_log(0, "do_guild_dungeon BLOCKED [%s]: no quest PC", ch->GetName());
		return;
	}

	CGuild* myGuild = ch->GetGuild();
	if (!myGuild)
	{
		sys_log(0, "do_guild_dungeon BLOCKED [%s]: not in a guild", ch->GetName());
		return;
	}

	char arg1[16];
	const char* rest = one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1)
		return;

	uint8_t actionIdx = 0;
	str_to_number(actionIdx, arg1);
	if (actionIdx < 1 || actionIdx > 5)
		return;

	if (actionIdx != 5)
	{
		TGuildMember* pMember = myGuild->GetMember(ch->GetPlayerID());
		if (!pMember)
		{
			sys_log(0, "do_guild_dungeon BLOCKED [%s]: not found in guild member list", ch->GetName());
			return;
		}
		//if (!myGuild->HasGradeAuth(pMember->grade, GUILD_AUTH_MANAGE_DUNGEON))
		//{
		//	sys_log(0, "do_guild_dungeon BLOCKED [%s]: grade %u lacks GUILD_AUTH_MANAGE_DUNGEON", ch->GetName(), pMember->grade);
		//	ch->ChatPacket(CHAT_TYPE_INFO, "You don't have permission to manage the guild dungeon.");
		//	return;
		//}

		//if (!CGuildManager::instance().CanFullManageGuildOnThisMap(ch))
		//	return;
	}

	/*
	 * 1 -> Dungeon Start
	 * 2 -> Buy Dungeon Ticket for Coupon
	 * 3 -> Buy Dungeon Ticket For SM
	 * 4 -> Tickets Refresh
	 * 5 -> Teleport Button
	 */

	if (actionIdx == 1)
	{
		char arg2[16];
		rest = one_argument(rest, arg2, sizeof(arg2));
		if (!*arg2)
			return;

		uint8_t dungeonIdx;
		str_to_number(dungeonIdx, arg2);

		SGuildDungeonInfo* dungData = CGuildManager::instance().GetDungeonInfoByIdx(dungeonIdx);
		if (!dungData)
		{
			sys_log(0, "do_guild_dungeon BLOCKED [%s]: dungeonIdx=%u not found", ch->GetName(), (uint32_t)dungeonIdx);
			ch->ChatPacket(CHAT_TYPE_INFO, "Invalid dungeon index.");
			return;
		}

		if (myGuild->GetLevel() < dungData->guildMinLevel)
		{
			sys_log(0, "do_guild_dungeon BLOCKED [%s]: guild level %u < minLevel %u", ch->GetName(), myGuild->GetLevel(), dungData->guildMinLevel);
			ch->ChatPacket(CHAT_TYPE_INFO, "Your guild level is too low for this dungeon.");
			return;
		}

		if (myGuild->GetDungeonTicketCount(GUILD_TICKETS_TYPE_FREE)+myGuild->GetDungeonTicketCount(GUILD_TICKETS_TYPE_PAID) <= 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "No dungeon tickets available.");
			return;
		}

		sys_log(0, "do_guild_dungeon [%s]: calling StartDungeon(%u) guild=%s", ch->GetName(), (uint32_t)dungeonIdx, myGuild->GetName());
		if (myGuild->StartDungeon(dungeonIdx))
		{
			sys_log(0, "do_guild_dungeon [%s]: StartDungeon SUCCESS guild=%s", ch->GetName(), myGuild->GetName());
			myGuild->GuildLog(16, ch->GetName(), ch->GetPlayerID(), NULL, dungeonIdx);
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Your guild already has an active raid dungeon. Close it first!");
		}
	}
	else if (actionIdx == 2)
	{
		if (myGuild->GetDungeonTicketCount(GUILD_TICKETS_TYPE_PAID_LIMIT) >= GUILD_TICKET_MAX_PAID_LIMIT)
			return;
		myGuild->RequestBuyTicket(ch, true);
	}
	else if (actionIdx == 3)
	{
		if (myGuild->GetDungeonTicketCount(GUILD_TICKETS_TYPE_PAID_LIMIT) >= GUILD_TICKET_MAX_PAID_LIMIT)
			return;
		myGuild->RequestBuyTicket(ch, false);
	}
	else if (actionIdx == 4)
	{
		if (get_global_time() <= myGuild->GetTicketsRefreshTime())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GILDIA_RAJD_INFO_1"));
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GILDIA_RAJD_INFO_2"));
			return;
		}

		myGuild->CheckRefreshTicketLimits();
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GILDIA_RAJD_ODNOWIENIE"));
	}
	else if (actionIdx == 5)
	{
		char arg2[16];
		rest = one_argument(rest, arg2, sizeof(arg2));
		if (!*arg2)
			return;

		uint8_t dungeonIdx;
		str_to_number(dungeonIdx, arg2);

		//if (myGuild->IsRecruit(ch->GetPlayerID()))
		//{
		//	if (test_server || ch->IsGM())
		//		ch->ChatPacket(CHAT_TYPE_INFO, "[GM_TEST_INFO] Recruit cant teleport to raid!", dungeonIdx);
		//	return;
		//}

		const SGuildDungeonInfo* dungInfo = CGuildManager::instance().GetDungeonInfoByIdx(dungeonIdx);
		if (!dungInfo)
		{
			if (test_server || ch->IsGM())
				ch->ChatPacket(CHAT_TYPE_INFO, "[GM_TEST_INFO] Dungeon index %u not exists!", dungeonIdx);
			return;
		}

		if (myGuild->GetDungeonIndex() == 0 || myGuild->GetDungeonIndex() != dungeonIdx)
		{
			if (test_server || ch->IsGM())
				ch->ChatPacket(CHAT_TYPE_INFO, "[GM_TEST_INFO] Dungeon index mismatch %u - %u!", myGuild->GetDungeonIndex(), dungeonIdx);
			return;
		}

		if (myGuild->GetDungeonMapIndex() < 10000)
		{
			if (test_server || ch->IsGM())
				ch->ChatPacket(CHAT_TYPE_INFO, "[GM_TEST_INFO] Dungeon map index is not dungeon %d!", myGuild->GetDungeonMapIndex());
			return;
		}

        // if (get_global_time() >= myGuild->GetDungeonStartTime() + dungInfo->maxTimeMin*60)
        // {
            // ch->ChatPacket(CHAT_TYPE_INFO, "Niestety czas na ukonczenie wyzwania minal!");
            // ch->ChatPacket(CHAT_TYPE_INFO, "Rajd zostal zamkniety!");
            // myGuild->EndDungeon(false);
            // return;
        // }

		sys_log(0, "GUILD_DUNGEON_WARP: player=%s guild=%s (id=%u) dungeonIdx=%u privateMapIdx=%d startX=%d startY=%d",
			ch->GetName(), myGuild->GetName(), myGuild->GetID(), dungeonIdx,
			myGuild->GetDungeonMapIndex(), dungInfo->startX, dungInfo->startY);
		ch->WarpSet(dungInfo->startX, dungInfo->startY, myGuild->GetDungeonMapIndex());
	}
}

ACMD(do_guild_guard_items)
{
	if (!ch->CanWarp() || quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID())->IsRunning())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10122]");
		return;
	}

	quest::PC* pPC = quest::CQuestManager::instance().GetPC(ch->GetPlayerID());
	if (!pPC)
		return;

	CGuild* myGuild = ch->GetGuild();
	if (!myGuild)
		return;

	char arg1[16];
	const char* rest = one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1)
		return;

	uint8_t actionIdx = 0;
	str_to_number(actionIdx, arg1);
	if (actionIdx < 0 || actionIdx > 3)
		return;

	if (!myGuild->HasGradeAuth(myGuild->GetMember(ch->GetPlayerID())->grade, GUILD_AUTH_MANAGE_GUARD))
		return;

	//if (!CGuildManager::instance().CanFullManageGuildOnThisMap(ch))
	//	return;

	/*
	 * 0 -> Deactivate set
	 * 1 -> Unequip item
	 * 2 -> Activate Set 1
	 * 3 -> Activate Set 2
	 */

	if (actionIdx == 0)
	{
		if (myGuild->GuardDeactivateItemsSet())
		{
			if (test_server)
				ch->ChatPacket(CHAT_TYPE_INFO, "[TEST_SERVER] On test server cooldown is 5 seconds, normally it is 180 seconds.");

			myGuild->GuildLog(4, ch->GetName(), ch->GetPlayerID());
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Guardian bonus change is on cooldown. Please wait.");
		}
	}
	else if (actionIdx == 1)
	{
		char arg2[16];
		char arg3[16];
		rest = two_arguments(rest, arg2, sizeof(arg2), arg3, sizeof(arg3));
		if (!*arg2 || !*arg3)
			return;

		uint8_t setID = 0;
		str_to_number(setID, arg2);
		if (setID != 0 && setID != 1)
			return;

		uint8_t slotIdx = 0;
		str_to_number(slotIdx, arg3);
		if (slotIdx < 0 || slotIdx > 8)
			return;

		myGuild->UnequipItem(ch, setID, slotIdx);
	}
	else if (actionIdx == 2 || actionIdx == 3)
	{
		uint8_t setID = (actionIdx == 2) ? 0 : 1;

		if (myGuild->GetGuildMoney() < GUILD_GUARD_ACTIVATE_FEE)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Not enough guild gold to activate guardian bonuses. Required: %d.", GUILD_GUARD_ACTIVATE_FEE);
			return;
		}

		if (myGuild->GuardActivateItemsSet(setID))
		{
			myGuild->ChangeMoney(-GUILD_GUARD_ACTIVATE_FEE);

			if (test_server)
				ch->ChatPacket(CHAT_TYPE_INFO, "[TEST_SERVER] On test server cooldown is 5 seconds, normally it is 180 seconds. Bonuses last 24h.");

			myGuild->GuildLog(5, ch->GetName(), ch->GetPlayerID());
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Guardian bonus change is on cooldown. Please wait.");
		}
	}
}

// ============================================================
//  Guild Item Donation System
//  Client sends: /guild_donate_material <vnum> <count>
//  Guild leader view:  /guild_view_storage
// ============================================================

static const DWORD GUILD_DONATE_STONE_VNUM   = 90010;
static const DWORD GUILD_DONATE_LOG_VNUM     = 90011;
static const DWORD GUILD_DONATE_PLYWOOD_VNUM = 90012;
static const int   GUILD_DONATE_DAILY_LIMIT  = 15;

ACMD(do_guild_donate_material)
{
	if (!ch) return;

	CGuild* myGuild = ch->GetGuild();
	if (!myGuild)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You must be in a guild to donate materials.");
		return;
	}

	char arg1[16], arg2[16];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	if (!*arg1 || !*arg2)
		return;

	DWORD vnum = 0;
	str_to_number(vnum, arg1);
	int count = 0;
	str_to_number(count, arg2);

	if (count <= 0)
		return;

	if (vnum != GUILD_DONATE_STONE_VNUM && vnum != GUILD_DONATE_LOG_VNUM && vnum != GUILD_DONATE_PLYWOOD_VNUM)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "This item cannot be donated to the guild.");
		return;
	}

	// --- Per-item daily limit (separate counter per vnum, shared day flag) ---
	// cntFlag: "gd_s_cnt" / "gd_l_cnt" / "gd_p_cnt"
	const char* cntFlag =
		(vnum == GUILD_DONATE_STONE_VNUM)   ? "guild_donation.gd_s_cnt" :
		(vnum == GUILD_DONATE_LOG_VNUM)     ? "guild_donation.gd_l_cnt" :
		                                      "guild_donation.gd_p_cnt";

	int today   = (int)(get_global_time() / 86400);
	int lastDay = ch->GetQuestFlag("guild_donation.gd_day");

	if (lastDay != today)
	{
		// New day: reset all three counters at once
		ch->SetQuestFlag("guild_donation.gd_day",   today);
		ch->SetQuestFlag("guild_donation.gd_s_cnt", 0);
		ch->SetQuestFlag("guild_donation.gd_l_cnt", 0);
		ch->SetQuestFlag("guild_donation.gd_p_cnt", 0);
	}

	int donated = ch->GetQuestFlag(cntFlag);

	if (donated >= GUILD_DONATE_DAILY_LIMIT)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[Guild Donation] Daily limit of %d for this item reached. Resets at midnight.", GUILD_DONATE_DAILY_LIMIT);
		return;
	}

	int remaining = GUILD_DONATE_DAILY_LIMIT - donated;
	if (count > remaining)
		count = remaining;

	int hasCount = (int)ch->CountSpecifyItem(vnum);
	if (hasCount < count)
		count = hasCount;

	if (count <= 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[Guild Donation] You don't have any of this item.");
		return;
	}

	// Remove from player
	ch->RemoveSpecifyItem(vnum, count);

	// Update guild storage in memory and persist to DB
	if (vnum == GUILD_DONATE_STONE_VNUM)
	{
		myGuild->AddDonateStone(count);
		DBManager::instance().Query("UPDATE guild%s SET donate_stone=%d WHERE id=%u",
			get_table_postfix(), myGuild->GetDonateStone(), myGuild->GetID());
	}
	else if (vnum == GUILD_DONATE_LOG_VNUM)
	{
		myGuild->AddDonateLog(count);
		DBManager::instance().Query("UPDATE guild%s SET donate_log=%d WHERE id=%u",
			get_table_postfix(), myGuild->GetDonateLog(), myGuild->GetID());
	}
	else
	{
		myGuild->AddDonatePlywood(count);
		DBManager::instance().Query("UPDATE guild%s SET donate_plywood=%d WHERE id=%u",
			get_table_postfix(), myGuild->GetDonatePlywood(), myGuild->GetID());
	}

	// Update per-item daily counter
	ch->SetQuestFlag(cntFlag, donated + count);

	// Sync to other game cores
	myGuild->BroadcastDonateMaterials();

	// Broadcast updated storage to all online guild members (updates their UI)
	myGuild->SendStorageInfo(NULL);

	ch->ChatPacket(CHAT_TYPE_INFO, "[Guild Donation] Donated %d item(s). Daily remaining: %d.",
		count, GUILD_DONATE_DAILY_LIMIT - donated - count);
}

ACMD(do_guild_view_storage)
{
	if (!ch) return;

	CGuild* myGuild = ch->GetGuild();
	if (!myGuild)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You must be in a guild.");
		return;
	}

	myGuild->SendStorageInfo(ch);
}

ACMD(do_guild_upgrade_bonus)
{
	if (!ch) return;

	CGuild* myGuild = ch->GetGuild();
	if (!myGuild)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You must be in a guild.");
		return;
	}

	// Only guild master (grade 1)
	const TGuildMember* pMember = myGuild->GetMember(ch->GetPlayerID());
	if (!pMember || pMember->grade != 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[Guild Upgrade] Only the guild leader can upgrade bonuses.");
		return;
	}

	char arg1[8];
	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1) return;

	int idx = 0;
	str_to_number(idx, arg1);
	if (idx < 0 || idx >= 5)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[Guild Upgrade] Invalid bonus index.");
		return;
	}

	uint8_t curLevel = myGuild->GetBonusSkillLevel(idx);
	if (curLevel >= GUILD_BONUS_MAX_LEVEL)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[Guild Upgrade] Bonus already at maximum level %d.", GUILD_BONUS_MAX_LEVEL);
		return;
	}

	int cost = GUILD_BONUS_UPGRADE_COST;
	if (myGuild->GetDonateStone() < cost || myGuild->GetDonateLog() < cost || myGuild->GetDonatePlywood() < cost)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[Guild Upgrade] Not enough materials. Need %d of each (Stone/Log/Plywood).", cost);
		ch->ChatPacket(CHAT_TYPE_INFO, "[Guild Upgrade] Have: Stone %d | Log %d | Plywood %d",
			myGuild->GetDonateStone(), myGuild->GetDonateLog(), myGuild->GetDonatePlywood());
		return;
	}

	// Deduct materials and persist to DB
	myGuild->AddDonateStone(-cost);
	myGuild->AddDonateLog(-cost);
	myGuild->AddDonatePlywood(-cost);

	DBManager::instance().Query(
		"UPDATE guild%s SET donate_stone=%d, donate_log=%d, donate_plywood=%d WHERE id=%u",
		get_table_postfix(),
		myGuild->GetDonateStone(), myGuild->GetDonateLog(), myGuild->GetDonatePlywood(),
		myGuild->GetID());

	// Sync material changes to other game cores
	myGuild->BroadcastDonateMaterials();

	// Upgrade the skill level (saves abySkill via SaveSkill(), broadcasts via SendDBSkillUpdate + SendSkillInfoPacket)
	myGuild->UpgradeBonusSkill((uint8_t)idx);

	// Broadcast updated storage (donate counts + new skill levels) to all members
	myGuild->SendStorageInfo(NULL);

	ch->ChatPacket(CHAT_TYPE_INFO, "[Guild Upgrade] Bonus #%d upgraded to level %d!", idx, curLevel + 1);
}

ACMD(do_world_boss_request)
{
	if (!ch) return;
	
	sys_log(0, "Player %s requested world boss data", ch->GetName());
	
	MonsterSpawner::instance().SendPacket(ch, -1);
}

ACMD(do_world_boss_teleport)
{
    if (!ch)
        return;

    char arg1[256];
    one_argument(argument, arg1, sizeof(arg1));

    if (!*arg1)
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "Usage: /world_boss_teleport <boss_vnum>");
        return;
    }

    uint32_t boss_vnum = atoi(arg1);
    
    // Get the boss entry from MonsterSpawner
    auto& spawner = MonsterSpawner::instance();
    
    // Temporary: Send through MonsterSpawner
    if (!spawner.TeleportToBoss(ch, boss_vnum))
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "Cannot teleport to this boss.");
    }
}

#ifdef ENABLE_ITEMSHOP
ACMD(do_ishop)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);

	if (vecArgs.size() < 2)
	{
		return;
	}
	else if (vecArgs[1] == "data")
	{
		if (ch->GetProtectTime("itemshop.load") == 1)
		{
			return;
		}

		ch->SetProtectTime("itemshop.load", 1);

		if (vecArgs.size() < 3)
		{
			return;
		}

		int updateTime;
		str_to_number(updateTime, vecArgs[2].c_str());

		CHARACTER_MANAGER::Instance().LoadItemShopData(ch, CHARACTER_MANAGER::Instance().GetItemShopUpdateTime() != updateTime);
	}
	else if (vecArgs[1] == "log")
	{
		if (ch->GetProtectTime("itemshop.log") == 1)
		{
			return;
		}

		ch->SetProtectTime("itemshop.log", 1);

		CHARACTER_MANAGER::Instance().LoadItemShopLog(ch);
	}
	else if (vecArgs[1] == "buy")
	{
		if (vecArgs.size() < 4)
		{
			return;
		}

		int itemID;
		str_to_number(itemID, vecArgs[2].c_str());

		int itemCount;
		str_to_number(itemCount, vecArgs[3].c_str());

/* check already in: LoadItemShopBuy
		if (itemCount < 1 || itemCount > 20)
		{
			return;
		}
*/
		CHARACTER_MANAGER::Instance().LoadItemShopBuy(ch, itemID, itemCount);
	}
	else if (vecArgs[1] == "wheel")
	{
		if (vecArgs.size() < 3)
		{
			return;
		}
		else if (vecArgs[2] == "start")
		{
			if (vecArgs.size() < 4)
			{
				return;
			}
		
			BYTE ticketType;
			if (!str_to_number(ticketType, vecArgs[3].c_str()))
			{
				return;
			}
		
			if (ticketType > 2)
			{
				return;
			}
			else if (ch->GetProtectTime("WheelWorking") != 0)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "998");
				return;
			}
		
			// Define reward tables
			std::map<std::pair<long, long>, int> m_ultra_rare_item;
			std::map<std::pair<long, long>, int> m_rare_item;
			std::map<std::pair<long, long>, int> m_normal_item;
		
			if (ticketType == 0) // Christmas Ticket Mode
			{
				// Check for Christmas ticket
				if (ch->CountSpecifyItem(201230) <= 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "You dont have required item to spin easter wheel");
					return;
				}
		
				ch->RemoveSpecifyItem(201230, 1);
		
				// Christmas rare items (5% chance)
				m_rare_item = {
					{{100700,2}, 2},
					{{23000,1}, 2},
					{{90029,2}, 2},
					{{34000,2}, 2},
					{{91232,1}, 2},
					{{91233,1}, 2},
					{{91234,1}, 2},
					{{91235,1}, 2},
				};
		
				// Christmas normal items (30% chance)
				m_normal_item = {
					{{80460,5}, 30},
					{{80450,5}, 30},
					{{80458,5}, 30},
					{{30502,5}, 30},
					{{30505,5}, 10},
					{{85004,1}, 30},
					{{79016,2}, 30},
					{{31150,5}, 30},
					{{31151,5}, 30},
					{{31152,5}, 30},
					{{31153,5}, 30},
					{{31154,5}, 30},
					{{31155,5}, 30},
					{{203032,5}, 30},
				};
			}
			else if (ticketType == 1) // Coin Mode
			{
				long long act_lldCoins;
		#ifdef USE_ITEMSHOP_RENEWED
				long long act_lldJCoins;
				ch->GetAccountMoney(act_lldCoins, act_lldJCoins);
		#else
				ch->GetAccountMoney(act_lldCoins);
		#endif
		
				if (act_lldCoins < 120)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "997");
					return;
				}
		
		#ifdef USE_ITEMSHOP_RENEWED
				ch->SetAccountMoney(120, 0, false);
				ch->GetAccountMoney(act_lldCoins, act_lldJCoins);
		#else
				ch->SetAccountMoney(120, false);
				ch->GetAccountMoney(act_lldCoins);
		#endif
		
				LPDESC d = ch->GetDesc();
				if (d)
				{
					LogManager::Instance().WheelOfFortuneLog(d->GetAccountTable().id, ch->GetPlayerID(), ch->GetMapIndex(), ch->GetX(), ch->GetY(), 10);
		
					if (ch->GetProtectTime("itemshop.log") == 1)
					{
						char szIP[16] = {};
						strlcpy(szIP, d->GetHostName(), sizeof(szIP));
		
						char szTime[21];
						time_t now = time(0);
						struct tm tstruct = *localtime(&now);
						strftime(szTime, sizeof(szTime), "%Y-%m-%d %X", &tstruct);
		
						ch->ChatPacket(CHAT_TYPE_COMMAND, "ItemShopAppendLog %s %d %s %s 1 1 5", szTime, time(0), ch->GetName(), szIP);
					}
				}
		
				// Default rare items (5% chance)
				m_rare_item = {
					{{50166,1}, 1},
					{{50167,1}, 1},
					{{80040,1}, 3},
					{{10161,1}, 3},
					{{10156,1}, 3},
					{{10151,1}, 3},
					{{71180,1}, 2},
					{{56004,1}, 1},
					{{61400,1}, 3},
				};
		
				// Default normal items (30% chance)
				m_normal_item = {
					{{203000,3}, 30},
					{{203001,1}, 30},
					{{203002,1}, 30},
					{{203003,1}, 30},
					{{80032,3}, 10},
					{{79016,2}, 30},
					{{79016,3}, 30},
					{{79016,4}, 30},
					{{100700,2}, 30},
					{{100700,3}, 30},
					{{100700,4}, 30},
					{{31150,3}, 30},
					{{31151,3}, 30},
					{{31152,3}, 30},
					{{31153,3}, 30},
					{{31154,3}, 30},
					{{31155,3}, 30},
					{{90029,1}, 30},
					{{203034,1}, 7},
				};
			}
#ifdef ENABLE_CHEQUE_SYSTEM
			else if (ticketType == 2) // Cheque Mode
			{
				const int64_t CHEQUE_WHEEL_COST = 150;

				if (ch->GetCheque() < CHEQUE_WHEEL_COST)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "997");
					return;
				}

				ch->PointChange(POINT_CHEQUE, -CHEQUE_WHEEL_COST);

				LPDESC d = ch->GetDesc();
				if (d)
				{
					LogManager::Instance().WheelOfFortuneLog(d->GetAccountTable().id, ch->GetPlayerID(), ch->GetMapIndex(), ch->GetX(), ch->GetY(), 2);
				}

				// Cheque rare items
				m_rare_item = {
					{{91236,1}, 2},
					{{91237,1}, 2},
					{{91238,1}, 2},
					{{91239,1}, 2},
					{{91240,1}, 2},
					{{91241,1}, 2},
					{{91242,1}, 2},
					{{91243,1}, 2},
					{{203034,1}, 1},
					{{71180,1}, 1},
					{{203012,1}, 1},
				};

				// Cheque ultra rare items (roll 0-1000, ~0.3% per pass each)
				m_ultra_rare_item = {
					{{91244,1}, 7},
					{{52959,1}, 7},
					{{53947,1}, 7},
				};

				// Cheque normal items
				m_normal_item = {
					{{201000,100}, 50},
					{{115100,1}, 30},
					{{125100,1}, 30},
					{{135100,1}, 30},
					{{145100,1}, 30},
					{{155100,1}, 30},
					{{165100,1}, 30},
					{{100700,2}, 30},
					{{100700,3}, 30},
					{{79016,5}, 30},
					{{79016,7}, 30},
					{{79016,10}, 30},
					{{71052,10}, 30},
					{{71052,20}, 30},
					{{79012,10}, 30},
					{{79012,20}, 30},
				};
			}
#endif

			std::vector<std::pair<long, long>> m_send_items;
		
			while (m_send_items.size() < 10)
			{
				// Ultra rare items (1/1000 scale)
				for (auto it = m_ultra_rare_item.begin(); it != m_ultra_rare_item.end() && m_send_items.size() < 10; ++it)
				{
					int random = number(0, 1000);
					if (it->second >= random)
					{
						auto itFind = std::find(m_send_items.begin(), m_send_items.end(), it->first);
						if (itFind == m_send_items.end())
						{
							m_send_items.emplace_back(it->first.first, it->first.second);
						}
					}
				}

				// First try rare items
				for (auto it = m_rare_item.begin(); it != m_rare_item.end() && m_send_items.size() < 10; ++it)
				{
					int random = number(0, 100);
					if (it->second >= random)
					{
						auto itFind = std::find(m_send_items.begin(), m_send_items.end(), it->first);
						if (itFind == m_send_items.end())
						{
							m_send_items.emplace_back(it->first.first, it->first.second);
						}
					}
				}
		
				// Then fill with normal items
				for (auto it = m_normal_item.begin(); it != m_normal_item.end() && m_send_items.size() < 10; ++it)
				{
					int random = number(0, 100);
					if (it->second >= random)
					{
						auto itFind = std::find(m_send_items.begin(), m_send_items.end(), it->first);
						if (itFind == m_send_items.end())
						{
							m_send_items.emplace_back(it->first.first, it->first.second);
						}
					}
				}
			}
		
			std::string cmd_wheel = "";
		
			if (!m_send_items.empty())
			{
				for (auto it = m_send_items.begin(); it != m_send_items.end(); ++it)
				{
					cmd_wheel += std::to_string(it->first);
					cmd_wheel += "|";
					cmd_wheel += std::to_string(it->second);
					cmd_wheel += "#";
				}
			}
		
			int luckyWheel = number(0, 9);
			{
				if (luckyWheel == 0)
				{
					if (number(0, 1) == 0)
					{
						luckyWheel = number(0, 9);
					}
				}
			}
		
			ch->SetProtectTime("WheelLuckyIndex", luckyWheel);
			ch->SetProtectTime("WheelLuckyItemVnum", m_send_items[luckyWheel].first);
			ch->SetProtectTime("WheelLuckyItemCount", m_send_items[luckyWheel].second);
			ch->SetProtectTime("WheelWorking", 1);
		
			ch->ChatPacket(CHAT_TYPE_COMMAND, "SetWheelItemData %s", cmd_wheel.c_str());
			ch->ChatPacket(CHAT_TYPE_COMMAND, "OnSetWhell %d", luckyWheel);
		}
		else if (vecArgs[2] == "done")
		{
			if (ch->GetProtectTime("WheelWorking") == 0)
			{
				return;
			}

			ch->AutoGiveItem(ch->GetProtectTime("WheelLuckyItemVnum"), ch->GetProtectTime("WheelLuckyItemCount"));

			ch->ChatPacket(CHAT_TYPE_COMMAND, "GetWheelGiftData %d %d", ch->GetProtectTime("WheelLuckyItemVnum"), ch->GetProtectTime("WheelLuckyItemCount"));

			ch->SetProtectTime("WheelLuckyIndex", 0);
			ch->SetProtectTime("WheelLuckyItemVnum", 0);
			ch->SetProtectTime("WheelLuckyItemCount", 0);
			ch->SetProtectTime("WheelWorking", 0);
		}
	}
	else if (vecArgs[1] == "currency")
	{
		ch->RefreshAccountMoney();
	}
}
#endif

#ifdef __DUNGEON_INFO__
ACMD(do_dungeon_info)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 2) { return; }
	else if (vecArgs[1] == "rank")
	{
		if (vecArgs.size() < 4) { return; }
		DWORD mobIdx, rankIdx;
		if (!str_to_number(mobIdx, vecArgs[2].c_str()) || !str_to_number(rankIdx, vecArgs[3].c_str()))
			return;
		CHARACTER_MANAGER::Instance().SendDungeonRank(ch, mobIdx, rankIdx);
	}
	else if (vecArgs[1] == "test_cooldown" && ch->IsGM())
	{
		ch->SetQuestFlag("jotun.cooldown", time(0) + 90);
		ch->SetQuestFlag("orc_camp_dungeon.cooldown", time(0) + 30);
		ch->SetQuestFlag("deviltower.cooldown", time(0) + 90);
		ch->SetQuestFlag("sanctum.cooldown", time(0) + 90);
		ch->SetQuestFlag("hwang.cooldown", time(0) + 60);
		ch->SetQuestFlag("beran.cooldown", time(0) + 30);
		ch->SetQuestFlag("underwater.cooldown", time(0) + 60);
		ch->SetQuestFlag("spider.cooldown", time(0) + 60);
		ch->SetQuestFlag("dragonlair.cooldown", time(0) + 60);
		ch->SetQuestFlag("nightmare.cooldown", time(0) + 60);
		ch->SetQuestFlag("razador.cooldown", time(0) + 60);
		ch->SetQuestFlag("duratus.cooldown", time(0) + 90);
		ch->SetQuestFlag("alastoreasy.cooldown", time(0) + 90);
		ch->SetQuestFlag("alastorhard.cooldown", time(0) + 90);
		ch->SetQuestFlag("serpent.cooldown", time(0) + 90);
		ch->SetQuestFlag("zodiactemple.cooldown", time(0) + 90);
		ch->SetQuestFlag("rxdragonlair.cooldown", time(0) + 240);

		ch->SendDungeonCooldown(0);
	}
	else if (vecArgs[1] == "update" && ch->IsGM())
		ch->SendDungeonCooldown(0);
	else if (vecArgs[1] == "cooldown")
		ch->SendDungeonCooldown(0);
	else if (vecArgs[1] == "reset_fastest" && ch->IsGM())
	{
		CHARACTER_MANAGER::Instance().ResetFastestRankings();
		ch->ChatPacket(CHAT_TYPE_INFO, "Fastest rankings reset. Next auto-reset at midnight.");
	}
}
#endif

ACMD(do_enable_mission_notifications)
{
	if (!ch)
		return;

	int current = ch->GetQuestFlag("collector.notifications");
	int toggled = current ? 0 : 1;
	ch->SetQuestFlag("collector.notifications", toggled);
	ch->ChatPacket(CHAT_TYPE_COMMAND, "SetMissionNotifications %d", toggled);
}

#ifdef ENABLE_VOTE4BUFF
ACMD(do_vote4buff)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
		return;

	const std::string arg(arg1);

	if (arg == "vote")
	{
		int32_t time_left = 0;
		if (!*arg2 || !str_to_number(time_left, arg2))
			return;

		ch->Vote4Buff_Vote(MAX(0, time_left), true);
	}
	else if (arg == "bonus")
	{
		int index = 0;
		if (!*arg2 || !str_to_number(index, arg2))
			return;

		ch->Vote4Buff_SelectBonus(index);
	}
	else if (arg == "reset" && test_server)
	{
		ch->Vote4Buff_Reset();
	}
}
#endif

ACMD(do_open_shop)
{
	static const std::set<DWORD> allowedShopVnums = {
		16,
		17,
	};

	static const std::set<DWORD> allowedNPCVnums = {
		20996,
	};

	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 2) { return; }

	DWORD dwVnum = 0;
	str_to_number(dwVnum, vecArgs[1].c_str());

	// Check if it's an allowed shop vnum (regular shops)
	if (allowedShopVnums.find(dwVnum) != allowedShopVnums.end())
	{
		CShopManager::Instance().StartShopping(ch, NULL, dwVnum);
		return;
	}

	// Check if it's an allowed NPC vnum (shopex)
	if (allowedNPCVnums.find(dwVnum) != allowedNPCVnums.end())
	{
		LPSHOP pkShop = CShopManager::Instance().GetByNPCVnum(dwVnum);
		if (!pkShop)
		{
			sys_log(0, "SHOP: NO SHOPEX FOR NPC %u", dwVnum);
			return;
		}
		pkShop->AddGuest(ch, 0, false);
		return;
	}

	sys_log(0, "SHOP: UNAUTHORIZED OPEN ATTEMPT: %s tried vnum %u", ch->GetName(), dwVnum);
}

#ifdef __VIP_SYSTEM__
ACMD(do_convert_vip_affect)
{
	auto pAffect = ch->FindAffect(EAffectTypes::AFFECT_VIP);
	if (nullptr == pAffect)
		return;

	const auto leftDuration = pAffect->lDuration;
	if (leftDuration <= 0)
		return;

	// Promoter premiums (lApplyValue == 1) must be returned as the untradable
	// ITEM_VIP_UNIQUE_PROMO so they cannot be traded/given away after unequipping.
	const DWORD dwReturnVnum = (pAffect->lApplyValue != 0) ? ITEM_VIP_UNIQUE_PROMO : ITEM_VIP_UNIQUE;

	if (auto item = ch->AutoGiveItem(dwReturnVnum); item)
	{
		if (ch->RemoveVIP())
			item->SetSocket(0, leftDuration);
		else
			M2_DESTROY_ITEM(item);
	}
}	
#endif

#ifdef ENABLE_AUTO_SELECT_SKILL
ACMD(do_skillauto)
{
    std::vector<std::string> vecArgs;
    split_argument(argument, vecArgs);
    if (vecArgs.size() < 2) { return; }

    if (vecArgs[1] == "select")
    {
        if (vecArgs.size() < 3) { return; }
        if (ch->GetSkillGroup() != 0)
            return;

        BYTE skillIndex;
        str_to_number(skillIndex, vecArgs[2].c_str());
        if (skillIndex < 1 || skillIndex > 2)
            return;

        // Skill table: [job][group] = { skill ids }
        static const DWORD skillTable[4][3][6] = {
            // job 0 (Warrior)
            { {}, {1,  2,  3,  4,  5,  0}, {16, 17, 18, 19, 20,  0} },
            // job 1 (Assassin)
            { {}, {31, 32, 33, 34, 35,  0}, {46, 47, 48, 49, 50,  0} },
            // job 2 (Sura)
            { {}, {61, 62, 63, 64, 65, 66}, {76, 77, 78, 79, 80, 81} },
            // job 3 (Shaman)
            { {}, {91, 92, 93, 94, 95, 96}, {106,107,108,109,110,111} },
        };

        BYTE job = ch->GetJob();
        if (job > 3)
            return;

        ch->RemoveGoodAffect();
        ch->SetSkillGroup(skillIndex);
        ch->ClearSkill();

        // Set skills from table to level 40
        const DWORD* skills = skillTable[job][skillIndex];
        for (int i = 0; i < 6; ++i)
        {
            if (skills[i] == 0)
                break;
            ch->SetSkillLevel(skills[i], 40);
        }

        // Fixed skills
        ch->SetSkillLevel(131, 10);
        ch->SetSkillLevel(137, 40);
        ch->SkillLevelPacket();
    }
}
#endif

#ifdef _ENABLE_BATTLEPASS_
const char* battlepass[] = {
	"open",
	"premium",

	"\n",
};

ACMD(do_battle_pass)
{
	char arg1[256], arg2[256];

	int i, len;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: battle_pass <field> <value>");
		return;
	}

	len = strlen(arg1);

	for (i = 0; *(battlepass[i]) != '\n'; i++)
		if (!strncmp(arg1, battlepass[i], len))
			break;

	switch (i)
	{
	case 0:
		{
			int value = 0;
			str_to_number(value, arg2);
			if (ch->IsBPOpen())
				return;

			ch->SetBPOpen(true);
			ch->SendBPState(BATTLEPASS_GC_SUB_INIT);
		}
		break;

	case 1:
		{
			ch->UpgradeToPremium();
		}
	}
}
const char* battlepassadmin[] = {
	"complete",
	"get",
	"reset",

	"\n",
};

ACMD(do_battle_pass_admin)
{
	char arg1[256], arg2[256];

	int i, len;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: bp_fix_missions <field> <value>");
		return;
	}

	len = strlen(arg1);

	for (i = 0; *(battlepassadmin[i]) != '\n'; i++)
		if (!strncmp(arg1, battlepassadmin[i], len))
			break;

	switch (i)
	{
	case 0:
		{
			int mission = 0;
			str_to_number(mission, arg2);
			ch->ForceComplete(static_cast<uint16_t>(mission));
		}
		break;

	case 1:
		{
			for (int _g = 0; _g < 3; ++_g)
			{
				for (const auto& slot : ch->GetBPGroups()[_g])
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "DefID: %d, Track: %d, Progress: %d", slot.defId, slot.track, slot.progress);
				}
			}
		}
		break;
	case 2:
		{
			ch->ResetDailyWeekly();
		}
	}
}

#endif
#ifdef __AUTO_SKILL_READER__
ACMD(do_auto_skill_reader)
{
	char szArg1[256], szArg2[256];
	two_arguments(argument, szArg1, sizeof(szArg1), szArg2, sizeof(szArg2));

	if (!*szArg1 || !*szArg2)
	{
		return;
	}

	BYTE bSkillIndex;
	str_to_number(bSkillIndex, szArg1);

	BYTE bStatus;
	str_to_number(bStatus, szArg2);

	ch->GetAutoSkill(bSkillIndex, bStatus == 0 ? false : true);
}
#endif

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
#include "locale_item_manager.h"
ACMD(do_language)
{
	char szLang[8];
	one_argument(argument, szLang, sizeof(szLang));

	if (!*szLang)
		return;

	// Sanitize: accept only short lowercase alpha codes like "en", "cz", "pl"
	for (int i = 0; szLang[i]; ++i)
	{
		if (!islower((unsigned char)szLang[i]))
			return;
	}

	ch->SetLanguage(szLang);
}
#endif

#ifdef ENABLE_MULTI_FARM_BLOCK
ACMD(do_multi_farm)
{
	if (!ch->GetDesc())
		return;
	if (ch->GetProtectTime("multi-farm") > get_global_time())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You need be slow! You can try after %d second.", ch->GetProtectTime("multi-farm") - get_global_time());
		return;
	}
	ch->SetProtectTime("multi-farm", get_global_time() + 10);
	CHARACTER_MANAGER::Instance().CheckMultiFarmAccount(ch->GetCompositeHWID().c_str(), ch->GetPlayerID(), ch->GetName(), !ch->GetRewardStatus());
}
#endif