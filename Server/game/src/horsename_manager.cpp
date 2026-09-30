#include "stdafx.h"
#include "horsename_manager.h"
#include "desc_client.h"
#include "char_manager.h"
#include "char.h"
#include "affect.h"
#include "utils.h"

CHorseNameManager::CHorseNameManager()
{
	m_mapHorseNames.clear();
#ifdef ENABLE_NEW_PET_SYSTEM
	m_mapPetNames.clear();
#endif
}

const char* CHorseNameManager::GetHorseName(DWORD dwPlayerID)
{
	std::map<DWORD, std::string>::iterator iter;

	iter = m_mapHorseNames.find(dwPlayerID);

	if ( iter != m_mapHorseNames.end() )
	{
		return iter->second.c_str();
	}
	else
	{
		return NULL;
	}
}

void CHorseNameManager::UpdateHorseName(DWORD dwPlayerID, const char* szHorseName, bool broadcast)
{
	if ( szHorseName == NULL )
	{
		sys_err("HORSE_NAME: NULL NAME (%u)", dwPlayerID);
		szHorseName = "";
	}

	sys_log(0, "HORSENAME: update %u %s", dwPlayerID, szHorseName);

	m_mapHorseNames[dwPlayerID] = szHorseName;

	if ( broadcast == true )
	{
		BroadcastHorseName(dwPlayerID, szHorseName);
	}
}

void CHorseNameManager::BroadcastHorseName(DWORD dwPlayerID, const char* szHorseName)
{
	TPacketUpdateHorseName packet;
	packet.dwPlayerID = dwPlayerID;
	strlcpy(packet.szHorseName, szHorseName, sizeof(packet.szHorseName));

	db_clientdesc->DBPacket(HEADER_GD_UPDATE_HORSE_NAME, 0, &packet, sizeof(TPacketUpdateHorseName));
}

void CHorseNameManager::Validate(LPCHARACTER pChar)
{
	CAffect *pkAff = pChar->FindAffect(AFFECT_HORSE_NAME);

	if ( pkAff != NULL )
	{
		if ( pChar->GetQuestFlag("horse_name.valid_till") < get_global_time() )
		{
			pChar->HorseSummon(false, true);
			pChar->RemoveAffect(pkAff);
			UpdateHorseName(pChar->GetPlayerID(), "", true);
			pChar->HorseSummon(true, true);
		}
		else
		{
			++(pkAff->lDuration);
		}
	}
}

#ifdef ENABLE_NEW_PET_SYSTEM
const char* CHorseNameManager::GetPetName(DWORD dwPlayerID)
{
	std::map<DWORD, std::string>::iterator iter;

	iter = m_mapPetNames.find(dwPlayerID);

	if (iter != m_mapPetNames.end())
	{
		return iter->second.c_str();
	}
	else
	{
		return NULL;
	}
}

void CHorseNameManager::UpdatePetName(DWORD dwPlayerID, const char* szPetName, bool broadcast)
{
	if (szPetName == NULL)
	{
		sys_err("PET_NAME: NULL NAME (%u)", dwPlayerID);
		szPetName = "";
	}

	sys_log(0, "PET_NAME: update %u %s", dwPlayerID, szPetName);

	m_mapPetNames[dwPlayerID] = szPetName;

	if (broadcast == true)
	{
		BroadcastPetName(dwPlayerID, szPetName);
	}

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(dwPlayerID);
	if (ch)
	{
		ch->SummonPet();
		ch->SendPetInfo();
	}
}

void CHorseNameManager::BroadcastPetName(DWORD dwPlayerID, const char* szPetName)
{
	TPacketUpdateHorseName packet;
	packet.dwPlayerID = dwPlayerID;
	strlcpy(packet.szHorseName, szPetName, sizeof(packet.szHorseName));

	db_clientdesc->DBPacket(HEADER_GD_UPDATE_PET_NAME, 0, &packet, sizeof(TPacketUpdateHorseName));
}
#endif

//martysama0134's ceqyqttoaf71vasf9t71218
