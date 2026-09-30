#include "stdafx.h"
#include "constants.h"
#include "questmanager.h"
#include "questlua.h"
#include "dungeon.h"
#include "char.h"
#include "party.h"
#include "buffer_manager.h"
#include "char_manager.h"
#include "packet.h"
#include "desc_client.h"
#include "desc_manager.h"
#ifdef ENABLE_D_NJGUILD
#include "guild.h"
#include "utils.h"
#include "config.h"
#include "guild_manager.h"
#include "../../common/stl.h"
#include "db.h"
#include "affect.h"
#include "p2p.h"
#include "war_map.h"
#include "sectree_manager.h"
#include "locale_service.h"
#endif
#ifdef __DUNGEON_INFO_ENABLE__
	#include "DungeonInfoManager.hpp"
#endif
#ifdef __CROSS_CHANNEL_DUNGEON_WARP__
	#include "config.h"
	#include "p2p.h"
#endif

#include <fmt/format.h>
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
#include "AchievementTitle.h"
#endif
#ifdef ENABLE_SUMMER_HWID_LIMIT
#include "summer_hwid_manager.h"
using namespace SummerHwidManager;
#endif

#undef sys_err
#ifndef _WIN32
#define sys_err(fmt, args...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, ##args)
#else
#define sys_err(fmt, ...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#endif

template <class Func> Func CDungeon::ForEachMember(Func f)
{
	itertype(m_set_pkCharacter) it;

	for (it = m_set_pkCharacter.begin(); it != m_set_pkCharacter.end(); ++it)
	{
		sys_log(0, "Dungeon ForEachMember %s", (*it)->GetName());
		f(*it);
	}
	return f;
}

namespace quest
{
	// "dungeon" lua functions

	ALUA(dungeon_notice)
	{
		if (!lua_isstring(L, 1))
			return 0;

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
			pDungeon->Notice(lua_tostring(L, 1));
		return 0;
	}

	ALUA(dungeon_set_quest_flag)
	{
		CQuestManager & q = CQuestManager::instance();

		FSetQuestFlag f;

		f.flagname = q.GetCurrentPC()->GetCurrentQuestName() + "." + lua_tostring(L, 1);
		f.value = (int) rint(lua_tonumber(L, 2));

		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
			pDungeon->ForEachMember(f);

		return 0;
	}

	ALUA(dungeon_set_flag)
	{
		if (!lua_isstring(L,1) || !lua_isnumber(L,2))
		{
			sys_err("wrong set flag");
		}
		else
		{
			CQuestManager& q = CQuestManager::instance();
			LPDUNGEON pDungeon = q.GetCurrentDungeon();

			if (pDungeon)
			{
				const char* sz = lua_tostring(L,1);
				int value = int(lua_tonumber(L, 2));
				pDungeon->SetFlag(sz, value);
			}
			else
			{
				sys_err("no dungeon !!!");
			}
		}
		return 0;
	}

	ALUA(dungeon_get_flag)
	{
		if (!lua_isstring(L,1))
		{
			sys_err("wrong get flag");
		}

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
		{
			const char* sz = lua_tostring(L,1);
			lua_pushnumber(L, pDungeon->GetFlag(sz));
		}
		else
		{
			sys_err("no dungeon !!!");
			lua_pushnumber(L, 0);
		}

		return 1;
	}

	ALUA(dungeon_get_flag_from_map_index)
	{
		if (!lua_isstring(L,1) || !lua_isnumber(L,2))
		{
			sys_err("wrong get flag");
		}

		DWORD dwMapIndex = (DWORD) lua_tonumber(L, 2);
		if (dwMapIndex)
		{
			LPDUNGEON pDungeon = CDungeonManager::instance().FindByMapIndex(dwMapIndex);
			if (pDungeon)
			{
				const char* sz = lua_tostring(L,1);
				lua_pushnumber(L, pDungeon->GetFlag(sz));
			}
			else
			{
				sys_err("no dungeon !!!");
				lua_pushnumber(L, 0);
			}
		}
		else
		{
			lua_pushboolean(L, 0);
		}
		return 1;
	}

	ALUA(dungeon_get_map_index)
	{
		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
		{
			sys_log(0, "Dungeon GetMapIndex %d",pDungeon->GetMapIndex());
			lua_pushnumber(L, pDungeon->GetMapIndex());
		}
		else
		{
			sys_err("no dungeon !!!");
			lua_pushnumber(L, 0);
		}

		return 1;
	}

	ALUA(dungeon_regen_file)
	{
		if (!lua_isstring(L,1))
		{
			sys_err("wrong filename");
			return 0;
		}

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
			pDungeon->SpawnRegen(lua_tostring(L,1));

		return 0;
	}

	ALUA(dungeon_set_regen_file)
	{
		if (!lua_isstring(L,1))
		{
			sys_err("wrong filename");
			return 0;
		}
		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
			pDungeon->SpawnRegen(lua_tostring(L,1), false);
		return 0;
	}

	ALUA(dungeon_clear_regen)
	{
		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();
		if (pDungeon)
			pDungeon->ClearRegen();
		return 0;
	}

	ALUA(dungeon_check_eliminated)
	{
		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();
		if (pDungeon)
			pDungeon->CheckEliminated();
		return 0;
	}

	ALUA(dungeon_set_exit_all_at_eliminate)
	{
		if (!lua_isnumber(L,1))
		{
			sys_err("wrong time");
			return 0;
		}

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
			pDungeon->SetExitAllAtEliminate((int32_t)lua_tonumber(L, 1));

		return 0;
	}

	ALUA(dungeon_set_warp_at_eliminate)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("wrong time");
			return 0;
		}

		if (!lua_isnumber(L, 2))
		{
			sys_err("wrong map index");
			return 0;
		}

		if (!lua_isnumber(L, 3))
		{
			sys_err("wrong X");
			return 0;
		}

		if (!lua_isnumber(L, 4))
		{
			sys_err("wrong Y");
			return 0;
		}

		const char * c_pszRegenFile = NULL;

		if (lua_gettop(L) >= 5)
			c_pszRegenFile = lua_tostring(L,5);

#ifdef ENABLE_MINIMAP_DUNGEONINFO
		BYTE bStage;
		if (!lua_isnumber(L, 6))
			bStage = -1;
		else
			bStage = (BYTE)lua_tonumber(L,6);
		
		BYTE bStageMax;
		if (!lua_isnumber(L, 7))
			bStageMax = -1;
		else
			bStageMax = (BYTE)lua_tonumber(L,7);
		
		const char * stNotice = NULL;

		if (lua_gettop(L) >= 8)
			stNotice = lua_tostring(L,8);
#endif
		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
		{
#ifdef ENABLE_MINIMAP_DUNGEONINFO
			pDungeon->SetWarpAtEliminate((int32_t)lua_tonumber(L,1),
										 (int32_t)lua_tonumber(L,2),
										 (int32_t)lua_tonumber(L,3),
										 (int32_t)lua_tonumber(L,4),
										 c_pszRegenFile, bStage, bStageMax, stNotice);
#else
			pDungeon->SetWarpAtEliminate((int32_t)lua_tonumber(L,1),
										 (int32_t)lua_tonumber(L,2),
										 (int32_t)lua_tonumber(L,3),
										 (int32_t)lua_tonumber(L,4),
										 c_pszRegenFile);
#endif
		}
		else
			sys_err("cannot find dungeon");

		return 0;
	}

	ALUA(dungeon_new_jump)
	{
		if (lua_gettop(L) < 3)
		{
			sys_err("not enough argument");
			return 0;
		}

		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3))
		{
			sys_err("wrong argument");
			return 0;
		}

		int32_t lMapIndex = (int32_t)lua_tonumber(L,1);

		LPDUNGEON pDungeon = CDungeonManager::instance().Create(lMapIndex);

		if (!pDungeon)
		{
			sys_err("cannot create dungeon %d", lMapIndex);
			return 0;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		//ch->WarpSet(pDungeon->GetMapIndex(), (int) lua_tonumber(L, 2), (int)lua_tonumber(L, 3));
		ch->WarpSet((int) lua_tonumber(L, 2), (int)lua_tonumber(L, 3), pDungeon->GetMapIndex());
		return 0;
	}

#ifdef ENABLE_D_NJGUILD
	ALUA(dungeon_new_jump_guild)
	{
		if (lua_gettop(L)<3 || !lua_isnumber(L,1) || !lua_isnumber(L, 2) || !lua_isnumber(L,3))
		{
			sys_err("not enough argument");
			return 0;
		}

		int32_t lMapIndex = (int32_t)lua_tonumber(L,1);

		LPDUNGEON pDungeon = CDungeonManager::instance().Create(lMapIndex);

		if (!pDungeon)
		{
			sys_err("cannot create dungeon %d", lMapIndex);
			return 0;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (ch->GetGuild() == NULL)
		{
			sys_err ("cannot go to dungeon alone.");
			return 0;
		}

		pDungeon->JumpGuild(ch->GetGuild(), ch->GetMapIndex(), (int)lua_tonumber(L, 2), (int)lua_tonumber(L, 3));

		return 0;
	}

#endif
	ALUA(dungeon_new_jump_all)
	{
		if (lua_gettop(L)<3 || !lua_isnumber(L,1) || !lua_isnumber(L, 2) || !lua_isnumber(L,3))
		{
			sys_err("not enough argument");
			return 0;
		}

		int32_t lMapIndex = (int32_t)lua_tonumber(L,1);

		LPDUNGEON pDungeon = CDungeonManager::instance().Create(lMapIndex);

		if (!pDungeon)
		{
			sys_err("cannot create dungeon %d", lMapIndex);
			return 0;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		pDungeon->JumpAll(ch->GetMapIndex(), (int)lua_tonumber(L, 2), (int)lua_tonumber(L, 3));

		return 0;
	}

	ALUA(dungeon_new_jump_party)
	{
		if (lua_gettop(L)<3 || !lua_isnumber(L,1) || !lua_isnumber(L, 2) || !lua_isnumber(L,3))
		{
			sys_err("not enough argument");
			return 0;
		}

		int32_t lMapIndex = (int32_t)lua_tonumber(L,1);

		LPDUNGEON pDungeon = CDungeonManager::instance().Create(lMapIndex);

		if (!pDungeon)
		{
			sys_err("cannot create dungeon %d", lMapIndex);
			return 0;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (ch->GetParty() == NULL)
		{
			sys_err ("cannot go to dungeon alone.");
			return 0;
		}
		pDungeon->JumpParty(ch->GetParty(), ch->GetMapIndex(), (int)lua_tonumber(L, 2), (int)lua_tonumber(L, 3));

		return 0;
	}

	ALUA(dungeon_jump_all)
	{
		if (lua_gettop(L)<2 || !lua_isnumber(L, 1) || !lua_isnumber(L,2))
			return 0;

		LPDUNGEON pDungeon = CQuestManager::instance().GetCurrentDungeon();

		if (!pDungeon)
			return 0;

		pDungeon->JumpAll(pDungeon->GetMapIndex(), (int)lua_tonumber(L, 1), (int)lua_tonumber(L, 2));
		return 0;
	}

	ALUA(dungeon_warp_all)
	{
		if (lua_gettop(L)<2 || !lua_isnumber(L, 1) || !lua_isnumber(L,2))
			return 0;

		LPDUNGEON pDungeon = CQuestManager::instance().GetCurrentDungeon();

		if (!pDungeon)
			return 0;

		pDungeon->WarpAll(pDungeon->GetMapIndex(), (int)lua_tonumber(L, 1), (int)lua_tonumber(L, 2));
		return 0;
	}

	ALUA(dungeon_get_kill_stone_count)
	{
		if (!lua_isnumber(L,1) || !lua_isnumber(L,2))
		{
			lua_pushnumber(L, 0);
			return 1;
		}

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
		{
			lua_pushnumber(L, pDungeon->GetKillStoneCount());
			return 1;
		}

		lua_pushnumber(L, 0);
		return 1;
	}

	ALUA(dungeon_get_kill_mob_count)
	{
		if (!lua_isnumber(L,1) || !lua_isnumber(L,2))
		{
			lua_pushnumber(L, 0);
			return 1;
		}

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
		{
			lua_pushnumber(L, pDungeon->GetKillMobCount());
			return 1;
		}

		lua_pushnumber(L, 0);
		return 1;
	}

	ALUA(dungeon_is_use_potion)
	{
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2))
		{
			lua_pushboolean(L, 1);
			return 1;
		}

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
		{
			lua_pushboolean(L, pDungeon->IsUsePotion());
			return 1;
		}

		lua_pushboolean(L, 1);
		return 1;
	}

	ALUA(dungeon_revived)
	{
		if (!lua_isnumber(L,1) || !lua_isnumber(L,2))
		{
			lua_pushboolean(L, 1);
			return 1;
		}

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
		{
			lua_pushboolean(L, pDungeon->IsUseRevive());
			return 1;
		}

		lua_pushboolean(L, 1);
		return 1;
	}

	ALUA(dungeon_set_dest)
	{
		if (!lua_isnumber(L,1) || !lua_isnumber(L,2))
			return 0;

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		LPPARTY pParty = ch->GetParty();
		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon && pParty)
			pDungeon->SendDestPositionToParty(pParty, (int)lua_tonumber(L,1), (int)lua_tonumber(L,2));

		return 0;
	}

	ALUA(dungeon_unique_set_maxhp)
	{
		if (!lua_isstring(L,1) || !lua_isnumber(L,2))
			return 0;

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
			pDungeon->UniqueSetMaxHP(lua_tostring(L,1), (int)lua_tonumber(L,2));

		return 0;
	}

	ALUA(dungeon_unique_set_hp)
	{
		if (!lua_isstring(L,1) || !lua_isnumber(L,2))
			return 0;

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
			pDungeon->UniqueSetHP(lua_tostring(L,1), (int)lua_tonumber(L,2));

		return 0;
	}

	ALUA(dungeon_unique_set_def_grade)
	{
		if (!lua_isstring(L,1) || !lua_isnumber(L,2))
			return 0;

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
			pDungeon->UniqueSetDefGrade(lua_tostring(L,1), (int)lua_tonumber(L,2));

		return 0;
	}

	ALUA(dungeon_unique_set_immortal)
	{
		if (!lua_isstring(L,1) || !lua_isnumber(L,2))
			return 0;

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
			pDungeon->UniqueSetImmortal(lua_tostring(L,1), (int)lua_tonumber(L,2));

		return 0;
	}

	ALUA(dungeon_unique_get_hp_perc)
	{
		if (!lua_isstring(L,1))
		{
			lua_pushnumber(L,0);
			return 1;
		}

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
		{
			lua_pushnumber(L, pDungeon->GetUniqueHpPerc(lua_tostring(L,1)));
			return 1;
		}

		lua_pushnumber(L,0);
		return 1;
	}

	ALUA(dungeon_is_unique_dead)
	{
		if (!lua_isstring(L,1))
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
		{
			lua_pushboolean(L, pDungeon->IsUniqueDead(lua_tostring(L,1))?1:0);
			return 1;
		}

		lua_pushboolean(L, 0);
		return 1;
	}

	ALUA(dungeon_purge_unique)
	{
		if (!lua_isstring(L,1))
			return 0;
		sys_log(0,"QUEST_DUNGEON_PURGE_UNIQUE %s", lua_tostring(L,1));

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
			pDungeon->PurgeUnique(lua_tostring(L,1));

		return 0;
	}

	struct FPurgeArea
	{
		int x1, y1, x2, y2;
		LPCHARACTER ExceptChar;

		FPurgeArea(int a, int b, int c, int d, LPCHARACTER p)
			: x1(a), y1(b), x2(c), y2(d),
			ExceptChar(p)
		{}

		void operator () (LPENTITY ent)
		{
			if (true == ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER pChar = static_cast<LPCHARACTER>(ent);

				if (pChar == ExceptChar)
					return;
				
			#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
				if (!pChar->IsPet() && !pChar->IsHorse() && !pChar->IsBuffNPC() && (true == pChar->IsMonster() || true == pChar->IsStone() || true == pChar->IsBuffNPC() || true == pChar->IsHorse()))
			#else
				if (!pChar->IsPet() && !pChar->IsBuffNPC() && !pChar->IsHorse() && (true == pChar->IsMonster() || true == pChar->IsStone() || true == pChar->IsBuffNPC() || true == pChar->IsHorse()))
			#endif
				{
					if (x1 <= pChar->GetX() && pChar->GetX() <= x2 && y1 <= pChar->GetY() && pChar->GetY() <= y2)
					{
						M2_DESTROY_CHARACTER(pChar);
					}
				}
			}
		}
	};

	ALUA(dungeon_purge_area)
	{
		if (!lua_isnumber(L,1) || !lua_isnumber(L,2) || !lua_isnumber(L,3) || !lua_isnumber(L,4))
			return 0;
		sys_log(0,"QUEST_DUNGEON_PURGE_AREA");

		int x1 = lua_tonumber(L, 1);
		int y1 = lua_tonumber(L, 2);
		int x2 = lua_tonumber(L, 3);
		int y2 = lua_tonumber(L, 4);

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();
		if (!pDungeon)
			return 0;

		const int mapIndex = pDungeon->GetMapIndex();

		if (0 == mapIndex)
		{
			sys_err("_purge_area: cannot get a map index with (%u, %u)", x1, y1);
			return 0;
		}

		LPSECTREE_MAP pSectree = SECTREE_MANAGER::instance().GetMap(mapIndex);

		if (NULL != pSectree)
		{
			FPurgeArea func(x1, y1, x2, y2, CQuestManager::instance().GetCurrentNPCCharacterPtr());

			pSectree->for_each(func);
		}

		return 0;
	}

#ifdef ENABLE_NEWSTUFF
	struct FKillArea
	{
		int x1, y1, x2, y2;
		LPCHARACTER ExceptChar;

		FKillArea(int a, int b, int c, int d, LPCHARACTER p)
			: x1(a), y1(b), x2(c), y2(d),
			ExceptChar(p)
		{}

		void operator () (LPENTITY ent)
		{
			if (true == ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER pChar = static_cast<LPCHARACTER>(ent);

				if (pChar == ExceptChar)
					return;

				if (!pChar->IsPet() && !pChar->IsBuffNPC() && !pChar->IsHorse() && (true == pChar->IsMonster() || true == pChar->IsStone() || true == pChar->IsBuffNPC() || true == pChar->IsHorse()))
				{
					if (x1 <= pChar->GetX() && pChar->GetX() <= x2 && y1 <= pChar->GetY() && pChar->GetY() <= y2)
					{
						if (!pChar->IsDead())
							pChar->DeadNoReward(); // @fixme188 from Dead()
					}
				}
			}
		}
	};

	ALUA(dungeon_kill_area)
	{
		if (!lua_isnumber(L,1) || !lua_isnumber(L,2) || !lua_isnumber(L,3) || !lua_isnumber(L,4))
			return 0;
		sys_log(0,"QUEST_DUNGEON_KILL_AREA");

		int x1 = lua_tonumber(L, 1);
		int y1 = lua_tonumber(L, 2);
		int x2 = lua_tonumber(L, 3);
		int y2 = lua_tonumber(L, 4);

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		const int mapIndex = pDungeon->GetMapIndex();

		if (0 == mapIndex)
		{
			sys_err("_kill_area: cannot get a map index with (%u, %u)", x1, y1);
			return 0;
		}

		LPSECTREE_MAP pSectree = SECTREE_MANAGER::instance().GetMap(mapIndex);

		if (NULL != pSectree)
		{
			FKillArea func(x1, y1, x2, y2, CQuestManager::instance().GetCurrentNPCCharacterPtr());

			pSectree->for_each(func);
		}

		return 0;
	}
#endif

	ALUA(dungeon_kill_unique)
	{
		if (!lua_isstring(L,1))
			return 0;
		sys_log(0,"QUEST_DUNGEON_KILL_UNIQUE %s", lua_tostring(L,1));

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
			pDungeon->KillUnique(lua_tostring(L,1));

		return 0;
	}

	ALUA(dungeon_spawn_stone_door)
	{
		if (!lua_isstring(L,1) || !lua_isstring(L,2))
			return 0;
		sys_log(0,"QUEST_DUNGEON_SPAWN_STONE_DOOR %s %s", lua_tostring(L,1), lua_tostring(L,2));

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
			pDungeon->SpawnStoneDoor(lua_tostring(L,1), lua_tostring(L,2));

		return 0;
	}

	ALUA(dungeon_spawn_wooden_door)
	{
		if (!lua_isstring(L,1) || !lua_isstring(L,2))
			return 0;
		sys_log(0,"QUEST_DUNGEON_SPAWN_WOODEN_DOOR %s %s", lua_tostring(L,1), lua_tostring(L,2));

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
			pDungeon->SpawnWoodenDoor(lua_tostring(L,1), lua_tostring(L,2));

		return 0;
	}

	ALUA(dungeon_spawn_move_group)
	{
		if (!lua_isnumber(L,1) || !lua_isstring(L,2) || !lua_isstring(L,3))
			return 0;
		sys_log(0,"QUEST_DUNGEON_SPAWN_MOVE_GROUP %d %s %s", (int)lua_tonumber(L,1), lua_tostring(L,2), lua_tostring(L,3));

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
			pDungeon->SpawnMoveGroup((DWORD)lua_tonumber(L,1), lua_tostring(L,2), lua_tostring(L,3));

		return 0;
	}

	ALUA(dungeon_spawn_move_unique)
	{
		if (!lua_isstring(L,1) || !lua_isnumber(L,2) || !lua_isstring(L,3) || !lua_isstring(L,4))
			return 0;
		sys_log(0,"QUEST_DUNGEON_SPAWN_MOVE_UNIQUE %s %d %s %s", lua_tostring(L,1), (int)lua_tonumber(L,2), lua_tostring(L,3), lua_tostring(L,4));

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
			pDungeon->SpawnMoveUnique(lua_tostring(L,1), (DWORD)lua_tonumber(L,2), lua_tostring(L,3), lua_tostring(L,4));

		return 0;
	}

	ALUA(dungeon_spawn_unique)
	{
		if (!lua_isstring(L,1) || !lua_isnumber(L,2) || !lua_isstring(L,3))
			return 0;
		sys_log(0,"QUEST_DUNGEON_SPAWN_UNIQUE %s %d %s", lua_tostring(L,1), (int)lua_tonumber(L,2), lua_tostring(L,3));

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
			pDungeon->SpawnUnique(lua_tostring(L,1), (DWORD)lua_tonumber(L,2), lua_tostring(L,3));

		return 0;
	}

	ALUA(dungeon_spawn)
	{
		if (!lua_isnumber(L,1) || !lua_isstring(L,2))
			return 0;
		sys_log(0,"QUEST_DUNGEON_SPAWN %d %s", (int)lua_tonumber(L,1), lua_tostring(L,2));

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
			pDungeon->Spawn((DWORD)lua_tonumber(L,1), lua_tostring(L,2));

		return 0;
	}

	ALUA(dungeon_set_unique)
	{
		if (!lua_isstring(L, 1) || !lua_isnumber(L, 2))
			return 0;

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		DWORD vid = (DWORD) lua_tonumber(L, 2);

		if (pDungeon)
			pDungeon->SetUnique(lua_tostring(L, 1), vid);
		return 0;
	}

#ifdef NEW_ICEDAMAGE_SYSTEM
	ALUA(dungeon_get_damage_from_race)
	{
		if (!lua_isnumber(L, 1))
			return 0;

		// extra check for d BEGIN
		// CQuestManager& q = CQuestManager::instance();
		// LPDUNGEON pDungeon = q.GetCurrentDungeon();
		// if (!pDungeon)
			// return 0;
		// extra check for d END
		DWORD vid = (DWORD) lua_tonumber(L, 1);

		LPCHARACTER ch = CHARACTER_MANAGER::instance().Find(vid);

		lua_pushnumber(L, (ch)?ch->GetNoDamageRaceFlag():0);
		return 1;
	}

	ALUA(dungeon_get_damage_from_affect)
	{
		if (!lua_isnumber(L, 1))
			return 0;

		// extra check for d BEGIN
		// CQuestManager& q = CQuestManager::instance();
		// LPDUNGEON pDungeon = q.GetCurrentDungeon();
		// if (!pDungeon)
			// return 0;
		// extra check for d END
		DWORD vid = (DWORD) lua_tonumber(L, 1);

		LPCHARACTER ch = CHARACTER_MANAGER::instance().Find(vid);

		lua_newtable(L);
		if (ch)
		{
			const std::set<DWORD> & tmp_setNDAFlag = ch->GetNoDamageAffectFlag();
			if (tmp_setNDAFlag.size())
			{
				DWORD dwTmpLuaIdx = 1;
				for (std::set<DWORD>::iterator it = tmp_setNDAFlag.begin(); it != tmp_setNDAFlag.end(); ++it)
				{
					lua_pushnumber(L, *it);
					lua_rawseti(L, -2, dwTmpLuaIdx++);
				}
			}
		}
		return 1;
	}

	ALUA(dungeon_set_damage_from_race)
	{
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2))
			return 0;

		// extra check for d BEGIN
		// CQuestManager& q = CQuestManager::instance();
		// LPDUNGEON pDungeon = q.GetCurrentDungeon();
		// if (!pDungeon)
			// return 0;
		// extra check for d END
		DWORD vid = (DWORD) lua_tonumber(L, 2);

		LPCHARACTER ch = CHARACTER_MANAGER::instance().Find(vid);
		if (ch)
			ch->SetNoDamageRaceFlag(lua_tonumber(L, 1));
		return 0;
	}

	ALUA(dungeon_set_damage_from_affect)
	{
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2))
			return 0;

		// extra check for d BEGIN
		// CQuestManager& q = CQuestManager::instance();
		// LPDUNGEON pDungeon = q.GetCurrentDungeon();
		// if (!pDungeon)
			// return 0;
		// extra check for d END
		DWORD vid = (DWORD) lua_tonumber(L, 2);

		LPCHARACTER ch = CHARACTER_MANAGER::instance().Find(vid);
		if (ch)
			ch->SetNoDamageAffectFlag(lua_tonumber(L, 1));
		return 0;
	}

	ALUA(dungeon_unset_damage_from_race)
	{
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2))
			return 0;

		// extra check for d BEGIN
		// CQuestManager& q = CQuestManager::instance();
		// LPDUNGEON pDungeon = q.GetCurrentDungeon();
		// if (!pDungeon)
			// return 0;
		// extra check for d END
		DWORD vid = (DWORD) lua_tonumber(L, 2);

		LPCHARACTER ch = CHARACTER_MANAGER::instance().Find(vid);
		if (ch)
			ch->UnsetNoDamageRaceFlag(lua_tonumber(L, 1));
		return 0;
	}

	ALUA(dungeon_unset_damage_from_affect)
	{
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2))
			return 0;

		// extra check for d BEGIN
		// CQuestManager& q = CQuestManager::instance();
		// LPDUNGEON pDungeon = q.GetCurrentDungeon();
		// if (!pDungeon)
			// return 0;
		// extra check for d END
		DWORD vid = (DWORD) lua_tonumber(L, 2);

		LPCHARACTER ch = CHARACTER_MANAGER::instance().Find(vid);
		if (ch)
			ch->UnsetNoDamageAffectFlag(lua_tonumber(L, 1));
		return 0;
	}

	ALUA(dungeon_reset_damage_from_race)
	{
		if (!lua_isnumber(L, 1))
			return 0;

		// extra check for d BEGIN
		// CQuestManager& q = CQuestManager::instance();
		// LPDUNGEON pDungeon = q.GetCurrentDungeon();
		// if (!pDungeon)
			// return 0;
		// extra check for d END
		DWORD vid = (DWORD) lua_tonumber(L, 1);

		LPCHARACTER ch = CHARACTER_MANAGER::instance().Find(vid);
		if (ch)
			ch->ResetNoDamageRaceFlag();
		return 0;
	}

	ALUA(dungeon_reset_damage_from_affect)
	{
		if (!lua_isnumber(L, 1))
			return 0;

		// extra check for d BEGIN
		// CQuestManager& q = CQuestManager::instance();
		// LPDUNGEON pDungeon = q.GetCurrentDungeon();
		// if (!pDungeon)
			// return 0;
		// extra check for d END
		DWORD vid = (DWORD) lua_tonumber(L, 1);

		LPCHARACTER ch = CHARACTER_MANAGER::instance().Find(vid);
		if (ch)
			ch->ResetNoDamageAffectFlag();
		return 0;
	}
#endif
#ifdef ENABLE_NEWSTUFF
	ALUA(dungeon_is_available0)
	{
		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		lua_pushboolean(L, pDungeon!=NULL);
		return 1;
	}
#endif

	ALUA(dungeon_get_unique_vid)
	{
		if (!lua_isstring(L,1))
		{
			lua_pushnumber(L,0);
			return 1;
		}

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
		{
			lua_pushnumber(L, pDungeon->GetUniqueVid(lua_tostring(L,1)));
			return 1;
		}

		lua_pushnumber(L,0);
		return 1;
	}

	ALUA(dungeon_spawn_mob)
	{
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3))
		{
			sys_err("invalid argument");
			return 0;
		}

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		DWORD vid = 0;

		if (pDungeon)
		{
			DWORD dwVnum = (DWORD) lua_tonumber(L, 1);
			int32_t x = (int32_t) lua_tonumber(L, 2);
			int32_t y = (int32_t) lua_tonumber(L, 3);
			float radius = lua_isnumber(L, 4) ? (float) lua_tonumber(L, 4) : 0;
			DWORD count = (lua_isnumber(L, 5)) ? (DWORD) lua_tonumber(L, 5) : 1;

			sys_log(0, "dungeon_spawn_mob %u %d %d", dwVnum, x, y);

			if (count == 0)
				count = 1;

			while (count --)
			{
				if (radius<1)
				{
					LPCHARACTER ch = pDungeon->SpawnMob(dwVnum, x, y);
					if (ch && !vid)
						vid = ch->GetVID();
				}
				else
				{
					float angle = number(0, 999) * M_PI * 2 / 1000;
					float r = number(0, 999) * radius / 1000;

					int32_t nx = x + (int32_t)(r * cos(angle));
					int32_t ny = y + (int32_t)(r * sin(angle));

					LPCHARACTER ch = pDungeon->SpawnMob(dwVnum, nx, ny);
					if (ch && !vid)
						vid = ch->GetVID();
				}
			}
		}

		lua_pushnumber(L, vid);
		return 1;
	}

	ALUA(dungeon_spawn_mob_dir)
	{
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3) || !lua_isnumber(L, 4))
		{
			sys_err("invalid argument");
			return 0;
		}

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		DWORD vid = 0;

		if (pDungeon)
		{
			DWORD dwVnum = (DWORD) lua_tonumber(L, 1);
			int32_t x = (int32_t) lua_tonumber(L, 2);
			int32_t y = (int32_t) lua_tonumber(L, 3);
			BYTE dir = (int) lua_tonumber(L, 4);

			LPCHARACTER ch = pDungeon->SpawnMob(dwVnum, x, y, dir);
			if (ch && !vid)
				vid = ch->GetVID();
		}
		lua_pushnumber(L, vid);
		return 1;
	}

	ALUA(dungeon_spawn_mob_ac_dir)
	{
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3) || !lua_isnumber(L, 4))
		{
			sys_err("invalid argument");
			return 0;
		}

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		DWORD vid = 0;

		if (pDungeon)
		{
			DWORD dwVnum = (DWORD) lua_tonumber(L, 1);
			int32_t x = (int32_t) lua_tonumber(L, 2);
			int32_t y = (int32_t) lua_tonumber(L, 3);
			BYTE dir = (int) lua_tonumber(L, 4);

			LPCHARACTER ch = pDungeon->SpawnMob_ac_dir(dwVnum, x, y, dir);
			if (ch && !vid)
				vid = ch->GetVID();
		}
		lua_pushnumber(L, vid);
		return 1;
	}

	ALUA(dungeon_spawn_goto_mob)
	{
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3) || !lua_isnumber(L, 4))
			return 0;

		int32_t lFromX = (int32_t)lua_tonumber(L, 1);
		int32_t lFromY = (int32_t)lua_tonumber(L, 2);
		int32_t lToX   = (int32_t)lua_tonumber(L, 3);
		int32_t lToY   = (int32_t)lua_tonumber(L, 4);

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
			pDungeon->SpawnGotoMob(lFromX, lFromY, lToX, lToY);

		return 0;
	}

	ALUA(dungeon_spawn_name_mob)
	{
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3) || !lua_isstring(L, 4))
			return 0;

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
		{
			DWORD dwVnum = (DWORD) lua_tonumber(L, 1);
			int32_t x = (int32_t)lua_tonumber(L, 2);
			int32_t y = (int32_t)lua_tonumber(L, 3);
			pDungeon->SpawnNameMob(dwVnum, x, y, lua_tostring(L, 4));
		}
		return 0;
	}

	ALUA(dungeon_spawn_group)
	{
		// argument: vnum,x,y,radius,aggressive,count

		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3) || !lua_isnumber(L, 4) || !lua_isnumber(L, 6))
		{
			sys_err("invalid argument");
			return 0;
		}

		DWORD vid = 0;

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
		{
			DWORD group_vnum = (DWORD)lua_tonumber(L, 1);
			int32_t local_x = (int32_t) lua_tonumber(L, 2) * 100;
			int32_t local_y = (int32_t) lua_tonumber(L, 3) * 100;
			float radius = (float) lua_tonumber(L, 4) * 100;
			bool bAggressive = lua_toboolean(L, 5);
			DWORD count = (DWORD) lua_tonumber(L, 6);

			LPCHARACTER chRet = pDungeon->SpawnGroup(group_vnum, local_x, local_y, radius, bAggressive, count);
			if (chRet)
				vid = chRet->GetVID();
		}

		lua_pushnumber(L, vid);
		return 1;
	}

	ALUA(dungeon_join)
	{
		if (lua_gettop(L) < 1 || !lua_isnumber(L, 1))
			return 0;

		int32_t lMapIndex = (int32_t)lua_tonumber(L, 1);
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

#ifdef __DUNGEON_LIMIT_RATES__
		auto IncrementDungeonCount = [](LPCHARACTER ch_party, int mapIndex, uint16_t value) -> bool {
			return DungeonRates::AppendCount(ch_party, mapIndex, value);
		};

		bool bMatched = true; // Start with true, set to false if ANY member fails
		if (auto* party = ch->GetParty()) {
			// Use a vector to collect all results
			std::vector<bool> results;
			
			// First, collect all party members
			std::vector<LPCHARACTER> members;
			party->ForEachOnMapMemberBool([&members](LPCHARACTER member) {
				members.push_back(member);
				return true; // Always continue to collect all
			}, lMapIndex, false); // false = don't check map
			
			// Now process all members
			for (LPCHARACTER member : members) {
				if (member) {
					bool memberResult = IncrementDungeonCount(member, lMapIndex, 1);
					results.push_back(memberResult);
					if (!memberResult) {
						bMatched = false;
					}
					sys_log(0, "DEBUG: Member %s result: %s", 
						member->GetName(), memberResult ? "SUCCESS" : "FAILED");
				}
			}
		} else {
			bMatched = IncrementDungeonCount(ch, lMapIndex, 1);
		}

		if (!bMatched)
		{ 
			sys_log(0, "Dungeon join blocked due to rate limits.");
			return 0; // RETURN EARLY - don't create dungeon at all
		}
#endif

		LPDUNGEON pDungeon = CDungeonManager::instance().Create(lMapIndex);

		if (!pDungeon)
		{
			#ifndef __CROSS_CHANNEL_DUNGEON_WARP__
				return 0;
			#else
				// Send request to all peers if anyone has this map in pool
				// Bear in mind we got to save pid(s) as well
				TPacketGGCreateDungeonInstance p{};
				p.bHeader = HEADER_GG_CREATE_DUNGEON_INSTANCE;
				p.lMapIndex = lMapIndex;
				p.bChannel = g_bChannel;
				p.bRequest = true;
				p.bCount = 1;

				if (ch->GetParty())
					p.bCount = ch->GetParty()->PopulatePartyPids(p.aPids);
				else
					p.aPids[0] = ch->GetPlayerID();

				P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGCreateDungeonInstance));
				sys_log(0, "A request from PID: %d regarding map %d has been sent over cores. Waiting for response.", ch->GetPlayerID(), lMapIndex);

				return 0;
			#endif
		}

		if (ch->GetParty() && ch->GetParty()->GetLeaderPID() == ch->GetPlayerID())
			pDungeon->JoinParty(ch->GetParty());
		else if (!ch->GetParty())
			pDungeon->Join(ch);

		return 0;
	}

	ALUA(dungeon_exit)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		ch->ExitToSavedLocation();
		return 0;
	}

	ALUA(dungeon_exit_all)
	{
		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
			pDungeon->ExitAll();

		return 0;
	}

	struct FSayDungeonByItemGroup
	{
		const CDungeon::ItemGroup* item_group;
		std::string can_enter_ment;
		std::string cant_enter_ment;
		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = (LPCHARACTER) ent;

				if (ch->IsPC())
				{
					struct ::packet_script packet_script;
					TEMP_BUFFER buf;

					for (CDungeon::ItemGroup::const_iterator it = item_group->begin(); it != item_group->end(); it++)
					{
						if(ch->CountSpecifyItem(it->first) >= it->second)
						{
							packet_script.header = HEADER_GC_SCRIPT;
							packet_script.skin = quest::CQuestManager::QUEST_SKIN_NORMAL;
							packet_script.src_size = can_enter_ment.size();
							packet_script.size = packet_script.src_size + sizeof(struct packet_script);

							buf.write(&packet_script, sizeof(struct packet_script));
							buf.write(&can_enter_ment[0], can_enter_ment.size());
							ch->GetDesc()->Packet(buf.read_peek(), buf.size());
							return;
						}
					}

					packet_script.header = HEADER_GC_SCRIPT;
					packet_script.skin = quest::CQuestManager::QUEST_SKIN_NORMAL;
					packet_script.src_size = cant_enter_ment.size();
					packet_script.size = packet_script.src_size + sizeof(struct packet_script);

					buf.write(&packet_script, sizeof(struct packet_script));
					buf.write(&cant_enter_ment[0], cant_enter_ment.size());
					ch->GetDesc()->Packet(buf.read_peek(), buf.size());
				}
			}
		}
	};

	ALUA(dungeon_say_diff_by_item_group)
	{
		if (!lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3))
		{
			sys_log(0, "QUEST wrong set flag");
			return 0;
		}

		CQuestManager& q = CQuestManager::instance();
		CDungeon * pDungeon = q.GetCurrentDungeon();

		if(!pDungeon)
		{
			sys_err("QUEST : no dungeon");
			return 0;
		}

		SECTREE_MAP * pMap = SECTREE_MANAGER::instance().GetMap(pDungeon->GetMapIndex());

		if (!pMap)
		{
			sys_err("cannot find map by index %d", pDungeon->GetMapIndex());
			return 0;
		}
		FSayDungeonByItemGroup f;
		sys_log (0,"diff_by_item");

		std::string group_name (lua_tostring (L, 1));
		f.item_group = pDungeon->GetItemGroup (group_name);

		if (f.item_group == NULL)
		{
			sys_err ("invalid item group");
			return 0;
		}

		f.can_enter_ment = lua_tostring(L, 2);
		f.can_enter_ment+= "[ENTER][ENTER][ENTER][ENTER][DONE]";
		f.cant_enter_ment = lua_tostring(L, 3);
		f.cant_enter_ment+= "[ENTER][ENTER][ENTER][ENTER][DONE]";

		pMap -> for_each( f );

		return 0;
	}

	struct FExitDungeonByItemGroup
	{
		const CDungeon::ItemGroup* item_group;

		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = (LPCHARACTER) ent;

				if (ch->IsPC())
				{
					for (CDungeon::ItemGroup::const_iterator it = item_group->begin(); it != item_group->end(); it++)
					{
						if(ch->CountSpecifyItem(it->first) >= it->second)
						{
							return;
						}
					}
					ch->ExitToSavedLocation();
				}
			}
		}
	};

	ALUA(dungeon_exit_all_by_item_group)
	{
		if (!lua_isstring(L, 1))
		{
			sys_log(0, "QUEST wrong set flag");
			return 0;
		}

		CQuestManager& q = CQuestManager::instance();
		CDungeon * pDungeon = q.GetCurrentDungeon();

		if(!pDungeon)
		{
			sys_err("QUEST : no dungeon");
			return 0;
		}

		SECTREE_MAP * pMap = SECTREE_MANAGER::instance().GetMap(pDungeon->GetMapIndex());

		if (!pMap)
		{
			sys_err("cannot find map by index %d", pDungeon->GetMapIndex());
			return 0;
		}
		FExitDungeonByItemGroup f;

		std::string group_name (lua_tostring (L, 1));
		f.item_group = pDungeon->GetItemGroup (group_name);

		if (f.item_group == NULL)
		{
			sys_err ("invalid item group");
			return 0;
		}

		pMap -> for_each( f );

		return 0;
	}

	struct FDeleteItemInItemGroup
	{
		const CDungeon::ItemGroup* item_group;

		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = (LPCHARACTER) ent;

				if (ch->IsPC())
				{
					for (CDungeon::ItemGroup::const_iterator it = item_group->begin(); it != item_group->end(); it++)
					{
						if(ch->CountSpecifyItem(it->first) >= it->second)
						{
							ch->RemoveSpecifyItem (it->first, it->second);
							return;
						}
					}
				}
			}
		}
	};

	ALUA(dungeon_delete_item_in_item_group_from_all)
	{
		if (!lua_isstring(L, 1))
		{
			sys_log(0, "QUEST wrong set flag");
			return 0;
		}

		CQuestManager& q = CQuestManager::instance();
		CDungeon * pDungeon = q.GetCurrentDungeon();

		if(!pDungeon)
		{
			sys_err("QUEST : no dungeon");
			return 0;
		}

		SECTREE_MAP * pMap = SECTREE_MANAGER::instance().GetMap(pDungeon->GetMapIndex());

		if (!pMap)
		{
			sys_err("cannot find map by index %d", pDungeon->GetMapIndex());
			return 0;
		}
		FDeleteItemInItemGroup f;

		std::string group_name (lua_tostring (L, 1));
		f.item_group = pDungeon->GetItemGroup (group_name);

		if (f.item_group == NULL)
		{
			sys_err ("invalid item group");
			return 0;
		}

		pMap -> for_each( f );

		return 0;
	}

	ALUA(dungeon_kill_all)
	{
		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
			pDungeon->KillAll();

		return 0;
	}

	ALUA(dungeon_purge)
	{
		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
			pDungeon->Purge();

		return 0;
	}

	ALUA(dungeon_exit_all_to_start_position)
	{
		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
			pDungeon->ExitAllToStartPosition();

		return 0;
	}

	ALUA(dungeon_count_monster)
	{
		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
			lua_pushnumber(L, pDungeon->CountMonster());
		else
		{
			sys_err("not in a dungeon");
			lua_pushnumber(L, INT_MAX);
		}

		return 1;
	}

	ALUA(dungeon_select)
	{
		DWORD dwMapIndex = (DWORD) lua_tonumber(L, 1);
		if (dwMapIndex)
		{
			LPDUNGEON pDungeon = CDungeonManager::instance().FindByMapIndex(dwMapIndex);
			if (pDungeon)
			{
				CQuestManager::instance().SelectDungeon(pDungeon);
				lua_pushboolean(L, 1);
			}
			else
			{
				CQuestManager::instance().SelectDungeon(NULL);
				lua_pushboolean(L, 0);
			}
		}
		else
		{
			CQuestManager::instance().SelectDungeon(NULL);
			lua_pushboolean(L, 0);
		}
		return 1;
	}

	ALUA(dungeon_find)
	{
		DWORD dwMapIndex = (DWORD) lua_tonumber(L, 1);
		if (dwMapIndex)
		{
			LPDUNGEON pDungeon = CDungeonManager::instance().FindByMapIndex(dwMapIndex);
			if (pDungeon)
			{
				lua_pushboolean(L, 1);
			}
			else
			{
				lua_pushboolean(L, 0);
			}
		}
		else
		{
			lua_pushboolean(L, 0);
		}
		return 1;
	}

	ALUA(dungeon_all_near_to)
	{
		LPDUNGEON pDungeon = CQuestManager::instance().GetCurrentDungeon();

		if (pDungeon != NULL)
		{
			lua_pushboolean(L, pDungeon->IsAllPCNearTo( (int)lua_tonumber(L, 1), (int)lua_tonumber(L, 2), 30));
		}
		else
		{
			lua_pushboolean(L, false);
		}

		return 1;
	}

	ALUA(dungeon_set_warp_location)
	{
		LPDUNGEON pDungeon = CQuestManager::instance().GetCurrentDungeon();

		if (pDungeon == NULL)
		{
			return 0;
		}

		if (lua_gettop(L)<3 || !lua_isnumber(L, 1) || !lua_isnumber(L,2) || !lua_isnumber(L, 3))
		{
			return 0;
		}

		FSetWarpLocation f ((int)lua_tonumber(L, 1), (int)lua_tonumber(L, 2), (int)lua_tonumber(L, 3));
		pDungeon->ForEachMember (f);

		return 0;
	}

	ALUA(dungeon_set_item_group)
	{
		if (!lua_isstring (L, 1) || !lua_isnumber (L, 2))
		{
			return 0;
		}
		std::string group_name (lua_tostring (L, 1));
		int size = lua_tonumber (L, 2);

		CDungeon::ItemGroup item_group;

		for (int i = 0; i < size; i++)
		{
			if (!lua_isnumber (L, i * 2 + 3) || !lua_isnumber (L, i * 2 + 4))
			{
				return 0;
			}
			item_group.emplace_back(lua_tonumber (L, i * 2 + 3), lua_tonumber (L, i * 2 + 4));
		}
		LPDUNGEON pDungeon = CQuestManager::instance().GetCurrentDungeon();

		if (pDungeon == NULL)
		{
			return 0;
		}

		pDungeon->CreateItemGroup (group_name, item_group);
		return 0;
	}

	ALUA(dungeon_set_quest_flag2)
	{
		CQuestManager & q = CQuestManager::instance();

		FSetQuestFlag f;

		if (!lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isnumber(L, 3))
		{
			sys_err("Invalid Argument");
		}

		f.flagname = string (lua_tostring(L, 1)) + "." + lua_tostring(L, 2);
		f.value = (int) rint(lua_tonumber(L, 3));

		LPDUNGEON pDungeon = q.GetCurrentDungeon();

		if (pDungeon)
			pDungeon->ForEachMember(f);

		return 0;
	}

#if defined(__DUNGEON_INFO_SYSTEM__)
	int dungeon_update_ranking(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		const std::string c_szQuestName = q.GetCurrentPC()->GetCurrentQuestName();

		LPDUNGEON pDungeon = q.GetCurrentDungeon();
		if (pDungeon)
		{
			FUpdateDungeonRanking f(c_szQuestName);
			pDungeon->ForEachMember(f);
		}

		return 0;
	}
#endif
#ifdef ENABLE_MINIMAP_DUNGEONINFO
	int dungeon_minimap_dungeoninfo_status(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
			return 0;

		CQuestManager & q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();
		
		if (pDungeon)
			pDungeon->SetMiniMapDungeonInfoStatus((DWORD)lua_tonumber(L, 1));
			
		return 0;
	}

	int dungeon_minimap_dungeoninfo_stage(lua_State* L)
	{
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) 
			return 0;

		CQuestManager & q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();
		if (pDungeon)
			pDungeon->SetMiniMapDungeonInfoStage((DWORD)lua_tonumber(L, 1), (DWORD)lua_tonumber(L, 2));

		return 0;
	}

	int dungeon_minimap_dungeoninfo_get_stage(lua_State* L)
	{
		CQuestManager & q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();
		if (pDungeon) {
			//sys_log(0, "Dungeon GetMiniMapDungeonInfoStage %d",pDungeon->GetMiniMapDungeonInfoStage());
			lua_pushnumber(L, pDungeon->GetMiniMapDungeonInfoStage(0));
		}
		else {
			sys_err("no dungeon !!!");
			lua_pushnumber(L, 0);
		}

		return 0;
	}

	int dungeon_minimap_dungeoninfo_timer(lua_State* L)
	{
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) 
			return 0;

		CQuestManager & q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();
		
		if (pDungeon)
			pDungeon->SetMiniMapDungeonInfoTimer((DWORD)lua_tonumber(L, 1), (DWORD)lua_tonumber(L, 2));
			
		return 0;
	}

	int dungeon_minimap_dungeoninfo_gauge(lua_State* L)
	{
		
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) 
			return 0;
			
		CQuestManager & q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();
		if (pDungeon)
			pDungeon->SetMiniMapDungeonInfoGauge((DWORD)lua_tonumber(L, 1), (DWORD)lua_tonumber(L, 2), (DWORD)lua_tonumber(L, 3));
			
		return 0;
	}

	int dungeon_minimap_dungeoninfo_notice(lua_State* L)
	{
		if (!lua_isstring(L, 1)) 
			return 0;

		const char * strNotice = NULL;

		if (lua_gettop(L) >= 1)
			strNotice = lua_tostring(L, 1);
		
		CQuestManager & q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();
		
		if (pDungeon)
			pDungeon->SetMiniMapDungeonInfoNotice(strNotice);

		return 0;
	}

	int dungeon_minimap_dungeoninfo_button(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
			return 0;

		CQuestManager & q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();
		
		if (pDungeon)
			pDungeon->SetMiniMapDungeonInfoButton((DWORD)lua_tonumber(L, 1));
			
		return 0;
	}
#endif
#ifdef __DUNGEON_INFO_ENABLE__
	int dungeon_remove_cooldown(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch)
			return 0;

		if (!lua_isstring(L, 1))
			return 0;

		CDungeonInfoManager::instance().ClearDelay(ch, lua_tostring(L, 1));
		return 0;
	}
#endif
#ifdef ENABLE_DUNGEON_RETURN
	int dungeon_can_return(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!ch)
		{
			lua_pushboolean(L, false);
			return 1;
		}
		
		if (ch->GetDungeon())
		{
			lua_pushboolean(L, false);
			return 1;
		}

		lua_pushboolean(L, CDungeonManager::instance().FindDungeonByPID(ch->GetPlayerID()) ? true : false);
		return 1;
	}

	int dungeon_return(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!ch)
			return 0;

		LPDUNGEON dng = CDungeonManager::instance().FindDungeonByPID(ch->GetPlayerID());

		if (!dng)
			return 0;

		int32_t x, y, map_index;
		std::tie(x, y, map_index) = dng->GetDungeonCoords(ch);

		ch->WarpSet(x, y, map_index);
		return 0;
	}
	
	int dungeon_clear_saved_position(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!ch)
		{
			lua_pushboolean(L, false);
			return 1;
		}

		LPDUNGEON dng = CDungeonManager::instance().FindDungeonByPID(ch->GetPlayerID());

		if (dng)
			dng->UpdateCoords(ch, true);

		return 1;
	}
#endif

#ifdef __DUNGEON_LIMIT_RATES__
	using namespace DungeonRates;
	int dungeon_rate_limits_can_enter(lua_State* L) {
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!ch) {
			lua_pushboolean(L, false);
			return 0;
		}
	
		int32_t lMapIndex = (int32_t)lua_tonumber(L,1);
		auto CanEnterDungeon = [](LPCHARACTER ch_party, int mapIndex) -> bool {
			return DungeonRates::CanEnterDungeon(ch_party, mapIndex);
		};

		bool bMatched = false;

		if (auto* party = ch->GetParty()) {
			bMatched = party->ForEachOnMapMemberBool(
				[&](LPCHARACTER member) {
					return CanEnterDungeon(member, lMapIndex); // FIXED: was IncrementDungeonCount
				},
				lMapIndex,
				false // Don't check map - process all members
			);
		} else {
			bMatched = CanEnterDungeon(ch, lMapIndex);
		}

		lua_pushboolean(L, bMatched);

		return 1;
	}

	int dungeon_rate_limits_append_count(lua_State* L) {
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!ch) {
			lua_pushboolean(L, false);
			return 0;
		}
	
		int32_t lMapIndex = (int32_t)lua_tonumber(L,1);
		auto IncrementDungeonCount = [](LPCHARACTER ch_party, int mapIndex, uint16_t value) -> bool {
			return DungeonRates::AppendCount(ch_party, mapIndex, value);
		};

		bool bMatched = false;

		if (auto* party = ch->GetParty()) {
			bMatched = party->ForEachOnMapMemberBool(
				[&](LPCHARACTER member) {
					return IncrementDungeonCount(member, lMapIndex, 1);
				},
				lMapIndex,
				false // Don't check map - process all members
			);
		} else {
			bMatched = IncrementDungeonCount(ch, lMapIndex, 1);
		}

		if (!bMatched)
		{
			sys_log(0, "Dungeon join .. due to rate limits we're not able to create dungeon.");
			lua_pushboolean(L, false);
			return 0;
		}

		lua_pushboolean(L, true);
		return 1;
	}
#endif

	int dungeon_fast_destroy(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();
		if (pDungeon)
			pDungeon->SetFastDestroy(true);

		return 1;
	}
	
#ifdef ENABLE_SUMMER_HWID_LIMIT
	// Check if character can enter summer dungeon
	int dungeon_summer_can_enter(lua_State* L) {
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		
		if (!ch) {
			lua_pushboolean(L, false);
			return 1;
		}
		
		if (!lua_isnumber(L, 1)) {
			sys_err("dungeon_summer_can_enter: Invalid dungeon index");
			lua_pushboolean(L, false);
			return 1;
		}
		
		uint32_t dungeonIndex = static_cast<uint32_t>(lua_tonumber(L, 1));
		uint32_t cooldownHours = lua_isnumber(L, 2) ? static_cast<uint32_t>(lua_tonumber(L, 2)) : 3;
		
		bool canEnter = CanAccessSummerDungeon(ch, dungeonIndex, cooldownHours);
		
		lua_pushboolean(L, canEnter);
		return 1;
	}
	
	// Set summer dungeon cooldown
	int dungeon_summer_set_cooldown(lua_State* L) {
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		
		if (!ch) {
			lua_pushboolean(L, false);
			return 1;
		}
		
		if (!lua_isnumber(L, 1)) {
			sys_err("dungeon_summer_set_cooldown: Invalid dungeon index");
			lua_pushboolean(L, false);
			return 1;
		}
		
		uint32_t dungeonIndex = static_cast<uint32_t>(lua_tonumber(L, 1));
		uint32_t cooldownHours = lua_isnumber(L, 2) ? static_cast<uint32_t>(lua_tonumber(L, 2)) : 3;
		
		bool success = SetSummerDungeonCooldown(ch, dungeonIndex, cooldownHours);
		
		lua_pushboolean(L, success);
		return 1;
	}
	
	// Get remaining cooldown time
	int dungeon_summer_get_cooldown(lua_State* L) {
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		
		if (!ch) {
			lua_pushnumber(L, 0);
			return 1;
		}
		
		if (!lua_isnumber(L, 1)) {
			sys_err("dungeon_summer_get_cooldown: Invalid dungeon index");
			lua_pushnumber(L, 0);
			return 1;
		}
		
		uint32_t dungeonIndex = static_cast<uint32_t>(lua_tonumber(L, 1));
		time_t remaining = GetSummerDungeonCooldownRemaining(ch, dungeonIndex);
		
		lua_pushnumber(L, static_cast<lua_Number>(remaining));
		return 1;
	}
	
	// Clear summer dungeon cooldown (admin function)
	int dungeon_summer_clear_cooldown(lua_State* L) {
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		
		if (!ch) {
			lua_pushboolean(L, false);
			return 1;
		}
		
		if (!lua_isnumber(L, 1)) {
			sys_err("dungeon_summer_clear_cooldown: Invalid dungeon index");
			lua_pushboolean(L, false);
			return 1;
		}
		
		uint32_t dungeonIndex = static_cast<uint32_t>(lua_tonumber(L, 1));
		ClearSummerDungeonCooldown(ch, dungeonIndex);
		
		lua_pushboolean(L, true);
		return 1;
	}
#endif

#ifdef __DUNGEON_INFO__
	struct FDungeonInfoUpdate
	{
		void operator () (LPENTITY ent)
		{
			if (true == ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER pChar = static_cast<LPCHARACTER>(ent);
				if (pChar->IsPC())
					pChar->SendDungeonCooldown(0);
			}
		}
	};
	int dungeon_update_info(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();
		if (!pDungeon)
			return 0;
		LPSECTREE_MAP pSectree = SECTREE_MANAGER::instance().GetMap(pDungeon->GetMapIndex());
		if (NULL != pSectree)
		{
			FDungeonInfoUpdate func;
			pSectree->for_each(func);
		}
		return 0;
	}
	struct FDungeonInfoFlagCompleted
	{
		std::string flag;
		int value;
		FDungeonInfoFlagCompleted(const char* szFlag, int _value) : flag(szFlag), value(_value){}
		void operator () (LPENTITY ent)
		{
			if (true == ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER pChar = static_cast<LPCHARACTER>(ent);
				if (pChar->IsPC())
					pChar->SetQuestFlag(flag.c_str(), pChar->GetQuestFlag(flag.c_str()) + value);
			}
		}
	};
	int dungeon_increase_completed(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();
		if (!pDungeon)
			return 0;

		if (!lua_isnumber(L, 1))
			return 0;

		char buf[124];
		snprintf(buf, sizeof(buf), "dungeon.%u_completed", (int)lua_tonumber(L, 1));
		LPSECTREE_MAP pSectree = SECTREE_MANAGER::instance().GetMap(pDungeon->GetMapIndex());
		if (NULL != pSectree)
		{
			FDungeonInfoFlagCompleted func(buf, 1);
			pSectree->for_each(func);
		}
		return 0;
	}
	struct FDungeonInfoFlagFastest
	{
		std::string flag;
		int value;
		FDungeonInfoFlagFastest(const char* szFlag, int _value) : flag(szFlag), value(_value) {}
		void operator () (LPENTITY ent)
		{
			if (true == ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER pChar = static_cast<LPCHARACTER>(ent);
				if (pChar->IsPC())
				{
					const int lastTime = pChar->GetQuestFlag(flag.c_str());
					if (lastTime != 0 && lastTime < value)
						return;
					pChar->SetQuestFlag(flag.c_str(), value);
				}
			}
		}
	};
	int dungeon_set_fastest(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();
		if (!pDungeon)
			return 0;
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2))
			return 0;
		char buf[124];
		snprintf(buf, sizeof(buf), "dungeon.%u_fastest", (int)lua_tonumber(L, 1));
		LPSECTREE_MAP pSectree = SECTREE_MANAGER::instance().GetMap(pDungeon->GetMapIndex());
		if (NULL != pSectree)
		{
			FDungeonInfoFlagFastest func(buf, (int)lua_tonumber(L, 2));
			pSectree->for_each(func);
		}
		return 0;
	}
#endif

	
	void RegisterDungeonFunctionTable()
	{
		luaL_reg dungeon_functions[] =
		{
			{ "join",			dungeon_join		},
			{ "exit",			dungeon_exit		},
			{ "exit_all",		dungeon_exit_all	},
			{ "set_item_group",	dungeon_set_item_group	},
			{ "exit_all_by_item_group",	dungeon_exit_all_by_item_group},
			{ "say_diff_by_item_group",	dungeon_say_diff_by_item_group},
			{ "delete_item_in_item_group_from_all", dungeon_delete_item_in_item_group_from_all},
			{ "purge",			dungeon_purge		},
			{ "kill_all",		dungeon_kill_all	},
			{ "spawn",			dungeon_spawn		},
			{ "spawn_mob",		dungeon_spawn_mob	},
			{ "spawn_mob_dir",	dungeon_spawn_mob_dir	},
			{ "spawn_mob_ac_dir",	dungeon_spawn_mob_ac_dir	},
			{ "spawn_name_mob",	dungeon_spawn_name_mob	},
			{ "spawn_goto_mob",		dungeon_spawn_goto_mob	},
			{ "spawn_group",		dungeon_spawn_group	},
			{ "spawn_unique",		dungeon_spawn_unique	},
			{ "spawn_move_unique",		dungeon_spawn_move_unique},
			{ "spawn_move_group",		dungeon_spawn_move_group},
			{ "spawn_stone_door",		dungeon_spawn_stone_door},
			{ "spawn_wooden_door",		dungeon_spawn_wooden_door},
			{ "purge_unique",		dungeon_purge_unique	},
			{ "purge_area",			dungeon_purge_area	},
			{ "kill_unique",		dungeon_kill_unique	},
#ifdef ENABLE_NEWSTUFF
			{ "kill_area",			dungeon_kill_area	},
#endif
			{ "is_unique_dead",		dungeon_is_unique_dead	},
			{ "unique_get_hp_perc",		dungeon_unique_get_hp_perc},
			{ "unique_set_def_grade",	dungeon_unique_set_def_grade},
			{ "unique_set_immortal",	dungeon_unique_set_immortal},
			{ "unique_set_hp",		dungeon_unique_set_hp	},
			{ "unique_set_maxhp",		dungeon_unique_set_maxhp},
			{ "get_unique_vid",		dungeon_get_unique_vid},
			{ "get_kill_stone_count",	dungeon_get_kill_stone_count},
			{ "get_kill_mob_count",		dungeon_get_kill_mob_count},
			{ "is_use_potion",		dungeon_is_use_potion	},
			{ "revived",			dungeon_revived		},
			{ "set_dest",			dungeon_set_dest	},
			{ "jump_all",			dungeon_jump_all	},
			{ "warp_all",		dungeon_warp_all	},
			{ "new_jump_all",		dungeon_new_jump_all	},
#ifdef ENABLE_D_NJGUILD
			// d.new_jump_guild(map_index, x, y)
			{ "new_jump_all_guild",		dungeon_new_jump_guild	},	// [return nothing]
			{ "new_jump_guild",			dungeon_new_jump_guild	},	// alias
#endif
			{ "new_jump_party",		dungeon_new_jump_party	},
			{ "new_jump",			dungeon_new_jump	},
			{ "regen_file",			dungeon_regen_file	},
			{ "set_regen_file",		dungeon_set_regen_file	},
			{ "clear_regen",		dungeon_clear_regen	},
			{ "set_exit_all_at_eliminate",	dungeon_set_exit_all_at_eliminate},
			{ "set_warp_at_eliminate",	dungeon_set_warp_at_eliminate},
			{ "get_map_index",		dungeon_get_map_index	},
			{ "check_eliminated",		dungeon_check_eliminated},
			{ "exit_all_to_start_position",	dungeon_exit_all_to_start_position },
			{ "count_monster",		dungeon_count_monster	},
			{ "setf",					dungeon_set_flag	},
			{ "getf",					dungeon_get_flag	},
			{ "getf_from_map_index",	dungeon_get_flag_from_map_index	},
			{ "set_unique",			dungeon_set_unique	},
#ifdef NEW_ICEDAMAGE_SYSTEM
			{ "get_damage_from_race",			dungeon_get_damage_from_race	},	// [return lua number]
			{ "get_damage_from_affect",			dungeon_get_damage_from_affect	},	// [return lua table]
			{ "set_damage_from_race",			dungeon_set_damage_from_race	},	// [return nothing]
			{ "set_damage_from_affect",			dungeon_set_damage_from_affect	},	// [return nothing]
			{ "unset_damage_from_race",			dungeon_unset_damage_from_race	},	// [return nothing]
			{ "unset_damage_from_affect",		dungeon_unset_damage_from_affect},	// [return nothing]
			{ "reset_damage_from_race",			dungeon_reset_damage_from_race	},	// [return nothing]
			{ "reset_damage_from_affect",		dungeon_reset_damage_from_affect},	// [return nothing]
#endif
#ifdef ENABLE_NEWSTUFF
			{ "is_available0",					dungeon_is_available0			},	// [return lua boolean]
			{ "is_available",					dungeon_is_available0			},	// alias
#endif
			{ "select",			dungeon_select		},
			{ "find",			dungeon_find		},
			{ "notice",			dungeon_notice		},
			{ "setqf",			dungeon_set_quest_flag	},
			{ "all_near_to",	dungeon_all_near_to	},
			{ "set_warp_location",	dungeon_set_warp_location	},
			{ "setqf2",			dungeon_set_quest_flag2	},
#if defined(__DUNGEON_INFO_SYSTEM__)
			{ "update_ranking", dungeon_update_ranking },
#endif

#ifdef ENABLE_MINIMAP_DUNGEONINFO
			{ "set_minimap_dungeoninfo_status", dungeon_minimap_dungeoninfo_status },
			{ "set_minimap_dungeoninfo_stage", dungeon_minimap_dungeoninfo_stage },
			{ "get_minimap_dungeoninfo_stage", dungeon_minimap_dungeoninfo_get_stage },
			{ "set_minimap_dungeoninfo_timer", dungeon_minimap_dungeoninfo_timer },
			{ "set_minimap_dungeoninfo_gauge", dungeon_minimap_dungeoninfo_gauge },
			{ "set_minimap_dungeoninfo_notice", dungeon_minimap_dungeoninfo_notice },
			{ "set_minimap_dungeoninfo_button", dungeon_minimap_dungeoninfo_button },
#endif
#ifdef ENABLE_DUNGEON_RETURN
			{ "can_return",					dungeon_can_return				},
			{ "return_to_saved_position",	dungeon_return					},
			{ "clear_saved_position",		dungeon_clear_saved_position	},
#endif
#ifdef __DUNGEON_INFO_ENABLE__
			{ "remove_cooldown",		dungeon_remove_cooldown },
#endif

#ifdef __DUNGEON_LIMIT_RATES__
			{ "rate_limit_can_enter", dungeon_rate_limits_can_enter },
			{ "rate_limit_append_count", dungeon_rate_limits_append_count },
#endif
#ifdef ENABLE_SUMMER_HWID_LIMIT
            { "summer_can_enter",        dungeon_summer_can_enter     },    // d.summer_can_enter(dungeon_index, cooldown_hours=3) -> boolean
            { "summer_set_cooldown",     dungeon_summer_set_cooldown  },    // d.summer_set_cooldown(dungeon_index, cooldown_hours=3) -> boolean  
            { "summer_get_cooldown",     dungeon_summer_get_cooldown  },    // d.summer_get_cooldown(dungeon_index) -> number (seconds)
            { "summer_clear_cooldown",   dungeon_summer_clear_cooldown},    // d.summer_clear_cooldown(dungeon_index) -> boolean (admin)
#endif
			{ "fast_destroy", dungeon_fast_destroy },
#ifdef __DUNGEON_INFO__
			{ "update_info",			dungeon_update_info		},
			{ "increase_completed",			dungeon_increase_completed		},
			{ "set_fastest",			dungeon_set_fastest		},
#endif

			{ NULL,				NULL			}
		};

		CQuestManager::instance().AddLuaFunctionTable("d", dungeon_functions);
	}
}
//martysama0134's ceqyqttoaf71vasf9t71218
