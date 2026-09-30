#include "stdafx.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "packet.h"
#include "guild.h"
#include "vector.h"
#include "questmanager.h"
#include "item.h"
#include "horsename_manager.h"
#include "locale_service.h"
#include "arena.h"

#include "../../common/VnumHelper.h"

bool CHARACTER::StartRiding()
{
	if (IsDead())
	{
		return false;
	}

	if (IsPolymorphed())
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;882]");
		return false;
	}

	//if (thecore_pulse() < GetNextCanMountTime())
	//{
	//	ChatPacket(CHAT_TYPE_INFO, "[LS;10011]");
	//	return false;
	//}
	//SetNextCanMountTime(thecore_pulse() + passes_per_sec / 2);

	LPITEM armor = GetWear(WEAR_BODY);

	if (armor && (armor->GetVnum() >= 11901 && armor->GetVnum() <= 11904))
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;10012]");
		return false;
	}

	//if (CArenaManager::instance().IsArenaMap(GetMapIndex()))
	//{
	//	return false;
	//}
	//
	//if (CWarMapManager::instance().IsWarMap(GetMapIndex()))
	//{
	//	return false;
	//}

	DWORD dwMountVnum = m_chHorse ? m_chHorse->GetRaceNum() : GetMyHorseVnum();

	//if (!IS_MOUNTABLE_ZONE(GetMapIndex(),dwMountVnum == GetMyHorseVnum()))
	//{
	//	//ChatInfoTrans(("RIDING_IS_BLOCKED_HERE"));
	//	ChatPacket(CHAT_TYPE_INFO, "[LS;884]");
	//	return false;
	//}

	if (!CHorseRider::StartRiding())
	{
		if (GetHorseLevel() <= 0)
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;884]");
		}
		else if (GetHorseHealth() <= 0)
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;885]");
		}
		else if (GetHorseStamina() <= 0)
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;886]");
		}

		return false;
	}

#ifdef ENABLE_AFK_MODE_SYSTEM
	if (IsAway())
	{
		SetAway(false);
		if (IsAffectFlag(AFF_AFK))
		{
			RemoveAffect(AFFECT_AFK);
		}
	}
#endif

	HorseSummon(false);
	MountVnum(dwMountVnum);

#ifdef ENABLE_MOUNT_SYSTEM
	ComputePoints();
	UpdatePacket();
#endif
	if (IsPC()) {
		GetAbuseController()->ResetMovementSpeedhackChecker();
	}
	return true;
}

bool CHARACTER::StopRiding()
{
	if (CHorseRider::StopRiding())
	{
		SetNextCanMountTime(thecore_pulse() + passes_per_sec / 2);
		quest::CQuestManager::instance().Unmount(GetPlayerID());

		if (!IsDead() && !IsStun())
		{
			DWORD dwOldVnum = GetMountVnum();
			MountVnum(0);

			HorseSummon(true, false, dwOldVnum);
		}
		else
		{
			m_dwMountVnum = 0;
			ComputePoints();
			UpdatePacket();
		}

		PointChange(POINT_ST, 0);
		PointChange(POINT_DX, 0);
		PointChange(POINT_HT, 0);
		PointChange(POINT_IQ, 0);

#ifdef ENABLE_MOUNT_SYSTEM
		ComputePoints();
		UpdatePacket();
#endif

		return true;
	}
	if (IsPC()) {
		GetAbuseController()->ResetMovementSpeedhackChecker();
	}
	return false;
}

EVENTFUNC(horse_dead_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "horse_dead_event> <Factor> Null pointer" );
		return 0;
	}

	// <Factor>
	LPCHARACTER ch = info->ch;
	if (ch == NULL) {
		return 0;
	}
	ch->HorseSummon(false);
	return 0;
}

void CHARACTER::SetRider(LPCHARACTER ch)
{
	if (m_chRider)
		m_chRider->ClearHorseInfo();

	m_chRider = ch;

	if (m_chRider)
		m_chRider->SendHorseInfo();
}

LPCHARACTER CHARACTER::GetRider() const
{
	return m_chRider;
}

void CHARACTER::HorseSummon(bool bSummon, bool bFromFar, DWORD dwVnum, const char* pPetName)
{
	if (bSummon)
	{
		if (m_chHorse != NULL)
			return;

		if (GetHorseLevel() <= 0)
			return;

		if (IsRiding())
			return;

		if (IsObserverMode())
			return;

#ifdef __RENEWAL_MOUNT__
		if (MountBlockMap())
			return;
#endif
		sys_log(0, "HorseSummon : %s lv:%d bSummon:%d fromFar:%d", GetName(), GetLevel(), bSummon, bFromFar);

		int32_t x = GetX();
		int32_t y = GetY();

		if (GetHorseHealth() <= 0)
			bFromFar = false;

		if (bFromFar)
		{
			x += (number(0, 1) * 2 - 1) * number(2000, 2500);
			y += (number(0, 1) * 2 - 1) * number(2000, 2500);
		}
		else
		{
			x += number(-100, 100);
			y += number(-100, 100);
		}

		m_chHorse = CHARACTER_MANAGER::instance().SpawnMob((0 == dwVnum) ? GetMyHorseVnum() : dwVnum, GetMapIndex(), x, y, GetZ(), false, (int)(GetRotation() + 180), false);

		if (!m_chHorse)
		{
			ChatPacket(CHAT_TYPE_INFO, "Summon your horse first.");
			return;
		}

#ifndef __RENEWAL_MOUNT__
		if (GetHorseHealth() <= 0)
		{
			m_chHorse->SetPosition(POS_DEAD);
			char_event_info* info = AllocEventInfo<char_event_info>();
			info->ch = this;
			m_chHorse->m_pkDeadEvent = event_create(horse_dead_event, info, PASSES_PER_SEC(60));
		}
#endif

		m_chHorse->SetLevel(GetHorseLevel());
		const char* pHorseName = CHorseNameManager::instance().GetHorseName(GetPlayerID());
		m_chHorse->SetName(GetName());
		m_chHorse->SetHorse();

		if (pHorseName != NULL && strlen(pHorseName) != 0)
		{
			m_chHorse->m_stName = GetName();
			m_chHorse->m_stName += " -";
			m_chHorse->m_stName += pHorseName;
		}
		else
		{
			m_chHorse->m_stName = GetName();
			m_chHorse->m_stName += " - Horse";
		}

#ifndef __RENEWAL_MOUNT__
		if ((GetHorseHealth() <= 0))
		{
			TPacketGCDead pack;
			pack.header = HEADER_GC_DEAD;
			pack.vid = m_chHorse->GetVID();
			PacketAround(&pack, sizeof(pack));
		}
#endif

		m_chHorse->SetRider(this);
	}
	else
	{
		if (!m_chHorse)
			return;

		LPCHARACTER chHorse = m_chHorse;

		chHorse->SetRider(NULL);
		if (!bFromFar)
			M2_DESTROY_CHARACTER(chHorse);
		else
		{
			chHorse->SetNowWalking(false);
			float fx, fy;
			chHorse->SetRotation(GetDegreeFromPositionXY(chHorse->GetX(), chHorse->GetY(), GetX(), GetY()) + 180);
			GetDeltaByDegree(chHorse->GetRotation(), 3500, &fx, &fy);
			chHorse->Goto((int32_t)(chHorse->GetX() + fx), (int32_t)(chHorse->GetY() + fy));
			chHorse->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
		}
		
		m_chHorse = NULL;
	}
#ifdef ENABLE_MOUNT_SYSTEM
	ComputePoints();
	UpdatePacket();
	//ComputeMountBonus();
#endif
}

DWORD CHARACTER::GetMyHorseVnum() const
{
#ifdef ENABLE_MOUNT_SYSTEM
	LPITEM costumeMount = GetWear(WEAR_COSTUME_MOUNT);
	if (costumeMount)
		return costumeMount->GetValue(5);
#endif
	int delta = 0;

	if (GetGuild())
	{
		++delta;

		if (GetGuild()->GetMasterPID() == GetPlayerID())
		{
			++delta;
		}
	}

	return c_aHorseStat[GetHorseLevel()].iNPCRace + delta;
}

void CHARACTER::HorseDie()
{
	CHorseRider::HorseDie();
	HorseSummon(false);
}

bool CHARACTER::ReviveHorse()
{
	if (CHorseRider::ReviveHorse())
	{
		HorseSummon(false);
		HorseSummon(true);
		return true;
	}
	return false;
}

void CHARACTER::ClearHorseInfo()
{
	if (!IsHorseRiding())
	{
		ChatPacket(CHAT_TYPE_COMMAND, "hide_horse_state");

		m_bSendHorseLevel = 0;
		m_bSendHorseHealthGrade = 0;
		m_bSendHorseStaminaGrade = 0;
	}

	m_chHorse = NULL;
}

void CHARACTER::SendHorseInfo()
{
	if (m_chHorse || IsHorseRiding())
	{
		int iHealthGrade;
		int iStaminaGrade;

		if (GetHorseHealth() == 0)
			iHealthGrade = 0;
		else if (GetHorseHealth() * 10 <= GetHorseMaxHealth() * 3)
			iHealthGrade = 1;
		else if (GetHorseHealth() * 10 <= GetHorseMaxHealth() * 7)
			iHealthGrade = 2;
		else
			iHealthGrade = 3;

		if (GetHorseStamina() * 10 <= GetHorseMaxStamina())
			iStaminaGrade = 0;
		else if (GetHorseStamina() * 10 <= GetHorseMaxStamina() * 3)
			iStaminaGrade = 1;
		else if (GetHorseStamina() * 10 <= GetHorseMaxStamina() * 7)
			iStaminaGrade = 2;
		else
			iStaminaGrade = 3;

		if (m_bSendHorseLevel != GetHorseLevel() ||
				m_bSendHorseHealthGrade != iHealthGrade ||
				m_bSendHorseStaminaGrade != iStaminaGrade)
		{
			ChatPacket(CHAT_TYPE_COMMAND, "horse_state %d %d %d", GetHorseLevel(), iHealthGrade, iStaminaGrade);

			m_bSendHorseLevel = GetHorseLevel();
			m_bSendHorseHealthGrade = iHealthGrade;
			m_bSendHorseStaminaGrade = iStaminaGrade;
		}
	}
}

bool CHARACTER::CanUseHorseSkill()
{
	if(IsRiding())
		return true;

	return false;
}

void CHARACTER::SetHorseLevel(int iLevel)
{
	CHorseRider::SetHorseLevel(iLevel);
	SetSkillLevel(SKILL_HORSE, GetHorseLevel());
}

#ifdef ENABLE_MOUNT_SYSTEM
void CHARACTER::SetMountExp(int iAmount)
{
	int iRequiredExp = mount_exp_table[GetMountLevel()];
	if (iAmount >= iRequiredExp)
	{
		SetMountLevel(GetMountLevel() + 1);

		DWORD restExp = iAmount - iRequiredExp;
		m_points.iMountExp = restExp;
		SetMountExp(restExp);
	}
	else
	{
		m_points.iMountExp = iAmount;
		SendMountInfoPacket();
	}

}
#endif

#ifdef __RENEWAL_MOUNT__
bool CHARACTER::MountBlockMap()
{
	if (IsObserverMode())
		return true;

	if (GetWarMap())
		return true;

	switch (GetRealMapIndex())
	{
		case 110:
		case 111:
		case 358:
		case 364:
			return true;
	}
	return false;
}
#endif

//martysama0134's 7f12f88f86c76f82974cba65d7406ac8
