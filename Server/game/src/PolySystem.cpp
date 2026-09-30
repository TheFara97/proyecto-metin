#include "stdafx.h"

#ifdef __ENABLE_POLYMORPH_SYSTEM__
#include "char.h"
#include "packet.h"
#include "desc.h"
#include "buffer_manager.h"
#include "item.h"
#include "item_manager.h"
#include "questmanager.h"
#include "PolySystem.h"
#ifdef BATTLE_PASS
#include "CBattlePass.h"
#endif


bool CPolymorphMgr::LoadPolyStage(TPolymorphStage* pStage, uint16_t wSize)
{
	if (!m_StageMap.empty())
		m_StageMap.clear();

	for (uint16_t i = 0; i < wSize; i++, pStage++)
	{
		m_StageMap.emplace(pStage->byStage, *pStage);
	}

	sys_log(0, "CPolymorphMgr::LoadPolyStage -> Size %d", m_StageMap.size());
	return m_StageMap.size();
}

bool CPolymorphMgr::LoadPolySkins(TPolymorphSkin* pSkin, uint16_t wSize)
{
	if (!m_SkinsMap.empty())
		m_SkinsMap.clear();

	for (uint16_t i = 0; i < wSize; i++, pSkin++)
	{
		m_SkinsMap.emplace(pSkin->byIndex, *pSkin);
	}

	sys_log(0, "CPolymorphMgr::LoadPolySkins -> Size %d", m_SkinsMap.size());
	return m_SkinsMap.size();
}

bool CPolymorphMgr::IsUnlockedSkin(CHARACTER* pChar, uint8_t bySkinIndex)
{
	if (!pChar || !pChar->GetDesc())
		return false;

	quest::PC* pPC = quest::CQuestManager::instance().GetPC(pChar->GetPlayerID()); // Player is not loaded in quests yet...
	if (!pPC)
		return false;

	if (!pPC->IsLoaded())
		return false;
	
	if (bySkinIndex == 0) // 0 - index is an deafult skin so it's always unlocked.
		return true;

	const std::string strSkinFlag = "polyskin.skin" + std::to_string(bySkinIndex);
	if (!pPC->GetFlag(strSkinFlag)) // Skin is not unlocked
		return false;

	return true;
}

bool CPolymorphMgr::UnlockSkin(CHARACTER* pChar, uint8_t bySkinIndex)
{
	if (!pChar || !pChar->GetDesc())
		return false;

	if (!pChar->GetDesc()->IsPhase(PHASE_GAME))
		return false;

	if (IsUnlockedSkin(pChar, bySkinIndex)) // It's already unlocked...
	{
		pChar->ChatPacket(CHAT_TYPE_INFO, "[LS;2372]");
		return false;
	}

	const std::string strSkinFlag = "polyskin.skin" + std::to_string(bySkinIndex); 
	pChar->SetQuestFlag(strSkinFlag, 1); // We're unlocking skin we're happy so we're returning happy true.
	pChar->ChatPacket(CHAT_TYPE_INFO, "[LS;2373]");
	SendUnlockSkin(pChar, bySkinIndex);
	return true;
}

void CPolymorphMgr::ChangeSkin(CHARACTER* pChar, uint8_t bySkinIndex)
{
	if (!pChar || !pChar->GetDesc())
		return;

	if (!pChar->GetDesc()->IsPhase(PHASE_GAME))
		return;

	const auto fIter = m_SkinsMap.find(bySkinIndex);
	if (fIter == m_SkinsMap.end())
		return;

	if (pChar->IsPolymorphed())
	{
		pChar->ChatPacket(CHAT_TYPE_INFO, "[LS;2374]");
		return;
	}

	if (pChar->GetPolySkin() == bySkinIndex)
	{
		pChar->ChatPacket(CHAT_TYPE_INFO, "[LS;2375]");
		return;
	}

	if (!IsUnlockedSkin(pChar, bySkinIndex))
	{
		pChar->ChatPacket(CHAT_TYPE_INFO, "[LS;2376]");
		return;
	}

	const auto fSkin = fIter->second;
	pChar->SetPolySkin(fSkin.byIndex);
	SendChangeSkin(pChar);
}

uint32_t CPolymorphMgr::GetSkinVnum(uint8_t bySkinIndex) const
{
	auto fIter = m_SkinsMap.find(bySkinIndex);
	if (fIter == m_SkinsMap.end())
		return 0;

	return fIter->second.dwSkinVnum;
}

void CPolymorphMgr::BuyPolymorph(CHARACTER* pChar, uint8_t bStage)
{
	if (!pChar || !pChar->GetDesc())
		return;

	if (!pChar->IsPolyShopping())
	{
		pChar->ChatPacket(CHAT_TYPE_INFO, "[LS;2377]");
		return;
	}

	if (!pChar->CanPolyShop())
	{
		pChar->ChatPacket(CHAT_TYPE_INFO, "[LS;2378]");
		return;
	}

	// Check another windows to make sure it's safe.
	const auto fIter = m_StageMap.find(bStage);
	if (fIter == m_StageMap.end())
		return;

	const auto fStage = fIter->second;

	// Check 1 Space in Inventory
	if (pChar->GetEmptyInventory(1) == -1)
	{
		pChar->ChatPacket(CHAT_TYPE_INFO, "[LS;1130]");
		return;
	}

	// Check Items
	for (auto needItem : fStage.tRequired)
	{
		if (pChar->CountSpecifyItem(needItem.dwVnum) < needItem.wCount)
		{
			pChar->ChatPacket(CHAT_TYPE_INFO, "[LS;2379]");
			return;
		}
	}

	// Remove Items
	for (auto needItem : fStage.tRequired)
	{
		pChar->RemoveSpecifyItem(needItem.dwVnum, needItem.wCount);
	}

	// Give Poly Item
	CItem* pItem = ITEM_MANAGER::instance().CreateItem(fStage.dwVnum);
	if (!pItem)
	{
		sys_err("CPolymorphMgr::BuyPolymorph -> Cannot create item with vnum: %d", fStage.dwVnum);
		return;
	}

	pItem->SetSocket(0, fStage.dwMonster);
	pItem->SetSocket(1, fStage.tApplies[0].bType); // We wanna set socket to make sure it wont stack with other poly items.
	pItem->SetSocket(2, fStage.tApplies[1].bType); // We wanna set socket to make sure it wont stack with other poly items.

	for (uint8_t i = 0; i < MAX_POLY_BONUS; i++)
	{
		if (fStage.tApplies[i].bType && fStage.tApplies[i].lValue)
			pItem->SetForceAttribute(i, fStage.tApplies[i].bType, fStage.tApplies[i].lValue);
	}

#ifdef BATTLE_PASS
	pChar->GetBattlePass()->IncrementWeeklyMission(CBattlePass::WEEKLY_MISSION_MARBLE_MAKE);
#endif

	pChar->AutoGiveItem(pItem);
	pChar->SetMyShopTime();
	// pChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Poly System> You bought item successfull"));
}


int32_t CPolymorphMgr::ReceivePacket(CHARACTER* pChar, const char* c_pData, size_t uiBytes)
{
	const TPacketCGPolySystem* p = reinterpret_cast<const TPacketCGPolySystem*>(c_pData);
	if (uiBytes < sizeof(TPacketCGPolySystem))
	{
		sys_log(0, "CPolymorphMgr::ReceivePacket -> Packet is too small.");
		return -1;
	}

	const char* c_pRestData = c_pData + sizeof(TPacketCGPolySystem);
	uiBytes -= sizeof(TPacketCGPolySystem);

	switch (p->bySubheader)
	{
		case SUBHEADER_POLY_OPEN:
		{
			if (pChar->IsDead())
			{
				pChar->ChatPacket(CHAT_TYPE_INFO, "[LS;2378]");
				return 0;
			}

			if (!pChar->CanPolyShop())
			{
				pChar->ChatPacket(CHAT_TYPE_INFO, "[LS;2378]");
				return 0;
			}

			pChar->SetPolyShopping(true);
			SendOpen(pChar);
		}
		break;

		case SUBHEADER_POLY_CLOSE:
		{
			pChar->SetPolyShopping(false);
		}
		break;

		case SUBHEADER_POLY_BUY:
		{
			if (uiBytes < sizeof(uint8_t))
			{
				sys_log(0, "CPolymorphMgr::ReceivePacket -> SUBHEADER_POLY_SKIN_CHANGE: Wrong packet size");
				return -1;
			}

			const uint8_t subPacket = *reinterpret_cast<const uint8_t*>(c_pRestData);
			BuyPolymorph(pChar, subPacket);
			return sizeof(subPacket);
		}

		case SUBHEADER_POLY_SKIN_CHANGE:
		{
			if (uiBytes < sizeof(uint8_t))
			{
				sys_log(0, "CPolymorphMgr::ReceivePacket -> SUBHEADER_POLY_SKIN_CHANGE: Wrong packet size");
				return -1;
			}

			const uint8_t subPacket = *reinterpret_cast<const uint8_t*>(c_pRestData);
			ChangeSkin(pChar, subPacket);
			return sizeof(subPacket);
		}
	}

	return 0;
}

void CPolymorphMgr::SendUnlockSkin(CHARACTER* pChar, uint8_t bySkinIndex)
{
	if (!pChar || !pChar->GetDesc())
		return;

	if (!pChar->GetDesc()->IsPhase(PHASE_GAME))
		return;

	WORD packetSize = sizeof(TPacketGCPolySystem) + sizeof(BYTE);
	TEMP_BUFFER buffer(packetSize);

	TPacketGCPolySystem pack = { HEADER_GC_POLY_SYSTEM, packetSize, SUBHEADER_POLY_SKIN_UNLOCK };
	buffer.write(&pack, sizeof(pack));
	buffer.write(&bySkinIndex, sizeof(bySkinIndex));

	pChar->GetDesc()->Packet(buffer.read_peek(), buffer.size());
}

void CPolymorphMgr::SendChangeSkin(CHARACTER* pChar)
{
	if (!pChar || !pChar->GetDesc())
		return;

	if (!pChar->GetDesc()->IsPhase(PHASE_GAME))
		return;

	WORD packetSize = sizeof(TPacketGCPolySystem) + sizeof(BYTE);
	TEMP_BUFFER buffer(packetSize);

	TPacketGCPolySystem pack = { HEADER_GC_POLY_SYSTEM, packetSize, SUBHEADER_POLY_SKIN_CHANGE };
	buffer.write(&pack, sizeof(pack));

	BYTE actualSkin = pChar->GetPolySkin();
	buffer.write(&actualSkin, sizeof(actualSkin));

	pChar->GetDesc()->Packet(buffer.read_peek(), buffer.size());
}

void CPolymorphMgr::SendOpen(CHARACTER* pChar)
{
	if (!pChar || !pChar->GetDesc())
		return;

	if (!pChar->GetDesc()->IsPhase(PHASE_GAME))
		return;

	TPacketGCPolySystem pack = { HEADER_GC_POLY_SYSTEM, sizeof(TPacketGCPolySystem), SUBHEADER_POLY_OPEN };
	pChar->GetDesc()->Packet(&pack, sizeof(pack));
}

void CPolymorphMgr::SendClose(CHARACTER* pChar)
{
	if (!pChar || !pChar->GetDesc())
		return;
	
	pChar->SetPolyShopping(false);
	TPacketGCPolySystem pack = { HEADER_GC_POLY_SYSTEM, sizeof(TPacketGCPolySystem), SUBHEADER_POLY_CLOSE };
	pChar->GetDesc()->Packet(&pack, sizeof(pack));
}

void CPolymorphMgr::SendData(CHARACTER* pChar)
{
	if (!pChar || !pChar->GetDesc())
		return;

	if (!pChar->GetDesc()->IsPhase(PHASE_GAME))
		return;

	WORD packetSize = sizeof(TPolymorphStage) * m_StageMap.size() + sizeof(TPolymorphSkin) * m_SkinsMap.size() + sizeof(BYTE) * m_SkinsMap.size() + sizeof(TPacketGCPolySystem) + sizeof(WORD) + sizeof(WORD) + sizeof(BYTE);
	TEMP_BUFFER buff(packetSize);

	TPacketGCPolySystem pack = { HEADER_GC_POLY_SYSTEM, packetSize, SUBHEADER_POLY_DATA };
	buff.write(&pack, sizeof(pack));

	BYTE bActualSkin = pChar->GetPolySkin();
	buff.write(&bActualSkin, sizeof(bActualSkin));

	WORD stageSize = m_StageMap.size();
	buff.write(&stageSize, sizeof(stageSize));

	for (const auto& it : m_StageMap)
	{
		buff.write(&it.second, sizeof(it.second));
	}

	WORD skinsSize = m_SkinsMap.size();
	buff.write(&skinsSize, sizeof(skinsSize));

	for (const auto& it : m_SkinsMap)
	{
		buff.write(&it.second, sizeof(it.second));

		BYTE bHasSkin = static_cast<BYTE>(IsUnlockedSkin(pChar, it.second.byIndex));
		buff.write(&bHasSkin, sizeof(bHasSkin));
	}

	pChar->GetDesc()->Packet(buff.read_peek(), buff.size());
}















#endif
