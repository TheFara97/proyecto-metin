#include "stdafx.h"

#ifdef ENABLE_SWITCHBOT
#include "switchbot.h"
#include "desc.h"
#include "item.h"
#include "item_manager.h"
#include "char_manager.h"
#include "buffer_manager.h"
#include "char.h"
#include "config.h"
#include "p2p.h"
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
#include "locale_item_manager.h"
#endif
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
#include "AchievementTitle.h"
#endif
#ifdef _ENABLE_BATTLEPASS_
#include "BattlePassManager.h"
#endif

bool ValidPosition(DWORD wCell)
{
	return wCell < SWITCHBOT_SLOT_COUNT;
}

const float c_fSpeed = 0.1f;

bool SwitchbotHelper::IsValidItem(LPITEM pkItem)
{
	if (!pkItem)
	{
		return false;
	}

	switch (pkItem->GetType())
	{
		case ITEM_WEAPON:
			return true;

		case ITEM_ARMOR:
			switch (pkItem->GetSubType())
			{
			case ARMOR_BODY:
			case ARMOR_HEAD:
			case ARMOR_SHIELD:
			case ARMOR_WRIST:
			case ARMOR_FOOTS:
			case ARMOR_NECK:
			case ARMOR_EAR:
			case ARMOR_GLOVE:
				return true;
			}
		
		case ITEM_BELT:
			return true;

		default:
			return false;
	}
}


CSwitchbot::CSwitchbot()
{
	m_pkSwitchEvent = nullptr;
	m_table = {};
	m_isWarping = false;
}

CSwitchbot::~CSwitchbot()
{
	if (m_pkSwitchEvent)
	{
		event_cancel(&m_pkSwitchEvent);
	}
	
	m_pkSwitchEvent = nullptr;
	m_table = {};
	m_isWarping = false;
}

void CSwitchbot::RegisterItem(WORD wCell, DWORD item_id)
{
	if (!ValidPosition(wCell))
	{
		return;
	}

	m_table.items[wCell] = item_id;
}

void CSwitchbot::UnregisterItem(WORD wCell)
{
	if (!ValidPosition(wCell))
	{
		return;
	}

	m_table.items[wCell] = 0;
	m_table.active[wCell] = false;
	m_table.finished[wCell] = false;
	memset(&m_table.alternatives[wCell], 0, sizeof(m_table.alternatives[wCell]));
}

void CSwitchbot::SetAttributes(BYTE slot, const std::vector<TSwitchbotAttributeAlternativeTable> vec_alternatives)
{
	if (!ValidPosition(slot))
		return;

	for (BYTE alternative = 0; alternative < SWITCHBOT_ALTERNATIVE_COUNT; ++alternative)
	{
		for (BYTE attrIdx = 0; attrIdx < MAX_NORM_ATTR_NUM; ++attrIdx)
		{
			m_table.alternatives[slot][alternative].attributes[attrIdx].bType = vec_alternatives[alternative].attributes[attrIdx].bType;
			m_table.alternatives[slot][alternative].attributes[attrIdx].sValue = vec_alternatives[alternative].attributes[attrIdx].sValue;
		}
	}
}

void CSwitchbot::SetActive(BYTE slot, bool active)
{
	if (!ValidPosition(slot))
		return;
	m_table.active[slot] = active;
	m_table.finished[slot] = false;
	LPITEM pkItem = ITEM_MANAGER::Instance().Find(m_table.items[slot]);
	if (pkItem)
		pkItem->SetUpdateStatus(active);

	LPCHARACTER owner = pkItem->GetOwner();
}

EVENTINFO(TSwitchbotEventInfo)
{
	CSwitchbot* pkSwitchbot;
	TSwitchbotEventInfo() : pkSwitchbot(nullptr)
	{}
};

EVENTFUNC(switchbot_event)
{
	TSwitchbotEventInfo* info = dynamic_cast<TSwitchbotEventInfo*>(event->info);

	if (info == NULL)
	{
		sys_err("switchbot_event> <Factor> Info Null pointer");
		return 0;
	}

	if (!info->pkSwitchbot)
	{
		sys_err("switchbot_event> <Factor> Switchbot Null pointer");
		return 0;
	}

	info->pkSwitchbot->SwitchItems();

	return PASSES_PER_SEC(c_fSpeed);
}

void CSwitchbot::Start()
{
	TSwitchbotEventInfo* info = AllocEventInfo<TSwitchbotEventInfo>();
	info->pkSwitchbot = this;

	m_pkSwitchEvent = event_create(switchbot_event, info, c_fSpeed);

	CSwitchbotManager::Instance().SendSwitchbotUpdate(m_table.player_id);
}

void CSwitchbot::Stop()
{
	if (m_pkSwitchEvent)
	{
		event_cancel(&m_pkSwitchEvent);
		m_pkSwitchEvent = nullptr;
	}

	memset(&m_table.active, 0, sizeof(m_table.active));

	CSwitchbotManager::Instance().SendSwitchbotUpdate(m_table.player_id);
	for (BYTE bSlot = 0; bSlot < SWITCHBOT_SLOT_COUNT; ++bSlot)
	{
		LPITEM pkItem = ITEM_MANAGER::Instance().Find(m_table.items[bSlot]);
		if (!pkItem)
			continue;

		pkItem->SetUpdateStatus(false);
		LPCHARACTER pkOwner = pkItem->GetOwner();
		if (!pkOwner)
			return;
		
		SendItemUpdate(pkOwner, bSlot, pkItem);
	}
}

void CSwitchbot::SendSingleUpdate(BYTE slot)
{
	LPITEM pkItem = ITEM_MANAGER::Instance().Find(m_table.items[slot]);
	if (!pkItem)
		return;

	LPCHARACTER pkOwner = pkItem->GetOwner();
	if (!pkOwner)
		return;

	SendItemUpdate(pkOwner, slot, pkItem);
	CSwitchbotManager::Instance().SendSwitchbotUpdate(m_table.player_id);
}

void CSwitchbot::Pause()
{
	if (m_pkSwitchEvent)
	{
		event_cancel(&m_pkSwitchEvent);
		m_pkSwitchEvent = nullptr;
	}
	for (BYTE bSlot = 0; bSlot < SWITCHBOT_SLOT_COUNT; ++bSlot)
	{
		LPITEM pkItem = ITEM_MANAGER::Instance().Find(m_table.items[bSlot]);
		if (!pkItem)
			continue;
		LPCHARACTER pkOwner = pkItem->GetOwner();
		if (!pkOwner)
			return;
		SendItemUpdate(pkOwner, bSlot, pkItem);
	}
}

bool CSwitchbot::IsActive(BYTE slot)
{
	if (!ValidPosition(slot))
	{
		return false;
	}

	return m_table.active[slot];
}

bool CSwitchbot::HasActiveSlots()
{
	for (const auto& it : m_table.active)
	{
		if (it)
		{
			return true;
		}
	}

	return false;
}

bool CSwitchbot::IsSwitching()
{
	return m_pkSwitchEvent != nullptr;
}

void CSwitchbot::SwitchItems()
{
	for (BYTE bSlot = 0; bSlot < SWITCHBOT_SLOT_COUNT; ++bSlot)
	{
		if (!m_table.active[bSlot])
		{
			continue;
		}

		m_table.finished[bSlot] = false;

		const DWORD item_id = m_table.items[bSlot];

		LPITEM pkItem = ITEM_MANAGER::Instance().Find(item_id);
		if (!pkItem)
		{
			continue;
		}

		LPCHARACTER pkOwner = pkItem->GetOwner();
		if (!pkOwner)
		{
			return;
		}

		if (CheckItem(pkItem, bSlot))
		{
			LPDESC desc = pkOwner->GetDesc();
			if (desc)
			{
				char buf[255];
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
				const std::string& localName = CLocaleItemManager::instance().Find(pkItem->GetVnum(), pkOwner->GetLanguage());
				const char* itemName = localName.empty() ? pkItem->GetName() : localName.c_str();
#else
				const char* itemName = pkItem->GetName();
#endif
				int len = snprintf(buf, sizeof(buf), "[LS;10150;%s;%d]", itemName, bSlot + 1);

				TPacketGCWhisper pack;
				pack.bHeader = HEADER_GC_WHISPER;
				pack.bType = WHISPER_TYPE_SYSTEM;
				pack.wSize = sizeof(TPacketGCWhisper) + len;
				strlcpy(pack.szNameFrom, "[Switchbot]", sizeof(pack.szNameFrom));
				pkOwner->GetDesc()->BufferedPacket(&pack, sizeof(pack));
				pkOwner->GetDesc()->Packet(buf, len);
				SendItemUpdate(pkOwner, bSlot, pkItem);
			}

			SetActive(bSlot, false);

			m_table.finished[bSlot] = true;

			if (!HasActiveSlots())
			{
				Stop();
			}
			else
			{
				CSwitchbotManager::Instance().SendSwitchbotUpdate(m_table.player_id);
			}
		}
		else
		{
			bool stop = false;
			if (SWITCHBOT_PRICE_TYPE == 1)
			{

				for (const auto& itemVnum : c_arSwitchingItems)
				{

					//CHECK_LIMITED_ITEM START
					if (itemVnum == 71151 || itemVnum == 76023)
					{
						if ((pkItem->GetType() == ITEM_WEAPON) || (pkItem->GetType() == ITEM_ARMOR && pkItem->GetSubType() == ARMOR_BODY))
						{
							bool bCanUse = true;
							for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
							{
								if (pkItem->GetLimitType(i) == LIMIT_LEVEL && pkItem->GetLimitValue(i) > 40)
								{
									bCanUse = false;
									break;
								}
							}
							if (false == bCanUse)
							{
								continue;
							}
						}
						else
						{
							continue;
						}
					}
					//CHECK_LIMITED_ITEM END
					
					if (pkOwner->CountSpecifyItem(itemVnum) < SWITCHBOT_PRICE_AMOUNT)
					{
						stop = true;
					}
				}
			}
			else if (SWITCHBOT_PRICE_TYPE == 2)
			{
				if (pkOwner->GetGold() < SWITCHBOT_PRICE_AMOUNT) {
					stop = true;
				}
#ifdef ENABLE_BELT_ATTRIBUTES
				if (pkItem->GetType() == ITEM_BELT && pkOwner->CountSpecifyItem(SWITCHBOT_ZMIANKA_VNUM) <= 0) {
					stop = true;
				}
#endif
				if (pkItem->GetType() == ITEM_ARMOR && pkItem->GetSubType() == ARMOR_GLOVE && pkOwner->CountSpecifyItem(SWITCHBOT_ZMIANKA_GLOVE_VNUM) <= 0) {
					stop = true;
				}
			}

			if (pkItem->GetType() == ITEM_ARMOR && pkItem->GetSubType() == ARMOR_GLOVE && pkItem->GetSocket(0) != 0)
			{
				Stop();
				pkOwner->ChatPacket(CHAT_TYPE_INFO, "[LS;2256]");
				return;
			}

			if (stop)
			{
				Stop();

				if (SWITCHBOT_PRICE_TYPE == 1)
				{
					pkOwner->ChatPacket(CHAT_TYPE_INFO, "[LS;2342]");
				}
				else
				{
#ifdef ENABLE_BELT_ATTRIBUTES
					if (pkItem->GetType() == ITEM_BELT && pkOwner->CountSpecifyItem(SWITCHBOT_ZMIANKA_VNUM) <= 0)
						pkOwner->ChatPacket(CHAT_TYPE_INFO, "[LS;2342]");
					else
						pkOwner->ChatPacket(CHAT_TYPE_INFO, "[LS;2343]");

					if (pkItem->GetType() == ITEM_ARMOR && pkItem->GetSubType() == ARMOR_GLOVE && pkOwner->CountSpecifyItem(SWITCHBOT_ZMIANKA_GLOVE_VNUM) <= 0)
						pkOwner->ChatPacket(CHAT_TYPE_INFO, "[LS;2342]");
					else
						pkOwner->ChatPacket(CHAT_TYPE_INFO, "[LS;2343]");
#else
					pkOwner->ChatPacket(CHAT_TYPE_INFO, "[LS;2343]");
#endif
				}

				return;
			}
			

#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
			const int enchantUsed = pkOwner->GetQuestFlag("AchievementTitle.enchant_used") + 1;
			pkOwner->SetQuestFlag("AchievementTitle.enchant_used", enchantUsed);
			
			CTitleAchievementTitle::instance().CheckAndUnlockTitles(pkOwner);
			
			// Send progress update to client
			pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "title_progress_enchant_used %d", enchantUsed);
#endif

			if (SWITCHBOT_PRICE_TYPE == 1)
			{
				for (const auto& itemVnum : c_arSwitchingItems)
				{
					LPITEM pkItem = pkOwner->FindSpecifyItem(itemVnum);
					if (pkItem)
					{
#ifdef _ENABLE_BATTLEPASS_
						BattlePassManager::Instance().Notify(MISSION_TYPE_SWITCH_ITEMS, pkOwner, pkItem->GetVnum(), 1);
#endif
						// pkItem->SetCount(pkItem->GetCount() - SWITCHBOT_PRICE_AMOUNT);
						break;
					}
				}
	
			}
			else if (SWITCHBOT_PRICE_TYPE == 2)
			{
				pkOwner->PointChange(POINT_GOLD, -SWITCHBOT_PRICE_AMOUNT, false);

#ifdef ENABLE_BELT_ATTRIBUTES
				LPITEM item = pkOwner->FindSpecifyItem(SWITCHBOT_ZMIANKA_VNUM);
				if (pkItem->GetType() == ITEM_BELT && item)
				{
					item->SetCount(item->GetCount() - 1);
				}

				LPITEM item2 = pkOwner->FindSpecifyItem(SWITCHBOT_ZMIANKA_GLOVE_VNUM);
				if (pkItem->GetType() == ITEM_ARMOR && pkItem->GetSubType() == ARMOR_GLOVE && item2)
				{
					item2->SetCount(item2->GetCount() - 1);
				}
#endif
			}

#ifdef ENABLE_BELT_ATTRIBUTES
			if (pkItem->GetType() == ITEM_BELT) {
				pkItem->SetSkipUpdatePacket(true);
#ifdef _ENABLE_BATTLEPASS_
				BattlePassManager::Instance().Notify(MISSION_TYPE_SWITCH_ITEMS, pkOwner, pkItem->GetVnum(), 1);
#endif
				pkItem->ChangeBeltAttribute();
				pkItem->SetSkipUpdatePacket(false);
			} else if (pkItem->GetType() == ITEM_ARMOR && pkItem->GetSubType() == ARMOR_GLOVE) {
				pkItem->SetSkipUpdatePacket(true);
#ifdef _ENABLE_BATTLEPASS_
				BattlePassManager::Instance().Notify(MISSION_TYPE_SWITCH_ITEMS, pkOwner, pkItem->GetVnum(), 1);
#endif
				pkItem->ChangeGloveAttribute();
				pkItem->SetSkipUpdatePacket(false);
			} else {
				pkItem->SetSkipUpdatePacket(true);
#ifdef _ENABLE_BATTLEPASS_
				BattlePassManager::Instance().Notify(MISSION_TYPE_SWITCH_ITEMS, pkOwner, pkItem->GetVnum(), 1);
#endif
				pkItem->ChangeAttribute();
				pkItem->SetSkipUpdatePacket(false);
			}
#else

			pkItem->SetSkipUpdatePacket(true);

			pkItem->ChangeAttribute();
			
			pkItem->SetSkipUpdatePacket(false);

#endif
		}
	}
}

bool CSwitchbot::CheckItem(LPITEM pkItem, BYTE slot)
{
	if (!ValidPosition(slot))
	{
		return false;
	}

	if (!pkItem)
	{
		return false;
	}

	bool checked = 0;

	for (const auto& alternative : m_table.alternatives[slot])
	{
		if (!alternative.IsConfigured())
		{
			continue;
		}

		BYTE configuredAttrCount = 0;
		BYTE correctAttrCount = 0;

		for (const auto& destAttr : alternative.attributes)
		{
			if (!destAttr.bType || !destAttr.sValue)
			{
				continue;
			}

			++configuredAttrCount;

			for (BYTE attrIdx = 0; attrIdx < MAX_NORM_ATTR_NUM; ++attrIdx)
			{
				const TPlayerItemAttribute& curAttr = pkItem->GetAttribute(attrIdx);

				if (curAttr.bType != destAttr.bType || curAttr.sValue < destAttr.sValue)
				{
					continue;
				}

				++correctAttrCount;
				break;
			}
		}

		checked = true;

		if (configuredAttrCount == correctAttrCount)
		{
			return true;
		}
	}

	if (!checked)
	{
		return true;
	}

	return false;
}

void CSwitchbot::SendItemUpdate(LPCHARACTER ch, BYTE slot, LPITEM item)
{
	LPDESC desc = ch->GetDesc();
	if (!desc)
	{
		return;
	}

	TPacketGCSwitchbot pack;
	pack.header = HEADER_GC_SWITCHBOT;
	pack.subheader = SUBHEADER_GC_SWITCHBOT_UPDATE_ITEM;
	pack.size = sizeof(TPacketGCSwitchbot) + sizeof(TSwitchbotUpdateItem);

	TSwitchbotUpdateItem update = {};
	update.slot = slot;
	update.vnum = item->GetVnum();
	update.count = item->GetCount();

	thecore_memcpy(update.alSockets, item->GetSockets(), sizeof(update.alSockets));
	thecore_memcpy(update.aAttr, item->GetAttributes(), sizeof(update.aAttr));

	desc->BufferedPacket(&pack, sizeof(pack));
	desc->Packet(&update, sizeof(TSwitchbotUpdateItem));
}

void CSwitchbotManager::RegisterItem(DWORD player_id, DWORD item_id, WORD wCell)
{
	CSwitchbot* pkSwitchbot = FindSwitchbot(player_id);
	if (!pkSwitchbot)
	{
		pkSwitchbot = new CSwitchbot();
		pkSwitchbot->SetPlayerId(player_id);
		m_map_Switchbots.insert(std::make_pair(player_id, pkSwitchbot));
	}

	if (pkSwitchbot->IsWarping())
	{
		return;
	}

	pkSwitchbot->RegisterItem(wCell, item_id);
	SendSwitchbotUpdate(player_id);
}

void CSwitchbotManager::UnregisterItem(DWORD player_id, WORD wCell)
{
	CSwitchbot* pkSwitchbot = FindSwitchbot(player_id);
	if (!pkSwitchbot)
	{
		return;
	}

	if (pkSwitchbot->IsWarping())
	{
		return;
	}

	pkSwitchbot->UnregisterItem(wCell);
	SendSwitchbotUpdate(player_id);
}

void CSwitchbotManager::Start(DWORD player_id, BYTE slot, std::vector<TSwitchbotAttributeAlternativeTable> vec_alternatives)
{
	if (!ValidPosition(slot))
	{
		return;
	}

	CSwitchbot* pkSwitchbot = FindSwitchbot(player_id);
	if (!pkSwitchbot)
	{
		sys_err("No Switchbot found for player_id %d slot %d", player_id, slot);
		return;
	}

	if (pkSwitchbot->IsActive(slot))
	{
		sys_err("Switchbot slot %d already running for player_id %d", slot, player_id);
		return;
	}

	pkSwitchbot->SetActive(slot, true);
	pkSwitchbot->SetAttributes(slot, vec_alternatives);

	if (pkSwitchbot->HasActiveSlots() && !pkSwitchbot->IsSwitching())
	{
		pkSwitchbot->Start();
	}
	else
	{
		SendSwitchbotUpdate(player_id);
	}
}

void CSwitchbotManager::Stop(DWORD player_id, BYTE slot)
{
	if (!ValidPosition(slot))
	{
		return;
	}

	CSwitchbot* pkSwitchbot = FindSwitchbot(player_id);
	if (!pkSwitchbot)
	{
		sys_err("No Switchbot found for player_id %d slot %d", player_id, slot);
		return;
	}

	if (!pkSwitchbot->IsActive(slot))
	{
		sys_err("Switchbot slot %d is not running for player_id %d", slot, player_id);
		return;
	}

	pkSwitchbot->SetActive(slot, false);

	if (!pkSwitchbot->HasActiveSlots() && pkSwitchbot->IsSwitching())
	{
		pkSwitchbot->Stop();
	}
	else
	{
		SendSwitchbotUpdate(player_id);
	}
}

bool CSwitchbotManager::IsActive(DWORD player_id, BYTE slot)
{
	if (!ValidPosition(slot))
	{
		return false;
	}

	CSwitchbot* pkSwitchbot = FindSwitchbot(player_id);
	if (!pkSwitchbot)
	{
		return false;
	}

	return pkSwitchbot->IsActive(slot);
}

bool CSwitchbotManager::IsWarping(DWORD player_id)
{
	CSwitchbot* pkSwitchbot = FindSwitchbot(player_id);
	if (!pkSwitchbot)
	{
		return false;
	}

	return pkSwitchbot->IsWarping();
}

void CSwitchbotManager::SetIsWarping(DWORD player_id, bool warping)
{
	CSwitchbot* pkSwitchbot = FindSwitchbot(player_id);
	if (!pkSwitchbot)
	{
		return;
	}

	pkSwitchbot->SetIsWarping(warping);
}

CSwitchbot* CSwitchbotManager::FindSwitchbot(DWORD player_id)
{
	const auto& it = m_map_Switchbots.find(player_id);
	if (it == m_map_Switchbots.end())
	{
		return nullptr;
	}
	return it->second.get();
}

void CSwitchbotManager::P2PSendSwitchbot(DWORD player_id, WORD wTargetPort)
{
	CSwitchbot* pkSwitchbot = FindSwitchbot(player_id);
	if (!pkSwitchbot)
	{
		sys_err("No switchbot found to transfer. (pid %d source_port %d target_port %d)", player_id, mother_port, wTargetPort);
		return;
	}

	pkSwitchbot->Pause();

	TPacketGGSwitchbot pack;
	pack.wPort = wTargetPort;
	pack.table = pkSwitchbot->GetTable();

	m_map_Switchbots.erase(player_id);

	P2P_MANAGER::Instance().Send(&pack, sizeof(pack));
}

void CSwitchbotManager::P2PReceiveSwitchbot(TSwitchbotTable table)
{
	CSwitchbot* pkSwitchbot = FindSwitchbot(table.player_id);
	if (!pkSwitchbot)
	{
		pkSwitchbot = new CSwitchbot();
		m_map_Switchbots.insert(std::make_pair(table.player_id, pkSwitchbot));
	}

	pkSwitchbot->SetTable(table);
}

void CSwitchbotManager::SendItemAttributeInformations(LPCHARACTER ch)
{
	if (!ch)
	{
		return;
	}

	LPDESC desc = ch->GetDesc();
	if (!desc)
	{
		return;
	}

	TPacketGCSwitchbot pack;
	pack.header = HEADER_GC_SWITCHBOT;
	pack.subheader = SUBHEADER_GC_SWITCHBOT_SEND_ATTRIBUTE_INFORMATION;
	pack.size = sizeof(TPacketGCSwitchbot);

	TEMP_BUFFER buf;
	for (BYTE bAttributeSet = 0; bAttributeSet < ATTRIBUTE_SET_MAX_NUM; ++bAttributeSet)
	{
		for (int iApplyNum = 0; iApplyNum < MAX_APPLY_NUM; ++iApplyNum)
		{
			const TItemAttrTable& r = g_map_itemAttr[iApplyNum];
			BYTE max = r.bMaxLevelBySet[bAttributeSet];
			if (max > 0)
			{
				TSwitchbottAttributeTable table = {};
				table.attribute_set = bAttributeSet;
				table.apply_num = iApplyNum;
				table.max_value = r.lValues[max-1];

				buf.write(&table, sizeof(table));
			}
#ifdef ENABLE_BELT_ATTRIBUTES
			const TItemAttrTable& r1 = g_map_beltAttr[iApplyNum];
			BYTE max1 = r1.bMaxLevelBySet[bAttributeSet];
			if (max1 > 0)
			{
				TSwitchbottAttributeTable table = {};
				table.attribute_set = bAttributeSet;
				table.apply_num = iApplyNum;
				table.max_value = r1.lValues[max1-1];

				buf.write(&table, sizeof(table));
			}
#endif
			const TItemAttrTable& r2 = g_map_gloveAttr[iApplyNum];
			BYTE max2 = r2.bMaxLevelBySet[bAttributeSet];
			if (max2 > 0)
			{
				TSwitchbottAttributeTable table = {};
				table.attribute_set = bAttributeSet;
				table.apply_num = iApplyNum;
				table.max_value = r2.lValues[max2-1];

				buf.write(&table, sizeof(table));
			}
		}
	}

	if (buf.size())
	{
		pack.size += buf.size();
		desc->BufferedPacket(&pack, sizeof(pack));
		desc->Packet(buf.read_peek(), buf.size());
	}
	else
	{
		desc->Packet(&pack, sizeof(pack));
	}
}

void CSwitchbotManager::SendSwitchbotUpdate(DWORD player_id)
{
	CSwitchbot* pkSwitchbot = FindSwitchbot(player_id);
	if (!pkSwitchbot)
	{
		return;
	}

	LPCHARACTER ch = CHARACTER_MANAGER::Instance().FindByPID(player_id);
	if (!ch)
	{
		return;
	}

	LPDESC desc = ch->GetDesc();
	if (!desc)
	{
		return;
	}

	TSwitchbotTable table = pkSwitchbot->GetTable();

	TPacketGCSwitchbot pack;
	pack.header = HEADER_GC_SWITCHBOT;
	pack.subheader = SUBHEADER_GC_SWITCHBOT_UPDATE;
	pack.size = sizeof(TPacketGCSwitchbot) + sizeof(TSwitchbotTable);

	desc->BufferedPacket(&pack, sizeof(pack));
	desc->Packet(&table, sizeof(table));
}

void CSwitchbotManager::EnterGame(LPCHARACTER ch)
{
	SendItemAttributeInformations(ch);
	SetIsWarping(ch->GetPlayerID(), false);
	SendSwitchbotUpdate(ch->GetPlayerID());
	
	CSwitchbot* pkSwitchbot = FindSwitchbot(ch->GetPlayerID());
	if (pkSwitchbot && pkSwitchbot->HasActiveSlots() && !pkSwitchbot->IsSwitching())
	{
		pkSwitchbot->Start();
	}
}

#endif
