 /*
 /\><><><><><><><><><><><><><><><><><><><><><><><><><><><>/\
 \/		|===================================|		  \/
 /\		|	   System: Pety Expione v1	 |		  /\
 \/		|	Udost�pniony przez kamil2321   |		  \/
 /\		|===================================|		  /\
 \/					 GG: 1889862					   \/
 /\			 strona: https://kamil2321.pl			  /\
 \/			 Forum: https://metin2time.eu			  \/
 /\				   Google: kamil2321				   /\
 \/				  YouTube: kamil2321				   \/
 /\		|===================================|		  /\
 \/				  Do u�ytku w�asnego!				  \/
 /\	W razie problem�w/ poprawek itp. prosz� pisa�.	 /\
 \/><><><><><><><><><><><><><><><><><><><><><><><><><><><>\/
*/
#include "stdafx.h"
#include "utils.h"
#include "vector.h"
#include "char.h"
#include "sectree_manager.h"
#include "char_manager.h"
#include "mob_manager.h"
#include "PetSystem.h"
#include "../../common/VnumHelper.h"
#include "packet.h"
#include "item_manager.h"
#include "item.h"
#include <iostream>

#ifdef ENABLE_NEW_PET_SYSTEM
	#include <boost/algorithm/string.hpp>
	#include <boost/format.hpp>
	#include "horsename_manager.h"
#endif

extern int passes_per_sec;
EVENTINFO(petsystem_event_info)
{
	CPetSystem* pPetSystem;
};

// PetSystem�� update ���ִ� event.
// PetSystem�� CHRACTER_MANAGER���� ���� FSM���� update ���ִ� ���� chracters�� �޸�,
// Owner�� STATE�� update �� �� _UpdateFollowAI �Լ��� update ���ش�.
// �׷��� owner�� state�� update�� CHRACTER_MANAGER���� ���ֱ� ������,
// petsystem�� update�ϴٰ� pet�� unsummon�ϴ� �κп��� ������ �����.
// (CHRACTER_MANAGER���� update �ϸ� chracter destroy�� pending�Ǿ�, CPetSystem������ dangling �����͸� ������ �ְ� �ȴ�.)
// ���� PetSystem�� ������Ʈ ���ִ� event�� �߻���Ŵ.
EVENTFUNC(petsystem_update_event)
{
	petsystem_event_info* info = dynamic_cast<petsystem_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "check_speedhack_event> <Factor> Null pointer" );
		return 0;
	}

	CPetSystem*	pPetSystem = info->pPetSystem;

	if (NULL == pPetSystem)
	{
		return 0;
	}


	pPetSystem->Update(0);
	// 0.25�ʸ��� ����.
	return PASSES_PER_SEC(1) / 4;
}

/// NOTE: 1ĳ���Ͱ� ��� ���� ���� �� �ִ��� ����... ĳ���͸��� ������ �ٸ��� �ҰŶ�� ������ �ֵ... ��..
/// ���� �� �ִ� ������ ���ÿ� ��ȯ�� �� �ִ� ������ Ʋ�� �� �ִµ� �̷��� ��ȹ ������ �ϴ� ����
const float PET_COUNT_LIMIT = 3;

///////////////////////////////////////////////////////////////////////////////////////
//  CPetActor
///////////////////////////////////////////////////////////////////////////////////////

CPetActor::CPetActor(LPCHARACTER owner, DWORD vnum, DWORD options)
{
	m_dwVnum = vnum;
	m_dwVID = 0;
	m_dwOptions = options;
	m_dwLastActionTime = 0;

	m_pkChar = 0;
	m_pkOwner = owner;

	m_originalMoveSpeed = 0;

	m_dwSummonItemVID = 0;
	m_dwSummonItemVnum = 0;
}

CPetActor::~CPetActor()
{
	// Do NOT call Unsummon() here — it calls ComputePoints()/RemoveAffect()
	// on m_pkOwner which may be in mid-destruction (e.g. CPetSystem::Destroy
	// called from CHARACTER::Destroy). Just clean up the pet character directly.
	if (NULL != m_pkChar)
	{
		M2_DESTROY_CHARACTER(m_pkChar);
		m_pkChar = 0;
	}
	m_dwVID = 0;
	m_pkOwner = 0;
}

void CPetActor::SetName(const char* name)
{
#ifdef ENABLE_NEW_PET_SYSTEM
	if (true == IsSummoned())
	{
		m_pkChar->SetName(name);
	}

	m_name = name;
#else
	std::string petName = m_pkOwner->GetName();

	if (0 != m_pkOwner &&
			0 == name &&
			0 != m_pkOwner->GetName())
	{
		petName += "'s Pet";
	}
	else
	{
		petName += name;
	}

	if (true == IsSummoned())
	{
		m_pkChar->SetName(petName);
	}

	m_name = petName;
#endif
}

#ifdef ENABLE_NEW_PET_SYSTEM
void CPetActor::SetLevel(BYTE level)
{
	if (!m_pkOwner || !m_pkChar || !IsSummoned())
	{
		return;
	}

	if (level < 0)
	{
		level = 1;
	}

	if (true == IsSummoned())
	{
		m_pkChar->SetLevel(level);
		m_pkChar->SendPetLevelUpEffect(m_pkChar->GetVID(), 1, level, 1);
	}

	m_level = level;
}
#endif

bool CPetActor::Mount()
{
	if (0 == m_pkOwner)
	{
		return false;
	}

	if (true == HasOption(EPetOption_Mountable))
	{
		m_pkOwner->MountVnum(m_dwVnum);
	}

	return m_pkOwner->GetMountVnum() == m_dwVnum;;
}

void CPetActor::Unmount()
{
	if (0 == m_pkOwner)
	{
		return;
	}

	if (m_pkOwner->IsHorseRiding())
	{
		m_pkOwner->StopRiding();
	}
}

#ifdef ENABLE_NEW_PET_SYSTEM
void CPetActor::Unsummon(bool isExcept)
{
	if (true == this->IsSummoned())
	{

		// Deactivate pet
		LPITEM item = ITEM_MANAGER::instance().FindByVID(m_dwSummonItemVID);

		if (item != NULL)
		{
			if (!isExcept)
			{
				item->SetSocket(2, 0);
			}

			item->Lock(false);
		}

		// ���� ����
		this->ClearBuff();
		m_pkOwner->RemoveAffect(AFFECT_NEW_PET);
		this->SetSummonItem(NULL);
		if (NULL != m_pkOwner)
		{
			m_pkOwner->ComputePoints();
		}

		if (NULL != m_pkChar)
		{
			M2_DESTROY_CHARACTER(m_pkChar);
		}

		m_pkChar = 0;
		m_dwVID = 0;
	}
}
#else
void CPetActor::Unsummon()
{
	if (true == this->IsSummoned())
	{
		// ���� ����
		this->ClearBuff();
		this->SetSummonItem(NULL);
		if (NULL != m_pkOwner)
		{
			m_pkOwner->ComputePoints();
		}

		if (NULL != m_pkChar)
		{
			M2_DESTROY_CHARACTER(m_pkChar);
		}

		m_pkChar = 0;
		m_dwVID = 0;
	}
}
#endif

#ifdef ENABLE_NEW_PET_SYSTEM
	DWORD CPetActor::Summon(const char* petName, LPITEM pSummonItem, bool bSpawnFar, BYTE level)
#else
	DWORD CPetActor::Summon(const char* petName, LPITEM pSummonItem, bool bSpawnFar)
#endif
{
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
		m_pkChar->Show (m_pkOwner->GetMapIndex(), x, y);
		m_dwVID = m_pkChar->GetVID();

		return m_dwVID;
	}

	m_pkChar = CHARACTER_MANAGER::instance().SpawnMob(
				   m_dwVnum,
				   m_pkOwner->GetMapIndex(),
				   x, y, z,
				   false, (int)(m_pkOwner->GetRotation() + 180), false);

	if (0 == m_pkChar)
	{
		sys_err("[CPetSystem::Summon] Failed to summon the pet. (vnum: %d)", m_dwVnum);
		return 0;
	}

	m_pkChar->SetPet();

//	m_pkOwner->DetailLog();
//	m_pkChar->DetailLog();

	//���� ������ ������ ������ ������.
	m_pkChar->SetEmpire(m_pkOwner->GetEmpire());

	m_dwVID = m_pkChar->GetVID();

	this->SetName(petName);
#ifdef ENABLE_NEW_PET_SYSTEM
	this->SetLevel(level);
#endif

	// SetSummonItem(pSummonItem)�� �θ� �Ŀ� ComputePoints�� �θ��� ���� �����.
#ifdef ENABLE_NEW_PET_SYSTEM
	if (pSummonItem)
	{
		this->SetSummonItem(pSummonItem);
		pSummonItem->SetSocket(2, 1);
		pSummonItem->Lock(true);
	}
#else
	this->SetSummonItem(pSummonItem);
#endif
	m_pkOwner->ComputePoints();
	m_pkChar->Show(m_pkOwner->GetMapIndex(), x, y, z);

	return m_dwVID;
}

bool CPetActor::_UpdatAloneActionAI(float fMinDist, float fMaxDist)
{
	float fDist = number(fMinDist, fMaxDist);
	float r = (float)number (0, 359);
	float dest_x = GetOwner()->GetX() + fDist * cos(r);
	float dest_y = GetOwner()->GetY() + fDist * sin(r);

	//m_pkChar->SetRotation(number(0, 359));		// ������ �������� ����

	//GetDeltaByDegree(m_pkChar->GetRotation(), fDist, &fx, &fy);

	// ������ ���� �Ӽ� üũ; ���� ��ġ�� �߰� ��ġ�� �������ٸ� ���� �ʴ´�.
	//if (!(SECTREE_MANAGER::instance().IsMovablePosition(m_pkChar->GetMapIndex(), m_pkChar->GetX() + (int) fx, m_pkChar->GetY() + (int) fy)
	//			&& SECTREE_MANAGER::instance().IsMovablePosition(m_pkChar->GetMapIndex(), m_pkChar->GetX() + (int) fx/2, m_pkChar->GetY() + (int) fy/2)))
	//	return true;

	m_pkChar->SetNowWalking(true);

	//if (m_pkChar->Goto(m_pkChar->GetX() + (int) fx, m_pkChar->GetY() + (int) fy))
	//	m_pkChar->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
	if (!m_pkChar->IsStateMove() && m_pkChar->Goto(dest_x, dest_y))
	{
		m_pkChar->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
	}

	m_dwLastActionTime = get_dword_time();

	return true;
}

// char_state.cpp StateHorse�Լ� �׳� C&P -_-;
bool CPetActor::_UpdateFollowAI()
{
	if (0 == m_pkChar->m_pkMobData)
	{
		//sys_err("[CPetActor::_UpdateFollowAI] m_pkChar->m_pkMobData is NULL");
		return false;
	}

	// NOTE: ĳ����(��)�� ���� �̵� �ӵ��� �˾ƾ� �ϴµ�, �ش� ��(m_pkChar->m_pkMobData->m_table.sMovingSpeed)�� ���������� �����ؼ� �˾Ƴ� ���� ������
	// m_pkChar->m_pkMobData ���� invalid�� ��찡 ���� �߻���. ���� �ð������ ������ ������ �ľ��ϰ� �ϴ��� m_pkChar->m_pkMobData ���� �ƿ� ������� �ʵ��� ��.
	// ���⼭ �Ź� �˻��ϴ� ������ ���� �ʱ�ȭ �� �� ���� ���� ����� �������� ��쵵 ����.. -_-;; �ФФФФФФФФ�
	if (0 == m_originalMoveSpeed)
	{
		const CMob* mobData = CMobManager::Instance().Get(m_dwVnum);

		if (0 != mobData)
		{
			m_originalMoveSpeed = mobData->m_table.sMovingSpeed;
		}
	}
	float	START_FOLLOW_DISTANCE = 150.0f;		// �� �Ÿ� �̻� �������� �Ѿư��� ������
	float	START_RUN_DISTANCE = 600.0f;		// �� �Ÿ� �̻� �������� �پ �Ѿư�.

	float	RESPAWN_DISTANCE = 4500.f;			// �� �Ÿ� �̻� �־����� ���� ������ ��ȯ��.
	int		APPROACH = 200;						// ���� �Ÿ�

	bool bDoMoveAlone = true;					// ĳ���Ϳ� ������ ���� �� ȥ�� �������� �����ϰ��� ���� -_-;
	bool bRun = false;							// �پ�� �ϳ�?

	DWORD currentTime = get_dword_time();

	int32_t ownerX = m_pkOwner->GetX();
	int32_t ownerY = m_pkOwner->GetY();
	int32_t charX = m_pkChar->GetX();
	int32_t charY = m_pkChar->GetY();

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
		if ( fDist >= START_RUN_DISTANCE)
		{
			bRun = true;
		}

		m_pkChar->SetNowWalking(!bRun);		// NOTE: �Լ� �̸����� ���ߴ°��� �˾Ҵµ� SetNowWalking(false) �ϸ� �ٴ°���.. -_-;

		Follow(APPROACH);

		m_pkChar->SetLastAttacked(currentTime);
		m_dwLastActionTime = currentTime;
	}
	//else
	//{
	//	if (fabs(m_pkChar->GetRotation() - GetDegreeFromPositionXY(charX, charY, ownerX, ownerX)) > 10.f || fabs(m_pkChar->GetRotation() - GetDegreeFromPositionXY(charX, charY, ownerX, ownerX)) < 350.f)
	//	{
	//		m_pkChar->Follow(m_pkOwner, APPROACH);
	//		m_pkChar->SetLastAttacked(currentTime);
	//		m_dwLastActionTime = currentTime;
	//	}
	//}
	// Follow �������� ���ΰ� ���� �Ÿ� �̳��� ��������ٸ� ����
	else
	{
		m_pkChar->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
	}
	//else if (currentTime - m_dwLastActionTime > number(5000, 12000))
	//{
	//	this->_UpdatAloneActionAI(START_FOLLOW_DISTANCE / 2, START_FOLLOW_DISTANCE);
	//}

	return true;
}

bool CPetActor::Update(DWORD deltaTime)
{
	bool bResult = true;

	// �� ������ �׾��ų�, ��ȯ�� ���� ���°� �̻��ϴٸ� ���� ����. (NOTE: �������� �̷� ���� ������ ��ȯ�� ���� DEAD ���¿� ������ ��찡 ����-_-;)
	// ���� ��ȯ�� �������� ���ų�, ���� ���� ���°� �ƴ϶�� ���� ����.

#ifdef ENABLE_NEW_PET_SYSTEM
	if (IsSummoned() && m_pkChar->IsDead())
#else
	if (m_pkOwner->IsDead() || (IsSummoned() && m_pkChar->IsDead())
			|| NULL == ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID())
			|| ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID())->GetOwner() != this->GetOwner()
	   )
#endif
	{
		this->Unsummon();
		return true;
	}



	if (this->IsSummoned() && HasOption(EPetOption_Followable))
	{
		bResult = bResult && this->_UpdateFollowAI();
	}

	return bResult;
}

//NOTE : ����!!! MinDistance�� ũ�� ������ �� ������ŭ�� ��ȭ������ follow���� �ʴ´�,
bool CPetActor::Follow(float fMinDistance)
{
	// ������ ��ġ�� �ٶ���� �Ѵ�.
	if ( !m_pkOwner || !m_pkChar)
	{
		return false;
	}

	float fOwnerX = m_pkOwner->GetX();
	float fOwnerY = m_pkOwner->GetY();

	float fPetX = m_pkChar->GetX();
	float fPetY = m_pkChar->GetY();

	float fDist = DISTANCE_SQRT(fOwnerX - fPetX, fOwnerY - fPetY);
	if (fDist <= fMinDistance)
	{
		return false;
	}

	m_pkChar->SetRotationToXY(fOwnerX, fOwnerY);

	float fx, fy;

	float fDistToGo = fDist - fMinDistance;
	GetDeltaByDegree(m_pkChar->GetRotation(), fDistToGo, &fx, &fy);

	if (!m_pkChar->Goto((int)(fPetX + fx + 0.5f), (int)(fPetY + fy + 0.5f)) )
	{
		return false;
	}

	m_pkChar->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0, 0);

	return true;
}

void CPetActor::SetSummonItem (LPITEM pItem)
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

void CPetActor::GiveBuff()
{
#ifndef ENABLE_NEW_PET_SYSTEM
	// ��Ȳ �� ������ ���������� �߻���.
	if (34004 == m_dwVnum || 34009 == m_dwVnum)
	{
		if (NULL == m_pkOwner->GetDungeon())
		{
			return;
		}
	}
	LPITEM item = ITEM_MANAGER::instance().FindByVID(m_dwSummonItemVID);
	if (NULL != item)
	{
		item->ModifyPoints(true);
	}
#endif
	return ;
}

void CPetActor::ClearBuff()
{
#ifndef ENABLE_NEW_PET_SYSTEM
	if (NULL == m_pkOwner)
	{
		return ;
	}
	TItemTable* item_proto = ITEM_MANAGER::instance().GetTable(m_dwSummonItemVnum);
	if (NULL == item_proto)
	{
		return;
	}
	for (int i = 0; i < ITEM_APPLY_MAX_NUM; i++)
	{
		if (item_proto->aApplies[i].bType == APPLY_NONE)
		{
			continue;
		}
		m_pkOwner->ApplyPoint(item_proto->aApplies[i].bType, -item_proto->aApplies[i].lValue);
	}
#endif
	return ;
}

///////////////////////////////////////////////////////////////////////////////////////
//  CPetSystem
///////////////////////////////////////////////////////////////////////////////////////

CPetSystem::CPetSystem(LPCHARACTER owner)
{
//	assert(0 != owner && "[CPetSystem::CPetSystem] Invalid owner");

	m_pkOwner = owner;
	m_dwUpdatePeriod = 400;

	m_dwLastUpdateTime = 0;
}

CPetSystem::~CPetSystem()
{
	Destroy();
}

void CPetSystem::Destroy()
{
	for (TPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CPetActor* petActor = iter->second;

		if (0 != petActor)
		{
			delete petActor;
		}
	}
	event_cancel(&m_pkPetSystemUpdateEvent);
	m_petActorMap.clear();
}

/// �� �ý��� ������Ʈ. ��ϵ� ����� AI ó�� ���� ��.
bool CPetSystem::Update(DWORD deltaTime)
{
	bool bResult = true;

	DWORD currentTime = get_dword_time();

	// CHARACTER_MANAGER���� ĳ���ͷ� Update�� �� �Ű������� �ִ� (Pulse��� �Ǿ��ִ�)���� ���� �����Ӱ��� �ð��������� �˾Ҵµ�
	// ���� �ٸ� ���̶�-_-; ���⿡ �Է����� ������ deltaTime�� �ǹ̰� �����Ф�

	if (m_dwUpdatePeriod > currentTime - m_dwLastUpdateTime)
	{
		return true;
	}

	std::vector <CPetActor*> v_garbageActor;

	for (TPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CPetActor* petActor = iter->second;

		if (0 != petActor && petActor->IsSummoned())
		{
			LPCHARACTER pPet = petActor->GetCharacter();

			if (NULL == CHARACTER_MANAGER::instance().Find(pPet->GetVID()))
			{
				v_garbageActor.push_back(petActor);
			}
			else
			{
				bResult = bResult && petActor->Update(deltaTime);
			}
		}
	}
	for (std::vector<CPetActor*>::iterator it = v_garbageActor.begin(); it != v_garbageActor.end(); it++)
	{
		DeletePet(*it);
	}

	m_dwLastUpdateTime = currentTime;

	return bResult;
}

/// ���� ��Ͽ��� ���� ����
void CPetSystem::DeletePet(DWORD mobVnum)
{
	TPetActorMap::iterator iter = m_petActorMap.find(mobVnum);

	if (m_petActorMap.end() == iter)
	{
		sys_err("[CPetSystem::DeletePet] Can't find pet on my list (VNUM: %d)", mobVnum);
		return;
	}

	CPetActor* petActor = iter->second;

	if (0 == petActor)
	{
		sys_err("[CPetSystem::DeletePet] Null Pointer (petActor)");
	}
	else
	{
		delete petActor;
	}

	m_petActorMap.erase(iter);
}

/// ���� ��Ͽ��� ���� ����
void CPetSystem::DeletePet(CPetActor* petActor)
{
	for (TPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		if (iter->second == petActor)
		{
			delete petActor;
			m_petActorMap.erase(iter);

			return;
		}
	}

	sys_err("[CPetSystem::DeletePet] Can't find petActor(0x%x) on my list(size: %d) ", petActor, m_petActorMap.size());
}

void CPetSystem::Unsummon(DWORD vnum, bool bDeleteFromList)
{
	CPetActor* actor = this->GetByVnum(vnum);

	if (0 == actor)
	{
		sys_err("[CPetSystem::GetByVnum(%d)] Null Pointer (petActor)", vnum);
		return;
	}
	actor->Unsummon();
	if (true == bDeleteFromList)
	{
		this->DeletePet(actor);
	}

	bool bActive = false;
	for (TPetActorMap::iterator it = m_petActorMap.begin(); it != m_petActorMap.end(); it++)
	{
		bActive |= it->second->IsSummoned();
	}
	if (false == bActive)
	{
		event_cancel(&m_pkPetSystemUpdateEvent);
		m_pkPetSystemUpdateEvent = NULL;
	}
}

#ifdef ENABLE_NEW_PET_SYSTEM
	CPetActor* CPetSystem::Summon(DWORD mobVnum, LPITEM pSummonItem, const char* petName, bool bSpawnFar, BYTE level, DWORD options)
#else
	CPetActor* CPetSystem::Summon(DWORD mobVnum, LPITEM pSummonItem, const char* petName, bool bSpawnFar, DWORD options)
#endif
{
	CPetActor* petActor = this->GetByVnum(mobVnum);

	// ��ϵ� ���� �ƴ϶�� ���� ���� �� ���� ��Ͽ� �����.
	if (0 == petActor)
	{
		petActor = M2_NEW CPetActor(m_pkOwner, mobVnum, options);
		m_petActorMap.insert(std::make_pair(mobVnum, petActor));
	}

#ifdef ENABLE_NEW_PET_SYSTEM
	DWORD petVID = petActor->Summon(petName, pSummonItem, bSpawnFar, level);
#else
	DWORD petVID = petActor->Summon(petName, pSummonItem, bSpawnFar);
#endif

	if (NULL == m_pkPetSystemUpdateEvent)
	{
		petsystem_event_info* info = AllocEventInfo<petsystem_event_info>();

		info->pPetSystem = this;

		m_pkPetSystemUpdateEvent = event_create(petsystem_update_event, info, PASSES_PER_SEC(1) / 4);	// 0.25��
	}

	return petActor;
}


CPetActor* CPetSystem::GetByVID(DWORD vid) const
{
	CPetActor* petActor = 0;

	bool bFound = false;

	for (TPetActorMap::const_iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		petActor = iter->second;

		if (0 == petActor)
		{
			sys_err("[CPetSystem::GetByVID(%d)] Null Pointer (petActor)", vid);
			continue;
		}

		bFound = petActor->GetVID() == vid;

		if (true == bFound)
		{
			break;
		}
	}

	return bFound ? petActor : 0;
}

/// ��� �� �� �߿��� �־��� �� VNUM�� ���� ���͸� ��ȯ�ϴ� �Լ�.
CPetActor* CPetSystem::GetByVnum(DWORD vnum) const
{
	CPetActor* petActor = 0;

	TPetActorMap::const_iterator iter = m_petActorMap.find(vnum);

	if (m_petActorMap.end() != iter)
	{
		petActor = iter->second;
	}

	return petActor;
}

size_t CPetSystem::CountSummoned() const
{
	size_t count = 0;

	for (TPetActorMap::const_iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CPetActor* petActor = iter->second;

		if (0 != petActor)
		{
			if (petActor->IsSummoned())
			{
				++count;
			}
		}
	}

	return count;
}

void CPetSystem::RefreshBuff()
{
	for (TPetActorMap::const_iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CPetActor* petActor = iter->second;

		if (0 != petActor)
		{
			if (petActor->IsSummoned())
			{
				petActor->GiveBuff();
			}
		}
	}
}

#ifdef ENABLE_NEW_PET_SYSTEM
enum PET_CONFIG
{
	PET_MAX_SKILL_LV = 40,
	PET_MAX_SKILL_LV_BY_POINT = 10,
	PET_SKILL_START = 301,
	PET_SKILL_END = 312,
	PET_SKILL_COUNT = PET_SKILL_END - PET_SKILL_START,
	// PET_MAX_LV = PET_MAX_SKILL_LV_BY_POINT * PET_SKILL_COUNT,
	PET_MAX_LV = 150,
	PET_MAX_AFFECTS = 2,
	PET_DEFAULT_MODEL = 34005,
	PET_SKILL_MAX_LEN = 30,
	PET_ITEM_VALUE_MODEL_IDX = 0,
};

static int PET_EXP_DATA[PET_MAX_LV] =
{
	10000000,
	10000000,
	10000000,
	10000000,
	10000000,
	10000000,
	10000000,
	10000000,
	10000000,
	10000000,
	10000000,
	15000000,
	15000000,
	15000000,
	15000000,
	15000000,
	15000000,
	15000000,
	15000000,
	15000000,
	15000000,
	15000000,
	15000000,
	20000000,
	20000000,
	20000000,
	20000000,
	20000000,
	20000000,
	20000000,
	20000000,
	20000000,
	25000000,
	25000000,
	25000000,
	25000000,
	25000000,
	25000000,
	30000000,
	30000000,
	30000000,
	30000000,
	30000000,
	30000000,
	30000000,
	30000000,
	30000000,
	40000000,
	40000000,
	40000000,
	40000000,
	40000000,
	40000000,
	40000000,
	40000000,
	40000000,
	50000000,
	50000000,
	50000000,
	50000000,
	50000000,
	50000000,
	50000000,
	50000000,
	50000000,
	50000000,
	60000000,
	60000000,
	60000000,
	60000000,
	60000000,
	60000000,
	60000000,
	60000000,
	60000000,
	60000000,
	70000000,
	70000000,
	70000000,
	70000000,
	70000000,
	70000000,
	70000000,
	70000000,
	70000000,
	70000000,
	80000000,
	80000000,
	80000000,
	80000000,
	80000000,
	80000000,
	80000000,
	80000000,
	80000000,
	80000000,
	90000000,
	90000000,
	90000000,
	90000000,
	90000000,
	90000000,
	100000000,
	100000000,
	100000000,
	100000000,
	100000000,
	100000000,
	100000000,
	110000000,
	110000000,
	110000000,
	110000000,
	110000000,
	120000000,
	120000000,
	120000000,
	120000000,
	120000000,
	130000000,
	130000000,
	130000000,
	130000000,
	130000000,
	140000000,
	140000000,
	140000000,
	140000000,
	140000000,
	150000000,
	150000000,
	150000000,
	150000000,
	150000000,
	160000000,
	160000000,
	160000000,
	160000000,
	160000000,
	170000000,
	170000000,
	170000000,
	170000000,
	170000000,
	180000000,
	180000000,
	180000000,
	180000000,
	180000000,
};

static int PET_AFFECTS[PET_SKILL_COUNT][PET_MAX_AFFECTS][PET_MAX_SKILL_LV + 1] =
{
	{
		//skill1
		{APPLY_MAX_HP, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000, 2100, 2200, 2300, 2400, 2500, 2600, 2700, 2900, 3200, 3500, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },//bonus1
	},
	{
		//skill2
		{APPLY_ATTBONUS_MONSTER, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },//bonus1
	},
	{
		//skill3
		{APPLY_NORMAL_HIT_DAMAGE_BONUS, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 10, 11, 12, 13, 14, 15, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },//bonus1
	},
	{
		//skill4
		{APPLY_SKILL_DAMAGE_BONUS, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 10, 11, 12, 13, 14, 15, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },//bonus1
	},
	{
		//skill5
		{APPLY_ATT_GRADE_BONUS, 6, 12, 18, 24, 30, 36, 42, 48, 54, 60, 66, 72, 78, 84, 90, 96, 102, 108, 114, 120, 126, 132, 138, 142, 148, 148, 165, 175, 185, 200, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },//bonus1
		{APPLY_MAGIC_ATT_GRADE, 6, 12, 18, 24, 30, 36, 42, 48, 54, 60, 66, 72, 78, 84, 90, 96, 102, 108, 114, 120, 126, 132, 138, 142, 148, 148, 165, 175, 185, 200, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },//bonus1
	},
	{
		//skill6
		{APPLY_ATTBONUS_HUMAN, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },//bonus1
	},
	{
		//skill7
		{APPLY_STEAL_HP, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 10, 11, 12, 13, 14, 15, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }//bonus1
	},
	{
		//skill8
		{APPLY_ATTBONUS_WLADCA, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 10, 11, 12, 13, 14, 15, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },//bonus1
	},
	{
		//skill9
		{APPLY_SKILL_DEFEND_BONUS, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },//bonus1
	},
	{
		//skill10
		{APPLY_ATTBONUS_STONE, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 10, 11, 12, 13, 14, 15, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },//bonus1
	},
	{
		//skill11
		{APPLY_ATTBONUS_BOSS, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 10, 11, 12, 13, 14, 15, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },//bonus1
	},
};

DWORD CPetSystem::GetPetModel() const
{
	if (m_pkOwner)
	{
		LPITEM pet_item = m_pkOwner->GetWear(WEAR_NEW_PET);
		if (pet_item && pet_item->GetValue(PET_ITEM_VALUE_MODEL_IDX))
		{
			return pet_item->GetValue(PET_ITEM_VALUE_MODEL_IDX);
		}
	}

	return PET_DEFAULT_MODEL;
}
bool CPetSystem::SetExpPercent(int amount)
{
	if (m_pkOwner)
	{
		int pet_level = GetExp(false, true);
		if (pet_level >= PET_MAX_LV)
		{
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, "[LS;2341]");
			return false;
		}
		int64_t pet_max_exp = GetExp(true);
		int64_t add_exp = (pet_max_exp * amount) / 100;

		m_pkOwner->SetQuestFlag("pet_system.exp", m_pkOwner->GetQuestFlag("pet_system.exp") + add_exp);

		int pet_exp = GetExp();

		int pet_exp_need = GetExp(true);
		if (pet_exp >= pet_exp_need)
		{
			m_pkOwner->SetQuestFlag("pet_system.exp", 0);
			m_pkOwner->SetQuestFlag("pet_system.lv", pet_level + 1);
			int points = m_pkOwner->GetQuestFlag("pet_system.points");
			m_pkOwner->SetQuestFlag("pet_system.points", points + 1);
			AddPetAffect();
		}
		SendPetInfo();
		return true;
	}
	return false;
}
void CPetSystem::SummonPet()
{
	if (!m_pkOwner)
	{
		return;
	}

	if (CountSummoned() > 0)
	{
		for (TPetActorMap::const_iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
		{
			CPetActor* petActor = iter->second;
			if (petActor && petActor->IsSummoned())
			{
				petActor->Unsummon();
			}
		}
	}


	LPITEM pet_item = m_pkOwner->GetWear(WEAR_NEW_PET);
	if (!pet_item)
	{
		return;
	}

	Summon(GetPetModel(), NULL, GetOrgName().c_str(), false, m_pkOwner->GetQuestFlag("pet_system.lv"));
	AddPetAffect();
}

void CPetSystem::UpdatePet()
{
	if (!m_pkOwner || CountSummoned() < 1)
	{
		return;
	}

	for (TPetActorMap::const_iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CPetActor* petActor = iter->second;
		if (petActor && petActor->IsSummoned())
		{

			LPCHARACTER pet = petActor->GetCharacter();
			if (!pet)
			{
				continue;
			}

			int current_level = m_pkOwner->GetQuestFlag("pet_system.lv");
			petActor->SetLevel(current_level);

			pet->UpdatePacket();
			//pet->EffectPacket(SE_PET_LV_UP);
		}
	}
}

bool CPetSystem::CheckExtraBonus()
{
	char act_skill[PET_SKILL_MAX_LEN];
	for (unsigned int i = PET_SKILL_START; i < PET_SKILL_END; i++)
	{
		const int id = (i - PET_SKILL_START);
		snprintf(act_skill, sizeof(act_skill), "pet_system.skill%d", i);
		if (m_pkOwner->GetQuestFlag(act_skill) != 30) {
			return false;
		}
	}
	return true;
}

void CPetSystem::AddPetAffect()
{
	if (!m_pkOwner || CountSummoned() < 1)
	{
		return;
	}

	m_pkOwner->RemoveAffect(AFFECT_NEW_PET);

	LPITEM pet_item = m_pkOwner->GetWear(WEAR_NEW_PET);
	if (!pet_item)
	{
		return;
	}

	char act_skill[PET_SKILL_MAX_LEN];
	for (unsigned int i = PET_SKILL_START; i < PET_SKILL_END; i++)
	{
		const int id = (i - PET_SKILL_START);
		snprintf(act_skill, sizeof(act_skill), "pet_system.skill%d", i);
		const int act_skill_level = MINMAX(0, m_pkOwner->GetQuestFlag(act_skill), PET_MAX_SKILL_LV);
		if (act_skill_level)
		{
			for (unsigned int b = 0; b < PET_MAX_AFFECTS; b++)
			{
				const int affect_id = PET_AFFECTS[id][b][0];
				const int affect_value = PET_AFFECTS[id][b][act_skill_level];
				if (affect_id > 0 && affect_value > 0)
				{
					if (!m_pkOwner->FindAffect(AFFECT_NEW_PET, aApplyInfo[affect_id].bPointType)) {
						if (CheckExtraBonus()) {
							m_pkOwner->AddAffect(AFFECT_NEW_PET, aApplyInfo[affect_id].bPointType, affect_value+((affect_value*30)/100), AFF_NONE, 60 * 60 * 24 * 30, 0, false);
						} else {
							m_pkOwner->AddAffect(AFFECT_NEW_PET, aApplyInfo[affect_id].bPointType, affect_value, AFF_NONE, 60 * 60 * 24 * 30, 0, false);
						}
					}
				}
			}
		}
	}
}

void CPetSystem::SendPetInfo(bool summon)
{
	if (!m_pkOwner)
	{
		return;
	}

	if (m_pkOwner->IsObserverMode() || m_pkOwner->IsAffectFlag(AFF_INVISIBILITY))
	{
		m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "pet_info clear");
		return;
	}

	if (summon)
	{
		SummonPet();
	}

	char skill_data[CHAT_MAX_LEN - 1];
	int skill_len = 0;
	char act_skill[PET_SKILL_MAX_LEN];

	for (unsigned int i = PET_SKILL_START; i < PET_SKILL_END; i++)
	{
		int id = (i - PET_SKILL_START);
		snprintf(act_skill, sizeof(act_skill), "pet_system.skill%d", i);
		int act_skill_level = MINMAX(0, m_pkOwner->GetQuestFlag(act_skill), PET_MAX_SKILL_LV);
		skill_len += snprintf(skill_data + skill_len, sizeof(skill_data) - skill_len, "%d&", act_skill_level);
		for (unsigned int b = 0; b < PET_MAX_AFFECTS; b++)
		{
			const int affect_id = PET_AFFECTS[id][b][0];
			int affect_value = 0;
			if (act_skill_level)
			{
				affect_value = PET_AFFECTS[id][b][act_skill_level];
			}
			skill_len += snprintf(skill_data + skill_len, sizeof(skill_data) - skill_len, "%d,%d@", affect_id, affect_value);
		}
		skill_len += snprintf(skill_data + skill_len, sizeof(skill_data) - skill_len, "#");
	}

	std::string pet_name = GetOrgName();
	for (size_t i = 0; i < pet_name.length(); ++i)
	{
		if (pet_name[i] == ' ' || pet_name[i] == '\t')
			pet_name[i] = '+';
	}
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "pet_info %s|%d|%d|%d|%d|%d|%s", pet_name.c_str(), GetPetModel(), GetExp(false, true), GetExp(), GetExp(true), (int)m_pkOwner->GetQuestFlag("pet_system.points"), skill_data);
}

void CPetSystem::SetExp(int count)
{
	if (!m_pkOwner || count < 1 || CountSummoned() < 1)
	{
		return;
	}

	LPITEM pet_item = m_pkOwner->GetWear(WEAR_NEW_PET);
	if (!pet_item)
	{
		return;
	}

	const int pet_level = GetExp(false, true);
	if (pet_level >= PET_MAX_LV)
	{
		return;
	}

	m_pkOwner->SetQuestFlag("pet_system.exp", (int)m_pkOwner->GetQuestFlag("pet_system.exp") + count);

	const int pet_exp = GetExp();
	const int pet_exp_need = GetExp(true);
	if (pet_exp >= pet_exp_need)
	{
		m_pkOwner->SetQuestFlag("pet_system.exp", 0);
		m_pkOwner->SetQuestFlag("pet_system.lv", pet_level + 1);
		m_pkOwner->SetQuestFlag("pet_system.points", (int)m_pkOwner->GetQuestFlag("pet_system.points") + 1);
		UpdatePet();
		SendPetInfo();
		return;
	}

	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "pet_info state|%d|%d", (int)m_pkOwner->GetQuestFlag("pet_system.exp"), (int)m_pkOwner->GetQuestFlag("pet_system.points"));
}

int CPetSystem::GetExp(bool exp_need, bool lv_only) const
{
	if (!m_pkOwner)
	{
		return 0;
	}
	if (exp_need)
	{
		const int level = (int)m_pkOwner->GetQuestFlag("pet_system.lv");
		const int safe_index = std::clamp(level, 0, PET_MAX_LV - 1);
		return PET_EXP_DATA[safe_index];
	}
	else if (lv_only)
	{
		return m_pkOwner->GetQuestFlag("pet_system.lv");
	}
	return m_pkOwner->GetQuestFlag("pet_system.exp");
}

int CPetSystem::GetLevel() const
{
	if (!m_pkOwner)
	{
		return 0;
	}
	return m_pkOwner->GetQuestFlag("pet_system.lv");
}

void CPetSystem::SetSkillUp(int skill_id, bool set_from_quest)
{
	if (!m_pkOwner || skill_id < PET_SKILL_START || skill_id > PET_SKILL_END || CountSummoned() < 1)
	{
		return;
	}

	LPITEM pet_item = m_pkOwner->GetWear(WEAR_NEW_PET);
	if (!pet_item)
	{
		return;
	}

	char act_skill[PET_SKILL_MAX_LEN];
	snprintf(act_skill, sizeof(act_skill), "pet_system.skill%d", skill_id);
	const int act_skill_level = (int)m_pkOwner->GetQuestFlag(act_skill);
	const int points = (int)m_pkOwner->GetQuestFlag("pet_system.points");

	if (!set_from_quest && points < 1)
	{
		return;
	}

	if (act_skill_level >= (set_from_quest ? PET_MAX_SKILL_LV : PET_MAX_SKILL_LV_BY_POINT))
	{
		return;
	}

	if (!set_from_quest)
	{
		m_pkOwner->SetQuestFlag("pet_system.points", points - 1);
	}

	m_pkOwner->SetQuestFlag(act_skill, act_skill_level + 1);
	AddPetAffect();
	SendPetInfo();
}

std::string CPetSystem::GetOrgName() const
{
	const CMob * pkMob = CMobManager::instance().Get(GetPetModel());
	if (!m_pkOwner || !pkMob)
	{
		return "None";
	}

	const char* pPetName = CHorseNameManager::instance().GetPetName(m_pkOwner->GetPlayerID());

	std::string pet_name = m_pkOwner->GetName();
	pet_name += " (";

	if (pPetName != NULL && strlen(pPetName))
	{
		pet_name += pPetName;
	}
	else
	{
		pet_name += pkMob->m_table.szLocaleName;
	}
	pet_name += ")";

	return pet_name.c_str();
}
#endif

#ifdef ENABLE_NEW_PET_SYSTEM_BOOK
namespace NewPetSystemHelper
{
// Constants
const BYTE SKILL_BONUS_VALUE = 50;
const BYTE PET_MAX_SKILL_LV = ::PET_MAX_SKILL_LV;
const std::string PET_SKILL_LEVEL = "pet_system.skill%d";
const std::string PET_SKILL_DELAY = "pet_system.skill_delay_id%d_grade_%d";
const std::string PET_SKILL_TRAIN_COUNT = "pet_system.skill_train_%d";
const int PET_SKILL_MASTER_LEVEL = 10;
const int PET_SKILL_GRAND_MASTER_LEVEL = 20;
const int PET_SKILL_GRAND_PERFECT_LEVEL = 30;

std::map<DWORD, SSkillBookInfo> m_book_config;
std::vector<WORD> v_required_items(PET_MAX_SKILL_LV + 1, 1); // Set 1 by default

void AddBookRecord(const DWORD dwVnum, const WORD wSkillID, const BYTE bSkillGrade, const BYTE bChance, const time_t ttDelay)
{
	m_book_config.insert({dwVnum, {wSkillID, bSkillGrade, bChance, ttDelay}});
}

void AddBookCount(const WORD wLevel, const WORD wCount)
{
	v_required_items.at(wLevel) = wCount;
}

bool FindBookByVnum(const DWORD dwVnum)
{
	return (m_book_config.find(dwVnum) != m_book_config.end());
}

// Beware to check if id existed before
const SSkillBookInfo & GetBookInfo(const DWORD dwVnum)
{
	return m_book_config[dwVnum];
}

BYTE GetSkillLevel(LPCHARACTER ch, const WORD wSkillID)
{
	return ch->GetQuestFlag((boost::format{PET_SKILL_LEVEL.c_str()} % wSkillID).str());
}

BYTE GetSkillGrade(const WORD wSkillLevel)
{
	// Hardcoding
	if (wSkillLevel < PET_SKILL_MASTER_LEVEL)
	{
		return 0;
	}
	else if (wSkillLevel >= PET_SKILL_MASTER_LEVEL && wSkillLevel < PET_SKILL_GRAND_MASTER_LEVEL)
	{
		return 1;
	}
	else if (wSkillLevel >= PET_SKILL_GRAND_MASTER_LEVEL && wSkillLevel < PET_SKILL_GRAND_PERFECT_LEVEL)
	{
		return 2;
	}

	return 3;
}

time_t GetSkillDelay(LPCHARACTER ch, const WORD wSkillID, const WORD wSkillLevel)
{
	return ch->GetQuestFlag((boost::format{PET_SKILL_DELAY.c_str()} % wSkillID % GetSkillGrade(wSkillID)).str());
}

void SetSkillDelay(LPCHARACTER ch, const WORD wSkillID, const WORD wSkillLevel, const time_t ttDelay)
{
	ch->SetQuestFlag((boost::format{PET_SKILL_DELAY.c_str()} % wSkillID % GetSkillGrade(wSkillID)).str(), get_global_time() + ttDelay);
}

int TrainSkill(LPCHARACTER ch, const WORD wSkillID)
{
	// Incrementing flag
	int iFlagValue = ch->GetQuestFlag((boost::format{PET_SKILL_TRAIN_COUNT.c_str()} % wSkillID).str()) + 1;
	ch->SetQuestFlag((boost::format{PET_SKILL_TRAIN_COUNT.c_str()} % wSkillID).str(), iFlagValue);

	// Checking if user's reached value
	if (iFlagValue >= v_required_items.at(GetSkillLevel(ch, wSkillID)))
	{
		// Clearing flag
		ch->SetQuestFlag((boost::format{PET_SKILL_TRAIN_COUNT.c_str()} % wSkillID).str(), 0);

		WORD wNextLevel = GetSkillLevel(ch, wSkillID) + 1;
		// Checking if skill is upgradeable
		if (wNextLevel == PET_SKILL_MASTER_LEVEL - 3)
		{
			wNextLevel = PET_SKILL_MASTER_LEVEL;
		}

		// Updating flag
		ch->SetQuestFlag((boost::format{PET_SKILL_LEVEL.c_str()} % wSkillID).str(), wNextLevel);

		// Refreshing pet
		ch->AddPetAffect();
		ch->SendPetInfo();

		return 0;
	}

	return (v_required_items.at(wSkillID) - iFlagValue);
}
}
#endif

