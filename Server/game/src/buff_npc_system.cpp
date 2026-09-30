#include "stdafx.h"

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM

#include "utils.h"
#include "vector.h"
#include "char.h"
#include "sectree_manager.h"
#include "char_manager.h"
#include "mob_manager.h"
#include "buff_npc_system.h"
#include "../../common/VnumHelper.h"
#include "packet.h"
#include "item_manager.h"
#include "item.h"
#include "affect.h"
#include "skill.h"
#include "db.h"
#ifdef ENABLE_NEWSTUFF
#include "../../common/PulseManager.h"
#endif

extern int passes_per_sec;

const	DWORD m_dwSkillVnum[3] = { 94, 96, 111 };

const	DWORD m_dwLearnGrandmasterSkillCost[10] = {
	500000,
	1000000,
	2000000,
	3000000,
	5000000,
	7500000,
	12000000,
	15000000,
	20000000,
	25000000,
};

EVENTINFO(buffnpcsystem_event_info)
{
	CBuffNPCActor* pBuffNPCSystem;
};

EVENTFUNC(buffnpcsystem_update_event)
{
	buffnpcsystem_event_info* info = dynamic_cast<buffnpcsystem_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "check_speedhack_event> <Factor> Null pointer" );
		return 0;
	}

	CBuffNPCActor*	pBuffNPCSystem = info->pBuffNPCSystem;

	if (NULL == pBuffNPCSystem)
		return 0;

	pBuffNPCSystem->Update(0);
	return PASSES_PER_SEC(1) / 4;
}

///////////////////////////////////////////////////////////////////////////////////////
//  CbuffNPCActor
///////////////////////////////////////////////////////////////////////////////////////

CBuffNPCActor::CBuffNPCActor(LPCHARACTER owner, DWORD options)
{
	m_dwVID = 0;
	m_dwOptions = options;
	m_dwLastActionTime = 0;

	m_dwIntValue = 0;
		
	m_pkChar = 0;
	m_pkOwner = owner;

	m_originalMoveSpeed = 0;

	m_dwSex = -1;
	m_dwIntValue = 0;
	
	for (int i = 0; i < 3; i++) {
		m_dwSkill[i] = 0;
		m_dwSkillNextUse[i] = 0;
		m_dwSkillInUse[i] = 0;
	}
	
	m_dwUpdatePeriod = 400;
	m_dwLastUpdateTime = 0;
}

CBuffNPCActor::~CBuffNPCActor()
{
	this->Destroy();
}

void CBuffNPCActor::Destroy()
{
	SetIsActive(false);

	// Do NOT call Unsummon() here — it calls ComputePoints/ChatPacket on
	// m_pkOwner which may be in mid-destruction. Clean up the buff NPC
	// character directly without touching the owner.
	if (NULL != m_pkChar)
	{
		SaveSQL();
		m_pkChar->SetBuffNPC(false);
		M2_DESTROY_CHARACTER(m_pkChar);
		m_pkChar = 0;
	}

	if (m_pkBuffNPCSystemExpireEvent)
		event_cancel(&m_pkBuffNPCSystemExpireEvent);

	m_pkOwner = 0;
	m_dwSex = -1;
	m_dwIntValue = 0;
	
	for (int i = 0; i < 3; i++) {
		m_dwSkill[i] = 0;
		m_dwSkillNextUse[i] = 0;
		m_dwSkillInUse[i] = 0;
	}

	m_pkBuffNPCSystemExpireEvent = NULL;
}

void CBuffNPCActor::LoadBuffNPC()
{
	std::unique_ptr<SQLMsg> pMsg2(DBManager::instance().DirectQuery("SELECT name, sex, intvalue, skill0lv, skill1lv, skill2lv FROM player_buff_npc WHERE player_id = %lu", m_pkOwner->GetPlayerID()));
	if (pMsg2->Get()->uiNumRows > 0) {
		MYSQL_ROW row = mysql_fetch_row(pMsg2->Get()->pSQLResult);			
		this->SetName(row[0]);		
		this->m_dwSex					= atoi(row[1]);
		this->m_dwVnum 				= atoi(row[1])+10;
		this->m_dwIntValue				= atoi(row[2]);
		this->m_dwSkill[0] 				= atoi(row[3]);
		this->m_dwSkill[1] 				= atoi(row[4]);
		this->m_dwSkill[2] 				= atoi(row[5]);
	}
	else {
		SetIsActive(false);
		return;
	}
	
	for (int i = 0; i < 3; i++) {
		char flag[128+1];
		memset(flag, 0, sizeof(flag));
		snprintf(flag, sizeof(flag), "buff_npc_learn.%u.use_deactivate", m_dwSkillVnum[i]);
		m_dwSkillInUse[i] = m_pkOwner->GetQuestFlag(flag);
	}

	SetIsActive(true);
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "BuffNPCBasicInfo %s %d %d", m_name.c_str(), m_dwSex, m_dwIntValue);
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "BuffNPCSkillInfo %d %d %d", m_dwSkill[0], m_dwSkill[1], m_dwSkill[2]);
}

DWORD CBuffNPCActor::Summon()
{
#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(m_pkOwner->GetPlayerID(), ePulse::BuffSummon, std::chrono::milliseconds(5000)))
    {
        m_pkOwner->ChatPacket(CHAT_TYPE_INFO, "[LS;2110;%.2f]", PULSEMANAGER_CLOCK_TO_SEC2(m_pkOwner->GetPlayerID(), ePulse::BuffSummon));
        return false;
    }
#endif
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT * FROM srv1_player.player_buff_npc WHERE player_id=%ld LIMIT 1", m_pkOwner->GetPlayerID()));
	if (pMsg->Get()->uiNumRows == 0) {
		return 0;
	}

	int32_t x = m_pkOwner->GetX();
	int32_t y = m_pkOwner->GetY();
	int32_t z = m_pkOwner->GetZ();

	x += number(-300, 300);
	y += number(-300, 300);

	if (0 != m_pkChar) {
		m_pkChar->Show (m_pkOwner->GetMapIndex(), x, y);
		m_dwVID = m_pkChar->GetVID();

		return m_dwVID;
	}
	
	m_pkChar = CHARACTER_MANAGER::instance().SpawnMob( m_dwVnum, m_pkOwner->GetMapIndex(), x, y, z, false, (int)(m_pkOwner->GetRotation()+180), false);

	if (0 == m_pkChar) {
		sys_err("[CBuffNPCSystem::Summon] Failed to summon buff. (vnum: %d)", m_dwVnum);
		return 0;
	}

	m_pkChar->SetBuffNPC(true);
	m_pkChar->SetEmpire(m_pkOwner->GetEmpire());
	m_dwVID = m_pkChar->GetVID();

	SetName(m_name.c_str());
	
	buffnpcsystem_event_info* info = AllocEventInfo<buffnpcsystem_event_info>();
	info->pBuffNPCSystem = this;
	m_pkBuffNPCSystemExpireEvent = event_create(buffnpcsystem_update_event, info, PASSES_PER_SEC(1) / 4);
		
	m_pkOwner->ComputePoints();
	m_pkOwner->SetBuffNPCInt(m_dwIntValue);
	m_pkChar->Show(m_pkOwner->GetMapIndex(), x, y, z);
	
	UpdateBuffEquipment();
	LPITEM iExpItem = m_pkOwner->GetBuffWear(3);
	if (iExpItem != NULL)
		iExpItem->StartUniqueExpireEvent();
	
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "BuffNPCSummon");

	return m_dwVID;
}

void CBuffNPCActor::Unsummon()
{
	if (true == this->IsSummoned())
	{
		if (NULL != m_pkOwner)
			m_pkOwner->ComputePoints();

		if (NULL != m_pkChar) {
			SaveSQL();
			m_pkChar->SetBuffNPC(false);
			M2_DESTROY_CHARACTER(m_pkChar);
			m_pkChar = 0;
		}

		if (NULL != m_pkOwner)
		{
			LPITEM iExpItem = m_pkOwner->GetBuffWear(3);
			if (iExpItem != NULL)
				iExpItem->StopUniqueExpireEvent();

			m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "BuffNPCUnsummon");
		}

		event_cancel(&m_pkBuffNPCSystemExpireEvent);
	}
}

void CBuffNPCActor::ClearBuffNPC()
{
	m_dwSex = -1;
	m_dwIntValue = 0;
	
	for (int i = 0; i < 3; i++) {
		m_dwSkill[i] = 0;
		m_dwSkillNextUse[i] = 0;
		m_dwSkillInUse[i] = 0;
	}
	
	SetIsActive(false);
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "BuffNPCClear");
}

void CBuffNPCActor::SaveSQL() {
	DBManager::instance().DirectQuery
	(
		"UPDATE player_buff_npc SET player_name='%s', name='%s', sex=%d, intvalue=%d, skill0lv=%d, skill1lv=%d, skill2lv=%d WHERE player_id = %d ",
		m_pkOwner->GetName(), m_name.c_str(), this->m_dwSex, this->m_dwIntValue,
		this->m_dwSkill[0], this->m_dwSkill[1], this->m_dwSkill[2], 
		m_pkOwner->GetPlayerID()
	);
}

void CBuffNPCActor::UpdateBuffEquipment()
{
	if (NULL == m_pkChar)
		return;
	
	LPITEM	iWeapon = m_pkOwner->GetBuffWear(0);
	LPITEM	iHead = m_pkOwner->GetBuffWear(1);
	LPITEM	iArmor = m_pkOwner->GetBuffWear(2);

	if (iWeapon != NULL)
		m_pkChar->SetBuffWearWeapon(iWeapon->GetVnum());
	else
		m_pkChar->SetBuffWearWeapon(0);

	if (iHead != NULL)
		m_pkChar->SetBuffWearHead(iHead->GetVnum());
	else
		m_pkChar->SetBuffWearHead(0);
	
	if (iArmor != NULL)
		m_pkChar->SetBuffWearArmor(iArmor->GetVnum());
	else
		m_pkChar->SetBuffWearArmor(0);

	// Recalculate intelligence: base 4 + socket(0) values from equipped buff costumes
	DWORD totalInt = 4;
	if (iWeapon != NULL) totalInt += (DWORD)iWeapon->GetSocket(0);
	if (iHead != NULL)   totalInt += (DWORD)iHead->GetSocket(0);
	if (iArmor != NULL)  totalInt += (DWORD)iArmor->GetSocket(0);
	if (totalInt != m_dwIntValue)
	{
		m_dwIntValue = totalInt;
		if (m_pkChar)
			m_pkChar->SetBuffNPCInt(m_dwIntValue);
		if (m_pkOwner)
			m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "BuffNPCBasicInfo %s %d %d", m_name.c_str(), m_dwSex, m_dwIntValue);
	}

	m_pkChar->UpdatePacket();
}

void CBuffNPCActor::SetName(const std::string& name)
{
    std::string buffName = name.empty() ? std::string(m_pkOwner->GetName()) + "'s Buff" : name;

    if (IsSummoned()) {
        m_pkChar->SetName(buffName);
        m_pkChar->UpdatePacket();
    }

    m_name = buffName;
    m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "BuffNPCBasicInfo %s %d %d", m_name.c_str(), m_dwSex, m_dwIntValue);
}

void CBuffNPCActor::SetInt(DWORD value)
{
	m_dwIntValue = value;
	if (m_pkChar)
		m_pkChar->SetBuffNPCInt(m_dwIntValue);
	if (m_pkOwner)
		m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "BuffNPCBasicInfo %s %d %d", m_name.c_str(), m_dwSex, m_dwIntValue);
}

bool CBuffNPCActor::UseSkill()
{
	if (NULL == m_pkOwner || m_pkOwner->IsDead() != false || m_pkOwner->IsStun() != false || get_dword_time() - this->GetLastSkillTime() < 4000)
		return false;

	if(m_dwSkillInUse[0] != 1 && this->m_dwSkill[0] > 0 && m_pkOwner->IsAffectFlag(AFF_HOSIN) == false && m_dwSkillNextUse[0] < get_dword_time())
	{
		CSkillProto * pkSk = CSkillManager::instance().Get(SKILL_HOSIN);
		int iCooltime = (int) pkSk->kCooldownPoly.Eval();
		m_dwSkillNextUse[0] = iCooltime * 1000 + get_dword_time();

		m_pkOwner->ComputeSkill(SKILL_HOSIN, m_pkOwner, this->m_dwSkill[0], true);
		this->SetLastSkillTime(get_dword_time());
		m_pkChar->SendBuffNPCUseSkillPacket(SKILL_HOSIN, this->m_dwSkill[0]);
		m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "BuffNPCSkillCooltime %d %d", 0, iCooltime);
		return true;
	}
	else if(m_dwSkillInUse[1] != 1 && this->m_dwSkill[1] > 0 && m_pkOwner->IsAffectFlag(AFF_GICHEON) == false && m_dwSkillNextUse[1] < get_dword_time())
	{
		CSkillProto * pkSk = CSkillManager::instance().Get(SKILL_GICHEON);
		int iCooltime = (int) pkSk->kCooldownPoly.Eval();
		m_dwSkillNextUse[1] = iCooltime * 1000 + get_dword_time();
		
		m_pkOwner->ComputeSkill(SKILL_GICHEON, m_pkOwner, this->m_dwSkill[1], true);	
		this->SetLastSkillTime(get_dword_time());	
		m_pkChar->SendBuffNPCUseSkillPacket(SKILL_GICHEON, this->m_dwSkill[1]);
		m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "BuffNPCSkillCooltime %d %d", 1, iCooltime);
		return true;
	}
	else if(m_dwSkillInUse[2] != 1 && this->m_dwSkill[2] > 0 && m_pkOwner->IsAffectFlag(AFF_JEUNGRYEOK) == false && m_dwSkillNextUse[2] < get_dword_time())
	{
		CSkillProto * pkSk = CSkillManager::instance().Get(SKILL_JEUNGRYEOK);
		int iCooltime = (int) pkSk->kCooldownPoly.Eval();
		m_dwSkillNextUse[2] = iCooltime * 1000 + get_dword_time();
		
		m_pkOwner->ComputeSkill(SKILL_JEUNGRYEOK, m_pkOwner, this->m_dwSkill[2], true);	
		this->SetLastSkillTime(get_dword_time());	
		m_pkChar->SendBuffNPCUseSkillPacket(SKILL_JEUNGRYEOK, this->m_dwSkill[2]);
		m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "BuffNPCSkillCooltime %d %d", 2, iCooltime);
		return true;
	}
	return false;
}

void CBuffNPCActor::SetSkill(DWORD dwSkillSlot, DWORD dwSkillLevel)
{
	m_dwSkill[dwSkillSlot] = dwSkillLevel;
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "BuffNPCSkillInfo %d %d %d", m_dwSkill[0], m_dwSkill[1], m_dwSkill[2]);
}

void CBuffNPCActor::SetChangeSex()
{	
#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(m_pkOwner->GetPlayerID(), ePulse::BuffChangeSex, std::chrono::milliseconds(10000)))
    {
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, "[LS;2200;%.2f]", PULSEMANAGER_CLOCK_TO_SEC2(m_pkOwner->GetPlayerID(), ePulse::BuffChangeSex));
        return;
    }
#endif

	if (false == IsSummoned()) {
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, "[LS;2201]");
		return;
	}
	
	if (!m_pkOwner->IsBuffEquipEmpty()){
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, "[LS;2202]");
		return;
	}

	if (m_dwSex == 0)
		m_dwSex = 1;
	else if (m_dwSex == 1)
		m_dwSex = 0;
	else
		m_dwSex = -1;
	
	m_dwVnum = m_dwSex+10;
	SaveSQL();
	
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "BuffNPCBasicInfo %s %d %d", m_name.c_str(), m_dwSex, m_dwIntValue);
	if (IsSummoned())
		Unsummon();

	Summon();
}

int CBuffNPCActor::GetSkillID(int iSkillVnum)
{
	for (int i = 0; i < 3; i++)
	{
		if (iSkillVnum == m_dwSkillVnum[i])
			return i;
	}
	return -1;
}

int CBuffNPCActor::GetSkillLevel(int iSkillVnum)
{
	return m_dwSkill[iSkillVnum];
}

int CBuffNPCActor::GetSkillGrade(int iSkillLevel)
{
	if (iSkillLevel >= 0 && iSkillLevel < 20)
		return 0;
	else if (iSkillLevel >= 20 && iSkillLevel < 30)
		return 1;
	else if (iSkillLevel >= 30 && iSkillLevel < 40) 
		return 2;
	else if (iSkillLevel == 40)
		return 3;
	else
		return -1;
}

bool CBuffNPCActor::Update(DWORD deltaTime)
{
	bool bResult = true;

	DWORD currentTime = get_dword_time();

	if (this->UseSkill())
		return true;

	if (this->IsSummoned() && HasOption(EBuffNPCOption_Followable))
		bResult = bResult && this->_UpdateFollowAI();

	m_dwLastUpdateTime = currentTime;
	
	return bResult;
}

bool CBuffNPCActor::_UpdateFollowAI()
{
	if (0 == m_pkChar->m_pkMobData) {
		return false;
	}

	if (0 == m_originalMoveSpeed)
	{
		const CMob* mobData = CMobManager::Instance().Get(m_dwVnum);

		if (0 != mobData)
			m_originalMoveSpeed = mobData->m_table.sMovingSpeed;
	}
	float	START_FOLLOW_DISTANCE = 200.0f;
	// float	START_RUN_DISTANCE = 900.0f;

	float	RESPAWN_DISTANCE = 1800.0f;
	int		APPROACH = 200;

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

bool CBuffNPCActor::Follow(float fMinDistance)
{
	if( !m_pkOwner || !m_pkChar) 
		return false;

	float fOwnerX = m_pkOwner->GetX();
	float fOwnerY = m_pkOwner->GetY();

	float fBuffX = m_pkChar->GetX();
	float fBuffY = m_pkChar->GetY();

	float fDist = DISTANCE_SQRT(fOwnerX - fBuffX, fOwnerY - fBuffY);
	if (fDist <= fMinDistance)
		return false;

	m_pkChar->SetRotationToXY(fOwnerX, fOwnerY);

	float fx, fy;

	float fDistToGo = fDist - fMinDistance;
	GetDeltaByDegree(m_pkChar->GetRotation(), fDistToGo, &fx, &fy);
	
	if (!m_pkChar->Goto((int)(fBuffX+fx+0.5f), (int)(fBuffY+fy+0.5f)) )
		return false;

	m_pkChar->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0, 0);

	return true;
}

bool CBuffNPCActor::_UpdatAloneActionAI()
{
	float fDist = number(500.f, 900.f);
	float r = (float)number (0, 359);
	float dest_x = GetOwner()->GetX() + fDist * cos(r);
	float dest_y = GetOwner()->GetY() + fDist * sin(r);
	
	m_pkChar->SetNowWalking(true);
	
	if (!m_pkChar->IsStateMove() && m_pkChar->Goto(dest_x, dest_y))
		m_pkChar->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);

	m_dwLastActionTime = get_dword_time();

	return true;
}
#endif