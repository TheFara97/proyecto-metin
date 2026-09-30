#include "stdafx.h"
#include "mining.h"
#include "char.h"
#include "char_manager.h"
#include "item_manager.h"
#include "item.h"
#include "config.h"
#include "db.h"
#include "log.h"
#include "skill.h"
#ifdef RANKING_SYSTEM
#include "server_ranking_manager.h"
#include "ranking_manager.h"
#endif
#ifdef BATTLE_PASS
#include "CBattlePass.h"
#endif
#ifdef __ENABLE_MINING_EVENT__
	#include "MiningEvent.h"
#endif
#define MINING_EVENT_CHEST_VNUM 203010

namespace mining
{
	enum
	{
		MAX_ORE = 9,
		ORE_COUNT_FOR_REFINE = 100,
		PICK_MAX_LEVEL = 15,
		EXTRA_ITEMS_COUNT = 3,
	};

	struct SInfo
	{
		DWORD dwLoadVnum;
		DWORD dwRawOreVnum;
		DWORD dwRefineVnum;
	};

	SInfo info[MAX_ORE] =
	{
		{ 20047, 50601, 50621 },
		{ 20048, 50602, 50639 },
		{ 20049, 50603, 50640 },
		{ 20050, 50604, 50641 },
		{ 20051, 50605, 50642 },
		{ 20052, 50606, 50643 },
		{ 20053, 50607, 50644 },
		{ 20054, 50646, 50647 },
		{ 20059, 50648, 50649 },
		// { 20056, 50610, 50630 },
		// { 20057, 50611, 50631 },
		// { 20058, 50612, 50632 },
		// { 20059, 50613, 50633 },
		// { 30301, 50614, 50634 },
		// { 30302, 50615, 50635 },
		// { 30303, 50616, 50636 },
		// { 30304, 50617, 50637 },
		// { 30305, 50618, 50638 },
		// { 30306, 50619, 50639 },
	};

	int ore_drop_count_info[PICK_MAX_LEVEL + 1][2] =
	{
		{ 10,  20 },	// + 0
		{ 12,  22 },	// + 1
		{ 15,  25 },	// + 2
		{ 19,  29 },	// + 3
		{ 24,  34 },	// + 4
		{ 30,  40 },	// + 5
		{ 36,  46 },	// + 6
		{ 43,  53 },	// + 7
		{ 52,  62 },	// + 8
		{ 61,  71 },	// + 9
		{ 71,  81 },	// + 10
		{ 82,  92 },	// + 11
		{ 94,  104 },	// + 12
		{ 107,  117 },	// + 13
		{ 121,  131 },	// + 14
		{ 150,  165 },	// + 15
	};

	int mining_extra_drop_items[EXTRA_ITEMS_COUNT][4] =
	{
		//poziom pasywki, przedmiot, ilosc, szansa 10000 = 100%
		{ 0, 203007, 1, 200 },
		{ 30, 203007, 1, 300 },
		{ 40, 203007, 1, 350 },
	};
	
	int mining_extra_drop_items_summer[EXTRA_ITEMS_COUNT][4] =
	{
		//poziom pasywki, przedmiot, ilosc, szansa 10000 = 100%
		{ 0, 201220, 1, 1200 },   // Summer item 1 - 8% chance, no skill requirement
		{ 30, 201220, 1, 1200 },  // Summer item 2 - 8% chance, requires level 30 mining skill
		{ 40, 201220, 1, 1200 },  // Summer item 3 - 8% chance, requires level 40 mining skill
	};
	
	// int mining_extra_drop_items[EXTRA_ITEMS_COUNT][4] =
	// {
		// { 0, 29060, 1, 10000 },
		// { 30, 29070, 1, 10000 },
		// { 40, 29070, 1, 10000 },
		// { 0, 29050, 1, 10000 },
		// { 0, 29051, 1, 10000 },
		// { 30, 20100, 1, 10000 },
		// { 20, 30626, 1, 10000 },
		// { 40, 30626, 1, 10000 },
		// { 40, 30625, 1, 10000 },
		// { 20, 50590, 1, 10000 },
		// { 30, 50591, 1, 10000 },
		// { 40, 50592, 1, 10000 },
		// { 40, 50593, 1, 10000 },
		// { 40, 50594, 1, 10000 },
		// { 40, 26901, 1, 10000 },
	// };

	int passive_skill_bonus_values[SKILL_MAX_LEVEL + 1][3] = {
		// szansza_na_wydobycie, szansa_na_ilosc, skrocony_czas_kopania_w_sekundach
		{ 0, 0, 0 },		// 0
		{ 1, 0, 0 },		// 1
		{ 1, 1, 0 },		// 2
		{ 2, 1, 0 },		// 3
		{ 2, 2, 0 },		// 4
		{ 3, 2, 0 },		// 5
		{ 3, 3, 0 },		// 6
		{ 4, 3, 0 },		// 7
		{ 4, 4, 0 },		// 8
		{ 5, 4, 0 },		// 9
		{ 5, 5, 0 },		// 10
		{ 6, 5, 0 },		// 11
		{ 6, 6, 0 },		// 12
		{ 7, 6, 0 },		// 13
		{ 7, 7, 0 },		// 14
		{ 8, 7, 0 },		// 15
		{ 8, 8, 0 },		// 16
		{ 9, 8, 0 },		// 17
		{ 9, 9, 0 },		// 18
		{ 10, 9, 0 },		// 19
		{ 10, 10, 0 },		// M1
		{ 11, 10, 0 },		// M2
		{ 11, 11, 0 },		// M3
		{ 12, 11, 0 },		// M4
		{ 12, 12, 0 },		// M5
		{ 13, 12, 0 },		// M6
		{ 13, 13, 0 },		// M7
		{ 14, 13, 0 },		// M8
		{ 14, 14, 0 },		// M9
		{ 15, 14, 0 },		// M10
		{ 15, 15, 0 },		// G1
		{ 16, 15, 0 },		// G2
		{ 16, 16, 0 },		// G3
		{ 17, 16, 0 },		// G4
		{ 17, 17, 0 },		// G5
		{ 18, 17, 0 },		// G6
		{ 18, 18, 0 },		// G7
		{ 19, 18, 0 },		// G8
		{ 19, 19, 0 },		// G9
		{ 20, 19, 0 },		// G10
		{ 20, 20, 1 },		// P
	};

	DWORD GetRawOreFromLoad(DWORD dwLoadVnum)
	{
		for (int i = 0; i < MAX_ORE; ++i)
		{
			if (info[i].dwLoadVnum == dwLoadVnum)
				return info[i].dwRawOreVnum;
		}
		return 0;
	}

	DWORD GetRefineFromRawOre(DWORD dwRawOreVnum)
	{
		for (int i = 0; i < MAX_ORE; ++i)
		{
			if (info[i].dwRawOreVnum == dwRawOreVnum)
				return info[i].dwRefineVnum;
		}
		return 0;
	}

	// built in + from bonuses
	int GetPickAxeBonusValue(LPITEM pick, uint8_t attrType)
	{
		int value = 0;
		if (pick)
		{
			value = pick->FindApplyValue(attrType);
			for (int i = 0; i < 3; i++)
			{
				const TPlayerItemAttribute& attr = pick->GetAttribute(i);
				if (attr.bType == attrType)
				{
					value += attr.sValue;
					break;
				}
			}
		}

		return value;
	}

	void DecreasePickAxeBonuses(LPITEM pick)
	{
		int toSetIndex = 0;
		for (int i = 0; i < 3; i++)
		{
			const TPlayerItemAttribute& attr = pick->GetAttribute(i);
			if (attr.bType == 0)
				break;

			const int restCount = pick->GetAttribute(3 + i).sValue - 1;
			if (restCount > 0)
			{
				pick->SetForceAttribute(toSetIndex, attr.bType, attr.sValue, false);
				pick->SetForceAttribute(3 + toSetIndex, 1, restCount, false);
				if (i != toSetIndex)
				{
					pick->SetForceAttribute(i, 0, 0, false);
					pick->SetForceAttribute(3 + i, 0, 0, false);
				}
				toSetIndex++;
			}
			else
			{
				pick->SetForceAttribute(i, 0, 0, false);
				pick->SetForceAttribute(3 + i, 0, 0, false);
			}
		}

		pick->UpdatePacket();
		pick->Save();
	}

	int GetMiningBonusValue(LPCHARACTER ch, uint8_t attrType)
	{
		// 1 -> Built in bonuses in pickaxe
		// 2 -> Added bonuses in pickaxe
		// 3 -> Bonuses from points (for ex. builtin in artifact)
		// 4 -> Bonuses from passive skill
		// 5 -> Bonuses from premium packet

		/*
		{ POINT_MINING_SUCCESS_CHANCE, },	// APPLY_MINING_SUCCESS_CHANCE,	110
		{ POINT_MINING_MORE_ORES_CHANCE, },	// APPLY_MINING_MORE_ORES_CHANCE,111
		{ POINT_MINING_LESS_MINE_TIME, },	// APPLY_MINING_LESS_MINE_TIME,	112
		{ POINT_MINING_MORE_POINTS, },		// APPLY_MINING_MORE_POINTS,	113
		*/

		int value = GetPickAxeBonusValue(ch->GetWear(WEAR_WEAPON), attrType);
		value += ch->GetPoint(aApplyInfo[attrType].bPointType);

		if (attrType == APPLY_MINING_SUCCESS_CHANCE)
		{
			value += passive_skill_bonus_values[ch->GetSkillLevel(SKILL_MINING)][0];

			if (ch->IsVIP())
				value += 5;
		}
		else if (attrType == APPLY_MINING_MORE_ORES_CHANCE)
		{
			value += passive_skill_bonus_values[ch->GetSkillLevel(SKILL_MINING)][1];

			if (ch->IsVIP())
				value += 5;
		}
		else if (attrType == APPLY_MINING_MORE_POINTS)
		{
			value += passive_skill_bonus_values[ch->GetSkillLevel(SKILL_MINING)][1];

			if (ch->IsVIP())
				value += 20;
		}
		else if (attrType == APPLY_MINING_LESS_MINE_TIME)
		{
			value += passive_skill_bonus_values[ch->GetSkillLevel(SKILL_MINING)][2];

			if (ch->IsVIP())
				value += 1;
		}

		return value;
	}

	void SendMyOreCount(LPCHARACTER ch)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "my_ore %d", ch->GetQuestFlag("mining.my_ore_count"));
	}

	int GetOreDropCount(LPCHARACTER ch, int pickLevel, int& extraOreCount)
	{
		int dropCount = number(ore_drop_count_info[pickLevel][0], ore_drop_count_info[pickLevel][1]);
		if (number(1, 100) <= GetMiningBonusValue(ch, APPLY_MINING_MORE_ORES_CHANCE))
		{
			extraOreCount = dropCount;
			// 50% wiecej rudy
			dropCount *= 15;
			dropCount /= 10;

			extraOreCount = dropCount - extraOreCount;
		}
		return dropCount;
	}

	int GetPickLevel(LPITEM pick)
	{
		if (!pick)
			return 0;

		return MINMAX(0, pick->GetRefineLevel(), PICK_MAX_LEVEL);
	}

	void ExtraMiningItemDrop(LPCHARACTER ch)
	{
		// losuje 5 razy tylko jezeli wymagany poziom pasywki jest wiekszy, jezeli poziom pasywki bedzie pasowal to sprawdzam tylko szanse
		const int passiveLevel = ch->GetSkillLevel(SKILL_MINING);
		for (int i = 0; i < 3; i++)
		{
			if (passiveLevel < mining_extra_drop_items[i][0])
				continue;

			int myRandom = number(1, 10000);
			if (test_server)
				ch->ChatPacket(CHAT_TYPE_INFO, "%d %d %d isSuccess: %d", i, myRandom, mining_extra_drop_items[i][3], myRandom <= mining_extra_drop_items[i][3]);
			if (myRandom <= mining_extra_drop_items[i][3])
			{
				LPITEM item = ITEM_MANAGER::instance().CreateItem(mining_extra_drop_items[i][1], mining_extra_drop_items[i][2]);
				if (!item)
				{
					sys_err("cannot create item vnum %d", mining_extra_drop_items[i][1]);
					return;
				}
				
				PIXEL_POSITION pos;
				pos.x = ch->GetX() + number(-200, 200);
				pos.y = ch->GetY() + number(-200, 200);

				item->AddToGround(ch->GetMapIndex(), pos);
				item->StartDestroyEvent();
				item->SetOwnership(ch, 30);
			}
		}
	}
	
	void ExtraSummerMiningItemDrop(LPCHARACTER ch)
	{
		// Check if summer event is active (you may want to add a proper event check here)
		// if (!CHARACTER_MANAGER::Instance().CheckEventIsActive(SUMMER_MINING_EVENT, 0))
		//     return;
	
		const int passiveLevel = ch->GetSkillLevel(SKILL_MINING);
		for (int i = 0; i < EXTRA_ITEMS_COUNT; i++)
		{
			if (passiveLevel < mining_extra_drop_items_summer[i][0])
				continue;
	
			int myRandom = number(1, 10000);
			if (test_server)
				ch->ChatPacket(CHAT_TYPE_INFO, "Summer Drop %d: random=%d chance=%d isSuccess=%d", 
					i, myRandom, mining_extra_drop_items_summer[i][3], 
					myRandom <= mining_extra_drop_items_summer[i][3]);
			
			if (myRandom <= mining_extra_drop_items_summer[i][3])
			{
				LPITEM item = ITEM_MANAGER::instance().CreateItem(mining_extra_drop_items_summer[i][1], mining_extra_drop_items_summer[i][2]);
				if (!item)
				{
					sys_err("cannot create summer mining item vnum %d", mining_extra_drop_items_summer[i][1]);
					continue;
				}
				
				PIXEL_POSITION pos;
				pos.x = ch->GetX() + number(-200, 200);
				pos.y = ch->GetY() + number(-200, 200);
	
				item->AddToGround(ch->GetMapIndex(), pos);
				item->StartDestroyEvent();
				item->SetOwnership(ch, 30);
				
				//ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10093]");
			}
		}
	}

	void TryDropMiningEventChest(LPCHARACTER ch)
	{
		if (!ch)
			return;
	
		if (!CHARACTER_MANAGER::Instance().CheckEventIsActive(MINING_CHEST_EVENT, 0, ch))
			return;
	
		int chance = 20;
	
		//if (test_server)
		//{
		//	chance = 100;
		//}
		
		if (number(1, 100) <= chance)
		{
			LPITEM item = ITEM_MANAGER::instance().CreateItem(MINING_EVENT_CHEST_VNUM, 1);
			if (!item)
			{
				sys_err("TryDropMiningEventChest: Failed to create item vnum %d", MINING_EVENT_CHEST_VNUM);
				return;
			}
	
			PIXEL_POSITION pos;
			pos.x = ch->GetX() + number(-200, 200);
			pos.y = ch->GetY() + number(-200, 200);
	
			item->AddToGround(ch->GetMapIndex(), pos);
			item->StartDestroyEvent();
			item->SetOwnership(ch, 30);
	
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2451]");
		}
	}

	int GetOreDropChance(LPCHARACTER ch)
	{
		LPITEM pick = ch->GetWear(WEAR_WEAPON);
		if (!pick || pick->GetType() != ITEM_PICK)
			return 0;

		return 0 + GetMiningBonusValue(ch, APPLY_MINING_SUCCESS_CHANCE);
	}

	LPITEM OreDrop(LPCHARACTER ch, DWORD dwLoadVnum)
	{
		int extraOreCount = 0;
		const int allOreCount = GetOreDropCount(ch, GetPickLevel(ch->GetWear(WEAR_WEAPON)), extraOreCount);
		if (allOreCount == 0)
		{
			sys_err("Wrong ore fraction count");
			return nullptr;  // Vrátíme nullptr, pokud je počet rudy nulový
		}
	
		DWORD dwRawOreVnum = GetRawOreFromLoad(dwLoadVnum);
		LPITEM item = ITEM_MANAGER::instance().CreateItem(dwRawOreVnum, allOreCount);
		if (!item)
		{
			sys_err("cannot create item vnum %d", dwRawOreVnum);
			return nullptr;  // Pokud se položka nepodaří vytvořit, vrátíme nullptr
		}
	
		if (extraOreCount > 0)
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2451;%d]", extraOreCount);
	
		ch->SetQuestFlag("mining.my_ore_count", ch->GetQuestFlag("mining.my_ore_count") + allOreCount);
		SendMyOreCount(ch);
	
		PIXEL_POSITION pos;
		pos.x = ch->GetX() + number(-200, 200);
		pos.y = ch->GetY() + number(-200, 200);
	
		item->AddToGround(ch->GetMapIndex(), pos);
		item->StartDestroyEvent();
		item->SetOwnership(ch, 15);
	
		DBManager::instance().SendMoneyLog(MONEY_LOG_DROP, item->GetVnum(), item->GetCount());
	
		return item;  // Vrátíme položku, pokud vše proběhlo v pořádku
	}

	EVENTINFO(mining_event_info)
	{
		DWORD pid;
		DWORD vid_load;

		mining_event_info() : pid( 0 ) , vid_load( 0 ) { }
	};

	// REFINE_PICK
	bool Pick_Check(CItem& item)
	{
		if (item.GetType() != ITEM_PICK)
			return false;

		return true;
	}

	int Pick_GetMaxExp(CItem& pick)
	{
		return pick.GetValue(2);
	}

	int Pick_GetCurExp(CItem& pick)
	{
		return pick.GetSocket(0);
	}

	void Pick_IncCurExp(CItem& pick, int addCount)
	{
		int cur = Pick_GetCurExp(pick);
		pick.SetSocket(0, cur + addCount);
	}

	void Pick_MaxCurExp(CItem& pick)
	{
		int max = Pick_GetMaxExp(pick);
		pick.SetSocket(0, max);
	}

	bool Pick_Refinable(CItem& item)
	{
		if (Pick_GetCurExp(item) < Pick_GetMaxExp(item))
			return false;

		return true;
	}

	bool Pick_IsRefineSuccess(CItem& pick)
	{
		return (number(1,100) <= pick.GetValue(3));
	}

	int RealRefinePick(LPCHARACTER ch, LPITEM item, bool isRefineWithExtraItem)
	{
		if (!ch || !item)
			return 2;

		LogManager& rkLogMgr = LogManager::instance();
		ITEM_MANAGER& rkItemMgr = ITEM_MANAGER::instance();

		if (!Pick_Check(*item))
		{
			sys_err("REFINE_PICK_HACK pid(%u) item(%s:%d) type(%d)", ch->GetPlayerID(), item->GetName(), item->GetID(), item->GetType());
			rkLogMgr.RefineLog(ch->GetPlayerID(), item->GetName(), item->GetID(), -1, 1, "PICK_HACK");
			return 2;
		}

		CItem& rkOldPick = *item;

		if (!Pick_Refinable(rkOldPick))
			return 2;

		int iAdv = rkOldPick.GetValue(0) / 10;

		if (rkOldPick.IsEquipped() == true)
			return 2;

		if (Pick_IsRefineSuccess(rkOldPick))
		{
			rkLogMgr.RefineLog(ch->GetPlayerID(), rkOldPick.GetName(), rkOldPick.GetID(), iAdv, 1, "PICK");

			LPITEM pkNewPick = ITEM_MANAGER::instance().CreateItem(rkOldPick.GetRefinedVnum(), 1);
			if (pkNewPick)
			{
				item->CopyAttributeTo(pkNewPick);

				BYTE bCell = rkOldPick.GetCell();
				rkItemMgr.RemoveItem(item, "REMOVE (REFINE PICK)");
				pkNewPick->AddToCharacter(ch, TItemPos(INVENTORY, bCell));
				LogManager::instance().ItemLog(ch, pkNewPick, "REFINE PICK SUCCESS", pkNewPick->GetName());
				return 1;
			}

			return 2;
		}
		else
		{
			rkLogMgr.RefineLog(ch->GetPlayerID(), rkOldPick.GetName(), rkOldPick.GetID(), iAdv, 0, "PICK");
			
			rkOldPick.SetSocket(0, isRefineWithExtraItem ? Pick_GetCurExp(rkOldPick) / 2 : 0);

			rkLogMgr.ItemLog(ch, item, "REFINE PICK FAIL", item->GetName());
		}

		return 0;
	}

	void CHEAT_MAX_PICK(LPCHARACTER ch, LPITEM item)
	{
		if (!item)
			return;

		if (!Pick_Check(*item))
			return;

		CItem& pick = *item;
		Pick_MaxCurExp(pick);

		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("MINING_1"), Pick_GetCurExp(pick));
	}

	void PracticePick(LPCHARACTER ch, LPITEM item)
	{
		if (!item)
			return;

		if (!Pick_Check(*item))
			return;

		CItem& pick = *item;
		if (pick.GetRefinedVnum()<=0)
			return;
		
		if (Pick_Refinable(pick))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("MINING_2"));
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("MINING_3"));
		}
		else
		{
			if (number(1, 100) <= GetMiningBonusValue(ch, APPLY_MINING_MORE_POINTS))
			{
				Pick_IncCurExp(pick, 1 + 2);
				// ch->ChatPacket(CHAT_TYPE_INFO, "Zdobyto 2 punkty Kilofa wiecej dzieki bonusowi!");
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2452]");
			}
			else
				Pick_IncCurExp(pick, 1);

			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2453;%d;%d]", Pick_GetCurExp(pick), Pick_GetMaxExp(pick));

			if (Pick_Refinable(pick))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("MINING_2"));
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("MINING_3"));
			}
		}
	}
	// END_OF_REFINE_PICK

	EVENTFUNC(mining_event)
	{
		mining_event_info* info = dynamic_cast<mining_event_info*>( event->info );

		if ( info == NULL )
		{
			sys_err( "mining_event_info> <Factor> Null pointer" );
			return 0;
		}

		LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(info->pid);
		LPCHARACTER load = CHARACTER_MANAGER::instance().Find(info->vid_load);

		if (!ch)
			return 0;

		ch->mining_take();

		LPITEM pick = ch->GetWear(WEAR_WEAPON);

		// REFINE_PICK
		if (!pick || !Pick_Check(*pick))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("MINING_1"));
			return 0;
		}
		// END_OF_REFINE_PICK

		if (!load)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2466]");
			return 0;
		}

		DecreasePickAxeBonuses(pick);

		if (number(1, 100) <= GetOreDropChance(ch))
		{

#ifdef __ENABLE_MINING_EVENT__
			CMiningEventMgr::instance().SuccessMining(ch, load->GetRaceNum());
#endif
#ifdef RANKING_SYSTEM
			CServerRankingManager::instance().IncServerRankValue(ch, SERVER_RANK_TYPE_MINING);
			CRankingManager::instance().IncRankValue(ch, RANK_TYPE_MINING);
#endif
#ifdef BATTLE_PASS
			ch->GetBattlePass()->IncrementDailyMission(CBattlePass::DAILY_MISSION_MINING);
#endif

			const int po = number(6, 15);
			//if (ch->GetPremiumRemainSeconds(PREMIUM_PO) > 0)
			//{
			//	ch->PointChange(POINT_PKT_OSIAG, po*2, true);
			//	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[VIP]Zdobyles dodatkowe PO: %d"), po);
			//}
			//else
			//{
			ch->PointChange(POINT_PKT_OSIAG, po, true);
			//}
			
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2454]");
			LPITEM item = OreDrop(ch, load->GetRaceNum());
			if (item)
			{
				if (CHARACTER_MANAGER::Instance().CheckEventIsActive(DOUBLE_ORE_EVENT, 0, ch))
				{
					LPITEM doubleOreItem = OreDrop(ch, load->GetRaceNum());  // Vytvoříme druhou položku
					if (doubleOreItem && (ch->GetPremiumRemainSeconds(PREMIUM_PICKUP) > 0 || ch->IsVIP()))
					{
						ch->PickupItem(doubleOreItem->GetVID(), true);  // Pickup pro druhou položku
					}
				}
				ExtraMiningItemDrop(ch);
				ExtraSummerMiningItemDrop(ch);
				TryDropMiningEventChest(ch);
			
				if (ch->GetPremiumRemainSeconds(PREMIUM_PICKUP) > 0 || ch->IsVIP())
				{
					ch->PickupItem(item->GetVID(), true);
				}
			}

			PracticePick(ch, pick);
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2455]");
		}

		return 0;
	}

	LPEVENT CreateMiningEvent(LPCHARACTER ch, LPCHARACTER load, int seconds)
	{
		mining_event_info* info = AllocEventInfo<mining_event_info>();
		info->pid = ch->GetPlayerID();
		info->vid_load = load->GetVID();

		return event_create(mining_event, info, PASSES_PER_SEC(seconds));
	}

	bool OreRefine(LPCHARACTER ch, LPCHARACTER npc, LPITEM item, int cost, int pct, LPITEM metinstone_item)
	{
		if (!ch || !npc)
			return false;

		if (item->GetOwner() != ch)
		{
			sys_err("wrong owner");
			return false;
		}

		if (item->GetCount() < ORE_COUNT_FOR_REFINE)
		{
			sys_err("not enough count");
			return false;
		}

		DWORD dwRefinedVnum = GetRefineFromRawOre(item->GetVnum());

		if (dwRefinedVnum == 0)
			return false;

		ch->SetRefineNPC(npc);
		item->SetCount(item->GetCount() - ORE_COUNT_FOR_REFINE);
		int iCost = ch->ComputeRefineFee(cost, 1);

		if (ch->GetGold() < iCost)
			return false;

		ch->PayRefineFee(iCost);

		if (metinstone_item)
			ITEM_MANAGER::instance().RemoveItem(metinstone_item, "REMOVE (MELT)");

		if (number(1, 100) <= pct)
		{
			ch->AutoGiveItem(dwRefinedVnum, 1);
			return true;
		}

		return false;
	}

	bool IsVeinOfOre(DWORD vnum)
	{
		for (int i = 0; i < MAX_ORE; i++)
		{
			if (info[i].dwLoadVnum == vnum)
				return true;
		}
		return false;
	}
}
//martysama0134's ac258f174820b7674fa39b8b3a817d16
