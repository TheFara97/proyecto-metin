#include "stdafx.h"

#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "affect.h"
#include "packet.h"
#include "buffer_manager.h"
#include "desc_client.h"
#include "battle.h"
#include "guild.h"
#include "utils.h"
#include "locale_service.h"
#include "lua_incl.h"
#include "arena.h"
#include "horsename_manager.h"
#include "item.h"
#include "DragonSoul.h"
#include "../../common/CommonDefines.h"

#define IS_NO_SAVE_AFFECT(type) ((type) == AFFECT_WAR_FLAG || (type) == AFFECT_REVIVE_INVISIBLE || ((type) >= AFFECT_PREMIUM_START && (type) <= AFFECT_PREMIUM_END) || (type) == AFFECT_MOUNT_BONUS || (type) == AFFECT_VOTE4BUFF || (type >= 770 && type <= 775)) // @fixme156 added MOUNT_BONUS (if the game core crashes, the bonus would double if present in player.affect)
#define IS_NO_CLEAR_ON_DEATH_AFFECT(type) ((type) == AFFECT_BLOCK_CHAT || ((type) >= 500 && (type) < 600) || (type >= AFFECT_COLLECTIONS0 && type <= AFFECT_COLLECT_KOLEKCJONER_EXTRA) || (type == 316) || (type >= 740 && type <= 749) || (type >= 770 && type <= 775) || (type >= 900 && type <= 903) || (type == 750) || (type) == AFFECT_VOTE4BUFF)

void SendAffectRemovePacket(LPDESC d, DWORD pid, DWORD type, BYTE point)
{
	TPacketGCAffectRemove ptoc;
	ptoc.bHeader	= HEADER_GC_AFFECT_REMOVE;
	ptoc.dwType		= type;
	ptoc.bApplyOn	= point;
	d->Packet(&ptoc, sizeof(TPacketGCAffectRemove));

	TPacketGDRemoveAffect ptod;
	ptod.dwPID		= pid;
	ptod.dwType		= type;
	ptod.bApplyOn	= point;
	db_clientdesc->DBPacket(HEADER_GD_REMOVE_AFFECT, 0, &ptod, sizeof(ptod));
}

void SendAffectAddPacket(LPDESC d, CAffect * pkAff)
{
	TPacketGCAffectAdd ptoc;
	ptoc.bHeader		= HEADER_GC_AFFECT_ADD;
	ptoc.elem.dwType		= pkAff->dwType;
	ptoc.elem.bApplyOn		= pkAff->bApplyOn;
	ptoc.elem.lApplyValue	= pkAff->lApplyValue;
	ptoc.elem.dwFlag		= pkAff->dwFlag;
	ptoc.elem.lDuration		= pkAff->lDuration;
	ptoc.elem.lSPCost		= pkAff->lSPCost;
	d->Packet(&ptoc, sizeof(TPacketGCAffectAdd));
}
////////////////////////////////////////////////////////////////////
// Affect
CAffect * CHARACTER::FindAffect(DWORD dwType, BYTE bApply) const
{
	itertype(m_list_pkAffect) it = m_list_pkAffect.begin();

	while (it != m_list_pkAffect.end())
	{
		CAffect * pkAffect = *it++;

		if (pkAffect->dwType == dwType && (bApply == APPLY_NONE || bApply == pkAffect->bApplyOn))
			return pkAffect;
	}

	return NULL;
}

#ifdef ENABLE_SKILL_AFFECT_REMOVE
CAffect* CHARACTER::FindAffectByFlag(DWORD dwFlag) const
{
itertype(m_list_pkAffect) it = m_list_pkAffect.begin();

	while (it != m_list_pkAffect.end())
	{
		CAffect* pkAffect = *it++;

		if (!pkAffect)// @fixme199
			return NULL;
		
		if (pkAffect->dwFlag == dwFlag)
			return pkAffect;
	}

	return NULL;
}
#endif

EVENTFUNC(affect_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "affect_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL) { // <Factor>
		return 0;
	}

	if (!ch->UpdateAffect())
		return 0;
	else
		return passes_per_sec;
}

bool CHARACTER::UpdateAffect()
{
	if (GetPoint(POINT_HP_RECOVERY) > 0)
	{
		if (GetMaxHP() <= GetHP())
		{
			PointChange(POINT_HP_RECOVERY, -GetPoint(POINT_HP_RECOVERY));
		}
		else
		{
			int iVal = MIN(GetPoint(POINT_HP_RECOVERY), GetMaxHP() * 7 / 100);

			PointChange(POINT_HP, iVal);
			PointChange(POINT_HP_RECOVERY, -iVal);
		}
	}

	if (GetPoint(POINT_SP_RECOVERY) > 0)
	{
		if (GetMaxSP() <= GetSP())
			PointChange(POINT_SP_RECOVERY, -GetPoint(POINT_SP_RECOVERY));
		else
		{
			int iVal = MIN(GetPoint(POINT_SP_RECOVERY), GetMaxSP() * 7 / 100);

			PointChange(POINT_SP, iVal);
			PointChange(POINT_SP_RECOVERY, -iVal);
		}
	}

	if (GetPoint(POINT_HP_RECOVER_CONTINUE) > 0)
	{
		PointChange(POINT_HP, GetPoint(POINT_HP_RECOVER_CONTINUE));
	}

	if (GetPoint(POINT_SP_RECOVER_CONTINUE) > 0)
	{
		PointChange(POINT_SP, GetPoint(POINT_SP_RECOVER_CONTINUE));
	}

	AutoRecoveryItemProcess(AFFECT_AUTO_HP_RECOVERY);
	AutoRecoveryItemProcess(AFFECT_AUTO_SP_RECOVERY);

	if (GetMaxStamina() > GetStamina())
	{
		int iSec = (get_dword_time() - GetStopTime()) / 3000;
		if (iSec)
			PointChange(POINT_STAMINA, GetMaxStamina()/1);
	}

/*** Fix visual bug skill for Warrior and Sura ***/
   if (IsAffectFlag(AFF_GWIGUM) && !GetWear(WEAR_WEAPON))
   {
      RemoveAffect(SKILL_GWIGEOM);
   }

   if (IsAffectFlag(AFF_GEOMGYEONG) && !GetWear(WEAR_WEAPON))
   {
      RemoveAffect(SKILL_GEOMKYUNG);
   }
/*** End fix ***/

	if (ProcessAffect())
		if (GetPoint(POINT_HP_RECOVERY) == 0 && GetPoint(POINT_SP_RECOVERY) == 0 && GetStamina() == GetMaxStamina())
		{
			m_pkAffectEvent = NULL;
			return false;
		}

	return true;
}

void CHARACTER::StartAffectEvent()
{
	if (m_pkAffectEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();
	info->ch = this;
	m_pkAffectEvent = event_create(affect_event, info, passes_per_sec);
	sys_log(1, "StartAffectEvent %s %p %p", GetName(), this, get_pointer(m_pkAffectEvent));
}

void CHARACTER::ClearAffect(bool bSave
#ifdef ENABLE_NO_CLEAR_BUFF_WHEN_MONSTER_KILL
	, bool letBuffs
#endif
)
{
	TAffectFlag afOld = m_afAffectFlag;
	WORD	wMovSpd = GetPoint(POINT_MOV_SPEED);
	WORD	wAttSpd = GetPoint(POINT_ATT_SPEED);

	itertype(m_list_pkAffect) it = m_list_pkAffect.begin();

	while (it != m_list_pkAffect.end())
	{
		CAffect * pkAff = *it;

		if (bSave)
		{
			if ( IS_NO_CLEAR_ON_DEATH_AFFECT(pkAff->dwType) || IS_NO_SAVE_AFFECT(pkAff->dwType)
#ifdef __VIP_SYSTEM__
					|| pkAff->dwType == EAffectTypes::AFFECT_VIP
#endif
			)
			{
				++it;
				continue;
			}
#ifdef ENABLE_MULTI_FARM_BLOCK
			if (pkAff->dwType == AFFECT_MULTI_FARM_PREMIUM)
			{
				++it;
				continue;
			}
#endif
#ifdef ENABLE_NO_CLEAR_BUFF_WHEN_MONSTER_KILL
			if (letBuffs)
			{
				switch (pkAff->dwType)
				{
					case (SKILL_JEONGWI):
					case (SKILL_GEOMKYUNG):
					case (SKILL_CHUNKEON):
					case (SKILL_GWIGEOM):
					case (SKILL_TERROR):
					case (SKILL_JUMAGAP):
					case (SKILL_HOSIN):
					case (SKILL_REFLECT):
					case (SKILL_GICHEON):
					case (SKILL_KWAESOK):
					case (SKILL_JEUNGRYEOK):
					//case (SKILL_JEOKRANG):
					//case (SKILL_CHEONGRANG): // wolfman skill
					{
						++it;
						continue;
					}
				}
			}
#endif
#ifdef __ENABLE_COLLECTIONS_SYSTEM__
			if (pkAff->dwType == AFFECT_COLLECTIONS0 || pkAff->dwType == AFFECT_COLLECTIONS1 || pkAff->dwType == AFFECT_COLLECTIONS2 || pkAff->dwType == AFFECT_COLLECTIONS3 || pkAff->dwType == AFFECT_COLLECTIONS4 || pkAff->dwType == AFFECT_COLLECTIONS5 || pkAff->dwType == AFFECT_COLLECTIONS6 || pkAff->dwType == AFFECT_COLLECTIONS7 || pkAff->dwType == AFFECT_COLLECT_POLOWANIE)
			{
				++it;
				continue;
			}
#endif
#ifdef __AUTO_QUQUE_ATTACK__
			if (pkAff->dwType == AFFECT_AUTO_METIN_FARM)
			{
				++it;
				continue;
			}
#endif
			if (IsPC())
			{
				SendAffectRemovePacket(GetDesc(), GetPlayerID(), pkAff->dwType, pkAff->bApplyOn);
			}
		}

		ComputeAffect(pkAff, false);

		it = m_list_pkAffect.erase(it);
		CAffect::Release(pkAff);
	}

	if (afOld != m_afAffectFlag ||
			wMovSpd != GetPoint(POINT_MOV_SPEED) ||
			wAttSpd != GetPoint(POINT_ATT_SPEED))
		UpdatePacket();

	CheckMaximumPoints();

	if (m_list_pkAffect.empty())
		event_cancel(&m_pkAffectEvent);
}

int CHARACTER::ProcessAffect()
{
	bool	bDiff	= false;
	CAffect	*pkAff	= NULL;

	for (int i = 0; i <= PREMIUM_MAX_NUM; ++i)
	{
		int aff_idx = i + AFFECT_PREMIUM_START;

		pkAff = FindAffect(aff_idx);

		if (!pkAff)
			continue;

#ifdef __PREMIUM_PRIVATE_SHOP__
		if (GetPremiumRemainSeconds(PREMIUM_PRIVATE_SHOP))
			continue;
#endif

		int remain = GetPremiumRemainSeconds(i);

		if (remain < 0)
		{
			RemoveAffect(aff_idx);
			bDiff = true;
		}
		else
			pkAff->lDuration = remain + 1;
	}

	////////// HAIR_AFFECT
	pkAff = FindAffect(AFFECT_HAIR);
	if (pkAff)
	{
		// IF HAIR_LIMIT_TIME() < CURRENT_TIME()
		if ( this->GetQuestFlag("hair.limit_time") < get_global_time())
		{
			// SET HAIR NORMAL
			this->SetPart(PART_HAIR, 0);
			// REMOVE HAIR AFFECT
			RemoveAffect(AFFECT_HAIR);
		}
		else
		{
			// INCREASE AFFECT DURATION
			++(pkAff->lDuration);
		}
	}
	////////// HAIR_AFFECT

	CHorseNameManager::instance().Validate(this);

	TAffectFlag afOld = m_afAffectFlag;
	int32_t lMovSpd = GetPoint(POINT_MOV_SPEED);
	int32_t lAttSpd = GetPoint(POINT_ATT_SPEED);

	itertype(m_list_pkAffect) it;

	it = m_list_pkAffect.begin();

	while (it != m_list_pkAffect.end())
	{
		pkAff = *it;

		bool bEnd = false;

		if (pkAff->dwType >= GUILD_SKILL_START && pkAff->dwType <= GUILD_SKILL_END)
		{
			if (!GetGuild() || !GetGuild()->UnderAnyWar())
				bEnd = true;
		}

		if (pkAff->lSPCost > 0)
		{
			if (GetSP() < pkAff->lSPCost)
				bEnd = true;
			else
				PointChange(POINT_SP, -pkAff->lSPCost);
		}

		// AFFECT_DURATION_BUG_FIX
#ifdef __PREMIUM_PRIVATE_SHOP__
		if (pkAff->dwType == AFFECT_PREMIUM_PRIVATE_SHOP)
		{
		}
		else
#endif
		if ( --pkAff->lDuration <= 0 )
		{
			bEnd = true;
		}
		// END_AFFECT_DURATION_BUG_FIX

		if (bEnd)
		{
			it = m_list_pkAffect.erase(it);
			ComputeAffect(pkAff, false);
			bDiff = true;
			if (IsPC())
			{
				SendAffectRemovePacket(GetDesc(), GetPlayerID(), pkAff->dwType, pkAff->bApplyOn);
			}

			CAffect::Release(pkAff);

			continue;
		}

		++it;
	}

	if (bDiff)
	{
		if (afOld != m_afAffectFlag ||
				lMovSpd != GetPoint(POINT_MOV_SPEED) ||
				lAttSpd != GetPoint(POINT_ATT_SPEED))
		{
			UpdatePacket();
		}

		CheckMaximumPoints();
	}

	if (m_list_pkAffect.empty())
		return true;

	return false;
}

void CHARACTER::SaveAffect()
{
	TPacketGDAddAffect p;

	itertype(m_list_pkAffect) it = m_list_pkAffect.begin();

	while (it != m_list_pkAffect.end())
	{
		CAffect * pkAff = *it++;

		if (IS_NO_SAVE_AFFECT(pkAff->dwType))
			continue;

		sys_log(1, "AFFECT_SAVE: %u %u %d %d", pkAff->dwType, pkAff->bApplyOn, pkAff->lApplyValue, pkAff->lDuration);

		p.dwPID			= GetPlayerID();
		p.elem.dwType		= pkAff->dwType;
		p.elem.bApplyOn		= pkAff->bApplyOn;
		p.elem.lApplyValue	= pkAff->lApplyValue;
		p.elem.dwFlag		= pkAff->dwFlag;
		p.elem.lDuration	= pkAff->lDuration;
		p.elem.lSPCost		= pkAff->lSPCost;
		db_clientdesc->DBPacket(HEADER_GD_ADD_AFFECT, 0, &p, sizeof(p));
	}
}

EVENTINFO(load_affect_login_event_info)
{
	DWORD pid;
	DWORD count;
	char* data;

	load_affect_login_event_info()
	: pid( 0 )
	, count( 0 )
	, data( 0 )
	{
	}
};

EVENTFUNC(load_affect_login_event)
{
	load_affect_login_event_info* info = dynamic_cast<load_affect_login_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "load_affect_login_event_info> <Factor> Null pointer" );
		return 0;
	}

	DWORD dwPID = info->pid;
	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(dwPID);

	if (!ch)
	{
		M2_DELETE_ARRAY(info->data);
		return 0;
	}

	LPDESC d = ch->GetDesc();

	if (!d)
	{
		M2_DELETE_ARRAY(info->data);
		return 0;
	}

	if (d->IsPhase(PHASE_HANDSHAKE) ||
			d->IsPhase(PHASE_LOGIN) ||
			d->IsPhase(PHASE_SELECT) ||
			d->IsPhase(PHASE_DEAD) ||
			d->IsPhase(PHASE_LOADING))
	{
		return PASSES_PER_SEC(1);
	}
	else if (d->IsPhase(PHASE_CLOSE))
	{
		M2_DELETE_ARRAY(info->data);
		return 0;
	}
	else if (d->IsPhase(PHASE_GAME))
	{
		sys_log(1, "Affect Load by Event");
		ch->LoadAffect(info->count, (TPacketAffectElement*)info->data);
		M2_DELETE_ARRAY(info->data);
		return 0;
	}
	else
	{
		sys_err("input_db.cpp:quest_login_event INVALID PHASE pid %d", ch->GetPlayerID());
		M2_DELETE_ARRAY(info->data);
		return 0;
	}
}

void CHARACTER::LoadAffect(DWORD dwCount, TPacketAffectElement * pElements)
{
	m_bIsLoadedAffect = false;

	if (!GetDesc()->IsPhase(PHASE_GAME))
	{
		if (test_server)
			sys_log(0, "LOAD_AFFECT: Creating Event", GetName(), dwCount);

		load_affect_login_event_info* info = AllocEventInfo<load_affect_login_event_info>();

		info->pid = GetPlayerID();
		info->count = dwCount;
		info->data = M2_NEW char[sizeof(TPacketAffectElement) * dwCount];
		thecore_memcpy(info->data, pElements, sizeof(TPacketAffectElement) * dwCount);

		event_create(load_affect_login_event, info, PASSES_PER_SEC(1));

		return;
	}

	ClearAffect(true);

	if (test_server)
		sys_log(0, "LOAD_AFFECT: %s count %d", GetName(), dwCount);

	TAffectFlag afOld = m_afAffectFlag;

	int32_t lMovSpd = GetPoint(POINT_MOV_SPEED);
	int32_t lAttSpd = GetPoint(POINT_ATT_SPEED);

	for (DWORD i = 0; i < dwCount; ++i, ++pElements)
	{
		if (pElements->dwType == SKILL_MUYEONG)
			continue;

		if (AFFECT_AUTO_HP_RECOVERY == pElements->dwType || AFFECT_AUTO_SP_RECOVERY == pElements->dwType)
		{
			LPITEM item = FindItemByID( pElements->dwFlag );

			if (NULL == item)
				continue;

			item->Lock(true);
		}

		if (pElements->bApplyOn >= POINT_MAX_NUM)
		{
			sys_err("invalid affect data %s ApplyOn %u ApplyValue %d",
					GetName(), pElements->bApplyOn, pElements->lApplyValue);
			continue;
		}

		if (test_server)
		{
			sys_log(0, "Load Affect : Affect %s %d %d", GetName(), pElements->dwType, pElements->bApplyOn );
		}

		CAffect* pkAff = CAffect::Acquire();
		m_list_pkAffect.push_back(pkAff);

		pkAff->dwType		= pElements->dwType;
		pkAff->bApplyOn		= pElements->bApplyOn;
		pkAff->lApplyValue	= pElements->lApplyValue;
		pkAff->dwFlag		= pElements->dwFlag;
		pkAff->lDuration	= pElements->lDuration;
		pkAff->lSPCost		= pElements->lSPCost;

		SendAffectAddPacket(GetDesc(), pkAff);

		ComputeAffect(pkAff, true);

	}

	if ( CArenaManager::instance().IsArenaMap(GetMapIndex()) == true )
	{
		RemoveGoodAffect();
	}

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	RemoveAffect(AFFECT_MOUNT);
	RemoveAffect(AFFECT_MOUNT_BONUS);
#endif


	if (afOld != m_afAffectFlag || lMovSpd != GetPoint(POINT_MOV_SPEED) || lAttSpd != GetPoint(POINT_ATT_SPEED))
	{
		UpdatePacket();
	}

	StartAffectEvent();

	m_bIsLoadedAffect = true;

	ComputePoints(); // @fixme156
	DragonSoul_Initialize();
#ifdef ENABLE_VOTE4BUFF
	Vote4Buff_Initialize();
#endif

	// @fixme118 BEGIN (regain affect hp/mp)
	if (!IsDead())
	{
		PointChange(POINT_HP, GetMaxHP() - GetHP());
		PointChange(POINT_SP, GetMaxSP() - GetSP());
	}
	// @fixme118 END
}

bool CHARACTER::AddAffect(DWORD dwType, BYTE bApplyOn, int32_t lApplyValue, DWORD dwFlag, int32_t lDuration, int32_t lSPCost, bool bOverride, bool IsCube )
{
	// CHAT_BLOCK
	if (dwType == AFFECT_BLOCK_CHAT && lDuration > 1)
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;987]");
	}
	// END_OF_CHAT_BLOCK

	if (lDuration == 0)
	{
		sys_err("Character::AddAffect lDuration == 0 type %d", lDuration, dwType);
		lDuration = 1;
	}

#ifdef __SKILL_COSTUME__
	switch (dwType)
	{
		case SKILL_HOSIN:
		case SKILL_REFLECT:
		case SKILL_GICHEON:
		case SKILL_JEONGEOP:
		case SKILL_KWAESOK:
		case SKILL_JEUNGRYEOK:
		{
			CAffect * pkAffect = FindAffect(dwType);
			if (!pkAffect)
				break;

			RemoveAffect(pkAffect);	
		}
		break;
		default:
			break;
	}
#endif	
	
#if defined(ENABLE_IGNORE_LOWER_BUFFS)
	switch (dwType)
	{
		case SKILL_HOSIN:
		case SKILL_REFLECT:
		case SKILL_GICHEON:
		case SKILL_JEONGEOP:
		case SKILL_KWAESOK:
		case SKILL_JEUNGRYEOK:
		{
			const CAffect * pkAffect = FindAffect(dwType);
			if (!pkAffect)
				break;
				
			if (lApplyValue < pkAffect->lApplyValue)
			{
				//ChatPacket(CHAT_TYPE_INFO, "<AddAffect> has blocked receiving skill (%s) because power is (%ld%%) more small then current one (%ld%%).", CSkillManager::instance().Get(dwType)->szName, lApplyValue, pkAffect->lApplyValue);
				return false;
			}
		}
		break;
		default:
			break;
	}
#endif

	CAffect * pkAff = NULL;

	if (IsCube)
		pkAff = FindAffect(dwType,bApplyOn);
	else
		pkAff = FindAffect(dwType);

	if (dwFlag == AFF_STUN)
	{
		if (m_posDest.x != GetX() || m_posDest.y != GetY())
		{
			m_posDest.x = m_posStart.x = GetX();
			m_posDest.y = m_posStart.y = GetY();
			battle_end(this);

			SyncPacket();
		}
	}

	if (pkAff && bOverride)
	{
		ComputeAffect(pkAff, false);

		if (GetDesc())
			SendAffectRemovePacket(GetDesc(), GetPlayerID(), pkAff->dwType, pkAff->bApplyOn);
	}
	else
	{
		pkAff = CAffect::Acquire();
		m_list_pkAffect.push_back(pkAff);

	}

	sys_log(1, "AddAffect %s type %d apply %d %d flag %u duration %d", GetName(), dwType, bApplyOn, lApplyValue, dwFlag, lDuration);
	sys_log(0, "AddAffect %s type %d apply %d %d flag %u duration %d", GetName(), dwType, bApplyOn, lApplyValue, dwFlag, lDuration);

	pkAff->dwType	= dwType;
	pkAff->bApplyOn	= bApplyOn;
	pkAff->lApplyValue	= lApplyValue;
	pkAff->dwFlag	= dwFlag;
	pkAff->lDuration	= lDuration;
	pkAff->lSPCost	= lSPCost;

	WORD wMovSpd = GetPoint(POINT_MOV_SPEED);
	WORD wAttSpd = GetPoint(POINT_ATT_SPEED);

	ComputeAffect(pkAff, true);

	if (pkAff->dwFlag || wMovSpd != GetPoint(POINT_MOV_SPEED) || wAttSpd != GetPoint(POINT_ATT_SPEED))
		UpdatePacket();

	StartAffectEvent();

	if (IsPC())
	{
		SendAffectAddPacket(GetDesc(), pkAff);

		if (IS_NO_SAVE_AFFECT(pkAff->dwType))
			return true;

		TPacketGDAddAffect p;
		p.dwPID			= GetPlayerID();
		p.elem.dwType		= pkAff->dwType;
		p.elem.bApplyOn		= pkAff->bApplyOn;
		p.elem.lApplyValue	= pkAff->lApplyValue;
		p.elem.dwFlag		= pkAff->dwFlag;
		p.elem.lDuration	= pkAff->lDuration;
		p.elem.lSPCost		= pkAff->lSPCost;
		db_clientdesc->DBPacket(HEADER_GD_ADD_AFFECT, 0, &p, sizeof(p));
	}

	return true;
}

void CHARACTER::RefreshAffect()
{
	itertype(m_list_pkAffect) it = m_list_pkAffect.begin();

	while (it != m_list_pkAffect.end())
	{
		CAffect * pkAff = *it++;
		ComputeAffect(pkAff, true);
	}
}

void CHARACTER::ComputeAffect(CAffect * pkAff, bool bAdd)
{
#ifdef __ENABLE_ITEM_TOGGLE__
	if (pkAff->dwType == AFFECT_TOGGLE_BLOCK) return;
#endif

	if (bAdd && pkAff->dwType >= GUILD_SKILL_START && pkAff->dwType <= GUILD_SKILL_END)
	{
		if (!GetGuild())
			return;

		if (!GetGuild()->UnderAnyWar())
			return;
	}

	if (pkAff->dwFlag)
	{
		if (!bAdd)
			m_afAffectFlag.Reset(pkAff->dwFlag);
		else
			m_afAffectFlag.Set(pkAff->dwFlag);
	}

	if (bAdd)
		PointChange(pkAff->bApplyOn, pkAff->lApplyValue);
	else
		PointChange(pkAff->bApplyOn, -pkAff->lApplyValue);

	if (pkAff->dwType == SKILL_MUYEONG)
	{
		if (bAdd)
			StartMuyeongEvent();
		else
			StopMuyeongEvent();
	}
}

bool CHARACTER::RemoveAffect(CAffect * pkAff)
{
	if (!pkAff)
		return false;

	// AFFECT_BUF_FIX
	m_list_pkAffect.remove(pkAff);
	// END_OF_AFFECT_BUF_FIX

	ComputeAffect(pkAff, false);

	if (AFFECT_REVIVE_INVISIBLE != pkAff->dwType)
		ComputePoints();
	else  // @fixme110
		UpdatePacket();

	CheckMaximumPoints();

	if (test_server)
		sys_log(0, "AFFECT_REMOVE: %s (flag %u apply: %u)", GetName(), pkAff->dwFlag, pkAff->bApplyOn);

	if (IsPC())
	{
		SendAffectRemovePacket(GetDesc(), GetPlayerID(), pkAff->dwType, pkAff->bApplyOn);
	}

	CAffect::Release(pkAff);
	return true;
}

bool CHARACTER::RemoveAffect(DWORD dwType)
{
	// CHAT_BLOCK
	if (dwType == AFFECT_BLOCK_CHAT)
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;988]");
	}
	// END_OF_CHAT_BLOCK

	bool flag = false;

	CAffect * pkAff;

	while ((pkAff = FindAffect(dwType)))
	{
		RemoveAffect(pkAff);
		flag = true;
	}

	return flag;
}

bool CHARACTER::IsAffectFlag(DWORD dwAff) const
{
	return m_afAffectFlag.IsSet(dwAff);
}

void CHARACTER::RemoveGoodAffect()
{
	RemoveAffect(AFFECT_MOV_SPEED);
	RemoveAffect(AFFECT_ATT_SPEED);
	RemoveAffect(AFFECT_STR);
	RemoveAffect(AFFECT_DEX);
	RemoveAffect(AFFECT_INT);
	RemoveAffect(AFFECT_CON);
	RemoveAffect(AFFECT_CHINA_FIREWORK);
	RemoveAffect(SKILL_JEONGWI);
	RemoveAffect(SKILL_GEOMKYUNG);
	RemoveAffect(SKILL_CHUNKEON);
	RemoveAffect(SKILL_EUNHYUNG);
	RemoveAffect(SKILL_GYEONGGONG);
	RemoveAffect(SKILL_GWIGEOM);
	RemoveAffect(SKILL_TERROR);
	RemoveAffect(SKILL_JUMAGAP);
	RemoveAffect(SKILL_MANASHILED);
	RemoveAffect(SKILL_HOSIN);
	RemoveAffect(SKILL_REFLECT);
	RemoveAffect(SKILL_KWAESOK);
	RemoveAffect(SKILL_JEUNGRYEOK);
	RemoveAffect(SKILL_GICHEON);
}

bool CHARACTER::IsGoodAffect(BYTE bAffectType) const
{
	switch (bAffectType)
	{
		case (AFFECT_MOV_SPEED):
		case (AFFECT_ATT_SPEED):
		case (AFFECT_STR):
		case (AFFECT_DEX):
		case (AFFECT_INT):
		case (AFFECT_CON):
		case (AFFECT_CHINA_FIREWORK):
		case (SKILL_JEONGWI):
		case (SKILL_GEOMKYUNG):
		case (SKILL_CHUNKEON):
		case (SKILL_EUNHYUNG):
		case (SKILL_GYEONGGONG):
		case (SKILL_GWIGEOM):
		case (SKILL_TERROR):
		case (SKILL_JUMAGAP):
		case (SKILL_MANASHILED):
		case (SKILL_HOSIN):
		case (SKILL_REFLECT):
		case (SKILL_KWAESOK):
		case (SKILL_JEUNGRYEOK):
		case (SKILL_GICHEON):
			return true;
	}
	return false;
}

void CHARACTER::RemoveBadAffect()
{
	sys_log(0, "RemoveBadAffect %s", GetName());
	RemovePoison();
#ifdef ENABLE_WOLFMAN_CHARACTER
	RemoveBleeding();
#endif
	RemoveFire();

	RemoveAffect(AFFECT_STUN);

	RemoveAffect(AFFECT_SLOW);

	RemoveAffect(SKILL_TUSOK);

	//RemoveAffect(SKILL_CURSE);

	//RemoveAffect(SKILL_PABUP);

	//RemoveAffect(AFFECT_FAINT);

	//RemoveAffect(AFFECT_WEB);

	//RemoveAffect(AFFECT_SLEEP);

	//RemoveAffect(AFFECT_CURSE);

	//RemoveAffect(AFFECT_PARALYZE);

	//RemoveAffect(SKILL_BUDONG);
}

#ifdef ENABLE_VOTE4BUFF
static std::string Vote4Buff_GetHWID(CHARACTER* ch)
{
	if (!ch->IsPC() || !ch->GetDesc())
		return "";

	const DWORD aid = ch->GetDesc()->GetAccountTable().id;
	char query[512];
	snprintf(query, sizeof(query),
		"SELECT COALESCE("
			"NULLIF(hwid,''),"
			"NULLIF(cshield_hwid,''),"
			"NULLIF(CONCAT(IFNULL(cpu_id,''),'|',IFNULL(hdd_model,''),'|',IFNULL(machine_guid,''),'|',IFNULL(mac_addr,''),'|',IFNULL(hdd_serial,'')),'||||')"
		") FROM srv1_account.account WHERE id=%u", aid);

	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(query));
	if (!pMsg || !pMsg->Get() || !pMsg->Get()->pSQLResult)
		return "";

	MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
	return (row && row[0]) ? std::string(row[0]) : "";
}

static void Vote4Buff_ApplyBonus(CHARACTER* ch, int bonusIndex, int timeLeft)
{
	if (bonusIndex < 1 || bonusIndex > (int)s_VOTE4BUFF_BONUSES.size())
		return;
	ch->RemoveAffect(AFFECT_VOTE4BUFF);
	for (const auto& bonus : s_VOTE4BUFF_BONUSES[bonusIndex - 1])
		ch->AddAffect(AFFECT_VOTE4BUFF, aApplyInfo[bonus.first].bPointType, bonus.second, 0, timeLeft, 0, false);
	ch->SetQuestFlag("vote4buff.selected_bonus", bonusIndex);
}

void CHARACTER::Vote4Buff_Initialize()
{
	// Reapply the affect on login using quest flags already loaded from DB.
	const int iLastVote      = GetQuestFlag("vote4buff.last_vote");
	const int iSelectedBonus = GetQuestFlag("vote4buff.selected_bonus");
	const bool isVoteValid   = iLastVote && MAX(0, get_global_time() - iLastVote) <= s_VOTE4BUFF_DURATION;

	if (isVoteValid && iSelectedBonus)
	{
		RemoveAffect(AFFECT_VOTE4BUFF);
		const int timeLeft = MAX(0, (iLastVote + s_VOTE4BUFF_DURATION) - get_global_time());
		const int idx = iSelectedBonus - 1;
		if (idx < (int)s_VOTE4BUFF_BONUSES.size() && timeLeft)
		{
			for (int i = 0; i < (int)s_VOTE4BUFF_BONUSES[idx].size(); ++i)
			{
				const std::pair<int, int> bonus = s_VOTE4BUFF_BONUSES[idx].at(i);
				AddAffect(AFFECT_VOTE4BUFF, aApplyInfo[bonus.first].bPointType, bonus.second, 0, timeLeft, 0, false);
			}
		}
	}
}

// Called from input_login.cpp right before Vote4Buff_SendClientInfo(true),
// after quest flags are guaranteed to be loaded.
// Checks if another account on the same HWID already voted and had a bonus selected today,
// and if so auto-applies it so the button is hidden and bonus is active on login.
void CHARACTER::Vote4Buff_CheckHWID()
{
	const int iLastVote  = GetQuestFlag("vote4buff.last_vote");
	const bool isVoteValid = iLastVote && MAX(0, get_global_time() - iLastVote) <= s_VOTE4BUFF_DURATION;
	if (isVoteValid)
		return; // already has own vote, nothing to do

	const std::string hwid = Vote4Buff_GetHWID(this);
	if (hwid.empty())
		return;

	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
		"SELECT bonus_index, reward_time FROM srv1_player.vote4buff_hwid "
		"WHERE hwid_composite = '%s' AND bonus_index > 0 AND reward_time > %d LIMIT 1",
		hwid.c_str(), (int)(get_global_time() - s_VOTE4BUFF_DURATION)));

	if (!pMsg || !pMsg->Get() || pMsg->Get()->uiNumRows == 0)
		return;

	MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
	int bonusIndex = 0, rewardTime = 0;
	str_to_number(bonusIndex, row[0]);
	str_to_number(rewardTime, row[1]);

	const int timeLeft = MAX(0, (rewardTime + s_VOTE4BUFF_DURATION) - get_global_time());
	SetQuestFlag("vote4buff.last_vote", rewardTime);
	SetQuestFlag("vote4buff.change_bonus_cd", 0);
	Vote4Buff_ApplyBonus(this, bonusIndex, timeLeft);
}

void CHARACTER::Vote4Buff_SendClientInfo(bool withBonusInfo)
{
	if (withBonusInfo)
	{
		std::string data = "";

		for (int i = 0; i < s_VOTE4BUFF_ITEM_REWARDS.size(); ++i)
		{
			if (!data.empty()) data += "|";

			const std::pair<DWORD, int> item = s_VOTE4BUFF_ITEM_REWARDS.at(i);
			data += std::to_string(item.first) + ":" + std::to_string(item.second);
		}

		data += "#";

		for (int i = 0; i < s_VOTE4BUFF_BONUSES.size(); ++i)
		{
			if (i > 0) data += "|";

			for (int j = 0; j < s_VOTE4BUFF_BONUSES.at(i).size(); ++j)
			{
				if (j > 0) data += ",";

				const std::pair<int, int> bonus = s_VOTE4BUFF_BONUSES.at(i).at(j);
				data += std::to_string(bonus.first) + ":" + std::to_string(bonus.second);
			}
		}

		data += "#" + std::to_string(s_VOTE4BUFF_CHANGE_BONUS_PRICE);

		ChatPacket(CHAT_TYPE_COMMAND, "vote4buff_d %s", data.c_str());
		ChatPacket(CHAT_TYPE_COMMAND, "vote4buff_a %s", Vote4Buff_GetApiURL().c_str());
		ChatPacket(CHAT_TYPE_COMMAND, "vote4buff_u %s", Vote4Buff_GetURL().c_str());
	}

	ChatPacket(CHAT_TYPE_COMMAND, "vote4buff %d:%d:%d",
		GetQuestFlag("vote4buff.last_vote"),
		GetQuestFlag("vote4buff.selected_bonus"),
		GetQuestFlag("vote4buff.change_bonus_cd"));
}

bool CHARACTER::Vote4Buff_IsVoted()
{
	if (!IsLoadedAffect())
		return false;

	const int iLastVoteTime = GetQuestFlag("vote4buff.last_vote");
	if (!iLastVoteTime)
		return false;

	return MAX(0, get_global_time() - iLastVoteTime) <= s_VOTE4BUFF_DURATION;
}

void CHARACTER::Vote4Buff_Vote(int32_t time_left, bool is_main_voter)
{
	// avoid possible exploits by having duplicated affects
	if (!IsLoadedAffect())
		return;

	if (Vote4Buff_IsVoted())
		return;

	// remove the old affect if it exists
	RemoveAffect(AFFECT_VOTE4BUFF);

	// assign vote data to the player's data
	SetQuestFlag("vote4buff.last_vote", get_global_time() - MAX(0, s_VOTE4BUFF_DURATION - time_left));
	SetQuestFlag("vote4buff.selected_bonus", 0);
	
	// reset the change bonus cooldown on casting the vote so you can select a bonus
	SetQuestFlag("vote4buff.change_bonus_cd", 0);

	if (is_main_voter)
	{
		const std::string hwid = Vote4Buff_GetHWID(this);
		bool bHWIDAlreadyRewarded = false;
		int  iHWIDRewardTime = 0;
		int  iHWIDBonusIndex = 0;

		if (!hwid.empty())
		{
			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
				"SELECT bonus_index, reward_time FROM srv1_player.vote4buff_hwid "
				"WHERE hwid_composite = '%s' AND reward_time > %d LIMIT 1",
				hwid.c_str(), (int)(get_global_time() - s_VOTE4BUFF_DURATION)));

			if (pMsg && pMsg->Get() && pMsg->Get()->uiNumRows > 0)
			{
				MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
				str_to_number(iHWIDBonusIndex, row[0]);
				str_to_number(iHWIDRewardTime, row[1]);
				bHWIDAlreadyRewarded = true;
			}
		}

		if (!bHWIDAlreadyRewarded)
		{
			if (!s_VOTE4BUFF_ITEM_REWARDS.empty())
			{
				for (int i = 0; i < (int)s_VOTE4BUFF_ITEM_REWARDS.size(); ++i)
				{
					const std::pair<DWORD, int> item = s_VOTE4BUFF_ITEM_REWARDS.at(i);
					AutoGiveItem(item.first, item.second);
				}
			}
			if (!hwid.empty())
			{
				const int now = get_global_time();
				DBManager::instance().DirectQuery(
					"INSERT INTO srv1_player.vote4buff_hwid (hwid_composite, bonus_index, reward_time) "
					"VALUES ('%s', 0, %d) ON DUPLICATE KEY UPDATE bonus_index = 0, reward_time = %d",
					hwid.c_str(), now, now);
			}
		}
		else
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your hardware already received the vote reward today."));
			if (iHWIDBonusIndex > 0)
			{
				const int timeLeft = MAX(0, (iHWIDRewardTime + s_VOTE4BUFF_DURATION) - get_global_time());
				Vote4Buff_ApplyBonus(this, iHWIDBonusIndex, timeLeft);
			}
		}
	}

	Vote4Buff_SendClientInfo();
}

void CHARACTER::Vote4Buff_SelectBonus(int index)
{
	index = MINMAX(1, index, 3);

	// avoid possible exploits by having duplicated affects
	if (!IsLoadedAffect())
		return;

	if (!Vote4Buff_IsVoted())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need to vote before selecting a bonus."));
		return;
	}

	const int cooldown = GetQuestFlag("vote4buff.change_bonus_cd");
	if (cooldown > get_global_time())
	{
		// seconds variable in case you change the displayed chat message to display the time left
		const int seconds = MAX(0, cooldown - get_global_time());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are in cooldown, you must wait to change the bonus again."));
		return;
	}
	
	const bool isApplyCooldown = GetQuestFlag("vote4buff.selected_bonus") != 0;
	if (isApplyCooldown) // player is changing the bonus with one already selected
	{
		if (s_VOTE4BUFF_CHANGE_BONUS_PRICE && GetGold() < s_VOTE4BUFF_CHANGE_BONUS_PRICE)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have enough Yang."));
			return;
		}

		PointChange(POINT_GOLD, -s_VOTE4BUFF_CHANGE_BONUS_PRICE);
		SetQuestFlag("vote4buff.change_bonus_cd", get_global_time() + s_VOTE4BUFF_CHANGE_BONUS_COOLDOWN);
	}

	// remove old bonuses
	RemoveAffect(AFFECT_VOTE4BUFF);

	const int timeLeft = MINMAX(0, (GetQuestFlag("vote4buff.last_vote") + s_VOTE4BUFF_DURATION) - get_global_time(), s_VOTE4BUFF_DURATION);

	// assign the selected bonuses
	for (int i = 0; i < s_VOTE4BUFF_BONUSES[index - 1].size(); ++i)
	{
		const std::pair<int, int> bonus = s_VOTE4BUFF_BONUSES[index - 1].at(i);
		AddAffect(AFFECT_VOTE4BUFF, aApplyInfo[bonus.first].bPointType, bonus.second, 0, timeLeft, 0, false);
	}

	// assign the new bonus data into player's data
	SetQuestFlag("vote4buff.selected_bonus", index);

	// Write the chosen bonus to the DB so any same-HWID character gets it on next login
	const std::string hwid = Vote4Buff_GetHWID(this);
	if (!hwid.empty())
	{
		DBManager::instance().DirectQuery(
			"UPDATE srv1_player.vote4buff_hwid SET bonus_index = %d WHERE hwid_composite = '%s'",
			index, hwid.c_str());
	}

	Vote4Buff_SendClientInfo();
}

void CHARACTER::Vote4Buff_Reset()
{
	if (!test_server)
		return;

	SetQuestFlag("vote4buff.last_vote", 0);
	SetQuestFlag("vote4buff.selected_bonus", 0);
	SetQuestFlag("vote4buff.change_bonus_cd", 0);

	Vote4Buff_SendClientInfo();
}

std::string CHARACTER::Vote4Buff_GetApiURL()
{
	const std::string login = GetDesc() ? std::string(GetDesc()->GetAccountTable().login) : "";
	return "https://api.metin2pserver.info/API.php?ID=" + s_VOTE4BUFF_SERVER_ID + "&email=" + s_VOTE4BUFF_OWNER_EMAIL + "&name=" + login;
}

std::string CHARACTER::Vote4Buff_GetURL()
{
	const std::string login = GetDesc() ? std::string(GetDesc()->GetAccountTable().login) : "";
	return "https://www.metin2pserver.info/vote.htm?id=" + s_VOTE4BUFF_SERVER_ID + "&name=" + login;
}
#endif