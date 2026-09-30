#include "stdafx.h"
#include "utils.h"
#include "config.h"
#include "desc.h"
#include "desc_manager.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "mob_manager.h"
#include "battle.h"
#include "pvp.h"
#include "skill.h"
#include "start_position.h"
#include "profiler.h"
#include "cmd.h"
#include "dungeon.h"
#include "log.h"
#include "unique_item.h"
#include "priv_manager.h"
#include "db.h"
#include "vector.h"
#include "marriage.h"
#include "arena.h"
#include "regen.h"
#include "monarch.h"
#include "exchange.h"
#include "shop_manager.h"
#include "castle.h"
#include "ani.h"
#include "packet.h"
#include "party.h"
#include "affect.h"
#include "guild.h"
#include "guild_manager.h"
#include "questmanager.h"
#include "questlua.h"
#include "threeway_war.h"
#include "BlueDragon.h"
#include "DragonLair.h"
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	#include "offlineshop_manager.h"
#endif

#ifdef __WORLD_BOSS_YUMA__
#include "worldboss.h"
#endif

#ifdef ENABLE_RESP_SYSTEM
#include "resp_manager.h"
#endif
#define ENABLE_EFFECT_PENETRATE
#define ENABLE_NEWEXP_CALCULATION
// #define ENABLE_NO_DAMAGE_QUEST_RUNNING

#include <array>
#include <string>
#include <boost/format.hpp>
#ifdef __PREMIUM_PRIVATE_SHOP__
#include "private_shop.h"
#endif
#ifdef __ENABLE_POLYMORPH_SYSTEM__
	#include "PolySystem.h"
#endif
#ifdef __DUNGEON_INFO_ENABLE__
#include "DungeonInfoManager.hpp"
#endif
#include "ItemToggle.hpp"
#ifdef RANKING_SYSTEM
#include "server_ranking_manager.h"
#include "ranking_manager.h"
#endif
#ifdef ENABLE_PVP_RANKING
#include "pvp_ranking.h"
#endif
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
#include "AchievementTitle.h"
#endif
#include "MonsterSpawner.hpp"
#ifdef _ENABLE_BATTLEPASS_
#include "BattlePassManager.h"
#endif

#include "../../common/VnumHelper.h"
#include "p2p.h"
#include "Controller.hpp"
#ifdef ENABLE_ANTY_CHEAT
float CalcDistance(int a, int b)
{
	return sqrt(pow(a, 2) + pow(b, 2));
}

bool IsGoodWeaponType(BYTE weaponType)
{
	switch(weaponType)
	{
		case WEAPON_SWORD:
		case WEAPON_DAGGER:
		case WEAPON_BOW:
		case WEAPON_TWO_HANDED:
		case WEAPON_BELL:
		case WEAPON_FAN:
			return true;
			break;
		default:
			return false;
	}
}

float GetAttackRangeLimit(LPCHARACTER ch)
{
	if (!ch)
		return 0.0;

	LPITEM weapon = ch->GetWear(WEAR_WEAPON);

	if (!weapon)
		return 220.0;

	if (ch->IsRiding())
		return 750.0;
	
	if (ch->IsPolymorphed())
		return 400.0;

	if (!IsGoodWeaponType(weapon->GetSubType()))
		return 0.0;

	float rangeLimit[6] = {	// 6 bo tak
				350.0,		// WEAPON_SWORD,
				350.0,		// WEAPON_DAGGER,
				1000.0,		// WEAPON_BOW,
				430.0,		// WEAPON_TWO_HANDED,
				350.0,		// WEAPON_BELL,
				350.0,		// WEAPON_FAN,
	};
	return rangeLimit[weapon->GetSubType()];
}




int GetAttackLimit(LPCHARACTER ch)
{
    if (!ch)
        return 1000;
    
    int maxAttackSpeed = 70;
    int curAttackSpeed = MIN(ch->GetPoint(POINT_ATT_SPEED), 170) - 100;
    
    int iMaxLimit[][10][10][10] =
    {
        {
            {{1000},},
        },
        {
            {{300,700},},
            {{380,1340,500},},
            // SURA
            {{450},},            //miecz
            // SZAMAN
            {{520,520},},        //dzwon, wachlarz
            //LIKAN
            {{440},},//szpony
        },
        // WIERZCHOWIEC - ADJUSTED FOR INVISIBLE TIME
        {
            // WOJOWNIK
            {{440,440},},        //miecz, 2reczna
            // NINJA - INCREASED LIMITS TO ACCOUNT FOR 300ms INVISIBLE TIME// NINJA - INCREASED LIMITS TO ACCOUNT FOR 300ms INVISIBLE TIME
			{{400,1350,350},},   // dagger/bow/sword - reduced max limits
            // SURA
            {{450},},            //miecz
            // SZAMAN - INCREASED LIMITS TO PREVENT 280ms ATTACKS
            {{500,720},},
            //LIKAN
            {{440},},//szpony
        },
        // POLI
        {
            {{800},},            //wszystkie klasy
        },
    };    
    
    int iMinLimit[][10][10][10] = 
    {
        // BEZ BRONI
        {
            {{460},},            //wszystkie klasy
        },
        // NORMALNIE
        {
            // WOJOWNIK
            {{245,245},},        //miecz, 2reczna
            // NINJA
            {{150,560,200},},    //sztylety, luk, miecz
            // SURA
            {{260},},            //miecz
            // SZAMAN
            {{200,200},},        //dzwon, wachlarz
            // LIKAN
            {{235},},//SZPONY
        },
        // WIERZCHOWIEC - ADJUSTED MIN LIMITS
        {
            // WOJOWNIK
            {{240,240},},        //miecz, 2reczna
            // NINJA - INCREASED MIN LIMITS TO RESPECT INVISIBLE TIME
            {{250,760,200},},    // dagger/bow/sword - allow 300ms+ attacks
            // SURA
            {{260},},            // increased from 240
            // SZAMAN - INCREASED MIN LIMITS TO PREVENT FAST ATTACKS
            {{240,380},},        // was 400/450, now 450/500 to ensure >400ms minimum even at max attack speed
            // LIKAN
            {{260},},            // increased from 240
        },
        // POLI
        {
            {{450},},//wszystkie_klasy
        },
    };
    
    // ... rest of function remains the same
    LPITEM pkWeapon = ch->GetWear(WEAR_WEAPON);
    int character = ch->GetJob(), type_weapon = 0;
    int limitMin = 0, limitMax = 0;
    
    if (ch->IsPolymorphed())
    {
        limitMin = iMinLimit[3][0][0][0];
        limitMax = iMaxLimit[3][0][0][0];
    }
    else
    {
        if (pkWeapon!=NULL)
        {
            BYTE item_type = pkWeapon->GetSubType();
            BYTE tab_type[10]={0, 0, 1, 1, 0, 1, 0, 0};
            type_weapon=tab_type[item_type];
            if(character == JOB_ASSASSIN && item_type == WEAPON_SWORD)
                type_weapon=2;
            
            ch->IsRiding() ? limitMin = iMinLimit[2][character][0][type_weapon] : limitMin = iMinLimit[1][character][0][type_weapon];
            ch->IsRiding() ? limitMax = iMaxLimit[2][character][0][type_weapon] : limitMax = iMaxLimit[1][character][0][type_weapon];
        }
        else
        {
            limitMin = iMinLimit[0][0][0][0];
            limitMax = iMaxLimit[0][0][0][0];
        }
    }
    
    int diference = limitMax - limitMin;
    int realLimit = limitMax - static_cast <int> (diference*((float)curAttackSpeed/maxAttackSpeed));
    
    if(realLimit <= 0)
        return 1000;
    
    return realLimit;
}
#endif

static DWORD __GetPartyExpNP(const DWORD level)
{
	if (!level || level > PLAYER_EXP_TABLE_MAX)
		return 14000;
	return party_exp_distribute_table[level];
}

static int __GetExpLossPerc(const DWORD level)
{
	if (!level || level > PLAYER_EXP_TABLE_MAX)
		return 1;
	return aiExpLossPercents[level];
}

DWORD AdjustExpByLevel(const LPCHARACTER ch, const DWORD exp)
{
	if (PLAYER_MAX_LEVEL_CONST < ch->GetLevel())
	{
		double ret = 0.95;
		double factor = 0.1;

		for (ssize_t i=0 ; i < ch->GetLevel()-100 ; ++i)
		{
			if ( (i%10) == 0)
				factor /= 2.0;

			ret *= 1.0 - factor;
		}

		ret = ret * static_cast<double>(exp);

		if (ret < 1.0)
			return 1;

		return static_cast<DWORD>(ret);
	}

	return exp;
}

bool CHARACTER::CanBeginFight() const
{
	if (!CanMove())
		return false;

	return m_pointsInstant.position == POS_STANDING && !IsDead() && !IsStun();
}

void CHARACTER::BeginFight(LPCHARACTER pkVictim)
{
	SetVictim(pkVictim);
	SetPosition(POS_FIGHTING);
	SetNextStatePulse(1);
}

bool CHARACTER::CanFight() const
{
	return m_pointsInstant.position >= POS_FIGHTING ? true : false;
}

#ifdef __ENABLE_STEJNE_DMG__
bool CHARACTER::IsStejneDMG() const
{
	bool poskozeni = false;
	if (GetRaceNum() == 61001 || GetRaceNum() == 61002 || GetRaceNum() == 61003 || GetRaceNum() == 61004 || GetRaceNum() == 61005 || GetRaceNum() == 61006 || GetRaceNum() == 61007 ||
		GetRaceNum() == 200200 || GetRaceNum() == 200305 || GetRaceNum() == 200312 || GetRaceNum() == 200313 || GetRaceNum() == 200314 || GetRaceNum() == 200315 || GetRaceNum() == 200301 || 
		GetRaceNum() == 200302 || GetRaceNum() == 200303 || GetRaceNum() == 200304 || GetRaceNum() == 200317 || GetRaceNum() == 202015)
		poskozeni = true;
	
	return poskozeni;
}
#endif

void CHARACTER::CreateFly(BYTE bType, LPCHARACTER pkVictim)
{
	TPacketGCCreateFly packFly;

	packFly.bHeader         = HEADER_GC_CREATE_FLY;
	packFly.bType           = bType;
	packFly.dwStartVID      = GetVID();
	packFly.dwEndVID        = pkVictim->GetVID();

	PacketAround(&packFly, sizeof(TPacketGCCreateFly));
}

void CHARACTER::DistributeSP(LPCHARACTER pkKiller, int iMethod)
{
	if (pkKiller->GetSP() >= pkKiller->GetMaxSP())
		return;

	bool bAttacking = (get_dword_time() - GetLastAttackTime()) < 3000;
	bool bMoving = (get_dword_time() - GetLastMoveTime()) < 3000;

	if (iMethod == 1)
	{
		int num = number(0, 3);

		if (!num)
		{
			int iLvDelta = GetLevel() - pkKiller->GetLevel();
			int iAmount = 0;

			if (iLvDelta >= 5)
				iAmount = 10;
			else if (iLvDelta >= 0)
				iAmount = 6;
			else if (iLvDelta >= -3)
				iAmount = 2;

			if (iAmount != 0)
			{
				iAmount += (iAmount * pkKiller->GetPoint(POINT_SP_REGEN)) / 100;

				if (iAmount >= 11)
					CreateFly(FLY_SP_BIG, pkKiller);
				else if (iAmount >= 7)
					CreateFly(FLY_SP_MEDIUM, pkKiller);
				else
					CreateFly(FLY_SP_SMALL, pkKiller);

				pkKiller->PointChange(POINT_SP, iAmount);
			}
		}
	}
	else
	{
		if (pkKiller->GetJob() == JOB_SHAMAN || (pkKiller->GetJob() == JOB_SURA && pkKiller->GetSkillGroup() == 2))
		{
			int iAmount;

			if (bAttacking)
				iAmount = 2 + GetMaxSP() / 100;
			else if (bMoving)
				iAmount = 3 + GetMaxSP() * 2 / 100;
			else
				iAmount = 10 + GetMaxSP() * 3 / 100;

			iAmount += (iAmount * pkKiller->GetPoint(POINT_SP_REGEN)) / 100;
			pkKiller->PointChange(POINT_SP, iAmount);
		}
		else
		{
			int iAmount;

			if (bAttacking)
				iAmount = 2 + pkKiller->GetMaxSP() / 200;
			else if (bMoving)
				iAmount = 2 + pkKiller->GetMaxSP() / 100;
			else
			{
				if (pkKiller->GetHP() < pkKiller->GetMaxHP())
					iAmount = 2 + (pkKiller->GetMaxSP() / 100);
				else
					iAmount = 9 + (pkKiller->GetMaxSP() / 100);
			}

			iAmount += (iAmount * pkKiller->GetPoint(POINT_SP_REGEN)) / 100;
			pkKiller->PointChange(POINT_SP, iAmount);
		}
	}
}

bool CHARACTER::Attack(LPCHARACTER pkVictim, BYTE bType)
{
	if (test_server && IsPC())
	{
		sys_log(0, "ATTACK_DEBUG: %s attacking %s, type=%d", GetName(), pkVictim->GetName(), bType);
		sys_log(0, "  Attacker pos: (%d, %d)", GetX(), GetY());
		sys_log(0, "  Victim pos: (%d, %d)", pkVictim->GetX(), pkVictim->GetY());

		int distance = DISTANCE_SQRT(GetX() - pkVictim->GetX(), GetY() - pkVictim->GetY());
		sys_log(0, "  Distance: %d", distance);
		sys_log(0, "  CanMove: %s", CanMove() ? "YES" : "NO");
		sys_log(0, "  IsAttackable: %s", battle_is_attackable(this, pkVictim) ? "YES" : "NO");
	}

	if (!CanMove())
		return false;

	// CASTLE
	if (IS_CASTLE_MAP(GetMapIndex()) && false == castle_can_attack(this, pkVictim))
		return false;
	// CASTLE

	// @fixme131
	if (!battle_is_attackable(this, pkVictim))
		return false;

	DWORD dwCurrentTime = get_dword_time();

	if (IsPC())
	{
		if (IS_SPEED_HACK(this, pkVictim, dwCurrentTime))
			return false;

		if (bType == 0 && dwCurrentTime < GetSkipComboAttackByTime())
			return false;
	    auto cheatController = GetCheatController();
	    if (cheatController)
	        cheatController->ReceiveAttackPacket();

	}
	else
	{
		MonsterChat(MONSTER_CHAT_ATTACK);
	}

	// if (pkVictim->CanBeginFight()) {
	// 	//Mob pull detection
	// 	if (IsPC() && pkVictim->IsNPC())
	// 		GetAbuseController()->InitiatedFight(pkVictim->GetXYZ(), pkVictim->IsAggressive());
	// }

	int iRet;

	if (bType == 0)
	{

#ifdef ENABLE_ANTY_CHEAT
		if (IsPC())
		{
			// RANGE LIMITER
			int distanceX = std::abs(lastAttackPosX - pkVictim->GetX());
			int32_t distanceY = std::abs(lastAttackPosY - pkVictim->GetY());
			int32_t distance = CalcDistance(distanceX, distanceY);
			float rangeLimit = GetAttackRangeLimit(this);
			
			//if ( distance > rangeLimit )
			//{
			//	if (test_server)
			//		ChatPacket(CHAT_TYPE_PARTY, "<ANTYCHEAT> attack range limit: %.1f // distance: %.1f", rangeLimit, distance);
			//	return false;
			//}
			// RANGE LIMITER

			int limmit = GetAttackLimit(this) + 200;
			if (!(GetFuncComboLimiterTime() + limmit >= dwCurrentTime))
			{
				if (dwCurrentTime - 5000 > GetFuncComboLimiterTime())
				{
					if (toDisconnectBfuncCombo == false)
					{
						if (test_server)
							ChatPacket(CHAT_TYPE_PARTY, "<ANTYCHEAT> Combo limit time: %d // attack range limit: %.1f // distance: %.1f", dwCurrentTime - 5000 > GetFuncComboLimiterTime(), rangeLimit, distance);
						toDisconnectBfuncCombo = true;
						LogManager::instance().HackLog("WAIT_HACK_L", this);
						ChatPacket(CHAT_TYPE_COMMAND, "quit");
						Disconnect("Shutdown(WAIT_HACK_L)");
					}
				}
				return false;
			}
		}
#endif
		
		switch (GetMobBattleType())
		{
			case BATTLE_TYPE_MELEE:
			case BATTLE_TYPE_POWER:
			case BATTLE_TYPE_TANKER:
			case BATTLE_TYPE_SUPER_POWER:
			case BATTLE_TYPE_SUPER_TANKER:
				iRet = battle_melee_attack(this, pkVictim);
				break;

			case BATTLE_TYPE_RANGE:
				FlyTarget(pkVictim->GetVID(), pkVictim->GetX(), pkVictim->GetY(), HEADER_CG_FLY_TARGETING);
				iRet = Shoot(0) ? BATTLE_DAMAGE : BATTLE_NONE;
				break;

			case BATTLE_TYPE_MAGIC:
				FlyTarget(pkVictim->GetVID(), pkVictim->GetX(), pkVictim->GetY(), HEADER_CG_FLY_TARGETING);
				iRet = Shoot(1) ? BATTLE_DAMAGE : BATTLE_NONE;
				break;

			default:
				sys_err("Unhandled battle type %d", GetMobBattleType());
				iRet = BATTLE_NONE;
				break;
		}
	}
	else
	{
		if (IsPC() == true)
		{
			if (dwCurrentTime - m_dwLastSkillTime > 1500)
			{
				sys_log(1, "HACK: Too long skill using term. Name(%s) PID(%u) delta(%u)",
						GetName(), GetPlayerID(), (dwCurrentTime - m_dwLastSkillTime));
				return false;
			}
		}

		sys_log(1, "Attack call ComputeSkill %d %s", bType, pkVictim?pkVictim->GetName():"");
		iRet = ComputeSkill(bType, pkVictim);
	}

    if (iRet != BATTLE_NONE) {
        pkVictim->SetSyncOwner(this);

        if (pkVictim->CanBeginFight())
            pkVictim->BeginFight(this);
    }


	if (iRet == BATTLE_DAMAGE || iRet == BATTLE_DEAD)
	{
		OnMove(true);
		pkVictim->OnMove();

		// only pc sets victim null. For npc, state machine will reset this.
		if (BATTLE_DEAD == iRet && IsPC())
			SetVictim(NULL);

		return true;
	}

	return false;
}

void CHARACTER::DeathPenalty(BYTE bTown)
{
	sys_log(1, "DEATH_PERNALY_CHECK(%s) town(%d)", GetName(), bTown);

	//CubeClose();
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	CloseAcce();
#endif
#ifdef ENABLE_AURA_SYSTEM
	CloseAura();
#endif
#ifdef __ENABLE_POLYMORPH_SYSTEM__
	CPolymorphMgr::instance().SendClose(this);
#endif

	if (GetLevel() < 10)
	{
		sys_log(0, "NO_DEATH_PENALTY_LESS_LV10(%s)", GetName());
		ChatPacket(CHAT_TYPE_INFO, "[LS;901]");
		return;
	}

   	if (number(0, 2))
	{
		sys_log(0, "NO_DEATH_PENALTY_LUCK(%s)", GetName());
		ChatPacket(CHAT_TYPE_INFO, "[LS;901]");
		return;
	}

	if (IS_SET(m_pointsInstant.instant_flag, INSTANT_FLAG_DEATH_PENALTY))
	{
		REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_DEATH_PENALTY);

		// NO_DEATH_PENALTY_BUG_FIX
		if (!bTown)
		{
			if (FindAffect(AFFECT_NO_DEATH_PENALTY))
			{
				sys_log(0, "NO_DEATH_PENALTY_AFFECT(%s)", GetName());
				ChatPacket(CHAT_TYPE_INFO, "[LS;901]");
				RemoveAffect(AFFECT_NO_DEATH_PENALTY);
				return;
			}
		}
		// END_OF_NO_DEATH_PENALTY_BUG_FIX

		int iLoss = ((GetNextExp() * __GetExpLossPerc(GetLevel())) / 100);

		iLoss = MIN(800000, iLoss);

		if (bTown)
			iLoss = 0;

		if (IsEquipUniqueItem(UNIQUE_ITEM_TEARDROP_OF_GODNESS))
			iLoss /= 2;

		sys_log(0, "DEATH_PENALTY(%s) EXP_LOSS: %d percent %d%%", GetName(), iLoss, __GetExpLossPerc(GetLevel()));

		PointChange(POINT_EXP, -iLoss, true);
	}
}

bool CHARACTER::IsStun() const
{
	if (IS_SET(m_pointsInstant.instant_flag, INSTANT_FLAG_STUN))
		return true;

	return false;
}

EVENTFUNC(StunEvent)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "StunEvent> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL) { // <Factor>
		return 0;
	}
	ch->m_pkStunEvent = NULL;
	ch->Dead();
	return 0;
}

void CHARACTER::Stun(bool bJustDie)
{
	if (IsStun())
		return;

	if (IsDead())
		return;

	if (!IsPC() && m_pkParty)
	{
		m_pkParty->SendMessage(this, PM_ATTACKED_BY, 0, 0);
	}

	sys_log(1, "%s: Stun %p", GetName(), this);

	PointChange(POINT_HP_RECOVERY, -GetPoint(POINT_HP_RECOVERY));
	PointChange(POINT_SP_RECOVERY, -GetPoint(POINT_SP_RECOVERY));
	
	CloseMyShop();

	event_cancel(&m_pkRecoveryEvent); // ?? ???? ???.
	
	if(bJustDie)
	{
		SET_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_STUN);
		Dead();
	}
	else
	{
		TPacketGCStun pack;
		pack.header	= HEADER_GC_STUN;
		pack.vid	= m_vid;
		PacketAround(&pack, sizeof(pack));
	
		SET_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_STUN);
	
		if (m_pkStunEvent)
			return;
	
		char_event_info* info = AllocEventInfo<char_event_info>();
	
		info->ch = this;
	
		m_pkStunEvent = event_create(StunEvent, info, PASSES_PER_SEC(3));
	}
}

EVENTINFO(SCharDeadEventInfo)
{
	bool isPC;
	uint32_t dwID;

	SCharDeadEventInfo()
	: isPC(0)
	, dwID(0)
	{
	}
};

EVENTFUNC(dead_event)
{
	const SCharDeadEventInfo* info = dynamic_cast<SCharDeadEventInfo*>(event->info);

	if ( info == NULL )
	{
		sys_err( "dead_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = NULL;

	if (true == info->isPC)
	{
		ch = CHARACTER_MANAGER::instance().FindByPID( info->dwID );
	}
	else
	{
		ch = CHARACTER_MANAGER::instance().Find( info->dwID );
	}

	if (NULL == ch)
	{
		sys_err("DEAD_EVENT: cannot find char pointer with %s id(%d)", info->isPC ? "PC" : "MOB", info->dwID );
		return 0;
	}

	ch->m_pkDeadEvent = NULL;

	if (ch->GetDesc())
	{
		ch->GetDesc()->SetPhase(PHASE_GAME);

		ch->SetPosition(POS_STANDING);

		PIXEL_POSITION pos;

		if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(ch->GetMapIndex(), ch->GetEmpire(), pos))
			if (ch->GetDungeon())
			{
				ch->WarpSet(1075900, 267800);
			} else {
				ch->WarpSet(pos.x, pos.y);
			}
		else
		{
			sys_err("cannot find spawn position (name %s)", ch->GetName());
			ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));
		}

		ch->PointChange(POINT_HP, (ch->GetMaxHP() / 2) - ch->GetHP(), true);

		ch->DeathPenalty(0);

		ch->StartRecoveryEvent();

		ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");
	}
	else
	{
		if (ch->IsMonster() == true)
		{
			if (ch->IsRevive() == false && ch->HasReviverInParty() == true)
			{
				ch->SetPosition(POS_STANDING);
				ch->SetHP(ch->GetMaxHP());

				ch->ViewReencode();

				ch->SetAggressive();
				ch->SetRevive(true);

				return 0;
			}
		}

		M2_DESTROY_CHARACTER(ch);
	}

	return 0;
}

bool CHARACTER::IsDead() const
{
	if (m_pointsInstant.position == POS_DEAD)
		return true;

	return false;
}

#define GetGoldMultipler() (distribution_test_server ? 3 : 1)

void CHARACTER::RewardGold(LPCHARACTER pkAttacker)
{
	// ADD_PREMIUM
	bool isAutoLoot =
		(pkAttacker->GetPremiumRemainSeconds(PREMIUM_AUTOLOOT) > 0 ||
		 pkAttacker->IsEquipUniqueGroup(UNIQUE_GROUP_AUTOLOOT)
#ifdef __ENABLE_AUTO_PICKUP__
		|| NSToggle::PerformPickup(pkAttacker)
#endif
			)
		? true : false;
	// END_OF_ADD_PREMIUM

	PIXEL_POSITION pos = GetXYZ(); // @fixme194 GetMovablePosition is useless here

	int iTotalGold = 0;
	int iGoldPercent = MobRankStats[GetMobRank()].iGoldPercent;

#ifdef ENABLE_EVENT_MANAGER
	if (pkAttacker->IsPC())
	{
		const auto event = CHARACTER_MANAGER::Instance().CheckEventIsActive(YANG_DROP_EVENT, pkAttacker->GetEmpire(), pkAttacker);
		if(event != NULL)
			iGoldPercent = iGoldPercent * (100 + (event->value[0]+CPrivManager::instance().GetPriv(pkAttacker, PRIV_GOLD_DROP))) / 100;
		else
			iGoldPercent = iGoldPercent * (100 + CPrivManager::instance().GetPriv(pkAttacker, PRIV_GOLD_DROP)) / 100;
	}
#else
	if (pkAttacker->IsPC())
		iGoldPercent = iGoldPercent * (100 + CPrivManager::instance().GetPriv(pkAttacker, PRIV_GOLD_DROP)) / 100;
#endif

//	if (pkAttacker->IsPC())
//		iGoldPercent = iGoldPercent * (100 + CPrivManager::instance().GetPriv(pkAttacker, PRIV_GOLD_DROP)) / 100;

	iGoldPercent = iGoldPercent * CHARACTER_MANAGER::instance().GetMobGoldDropRate(pkAttacker) / 100;

	// ADD_PREMIUM
	if (pkAttacker->GetPremiumRemainSeconds(PREMIUM_GOLD) > 0 ||
			pkAttacker->IsEquipUniqueGroup(UNIQUE_GROUP_LUCKY_GOLD))
		iGoldPercent += iGoldPercent;
	// END_OF_ADD_PREMIUM

	if (iGoldPercent > 100)
		iGoldPercent = 100;

	int iPercent;

	if (GetMobRank() >= MOB_RANK_BOSS)
		iPercent = ((iGoldPercent * PERCENT_LVDELTA_BOSS(pkAttacker->GetLevel(), GetLevel())) / 100);
	else
		iPercent = ((iGoldPercent * PERCENT_LVDELTA(pkAttacker->GetLevel(), GetLevel())) / 100);

	if (number(1, 100) > iPercent)
		return;

	int iGoldMultipler = GetGoldMultipler();

	if (1 == number(1, 50000))
		iGoldMultipler *= 10;
	else if (1 == number(1, 10000))
		iGoldMultipler *= 5;

	if (pkAttacker->GetPoint(POINT_GOLD_DOUBLE_BONUS))
		if (number(1, 100) <= pkAttacker->GetPoint(POINT_GOLD_DOUBLE_BONUS))
			iGoldMultipler *= 2;

	if (test_server)
		pkAttacker->ChatPacket(CHAT_TYPE_PARTY, "gold_mul %d rate %d", iGoldMultipler, CHARACTER_MANAGER::instance().GetMobGoldAmountRate(pkAttacker));

	LPITEM item;

	int iGold10DropPct = 100;

#ifdef ENABLE_EVENT_MANAGER
	const auto event = CHARACTER_MANAGER::Instance().CheckEventIsActive(YANG_DROP_EVENT, pkAttacker->GetEmpire(), pkAttacker);
	if(event != NULL)
		iGold10DropPct = (iGold10DropPct * 100) / (100 + event->value[0] + CPrivManager::instance().GetPriv(pkAttacker, PRIV_GOLD10_DROP));
	else
		iGold10DropPct = (iGold10DropPct * 100) / (100 + CPrivManager::instance().GetPriv(pkAttacker, PRIV_GOLD10_DROP));
#else
	iGold10DropPct = (iGold10DropPct * 100) / (100 + CPrivManager::instance().GetPriv(pkAttacker, PRIV_GOLD10_DROP));
#endif

	if (GetMobRank() >= MOB_RANK_BOSS && !IsStone() && GetMobTable().dwGoldMax != 0)
	{
		if (1 == number(1, iGold10DropPct))
			iGoldMultipler *= 10;

		int iSplitCount = number(25, 35);

		for (int i = 0; i < iSplitCount; ++i)
		{
			int iGold = number(GetMobTable().dwGoldMin, GetMobTable().dwGoldMax) / iSplitCount;
			if (test_server)
				sys_log(0, "iGold %d", iGold);
			iGold = iGold * CHARACTER_MANAGER::instance().GetMobGoldAmountRate(pkAttacker) / 100;
			iGold *= iGoldMultipler;

			if (iGold == 0)
				continue;

			if (test_server)
			{
				sys_log(0, "Drop Moeny MobGoldAmountRate %d %d", CHARACTER_MANAGER::instance().GetMobGoldAmountRate(pkAttacker), iGoldMultipler);
				sys_log(0, "Drop Money gold %d GoldMin %d GoldMax %d", iGold, GetMobTable().dwGoldMax, GetMobTable().dwGoldMax);
			}

			if ((item = ITEM_MANAGER::instance().CreateItem(1, iGold)))
			{
				pos.x = GetX() + ((number(-14, 14) + number(-14, 14)) * 23);
				pos.y = GetY() + ((number(-14, 14) + number(-14, 14)) * 23);

				item->AddToGround(GetMapIndex(), pos);
				item->StartDestroyEvent();

				iTotalGold += iGold; // Total gold
			}
		}
	}

	else if (1 == number(1, iGold10DropPct))
	{
		for (int i = 0; i < 10; ++i)
		{
			int iGold = number(GetMobTable().dwGoldMin, GetMobTable().dwGoldMax);
			iGold = iGold * CHARACTER_MANAGER::instance().GetMobGoldAmountRate(pkAttacker) / 100;
			iGold *= iGoldMultipler;

			if (iGold == 0)
				continue;

			if ((item = ITEM_MANAGER::instance().CreateItem(1, iGold)))
			{
				pos.x = GetX() + (number(-7, 7) * 20);
				pos.y = GetY() + (number(-7, 7) * 20);

				item->AddToGround(GetMapIndex(), pos);
				item->StartDestroyEvent();

				iTotalGold += iGold; // Total gold
			}
		}
	}
	else
	{
		int iGold = number(GetMobTable().dwGoldMin, GetMobTable().dwGoldMax);
		iGold = iGold * CHARACTER_MANAGER::instance().GetMobGoldAmountRate(pkAttacker) / 100;
		iGold *= iGoldMultipler;

		int iSplitCount;

		if (iGold >= 3)
			iSplitCount = number(1, 3);
		else if (GetMobRank() >= MOB_RANK_BOSS)
		{
			iSplitCount = number(3, 10);

			if ((iGold / iSplitCount) == 0)
				iSplitCount = 1;
		}
		else
			iSplitCount = 1;

		if (iGold != 0)
		{
			iTotalGold += iGold; // Total gold

			for (int i = 0; i < iSplitCount; ++i)
			{
				if (isAutoLoot)
				{
					pkAttacker->GiveGold(iGold / iSplitCount);
				}
				else if ((item = ITEM_MANAGER::instance().CreateItem(1, iGold / iSplitCount)))
				{
					pos.x = GetX() + (number(-7, 7) * 20);
					pos.y = GetY() + (number(-7, 7) * 20);

					item->AddToGround(GetMapIndex(), pos);
					item->StartDestroyEvent();
				}
			}
		}
	}

	DBManager::instance().SendMoneyLog(MONEY_LOG_MONSTER, GetRaceNum(), iTotalGold);
}

#ifdef ENABLE_KILL_NOTICE
void CHARACTER::CreateKillNotice(const CMob* pkMob, LPCHARACTER pkAttacker) {
	if (!pkMob || !pkAttacker || GetDungeon()) {
        return;
    }
	auto sex = GET_SEX(pkAttacker);
    const auto* attackerName = pkAttacker->GetName();
    auto attackerLevel = pkAttacker->GetLevel();
    auto mobVnum = pkMob->m_table.dwVnum;

	if (sex == 0)
	{    
		BroadcastNotice("[LS;3001;%d;%s;%d;[MN;%d]]" g_bChannel, attackerName, attackerLevel, mobVnum);
	}
	else
	{
		BroadcastNotice("[LS;3002;%d;%s;%d;[MN;%d]]" g_bChannel, attackerName, attackerLevel, mobVnum);
	}
}
#endif

typedef struct {
	LPITEM item;
	bool bAutoPickup;
} item_and_pickup_info;

void CHARACTER::Reward(bool bItemDrop)
{
	if (GetRaceNum() == 5001)
	{
		// @fixme194 BEGIN GetMovablePosition should not return
		PIXEL_POSITION pos = GetXYZ();
		SECTREE_MANAGER::instance().GetMovablePosition(GetMapIndex(), GetX(), GetY(), pos);
		// @fixme194 END

		LPITEM item;
		int iGold = number(GetMobTable().dwGoldMin, GetMobTable().dwGoldMax);
		iGold = iGold * CHARACTER_MANAGER::instance().GetMobGoldAmountRate(NULL) / 100;
		iGold *= GetGoldMultipler();
		int iSplitCount = number(25, 35);

		sys_log(0, "WAEGU Dead gold %d split %d", iGold, iSplitCount);

		for (int i = 1; i <= iSplitCount; ++i)
		{
			if ((item = ITEM_MANAGER::instance().CreateItem(1, iGold / iSplitCount)))
			{
				if (i != 0)
				{
					pos.x = number(-7, 7) * 20;
					pos.y = number(-7, 7) * 20;

					pos.x += GetX();
					pos.y += GetY();
				}

				item->AddToGround(GetMapIndex(), pos);
				item->StartDestroyEvent();
			}
		}
		return;
	}

   	LPCHARACTER pkAttacker = DistributeExp();

#ifdef ENABLE_KILL_EVENT_FIX
	if (!pkAttacker && !(pkAttacker = GetMostAttacked()))
		return;
#else
	if (!pkAttacker)
		return;
#endif

	if (pkAttacker->IsPC())
	{
		// if ((GetLevel() - pkAttacker->GetLevel()) >= -10)
		// {
			// if (pkAttacker->GetRealAlignment() < 0)
			// {
				// if (pkAttacker->IsEquipUniqueItem(UNIQUE_ITEM_FASTER_ALIGNMENT_UP_BY_KILL))
					// pkAttacker->UpdateAlignment(14);
				// else
					// pkAttacker->UpdateAlignment(7);
			// }
			// else
				// pkAttacker->UpdateAlignment(2);
		// }

		pkAttacker->SetQuestNPCID(GetVID());
		quest::CQuestManager::instance().Kill(pkAttacker->GetPlayerID(), GetRaceNum());
		CHARACTER_MANAGER::instance().KillLog(GetRaceNum());

#ifdef ENABLE_KILL_NOTICE
		if (!pkAttacker->GetDungeon()) {
			if (IsBoss() || IsWladca() || IsElitka() || IsLegenda()) {
				const CMob* pkMob = CMobManager::instance().Get(GetRaceNum());
				if (pkMob) {
					CreateKillNotice(pkMob, pkAttacker);
				}
			}
		}
		//}
#endif

		if (!number(0, 9))
		{
			if (pkAttacker->GetPoint(POINT_KILL_HP_RECOVERY))
			{
				int iHP = pkAttacker->GetMaxHP() * pkAttacker->GetPoint(POINT_KILL_HP_RECOVERY) / 100;
				pkAttacker->PointChange(POINT_HP, iHP);
				CreateFly(FLY_HP_SMALL, pkAttacker);
			}

			if (pkAttacker->GetPoint(POINT_KILL_SP_RECOVER))
			{
				int iSP = pkAttacker->GetMaxSP() * pkAttacker->GetPoint(POINT_KILL_SP_RECOVER) / 100;
				pkAttacker->PointChange(POINT_SP, iSP);
				CreateFly(FLY_SP_SMALL, pkAttacker);
			}
		}
	}

	if (!bItemDrop)
		return;
	
#ifdef ENABLE_MULTI_FARM_BLOCK
	if(!pkAttacker->GetRewardStatus())
		return;
#endif

	// @fixme194 BEGIN GetMovablePosition should not return
	PIXEL_POSITION pos = GetXYZ();
	SECTREE_MANAGER::instance().GetMovablePosition(GetMapIndex(), GetX(), GetY(), pos);
	// @fixme194 END

	if (test_server)
		sys_log(0, "Drop money : Attacker %s", pkAttacker->GetName());
	RewardGold(pkAttacker);

	int32_t lMapIndex = GetMapIndex();
	int32_t mX = GetX();
	int32_t mY = GetY();

	static std::vector<LPITEM> s_vec_item;
	static std::vector<LPITEM> s_vec_item_auto_pickup;
	s_vec_item.clear();
	s_vec_item_auto_pickup.clear();
	bool bCreateDropItem = ITEM_MANAGER::instance().CreateDropItem(this, pkAttacker, s_vec_item);

	LPITEM item;
	
	if (pkAttacker && pkAttacker->IsPC() && this->IsSummerMob())
	{
		int doubleLootChance = pkAttacker->GetPoint(POINT_DOUBLE_LOOT_SUMMER);
		if (doubleLootChance > 0 && number(1, 100) <= doubleLootChance)
		{
			std::vector<LPITEM> duplicateItems;
			for (const auto& vItem : s_vec_item)
			{
				LPITEM rewardItem = ITEM_MANAGER::instance().CreateItem(vItem->GetVnum(), vItem->GetCount(), 0, true);
				if (rewardItem) 
					duplicateItems.emplace_back(rewardItem);
			}
			
			for (const auto& dupItem : duplicateItems)
				s_vec_item.emplace_back(dupItem);
			
			std::vector<LPITEM> duplicateAutoItems;
			for (const auto& vItem : s_vec_item_auto_pickup)
			{
				LPITEM rewardItem = ITEM_MANAGER::instance().CreateItem(vItem->GetVnum(), vItem->GetCount(), 0, true);
				if (rewardItem) 
					duplicateAutoItems.emplace_back(rewardItem);
			}
			
			for (const auto& dupItem : duplicateAutoItems)
				s_vec_item_auto_pickup.emplace_back(dupItem);
			
			pkAttacker->ChatPacket(CHAT_TYPE_INFO, "[LS;10090]");
		}
	}

	static const std::unordered_set<DWORD> s_dungeonBossVnums = {
		693, 1093, 2598, 202202, 202208, 202214, 202218, 202224, 202230, 202223
	};
	const bool bIsDungeonBoss = s_dungeonBossVnums.count(GetRaceNum()) > 0;

	if (pkAttacker && pkAttacker->IsPC() && !IsSummerMob())
	{
		// Check if any double drop events are currently active
		bool eventActive = false;
		const BYTE killerEmpire = pkAttacker->GetEmpire();

		// Check for metin double drop event (for stones)
		if (IsStone() && !IsGroupDungeonChest() && !IsSummerMob())
		{
			const TEventManagerData* eventPtr = CHARACTER_MANAGER::Instance().CheckEventIsActive(DOUBLE_METIN_LOOT_EVENT, killerEmpire, pkAttacker);
			if (eventPtr) eventActive = true;
		}
		// Check for boss double drop event (for bosses) - not on dungeon final bosses
		else if (GetMobRank() >= MOB_RANK_BOSS && !IsLegenda() && !IsSummerMob() && !bIsDungeonBoss)
		{
			const TEventManagerData* eventPtr = CHARACTER_MANAGER::Instance().CheckEventIsActive(DOUBLE_BOSS_LOOT_EVENT, killerEmpire, pkAttacker);
			if (eventPtr) eventActive = true;
		}

		// Only apply player double drop if no events are active and not a dungeon final boss
		if (!eventActive && !bIsDungeonBoss)
		{
			int doubleLootChance = pkAttacker->GetPoint(POINT_DOUBLE_LOOT_DROP);
			if (doubleLootChance > 0 && number(1, 100) <= doubleLootChance)
			{
				std::vector<LPITEM> duplicateItems;
				for (const auto& vItem : s_vec_item)
				{
					LPITEM rewardItem = ITEM_MANAGER::instance().CreateItem(vItem->GetVnum(), vItem->GetCount(), 0, true);
					if (rewardItem) 
						duplicateItems.emplace_back(rewardItem);
				}
				
				for (const auto& dupItem : duplicateItems)
					s_vec_item.emplace_back(dupItem);
				
				std::vector<LPITEM> duplicateAutoItems;
				for (const auto& vItem : s_vec_item_auto_pickup)
				{
					LPITEM rewardItem = ITEM_MANAGER::instance().CreateItem(vItem->GetVnum(), vItem->GetCount(), 0, true);
					if (rewardItem) 
						duplicateAutoItems.emplace_back(rewardItem);
				}
				
				for (const auto& dupItem : duplicateAutoItems)
					s_vec_item_auto_pickup.emplace_back(dupItem);
				
				//pkAttacker->ChatPacket(CHAT_TYPE_INFO, "[LS;10090]");
			}
		}
	}
	
#ifdef ENABLE_RENEWAL_DROP
	const bool bIsWBPersonalDrop = (GetRaceNum() == 201101
		|| GetRaceNum() == 201102
		|| GetRaceNum() == 201103
		|| GetRaceNum() == 201100
		|| GetRaceNum() == 201104
		|| GetRaceNum() == 201106
		|| GetRaceNum() == 201105);
#endif

	if (bCreateDropItem
#ifdef ENABLE_RENEWAL_DROP
		|| bIsWBPersonalDrop
#endif
	)
	{
#ifdef ENABLE_RENEWAL_DROP
		if (bIsWBPersonalDrop)
		{
			// ============================================================
			// WORLDBOSS PERSONAL DROP SYSTEM
			// Each player gets N weighted draws from the drop table below.
			// Items with minDmgPct > 0 are only accessible if the player
			// dealt at least that % of total boss damage.
			// Higher damage = more draws + eligibility for rarer items.
			//
			// { vnum, count, weight, minDmgPct }
			//   weight     - relative probability (higher = more common)
			//   minDmgPct  - min % of total damage required (0 = everyone)
			//
			// Draws per damage tier (edit iDrawCount block below):
			//   >= 30% dmg  -> 5 draws   [Elite]
			//   >= 15% dmg  -> 4 draws   [High]
			//   >=  5% dmg  -> 3 draws   [Mid]
			//   >=  1% dmg  -> 2 draws   [Low]
			//   <   1% dmg  -> 1 draw    [Participation]
			// ============================================================

			struct SWBDropEntry
			{
				DWORD vnum;
				int   count;
				int   weight;
				int   minDmgPct; // minimum % of total damage required
			};

			static const SWBDropEntry c_wbDropTable[] = {
				// --- Common (weight 500) ---
				{ 80008,  80, 500,  0 }, // Lump of Gold x80
				{ 80019,   5, 500,  0 }, // Fine Cloth x5
				{ 27992, 100, 500,  0 }, // White Pearl x100
				{ 27993, 100, 500,  0 }, // Blue Pearl x100
				{ 27994, 100, 500,  0 }, // Blood-Red Pearl x100
				{ 90037,   1, 500,  0 }, // Earth Gem x1
				{ 90038,   1, 500,  0 }, // Ice Gem x1
				{ 90039,   1, 500,  0 }, // Earth Gem x1
				{ 90040,   1, 500,  0 }, // Ice Gem x1
				{ 203000, 30, 500,  0 }, // Food

				// --- Uncommon (weight 150) ---
				{ 25045,   5, 150,  0 }, // Mystic Stone 20%
				{ 72348,   5, 150,  0 }, // Time Spiral 75%
				{ 72349,   5, 150,  0 }, // Time Spiral 100%
				{ 40018,   3, 150,  0 }, // Dragon attack +
				{ 50835,   3, 150,  0 }, // Blue Dew +
				{ 50840,   2, 150,  0 }, // Alcohol
				{ 79016,   1, 150,  0 }, // Change Relics

				// --- Rare (weight 40) ---
				{ 50259,  10,  40, 30 }, // Cor Draconis Legendary   (>= 30% dmg)
				{ 23000,   1,  40, 30 }, // Power Gloves+0           (>= 30% dmg)
				{ 34000,   1,  40, 50 }, // Legendary Enhancement    (>= 50% dmg)
				{ 100700,  2,  40, 50 }, // Enchant Alchemy          (>= 50% dmg)
				{ 79015,   1,  40, 50 }, // Add relic                (>= 50% dmg)

				// --- Very Rare (weight 8) ---
				{ 90029,   1,   8, 60 }, // Artifact Chest           (>= 60% dmg)
				{ 203012,  1,   8, 80 }, // Levelup Battlepass       (>= 80% dmg)
				{ 85004,   1,   8, 80 }, // Sash 11-25%              (>= 80% dmg)
			};
			static const int c_nWBDrops = static_cast<int>(sizeof(c_wbDropTable) / sizeof(c_wbDropTable[0]));

			// Destroy any items from mob_drop_item.txt (unused for these bosses)
			for (LPITEM pOld : s_vec_item)
				M2_DESTROY_ITEM(pOld);
			s_vec_item.clear();

			// Build player-damage list
			int64_t iTotalDamage = 0;
			std::vector<std::pair<LPCHARACTER, int64_t>> v_pairs;

			for (auto it = m_map_kDamage.begin(); it != m_map_kDamage.end(); ++it)
			{
				int64_t iDamage = it->second.iTotalDamage;
				if (iDamage <= 0)
					continue;
				LPCHARACTER ch = CHARACTER_MANAGER::instance().Find(it->first);
				if (ch && ch->IsPC())
				{
					v_pairs.push_back(std::make_pair(ch, iDamage));
					iTotalDamage += iDamage;
				}
			}

			if (!v_pairs.empty() && iTotalDamage > 0)
			{
				std::sort(v_pairs.begin(), v_pairs.end(),
					[](const std::pair<LPCHARACTER, int64_t>& a, const std::pair<LPCHARACTER, int64_t>& b) {
						return a.second > b.second;
					}
				);

				for (int i = 0; i < static_cast<int>(v_pairs.size()); ++i)
				{
					LPCHARACTER pPlayer = v_pairs[i].first;
					int64_t     iDamage = v_pairs[i].second;
					int         iDmgPct = static_cast<int>(static_cast<float>(iDamage) / static_cast<float>(iTotalDamage) * 100.0f);

					// Number of weighted draws based on damage tier
					int         iDrawCount;
					const char* szTierName;
					if (iDmgPct >= 30) {
						iDrawCount = 15; szTierName = "Elite";
					} else if (iDmgPct >= 15) {
						iDrawCount = 12; szTierName = "High";
					} else if (iDmgPct >= 5) {
						iDrawCount = 9; szTierName = "Mid";
					} else if (iDmgPct >= 1) {
						iDrawCount = 7;  szTierName = "Low";
					} else {
						iDrawCount = 5;  szTierName = "Participation";
					}

					// Sum weights of entries this player is eligible for
					int iTotalWeight = 0;
					for (int j = 0; j < c_nWBDrops; ++j)
					{
						if (iDmgPct >= c_wbDropTable[j].minDmgPct)
							iTotalWeight += c_wbDropTable[j].weight;
					}

					// Perform iDrawCount weighted draws (with replacement)
					int iItemsGiven = 0;
					if (iTotalWeight > 0)
					{
						for (int d = 0; d < iDrawCount; ++d)
						{
							int iRoll       = number(0, iTotalWeight - 1);
							int iCumulative = 0;
							for (int j = 0; j < c_nWBDrops; ++j)
							{
								if (iDmgPct < c_wbDropTable[j].minDmgPct)
									continue;

								iCumulative += c_wbDropTable[j].weight;
								if (iRoll < iCumulative)
								{
									LPITEM pItem = ITEM_MANAGER::instance().CreateItem(
										c_wbDropTable[j].vnum, c_wbDropTable[j].count);
									if (pItem)
									{
										pPlayer->AutoGiveItem(pItem);
										++iItemsGiven;
									}
									break;
								}
							}
						}
					}
				}
			}
		}
		else
#endif
		if (s_vec_item.size() == 0 && s_vec_item_auto_pickup.size() == 0)
		{
			// no drops
		}
		else if (s_vec_item.size() + s_vec_item_auto_pickup.size() == 1)
		{
			bool bAutoPickup = s_vec_item.size() == 0;
			if (bAutoPickup)
			{
				item = s_vec_item_auto_pickup[0];
				sys_log(0, "DROP_ITEM: %s %d %d from %s is_auto_pickup %d", item->GetName(), pos.x, pos.y, GetName(), bAutoPickup);
				if (item->GetVnum() == 50128)
				{
					pkAttacker->ChatPacket(CHAT_TYPE_COMMAND, "ITEM_QUEST_DROP");
				}
				pkAttacker->AutoGiveItem(item, true);
			}
			else
			{
				item = s_vec_item[0];
				sys_log(0, "DROP_ITEM: %s %d %d from %s is_auto_pickup %d", item->GetName(), pos.x, pos.y, GetName(), bAutoPickup);
			}

			if (!bAutoPickup)
			{
				item->AddToGround(lMapIndex, pos);
				item->SetOwnership(pkAttacker);
				item->StartDestroyEvent(60);

				pos.x = number(-7, 7) * 20;
				pos.y = number(-7, 7) * 20;
				pos.x += mX;
				pos.y += mY;

				if (pkAttacker->GetPremiumRemainSeconds(PREMIUM_PICKUP) > 0 || pkAttacker->IsVIP())
					pkAttacker->PickupItem(item->GetVID(), true);
			}
		}
		else
		{
#ifdef __NEW_DROP_SYSTEM__

			std::random_device random_device;
			std::mt19937 random_gen(random_device());

			auto GetRandomUint64 = [&](std::uint64_t min, std::uint64_t max)
			{
				std::uniform_int_distribution<std::uint64_t> uid(min, max);
				return uid(random_gen);
			};

			// copy all items into one vector
			static std::vector<item_and_pickup_info> s_vec_full_item;

			s_vec_full_item.clear();
			s_vec_full_item.resize(s_vec_item.size() + s_vec_item_auto_pickup.size());
			
			for(int i = 0; i < s_vec_item.size(); ++i)
			{
				s_vec_full_item[ i ].item = s_vec_item[ i ];
				s_vec_full_item[ i ].bAutoPickup = false;
			}

			for(int i = 0; i < s_vec_item_auto_pickup.size(); ++i)
			{
				s_vec_full_item[ s_vec_item.size() + i ].item = s_vec_item_auto_pickup[ i ];
				s_vec_full_item[ s_vec_item.size() + i ].bAutoPickup = true;
			}

			int iItemIdx = s_vec_full_item.size() - 1;

			std::uint64_t totalDamage = 0;
			std::uint64_t highestDamage = 0;

			std::vector<std::pair<LPCHARACTER, std::uint64_t>> v_players{ };

			// Add players to list
			for(auto& damageIt : m_map_kDamage)
			{
				int64_t rawDamage = damageIt.second.iTotalDamage;

#ifdef __FAKE_PC__
				playerDamage += damageIt.second.iTotalFakePCDamage;
#endif

				if(rawDamage <= 0)
					continue;

				uint64_t damageDone = static_cast<uint64_t>(rawDamage);

				LPCHARACTER lpCharacter = CHARACTER_MANAGER::instance().Find(damageIt.first);
			
				if(!lpCharacter)
					continue;

				if(damageDone > highestDamage)
					highestDamage = damageDone;

				totalDamage += damageDone;

				v_players.push_back(std::make_pair(lpCharacter, static_cast<std::uint64_t>(damageDone)));
			}

			std::uint64_t damageTreshold = ( highestDamage * 20 ) / 100;

			sys_log(!test_server, "----------------------------------------------------------------");

			for(auto & player : v_players)
			{
				float damagePercent = ( float( player.second ) / totalDamage ) * 100.f;
				sys_log(!test_server, "NEW_DROP_SYSTEM: Player %s done %.1f%% of damage to the mob.", player.first->GetName(), damagePercent);
			}

			// Used only for logging, can remove after
			float damageTresholdPerc = ( float( damageTreshold ) / totalDamage ) * 100.f;

			sys_log(!test_server, "NEW_DROP_SYSTEM: Minimum damage to get drop: %d (~%.2f%% of total damage)", damageTreshold, damageTresholdPerc);

			v_players.erase( std::remove_if(v_players.begin(), v_players.end(), [&](const std::pair<LPCHARACTER,std::uint64_t>& playerInfo)
			{
				bool remove = ( playerInfo.second < damageTreshold );
				if(remove)
				{
					sys_log(!test_server, "NEW_DROP_SYSTEM: Removed %s from getting drop, too low damage.", playerInfo.first->GetName(), playerInfo.second);
					totalDamage -= playerInfo.second;
				}
				return remove;
			}), v_players.end());

			// Sort by damage
			std::sort(v_players.begin(), v_players.end(), [](const std::pair<LPCHARACTER, std::uint64_t>& player_a, const std::pair<LPCHARACTER, std::uint64_t>& player_b)
			{
				return player_a.second > player_b.second;
			});

			// Create ticket vector
			// LPCHARACTER:range{min,max}
			std::vector<std::pair<LPCHARACTER, std::pair<std::uint64_t, std::uint64_t>>> v_tickets{ };

			std::uint64_t range_start = 1;

			for(auto & player : v_players)
			{
				std::uint64_t start = range_start;
				std::uint64_t end = range_start + player.second - 1;

				v_tickets.push_back(std::make_pair(player.first, std::make_pair(start, end)));

				range_start = end + 1;
			}

			// Used only for logging
			std::vector<std::pair<LPCHARACTER, int>> v_itemdropcount;

			if(!v_tickets.empty())
			{
				while(iItemIdx >= 0)
				{
					item_and_pickup_info& r_info = s_vec_full_item[ iItemIdx-- ];
					item = r_info.item;

					if(!item)
						continue;

					item->AddToGround(lMapIndex, pos);

					std::uint64_t winning_number = GetRandomUint64(1, totalDamage);

					auto winner = std::find_if(v_tickets.begin(), v_tickets.end(), [winning_number](const std::pair<LPCHARACTER, std::pair<std::uint64_t, std::uint64_t>>& ticket_entry)
					{
						return winning_number >= ticket_entry.second.first && winning_number <= ticket_entry.second.second;
					});

					LPCHARACTER ch = ( *winner ).first;
					 
					// LOGGING
					auto it = std::find_if(v_itemdropcount.begin(), v_itemdropcount.end(), [&](const std::pair<LPCHARACTER,int>& itemDropLog)
					{
						return itemDropLog.first == ch;
					});

					if(it != v_itemdropcount.end())
						( *it ).second++;
					else v_itemdropcount.push_back(std::make_pair(ch, 1));
					// ---

#ifndef NO_PARTY_DROP
					if(ch->GetParty())
						ch = ch->GetParty()->GetNextOwnership(ch, mX, mY);
#endif
					item->SetOwnership(ch);

					item->StartDestroyEvent();

					pos.x = number(-7, 7) * 20;
					pos.y = number(-7, 7) * 20;
					pos.x += mX;
					pos.y += mY;

					if(item->GetVnum() == 50128)
					{
						ch->ChatPacket(CHAT_TYPE_COMMAND, "ITEM_QUEST_DROP");
					}

					if (pkAttacker->GetPremiumRemainSeconds(PREMIUM_PICKUP) > 0 || pkAttacker->IsVIP())
						pkAttacker->PickupItem(item->GetVID(), true);
				}
			}

			for(auto & itemDropLog : v_itemdropcount)
				sys_log(!test_server, "NEW_DROP_SYSTEM: Player %s dropped %d items.", itemDropLog.first->GetName(), itemDropLog.second);

			sys_log(!test_server, "----------------------------------------------------------------");

			v_players.clear();
			v_tickets.clear();
			v_itemdropcount.clear();
#else
			// copy all items into one vector
			static std::vector<item_and_pickup_info> s_vec_full_item;
			s_vec_full_item.clear();

			s_vec_full_item.resize(s_vec_item.size() + s_vec_item_auto_pickup.size());
			for (int i = 0; i < s_vec_item.size(); ++i)
			{
				s_vec_full_item[i].item = s_vec_item[i];
				s_vec_full_item[i].bAutoPickup = false;
			}
			for (int i = 0; i < s_vec_item_auto_pickup.size(); ++i)
			{
				s_vec_full_item[s_vec_item.size() + i].item = s_vec_item_auto_pickup[i];
				s_vec_full_item[s_vec_item.size() + i].bAutoPickup = true;
			}
			
			int iItemIdx = s_vec_full_item.size() - 1;

			std::priority_queue<std::pair<std::uint64_t, LPCHARACTER> > pq;

			std::uint64_t total_dam = 0;

			for (TDamageMap::iterator it = m_map_kDamage.begin(); it != m_map_kDamage.end(); ++it)
			{
				uint64_t iDamage = it->second.iTotalDamage;
#ifdef __FAKE_PC__
				iDamage += it->second.iTotalFakePCDamage;
#endif
				if (iDamage > 0)
				{
					LPCHARACTER ch = CHARACTER_MANAGER::instance().Find(it->first);

					if (ch)
					{
						pq.push(std::make_pair(iDamage, ch));
						total_dam += iDamage;
					}
				}
			}

			std::vector<LPCHARACTER> v;

			while (!pq.empty() && (std::uint64_t)(pq.top().first) * 10 >= total_dam)
			{
				v.push_back(pq.top().second);
				pq.pop();
			}

			if (!v.empty())
			{
				std::vector<LPCHARACTER>::iterator it = v.begin();

				while (iItemIdx >= 0)
				{
					item_and_pickup_info& r_info = s_vec_full_item[iItemIdx--];
					item = r_info.item;

					if (!item)
					{
						sys_err("item null in vector idx %d", iItemIdx + 1);
						continue;
					}

					item->AddToGround(lMapIndex, pos);

					LPCHARACTER ch = *it;

#ifndef NO_PARTY_DROP
					if (ch->GetParty())
						ch = ch->GetParty()->GetNextOwnership(ch, mX, mY);
#endif
					++it;

					if (it == v.end())
						it = v.begin();

					item->SetOwnership(ch);

					item->StartDestroyEvent();

					pos.x = number(-7, 7) * 20;
					pos.y = number(-7, 7) * 20;
					pos.x += mX;
					pos.y += mY;
					
					if (item->GetVnum() == 50128)
					{
						//ch->SetItemDropQuest(item->GetVnum());
						ch->ChatPacket(CHAT_TYPE_COMMAND, "ITEM_QUEST_DROP");
					}

					//sys_log(0, "DROP_ITEM: %s %dx %d %d by %s", item->GetName(), item->GetCount(), pos.x, pos.y, mName);
				}
			}
#endif
		}
	}

	m_map_kDamage.clear();
}


class FPartyAlignmentCompute
{
	public:
		FPartyAlignmentCompute(int iAmount, int x, int y)
		{
			m_iAmount = iAmount;
			m_iCount = 0;
			m_iStep = 0;
			m_iKillerX = x;
			m_iKillerY = y;
		}

		void operator () (LPCHARACTER pkChr)
		{
			if (DISTANCE_SQRT(pkChr->GetX() - m_iKillerX, pkChr->GetY() - m_iKillerY) < PARTY_DEFAULT_RANGE)
			{
				if (m_iStep == 0)
				{
					++m_iCount;
				}
				else
				{
					pkChr->UpdateAlignment(m_iAmount / m_iCount);
				}
			}
		}

		int m_iAmount;
		int m_iCount;
		int m_iStep;

		int m_iKillerX;
		int m_iKillerY;
};

void CHARACTER::Dead(LPCHARACTER pkKiller, bool bImmediateDead)
{
	if (IsDead())
		return;
	if (IsPC()) {
		GetAbuseController()->MarkDead();
	}
#ifndef DISABLE_STOP_RIDING_WHEN_DIE
	{
		if (IsHorseRiding())
		{
			StopRiding();
		}
		else if (GetMountVnum())
		{
			RemoveAffect(AFFECT_MOUNT_BONUS);
			m_dwMountVnum = 0;
			UnEquipSpecialRideUniqueItem();

			UpdatePacket();
		}

	}
#endif

	if (!pkKiller && m_dwKillerPID)
		pkKiller = CHARACTER_MANAGER::instance().FindByPID(m_dwKillerPID);

	m_dwKillerPID = 0;

	bool isAgreedPVP = false;
	bool isUnderGuildWar = false;
	bool isDuel = false;
	bool isForked = false;

	if (pkKiller && pkKiller->IsPC())
	{
		if (pkKiller->m_pkChrTarget == this)
			pkKiller->SetTarget(NULL);

		if (!IsPC() && pkKiller->GetDungeon())
			pkKiller->GetDungeon()->IncKillCount(pkKiller, this);

		isAgreedPVP = CPVPManager::instance().Dead(this, pkKiller->GetPlayerID());
		isDuel = CArenaManager::instance().OnDead(pkKiller, this);

#ifdef __SPIN_WHEEL__
		if (IsStone())
		{
			const int newCount = pkKiller->GetQuestFlag("spin_wheel.count") + 1;
			if (newCount <= SPIN_WHEEL_STONE_COUNT)
			{
				pkKiller->SetQuestFlag("spin_wheel.count", newCount);
				pkKiller->ChatPacket(CHAT_TYPE_COMMAND, "SetSpinWheel %d 1", newCount);
			}
		}
#endif
		if (IsPC())
		{
			CGuild * g1 = GetGuild();
			CGuild * g2 = pkKiller->GetGuild();

			if (g1 && g2)
				if (g1->UnderWar(g2->GetID()))
					isUnderGuildWar = true;

			pkKiller->SetQuestNPCID(GetVID());
			quest::CQuestManager::instance().Kill(pkKiller->GetPlayerID(), quest::QUEST_NO_NPC);
			CGuildManager::instance().Kill(pkKiller, this);

#if defined(__BL_KILL_BAR__)
			TPacketGCKillBar kb;
			
			kb.bHeader = HEADER_GC_KILLBAR;
			kb.bKillerRace = static_cast<BYTE>(pkKiller->GetRaceNum());
			LPITEM KillerWeapon = pkKiller->GetWear(WEAR_WEAPON);
			kb.bKillerWeaponType = KillerWeapon ? KillerWeapon->GetSubType() : 255;
			kb.bVictimRace = static_cast<BYTE>(GetRaceNum());

			strlcpy(kb.szKiller, pkKiller->GetName(), sizeof(kb.szKiller));
			strlcpy(kb.szVictim, GetName(), sizeof(kb.szVictim));

			const DESC_MANAGER::DESC_SET& c_set_desc = DESC_MANAGER::instance().GetClientSet();
			for (DESC_MANAGER::DESC_SET::const_iterator it = c_set_desc.begin(); it != c_set_desc.end(); ++it)
			{
				LPDESC d_c = *it;
				if (!d_c)
					continue;

				LPCHARACTER c_c = d_c->GetCharacter();
				if (!c_c)
					continue;

				if (pkKiller->GetMapIndex() != c_c->GetMapIndex())
					continue;

				d_c->Packet(&kb, sizeof(kb));
			}
#endif
		}
	}

#ifdef ENABLE_QUEST_DIE_EVENT
	if (IsPC())
	{
		if (pkKiller)
			SetQuestNPCID(pkKiller->GetVID());
		// quest::CQuestManager::instance().Die(GetPlayerID(), quest::QUEST_NO_NPC);
		quest::CQuestManager::instance().Die(GetPlayerID(), (pkKiller)?pkKiller->GetRaceNum():quest::QUEST_NO_NPC);
	}
#endif

	//CHECK_FORKEDROAD_WAR
	if (IsPC())
	{
		if (CThreeWayWar::instance().IsThreeWayWarMapIndex(GetMapIndex()))
			isForked = true;
	}
	//END_CHECK_FORKEDROAD_WAR

	// CASTLE_SIEGE
	if (IS_CASTLE_MAP(GetMapIndex()))
	{
		if (CASTLE_FROG_VNUM == GetRaceNum())
			castle_frog_die(this, pkKiller);
		else if (castle_is_guard_vnum(GetRaceNum()))
			castle_guard_die(this, pkKiller);
		else if (castle_is_tower_vnum(GetRaceNum()))
			castle_tower_die(this, pkKiller);
	}
	// CASTLE_SIEGE

	if (true == isForked)
	{
		CThreeWayWar::instance().onDead( this, pkKiller );
	}

#ifdef _ENABLE_BATTLEPASS_
	BattlePassManager::Instance().Notify(MISSION_TYPE_KILL, pkKiller, this->GetRaceNum(), 1);
#endif

	SetPosition(POS_DEAD);
#ifdef ENABLE_NO_CLEAR_BUFF_WHEN_MONSTER_KILL
	if (pkKiller && IsPC() && !pkKiller->IsPC())
		ClearAffect(true, true);
	else
		ClearAffect(true);
#else
	ClearAffect(true);
#endif

#ifdef ENABLE_PVP_RANKING
	if (pkKiller && pkKiller->IsPC() && IsPC())
	{
		if (GetMapIndex() == CPvPRanking::instance().GetMapIndex() && quest::CQuestManager::instance().GetEventFlag("pvp_ranking") > 0)
		{
			CPvPRanking::instance().AddKill(pkKiller);
			CPvPRanking::instance().AddDead(this);
		}
	}
#endif

	if (pkKiller && IsPC())
	{
		if (!pkKiller->IsPC())
		{
			if (!isForked)
			{
				sys_log(1, "DEAD: %s %p WITH PENALTY", GetName(), this);
				SET_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_DEATH_PENALTY);
				LogManager::instance().CharLog(this, pkKiller->GetRaceNum(), "DEAD_BY_NPC", pkKiller->GetName());
			}
		}
		else
		{
			sys_log(1, "DEAD_BY_PC: %s %p KILLER %s %p", GetName(), this, pkKiller->GetName(), get_pointer(pkKiller));
			REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_DEATH_PENALTY);
			if (GetEmpire() != pkKiller->GetEmpire())
			{
				int iEP = MIN(GetPoint(POINT_EMPIRE_POINT), pkKiller->GetPoint(POINT_EMPIRE_POINT));

				PointChange(POINT_EMPIRE_POINT, -(iEP / 10));
				pkKiller->PointChange(POINT_EMPIRE_POINT, iEP / 5);

				if (GetPoint(POINT_EMPIRE_POINT) < 10)
				{
				}

				char buf[256];
				snprintf(buf, sizeof(buf),
						"%d %lld %d %s %d %lld %d %s",
						GetEmpire(), GetAlignment(), GetPKMode(), GetName(),
						pkKiller->GetEmpire(), pkKiller->GetAlignment(), pkKiller->GetPKMode(), pkKiller->GetName());

				LogManager::instance().CharLog(this, pkKiller->GetPlayerID(), "DEAD_BY_PC", buf);
			}
			else
			{
				char buf[256];
				snprintf(buf, sizeof(buf),
						"%d %lld %d %s %d %lld %d %s",
						GetEmpire(), GetAlignment(), GetPKMode(), GetName(),
						pkKiller->GetEmpire(), pkKiller->GetAlignment(), pkKiller->GetPKMode(), pkKiller->GetName());

				LogManager::instance().CharLog(this, pkKiller->GetPlayerID(), "DEAD_BY_PC", buf);
			}
		}
	}
	else
	{
		sys_log(1, "DEAD: %s %p", GetName(), this);
		REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_DEATH_PENALTY);
	}

	ClearSync();

	//sys_log(1, "stun cancel %s[%d]", GetName(), (DWORD)GetVID());
	event_cancel(&m_pkStunEvent);

	if (IsPC())
	{
		m_dwLastDeadTime = get_dword_time();
		SetKillerMode(false);
		if (GetDesc())
			GetDesc()->SetPhase(PHASE_DEAD);
	}
	else
	{
#ifdef ENABLE_RESP_SYSTEM
		CRespManager::instance().KillMob(this);
#endif
//		if (abs(pkKiller->GetLevel() - GetLevel()) <= 30)
//		{
//			if (IsStone())
//			{
//#ifdef RANKING_SYSTEM
//				CServerRankingManager::instance().IncServerRankValue(pkKiller, SERVER_RANK_TYPE_STONES);
//				CRankingManager::instance().IncRankValue(pkKiller, RANK_TYPE_STONES, GetRaceNum());
//#endif
//			}
//			else if (GetMobRank() >= MOB_RANK_BOSS)
//			{
//#ifdef RANKING_SYSTEM
//				CServerRankingManager::instance().IncServerRankValue(pkKiller, SERVER_RANK_TYPE_BOSSES);
//				CRankingManager::instance().IncRankValue(pkKiller, RANK_TYPE_BOSSES, GetRaceNum());
//#endif
//			}
//			else
//			{
//#ifdef RANKING_SYSTEM
//				CServerRankingManager::instance().IncServerRankValue(pkKiller, SERVER_RANK_TYPE_MONSTERS);
//				CRankingManager::instance().IncRankValue(pkKiller, RANK_TYPE_MONSTERS, 0);
//#endif
//			}
//		}
		if (!IS_SET(m_pointsInstant.instant_flag, INSTANT_FLAG_NO_REWARD))
		{
			if (!(pkKiller && pkKiller->IsPC() && pkKiller->GetGuild() && pkKiller->GetGuild()->UnderAnyWar(GUILD_WAR_TYPE_FIELD)))
			{
				if (GetMobTable().dwResurrectionVnum)
				{
					// DUNGEON_MONSTER_REBIRTH_BUG_FIX
					LPCHARACTER chResurrect = CHARACTER_MANAGER::instance().SpawnMob(GetMobTable().dwResurrectionVnum, GetMapIndex(), GetX(), GetY(), GetZ(), true, (int) GetRotation());
					if (GetDungeon() && chResurrect)
					{
						chResurrect->SetDungeon(GetDungeon());
					}
					// END_OF_DUNGEON_MONSTER_REBIRTH_BUG_FIX

					Reward(false);
				}
				else if (IsRevive() == true)
				{
					Reward(false);
				}
				else
				{
					Reward(true); // Drops gold, item, etc..
				}
			}
			else
			{
				if (pkKiller->m_dwUnderGuildWarInfoMessageTime < get_dword_time())
				{
					pkKiller->m_dwUnderGuildWarInfoMessageTime = get_dword_time() + 60000;
					pkKiller->ChatPacket(CHAT_TYPE_INFO, "[LS;558]");
				}
			}
		}
	}
	const auto isAchievEventActive = pkKiller ? CHARACTER_MANAGER::Instance().CheckEventIsActive(DOUBLE_ACHIEVEMENT_POINTS, 0, pkKiller) : nullptr;
#ifdef ENABLE_PUNKTY_OSIAGNIEC
	if (!IsPC() && (pkKiller && pkKiller->IsPC()))
	{
		if (GetMobTable().sPktOsiag > 0 &&
			GetLevel() <= (pkKiller->GetLevel() + 30) &&
			GetLevel() >= (pkKiller->GetLevel() - 30))
		{
			short pkt_osiag = GetMobTable().sPktOsiag;

			// If event is active, double the points
			if (isAchievEventActive)
				pkt_osiag *= 2;

			pkKiller->PointChange(POINT_PKT_OSIAG, pkt_osiag, true);
		}
	}
#endif

	// BOSS_KILL_LOG
	if (GetMobRank() >= MOB_RANK_BOSS && pkKiller && pkKiller->IsPC())
	{
		char buf[51];
		snprintf(buf, sizeof(buf), "%d %ld", g_bChannel, pkKiller->GetMapIndex());
		if (IsStone())
			LogManager::instance().CharLog(pkKiller, GetRaceNum(), "STONE_KILL", buf);
		else
			LogManager::instance().CharLog(pkKiller, GetRaceNum(), "BOSS_KILL", buf);
	}
	// END_OF_BOSS_KILL_LOG

#ifndef RENEWAL_DEAD_PACKET
	TPacketGCDead pack;
	pack.header	= HEADER_GC_DEAD;
	pack.vid	= m_vid;
	PacketAround(&pack, sizeof(pack));
#endif

	REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_STUN);

	if (GetDesc() != NULL) {
		itertype(m_list_pkAffect) it = m_list_pkAffect.begin();

		while (it != m_list_pkAffect.end())
			SendAffectAddPacket(GetDesc(), *it++);
	}

	if (isDuel == false)
	{
		if (m_pkDeadEvent)
		{
			sys_log(1, "DEAD_EVENT_CANCEL: %s %p %p", GetName(), this, get_pointer(m_pkDeadEvent));
			event_cancel(&m_pkDeadEvent);
		}

		if (IsStone())
			ClearStone();

		if (GetDungeon())
		{
			GetDungeon()->DeadCharacter(this);
		}

		SCharDeadEventInfo* pEventInfo = AllocEventInfo<SCharDeadEventInfo>();

		if (IsPC())
		{
			pEventInfo->isPC = true;
			pEventInfo->dwID = this->GetPlayerID();

			m_pkDeadEvent = event_create(dead_event, pEventInfo, PASSES_PER_SEC(180));
		}
		else
		{
			pEventInfo->isPC = false;
			pEventInfo->dwID = this->GetVID();

			if (IsRevive() == false && HasReviverInParty() == true)
			{
				m_pkDeadEvent = event_create(dead_event, pEventInfo, bImmediateDead ? 1 : PASSES_PER_SEC(3));
			}
			else
			{
				m_pkDeadEvent = event_create(dead_event, pEventInfo, bImmediateDead ? 1 : PASSES_PER_SEC(10));
			}
		}

		sys_log(1, "DEAD_EVENT_CREATE: %s %p %p", GetName(), this, get_pointer(m_pkDeadEvent));
	}

	if (m_pkExchange)
		m_pkExchange->Cancel();

#ifdef RENEWAL_DEAD_PACKET
	TPacketGCDead pack;
	pack.header	= HEADER_GC_DEAD;
	pack.vid	= m_vid;
	for (BYTE i = REVIVE_TYPE_HERE; i < REVIVE_TYPE_MAX; i++)
		pack.t_d[i] = CalculateDeadTime(i);
	PacketAround(&pack, sizeof(pack));
#endif

	//if (IsCubeOpen())
	//	CubeClose();
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	if (IsPC())
		CloseAcce();
#endif

#ifdef ENABLE_AURA_SYSTEM
	if (IsPC())
		CloseAura();
#endif

	CShopManager::instance().StopShopping(this);
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	COfflineShopManager::instance().StopShopping(this);
#endif

	CloseMyShop();
	CloseSafebox();

#ifdef __PREMIUM_PRIVATE_SHOP__
	ClosePrivateShopPanel(true);
	CloseShopSearch();
	if (GetViewingPrivateShop())
		GetViewingPrivateShop()->RemoveShopViewer(this);
#endif

	
	
	if (IsMonster() && 2493 == GetMobTable().dwVnum)
	{
		if (pkKiller && pkKiller->GetGuild())
			CDragonLairManager::instance().OnDragonDead(this, pkKiller->GetGuild()->GetID());
		else
			sys_err("DragonLair: Dragon killed by nobody");
	}

//#ifdef RANKING_SYSTEM
//	if (IsMonster() && pkKiller && pkKiller->IsPC())
//	{
//		if (GetLevel() > (pkKiller->GetLevel()+30) || GetLevel() < (pkKiller->GetLevel()-30))
//			return;
//		
//		if (GetMobRank() >= MOB_RANK_BOSS)
//		{
//			CServerRankingManager::instance().IncServerRankValue(pkKiller, SERVER_RANK_TYPE_BOSSES);
//			CRankingManager::instance().IncRankValue(pkKiller, RANK_TYPE_BOSSES, GetRaceNum());
//		}
//		else
//		{
//			CServerRankingManager::instance().IncServerRankValue(pkKiller, SERVER_RANK_TYPE_MONSTERS);
//			CRankingManager::instance().IncRankValue(pkKiller, RANK_TYPE_MONSTERS, 0);
//		}
//	}
//	else if (IsStone() && pkKiller && pkKiller->IsPC())
//	{
//			if (GetLevel() > (pkKiller->GetLevel()+30) || GetLevel() < (pkKiller->GetLevel()-30))//+-30lv
//				return;
//			CServerRankingManager::instance().IncServerRankValue(pkKiller, SERVER_RANK_TYPE_STONES);
//			CRankingManager::instance().IncRankValue(pkKiller, RANK_TYPE_STONES, GetRaceNum());
//		}
//	}
//#endif

//#ifdef WEEKLY_RANK_BYLUZER
	if (IsMonster() && pkKiller && pkKiller->IsPC())
	{
		MonsterSpawner::instance().OnMonsterDeath(GetVID());
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
		pkKiller->SetQuestFlag("monster_killed", pkKiller->GetQuestFlag("monster_killed") + 1);
#endif
		if (GetMobTable().dwVnum == 202230 || GetMobTable().dwVnum == 1093 || GetMobTable().dwVnum == 2598 || GetMobTable().dwVnum == 202218 || GetMobTable().dwVnum == 693 || GetMobTable().dwVnum == 202202 || GetMobTable().dwVnum == 202223 ||
		GetMobTable().dwVnum == 202214 || GetMobTable().dwVnum == 202208 || GetMobTable().dwVnum == 202208)
		{
		
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
			const int newBossDungeonsCount = pkKiller->GetQuestFlag("AchievementTitle.killed_dungeon_bosses") + 1;
			pkKiller->SetQuestFlag("AchievementTitle.killed_dungeon_bosses", newBossDungeonsCount);
			
			CTitleAchievementTitle::instance().CheckAndUnlockTitles(pkKiller);
			
			pkKiller->ChatPacket(CHAT_TYPE_COMMAND, "title_progress_killed_dungeon_bosses %d", newBossDungeonsCount);
#endif
			pkKiller->PointChange(POINT_WEEKLY4, 1);
		}
		else if (GetMobRank() >= MOB_RANK_BOSS)
		{
#ifdef _ENABLE_BATTLEPASS_
			BattlePassManager::Instance().Notify(MISSION_TYPE_KILL_BOSS, pkKiller, this->GetRaceNum(), 1);
#endif
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
			const int newBossCount = pkKiller->GetQuestFlag("AchievementTitle.boss_killed") + 1;
			pkKiller->SetQuestFlag("AchievementTitle.boss_killed", newBossCount);
			
			CTitleAchievementTitle::instance().CheckAndUnlockTitles(pkKiller);
			
			pkKiller->ChatPacket(CHAT_TYPE_COMMAND, "title_progress_boss_killed %d", newBossCount);
#endif
#ifdef RANKING_SYSTEM
			CServerRankingManager::instance().IncServerRankValue(pkKiller, SERVER_RANK_TYPE_BOSSES);
			CRankingManager::instance().IncRankValue(pkKiller, RANK_TYPE_BOSSES, GetRaceNum());
#endif
			
			if (!pkKiller->InDungeon()) {
				if (GetMobTable().dwVnum == 691 || GetMobTable().dwVnum == 2091 || GetMobTable().dwVnum == 3913 || GetMobTable().dwVnum == 1901) {
					pkKiller->PointChange(POINT_WEEKLY2, 1);
				} else if (GetMobTable().dwVnum == 3190 || GetMobTable().dwVnum == 3191 || GetMobTable().dwVnum == 8213 || GetMobTable().dwVnum == 8214) {
					pkKiller->PointChange(POINT_WEEKLY2, 1);
				} else if (GetMobTable().dwVnum == 6775 || GetMobTable().dwVnum == 6783 || GetMobTable().dwVnum == 6789 || GetMobTable().dwVnum == 34600 || GetMobTable().dwVnum == 34601) {
					pkKiller->PointChange(POINT_WEEKLY2, 1);
				} else if (GetMobTable().dwVnum == 3390 || GetMobTable().dwVnum == 3391) {
					pkKiller->PointChange(POINT_WEEKLY2, 1);
				} else if (GetMobTable().dwVnum == 201004 || GetMobTable().dwVnum == 201005) {
					pkKiller->PointChange(POINT_WEEKLY2, 1);
				} else if (GetMobTable().dwVnum == 201006 || GetMobTable().dwVnum == 201007) {
					pkKiller->PointChange(POINT_WEEKLY2, 1);
				} else if (GetMobTable().dwVnum == 200305 || GetMobTable().dwVnum == 200312) {
					pkKiller->PointChange(POINT_WEEKLY2, 1);
				}
			}
		}
		else
		{
			if (GetLevel() > (pkKiller->GetLevel()+25) || GetLevel() < (pkKiller->GetLevel()-25))
				return;
				
#ifdef RANKING_SYSTEM
			CServerRankingManager::instance().IncServerRankValue(pkKiller, SERVER_RANK_TYPE_MONSTERS);
			CRankingManager::instance().IncRankValue(pkKiller, RANK_TYPE_MONSTERS, 0);
#endif
			pkKiller->PointChange(POINT_WEEKLY1, 1);
		}
		pkKiller->CheckWeekly();
		
	}
	else if (IsStone() && pkKiller && pkKiller->IsPC())
	{
		// Easter
		//static const auto SPAWN_STONE_LIST = {
		//	8006,
		//	8007,
		//	8008,
		//	8009,
		//	2095,
		//	8013,
		//	8027,
		//	8052,
		//	8053,
		//	8210,
		//	8207,
		//	6798,
		//	8203,
		//	6801,
		//	6799,
		//	6802,
		//	6803,
		//};
		//
		//if (std::find(SPAWN_STONE_LIST.begin(), SPAWN_STONE_LIST.end(), GetRaceNum()) != SPAWN_STONE_LIST.end())
		//{
		//	static const auto SPAWN_STONE_PCT = 20;
		//	if (number(1, 100) <= SPAWN_STONE_PCT)
		//	{
		//		static auto& character_manager = CHARACTER_MANAGER::instance();
		//		character_manager.SpawnMob(61001, GetMapIndex(), GetX(), GetY(), GetZ(), true, static_cast<int>(GetRotation()), true);
		//		if (test_server)
		//			pkKiller->ChatPacket(CHAT_TYPE_INFO, "SpawnMob: name(%s), x(%ld), y(%ld), z(%ld)", GetName(), GetX(), GetY(), GetZ());
		//	}
		//}
		
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
		const int newStonesCount = pkKiller->GetQuestFlag("AchievementTitle.stones_killed") + 1;
		pkKiller->SetQuestFlag("AchievementTitle.stones_killed", newStonesCount);
		
		CTitleAchievementTitle::instance().CheckAndUnlockTitles(pkKiller);
		
		pkKiller->ChatPacket(CHAT_TYPE_COMMAND, "title_progress_stones_killed %d", newStonesCount);
#endif
#ifdef _ENABLE_BATTLEPASS_
		BattlePassManager::Instance().Notify(MISSION_TYPE_KILL_STONES, pkKiller, this->GetRaceNum(), 1);
#endif
#ifdef RANKING_SYSTEM
		CServerRankingManager::instance().IncServerRankValue(pkKiller, SERVER_RANK_TYPE_STONES);
		CRankingManager::instance().IncRankValue(pkKiller, RANK_TYPE_STONES, GetRaceNum());
#endif
		if (!pkKiller->InDungeon() && GetLevel() > 30)
		{
			if (GetLevel() > (pkKiller->GetLevel()+30) || GetLevel() < (pkKiller->GetLevel()-30))//+-20lv
				return;

			if (GetMobTable().dwVnum == 8008 || GetMobTable().dwVnum == 8009 || GetMobTable().dwVnum == 2095 || GetMobTable().dwVnum == 8013 || GetMobTable().dwVnum == 8027) {
				pkKiller->PointChange(POINT_WEEKLY3, 1);
			} else if (GetMobTable().dwVnum == 8052 || GetMobTable().dwVnum == 8053 || GetMobTable().dwVnum == 8210 || GetMobTable().dwVnum == 8207) {
				pkKiller->PointChange(POINT_WEEKLY3, 1);
			} else if (GetMobTable().dwVnum == 6798 || GetMobTable().dwVnum == 8203 || GetMobTable().dwVnum == 6801 || GetMobTable().dwVnum == 6799) {
				pkKiller->PointChange(POINT_WEEKLY3, 1);
			} else if (GetMobTable().dwVnum == 6802 || GetMobTable().dwVnum == 6803) {
				pkKiller->PointChange(POINT_WEEKLY3, 1);
			} else if (GetMobTable().dwVnum == 201008 || GetMobTable().dwVnum == 201009) {
				pkKiller->PointChange(POINT_WEEKLY3, 1);
			} else if (GetMobTable().dwVnum == 200306 || GetMobTable().dwVnum == 200307 || GetMobTable().dwVnum == 200308 || GetMobTable().dwVnum == 200309 || GetMobTable().dwVnum == 200310 || GetMobTable().dwVnum == 200311) {
				pkKiller->PointChange(POINT_WEEKLY3, 1);
			}
			pkKiller->CheckWeekly();
		}
	}
//#endif
	
#ifdef __ENABLE_POLYMORPH_SYSTEM__
	if (IsPC() && IsPolyShopping())
	{
		CPolymorphMgr::instance().SendClose(this);
	}
#endif

#ifdef __WORLD_BOSS_YUMA__
	if (true == IsMonster())
	{
		int iBossIndex = CWorldBossManager::instance().IsBoss(this->GetVID());
		if (iBossIndex != -1)
			CWorldBossManager::instance().OnDeathBoss(iBossIndex);
	}
#endif
}

struct FuncSetLastAttacked
{
	FuncSetLastAttacked(DWORD dwTime) : m_dwTime(dwTime)
	{
	}

	void operator () (LPCHARACTER ch)
	{
		ch->SetLastAttacked(m_dwTime);
	}

	DWORD m_dwTime;
};

void CHARACTER::SetLastAttacked(DWORD dwTime)
{
	assert(m_pkMobInst != NULL);

	m_pkMobInst->m_dwLastAttackedTime = dwTime;
	m_pkMobInst->m_posLastAttacked = GetXYZ();
}

void CHARACTER::SendDamagePacket(LPCHARACTER pAttacker, int64_t Damage, BYTE DamageFlag)
{
	if (IsPC() == true || (pAttacker->IsPC() == true && pAttacker->GetTarget() == this))
	{
		TPacketGCDamageInfo damageInfo;
		memset(&damageInfo, 0, sizeof(TPacketGCDamageInfo));

		damageInfo.header = HEADER_GC_DAMAGE_INFO;
		damageInfo.dwVID = (DWORD)GetVID();
		damageInfo.flag = DamageFlag;
		damageInfo.damage = Damage;

		if (GetDesc() != NULL)
		{
			GetDesc()->Packet(&damageInfo, sizeof(TPacketGCDamageInfo));
		}

		if (pAttacker->GetDesc() != NULL)
		{
			pAttacker->GetDesc()->Packet(&damageInfo, sizeof(TPacketGCDamageInfo));
		}
	}
}

//
// Arguments
//
// Return value
//    true		: dead
//    false		: not dead yet
//
bool CHARACTER::Damage(LPCHARACTER pAttacker, int64_t dam, EDamageType type) // returns true if dead
{
	if (pAttacker) {
		LPITEM weapon;
		if (!pAttacker->IsPolymorphed() && pAttacker->IsPC()) {
			if (!(weapon = pAttacker->GetWear(WEAR_WEAPON))) {
				return false;
			}
		}

#ifdef ENABLE_RENEWAL_DROP
		// Worldboss maps: mounted players cannot deal damage to other players
		if (pAttacker->IsPC() && IsPC() && pAttacker->IsRiding())
		{
			long lMap = pAttacker->GetMapIndex();
			for (int i = 0; i < WORLDBOSS_MAP_COUNT; ++i)
			{
				if (lMap == WORLDBOSS_MAP_INDICES[i])
				{
					//pAttacker->ChatPacket(CHAT_TYPE_INFO, "You cannot attack other players while mounted.");
					return false;
				}
			}
		}
#endif

		// legenda
		//if (GetRaceNum() == 6791) {
		//	if (pAttacker->GetLevel() > 174) {
		//		return false;
		//	}
		//}
		//if (GetRaceNum() == 8052) {
		//	if (pAttacker->GetLevel() > 124) {
		//		return false;
		//	}
		//}
		//if (GetRaceNum() == 8053) {
		//	if (pAttacker->GetLevel() > 124) {
		//		return false;
		//	}
		//}
		//if (GetRaceNum() == 3191) {
		//	if (pAttacker->GetLevel() > 124) {
		//		return false;
		//	}
		//}
		//if (GetRaceNum() == 3190) {
		//	if (pAttacker->GetLevel() > 124) {
		//		return false;
		//	}
		//}
		//if (GetRaceNum() == 8210) {
		//	if (pAttacker->GetLevel() > 124) {
		//		return false;
		//	}
		//}
		//if (GetRaceNum() == 8207) {
		//	if (pAttacker->GetLevel() > 124) {
		//		return false;
		//	}
		//}
		//if (GetRaceNum() == 8213) {
		//	if (pAttacker->GetLevel() > 124) {
		//		return false;
		//	}
		//}
		//if (GetRaceNum() == 8214) {
		//	if (pAttacker->GetLevel() > 124) {
		//		return false;
		//	}
		//}
		//if (GetRaceNum() == 2771) {
		//	if (pAttacker->GetLevel() > 124) {
		//		return false;
		//	}
		//}
		//if (GetRaceNum() == 2810) {
		//	if (pAttacker->GetLevel() > 149) {
		//		return false;
		//	}
		//}
		//if (GetRaceNum() == 6789) {
		//	if (pAttacker->GetLevel() > 149) {
		//		return false;
		//	}
		//}
		//if (GetRaceNum() == 6783) {
		//	if (pAttacker->GetLevel() > 149) {
		//		return false;
		//	}
		//}
		//if (GetRaceNum() == 8203) {
		//	if (pAttacker->GetLevel() > 149) {
		//		return false;
		//	}
		//}
		//if (GetRaceNum() == 6798) {
		//	if (pAttacker->GetLevel() > 149) {
		//		return false;
		//	}
		//}
		//if (GetRaceNum() == 200306) {
		//	if (pAttacker->GetLevel() > 99) {
		//		return false;
		//	}
		//}
		//if (GetRaceNum() == 200307) {
		//	if (pAttacker->GetLevel() > 124) {
		//		return false;
		//	}
		//}
		//if (GetRaceNum() == 200308) {
		//	if (pAttacker->GetLevel() > 149) {
		//		return false;
		//	}
		//}
		//if (GetRaceNum() == 200309) {
		//	if (pAttacker->GetLevel() > 174) {
		//		return false;
		//	}
		//}
		//if (GetRaceNum() == 200310) {
		//	if (pAttacker->GetLevel() > 200) {
		//		return false;
		//	}
		//}
		//if (GetRaceNum() == 200311) {
		//	if (pAttacker->GetLevel() < 201) {
		//		return false;
		//	}
		//}
		//if (GetRaceNum() == 34600) {
		//	if (pAttacker->GetLevel() > 174) {
		//		return false;
		//	}
		//}
		//if (GetRaceNum() == 34601) {
		//	if (pAttacker->GetLevel() > 174) {
		//		return false;
		//	}
		//}
		//if (GetRaceNum() == 6801) {
		//	if (pAttacker->GetLevel() > 174) {
		//		return false;
		//	}
		//}
		//if (GetRaceNum() == 6799) {
		//	if (pAttacker->GetLevel() > 174) {
		//		return false;
		//	}
		//}
		//if (GetRaceNum() == 2820) {
		//	if (pAttacker->GetLevel() > 174) {
		//		return false;
		//	}
		//}
	// STATEK WIDMO
		//if ((GetRaceNum() == 16155) || (GetRaceNum() == 16177)) {
		//	if (!pAttacker->IsPolymorphed()) {
		//	}
		//		return false;
		//}
		if ((GetRaceNum() == 200314) || (GetRaceNum() == 200315)) {
			if (!pAttacker->IsPolymorphed()) {
				return false;
			}
		}
		if ((GetRaceNum() >= 16163 && GetRaceNum() <= 16167) || (GetRaceNum() >= 16185 && GetRaceNum() <= 16189)) {
			if (!pAttacker->IsRiding()) {
				return false;
			}
		}
		if (GetRaceNum() == 16156 || 
			(GetRaceNum() >= 16168 && GetRaceNum() <= 16173) || 
			GetRaceNum() == 16178 || 
			(GetRaceNum() >= 16190 && GetRaceNum() <= 16195)) {
			if (pAttacker->IsPolymorphed() || pAttacker->IsRiding()) {
				return false;
			}
		}
	}

	if ((GetRaceNum() == 16159) || (GetRaceNum() == 16181)) {
		if (type != DAMAGE_TYPE_MELEE && type != DAMAGE_TYPE_RANGE && type != DAMAGE_TYPE_MAGIC) {
			return false;
		}
	}
//
	
#ifdef ENABLE_NEWSTUFF
	if (pAttacker && IsStone() && pAttacker->IsPC())
	{
		if (GetEmpire() && GetEmpire() == pAttacker->GetEmpire())
		{
			SendDamagePacket(pAttacker, 0, DAMAGE_BLOCK);
			return false;
		}
	}
#endif

	if (pAttacker && DAMAGE_TYPE_MAGIC == type)
		dam = (int)((float)dam * (100 + (pAttacker->GetPoint(POINT_MAGIC_ATT_BONUS_PER) + pAttacker->GetPoint(POINT_MELEE_MAGIC_ATT_BONUS_PER))) / 100.f + 0.5f);

	if (type != DAMAGE_TYPE_NORMAL && type != DAMAGE_TYPE_NORMAL_RANGE)
	{
		if (IsAffectFlag(AFF_TERROR))
		{
			int pct = GetSkillPower(SKILL_TERROR) / 400;

			if (number(1, 100) <= pct)
				return false;
		}
	}

	// blokada tp po otrzymaniu dmg od gracza
	//if (pAttacker && pAttacker->IsPC() && this->IsPC())
	//	this->SetPlayerDamageTime();

#ifdef ENABLE_NO_DAMAGE_QUEST_RUNNING
	if (pAttacker && pAttacker->IsPC())
	{
		auto * pPC = quest::CQuestManager::instance().GetPCForce(pAttacker->GetPlayerID());
		if (pPC->IsRunning())
		{
			if (test_server)
			{
				auto & mgr = quest::CQuestManager::instance();
				sys_err("QUEST There's suspended quest state for %s, can't run new quest state (quest: %s pc: %s)",
						pAttacker->GetName(),
						pPC->GetCurrentQuestName().c_str(),
						mgr.GetCurrentCharacterPtr() ? mgr.GetCurrentCharacterPtr()->GetName() : "<none>");
			}
			pAttacker->ChatPacket(CHAT_TYPE_INFO, "You can't deal damage while running quests");
			return false;
		}
	}
#endif

	if (GetRaceNum() == 5001)
	{
		bool bDropMoney = false;

		int iPercent = 0; // @fixme136
		if (GetMaxHP() >= 0)
			iPercent = (GetHP() * 100) / GetMaxHP();

		if (iPercent <= 10 && GetMaxSP() < 5)
		{
			SetMaxSP(5);
			bDropMoney = true;
		}
		else if (iPercent <= 20 && GetMaxSP() < 4)
		{
			SetMaxSP(4);
			bDropMoney = true;
		}
		else if (iPercent <= 40 && GetMaxSP() < 3)
		{
			SetMaxSP(3);
			bDropMoney = true;
		}
		else if (iPercent <= 60 && GetMaxSP() < 2)
		{
			SetMaxSP(2);
			bDropMoney = true;
		}
		else if (iPercent <= 80 && GetMaxSP() < 1)
		{
			SetMaxSP(1);
			bDropMoney = true;
		}

		if (bDropMoney)
		{
			DWORD dwGold = 1000;
			int iSplitCount = number(10, 13);

			sys_log(0, "WAEGU DropGoldOnHit %d times", GetMaxSP());

			for (int i = 1; i <= iSplitCount; ++i)
			{
				PIXEL_POSITION pos;
				LPITEM item;

				if ((item = ITEM_MANAGER::instance().CreateItem(1, dwGold / iSplitCount)))
				{
					if (i != 0)
					{
						pos.x = (number(-14, 14) + number(-14, 14)) * 20;
						pos.y = (number(-14, 14) + number(-14, 14)) * 20;

						pos.x += GetX();
						pos.y += GetY();
					}

					item->AddToGround(GetMapIndex(), pos);
					item->StartDestroyEvent();
				}
			}
		}
	}

	int iCurHP = GetHP();
	int iCurSP = GetSP();

	bool IsCritical = false;
	bool IsPenetrate = false;
	bool IsDeathBlow = false;

#ifdef __ENABLE_DAMAGE_RESTRICTIONS__
	int64_t maxDamage = 0;

	if (pAttacker && pAttacker->IsPC() && (IsMonster() || IsStone()))
	{
		maxDamage = CMobManager::instance().GetDamageRestriction(GetRaceNum(), (type >= DAMAGE_TYPE_MELEE && type <= DAMAGE_TYPE_MAGIC) ? DAMAGE_TYPE_MAGIC : DAMAGE_TYPE_NORMAL, (BYTE)pAttacker->GetLevel());
	}
#endif

	if (type == DAMAGE_TYPE_MELEE || type == DAMAGE_TYPE_RANGE || type == DAMAGE_TYPE_MAGIC)
	{
		if (pAttacker)
		{
			int iCriticalPct = pAttacker->GetPoint(POINT_CRITICAL_PCT);

			if (!IsPC())
				iCriticalPct += pAttacker->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_CRITICAL_BONUS);

			if (iCriticalPct)
			{

				iCriticalPct -= GetPoint(POINT_RESIST_CRITICAL);
				
				iCriticalPct = std::min(iCriticalPct, 100);
				if (number(1, 100) <= iCriticalPct)
				{
					IsCritical = true;
					dam *= 2;
					EffectPacket(SE_CRITICAL);
	
					if (IsAffectFlag(AFF_MANASHIELD))
					{
						RemoveAffect(AFF_MANASHIELD);
					}
				}
			}

			int iPenetratePct = pAttacker->GetPoint(POINT_PENETRATE_PCT);

			if (!IsPC())
				iPenetratePct += pAttacker->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_PENETRATE_BONUS);

			if (iPenetratePct)
			{
				{
					CSkillProto* pkSk = CSkillManager::instance().Get(SKILL_RESIST_PENETRATE);

					if (NULL != pkSk)
					{
						pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_RESIST_PENETRATE) / 100.0f);

						iPenetratePct -= static_cast<int>(pkSk->kPointPoly.Eval());
					}
				}

				if (iPenetratePct >= 10)
				{
					iPenetratePct = 5 + (iPenetratePct - 10) / 4;
				}
				else
				{
					iPenetratePct /= 2;
				}

				iPenetratePct -= GetPoint(POINT_RESIST_PENETRATE);

				if (number(1, 100) <= iPenetratePct)
				{
					IsPenetrate = true;

					if (test_server)
						ChatPacket(CHAT_TYPE_INFO, "[LS;911;%d]", GetPoint(POINT_DEF_GRADE) * (100 + GetPoint(POINT_DEF_BONUS)) / 100);

					dam += GetPoint(POINT_DEF_GRADE) * (100 + GetPoint(POINT_DEF_BONUS)) / 100;

					if (IsAffectFlag(AFF_MANASHIELD))
					{
						RemoveAffect(AFF_MANASHIELD);
					}
#ifdef ENABLE_EFFECT_PENETRATE
					EffectPacket(SE_PENETRATE);
#endif
				}
			}
		}
	}

	else if (type == DAMAGE_TYPE_NORMAL || type == DAMAGE_TYPE_NORMAL_RANGE)
	{
#ifdef ENABLE_ANTY_CHEAT
		if (pAttacker && pAttacker->IsPC())
		{
			auto it = simpleAttackRateCounterMap.find(pAttacker->GetVID());
			DWORD current_time = get_dword_time();
			if (it != simpleAttackRateCounterMap.end())
			{
				DWORD msSinceLastAttack = current_time - it->second;
				DWORD limit = GetAttackLimit(pAttacker);

				if (msSinceLastAttack < limit) 
				{
					if (test_server)
					{
						pAttacker->ChatPacket(CHAT_TYPE_PARTY, "<ANTYCHEAT> msSinceLastAttack=%d limit=%d", msSinceLastAttack, limit);
					}
					return false;
				}
				else
				{
					simpleAttackRateCounterMap[pAttacker->GetVID()] = current_time; 
				}
			}
			else
			{
				simpleAttackRateCounterMap.emplace(pAttacker->GetVID(), current_time);
			}
		}
#endif
		if (type == DAMAGE_TYPE_NORMAL)
		{
			if (GetPoint(POINT_BLOCK) && number(1, 100) <= GetPoint(POINT_BLOCK))
			{
				//if (test_server)
				//{
				//	pAttacker->ChatPacket(CHAT_TYPE_INFO, "[LS;1956;%s;%d]", GetName(), GetPoint(POINT_BLOCK));
				//	ChatPacket(CHAT_TYPE_INFO, "[LS;1956;%s;%d]", GetName(), GetPoint(POINT_BLOCK));
				//}

				SendDamagePacket(pAttacker, 0, DAMAGE_BLOCK);
				return false;
			}
		}
		else if (type == DAMAGE_TYPE_NORMAL_RANGE)
		{
			if (GetPoint(POINT_DODGE) && number(1, 100) <= GetPoint(POINT_DODGE))
			{
				//if (test_server)
				//{
				//	pAttacker->ChatPacket(CHAT_TYPE_INFO, "[LS;1959;%s;%d]", GetName(), GetPoint(POINT_DODGE));
				//	ChatPacket(CHAT_TYPE_INFO, "[LS;1959;%s;%d]", GetName(), GetPoint(POINT_DODGE));
				//}

				SendDamagePacket(pAttacker, 0, DAMAGE_DODGE);
				return false;
			}
		}

		if (IsAffectFlag(AFF_JEONGWIHON))
			dam = (int) (dam * (100 + GetSkillPower(SKILL_JEONGWI) * 25 / 100) / 100);

		if (IsAffectFlag(AFF_TERROR))
			dam = (int) (dam * (95 - GetSkillPower(SKILL_TERROR) / 5) / 100);

		if (IsAffectFlag(AFF_HOSIN))
			dam = dam * (100 - GetPoint(POINT_RESIST_NORMAL_DAMAGE)) / 100;
		
		if (pAttacker)
		{
			if (type == DAMAGE_TYPE_NORMAL)
			{
				if (GetPoint(POINT_REFLECT_MELEE))
				{
					int reflectDamage = dam * GetPoint(POINT_REFLECT_MELEE) / 100;

					if (pAttacker->IsImmune(IMMUNE_REFLECT))
						reflectDamage = int(reflectDamage / 3.0f + 0.5f);

					pAttacker->Damage(this, reflectDamage, DAMAGE_TYPE_SPECIAL);
				}
			}

			int iCriticalPct = pAttacker->GetPoint(POINT_CRITICAL_PCT);

			if (!IsPC())
				iCriticalPct += pAttacker->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_CRITICAL_BONUS);

			if (iCriticalPct)
			{
				iCriticalPct -= GetPoint(POINT_RESIST_CRITICAL);
			
				// Hard cap 200 % crit chance
				iCriticalPct = std::min(iCriticalPct, 200);
			
				if (iCriticalPct > 100)
				{
					int excess = iCriticalPct - 100;
			
					// 1 % crit chance = 0.2 % damage
					// (5 % crit = 1 % damage)
					float bonusMultiplier = 1.0f + (excess * 0.002f);
			
					dam *= bonusMultiplier;
				}
			
				if (number(1, 100) <= std::min(iCriticalPct, 100))
				{
					IsCritical = true;
					dam *= 2;
					EffectPacket(SE_CRITICAL);
				}
			}

			int iPenetratePct = pAttacker->GetPoint(POINT_PENETRATE_PCT);

			if (!IsPC())
				iPenetratePct += pAttacker->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_PENETRATE_BONUS);

			{
				CSkillProto* pkSk = CSkillManager::instance().Get(SKILL_RESIST_PENETRATE);

				if (NULL != pkSk)
				{
					pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_RESIST_PENETRATE) / 100.0f);

					iPenetratePct -= static_cast<int>(pkSk->kPointPoly.Eval());
				}
			}

			if (iPenetratePct)
			{
				iPenetratePct -= GetPoint(POINT_RESIST_PENETRATE);

				if (number(1, 100) <= iPenetratePct)
				{
					IsPenetrate = true;

					if (test_server)
						ChatPacket(CHAT_TYPE_INFO, "[LS;911;%d]", GetPoint(POINT_DEF_GRADE) * (100 + GetPoint(POINT_DEF_BONUS)) / 100);
					dam += GetPoint(POINT_DEF_GRADE) * (100 + GetPoint(POINT_DEF_BONUS)) / 100;
#ifdef ENABLE_EFFECT_PENETRATE
					EffectPacket(SE_PENETRATE);
#endif
				}
			}
			
#ifdef __ENABLE_STEJNE_DMG__
			bool enableStealers = true;
			if(IsNPC() && pAttacker->IsPC())
			{
				if(IsStejneDMG())
				{
					enableStealers = false;
				}
			}
			if(enableStealers)
			{
#endif

			// HP ˝şĆż
				if (pAttacker->GetPoint(POINT_STEAL_HP))
				{
					int pct = 1;
	
					if (number(1, 10) <= pct)
					{

#ifdef __ENABLE_DAMAGE_RESTRICTIONS__
						if (pAttacker->IsPC() && (IsMonster() || IsStone()))
						{
							if (maxDamage > 0 && dam > maxDamage)
								dam = maxDamage; // We need to scale stealed hp also with our damage destritions otherwise we can sometime steal more hp, beacause it changes hp for target.
						}
#endif
						int iHP = MIN(dam, MAX(0, iCurHP)) * pAttacker->GetPoint(POINT_STEAL_HP) / 100;
	
						if (iHP > 0 && GetHP() >= iHP)
						{
							CreateFly(FLY_HP_SMALL, pAttacker);
							pAttacker->PointChange(POINT_HP, iHP);
							PointChange(POINT_HP, -iHP);
						}
					}
				}
	
				// SP ˝şĆż
				if (pAttacker->GetPoint(POINT_STEAL_SP))
				{
					int pct = 1;
	
					if (number(1, 10) <= pct)
					{
						int iCur;
	
						if (IsPC())
							iCur = iCurSP;
						else
							iCur = iCurHP;
	
						int iSP = MIN(dam, MAX(0, iCur)) * pAttacker->GetPoint(POINT_STEAL_SP) / 100;
	
						if (iSP > 0 && iCur >= iSP)
						{
							CreateFly(FLY_SP_SMALL, pAttacker);
							pAttacker->PointChange(POINT_SP, iSP);
	
							if (IsPC())
								PointChange(POINT_SP, -iSP);
						}
					}
				}
#ifdef __ENABLE_STEJNE_DMG__
			}
#endif

			if (pAttacker->GetPoint(POINT_STEAL_GOLD))
			{
				if (number(1, 100) <= pAttacker->GetPoint(POINT_STEAL_GOLD))
				{
					int iAmount = number(1, GetLevel());
					pAttacker->PointChange(POINT_GOLD, iAmount);
					DBManager::instance().SendMoneyLog(MONEY_LOG_MISC, 1, iAmount);
				}
			}

			if (pAttacker->GetPoint(POINT_HIT_HP_RECOVERY) && number(0, 4) > 0)
			{
				int i = ((iCurHP>=0)?MIN(dam, iCurHP):dam) * pAttacker->GetPoint(POINT_HIT_HP_RECOVERY) / 100; //@fixme107

				if (i)
				{
					CreateFly(FLY_HP_SMALL, pAttacker);
					pAttacker->PointChange(POINT_HP, i);
				}
			}

			if (pAttacker->GetPoint(POINT_HIT_SP_RECOVERY) && number(0, 4) > 0)
			{
				int i = ((iCurHP>=0)?MIN(dam, iCurHP):dam) * pAttacker->GetPoint(POINT_HIT_SP_RECOVERY) / 100; //@fixme107

				if (i)
				{
					CreateFly(FLY_SP_SMALL, pAttacker);
					pAttacker->PointChange(POINT_SP, i);
				}
			}

			if (pAttacker->GetPoint(POINT_MANA_BURN_PCT))
			{
				if (number(1, 100) <= pAttacker->GetPoint(POINT_MANA_BURN_PCT))
					PointChange(POINT_SP, -50);
			}
		}
	}

	switch (type)
	{
		case DAMAGE_TYPE_NORMAL:
		case DAMAGE_TYPE_NORMAL_RANGE:
			if (pAttacker)
				if (pAttacker->GetPoint(POINT_NORMAL_HIT_DAMAGE_BONUS))
					dam = dam * (100 + pAttacker->GetPoint(POINT_NORMAL_HIT_DAMAGE_BONUS)) / 100;

			dam = dam * (100 - MIN(99, GetPoint(POINT_NORMAL_HIT_DEFEND_BONUS))) / 100;
			break;

		case DAMAGE_TYPE_MELEE:
		case DAMAGE_TYPE_RANGE:
		case DAMAGE_TYPE_FIRE:
		case DAMAGE_TYPE_ICE:
		case DAMAGE_TYPE_ELEC:
		case DAMAGE_TYPE_MAGIC:
			if (pAttacker)
			{
				int iSkillBonus = pAttacker->GetPoint(POINT_SKILL_DAMAGE_BONUS);
				int iPvMSkillBonus = pAttacker->GetPoint(POINT_ATTBONUS_PVM_SKILL);
	
				if (IsPvM())
				{
					// Empowered in PvM
					float fBoostedSkillBonus = iSkillBonus * 1.2f;
					dam = dam * (100.0f + fBoostedSkillBonus + iPvMSkillBonus) / 100.0f;
				}
				else
				{
					// Normal in PvP
					if (iSkillBonus)
						dam = dam * (100 + iSkillBonus) / 100;
				}
			}
	
			// Obrana proti skillům platí vždy
			dam = dam * (100 - MIN(99, GetPoint(POINT_SKILL_DEFEND_BONUS))) / 100;
			break;

		default:
			break;
	}

	if (IsAffectFlag(AFF_MANASHIELD))
	{
		// Dark Protection (skill 79) - damage reduction scales with skill level:
		// Level 1-20: 11% | M1-M10: 11%-21% | G1-P: 25%
		int reductionPct;
		const int masterType = GetSkillMasterType(79);
		const int skillLvl  = GetSkillLevel(79);

		if (masterType >= SKILL_GRAND_MASTER)
		{
			reductionPct = 25;
		}
		else if (masterType == SKILL_MASTER)
		{
			// M1 (level 21) → 11%, M10 (level 30) → 21%
			int masterLevel = MINMAX(1, skillLvl - 20, 10);
			reductionPct = masterLevel + 10;
		}
		else
		{
			reductionPct = 11;
		}

		dam = dam * (100 - reductionPct) / 100;
	}

	if (GetPoint(POINT_MALL_DEFBONUS) > 0)
	{
		int dec_dam = MIN(200, dam * GetPoint(POINT_MALL_DEFBONUS) / 100);
		dam -= dec_dam;
	}

	if (pAttacker)
	{
		if (pAttacker->GetPoint(POINT_MALL_ATTBONUS) > 0)
		{
			int add_dam = MIN(300, dam * pAttacker->GetLimitPoint(POINT_MALL_ATTBONUS) / 100);
			dam += add_dam;
		}

		if (pAttacker->IsPC() && IsPC())
		{
			// if (type == DAMAGE_TYPE_NORMAL)
			// {
			// 	pAttacker->ChatPacket(CHAT_TYPE_INFO, "test");
			// 	pAttacker->ChatPacket(CHAT_TYPE_INFO, "damage: %d", dam / 8);
			// }

			int iEmpire = pAttacker->GetEmpire();
			int32_t lMapIndex = pAttacker->GetMapIndex();
			int iMapEmpire = SECTREE_MANAGER::instance().GetEmpireFromMapIndex(lMapIndex);

			if (iEmpire && iMapEmpire && iEmpire != iMapEmpire)
			{
				dam = dam * 9 / 10;
			}
			
			if (!IsPC() && GetMonsterDrainSPPoint())
			{
				int iDrain = GetMonsterDrainSPPoint();

				if (iDrain <= pAttacker->GetSP())
					pAttacker->PointChange(POINT_SP, -iDrain);
				else
				{
					int iSP = pAttacker->GetSP();
					pAttacker->PointChange(POINT_SP, -iSP);
				}
			}

		}
		else if (pAttacker->IsGuardNPC())
		{
			SET_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_NO_REWARD);
			Stun();
			return true;
		}

		if (pAttacker->IsPC() && CMonarch::instance().IsPowerUp(pAttacker->GetEmpire()))
		{
			dam += dam / 10;
		}

		if (IsPC() && CMonarch::instance().IsDefenceUp(GetEmpire()))
		{
			dam -= dam / 10;
		}
	}

	if (!GetSectree())
		return false;
	
	// Check BANPK attribute with exceptions for special mobs
	if (GetSectree()->IsAttr(GetX(), GetY(), ATTR_BANPK))
	{
		// Allow damage to special non-PvP mobs in BANPK areas
		if (!IsSpecialNonPvpMob())
			return false;
	}

	if (!IsPC())
	{
		if (m_pkParty && m_pkParty->GetLeader())
			m_pkParty->GetLeader()->SetLastAttacked(get_dword_time());
		else
			SetLastAttacked(get_dword_time());

		MonsterChat(MONSTER_CHAT_ATTACKED);
	}

	if (IsStun())
	{
		Dead(pAttacker);
		return true;
	}

	if (IsDead())
		return true;

	/*/
	if (type == DAMAGE_TYPE_POISON)
	{
		if (GetHP() - dam <= 0)
		{
			dam = GetHP() - 1;
		}
	}
#ifdef ENABLE_WOLFMAN_CHARACTER
	else if (type == DAMAGE_TYPE_BLEEDING)
	{
		if (GetHP() - dam <= 0)
		{
			dam = GetHP();
		}
	}
#endif
	/*/
	// ------------------------

	// -----------------------
	// if (pAttacker && pAttacker->IsPC())
	// {
	// 	int iDmgPct = CHARACTER_MANAGER::instance().GetUserDamageRate(pAttacker);
	// 	dam = dam * iDmgPct / 100;
	// }
	if (IsMonster() && IsStoneSkinner())
	{
		
		if (GetHPPct() < GetMobTable().bStoneSkinPoint) {
			dam /= 2;
		}
	}
#ifdef ENABLE_DAMAGE_LIMIT_CONFIG
	if (pAttacker && pAttacker->IsPC() && !IsPC())
		GetDamageLimit(pAttacker->GetMapIndex(), pAttacker->GetLevel(), dam);
#endif

	if (pAttacker)
	{
		if (pAttacker->IsMonster() && pAttacker->IsDeathBlower())
		{
			if (pAttacker->IsDeathBlow())
			{
				if (number(JOB_WARRIOR, JOB_MAX_NUM-1) == GetJob()) // @fixme192 (1, 4)
				{
					IsDeathBlow = true;
					dam = dam * 4;
				}
			}
		}

		if (GetRaceNum() == 2493)
			dam = BlueDragon_Damage(this, pAttacker, dam);

		if (pAttacker->GetPoint(POINT_FINAL_DMG_BONUS) > 0)
		{
			int add_dam = ((dam * (100 + pAttacker->GetPoint(POINT_FINAL_DMG_BONUS))) / 100) - dam;
			dam += add_dam;
		}

#ifdef TAKE_LEGEND_DAMAGE_BOARD_SYSTEM
		if (IsLegenda() || IsElitka())
		{
			int iPulse = thecore_pulse();
			if (!(iPulse - GetLegendLastDamageInfo() < PASSES_PER_SEC(1)))
			{
				SetLegendLastDamageInfo();

				for (auto it = m_map_kDamageLegend.begin(); it != m_map_kDamageLegend.end(); )
				{
					LPCHARACTER ch = CHARACTER_MANAGER::instance().Find(it->second.dwVID);
					if (!ch)
						it = m_map_kDamageLegend.erase(it);
					else
						++it;
				}

				std::vector<std::pair<std::string, TBattleInfoLegend>> vec(m_map_kDamageLegend.begin(), m_map_kDamageLegend.end());

				std::sort(vec.begin(), vec.end(), [](const auto& a, const auto& b) {
					return a.second.iTotalDamage > b.second.iTotalDamage;
					});

				TPacketGCSendLegendDamage damagePacket;
				damagePacket.bHeader = HEADER_GC_LEGEND_DAMAGE_DATA;
				damagePacket.dwBossVID = GetVID();
				(GetVictim()) ? strncpy(damagePacket.cTarget, GetVictim()->GetName(), sizeof(damagePacket.cTarget)) : strncpy(damagePacket.cTarget, "", sizeof(damagePacket.cTarget));
				
				int lRecord = 0;
				for (const auto& pair : vec)
				{
					if (lRecord >= LEGEND_DAMAGE_BOARD_MAX_ENTRIES)
						break;

					damagePacket.legendDamageData[lRecord] = TPacketGCTableLegendDamage();
					strncpy(damagePacket.legendDamageData[lRecord].cName, pair.second.szName, sizeof(damagePacket.legendDamageData[lRecord].cName));
					damagePacket.legendDamageData[lRecord].iLevel = pair.second.iLevel;
					damagePacket.legendDamageData[lRecord].iRace = pair.second.iRace;
					damagePacket.legendDamageData[lRecord].iEmpire = pair.second.iEmpire;
					damagePacket.legendDamageData[lRecord].iDamage = pair.second.iTotalDamage;

					++lRecord;
				}

				if (lRecord < LEGEND_DAMAGE_BOARD_MAX_ENTRIES)
				{
					while (lRecord < LEGEND_DAMAGE_BOARD_MAX_ENTRIES)
					{
						damagePacket.legendDamageData[lRecord] = TPacketGCTableLegendDamage();
						strncpy(damagePacket.legendDamageData[lRecord].cName, "", sizeof(damagePacket.legendDamageData[lRecord].cName));
						damagePacket.legendDamageData[lRecord].iLevel = 0;
						damagePacket.legendDamageData[lRecord].iRace = 0;
						damagePacket.legendDamageData[lRecord].iEmpire = 0;
						damagePacket.legendDamageData[lRecord].iDamage = 0;
						lRecord++;
					}
				}

				PacketAround(&damagePacket, sizeof(damagePacket), this);
			}
		}
#endif

		BYTE damageFlag = 0;

		if (type == DAMAGE_TYPE_POISON)
			damageFlag = DAMAGE_POISON;
#if defined(ENABLE_WOLFMAN_CHARACTER) && !defined(USE_MOB_BLEEDING_AS_POISON)
		else if (type == DAMAGE_TYPE_BLEEDING)
			damageFlag = DAMAGE_BLEEDING;
#elif defined(ENABLE_WOLFMAN_CHARACTER) && defined(USE_MOB_BLEEDING_AS_POISON)
		else if (type == DAMAGE_TYPE_BLEEDING)
			damageFlag = DAMAGE_POISON;
#endif
		else
			damageFlag = DAMAGE_NORMAL;

		if (IsCritical == true)
			damageFlag |= DAMAGE_CRITICAL;

		if (IsPenetrate == true)
			damageFlag |= DAMAGE_PENETRATE;

		float damMul = this->GetDamMul();
		float tempDam = dam;
		dam = tempDam * damMul + 0.5f;

#ifdef __ENABLE_DAMAGE_RESTRICTIONS__
		if (pAttacker->IsPC() && (IsMonster() || IsStone()))
		{
			if (maxDamage > 0 && dam > maxDamage)
				dam = maxDamage; // And there we're scalling the whole damage.
		}
#endif
		
#ifdef __ENABLE_STEJNE_DMG__
		if (IsNPC() && pAttacker->IsPC())
		{
#ifdef __ENABLE_SKILL_DMG_MULTIPLIER__
			if (GetMobRank() == MOB_RANK_BOSS && (DAMAGE_TYPE_MAGIC == type || DAMAGE_TYPE_MELEE == type || DAMAGE_TYPE_RANGE == type))
			{
				if (pAttacker->GetJob() == JOB_WARRIOR && pAttacker->GetSkillGroup() == 1) //Warrior Aura
				{
					dam = dam * 2.66f;
				}
				else if (pAttacker->GetJob() == JOB_WARRIOR && pAttacker->GetSkillGroup() == 2) //Warrior Mental
				{
					dam = dam * 2.15f;
				}
				else if (pAttacker->GetJob() == JOB_ASSASSIN && pAttacker->GetSkillGroup() == 1) //Ninja dagger
				{
					dam = dam * 2.80f;
				}
				else if (pAttacker->GetJob() == JOB_ASSASSIN && pAttacker->GetSkillGroup() == 2) //Ninja bow
				{
					dam = dam * 3.20;
				}
				else if (pAttacker->GetJob() == JOB_SURA && pAttacker->GetSkillGroup() == 2) //Bm sura
				{
					dam = dam * 2.22f;
				}
				else if (pAttacker->GetJob() == JOB_SURA && pAttacker->GetSkillGroup() == 1) //Wp sura
				{
					dam = dam * 3.25f;
				}
				else if (pAttacker->GetJob() == JOB_SHAMAN && pAttacker->GetSkillGroup() == 1) //Shaman dragon
				{
					dam = dam * 2.15f;
				}
				else if (pAttacker->GetJob() == JOB_SHAMAN && pAttacker->GetSkillGroup() == 2) //Shaman heal
				{
					dam = dam * 2.15f;
				}
				else
				{
					dam = dam * 1.65f;
				}
			}
			//if (GetMobRank() == MOB_RANK_KING && (DAMAGE_TYPE_MAGIC == type || DAMAGE_TYPE_MELEE == type || DAMAGE_TYPE_RANGE == type))
			//{
			//	if (pAttacker->GetJob() == JOB_WARRIOR && pAttacker->GetSkillGroup() == 1) //Warrior Aura
			//	{
			//		dam = dam * 2.66f;
			//	}
			//	else if (pAttacker->GetJob() == JOB_WARRIOR && pAttacker->GetSkillGroup() == 2) //Warrior Mental
			//	{
			//		dam = dam * 2.15f;
			//	}
			//	else if (pAttacker->GetJob() == JOB_ASSASSIN && pAttacker->GetSkillGroup() == 1) //Ninja dagger
			//	{
			//		dam = dam * 2.80f;
			//	}
			//	else if (pAttacker->GetJob() == JOB_ASSASSIN && pAttacker->GetSkillGroup() == 2) //Ninja bow
			//	{
			//		dam = dam * 3.40f;
			//	}
			//	else if (pAttacker->GetJob() == JOB_SURA && pAttacker->GetSkillGroup() == 2) //Bm sura
			//	{
			//		dam = dam * 1.72f;
			//	}
			//	else if (pAttacker->GetJob() == JOB_SURA && pAttacker->GetSkillGroup() == 1) //Wp sura
			//	{
			//		dam = dam * 2.45f;
			//	}
			//	else if (pAttacker->GetJob() == JOB_SHAMAN && pAttacker->GetSkillGroup() == 1) //Shaman dragon
			//	{
			//		dam = dam * 1.65f;
			//	}
			//	else if (pAttacker->GetJob() == JOB_SHAMAN && pAttacker->GetSkillGroup() == 2) //Shaman heal
			//	{
			//		dam = dam * 1.38f;
			//	}
			//	else
			//	{
			//		dam = dam * 1.65f;
			//	}
			//}
#endif
//#ifdef __ENABLE_CHARACTER_BOOST__
//			if (DAMAGE_TYPE_NORMAL == type)
//			{
//				if (pAttacker->GetJob() == JOB_WARRIOR && pAttacker->GetSkillGroup() == 1) //Warrior aura
//				{
//					dam = dam * 1.16f;
//				}
//				else if (pAttacker->GetJob() == JOB_WARRIOR && pAttacker->GetSkillGroup() == 2) //Warrior Mental
//				{
//					dam = dam * 1.25f;
//				}
//				else if (pAttacker->GetJob() == JOB_SURA && pAttacker->GetSkillGroup() == 1) //Wp sura
//				{
//					dam = dam * 1.18f;
//				}
//				else if (pAttacker->GetJob() == JOB_SURA && pAttacker->GetSkillGroup() == 2) //Bm sura
//				{
//					dam = dam * 1.25f;
//				}
//				else if (pAttacker->GetJob() == JOB_ASSASSIN) //Ninja
//				{
//					dam = dam * 1.25f;
//				}
//				else if (pAttacker->GetJob() == JOB_SHAMAN) //Shaman
//				{
//					dam = dam * 1.02;
//				}
//			}
//#endif
			if(IsStejneDMG())
			{
				dam = 1;
			}
		}
#endif


		if (pAttacker && pAttacker->IsPC() &&
			(GetMapIndex() == 32 || GetMapIndex() == 50 || InDungeon()))
		{
			DWORD mobVnum = GetRaceNum();
		
			// ======================
			// BOSS LOGIC
			// ======================
			if (GetMobRank() == MOB_RANK_BOSS)
			{
				// Boss 201004 - only normal attacks allowed
				if (mobVnum == 201004 || mobVnum == 201073 || mobVnum == 201076 ||
					mobVnum == 201074 || mobVnum == 201075)
				{
					if (type == DAMAGE_TYPE_MAGIC ||
						type == DAMAGE_TYPE_MELEE ||
						type == DAMAGE_TYPE_RANGE)
					{
						if (test_server)
							pAttacker->ChatPacket(CHAT_TYPE_INFO,
								"This boss can only be damaged with normal attacks!");
		
						SendDamagePacket(pAttacker, 0, DAMAGE_BLOCK);
						return false;
					}
				}
				// Boss 201005 - only skills allowed
				else if (mobVnum == 201005 || mobVnum == 201069 || mobVnum == 201070 ||
						mobVnum == 201071 || mobVnum == 2010722 || mobVnum == 202200 ||
						mobVnum == 202201 || mobVnum == 202202)
				{
					if (type == DAMAGE_TYPE_NORMAL ||
						type == DAMAGE_TYPE_NORMAL_RANGE)
					{
						if (test_server)
							pAttacker->ChatPacket(CHAT_TYPE_INFO,
								"This boss can only be damaged with skills!");
		
						SendDamagePacket(pAttacker, 0, DAMAGE_BLOCK);
						return false;
					}
				}
			}
			else if (IsStone())
			{
				if (mobVnum == 202012 || mobVnum == 202003)
				{
					if (type != DAMAGE_TYPE_NORMAL &&
						type != DAMAGE_TYPE_NORMAL_RANGE)
					{
						if (test_server)
							pAttacker->ChatPacket(CHAT_TYPE_INFO,
								"This stone can only be damaged with skills!");
			
						SendDamagePacket(pAttacker, 0, DAMAGE_BLOCK);
						return false;
					}
				}
			}
			else if (IsMonster())
			{
				if (mobVnum == 202100 || mobVnum == 202101 || mobVnum == 202102)
				{
					if (type != DAMAGE_TYPE_NORMAL &&
						type != DAMAGE_TYPE_NORMAL_RANGE)
					{
						if (test_server)
							pAttacker->ChatPacket(CHAT_TYPE_INFO,
								"This monster can only be damaged with skills!");
			
						SendDamagePacket(pAttacker, 0, DAMAGE_BLOCK);
						return false;
					}
				}
			}
		}

		if (pAttacker)
		{
#ifdef __DUNGEON_INFO__
			if (pAttacker->IsPC() && !IsPC())
			{
				if (m_mapDungeonList.find(GetRaceNum()) != m_mapDungeonList.end())
				{
					char szBuff[254];
					snprintf(szBuff, sizeof(szBuff), "dungeon.%u_damage", GetRaceNum());
					if (pAttacker->GetQuestFlag(szBuff) < dam)
					{
						pAttacker->SetQuestFlag(szBuff, dam);
						pAttacker->SendDungeonCooldown(GetRaceNum());
					}
				}
			}
#endif
			SendDamagePacket(pAttacker, dam, damageFlag);
		}

		if (test_server)
		{
			int iTmpPercent = 0; // @fixme136
			if (GetMaxHP() >= 0)
				iTmpPercent = (GetHP() * 100) / GetMaxHP();

			if(pAttacker)
			{
				pAttacker->ChatPacket(CHAT_TYPE_INFO, "-> %s, DAM %lld HP %lld(%d%%) %s%s",
						GetName(),
						dam,
						GetHP(),
						iTmpPercent,
						IsCritical ? "crit " : "",
						IsPenetrate ? "pene " : "",
						IsDeathBlow ? "deathblow " : "");
			}

			ChatPacket(CHAT_TYPE_PARTY, "<- %s, DAM %lld HP %lld(%d%%) %s%s",
					pAttacker ? pAttacker->GetName() : 0,
					dam,
					GetHP(),
					iTmpPercent,
					IsCritical ? "crit " : "",
					IsPenetrate ? "pene " : "",
					IsDeathBlow ? "deathblow " : "");
		}
	}
	
#ifdef __DUNGEON_INFO_ENABLE__
	if (pAttacker && pAttacker->IsPC() && pAttacker->GetDungeon() && !IsPC())
		CDungeonInfoManager::instance().BroadcastHighscoreRecord(pAttacker, GetMapIndex(), EDIHighScoreTypes::GREATEST_DAMAGE, dam, this);
#endif

	if (!cannot_dead)
	{
#if defined(__DUNGEON_INFO_SYSTEM__)
		if (pAttacker)
			pAttacker->SetLastDamage(dam);
#endif
		if (GetHP() - dam <= 0) // @fixme137
			dam = GetHP();

		PointChange(POINT_HP, -dam, false);
	}
	
	if (IsPC() && pAttacker && pAttacker->IsPC())
	{
		MarkPVPHitReceived(get_dword_time());
		pAttacker->MarkPVPHitDealt(get_dword_time());
	}

	if (pAttacker && dam > 0 && IsNPC())
	{
		TDamageMap::iterator it = m_map_kDamage.find(pAttacker->GetVID());

		if (it == m_map_kDamage.end())
		{
			m_map_kDamage.emplace(pAttacker->GetVID(), TBattleInfo(dam, 0));
			it = m_map_kDamage.find(pAttacker->GetVID());
		}
		else
		{
			it->second.iTotalDamage += dam;
		}

		// Register damage for world boss highscore system
		//MonsterSpawner::instance().OnDamage(pAttacker, this, dam);

		// DungeonUpdateMainBossDmg disabled: client lacks GUILD_SUBHEADER_GC_DUNGEON_BOSS_TOP_DMG handler,
		// causing packet buffer desync and client crash on every hit. Re-enable once client is updated.
		//if (CMobVnumHelper::IsGuildDungeonBoss(GetRaceNum()) && pAttacker->GetGuild() && pAttacker->GetGuild()->GetDungeonMapIndex() == GetMapIndex())
		//	pAttacker->GetGuild()->DungeonUpdateMainBossDmg(pAttacker->GetPlayerID(), dam, GetHP() <= 0);

#ifdef TAKE_LEGEND_DAMAGE_BOARD_SYSTEM
		TDamageMapLegend::iterator itL = m_map_kDamageLegend.find(pAttacker->GetName());
		if (itL == m_map_kDamageLegend.end())
		{
			m_map_kDamageLegend.insert(TDamageMapLegend::value_type(pAttacker->GetName(), TBattleInfoLegend(pAttacker->GetVID(), pAttacker->GetName(), pAttacker->GetLevel(), pAttacker->GetRaceNum(), pAttacker->GetEmpire(), dam, 0)));
			itL = m_map_kDamageLegend.find(pAttacker->GetName());
		}
		else
		{
			itL->second.iTotalDamage += dam;
		}
#endif

		StartRecoveryEvent();
		UpdateAggrPointEx(pAttacker, type, dam, it->second);
	}

	if (GetHP() <= 0)
	{
		if (IsPC()) 
		{
			Stun();
		}
		else
		{
			if (!pAttacker || pAttacker->IsNPC())
			{
				m_dwKillerPID = 0;
		
			}
			else
			{
				m_dwKillerPID = pAttacker->GetPlayerID();
			}
	
			Dead(pAttacker);
		}
	}
	
	return false;
}

void CHARACTER::DistributeHP(LPCHARACTER pkKiller)
{
	if (pkKiller->GetDungeon())
		return;
}

static bool CanGetExp(DWORD pLevel, DWORD pMapIndex)
{
	switch (pMapIndex)
	{
		case 1:
		case 2:
		case 3:
		case 4:
		{
			if (pLevel >= 30)
				return false;
		}
		break;
		
		case 6:
		{
			if (pLevel >= 40)
				return false;
		}
		break;
		
		case 7:
		case 22:
		case 23:
		{
			if (pLevel >= 75)
				return false;
		}
		break;
		
		case 8:
		{
			if (pLevel >= 100)
				return false;
		}
		break;
		
		case 9:
		{
			if (pLevel >= 125)
				return false;
		}
		break;
		
		case 10:
		{
			if (pLevel >= 150)
				return false;
		}
		break;
		
		case 20:
		{
			if (pLevel >= 175)
				return false;
		}
		break;
	}

	return true;
}

#ifdef ENABLE_NEWEXP_CALCULATION
//#define NEW_GET_LVDELTA(me, victim) aiPercentByDeltaLev[MINMAX(0, (victim + 30) - me, MAX_EXP_DELTA_OF_LEV - 1)]
#define NEW_GET_LVDELTA(me, victim)	100
typedef long double rate_t;
static void GiveExp(LPCHARACTER from, LPCHARACTER to, int iExp)
{
	if (test_server && iExp < 0)
	{
		to->ChatPacket(CHAT_TYPE_INFO, "exp(%d) overflow", iExp);
		return;
	}
	
	if (!CanGetExp(to->GetLevel(), to->GetMapIndex()))
	{
		if (test_server) to->ChatPacket(CHAT_TYPE_INFO, "<test_server> blokada expa - zbyt wysoki lvl");
		return;
	}
	
	// decrease/increase exp based on player<>mob level
	rate_t lvFactor = static_cast<rate_t>(NEW_GET_LVDELTA(to->GetLevel(), from->GetLevel())) / 100.0L;
	iExp *= lvFactor;

#ifdef ENABLE_NEW_PET_SYSTEM
	if (to->PetGetLevel() < 40) {
		to->PetSetExp(iExp);
	}
#endif

	// start calculating rate exp bonus
	int iBaseExp = iExp;

	rate_t rateFactor = 100;
	if (distribution_test_server)
		rateFactor *= 3;


#ifdef ENABLE_EVENT_MANAGER
	const auto event = CHARACTER_MANAGER::Instance().CheckEventIsActive(EXP_EVENT, to->GetEmpire(), to);
	if(event != 0)
		rateFactor += event->value[0] + CPrivManager::instance().GetPriv(to, PRIV_EXP_PCT);
	else
		rateFactor += CPrivManager::instance().GetPriv(to, PRIV_EXP_PCT);
#else
	rateFactor += CPrivManager::instance().GetPriv(to, PRIV_EXP_PCT);
#endif

	if (to->IsEquipUniqueItem(UNIQUE_ITEM_LARBOR_MEDAL))
		rateFactor += 20;
	if (to->GetMapIndex() >= 660000 && to->GetMapIndex() < 670000)
		rateFactor += 20;
	if (to->GetPoint(POINT_EXP_DOUBLE_BONUS))
		if (number(1, 100) <= to->GetPoint(POINT_EXP_DOUBLE_BONUS))
			rateFactor += 30;
	if (to->IsEquipUniqueItem(UNIQUE_ITEM_DOUBLE_EXP))
		rateFactor += 50;
	if (to->IsEquipUniqueItem(UNIQUE_ITEM_DOUBLE_EXP2))
		rateFactor += 30;
	if (to->IsEquipUniqueItem(UNIQUE_ITEM_DOUBLE_EXP3))
		rateFactor += 20;
	
#ifdef __VIP_SYSTEM__
	if (to->IsVIP())
		rateFactor += 50;
#endif

	switch (to->GetMountVnum())
	{
		case 20110:
		case 20111:
		case 20112:
		case 20113:
			if (to->IsEquipUniqueItem(71115) || to->IsEquipUniqueItem(71117) || to->IsEquipUniqueItem(71119) ||
					to->IsEquipUniqueItem(71121) )
			{
				rateFactor += 10;
			}
			break;

		case 20114:
		case 20120:
		case 20121:
		case 20122:
		case 20123:
		case 20124:
		case 20125:
			rateFactor += 30;
			break;
	}

	if (to->GetPremiumRemainSeconds(PREMIUM_EXP) > 0)
		rateFactor += 50;
	if (to->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_EXP))
		rateFactor += 50;
	rateFactor += to->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_EXP_BONUS);
	rateFactor += to->GetPoint(POINT_RAMADAN_CANDY_BONUS_EXP);
	rateFactor += to->GetPoint(POINT_MALL_EXPBONUS);
	rateFactor += to->GetPoint(POINT_EXP);
	// useless (never used except for china intoxication) = always 100
	rateFactor = rateFactor * static_cast<rate_t>(CHARACTER_MANAGER::instance().GetMobExpRate(to))/100.0L;
	// fix underflow formula
	iExp = std::max<int>(0, iExp);
	rateFactor = std::max<rate_t>(100.0L, rateFactor);
	// apply calculated rate bonus
	iExp *= (rateFactor/100.0L);
	auto iOldExp = iExp;
	// you can get at maximum only 10% of the total required exp at once (so, you need to kill at least 10 mobs to level up) (useless)
	iExp = MIN(to->GetNextExp() / 10, iExp);
	// it recalculate the given exp if the player level is greater than the exp_table size (useless)
	iExp = AdjustExpByLevel(to, iExp);
	if (test_server)
		to->ChatPacket(CHAT_TYPE_INFO, "base_exp(%d) * rate(%Lf) = exp(%d) => exp+minGNE+adjust(%d)", iBaseExp, rateFactor/100.0L, iOldExp, iExp);
	// set
	to->PointChange(POINT_EXP, iExp, true);
	//from->CreateFly(FLY_EXP, to);

	// marriage
	{
		LPCHARACTER you = to->GetMarryPartner();
		if (you)
		{
			// sometimes, this overflows
			DWORD dwUpdatePoint = (2000.0L/to->GetLevel()/to->GetLevel()/3)*iExp;

			if (to->GetPremiumRemainSeconds(PREMIUM_MARRIAGE_FAST) > 0 ||
					you->GetPremiumRemainSeconds(PREMIUM_MARRIAGE_FAST) > 0)
				dwUpdatePoint *= 3;

			marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(to->GetPlayerID());

			// DIVORCE_NULL_BUG_FIX
			if (pMarriage && pMarriage->IsNear())
				pMarriage->Update(dwUpdatePoint);
			// END_OF_DIVORCE_NULL_BUG_FIX
		}
	}
}
#else
static void GiveExp(LPCHARACTER from, LPCHARACTER to, int iExp)
{
	iExp = CALCULATE_VALUE_LVDELTA(to->GetLevel(), from->GetLevel(), iExp);

	if (distribution_test_server)
		iExp *= 3;

	int iBaseExp = iExp;

#ifdef ENABLE_EVENT_MANAGER
	const auto event = CHARACTER_MANAGER::Instance().CheckEventIsActive(EXP_EVENT, to->GetEmpire(), to);
	if(event != 0)
		iExp = iExp * (100 + (event->value[0] + CPrivManager::instance().GetPriv(to, PRIV_EXP_PCT))) / 100;
	else
		iExp = iExp * (100 + CPrivManager::instance().GetPriv(to, PRIV_EXP_PCT)) / 100;
#else
	iExp = iExp * (100 + CPrivManager::instance().GetPriv(to, PRIV_EXP_PCT)) / 100;
#endif

	{
		if (to->IsEquipUniqueItem(UNIQUE_ITEM_LARBOR_MEDAL))
			iExp += iExp * 20 /100;

		if (to->GetMapIndex() >= 660000 && to->GetMapIndex() < 670000)
			iExp += iExp * 20 / 100;

		if (to->GetPoint(POINT_EXP_DOUBLE_BONUS))
			if (number(1, 100) <= to->GetPoint(POINT_EXP_DOUBLE_BONUS))
				iExp += iExp * 30 / 100;

		if (to->IsEquipUniqueItem(UNIQUE_ITEM_DOUBLE_EXP))
			iExp += iExp * 50 / 100;
		if (to->IsEquipUniqueItem(UNIQUE_ITEM_DOUBLE_EXP2))
			iExp += iExp * 30 / 100;
		if (to->IsEquipUniqueItem(UNIQUE_ITEM_DOUBLE_EXP3))
			iExp += iExp * 20 / 100;

		switch (to->GetMountVnum())
		{
			case 20110:
			case 20111:
			case 20112:
			case 20113:
				if (to->IsEquipUniqueItem(71115) || to->IsEquipUniqueItem(71117) || to->IsEquipUniqueItem(71119) ||
						to->IsEquipUniqueItem(71121) )
				{
					iExp += iExp * 10 / 100;
				}
				break;

			case 20114:
			case 20120:
			case 20121:
			case 20122:
			case 20123:
			case 20124:
			case 20125:

				iExp += iExp * 30 / 100;
				break;
		}
	}

	{
		if (to->GetPremiumRemainSeconds(PREMIUM_EXP) > 0)
		{
			iExp += (iExp * 50 / 100);
		}

		if (to->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_EXP) == true)
		{
			iExp += (iExp * 50 / 100);
		}

		iExp += iExp * to->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_EXP_BONUS) / 100;
	}

	iExp += (iExp * to->GetPoint(POINT_RAMADAN_CANDY_BONUS_EXP)/100);
	iExp += (iExp * to->GetPoint(POINT_MALL_EXPBONUS)/100);
	iExp += (iExp * to->GetPoint(POINT_EXP)/100);

	if (test_server)
	{
		sys_log(0, "Bonus Exp : Ramadan Candy: %d MallExp: %d PointExp: %d",
				to->GetPoint(POINT_RAMADAN_CANDY_BONUS_EXP),
				to->GetPoint(POINT_MALL_EXPBONUS),
				to->GetPoint(POINT_EXP)
			   );
	}

	iExp = iExp * CHARACTER_MANAGER::instance().GetMobExpRate(to) / 100;

	iExp = MIN(to->GetNextExp() / 10, iExp);

	if (test_server)
	{
		if (quest::CQuestManager::instance().GetEventFlag("exp_bonus_log") && iBaseExp>0)
			to->ChatPacket(CHAT_TYPE_INFO, "exp bonus %d%%", (iExp-iBaseExp)*100/iBaseExp);
		to->ChatPacket(CHAT_TYPE_INFO, "exp(%d) base_exp(%d)", iExp, iBaseExp);
	}

	iExp = AdjustExpByLevel(to, iExp);

	to->PointChange(POINT_EXP, iExp, true);
	//from->CreateFly(FLY_EXP, to);

	{
		LPCHARACTER you = to->GetMarryPartner();

		if (you)
		{
			DWORD dwUpdatePoint = 2000*iExp/to->GetLevel()/to->GetLevel()/3;

			if (to->GetPremiumRemainSeconds(PREMIUM_MARRIAGE_FAST) > 0 ||
					you->GetPremiumRemainSeconds(PREMIUM_MARRIAGE_FAST) > 0)
				dwUpdatePoint = (DWORD)(dwUpdatePoint * 3);

			marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(to->GetPlayerID());

			// DIVORCE_NULL_BUG_FIX
			if (pMarriage && pMarriage->IsNear())
				pMarriage->Update(dwUpdatePoint);
			// END_OF_DIVORCE_NULL_BUG_FIX
		}
	}
}
#endif

namespace NPartyExpDistribute
{
	struct FPartyTotaler
	{
		int		total;
		int		member_count;
		int		x, y;

		FPartyTotaler(LPCHARACTER center)
			: total(0), member_count(0), x(center->GetX()), y(center->GetY())
		{};

		void operator () (LPCHARACTER ch)
		{
			if (DISTANCE_SQRT(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
			{
				total += __GetPartyExpNP(ch->GetLevel());

				++member_count;
			}
		}
	};

	struct FPartyDistributor
	{
		int		total;
		LPCHARACTER	c;
		int		x, y;
		DWORD		_iExp;
		int		m_iMode;
		int		m_iMemberCount;

		FPartyDistributor(LPCHARACTER center, int member_count, int total, DWORD iExp, int iMode)
			: total(total), c(center), x(center->GetX()), y(center->GetY()), _iExp(iExp), m_iMode(iMode), m_iMemberCount(member_count)
			{
				if (m_iMemberCount == 0)
					m_iMemberCount = 1;
			};

		void operator () (LPCHARACTER ch)
		{
			if (!ch || (ch->GetMapIndex() != c->GetMapIndex()))
				return;

			if (DISTANCE_SQRT(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
			{
				DWORD iExp2 = 0;

				switch (m_iMode)
				{
					case PARTY_EXP_DISTRIBUTION_NON_PARITY:
						iExp2 = (DWORD) (_iExp * (float) __GetPartyExpNP(ch->GetLevel()) / total);
						break;

					case PARTY_EXP_DISTRIBUTION_PARITY:
						iExp2 = _iExp / m_iMemberCount;
						break;

					default:
						sys_err("Unknown party exp distribution mode %d", m_iMode);
						return;
				}

				GiveExp(c, ch, iExp2);
			}
		}
	};
}

typedef struct SDamageInfo
{
	int iDam;
	LPCHARACTER pAttacker;
	LPPARTY pParty;

	void Clear()
	{
		pAttacker = NULL;
		pParty = NULL;
	}

	inline void Distribute(LPCHARACTER ch, int iExp)
	{
		if (pAttacker)
			GiveExp(ch, pAttacker, iExp);
		else if (pParty)
		{
			NPartyExpDistribute::FPartyTotaler f(ch);
			pParty->ForEachOnlineMember(f);

			if (pParty->IsPositionNearLeader(ch))
				iExp = iExp * (100 + pParty->GetExpBonusPercent()) / 100;

			if (test_server)
			{
				if (quest::CQuestManager::instance().GetEventFlag("exp_bonus_log") && pParty->GetExpBonusPercent())
					pParty->ChatPacketToAllMember(CHAT_TYPE_INFO, "exp party bonus %d%%", pParty->GetExpBonusPercent());
			}

			if (pParty->GetExpCentralizeCharacter())
			{
				LPCHARACTER tch = pParty->GetExpCentralizeCharacter();

				if (DISTANCE_SQRT(ch->GetX() - tch->GetX(), ch->GetY() - tch->GetY()) <= PARTY_DEFAULT_RANGE)
				{
					int iExpCenteralize = (int) (iExp * 0.05f);
					iExp -= iExpCenteralize;

					GiveExp(ch, pParty->GetExpCentralizeCharacter(), iExpCenteralize);
				}
			}

			NPartyExpDistribute::FPartyDistributor fDist(ch, f.member_count, f.total, iExp, pParty->GetExpDistributionMode());
			pParty->ForEachOnlineMember(fDist);
		}
	}
} TDamageInfo;

#ifdef ENABLE_KILL_EVENT_FIX
LPCHARACTER CHARACTER::GetMostAttacked() {
	uint64_t iMostDam = 0;
	LPCHARACTER pkChrMostAttacked = NULL;
	auto it = m_map_kDamage.begin();

	while (it != m_map_kDamage.end()){
		//* getting information from the iterator
		const VID & c_VID = it->first;
		const uint64_t iDam = it->second.iTotalDamage;

		//* increasing the iterator
		++it;

		//* finding the character from his vid
		LPCHARACTER pAttacker = CHARACTER_MANAGER::instance().Find(c_VID);

		//* if the attacked is now offline
		if (!pAttacker)
			continue;

		//* if the attacker is not a player
		if( pAttacker->IsNPC())
			continue;

		//* if the player is too far
		if(DISTANCE_SQRT(GetX()-pAttacker->GetX(), GetY()-pAttacker->GetY())>5000)
			continue;

		if (iDam > iMostDam){
			pkChrMostAttacked = pAttacker;
			iMostDam = iDam;
		}
	}

	return pkChrMostAttacked;
}
#endif

LPCHARACTER CHARACTER::DistributeExp()
{
	int iExpToDistribute = GetExp();

	if (iExpToDistribute <= 0)
		return NULL;

	uint64_t iTotalDam = 0;
	LPCHARACTER pkChrMostAttacked = NULL;
	uint64_t iMostDam = 0;

	typedef std::vector<TDamageInfo> TDamageInfoTable;
	TDamageInfoTable damage_info_table;
	std::map<LPPARTY, TDamageInfo> map_party_damage;

	damage_info_table.reserve(m_map_kDamage.size());

	TDamageMap::iterator it = m_map_kDamage.begin();

	while (it != m_map_kDamage.end())
	{
		const VID & c_VID = it->first;
		uint64_t iDam = it->second.iTotalDamage;

		++it;

		LPCHARACTER pAttacker = CHARACTER_MANAGER::instance().Find(c_VID);

		if (!pAttacker || pAttacker->IsNPC() || DISTANCE_SQRT(GetX()-pAttacker->GetX(), GetY()-pAttacker->GetY())>5000)
			continue;

		iTotalDam += iDam;
		if (!pkChrMostAttacked || iDam > iMostDam)
		{
			pkChrMostAttacked = pAttacker;
			iMostDam = iDam;
		}

		if (pAttacker->GetParty())
		{
			std::map<LPPARTY, TDamageInfo>::iterator it = map_party_damage.find(pAttacker->GetParty());
			if (it == map_party_damage.end())
			{
				TDamageInfo di;
				di.iDam = iDam;
				di.pAttacker = NULL;
				di.pParty = pAttacker->GetParty();
				map_party_damage.emplace(di.pParty, di);
			}
			else
			{
				it->second.iDam += iDam;
			}
		}
		else
		{
			TDamageInfo di;

			di.iDam = iDam;
			di.pAttacker = pAttacker;
			di.pParty = NULL;

			//sys_log(0, "__ pq_damage %s %d", pAttacker->GetName(), iDam);
			//pq_damage.push(di);
			damage_info_table.emplace_back(di);
		}
	}

	for (std::map<LPPARTY, TDamageInfo>::iterator it = map_party_damage.begin(); it != map_party_damage.end(); ++it)
	{
		damage_info_table.emplace_back(it->second);
		//sys_log(0, "__ pq_damage_party [%u] %d", it->second.pParty->GetLeaderPID(), it->second.iDam);
	}

	SetExp(0);
	//m_map_kDamage.clear();

	if (iTotalDam == 0)
		return NULL;

	if (m_pkChrStone)
	{
		//sys_log(0, "__ Give half to Stone : %d", iExpToDistribute>>1);
		int iExp = iExpToDistribute >> 1;
		m_pkChrStone->SetExp(m_pkChrStone->GetExp() + iExp);
		iExpToDistribute -= iExp;
	}

	sys_log(1, "%s total exp: %d, damage_info_table.size() == %d, TotalDam %d",
			GetName(), iExpToDistribute, damage_info_table.size(), iTotalDam);
	//sys_log(1, "%s total exp: %d, pq_damage.size() == %d, TotalDam %d",
	//GetName(), iExpToDistribute, pq_damage.size(), iTotalDam);

	if (damage_info_table.empty())
		return NULL;

	DistributeHP(pkChrMostAttacked);

	{
		TDamageInfoTable::iterator di = damage_info_table.begin();
		{
			TDamageInfoTable::iterator it;

			for (it = damage_info_table.begin(); it != damage_info_table.end();++it)
			{
				if (it->iDam > di->iDam)
					di = it;
			}
		}

		int	iExp = iExpToDistribute / 5;
		iExpToDistribute -= iExp;

		float fPercent = (float) di->iDam / iTotalDam;

		if (fPercent > 1.0f)
		{
			sys_err("DistributeExp percent over 1.0 (fPercent %f name %s)", fPercent, di->pAttacker->GetName());
			fPercent = 1.0f;
		}

		iExp += (int) (iExpToDistribute * fPercent);

		//sys_log(0, "%s given exp percent %.1f + 20 dam %d", GetName(), fPercent * 100.0f, di.iDam);

		di->Distribute(this, iExp);

		if (fPercent == 1.0f)
			return pkChrMostAttacked;

		di->Clear();
	}

	{
		TDamageInfoTable::iterator it;

		for (it = damage_info_table.begin(); it != damage_info_table.end(); ++it)
		{
			TDamageInfo & di = *it;

			float fPercent = (float) di.iDam / iTotalDam;

			if (fPercent > 1.0f)
			{
				sys_err("DistributeExp percent over 1.0 (fPercent %f name %s)", fPercent, di.pAttacker->GetName());
				fPercent = 1.0f;
			}

			//sys_log(0, "%s given exp percent %.1f dam %d", GetName(), fPercent * 100.0f, di.iDam);
			di.Distribute(this, (int) (iExpToDistribute * fPercent));
		}
	}

	return pkChrMostAttacked;
}

int CHARACTER::GetArrowAndBow(LPITEM * ppkBow, LPITEM * ppkArrow, int iArrowCount/* = 1 */)
{
	LPITEM pkBow;

	if (!(pkBow = GetWear(WEAR_WEAPON)) || pkBow->GetProto()->bSubType != WEAPON_BOW)
	{
		return 0;
	}

	LPITEM pkArrow;

	if (!(pkArrow = GetWear(WEAR_ARROW)) || pkArrow->GetType() != ITEM_WEAPON ||
			(pkArrow->GetProto()->bSubType != WEAPON_ARROW
				#ifdef ENABLE_QUIVER_SYSTEM
				&& pkArrow->GetSubType() != WEAPON_QUIVER
				#endif
			)
		)
	{
		return 0;
	}

	iArrowCount = MIN(iArrowCount, pkArrow->GetCount());
#ifdef ENABLE_QUIVER_SYSTEM
	if (pkArrow->GetSubType() == WEAPON_QUIVER && pkArrow->GetRealUseLimit()) // no arrows if expired - 1 otherwise
		iArrowCount = 1;
#endif

	*ppkBow = pkBow;
	*ppkArrow = pkArrow;

	return iArrowCount;
}

void CHARACTER::UseArrow(LPITEM pkArrow, DWORD dwArrowCount)
{
#ifdef ENABLE_QUIVER_SYSTEM
	if (pkArrow->GetSubType() == WEAPON_QUIVER)
		return;
#endif

	int iCount = pkArrow->GetCount();
	DWORD dwVnum = pkArrow->GetVnum();
	// iCount = iCount - MIN(iCount, dwArrowCount);
	pkArrow->SetCount(iCount);

	if (iCount == 0)
	{
		LPITEM pkNewArrow = FindSpecifyItem(dwVnum);

		sys_log(0, "UseArrow : FindSpecifyItem %u %p", dwVnum, get_pointer(pkNewArrow));

		if (pkNewArrow)
			EquipItem(pkNewArrow);
	}
}

void CHARACTER::SetLastSuccessShootTime(DWORD value)
{
	dwLastSuccessShootTime = value;
}

DWORD CHARACTER::GetLastSuccessShootTime() const
{
	return dwLastSuccessShootTime;
}


class CFuncShoot
{
	public:
		LPCHARACTER	m_me;
		BYTE		m_bType;
		bool		m_bSucceed;

		CFuncShoot(LPCHARACTER ch, BYTE bType) : m_me(ch), m_bType(bType), m_bSucceed(FALSE)
		{
		}

		void operator () (DWORD dwTargetVID)
		{
			if (m_bType > 1)
			{
				if (g_bSkillDisable)
					return;

				m_me->m_SkillUseInfo[m_bType].SetMainTargetVID(dwTargetVID);
			}

			LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(dwTargetVID);

			if (!pkVictim)
				return;

			if (!battle_is_attackable(m_me, pkVictim))
				return;

			if (m_me->IsNPC())
			{
				if (DISTANCE_SQRT(m_me->GetX() - pkVictim->GetX(), m_me->GetY() - pkVictim->GetY()) > 5000)
					return;
			}

			#ifdef ENABLE_SKILL_COOLDOWN_CHECK
			if (m_me->IsPC() && m_bType > 0 && m_me->SkillIsOnCooldown(m_bType))
				return;
			#endif

			LPITEM pkBow, pkArrow;

			switch (m_bType)
			{
				case 0:
					{
						int iDam = 0;

						if (m_me->IsPC())
						{
							if (m_me->GetJob() != JOB_ASSASSIN)
								return;

							if (0 == m_me->GetArrowAndBow(&pkBow, &pkArrow))
								return;
#ifdef ENABLE_BOT_CONTROL
							if (m_me->IsBotControl())
								return;
							if (pkVictim->IsPC() && pkVictim->IsBotControl())
								return;
#endif

							if (m_me->GetSkillGroup() != 0)
								if (!m_me->IsNPC() && m_me->GetSkillGroup() != 2)
								{
									if (m_me->GetSP() < 5)
										return;

									m_me->PointChange(POINT_SP, -5);
								}

							iDam = CalcArrowDamage(m_me, pkVictim, pkBow, pkArrow);
							m_me->UseArrow(pkArrow, 1);

							// check speed hack
							DWORD	dwCurrentTime	= get_dword_time();
							if (IS_SPEED_HACK(m_me, pkVictim, dwCurrentTime))
								iDam	= 0;
						}
						else
							iDam = CalcMeleeDamage(m_me, pkVictim);

						NormalAttackAffect(m_me, pkVictim);

						iDam = iDam * (100 - pkVictim->GetPoint(POINT_RESIST_BOW)) / 100;

						//sys_log(0, "%s arrow %s dam %d", m_me->GetName(), pkVictim->GetName(), iDam);

						m_me->OnMove(true);
						pkVictim->OnMove();

						if (pkVictim->CanBeginFight())
							pkVictim->BeginFight(m_me);

						pkVictim->Damage(m_me, iDam, DAMAGE_TYPE_NORMAL_RANGE);

						if (iDam > 0)
							m_me->SetLastSuccessShootTime(get_dword_time());
					}
					break;

				case 1:
					{
						int iDam;

						if (m_me->IsPC())
							return;

						iDam = CalcMagicDamage(m_me, pkVictim);

						NormalAttackAffect(m_me, pkVictim);

#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
						const int resist_magic = MINMAX(0, pkVictim->GetPoint(POINT_RESIST_MAGIC), 100);
						const int resist_magic_reduction = MINMAX(0, (m_me->GetJob()==JOB_SURA) ? m_me->GetPoint(POINT_RESIST_MAGIC_REDUCTION)/2 : m_me->GetPoint(POINT_RESIST_MAGIC_REDUCTION), 50);
						const int total_res_magic = MINMAX(0, resist_magic - resist_magic_reduction, 100);
						iDam = iDam * (100 - total_res_magic) / 100;
#else
						iDam = iDam * (100 - pkVictim->GetPoint(POINT_RESIST_MAGIC)) / 100;
#endif

						//sys_log(0, "%s arrow %s dam %d", m_me->GetName(), pkVictim->GetName(), iDam);

						m_me->OnMove(true);
						pkVictim->OnMove();

						if (pkVictim->CanBeginFight())
							pkVictim->BeginFight(m_me);

						pkVictim->Damage(m_me, iDam, DAMAGE_TYPE_MAGIC);

					}
					break;

				case SKILL_YEONSA:
					{
						//int iUseArrow = 2 + (m_me->GetSkillPower(SKILL_YEONSA) *6/100);
						int iUseArrow = 1;

						{
							if (iUseArrow == m_me->GetArrowAndBow(&pkBow, &pkArrow, iUseArrow))
							{
								m_me->OnMove(true);
								pkVictim->OnMove();

								if (pkVictim->CanBeginFight())
									pkVictim->BeginFight(m_me);

								m_me->ComputeSkill(m_bType, pkVictim);
								m_me->UseArrow(pkArrow, iUseArrow);

								if (pkVictim->IsDead())
									break;

							}
							else
								break;
						}
					}
					break;

				case SKILL_KWANKYEOK:
					{
						int iUseArrow = 1;

						if (iUseArrow == m_me->GetArrowAndBow(&pkBow, &pkArrow, iUseArrow))
						{
							m_me->OnMove(true);
							pkVictim->OnMove();

							if (pkVictim->CanBeginFight())
								pkVictim->BeginFight(m_me);

							sys_log(0, "%s kwankeyok %s", m_me->GetName(), pkVictim->GetName());
							m_me->ComputeSkill(m_bType, pkVictim);
							m_me->UseArrow(pkArrow, iUseArrow);
						}
					}
					break;

				case SKILL_GIGUNG:
					{
						int iUseArrow = 1;
						if (iUseArrow == m_me->GetArrowAndBow(&pkBow, &pkArrow, iUseArrow))
						{
							m_me->OnMove(true);
							pkVictim->OnMove();

							if (pkVictim->CanBeginFight())
								pkVictim->BeginFight(m_me);

							sys_log(0, "%s gigung %s", m_me->GetName(), pkVictim->GetName());
							m_me->ComputeSkill(m_bType, pkVictim);
							m_me->UseArrow(pkArrow, iUseArrow);
						}
					}

					break;
				case SKILL_HWAJO:
					{
						int iUseArrow = 1;
						if (iUseArrow == m_me->GetArrowAndBow(&pkBow, &pkArrow, iUseArrow))
						{
							m_me->OnMove(true);
							pkVictim->OnMove();

							if (pkVictim->CanBeginFight())
								pkVictim->BeginFight(m_me);

							sys_log(0, "%s hwajo %s", m_me->GetName(), pkVictim->GetName());
							m_me->ComputeSkill(m_bType, pkVictim);
							m_me->UseArrow(pkArrow, iUseArrow);
						}
					}

					break;

				case SKILL_HORSE_WILDATTACK_RANGE:
					{
						int iUseArrow = 1;
						if (iUseArrow == m_me->GetArrowAndBow(&pkBow, &pkArrow, iUseArrow))
						{
							m_me->OnMove(true);
							pkVictim->OnMove();

							if (pkVictim->CanBeginFight())
								pkVictim->BeginFight(m_me);

							sys_log(0, "%s horse_wildattack %s", m_me->GetName(), pkVictim->GetName());
							m_me->ComputeSkill(m_bType, pkVictim);
							m_me->UseArrow(pkArrow, iUseArrow);
						}
					}

					break;

				case SKILL_PAERYONG:
				case SKILL_MARYUNG:
				case SKILL_TUSOK:
				case SKILL_BIPABU:
				case SKILL_NOEJEON:
				case SKILL_GEOMPUNG:
				case SKILL_SANGONG:
				case SKILL_MAHWAN:
				case SKILL_PABEOB:
				case SKILL_YONGBI:
				case SKILL_BYEURAK:
					{
						m_me->OnMove(true);
						pkVictim->OnMove();

						if (pkVictim->CanBeginFight())
							pkVictim->BeginFight(m_me);

						sys_log(0, "%s - Skill %d -> %s", m_me->GetName(), m_bType, pkVictim->GetName());
						m_me->ComputeSkill(m_bType, pkVictim);
					}
					break;

				case SKILL_CHAIN:
					{
						m_me->OnMove(true);
						pkVictim->OnMove();

						if (pkVictim->CanBeginFight())
							pkVictim->BeginFight(m_me);

						sys_log(0, "%s - Skill %d -> %s", m_me->GetName(), m_bType, pkVictim->GetName());
						m_me->ComputeSkill(m_bType, pkVictim);

					}
					break;

				// case SKILL_YONGBI:
				// 	{
				// 		m_me->OnMove(true);
				// 	}
				// 	break;

				default:
					sys_err("CFuncShoot: I don't know this type [%d] of range attack.", (int) m_bType);
					break;
			}

			m_bSucceed = TRUE;
		}
};

bool CHARACTER::Shoot(BYTE bType)
{
	sys_log(1, "Shoot %s type %u flyTargets.size %zu", GetName(), bType, m_vec_dwFlyTargets.size());

	if (!CanMove())
	{
		return false;
	}

	CFuncShoot f(this, bType);

	if (m_dwFlyTargetID != 0)
	{
		f(m_dwFlyTargetID);
		m_dwFlyTargetID = 0;
	}

	f = std::for_each(m_vec_dwFlyTargets.begin(), m_vec_dwFlyTargets.end(), f);
	m_vec_dwFlyTargets.clear();

	return f.m_bSucceed;
}

void CHARACTER::FlyTarget(DWORD dwTargetVID, int32_t x, int32_t y, BYTE bHeader)
{
	LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(dwTargetVID);
	TPacketGCFlyTargeting pack;

	//pack.bHeader	= HEADER_GC_FLY_TARGETING;
	pack.bHeader	= (bHeader == HEADER_CG_FLY_TARGETING) ? HEADER_GC_FLY_TARGETING : HEADER_GC_ADD_FLY_TARGETING;
	pack.dwShooterVID	= GetVID();

	if (pkVictim)
	{
		pack.dwTargetVID = pkVictim->GetVID();
		pack.x = pkVictim->GetX();
		pack.y = pkVictim->GetY();

		if (bHeader == HEADER_CG_FLY_TARGETING)
			m_dwFlyTargetID = dwTargetVID;
		else
			m_vec_dwFlyTargets.emplace_back(dwTargetVID);
	}
	else
	{
		pack.dwTargetVID = 0;
		pack.x = x;
		pack.y = y;
	}

	sys_log(1, "FlyTarget %s vid %d x %d y %d", GetName(), pack.dwTargetVID, pack.x, pack.y);
	PacketAround(&pack, sizeof(pack), this);
}

LPCHARACTER CHARACTER::GetNearestVictim(LPCHARACTER pkChr)
{
	if (NULL == pkChr)
		pkChr = this;

	float fMinDist = 99999.0f;
	LPCHARACTER pkVictim = NULL;

	TDamageMap::iterator it = m_map_kDamage.begin();

	while (it != m_map_kDamage.end())
	{
		const VID & c_VID = it->first;
		++it;

		LPCHARACTER pAttacker = CHARACTER_MANAGER::instance().Find(c_VID);

		if (!pAttacker)
			continue;

		if (pAttacker->IsAffectFlag(AFF_EUNHYUNG) ||
				pAttacker->IsAffectFlag(AFF_INVISIBILITY) ||
				pAttacker->IsAffectFlag(AFF_REVIVE_INVISIBLE))
			continue;

		if (pAttacker->IsDead())
			continue;

		float fDist = DISTANCE_SQRT(pAttacker->GetX() - pkChr->GetX(), pAttacker->GetY() - pkChr->GetY());

		if (fDist < fMinDist)
		{
			pkVictim = pAttacker;
			fMinDist = fDist;
		}
	}

	return pkVictim;
}

void CHARACTER::SetVictim(LPCHARACTER pkVictim)
{
	if (!pkVictim)
	{
		if (0 != (DWORD)m_kVIDVictim)
			MonsterLog("���� ����� ����");

		m_kVIDVictim.Reset();
		battle_end(this);
	}
	else
	{
		if (m_kVIDVictim != pkVictim->GetVID())
			MonsterLog("���� ����� ����: %s", pkVictim->GetName());

		m_kVIDVictim = pkVictim->GetVID();
		m_dwLastVictimSetTime = get_dword_time();
	}
}

LPCHARACTER CHARACTER::GetVictim() const
{
	return CHARACTER_MANAGER::instance().Find(m_kVIDVictim);
}

LPCHARACTER CHARACTER::GetProtege() const
{
	if (m_pkChrStone)
		return m_pkChrStone;

	if (m_pkParty)
		return m_pkParty->GetLeader();

	return NULL;
}

int64_t CHARACTER::GetAlignment() const
{
	return m_iAlignment;
}

int64_t CHARACTER::GetRealAlignment() const
{
	return m_iRealAlignment;
}

void CHARACTER::ShowAlignment(bool bShow)
{
	if (bShow)
	{
		if (m_iAlignment != m_iRealAlignment)
		{
			m_iAlignment = m_iRealAlignment;
			UpdatePacket();
		}
	}
	else
	{
		if (m_iAlignment != 0)
		{
			m_iAlignment = 0;
			UpdatePacket();
		}
	}
}

void CHARACTER::UpdateAlignment(int64_t iAmount)
{
	bool bShow = false;

	if (m_iAlignment == m_iRealAlignment)
		bShow = true;

	int64_t i = m_iAlignment;

#ifdef ENABLE_ALIGN_RENEWAL
	int64_t iOldAlignment = m_iRealAlignment;
	m_iRealAlignment = MINMAX(0, m_iRealAlignment + iAmount, 700000);
	OnAlignUpdate(iOldAlignment);
#else
	m_iRealAlignment = MINMAX(-200000, m_iRealAlignment + iAmount, 200000);
#endif


	if (bShow)
	{
		m_iAlignment = m_iRealAlignment;

		if (i != m_iAlignment)
			UpdatePacket();
	}
}

void CHARACTER::SetKillerMode(bool isOn)
{
	if ((isOn ? ADD_CHARACTER_STATE_KILLER : 0) == IS_SET(m_bAddChrState, ADD_CHARACTER_STATE_KILLER))
		return;

	if (isOn)
		SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_KILLER);
	else
		REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_KILLER);

	m_iKillerModePulse = thecore_pulse();
	UpdatePacket();
	sys_log(0, "SetKillerMode Update %s[%d]", GetName(), GetPlayerID());
}

bool CHARACTER::IsKillerMode() const
{
	return IS_SET(m_bAddChrState, ADD_CHARACTER_STATE_KILLER);
}

void CHARACTER::UpdateKillerMode()
{
	if (!IsKillerMode())
		return;

	if (thecore_pulse() - m_iKillerModePulse >= PASSES_PER_SEC(30))
		SetKillerMode(false);
}

void CHARACTER::SetPKMode(BYTE bPKMode)
{
	if (bPKMode >= PK_MODE_MAX_NUM)
		return;

	if (m_bPKMode == bPKMode)
		return;

	if (bPKMode == PK_MODE_GUILD && !GetGuild())
		bPKMode = PK_MODE_FREE;

	m_bPKMode = bPKMode;
	UpdatePacket();

	sys_log(0, "PK_MODE: %s %d", GetName(), m_bPKMode);
}

BYTE CHARACTER::GetPKMode() const
{
	return m_bPKMode;
}

struct FuncForgetMyAttacker
{
	LPCHARACTER m_ch;

#ifdef __RESET_PLAYER_DAMAGE__
	bool _fullClear = false;
#endif

	FuncForgetMyAttacker(LPCHARACTER ch
#ifdef __RESET_PLAYER_DAMAGE__
		, bool fullClear_ = false
#endif
	)
	{
		m_ch = ch;
#ifdef __RESET_PLAYER_DAMAGE__
		_fullClear = fullClear_;
#endif
	}
	void operator()(LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER ch = (LPCHARACTER) ent;
			if (ch->IsPC())
				return;
			if (ch->m_kVIDVictim == m_ch->GetVID())
				ch->SetVictim(NULL);

#ifdef __RESET_PLAYER_DAMAGE__
			if (_fullClear)
				ch->ErasePlayerDamage(m_ch);
#endif
		}
	}
};

#ifdef ENABLE_VS_PELERYNKA_VIP
struct FuncAggregateMonster
{
    LPCHARACTER m_ch;
    BYTE m_type;

    FuncAggregateMonster(LPCHARACTER ch, BYTE type = 0) : m_ch(ch), m_type(type){}
    void operator () (LPENTITY ent)
    {
        if (!ent->IsType(ENTITY_CHARACTER))
            return;

        LPCHARACTER ch = (LPCHARACTER) ent;
        if (!ch || ch->IsPC() || !ch->IsMonster() || ch->GetVictim())
            return;

		if (ch->GetRaceNum() == 3190  || ch->GetRaceNum() == 3191  || ch->GetRaceNum() == 8214  || ch->GetRaceNum() == 8213  || ch->GetRaceNum() == 6783  || ch->GetRaceNum() == 6789  ||
			ch->GetRaceNum() == 2810  || ch->GetRaceNum() == 2771  || ch->GetRaceNum() == 34600 || ch->GetRaceNum() == 34601 || ch->GetRaceNum() == 2820  || ch->GetRaceNum() == 3390  ||
			ch->GetRaceNum() == 3391  || ch->GetRaceNum() == 2760  || ch->GetRaceNum() == 201004 || ch->GetRaceNum() == 201005 || ch->GetRaceNum() == 201006 || ch->GetRaceNum() == 201007 ||
			ch->GetRaceNum() == 201014 || ch->GetRaceNum() == 201015 || ch->GetRaceNum() == 201016 || ch->GetRaceNum() == 201017 || ch->GetRaceNum() == 201069 || ch->GetRaceNum() == 201070 ||
			ch->GetRaceNum() == 201071 || ch->GetRaceNum() == 201072 || ch->GetRaceNum() == 201073 || ch->GetRaceNum() == 201074 || ch->GetRaceNum() == 201075 || ch->GetRaceNum() == 201076 ||
			ch->GetRaceNum() == 1092  || ch->GetRaceNum() == 1093  || ch->GetRaceNum() == 2598  || ch->GetRaceNum() == 693   || ch->GetRaceNum() == 691   || ch->GetRaceNum() == 692   ||
			ch->GetRaceNum() == 591   || ch->GetRaceNum() == 202218 || ch->GetRaceNum() == 202212 || ch->GetRaceNum() == 202213 || ch->GetRaceNum() == 202214 || ch->GetRaceNum() == 202206 ||
			ch->GetRaceNum() == 202207 || ch->GetRaceNum() == 202208 || ch->GetRaceNum() == 202202 || ch->GetRaceNum() == 202201 || ch->GetRaceNum() == 202200 || ch->GetRaceNum() == 2091  ||
			ch->GetRaceNum() == 202215 || ch->GetRaceNum() == 202216 || ch->GetRaceNum() == 202217 || ch->GetRaceNum() == 202225 || ch->GetRaceNum() == 202226)
			return;

        int distance = DISTANCE_SQRT(ch->GetX() - m_ch->GetX(), ch->GetY() - m_ch->GetY());
        int maxDistance = (m_type == 0) ? 5000 : (m_type == 1) ? 7500 : 5000;

		if (number(1, 100) <= 50)
			if (distance < maxDistance && ch->CanBeginFight())
				ch->BeginFight(m_ch);
	}
};
#else
struct FuncAggregateMonster
{
	LPCHARACTER m_ch;
	FuncAggregateMonster(LPCHARACTER ch)
	{
		m_ch = ch;
	}
	void operator()(LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER ch = (LPCHARACTER) ent;
			if (ch->IsPC())
				return;
			if (!ch->IsMonster())
				return;
			if (ch->GetVictim())
				return;

			if (number(1, 100) <= 50)
				if (DISTANCE_SQRT(ch->GetX() - m_ch->GetX(), ch->GetY() - m_ch->GetY()) < 5000)
					if (ch->CanBeginFight())
						ch->BeginFight(m_ch);
		}
	}
};
#endif

struct FuncAttractRanger
{
	LPCHARACTER m_ch;
	FuncAttractRanger(LPCHARACTER ch)
	{
		m_ch = ch;
	}

	void operator()(LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER ch = (LPCHARACTER) ent;
			if (ch->IsPC())
				return;
			if (!ch->IsMonster())
				return;
			if (ch->GetVictim() && ch->GetVictim() != m_ch)
				return;
			if (ch->GetMobAttackRange() > 150)
			{
				int iNewRange = 150;//(int)(ch->GetMobAttackRange() * 0.2);
				if (iNewRange < 150)
					iNewRange = 150;

				ch->AddAffect(AFFECT_BOW_DISTANCE, POINT_BOW_DISTANCE, iNewRange - ch->GetMobAttackRange(), AFF_NONE, 3*60, 0, false);
			}
		}
	}
};

struct FuncPullMonster
{
	LPCHARACTER m_ch;
	int m_iLength;
	FuncPullMonster(LPCHARACTER ch, int iLength = 300)
	{
		m_ch = ch;
		m_iLength = iLength;
	}

	void operator()(LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER ch = (LPCHARACTER) ent;
			if (ch->IsPC())
				return;
			if (!ch->IsMonster())
				return;
			//if (ch->GetVictim() && ch->GetVictim() != m_ch)
			//return;
			float fDist = DISTANCE_SQRT(m_ch->GetX() - ch->GetX(), m_ch->GetY() - ch->GetY());
			if (fDist > 3000 || fDist < 100)
				return;

			float fNewDist = fDist - m_iLength;
			if (fNewDist < 100)
				fNewDist = 100;

			float degree = GetDegreeFromPositionXY(ch->GetX(), ch->GetY(), m_ch->GetX(), m_ch->GetY());
			float fx;
			float fy;

			GetDeltaByDegree(degree, fDist - fNewDist, &fx, &fy);
			int32_t tx = (int32_t)(ch->GetX() + fx);
			int32_t ty = (int32_t)(ch->GetY() + fy);

			ch->Sync(tx, ty);
			ch->Goto(tx, ty);
			ch->CalculateMoveDuration();

			ch->SyncPacket();
		}
	}
};

void CHARACTER::ForgetMyAttacker(
#ifdef __RESET_PLAYER_DAMAGE__
	bool fullClear_
#endif
)
{
	LPSECTREE pSec = GetSectree();
	if (pSec)
	{
		FuncForgetMyAttacker f(this
#ifdef __RESET_PLAYER_DAMAGE__
			, fullClear_
#endif		
		);
		pSec->ForEachAround(f);
	}
	ReviveInvisible(5);
}

#ifdef ENABLE_VS_PELERYNKA_VIP
void CHARACTER::AggregateMonster(BYTE type)
{
	LPSECTREE pSec = GetSectree();
	if (pSec)
	{
		FuncAggregateMonster f(this, type);
		pSec->ForEachAround(f);
	}
}
#else
void CHARACTER::AggregateMonster();
{
	LPSECTREE pSec = GetSectree();
	if (pSec)
	{
		FuncAggregateMonster f(this);
		pSec->ForEachAround(f);
	}
}
#endif

void CHARACTER::AttractRanger()
{
	LPSECTREE pSec = GetSectree();
	if (pSec)
	{
		FuncAttractRanger f(this);
		pSec->ForEachAround(f);
	}
}

void CHARACTER::PullMonster()
{
	LPSECTREE pSec = GetSectree();
	if (pSec)
	{
		FuncPullMonster f(this);
		pSec->ForEachAround(f);
	}
}

void CHARACTER::UpdateAggrPointEx(LPCHARACTER pAttacker, EDamageType type, int64_t dam, CHARACTER::TBattleInfo & info)
{
	switch (type)
	{
		case DAMAGE_TYPE_NORMAL_RANGE:
			dam = (int) (dam*1.2f);
			break;

		case DAMAGE_TYPE_RANGE:
			dam = (int) (dam*1.5f);
			break;

		case DAMAGE_TYPE_MAGIC:
			dam = (int) (dam*1.2f);
			break;

		default:
			break;
	}

	if (pAttacker == GetVictim())
		dam = (int) (dam * 1.2f);

	info.iAggro += dam;

	if (info.iAggro < 0)
		info.iAggro = 0;

	//sys_log(0, "UpdateAggrPointEx for %s by %s dam %d total %d", GetName(), pAttacker->GetName(), dam, total);
	if (GetParty() && dam > 0 && type != DAMAGE_TYPE_SPECIAL)
	{
		LPPARTY pParty = GetParty();

		int iPartyAggroDist = dam;

		if (pParty->GetLeaderPID() == GetVID())
			iPartyAggroDist /= 2;
		else
			iPartyAggroDist /= 3;

		pParty->SendMessage(this, PM_AGGRO_INCREASE, iPartyAggroDist, pAttacker->GetVID());
	}

	ChangeVictimByAggro(info.iAggro, pAttacker);
}

void CHARACTER::UpdateAggrPoint(LPCHARACTER pAttacker, EDamageType type, int64_t dam)
{
	if (IsDead() || IsStun())
		return;

	TDamageMap::iterator it = m_map_kDamage.find(pAttacker->GetVID());

	if (it == m_map_kDamage.end())
	{
		m_map_kDamage.emplace(pAttacker->GetVID(), TBattleInfo(0, dam));
		it = m_map_kDamage.find(pAttacker->GetVID());
	}

#ifdef TAKE_LEGEND_DAMAGE_BOARD_SYSTEM
	if (IsLegenda() || IsElitka())
	{
		TDamageMapLegend::iterator itL = m_map_kDamageLegend.find(pAttacker->GetName());
		if (itL == m_map_kDamageLegend.end())
		{
			m_map_kDamageLegend.insert(TDamageMapLegend::value_type(pAttacker->GetName(), TBattleInfoLegend(pAttacker->GetVID(), pAttacker->GetName(), pAttacker->GetLevel(), pAttacker->GetRaceNum(), pAttacker->GetEmpire(), 0, dam)));
			itL = m_map_kDamageLegend.find(pAttacker->GetName());
		}
	}
#endif

	UpdateAggrPointEx(pAttacker, type, dam, it->second);
}

void CHARACTER::ChangeVictimByAggro(int iNewAggro, LPCHARACTER pNewVictim)
{
	if (get_dword_time() - m_dwLastVictimSetTime < 3000)
		return;

	if (pNewVictim == GetVictim())
	{
		if (m_iMaxAggro < iNewAggro)
		{
			m_iMaxAggro = iNewAggro;
			return;
		}

		TDamageMap::iterator it;
		TDamageMap::iterator itFind = m_map_kDamage.end();

		for (it = m_map_kDamage.begin(); it != m_map_kDamage.end(); ++it)
		{
			if (it->second.iAggro > iNewAggro)
			{
				LPCHARACTER ch = CHARACTER_MANAGER::instance().Find(it->first);

				if (ch && !ch->IsDead() && DISTANCE_SQRT(ch->GetX() - GetX(), ch->GetY() - GetY()) < 5000)
				{
					itFind = it;
					iNewAggro = it->second.iAggro;
				}
			}
		}

		if (itFind != m_map_kDamage.end())
		{
			m_iMaxAggro = iNewAggro;
			SetVictim(CHARACTER_MANAGER::instance().Find(itFind->first));
			m_dwStateDuration = 1;
		}
	}
	else
	{
		if (m_iMaxAggro < iNewAggro)
		{
			m_iMaxAggro = iNewAggro;
			SetVictim(pNewVictim);
			m_dwStateDuration = 1;
		}
	}
}


#ifdef ENABLE_EVENT_MANAGER
#include "item_manager.h"
#include "item.h"
#include "desc_client.h"
#include "desc_manager.h"
void CHARACTER_MANAGER::ClearEventData()
{
	m_eventData.clear();
}
void CHARACTER_MANAGER::CheckBonusEvent(LPCHARACTER ch)
{
	const TEventManagerData* eventPtr = CheckEventIsActive(BONUS_EVENT, ch->GetEmpire(), ch);
	if (eventPtr)
		ch->ApplyPoint(eventPtr->value[0], eventPtr->value[1]);
}
const TEventManagerData* CHARACTER_MANAGER::CheckEventIsActive(BYTE eventIndex, BYTE empireIndex, LPCHARACTER ch)
{
	for (const auto& [dayIndex, dayVector] : m_eventData)
	{
		for (const auto& eventData : dayVector)
		{
			if (eventData.eventIndex == eventIndex)
			{
				if (eventData.channelFlag != 0)
					if (eventData.channelFlag != g_bChannel)
						continue;
				if (eventData.empireFlag != 0 && empireIndex != 0)
					if (eventData.empireFlag != empireIndex)
						continue;

				if (ch != nullptr)
				{
					if (ch->IsPersonalEventActivated(eventData.eventID))
						return &eventData;
				}
				else
				{
					if (eventData.eventStatus == true)
						return &eventData;
				}
			}
		}
	}
	return NULL;
}
void CHARACTER_MANAGER::CheckEventForDrop(LPCHARACTER pkChr, LPCHARACTER pkKiller, std::vector<LPITEM>& vec_item)
{
	static const std::unordered_set<DWORD> s_dungeonBossVnums = {
		693, 1093, 2598, 202202, 202208, 202214, 202218, 202224, 202230, 202223
	};

	const BYTE killerEmpire = pkKiller->GetEmpire();
	const TEventManagerData* eventPtr = NULL;
	LPITEM rewardItem = NULL;

	if (pkChr->IsStone() && !pkChr->IsGroupDungeonChest() && !pkChr->IsSummerMob())
	{
		eventPtr = CheckEventIsActive(DOUBLE_METIN_LOOT_EVENT, killerEmpire, pkKiller);
		if (eventPtr)
		{
			const int prob = number(1, 100);
			const int success_prob = eventPtr->value[3];
			if (prob < success_prob)
			{
				std::vector<LPITEM> m_cache;
				for (const auto& vItem : vec_item)
				{
					rewardItem = ITEM_MANAGER::Instance().CreateItem(vItem->GetVnum(), vItem->GetCount(), 0, true);
					if (rewardItem) m_cache.emplace_back(rewardItem);
				}
				for (const auto& rItem : m_cache)
					vec_item.emplace_back(rItem);
			}
		}
	}
	else if (pkChr->GetMobRank() >= MOB_RANK_BOSS && !pkChr->IsLegenda() && !pkChr->IsSummerMob() && !s_dungeonBossVnums.count(pkChr->GetRaceNum()))
	{
		eventPtr = CheckEventIsActive(DOUBLE_BOSS_LOOT_EVENT, killerEmpire, pkKiller);
		if (eventPtr)
		{
			const int prob = number(1, 100);
			const int success_prob = eventPtr->value[3];
			if (prob < success_prob)
			{
				std::vector<LPITEM> m_cache;
				for (const auto& vItem : vec_item)
				{
					rewardItem = ITEM_MANAGER::Instance().CreateItem(vItem->GetVnum(), vItem->GetCount(), 0, true);
					if (rewardItem) m_cache.emplace_back(rewardItem);
				}
				for (const auto& rItem : m_cache)
					vec_item.emplace_back(rItem);
			}
		}
	}

	eventPtr = CheckEventIsActive(DUNGEON_CHEST_LOOT_EVENT, killerEmpire, pkKiller);
	if (eventPtr)
	{
		const int prob = number(1, 100);
		const int success_prob = eventPtr->value[3];
	
		const int chest_value0 = eventPtr->value[0];
		const int chest_value1 = eventPtr->value[1];
		const int chest_value2 = eventPtr->value[2];
	
		if (prob < success_prob)
		{
			// If you have different book index put here!
			constexpr std::array<DWORD, 8> m_lbookItems = { 90058, 90059, 90060, 90061, 90062, 90063, 90064, 203031 };
	
			// Temporary storage for new rewards
			std::vector<LPITEM> newItems;
	
			// Use a hash set for fast lookup of mission books
			std::unordered_set<DWORD> bookSet(m_lbookItems.begin(), m_lbookItems.end());
	
			for (const auto& vItem : vec_item)
			{
				const DWORD itemVnum = vItem->GetVnum();
	
				if (bookSet.find(itemVnum) != bookSet.end())
				{
					LPITEM rewardItem = nullptr;
	
					if (pkKiller->FindAffect(AFFECT_MALL) && pkKiller->GetPremiumRemainSeconds(PREMIUM_ITEM) < 1)
					{
						rewardItem = ITEM_MANAGER::Instance().CreateItem(itemVnum, chest_value2, 0, true);
					}
					else if (pkKiller->GetPremiumRemainSeconds(PREMIUM_ITEM) > 1)
					{
						rewardItem = ITEM_MANAGER::Instance().CreateItem(itemVnum, chest_value1, 0, true);
					}
					else
					{
						rewardItem = ITEM_MANAGER::Instance().CreateItem(itemVnum, chest_value0, 0, true);
					}
	
					if (rewardItem)
					{
						newItems.emplace_back(rewardItem);
					}
				}
			}
	
			// Add all new items to vec_item after the iteration
			vec_item.insert(vec_item.end(), newItems.begin(), newItems.end());
		}
	}

	eventPtr = CheckEventIsActive(MOONLIGHT_EVENT, killerEmpire, pkKiller);
	if (eventPtr)
	{
		const int prob = number(1, 100);
		const int success_prob = eventPtr->value[3];
		if (prob < success_prob)
		{
			LPITEM item = ITEM_MANAGER::Instance().CreateItem(50011, 1, 0, true);
			if (item) vec_item.emplace_back(item);
		}
	}

	eventPtr = CheckEventIsActive(VALENTINE_EVENT, killerEmpire, pkKiller);
	if (eventPtr)
	{
		const int prob = number(1, 1000);
		const int success_prob = eventPtr->value[3];
		if (prob < success_prob)
		{
			LPITEM item = ITEM_MANAGER::Instance().CreateItem(50025, 1, 0, true);
			if (item) vec_item.emplace_back(item);
		}
	}
}
void CHARACTER_MANAGER::CompareEventSendData(TEMP_BUFFER* buf)
{
	const BYTE dayCount = m_eventData.size();
	const BYTE subIndex = EVENT_MANAGER_LOAD;
	const int cur_Time = time(NULL);
	TPacketGCEventManager p;
	p.header = HEADER_GC_EVENT_MANAGER;
	p.size = sizeof(TPacketGCEventManager) + sizeof(BYTE)+sizeof(BYTE)+sizeof(int);
	for (const auto& [dayIndex, dayData] : m_eventData)
	{
		const BYTE dayEventCount = dayData.size();
		p.size += sizeof(BYTE) + sizeof(BYTE) + (dayEventCount * sizeof(TEventManagerData));
	}
	buf->write(&p, sizeof(TPacketGCEventManager));
	buf->write(&subIndex, sizeof(BYTE));
	buf->write(&dayCount, sizeof(BYTE));
	buf->write(&cur_Time, sizeof(int));
	for (const auto& [dayIndex, dayData] : m_eventData)
	{
		const BYTE dayEventCount = dayData.size();
		buf->write(&dayIndex, sizeof(BYTE));
		buf->write(&dayEventCount, sizeof(BYTE));
		if (dayEventCount > 0)
			buf->write(dayData.data(), dayEventCount * sizeof(TEventManagerData));
	}
}
void CHARACTER_MANAGER::UpdateAllPlayerEventData()
{
	TEMP_BUFFER buf;
	CompareEventSendData(&buf);
	const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();
	for (const auto& desc : c_ref_set)
	{
		if (!desc->GetCharacter())
			continue;
		desc->Packet(buf.read_peek(), buf.size());
	}
}
void CHARACTER_MANAGER::SendDataPlayer(LPCHARACTER ch)
{
	auto desc = ch->GetDesc();
	if (!desc)
		return;
	TEMP_BUFFER buf;
	CompareEventSendData(&buf);
	desc->Packet(buf.read_peek(), buf.size());
	ch->SendActivatedEventsToClient();
}
bool CHARACTER_MANAGER::CloseEventManuel(BYTE eventIndex)
{
	auto eventPtr = CheckEventIsActive(eventIndex);
	if (eventPtr != NULL)
	{
		const BYTE subHeader = EVENT_MANAGER_REMOVE_EVENT;
		db_clientdesc->DBPacketHeader(HEADER_GD_EVENT_MANAGER, 0, sizeof(BYTE)+sizeof(WORD));
		db_clientdesc->Packet(&subHeader, sizeof(BYTE));
		db_clientdesc->Packet(&eventPtr->eventID, sizeof(WORD));
		return true;
	}
	return false;
}
void CHARACTER_MANAGER::SetEventStatus(const WORD eventID, const bool eventStatus, const int endTime, const char* endTimeText)
{
	//eventStatus - 0-deactive  // 1-active

	TEventManagerData* eventData = NULL;
	for (auto it = m_eventData.begin(); it != m_eventData.end(); ++it)
	{
		if (it->second.size())
		{
			for (DWORD j = 0; j < it->second.size(); ++j)
			{
				TEventManagerData& pData = it->second[j];
				if (pData.eventID == eventID)
				{
					eventData = &pData;
					break;
				}
			}
		}
	}
	if (eventData == NULL)
		return;
	eventData->eventStatus = eventStatus;
	eventData->endTime = endTime;
	strlcpy(eventData->endTimeText, endTimeText, sizeof(eventData->endTimeText));

	// Auto open&close notice
	const std::map<BYTE, std::pair<std::string, std::string>> m_eventText = {
		{BONUS_EVENT,{"[LS;10027]","[LS;10028]"}},
		{DOUBLE_BOSS_LOOT_EVENT,{"[LS;10029]","[LS;10030]"}},
		{DOUBLE_METIN_LOOT_EVENT,{"[LS;10031]","[LS;10032]"}},
		{DOUBLE_MISSION_BOOK_EVENT,{"767","768"}},
		{DUNGEON_COOLDOWN_EVENT,{"769","770"}},
		{DUNGEON_CHEST_LOOT_EVENT,{"771","772"}},
		{MOONLIGHT_EVENT,{"[LS;10033]","[LS;10034]"}},
		{VALENTINE_EVENT,{"Valentynsky event zacal","Valentynsky event je u konce"}},
		{SWITCH_RARE_EVENT,{"[LS;10035]","[LS;10036]"}},
		{SWITCH_AVERAGE_EVENT,{"[LS;10037]","[LS;10038]"}},
		{SWITCH_RUNE_EVENT,{"[LS;10039]","[LS;10040]"}},
		{DOUBLE_ACHIEVEMENT_POINTS,{"[LS;10041]","[LS;10042]"}},
		{MINING_CHEST_EVENT,{"[LS;10043]","[LS;10044]"}},
		{DOUBLE_ORE_EVENT,{"[LS;10045]","[LS;10046]"}},
		{BIGGER_STAR_CHANCE,{"[LS;10047]","[LS;10048]"}},
	};

	const auto it = m_eventText.find(eventData->eventIndex);
	if (it != m_eventText.end())
	{
		if (eventStatus)
			SendNotice(it->second.first.c_str());
		else
			SendNotice(it->second.second.c_str());
	}

	const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();

	// Bonus event update status
	if (eventData->eventIndex == BONUS_EVENT)
	{
		for (const auto& desc : c_ref_set)
		{
			LPCHARACTER ch = desc->GetCharacter();
			if (!ch)
				continue;
			if (eventData->empireFlag != 0)
				if (eventData->empireFlag != ch->GetEmpire())
					continue;
			if (eventData->channelFlag != 0)
				if (eventData->channelFlag != g_bChannel)
					return;
			if (!eventStatus)
			{
				const int32_t value = eventData->value[1];
				ch->ApplyPoint(eventData->value[0], -value);
			}
			ch->ComputePoints();
		}
	}

	const int now = time(0);
	const BYTE subIndex = EVENT_MANAGER_EVENT_STATUS;
	
	TPacketGCEventManager p;
	p.header = HEADER_GC_EVENT_MANAGER;
	p.size = sizeof(TPacketGCEventManager)+sizeof(BYTE)+sizeof(WORD)+sizeof(bool)+sizeof(int)+sizeof(int)+sizeof(eventData->endTimeText);
	
	TEMP_BUFFER buf;
	buf.write(&p, sizeof(TPacketGCEventManager));
	buf.write(&subIndex, sizeof(BYTE));
	buf.write(&eventData->eventID, sizeof(WORD));
	buf.write(&eventData->eventStatus, sizeof(bool));
	buf.write(&eventData->endTime, sizeof(int));
	buf.write(&eventData->endTimeText, sizeof(eventData->endTimeText));
	buf.write(&now, sizeof(int));

	for (const auto& desc : c_ref_set)
	{
		if (!desc->GetCharacter())
			continue;
		desc->Packet(buf.read_peek(), buf.size());
	}

}
void CHARACTER_MANAGER::SetEventData(BYTE dayIndex, const std::vector<TEventManagerData>& m_data)
{
	const auto it = m_eventData.find(dayIndex);
	if (it == m_eventData.end())
		m_eventData.emplace(dayIndex, m_data);
	else
	{
		it->second.clear();
		for (const auto& newEvents : m_data)
			it->second.emplace_back(newEvents);
	}
}

static const int EVENT_PERSONAL_DURATION_DEFAULT = 1800; // fallback: 30 minutes

bool CHARACTER::ActivatePersonalEvent(WORD eventID)
{
	// Once used, can never be activated again regardless of expiry
	if (m_activatedEvents.count(eventID))
		return false;
	m_activatedEvents[eventID] = static_cast<int>(get_global_time());
	return true;
}

bool CHARACTER::IsPersonalEventActivated(WORD eventID) const
{
	auto it = m_activatedEvents.find(eventID);
	if (it == m_activatedEvents.end())
		return false;

	// Look up per-event duration from value[0]; fall back to default if 0
	int duration = EVENT_PERSONAL_DURATION_DEFAULT;
	bool found = false;
	for (const auto& [dayIdx, dayVec] : CHARACTER_MANAGER::Instance().GetEventData())
	{
		if (found) break;
		for (const auto& ev : dayVec)
		{
			if (ev.eventID == eventID)
			{
				if (ev.value[0] > 0)
					duration = static_cast<int>(ev.value[0]);
				found = true;
				break;
			}
		}
	}
	return (static_cast<int>(get_global_time()) - it->second) < duration;
}

bool CHARACTER::HasActivatedEventIndex(BYTE eventIndex) const
{
	return CHARACTER_MANAGER::Instance().CheckEventIsActive(eventIndex, 0, const_cast<CHARACTER*>(this)) != nullptr;
}

void CHARACTER::SetActivatedEvents(const std::vector<std::pair<WORD, int>>& events)
{
	m_activatedEvents.clear();
	for (const auto& e : events)
		m_activatedEvents[e.first] = e.second;
}

void CHARACTER::SendActivatedEventsToClient() const
{
	if (!GetDesc())
		return;

	const BYTE subIndex = EVENT_MANAGER_PLAYER_ACTIVATED;
	const WORD count = static_cast<WORD>(m_activatedEvents.size());

	TPacketGCEventManager p;
	p.header = HEADER_GC_EVENT_MANAGER;
	p.size = sizeof(TPacketGCEventManager) + sizeof(BYTE) + sizeof(WORD)
		+ count * (sizeof(WORD) + sizeof(int));

	TEMP_BUFFER buf;
	buf.write(&p, sizeof(TPacketGCEventManager));
	buf.write(&subIndex, sizeof(BYTE));
	buf.write(&count, sizeof(WORD));
	for (const auto& kv : m_activatedEvents)
	{
		WORD eid = kv.first;
		int ts = kv.second;
		buf.write(&eid, sizeof(WORD));
		buf.write(&ts, sizeof(int));
	}

	GetDesc()->Packet(buf.read_peek(), buf.size());
}

void CHARACTER::LoadActivatedEvents()
{
	m_activatedEvents.clear();
	const std::string hwid = GetCompositeHWID();
	if (hwid.empty())
		return;
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
		"SELECT event_id, activated_at FROM srv1_player.event_hwid_activations WHERE hwid_composite = '%s'",
		hwid.c_str()));
	if (!pMsg || !pMsg->Get() || pMsg->Get()->uiNumRows == 0)
		return;
	MYSQL_ROW row;
	while ((row = mysql_fetch_row(pMsg->Get()->pSQLResult)) != NULL)
	{
		WORD eid = 0;
		int ts = 0;
		str_to_number(eid, row[0]);
		str_to_number(ts, row[1]);
		m_activatedEvents[eid] = ts;
	}
}
#endif
//martysama0134's ceqyqttoaf71vasf9t71218
