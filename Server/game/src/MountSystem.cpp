#include "stdafx.h"
#include "config.h"
#include "utils.h"
#include "vector.h"
#include "char.h"
#include "sectree_manager.h"
#include "char_manager.h"
#include "mob_manager.h"
#include "MountSystem.h"
#include "../../common/VnumHelper.h"
#include "packet.h"
#include "item_manager.h"
#include "item.h"
#include "skill.h"

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
// const	DWORD m_dwSkillVnum[5] = { 295, 296, 297, 298, 299 };

EVENTINFO(mountsystem_event_info)
{
	CMountSystem* pMountSystem;
};

EVENTFUNC(mountsystem_update_event)
{
	mountsystem_event_info* info = dynamic_cast<mountsystem_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "<mountsystem_update_event> <Factor> Null pointer" );
		return 0;
	}

	CMountSystem*	pMountSystem = info->pMountSystem;

	if (NULL == pMountSystem)
		return 0;


	pMountSystem->Update(0);
	return PASSES_PER_SEC(1) / 4;
}

///////////////////////////////////////////////////////////////////////////////////////
//  CMountActor
///////////////////////////////////////////////////////////////////////////////////////

// static int AFFECTS[5][6][40 + 1] =
// {
// 	{
// 		{APPLY_ATTBONUS_MONSTER, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
// 	},
// 	{
// 		{APPLY_ATTBONUS_MONSTER, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
// 	},
// 	{
// 		{APPLY_ATTBONUS_MONSTER, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
// 	},
// 	{
// 		{APPLY_ATTBONUS_MONSTER, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
// 	},
// 	{
// 		{APPLY_ATTBONUS_MONSTER, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
// 	},
// };

CMountActor::CMountActor(LPCHARACTER owner, DWORD vnum)
{
	m_dwVnum = vnum;
	m_dwVID = 0;
	m_dwLastActionTime = 0;

	m_pkChar = 0;
	m_pkOwner = owner;

	m_originalMoveSpeed = 0;

	m_dwSummonItemVID = 0;
	m_dwSummonItemVnum = 0;

	// m_dwLevel = 0;
	// m_dwEXP = 0;
	// m_dwNextEXP = 0;
	// m_dwSkillPoints = 0;
	
	// for (int i = 0; i < 5; i++) {
	// 	m_dwSkill[i] = 0;
	// }
}

CMountActor::~CMountActor()
{
	this->Unsummon();
	m_pkOwner = 0;
}

void CMountActor::SetName()
{
	std::string petName = "Jezdecke zvire - ";

	if (true == IsSummoned())
	{
		petName += m_pkOwner->GetName();
		
		m_pkChar->SetName(petName);
	}

	m_name = petName;
}

// void CMountActor::ClearInfo()
// {
// 	// m_dwLevel = 0;
// 	// m_dwEXP = 0;
// 	// m_dwNextEXP = 0;
// 	// m_dwSkillPoints = 0;
	
// 	// for (int i = 0; i < 5; i++) {
// 	// 	m_dwSkill[i] = 0;
// 	// }

// 	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "MountNPCClear");
// }

// void CMountActor::AddMountAffect()
// {
// 	if (!m_pkOwner)// || false == IsSummoned())
// 	{
// 	 	return;
// 	}

// 	m_pkOwner->RemoveAffect(AFFECT_NEW_MOUNT);

// 	LPITEM mount_item = m_pkOwner->GetWear(WEAR_COSTUME_MOUNT);
// 	if (!mount_item)
// 	{
// 		return;
// 	}

// 	for (unsigned int i = 295; i < 300; i++)
// 	{
// 		const int id = (i - 295);
// 		const int skill_level = MINMAX(0, this->m_dwSkill[id], 10);
// 		if (skill_level)
// 		{
// 			for (unsigned int b = 0; b < 1; b++)
// 			{
// 				const int affect_id = AFFECTS[id][b][0];
// 				const int affect_value = AFFECTS[id][b][skill_level];
// 				if (affect_id > 0 && affect_value > 0)
// 				{
// 					m_pkOwner->AddAffect(AFFECT_NEW_MOUNT, aApplyInfo[affect_id].bPointType, affect_value, AFF_NONE, 60 * 60 * 24 * 30, 0, false);
// 				}
// 			}
// 		}
// 	}
// }

// void CMountActor::SendSkillInfo()
// {
// 	char skill_data[CHAT_MAX_LEN - 1];
// 	int skill_len = 0;

// 	for (unsigned int i = 295; i < 300; i++)
// 	{
// 		int id = (i - 295);
// 		int skill_level = MINMAX(0, this->m_dwSkill[id], 10);
// 		skill_len += snprintf(skill_data + skill_len, sizeof(skill_data) - skill_len, "%d&", skill_level);
// 		for (unsigned int b = 0; b < 1; b++)
// 		{
// 			const int affect_id = AFFECTS[id][b][0];
// 			int affect_value = 0;
// 			if (skill_level)
// 			{
// 				affect_value = AFFECTS[id][b][skill_level];
// 			}
// 			skill_len += snprintf(skill_data + skill_len, sizeof(skill_data) - skill_len, "%d,%d@", affect_id, affect_value);
// 		}
// 		skill_len += snprintf(skill_data + skill_len, sizeof(skill_data) - skill_len, "#");
// 	}

// 	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "MountNPCSkillInfo %d %s", m_dwSkillPoints, skill_data);
// }

// int CMountActor::GetSkillID(int iSkillVnum)
// {
// 	for (int i = 0; i < 5; i++)
// 	{
// 		if (iSkillVnum == m_dwSkillVnum[i])
// 			return i;
// 	}
// 	return -1;
// }

// int CMountActor::GetSkillGrade(int iSkillLevel)
// {
// 	if (iSkillLevel >= 0 && iSkillLevel < 10)
// 		return 0;
// 	else if (iSkillLevel >= 10 && iSkillLevel < 20)
// 		return 1;
// 	else if (iSkillLevel >= 20 && iSkillLevel < 30) 
// 		return 2;
// 	else if (iSkillLevel == 30)
// 		return 3;
// 	else
// 		return -1;
// }

bool CMountActor::Mount(LPITEM mountItem)
{
	if (0 == m_pkOwner)
		return false;
	
	if(!mountItem)
		return false;

	if (m_pkOwner->IsHorseRiding())
		m_pkOwner->StopRiding();
	
	if (m_pkOwner->GetHorse())
		m_pkOwner->HorseSummon(false);
	
	Unmount();

	m_pkOwner->AddAffect(AFFECT_MOUNT, POINT_MOUNT, m_dwVnum, AFF_NONE, (DWORD)mountItem->GetSocket(0) - time(0), 0, true);
	
	for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
	{
		if (mountItem->GetProto()->aApplies[i].bType == APPLY_NONE)
			continue;

		m_pkOwner->AddAffect(AFFECT_MOUNT_BONUS, aApplyInfo[mountItem->GetProto()->aApplies[i].bType].bPointType, mountItem->GetProto()->aApplies[i].lValue, AFF_NONE, (DWORD)mountItem->GetSocket(0) - time(0), 0, false);
	}

	// AddMountAffect();
	
	return m_pkOwner->GetMountVnum() == m_dwVnum;
}

void CMountActor::Unmount()
{
	if (0 == m_pkOwner)
		return;
	
	if (!m_pkOwner->GetMountVnum())
		return;
	
	m_pkOwner->RemoveAffect(AFFECT_MOUNT);
	m_pkOwner->RemoveAffect(AFFECT_MOUNT_BONUS);
	m_pkOwner->MountVnum(0);
	
	if (m_pkOwner->IsHorseRiding())
		m_pkOwner->StopRiding();
	
	if (m_pkOwner->GetHorse())
		m_pkOwner->HorseSummon(false);

	m_pkOwner->RemoveAffect(AFFECT_NEW_MOUNT);
}

void CMountActor::Unsummon()
{
	if (true == this->IsSummoned())
	{
		this->SetSummonItem(NULL);
		
		if (NULL != m_pkChar)
			// SaveSQL();
			M2_DESTROY_CHARACTER(m_pkChar);

		m_pkChar = 0;
		m_dwVID = 0;
	}
}

DWORD CMountActor::Summon(LPITEM pSummonItem, bool bSpawnFar)
{
	// if (!IsRidingMount())
	Unmount();

	int32_t x = m_pkOwner->GetX();
	int32_t y = m_pkOwner->GetY();
	int32_t z = m_pkOwner->GetZ();

	if (true == bSpawnFar)
	{
		x += (number(0, 1) * 2 - 1) * number(2000, 2500);
		y += (number(0, 1) * 2 - 1) * number(2000, 2500);
	}
	else
	{
		x += number(-100, 100);
		y += number(-100, 100);
	}
	
	if (0 != m_pkChar)
	{
		m_pkChar->Show(m_pkOwner->GetMapIndex(), x, y);
		m_dwVID = m_pkChar->GetVID();

		return m_dwVID;
	}

	if (m_pkOwner->IsHorseRiding())
		m_pkOwner->StopRiding();
	
	if (m_pkOwner->GetHorse())
		m_pkOwner->HorseSummon(false);

	// std::unique_ptr<SQLMsg> pMsg2(DBManager::instance().DirectQuery("SELECT level, exp, skill_point, skill0lv, skill1lv, skill2lv, skill3lv, skill4lv FROM player_mount WHERE player_id=%ld", m_pkOwner->GetPlayerID()));
	// if (pMsg2->Get()->uiNumRows > 0) {
	// 	MYSQL_ROW row = mysql_fetch_row(pMsg2->Get()->pSQLResult);
	// 	this->m_dwLevel					= atoi(row[0]);
	// 	if (this->m_dwEXP < atoi(row[1])) {
	// 		this->m_dwEXP				= atoi(row[1]);
	// 	}

	// 	this->m_dwSkillPoints			= atoi(row[2]);
	// 	this->m_dwSkill[0]			= atoi(row[3]);
	// 	this->m_dwSkill[1]			= atoi(row[4]);
	// 	this->m_dwSkill[2]			= atoi(row[5]);
	// 	this->m_dwSkill[3]			= atoi(row[6]);
	// 	this->m_dwSkill[4]			= atoi(row[7]);
	// }
	// else {
	// 	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("INSERT INTO player_mount (player_id, player_name, level, exp, skill_point) VALUES (%d, '%s', %d, %d, %d)", m_pkOwner->GetPlayerID(), m_pkOwner->GetName(), this->m_dwLevel, this->m_dwEXP, this->m_dwSkillPoints));
	// }

	// m_dwNextEXP = m_pkOwner->MountGetNextExpByLevel(m_dwLevel);
	// m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "MountNPCEXPInfo %d %d %d", m_dwLevel, m_dwEXP, m_dwNextEXP);
	// SendSkillInfo();
	m_pkChar = CHARACTER_MANAGER::instance().SpawnMob(m_dwVnum, m_pkOwner->GetMapIndex(), x, y, z, false, (int)(m_pkOwner->GetRotation()+180), false);

	if (0 == m_pkChar)
	{
		sys_err("[CMountActor::Summon] Failed to summon the mount. (vnum: %d)", m_dwVnum);
		return 0;
	}

	m_pkChar->SetMount();

	m_pkChar->SetEmpire(m_pkOwner->GetEmpire());

	m_dwVID = m_pkChar->GetVID();

	this->SetName();
	// this->SetLevel(this->m_dwLevel);
	this->SetSummonItem(pSummonItem);
	
	//m_pkOwner->ComputePoints();
	
	m_pkChar->Show(m_pkOwner->GetMapIndex(), x, y, z);

	return m_dwVID;
}

bool CMountActor::_UpdateFollowAI()
{
	if (0 == m_pkChar->m_pkMobData)
	{
		return false;
	}

	if (0 == m_originalMoveSpeed)
	{
		const CMob* mobData = CMobManager::Instance().Get(m_dwVnum);

		if (0 != mobData)
			m_originalMoveSpeed = mobData->m_table.sMovingSpeed;
	}
	float	START_FOLLOW_DISTANCE = 300.0f;
	// float	START_RUN_DISTANCE = 900.0f;

	float	RESPAWN_DISTANCE = 4500.f;
	int		APPROACH = 200;

	// bool bRun = false;

	DWORD currentTime = get_dword_time();

	int32_t ownerX = m_pkOwner->GetX();		int32_t ownerY = m_pkOwner->GetY();
	int32_t charX = m_pkChar->GetX();			int32_t charY = m_pkChar->GetY();

	float fDist = DISTANCE_SQRT(charX - ownerX, charY - ownerY);

	if (fDist >= RESPAWN_DISTANCE)
	{
		float fOwnerRot = m_pkOwner->GetRotation() * 3.141592f / 180.f;
		float fx = -APPROACH * cos(fOwnerRot);
		float fy = -APPROACH * sin(fOwnerRot);
		if (m_pkChar->Show(m_pkOwner->GetMapIndex(), ownerX + fx, ownerY + fy))
		{
			return true;
		}
	}

	if (fDist >= START_FOLLOW_DISTANCE)
	{
		m_pkChar->SetNowWalking(false);

		Follow(APPROACH);

		m_pkChar->SetLastAttacked(currentTime);
		m_dwLastActionTime = currentTime;
	}
	else
		m_pkChar->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);

	return true;
}

bool CMountActor::Update(DWORD deltaTime)
{
	bool bResult = true;

	if (m_pkOwner->IsDead() || (IsSummoned() && m_pkChar->IsDead())
		|| NULL == ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID())
		|| ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID())->GetOwner() != this->GetOwner()
		)
	{
		this->Unsummon();
		return true;
	}

	if (this->IsSummoned())
		bResult = bResult && this->_UpdateFollowAI();

	return bResult;
}

bool CMountActor::Follow(float fMinDistance)
{
	if( !m_pkOwner || !m_pkChar)
		return false;

	float fOwnerX = m_pkOwner->GetX();
	float fOwnerY = m_pkOwner->GetY();

	float fPetX = m_pkChar->GetX();
	float fPetY = m_pkChar->GetY();

	float fDist = DISTANCE_SQRT(fOwnerX - fPetX, fOwnerY - fPetY);
	if (fDist <= fMinDistance)
		return false;

	m_pkChar->SetRotationToXY(fOwnerX, fOwnerY);

	float fx, fy;

	float fDistToGo = fDist - fMinDistance;
	GetDeltaByDegree(m_pkChar->GetRotation(), fDistToGo, &fx, &fy);

	if (!m_pkChar->Goto((int)(fPetX+fx+0.5f), (int)(fPetY+fy+0.5f)) )
		return false;

	m_pkChar->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0, 0);
	
	return true;
}

void CMountActor::SetSummonItem(LPITEM pItem)
{
	if (NULL == pItem)
	{
		m_dwSummonItemVID = 0;
		m_dwSummonItemVnum = 0;
		return;
	}

	m_dwSummonItemVID = pItem->GetVID();
	m_dwSummonItemVnum = pItem->GetVnum();
}

// void CMountActor::SetLevel(DWORD level)
// {
// 	m_pkChar->SetLevel(static_cast<char>(level));
// 	m_dwLevel = level;
// 	SetNextExp(m_pkChar->MountGetNextExp());
// 	m_pkChar->UpdatePacket();
// }

// void CMountActor::SaveSQL() {
// 	DBManager::instance().DirectQuery
// 	(
// 		"UPDATE player_mount SET player_name='%s', level=%d, exp=%d, skill_point=%d, skill0lv=%d, skill1lv=%d, skill2lv=%d, skill3lv=%d, skill4lv=%d WHERE player_id = %d ",
// 		m_pkOwner->GetName(), this->GetLevel(), this->GetExp(), this->m_dwSkillPoints, this->m_dwSkill[0], this->m_dwSkill[1], this->m_dwSkill[2], this->m_dwSkill[3], this->m_dwSkill[4], m_pkOwner->GetPlayerID()
// 	);
// }

// void CMountActor::SetLevelUp()
// {
// 	if(GetLevel() == 50) {
// 		return;
// 	}

// 	SetLevel(GetLevel() + 1);
// 	m_pkChar->SendMountLevelUpEffect(m_pkChar->GetVID(), 1, GetLevel(), 1);
// 	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "MountNPCEXPInfo %d %d %d", GetLevel(), GetExp(), GetNextExp());
// 	m_dwEXP = 0;
// 	m_dwSkillPoints = m_dwSkillPoints + 1;
// 	SendSkillInfo();
// 	SaveSQL();
// 	return;
// }

// void CMountActor::SetSkill(DWORD dwSkillSlot, DWORD dwSkillLevel)
// {
// 	m_dwSkill[dwSkillSlot] = dwSkillLevel;
// 	SendSkillInfo();
// }

// void CMountActor::SetNextExp(int nextExp)
// {	
// 	m_dwNextEXP = nextExp;
// }

// void CMountActor::LearnSkillByPoint(DWORD dwSkillType)
// {
// 	if(m_dwSkillPoints > 0)
// 	{
// 		if(m_dwSkill[dwSkillType] >= 16)
// 		{
// #ifdef ASLAN_BUFF_NPC_USE_SKILL_17_TO_M
// 			m_dwSkill[dwSkillType] = 20;
// 			m_dwSkillPoints = m_dwSkillPoints - 1;
// #else
// 			if(m_dwSkill[dwSkillType] >= 19) {
// 				m_dwSkill[dwSkillType] = 20;
// 				m_dwSkillPoints = m_dwSkillPoints - 1;
// 			}
// 			else {
// 				int chance = number(1, 100);
// 				if(chance > ASLAN_BUFF_NPC_USE_SKILL_TECH_LEVEL_CHANCE) {
// 					m_dwSkill[dwSkillType] = this->m_dwSkill[dwSkillType] + 1;
// 					m_dwSkillPoints = m_dwSkillPoints - 1;
// 				}
// 				else {
// 					m_dwSkill[dwSkillType] = 20;
// 					m_dwSkillPoints = m_dwSkillPoints - 1;
// 				}
// 			}
// #endif
// 		}
// 		else
// 		{
// 			m_dwSkill[dwSkillType] = this->m_dwSkill[dwSkillType] + 1;
// 			m_dwSkillPoints = m_dwSkillPoints - 1;
// 		}
// 		SaveSQL();
// 		SendSkillInfo();
// 	}
// }
// void CMountActor::SetExp(DWORD exp)
// {
// 	if (false == IsSummoned())
// 			return;

// 	if(GetLevel() >= 50) {
// 		m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "MountNPCEXPInfo %d %d %d %d", m_dwLevel, 0, 0);
// 		return;
// 	}

// 	if (exp < 0)
// 		exp = MAX(m_dwEXP - exp, 0);

// 	if (GetExp() + exp >= GetNextExp())
// 		SetLevelUp();

// 	m_dwEXP += exp;
// 	m_pkChar->SetExp(m_dwEXP);
// 	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "MountNPCEXPInfo %d %d %d", m_dwLevel, m_dwEXP, m_dwNextEXP);
// }

///////////////////////////////////////////////////////////////////////////////////////
//  CMountSystem
///////////////////////////////////////////////////////////////////////////////////////

CMountSystem::CMountSystem(LPCHARACTER owner)
{
	m_pkOwner = owner;
	m_dwUpdatePeriod = 400;

	m_dwLastUpdateTime = 0;
}

CMountSystem::~CMountSystem()
{
	Destroy();
}

void CMountSystem::Destroy()
{
	// this->SaveSQL();
	for (TMountActorMap::iterator iter = m_mountActorMap.begin(); iter != m_mountActorMap.end(); ++iter)
	{
		CMountActor* mountActor = iter->second;

		if (0 != mountActor)
		{
			delete mountActor;
		}
	}
	event_cancel(&m_pkMountSystemUpdateEvent);
	m_mountActorMap.clear();
}

// void CMountSystem::SetExp(int iExp)
// {
// 	for (TMountActorMap::iterator iter = m_mountActorMap.begin(); iter != m_mountActorMap.end(); ++iter)
// 	{
// 		CMountActor* mountActor = iter->second;
// 		if (mountActor != 0)
// 		{
// 			if (mountActor->IsSummoned()) {
// 				mountActor->SetExp(iExp);
// 				break;
// 			}
// 		}
// 	}
// }

// void CMountSystem::LearnSkillByPoint2(DWORD dwSkillType)
// {
// 	for (TMountActorMap::iterator iter = m_mountActorMap.begin(); iter != m_mountActorMap.end(); ++iter)
// 	{
// 		CMountActor* mountActor = iter->second;
// 		if (mountActor != 0)
// 		{
// 			if (mountActor->IsSummoned()) {
// 				mountActor->LearnSkillByPoint(dwSkillType);
// 				break;
// 			} else {
// 				return m_pkOwner->ChatPacket(CHAT_TYPE_INFO, "Musisz zsi¹œæ z wierzchowca!");
// 			}
// 		}
// 	}
// }

bool CMountSystem::Update(DWORD deltaTime)
{
	bool bResult = true;

	DWORD currentTime = get_dword_time();

	if (m_dwUpdatePeriod > currentTime - m_dwLastUpdateTime)
		return true;

	std::vector <CMountActor*> v_garbageActor;

	for (TMountActorMap::iterator iter = m_mountActorMap.begin(); iter != m_mountActorMap.end(); ++iter)
	{
		CMountActor* mountActor = iter->second;

		if (0 != mountActor && mountActor->IsSummoned())
		{
			LPCHARACTER pMount = mountActor->GetCharacter();

			if (NULL == CHARACTER_MANAGER::instance().Find(pMount->GetVID()))
			{
				v_garbageActor.push_back(mountActor);
			}
			else
			{
				bResult = bResult && mountActor->Update(deltaTime);
			}
		}
	}
	for (std::vector<CMountActor*>::iterator it = v_garbageActor.begin(); it != v_garbageActor.end(); it++)
		DeleteMount(*it);

	m_dwLastUpdateTime = currentTime;

	return bResult;
}

void CMountSystem::DeleteMount(DWORD mobVnum)
{
	TMountActorMap::iterator iter = m_mountActorMap.find(mobVnum);

	if (m_mountActorMap.end() == iter)
	{
		sys_err("[CMountSystem::DeleteMount] Can't find mount on my list (VNUM: %d)", mobVnum);
		return;
	}

	CMountActor* mountActor = iter->second;

	if (0 == mountActor)
		sys_err("[CMountSystem::DeleteMount] Null Pointer (mountActor)");
	else
		delete mountActor;

	m_mountActorMap.erase(iter);
}

void CMountSystem::DeleteMount(CMountActor* mountActor)
{
	for (TMountActorMap::iterator iter = m_mountActorMap.begin(); iter != m_mountActorMap.end(); ++iter)
	{
		if (iter->second == mountActor)
		{
			delete mountActor;
			m_mountActorMap.erase(iter);

			return;
		}
	}

	sys_err("[CMountSystem::DeleteMount] Can't find mountActor(0x%x) on my list(size: %d) ", mountActor, m_mountActorMap.size());
}

void CMountSystem::Unsummon(DWORD vnum, bool bDeleteFromList)
{

	CMountActor* actor = this->GetByVnum(vnum);

	if (0 == actor)
	{
		sys_err("[CMountSystem::Unsummon(%d)] Null Pointer (actor)", vnum);
		return;
	}
	actor->Unsummon();

	if (true == bDeleteFromList)
		this->DeleteMount(actor);

	bool bActive = false;
	for (TMountActorMap::iterator it = m_mountActorMap.begin(); it != m_mountActorMap.end(); it++)
	{
		bActive |= it->second->IsSummoned();
	}
	if (false == bActive)
	{
		event_cancel(&m_pkMountSystemUpdateEvent);
		m_pkMountSystemUpdateEvent = NULL;
	}
}


void CMountSystem::Summon(DWORD mobVnum, LPITEM pSummonItem, bool bSpawnFar)
{
	
	CMountActor* mountActor = this->GetByVnum(mobVnum);

	if (0 == mountActor)
	{
		mountActor = M2_NEW CMountActor(m_pkOwner, mobVnum);
		m_mountActorMap.insert(std::make_pair(mobVnum, mountActor));
	}

	DWORD mountVID = mountActor->Summon(pSummonItem, bSpawnFar);

	if (!mountVID)
		sys_err("[CMountSystem::Summon(%d)] Null Pointer (mountVID)", pSummonItem->GetID());

	if (NULL == m_pkMountSystemUpdateEvent)
	{
		mountsystem_event_info* info = AllocEventInfo<mountsystem_event_info>();

		info->pMountSystem = this;

		m_pkMountSystemUpdateEvent = event_create(mountsystem_update_event, info, PASSES_PER_SEC(1) / 4);
	}

	//return mountActor;
}

// int CMountSystem::GetLevel()
// {
// 	for (TMountActorMap::iterator iter = m_mountActorMap.begin(); iter != m_mountActorMap.end(); ++iter)
// 	{
// 		CMountActor* mountActor = iter->second;
// 		if (mountActor != 0)
// 		{
// 			if (mountActor->IsSummoned()) {
// 				return mountActor->GetLevel();
// 			}
// 		}
// 	}
// 	return -1;
// }

// void CMountSystem::ClearInfo()
// {
// 	for (TMountActorMap::iterator iter = m_mountActorMap.begin(); iter != m_mountActorMap.end(); ++iter)
// 	{
// 		CMountActor* mountActor = iter->second;
// 		if (mountActor != 0)
// 		{
// 			return mountActor->ClearInfo();
// 		}
// 	}
// }

// void CMountSystem::SaveSQL()
// {
// 	for (TMountActorMap::iterator iter = m_mountActorMap.begin(); iter != m_mountActorMap.end(); ++iter)
// 	{
// 		CMountActor* mountActor = iter->second;
// 		if (mountActor != 0)
// 		{
// 			return mountActor->SaveSQL();
// 		}
// 	}
// }

void CMountSystem::Mount(DWORD mobVnum, LPITEM mountItem)
{
	CMountActor* mountActor = this->GetByVnum(mobVnum);

	if (!mountActor)
	{
		sys_err("[CMountSystem::Mount] Null Pointer (mountActor)");
		return;
	}
	
	if(!mountItem)
		return;
	
	//check timer
	// if (m_pkOwner->IncreaseMountCounter() >= 5)
	// {
		// m_pkOwner->ChatPacket(CHAT_TYPE_INFO, "<Mount> Asteapta 5 secunde pentru a face aceasta actiune");
		// return;
	// }
	// DWORD dwCurrentTime = get_dword_time();
	// if (dwCurrentTime - m_pkOwner->GetLastSkillTime() < 800)
	// {
	// 	m_pkOwner->ChatPacket(CHAT_TYPE_INFO, "Odczekaj chwilê!");
	// 	return;
	// }
	this->Unsummon(mobVnum, false);
	mountActor->Mount(mountItem);
}

void CMountSystem::Unmount(DWORD mobVnum)
{
	CMountActor* mountActor = this->GetByVnum(mobVnum);

	if (!mountActor)
	{
		sys_err("[CMountSystem::Mount] Null Pointer (mountActor)");
		return;
	}
	//check timer
	// if (m_pkOwner->IncreaseMountCounter() >= 5)
	// {
		// m_pkOwner->ChatPacket(CHAT_TYPE_INFO, "<Mount> Asteapta 5 secunde pentru a face aceasta actiune");
		// return;
	// }
	
	if(LPITEM pSummonItem = m_pkOwner->GetWear(WEAR_COSTUME_MOUNT))
	{
		this->Summon(mobVnum, pSummonItem, false);
	}
	mountActor->Unmount();
}

CMountActor* CMountSystem::GetByVID(DWORD vid) const
{
	CMountActor* mountActor = 0;

	bool bFound = false;

	for (TMountActorMap::const_iterator iter = m_mountActorMap.begin(); iter != m_mountActorMap.end(); ++iter)
	{
		mountActor = iter->second;

		if (0 == mountActor)
		{
			sys_err("[CMountSystem::GetByVID(%d)] Null Pointer (mountActor)", vid);
			continue;
		}

		bFound = mountActor->GetVID() == vid;

		if (true == bFound)
			break;
	}

	return bFound ? mountActor : 0;
}

CMountActor* CMountSystem::GetByVnum(DWORD vnum) const
{
	CMountActor* mountActor = 0;

	TMountActorMap::const_iterator iter = m_mountActorMap.find(vnum);

	if (m_mountActorMap.end() != iter)
		mountActor = iter->second;

	return mountActor;
}

size_t CMountSystem::CountSummoned() const
{
	size_t count = 0;

	for (TMountActorMap::const_iterator iter = m_mountActorMap.begin(); iter != m_mountActorMap.end(); ++iter)
	{
		CMountActor* mountActor = iter->second;

		if (0 != mountActor)
		{
			if (mountActor->IsSummoned())
				++count;
		}
	}

	return count;
}
#endif
