#include "stdafx.h"
#include "../../libgame/include/grid.h"
#include "constants.h"
#include "utils.h"
#include "config.h"
#include "offline_shop.h"
#include "desc.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "buffer_manager.h"
#include "packet.h"
#include "log.h"
#include "db.h"
#include "questmanager.h"
#include "mob_manager.h"
#include "locale_service.h"
#include "desc_client.h"
#include "group_text_parse_tree.h"
#include <boost/algorithm/string/predicate.hpp>
#include <cctype>
#include "offlineshop_manager.h"
#include "p2p.h"
#include "entity.h"
#include "sectree_manager.h"
#include "offlineshop_config.h"
#include "target.h"

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
extern bool offlineshop_map_allow_find(int mapIndex);

COfflineShopManager::COfflineShopManager()
{
}


COfflineShopManager::~COfflineShopManager()
{
}

struct FFindOfflineShop
{
	const char * szName;

	DWORD dwVID, dwRealOwner;
	FFindOfflineShop(const char * c_szName) : szName(c_szName), dwVID(0), dwRealOwner(0) {};

	void operator()(LPENTITY ent)
	{
		if (!ent)
			return;

		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER ch = (LPCHARACTER)ent;
			if (ch->IsOfflineShopNPC() && !strcmp(szName, ch->GetName()))
			{
				dwVID = ch->GetVID();
				dwRealOwner = ch->GetOfflineShopRealOwner();
				M2_DESTROY_CHARACTER(ch);
			}
		}
	}
};

bool COfflineShopManager::StartShopping(LPCHARACTER pkChr, LPCHARACTER pkChrShopKeeper)
{
	if (pkChr->GetOfflineShopOwner() == pkChrShopKeeper)
		return false;

	if (pkChrShopKeeper->IsPC())
		return false;

	sys_log(0, "OFFLINE_SHOP: START: %s", pkChr->GetName());
	return true;
}

LPOFFLINESHOP COfflineShopManager::CreateOfflineShop(LPCHARACTER npc, DWORD dwOwnerPID)
{
	if(!npc)
		return NULL;
	
	if (FindOfflineShop(npc->GetVID()))
		return NULL;
	
	if(FindMyOfflineShop(dwOwnerPID))
	{
		m_Map_pkOfflineShopByNPC2.erase(dwOwnerPID);
	}

	LPOFFLINESHOP pkOfflineShop = M2_NEW COfflineShop;
	pkOfflineShop->SetOfflineShopNPC(npc);

	m_map_pkOfflineShopByNPC.insert(TShopMap::value_type(npc->GetVID(), pkOfflineShop));
	m_Map_pkOfflineShopByNPC2.insert(TOfflineShopMap::value_type(dwOwnerPID, npc->GetVID()));
	return pkOfflineShop;
}

LPOFFLINESHOP COfflineShopManager::FindOfflineShop(DWORD dwVID)
{
	TShopMap::iterator it = m_map_pkOfflineShopByNPC.find(dwVID);

	if (it == m_map_pkOfflineShopByNPC.end())
		return NULL;

	return it->second;
}

void COfflineShopManager::DestroyOfflineShop(LPCHARACTER ch, DWORD dwVID, bool bDestroyAll)
{
	if (ch == NULL)
		return;
	
	if (g_bOfflineShopMapAllowLimit)
	{
		if (!offlineshop_map_allow_find(ch->GetMapIndex()))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2380]");
			return;
		}
	}

	if (dwVID == 0)
	{
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT mapIndex FROM %soffline_shop_npc WHERE owner_id = %u", get_table_postfix(), ch->GetPlayerID()));

		if (pMsg->Get()->uiNumRows == 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2401]");
			return;			
		}

		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2402]");
	}
	else
	{
		LPCHARACTER npc;
		if (!ch)
			npc = CHARACTER_MANAGER::instance().Find(dwVID);
		else
			npc = CHARACTER_MANAGER::instance().Find(FindMyOfflineShop(ch->GetPlayerID()));

		if (!npc)
		{
			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT mapIndex,channel FROM %soffline_shop_npc WHERE owner_id = %u", get_table_postfix(), ch->GetPlayerID()));

			if (pMsg->Get()->uiNumRows == 0)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2402]");
				return;
			}

			MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);

			int32_t lMapIndex = 0;
			str_to_number(lMapIndex, row[0]);

			BYTE bChannel = 0;
			str_to_number(bChannel, row[1]);

			if (g_bChannel != bChannel)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2402;%d]", bChannel);
				return;
			}

			char szName[CHARACTER_NAME_MAX_LEN + 1];
			snprintf(szName, sizeof(szName), "%s", ch->GetName());
			LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(lMapIndex);
			FFindOfflineShop offlineShop(szName);
			pMap->for_each(offlineShop);

			if (bDestroyAll)
			{
                --g_shopCount[lMapIndex];
				Giveback(ch);
				m_map_pkOfflineShopByNPC.erase(offlineShop.dwVID);
				m_Map_pkOfflineShopByNPC2.erase(offlineShop.dwRealOwner);
				DBManager::instance().DirectQuery("DELETE FROM %soffline_shop_npc WHERE owner_id = %u", get_table_postfix(), offlineShop.dwRealOwner);
				return;
			}
		}

		LPOFFLINESHOP pkOfflineShop;
		if (!ch)
			pkOfflineShop = FindOfflineShop(dwVID);
		else
			pkOfflineShop = FindOfflineShop(FindMyOfflineShop(ch->GetPlayerID()));

		if (!pkOfflineShop)
			return;

		if (npc->GetOfflineShopChannel() != g_bChannel)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2402;%d]", npc->GetOfflineShopChannel());
			return;
		}

		if (bDestroyAll)
		{		
			Giveback(ch);
			pkOfflineShop->Destroy(npc);
		}

		m_map_pkOfflineShopByNPC.erase(npc->GetVID());
		m_Map_pkOfflineShopByNPC2.erase(npc->GetOfflineShopRealOwner());
		M2_DELETE(pkOfflineShop);

        if (npc != NULL)
        {
            --g_shopCount[npc->GetMapIndex()];
        }
	}
}

void COfflineShopManager::Giveback(LPCHARACTER ch)
{
	if (!ch)
		return;

	char szQuery[2048];
	snprintf(szQuery, sizeof(szQuery), "SELECT pos,count,vnum,type,subtype,"
										"socket0,socket1,socket2,socket3,socket4,socket5, attrtype0, attrvalue0, attrtype1, attrvalue1, attrtype2, attrvalue2, attrtype3, attrvalue3, attrtype4, attrvalue4, attrtype5, attrvalue5, attrtype6, attrvalue6 FROM %soffline_shop_item WHERE owner_id = %u", get_table_postfix(), ch->GetPlayerID());
	
	std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
	if (pMsg->Get()->uiNumRows == 0)
	{
		sys_err("COfflineShopManager::GiveBack - There is nothing for this player [%s]", ch->GetName());
		return;
	}

	MYSQL_ROW row;
	while (NULL != (row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
	{
		int cur = 0;
		
		TPlayerItem item;
		str_to_number(item.pos, row[cur++]);
		str_to_number(item.count, row[cur++]);
		str_to_number(item.vnum, row[cur++]);

		TItemTable* proto = ITEM_MANAGER::instance().GetTable(item.vnum);

		str_to_number(proto->bType, row[cur++]);
		str_to_number(proto->bSubType, row[cur++]);
		
		// Set Sockets
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
			str_to_number(item.alSockets[i], row[cur++]);
		// End Of Set Sockets

		// Set Attributes
		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		{
			str_to_number(item.aAttr[i].bType, row[cur++]);
			str_to_number(item.aAttr[i].sValue, row[cur++]);
		}
		// End Of Set Attributes

		LPITEM pItem = ITEM_MANAGER::instance().CreateItem(item.vnum, item.count);
		if (pItem)
		{
			int iEmptyPos = 0;
			
			if (pItem->IsDragonSoul())
				iEmptyPos = ch->GetEmptyDragonSoulInventory(pItem);
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
			else if (pItem->IsSkillBook())
				iEmptyPos = ch->GetEmptySkillBookInventory(pItem->GetSize());
			else if (pItem->IsUpgradeItem())
				iEmptyPos = ch->GetEmptyUpgradeItemsInventory(pItem->GetSize());
			else if (pItem->IsStone())
				iEmptyPos = ch->GetEmptyStoneInventory(pItem->GetSize());
			else if (pItem->IsBox())
				iEmptyPos = ch->GetEmptyBoxInventory(pItem->GetSize());
			else if (pItem->IsEfsun())
				iEmptyPos = ch->GetEmptyEfsunInventory(pItem->GetSize());
			else if (pItem->IsCicek())
				iEmptyPos = ch->GetEmptyCicekInventory(pItem->GetSize());
#endif
			else
				iEmptyPos = ch->GetEmptyInventory(pItem->GetSize());

			if (iEmptyPos < 0)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1130]");
				return;
			}
			
			pItem->SetSockets(item.alSockets);
			pItem->SetAttributes(item.aAttr);
			
			if (pItem->IsDragonSoul())
				pItem->AddToCharacter(ch, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyPos));
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
			else if (pItem->IsSkillBook())
				pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
			else if (pItem->IsUpgradeItem())
				pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
			else if (pItem->IsStone())
				pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
			else if (pItem->IsBox())
				pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
			else if (pItem->IsEfsun())
				pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
			else if (pItem->IsCicek())
				pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
#endif
			else
				pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
			
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2403;%s]", pItem->GetName());
			DBManager::instance().DirectQuery("DELETE FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), ch->GetPlayerID(), item.pos);
		}
	}
}

void COfflineShopManager::Giveback2(LPCHARACTER ch)
{
	if (!ch)
		return;

	char szQuery[2048];
	snprintf(szQuery, sizeof(szQuery), "SELECT pos,count,vnum,type,subtype,"
										"socket0,socket1,socket2,socket3,socket4,socket5, attrtype0, attrvalue0, attrtype1, attrvalue1, attrtype2, attrvalue2, attrtype3, attrvalue3, attrtype4, attrvalue4, attrtype5, attrvalue5, attrtype6, attrvalue6 FROM %soffline_shop_item WHERE owner_id = %u", get_table_postfix(), ch->GetPlayerID());

	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
	
	if (pMsg->Get()->uiNumRows == 0)
		return;
	
	MYSQL_ROW row;
	while (NULL != (row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
	{
		int cur = 0;
		
		TPlayerItem item;
		str_to_number(item.pos, row[cur++]);
		str_to_number(item.count, row[cur++]);
		str_to_number(item.vnum, row[cur++]);

		TItemTable* proto = ITEM_MANAGER::instance().GetTable(item.vnum);

		str_to_number(proto->bType, row[cur++]);
		str_to_number(proto->bSubType, row[cur++]);
		
		// Set Sockets
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
			str_to_number(item.alSockets[i], row[cur++]);
		// End Of Set Sockets

		// Set Attributes
		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		{
			str_to_number(item.aAttr[i].bType, row[cur++]);
			str_to_number(item.aAttr[i].sValue, row[cur++]);
		}
		// End Of Set Attributes

		LPITEM pItem = ITEM_MANAGER::instance().CreateItem(item.vnum, item.count);
		if (pItem)
		{
			int iEmptyPos = 0;
			
			if (pItem->IsDragonSoul())
				iEmptyPos = ch->GetEmptyDragonSoulInventory(pItem);
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
			else if (pItem->IsSkillBook())
				iEmptyPos = ch->GetEmptySkillBookInventory(pItem->GetSize());
			else if (pItem->IsUpgradeItem())
				iEmptyPos = ch->GetEmptyUpgradeItemsInventory(pItem->GetSize());
			else if (pItem->IsStone())
				iEmptyPos = ch->GetEmptyStoneInventory(pItem->GetSize());
			else if (pItem->IsBox())
				iEmptyPos = ch->GetEmptyBoxInventory(pItem->GetSize());
			else if (pItem->IsEfsun())
				iEmptyPos = ch->GetEmptyEfsunInventory(pItem->GetSize());
			else if (pItem->IsCicek())
				iEmptyPos = ch->GetEmptyCicekInventory(pItem->GetSize());
#endif
			else
				iEmptyPos = ch->GetEmptyInventory(pItem->GetSize());

			if (iEmptyPos < 0)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1130]");
				return;
			}
			
			pItem->SetSockets(item.alSockets);
			pItem->SetAttributes(item.aAttr);
			if (pItem->IsDragonSoul())
				pItem->AddToCharacter(ch, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyPos));
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
			else if (pItem->IsSkillBook())
				pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
			else if (pItem->IsUpgradeItem())
				pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
			else if (pItem->IsStone())
				pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
			else if (pItem->IsBox())
				pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
			else if (pItem->IsEfsun())
				pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
			else if (pItem->IsCicek())
				pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
#endif
			else
				pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));

			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2403;%s]", pItem->GetName());
			DBManager::instance().DirectQuery("DELETE FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), ch->GetPlayerID(), item.pos);
		}
	}	
}

void COfflineShopManager::AddItem(LPCHARACTER ch, BYTE bDisplayPos, TItemPos itemPos, int64_t iPrice, int64_t iPrice2)
{
	if (!ch)
		return;

    if (ch->GetExchange() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->IsCubeOpen())
		return;

	if (quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID())->IsRunning() == true)
		return;

	if (quest::CQuestManager::instance().GetEventFlag("block_offline_shop_add_item"))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2404]");
		return;
	}

	if (g_bOfflineShopMapAllowLimit)
	{
		if (!offlineshop_map_allow_find(ch->GetMapIndex()))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2405]");
			return;
		}
	}

	// Fixed bug 6.21.2015
	if (bDisplayPos >= OFFLINE_SHOP_HOST_ITEM_MAX_NUM)
	{
		sys_err("Overflow offline shop slot count [%s]", ch->GetName());
		return;
	}
	// End Of fixed bug 6.21.2015
	
	// Check player has offline shop or not
	std::unique_ptr<SQLMsg> pmsg(DBManager::instance().DirectQuery("SELECT COUNT(*) FROM srv1_player.offline_shop_npc WHERE owner_id = %u", ch->GetPlayerID()));
	MYSQL_ROW row = mysql_fetch_row(pmsg->Get()->pSQLResult);
	
	BYTE bResult = 0;
	str_to_number(bResult, row[0]);
	
	if (!bResult)
	{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2401]");
		return;
	}
	// End Of Check player has offline shop or not
	
	if (!itemPos.IsValidItemPosition())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Item position unavailable! Please try again!");
		return;
	}
	
	LPITEM pkItem = ch->GetItem(itemPos);
	
	if (!pkItem)
		return;
	
	// Check
	const TItemTable * itemTable = pkItem->GetProto();
	if (itemTable && (IS_SET(itemTable->dwAntiFlags, ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_MYSHOP)))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2391]");
		return;
	}

	if (pkItem->isLocked())
		return;

	if (pkItem->IsEquipped())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2406]");
		return;
	}

	if (iPrice < 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2382]");
		return;
	}

	if (iPrice2 < 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2382]");
		return;
	}

	char szColumns[QUERY_MAX_LEN], szValues[QUERY_MAX_LEN];

	int iLen = snprintf(szColumns, sizeof(szColumns), "id,owner_id,pos,count,price_gold,price_cheque,vnum,type,subtype"
	);
	
	int iUpdateLen = snprintf(szValues, sizeof(szValues), 
															"%u,"
															"%u,"
															"%d,"
															"%u,"
															"%lld, "
															"%lld, "
															"%u, "
															"%d, "
															"%d"
															,
															pkItem->GetID(), ch->GetPlayerID(), bDisplayPos, pkItem->GetCount(), iPrice, iPrice2, pkItem->GetVnum(), pkItem->GetType(), pkItem->GetSubType()
	);

	if (g_bOfflineShopSocketMax == 3)
	{
		iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen, ",socket0,socket1,socket2");
		iUpdateLen += snprintf(szValues + iUpdateLen, sizeof(szValues) - iUpdateLen, ",%ld,%ld,%ld", pkItem->GetSocket(0), pkItem->GetSocket(1), pkItem->GetSocket(2));
	}
	else if(g_bOfflineShopSocketMax == 4)
	{
		iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen, ",socket0,socket1,socket2,socket3");
		iUpdateLen += snprintf(szValues + iUpdateLen, sizeof(szValues) - iUpdateLen, ",%ld,%ld,%ld,%ld", pkItem->GetSocket(0), pkItem->GetSocket(1), pkItem->GetSocket(2), pkItem->GetSocket(3));		
	}
	else if(g_bOfflineShopSocketMax == 5)
	{
		iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen, ",socket0,socket1,socket2,socket3,socket4");
		iUpdateLen += snprintf(szValues + iUpdateLen, sizeof(szValues) - iUpdateLen, ",%ld,%ld,%ld,%ld,%ld", pkItem->GetSocket(0), pkItem->GetSocket(1), pkItem->GetSocket(2), pkItem->GetSocket(3), pkItem->GetSocket(4));		
	}
	else if(g_bOfflineShopSocketMax == 6)
	{
		iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen, ",socket0,socket1,socket2,socket3,socket4,socket5");
		iUpdateLen += snprintf(szValues + iUpdateLen, sizeof(szValues) - iUpdateLen, ",%ld,%ld,%ld,%ld,%ld,%ld", pkItem->GetSocket(0), pkItem->GetSocket(1), pkItem->GetSocket(2), pkItem->GetSocket(3), pkItem->GetSocket(4), pkItem->GetSocket(5));		
	}

#ifdef USE_LENTS_SHOULDER_SASH
	iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen, ", attrtype0, attrvalue0, attrtype1, attrvalue1, attrtype2, attrvalue2, attrtype3, attrvalue3, attrtype4, attrvalue4, attrtype5, attrvalue5, attrtype6, attrvalue6, applytype0, applyvalue0, applytype1, applyvalue1, applytype2, applyvalue2, applytype3, applyvalue3, applytype4, applyvalue4, applytype5, applyvalue5, applytype6, applyvalue6, applytype7, applyvalue7");
	iUpdateLen += snprintf(szValues + iUpdateLen, sizeof(szValues) - iUpdateLen, ", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
			pkItem->GetAttributeType(0), pkItem->GetAttributeValue(0),
			pkItem->GetAttributeType(1), pkItem->GetAttributeValue(1),
			pkItem->GetAttributeType(2), pkItem->GetAttributeValue(2),
			pkItem->GetAttributeType(3), pkItem->GetAttributeValue(3),
			pkItem->GetAttributeType(4), pkItem->GetAttributeValue(4),
			pkItem->GetAttributeType(5), pkItem->GetAttributeValue(5),
			pkItem->GetAttributeType(6), pkItem->GetAttributeValue(6),
			pkItem->GetAttributeType(7), pkItem->GetAttributeValue(7),
			pkItem->GetAttributeType(8), pkItem->GetAttributeValue(8),
			pkItem->GetAttributeType(9), pkItem->GetAttributeValue(9),
			pkItem->GetAttributeType(10), pkItem->GetAttributeValue(10),
			pkItem->GetAttributeType(11), pkItem->GetAttributeValue(11),
			pkItem->GetAttributeType(12), pkItem->GetAttributeValue(12),
			pkItem->GetAttributeType(13), pkItem->GetAttributeValue(13),
			pkItem->GetAttributeType(14), pkItem->GetAttributeValue(14));
#else
	iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen, ",attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6");
	iUpdateLen += snprintf(szValues + iUpdateLen, sizeof(szValues) - iUpdateLen, ",%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
		pkItem->GetAttributeType(0), pkItem->GetAttributeValue(0),
		pkItem->GetAttributeType(1), pkItem->GetAttributeValue(1),
		pkItem->GetAttributeType(2), pkItem->GetAttributeValue(2),
		pkItem->GetAttributeType(3), pkItem->GetAttributeValue(3),
		pkItem->GetAttributeType(4), pkItem->GetAttributeValue(4),
		pkItem->GetAttributeType(5), pkItem->GetAttributeValue(5),
		pkItem->GetAttributeType(6), pkItem->GetAttributeValue(6));
#endif	

	LPCHARACTER npc = CHARACTER_MANAGER::instance().Find(FindMyOfflineShop(ch->GetPlayerID()));

	if (npc)
	{
		LPOFFLINESHOP pkOfflineShop = npc->GetOfflineShop();
		
		if (!pkOfflineShop)
			return;
		
		char szInsertQuery[QUERY_MAX_LEN];
		snprintf(szInsertQuery, sizeof(szInsertQuery), "INSERT INTO %soffline_shop_item (%s) VALUES (%s)", get_table_postfix(), szColumns, szValues);
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szInsertQuery));
		ITEM_MANAGER::instance().RemoveItem(pkItem);
		
		pkOfflineShop->BroadcastUpdateItem(bDisplayPos, ch->GetPlayerID());
		
		DWORD dwPulse = get_global_time() + 10;
		pkOfflineShop->SetActionPulse(dwPulse);
		
		LogManager::instance().ItemLog(ch, pkItem, "ADD ITEM OFFLINE SHOP", "");
	}
	else
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2405]");
}

void COfflineShopManager::RemoveItem(LPCHARACTER ch, BYTE bPos)
{
	if (!ch)
		return;
	
	if (quest::CQuestManager::instance().GetEventFlag("block_offline_shop_remove_item"))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2404]");
		return;
	}
	
	if (g_bOfflineShopMapAllowLimit)
	{
		if (!offlineshop_map_allow_find(ch->GetMapIndex()))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2380]");
			return;
		}
	}

	if (bPos >= OFFLINE_SHOP_HOST_ITEM_MAX_NUM)
	{
		sys_log(0, "OfflineShopManager::RemoveItem - Overflow slot! [%s]", ch->GetName());
		return;
	}
	
	// Check player has offline shop or not
	std::unique_ptr<SQLMsg> pMsgSelect(DBManager::instance().DirectQuery("SELECT COUNT(*) FROM srv1_player.offline_shop_npc WHERE owner_id = %u", ch->GetPlayerID()));
	MYSQL_ROW rowSelect = mysql_fetch_row(pMsgSelect->Get()->pSQLResult);
	
	BYTE bResult = 0;
	str_to_number(bResult, rowSelect[0]);
	
	if (!bResult)
	{
		// if (g_bOfflineShopSocketMax == 3)
#ifdef USE_LENTS_SHOULDER_SASH
		std::unique_ptr<SQLMsg> pMsgNoShop(DBManager::instance().DirectQuery("SELECT pos,count,vnum,type,subtype,socket0,socket1,socket2,socket3,socket4,socket5,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6,applytype0,applyvalue0,applytype1,applyvalue1,applytype2,applyvalue2,applytype3,applyvalue3,applytype4,applyvalue4,applytype5,applyvalue5,applytype6,applyvalue6,applytype7,applyvalue7 FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), ch->GetPlayerID(), bPos));
#else			
		std::unique_ptr<SQLMsg> pMsgNoShop(DBManager::instance().DirectQuery("SELECT pos,count,vnum,type,subtype,socket0,socket1,socket2,socket3,socket4,socket5,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6 FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), ch->GetPlayerID(), bPos));
#endif		
// 		else if(g_bOfflineShopSocketMax == 4)
// #ifdef USE_LENTS_SHOULDER_SASH			
// 			std::unique_ptr<SQLMsg> pMsgNoShop(DBManager::instance().DirectQuery("SELECT pos,count,vnum,socket0,socket1,socket2,socket3,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6,applytype0,applyvalue0,applytype1,applyvalue1,applytype2,applyvalue2,applytype3,applyvalue3,applytype4,applyvalue4,applytype5,applyvalue5,applytype6,applyvalue6,applytype7,applyvalue7 FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), ch->GetPlayerID(), bPos));			
// #else
// 			std::unique_ptr<SQLMsg> pMsgNoShop(DBManager::instance().DirectQuery("SELECT pos,count,vnum,socket0,socket1,socket2,socket3,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6 FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), ch->GetPlayerID(), bPos));			
// #endif		
// 		else if(g_bOfflineShopSocketMax == 5)
// #ifdef USE_LENTS_SHOULDER_SASH
// 			std::unique_ptr<SQLMsg> pMsgNoShop(DBManager::instance().DirectQuery("SELECT pos,count,vnum,socket0,socket1,socket2,socket3,socket4,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6,applytype0,applyvalue0,applytype1,applyvalue1,applytype2,applyvalue2,applytype3,applyvalue3,applytype4,applyvalue4,applytype5,applyvalue5,applytype6,applyvalue6,applytype7,applyvalue7 FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), ch->GetPlayerID(), bPos));			
// #else
// 			std::unique_ptr<SQLMsg> pMsgNoShop(DBManager::instance().DirectQuery("SELECT pos,count,vnum,socket0,socket1,socket2,socket3,socket4,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6 FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), ch->GetPlayerID(), bPos));			
// #endif		
// 		else if(g_bOfflineShopSocketMax == 6)
// #ifdef USE_LENTS_SHOULDER_SASH
// 			std::unique_ptr<SQLMsg> pMsgNoShop(DBManager::instance().DirectQuery("SELECT pos,count,vnum,socket0,socket1,socket2,socket3,socket4,socket5,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6,applytype0,applyvalue0,applytype1,applyvalue1,applytype2,applyvalue2,applytype3,applyvalue3,applytype4,applyvalue4,applytype5,applyvalue5,applytype6,applyvalue6,applytype7,applyvalue7 FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), ch->GetPlayerID(), bPos));			
// #else			
// 			std::unique_ptr<SQLMsg> pMsgNoShop(DBManager::instance().DirectQuery("SELECT pos,count,vnum,socket0,socket1,socket2,socket3,socket4,socket5,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6 FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), ch->GetPlayerID(), bPos));			
// #endif	

		if (pMsgNoShop->Get()->uiNumRows == 0)
		{
			sys_log(0, "OfflineShopManager::RemoveItem - This slot is empty! [%s]", ch->GetName());
			return;
		}
		
		TPlayerItem item;
		int rowsNoShop;
		if (!(rowsNoShop = mysql_num_rows(pMsgNoShop->Get()->pSQLResult)))
			return;

		for (int i = 0; i < rowsNoShop; ++i)
		{
			MYSQL_ROW row = mysql_fetch_row(pMsgNoShop->Get()->pSQLResult);
			int cur = 0;

			str_to_number(item.pos, row[cur++]);
			str_to_number(item.count, row[cur++]);
			str_to_number(item.vnum, row[cur++]);

			TItemTable* proto = ITEM_MANAGER::instance().GetTable(item.vnum);

			str_to_number(proto->bType, row[cur++]);
			str_to_number(proto->bSubType, row[cur++]);
			
			str_to_number(item.alSockets[0], row[cur++]);
			str_to_number(item.alSockets[1], row[cur++]);
			str_to_number(item.alSockets[2], row[cur++]);
			str_to_number(item.alSockets[3], row[cur++]);
			str_to_number(item.alSockets[4], row[cur++]);
			str_to_number(item.alSockets[5], row[cur++]);
			for (int j = 0; j < ITEM_ATTRIBUTE_MAX_NUM; j++)
			{
				str_to_number(item.aAttr[j].bType, row[cur++]);
				str_to_number(item.aAttr[j].sValue, row[cur++]);
			}
		}
		
		LPITEM pItem = ITEM_MANAGER::instance().CreateItem(item.vnum, item.count);
		if (!pItem)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2400]");
			return;
		}
		
		pItem->SetAttributes(item.aAttr);
		pItem->SetSockets(item.alSockets);

		int iEmptyPos;
		if (pItem->IsDragonSoul())
			iEmptyPos = ch->GetEmptyDragonSoulInventory(pItem);
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
		else if (pItem->IsSkillBook())
			iEmptyPos = ch->GetEmptySkillBookInventory(pItem->GetSize());
		else if (pItem->IsUpgradeItem())
			iEmptyPos = ch->GetEmptyUpgradeItemsInventory(pItem->GetSize());
		else if (pItem->IsStone())
			iEmptyPos = ch->GetEmptyStoneInventory(pItem->GetSize());
		else if (pItem->IsBox())
			iEmptyPos = ch->GetEmptyBoxInventory(pItem->GetSize());
		else if (pItem->IsEfsun())
			iEmptyPos = ch->GetEmptyEfsunInventory(pItem->GetSize());
		else if (pItem->IsCicek())
			iEmptyPos = ch->GetEmptyCicekInventory(pItem->GetSize());
#endif
		else
			iEmptyPos = ch->GetEmptyInventory(pItem->GetSize());

		if (iEmptyPos < 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1130]");
			return;
		}

		if (pItem->IsDragonSoul())
		{
			pItem->AddToCharacter(ch, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyPos));
		}
		else
		{
			pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
		}
		
		DBManager::instance().DirectQuery("DELETE FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), ch->GetPlayerID(), bPos);
		
		return;
	}
	// End Of Check player has offline shop or not
	
	DWORD dwShopVid = FindMyOfflineShop(ch->GetPlayerID());
	LPCHARACTER npc = CHARACTER_MANAGER::instance().Find(dwShopVid);

	if (npc)
	{
		LPOFFLINESHOP pkOfflineShop = npc->GetOfflineShop();

		// Check pkOfflineShop
		if (!pkOfflineShop)
			return;

		// if (g_bOfflineShopSocketMax == 3)
#ifdef USE_LENTS_SHOULDER_SASH
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT pos,count,vnum,type,subtype,socket0,socket1,socket2,socket3,socket4,socket5,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6,applytype0,applyvalue0,applytype1,applyvalue1,applytype2,applyvalue2,applytype3,applyvalue3,applytype4,applyvalue4,applytype5,applyvalue5,applytype6,applyvalue6,applytype7,applyvalue7 FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), ch->GetPlayerID(), bPos));
#else			
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT pos,count,vnum,type,subtype,socket0,socket1,socket2,socket3,socket4,socket5,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6 FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), ch->GetPlayerID(), bPos));
#endif		
// 		else if(g_bOfflineShopSocketMax == 4)
// #ifdef USE_LENTS_SHOULDER_SASH			
// 			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT pos,count,vnum,socket0,socket1,socket2,socket3,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6,applytype0,applyvalue0,applytype1,applyvalue1,applytype2,applyvalue2,applytype3,applyvalue3,applytype4,applyvalue4,applytype5,applyvalue5,applytype6,applyvalue6,applytype7,applyvalue7 FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), ch->GetPlayerID(), bPos));			
// #else
// 			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT pos,count,vnum,socket0,socket1,socket2,socket3,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6 FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), ch->GetPlayerID(), bPos));			
// #endif		
// 		else if(g_bOfflineShopSocketMax == 5)
// #ifdef USE_LENTS_SHOULDER_SASH
// 			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT pos,count,vnum,socket0,socket1,socket2,socket3,socket4,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6,applytype0,applyvalue0,applytype1,applyvalue1,applytype2,applyvalue2,applytype3,applyvalue3,applytype4,applyvalue4,applytype5,applyvalue5,applytype6,applyvalue6,applytype7,applyvalue7 FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), ch->GetPlayerID(), bPos));			
// #else
// 			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT pos,count,vnum,socket0,socket1,socket2,socket3,socket4,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6 FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), ch->GetPlayerID(), bPos));			
// #endif		
// 		else if(g_bOfflineShopSocketMax == 6)
// #ifdef USE_LENTS_SHOULDER_SASH
// 			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT pos,count,vnum,socket0,socket1,socket2,socket3,socket4,socket5,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6,applytype0,applyvalue0,applytype1,applyvalue1,applytype2,applyvalue2,applytype3,applyvalue3,applytype4,applyvalue4,applytype5,applyvalue5,applytype6,applyvalue6,applytype7,applyvalue7 FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), ch->GetPlayerID(), bPos));			
// #else			
// 			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT pos,count,vnum,socket0,socket1,socket2,socket3,socket4,socket5,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6 FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), ch->GetPlayerID(), bPos));			
// #endif	

		if (pMsg->Get()->uiNumRows == 0)
		{
			sys_log(0, "OfflineShopManager::RemoveItem - This slot is empty! [%s]", ch->GetName());
			return;
		}

		TPlayerItem item;
		int rows;
		if (!(rows = mysql_num_rows(pMsg->Get()->pSQLResult)))
			return;

		for (int i = 0; i < rows; ++i)
		{
			MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
			int cur = 0;

			str_to_number(item.pos, row[cur++]);
			str_to_number(item.count, row[cur++]);
			str_to_number(item.vnum, row[cur++]);
			TItemTable* proto = ITEM_MANAGER::instance().GetTable(item.vnum);
			str_to_number(proto->bType, row[cur++]);
			str_to_number(proto->bSubType, row[cur++]);
			str_to_number(item.alSockets[0], row[cur++]);
			str_to_number(item.alSockets[1], row[cur++]);
			str_to_number(item.alSockets[2], row[cur++]);
			str_to_number(item.alSockets[3], row[cur++]);
			str_to_number(item.alSockets[4], row[cur++]);
			str_to_number(item.alSockets[5], row[cur++]);
			for (int j = 0; j < ITEM_ATTRIBUTE_MAX_NUM; j++)
			{
				str_to_number(item.aAttr[j].bType, row[cur++]);
				str_to_number(item.aAttr[j].sValue, row[cur++]);
			}
		}

		LPITEM pItem = ITEM_MANAGER::instance().CreateItem(item.vnum, item.count);
		if (!pItem)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2400]");
			return;
		}
		
		pItem->SetAttributes(item.aAttr);
		pItem->SetSockets(item.alSockets);

		int iEmptyPos;
		if (pItem->IsDragonSoul())
			iEmptyPos = ch->GetEmptyDragonSoulInventory(pItem);
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
		else if (pItem->IsSkillBook())
			iEmptyPos = ch->GetEmptySkillBookInventory(pItem->GetSize());
		else if (pItem->IsUpgradeItem())
			iEmptyPos = ch->GetEmptyUpgradeItemsInventory(pItem->GetSize());
		else if (pItem->IsStone())
			iEmptyPos = ch->GetEmptyStoneInventory(pItem->GetSize());
		else if (pItem->IsBox())
			iEmptyPos = ch->GetEmptyBoxInventory(pItem->GetSize());
		else if (pItem->IsEfsun())
			iEmptyPos = ch->GetEmptyEfsunInventory(pItem->GetSize());
		else if (pItem->IsCicek())
			iEmptyPos = ch->GetEmptyCicekInventory(pItem->GetSize());
#endif
		else
			iEmptyPos = ch->GetEmptyInventory(pItem->GetSize());

		if (iEmptyPos < 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1130]");
			return;
		}

		if (pItem->IsDragonSoul())
			pItem->AddToCharacter(ch, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyPos));
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
		else if (pItem->IsSkillBook())
			pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
		else if (pItem->IsUpgradeItem())
			pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
		else if (pItem->IsStone())
			pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
		else if (pItem->IsBox())
			pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
		else if (pItem->IsEfsun())
			pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
		else if (pItem->IsCicek())
			pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
#endif
		else 
			pItem->AddToCharacter(ch, TItemPos(INVENTORY,iEmptyPos));

		DBManager::instance().DirectQuery("DELETE FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), ch->GetPlayerID(), bPos);
		pkOfflineShop->BroadcastUpdateItem(bPos, ch->GetPlayerID(), true);
		
		DWORD dwPulse = get_global_time() + 10;
		pkOfflineShop->SetActionPulse(dwPulse);

		if (LeftItemCount(ch) == 0)	
			pkOfflineShop->Destroy(npc);
		
		LogManager::instance().ItemLog(ch, pItem, "DELETE OFFLINE SHOP ITEM", "");
	}
	else
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2405]");
}

void COfflineShopManager::Refresh(LPCHARACTER ch)
{
	if (!ch)
		return;

	LPCHARACTER npc = CHARACTER_MANAGER::instance().Find(FindMyOfflineShop(ch->GetPlayerID()));
	if (!npc)
	{
		TPacketGCShop pack{};
		pack.header = HEADER_GC_OFFLINE_SHOP;
		pack.subheader = SHOP_SUBHEADER_GC_UPDATE_ITEM2;

		TPacketGCOfflineShopStart pack2{};
		pack2.owner_vid = 0;

		// if (g_bOfflineShopSocketMax == 3)
#ifdef USE_LENTS_SHOULDER_SASH
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT pos,count,vnum,type,subtype,price_gold,price_cheque,socket0,socket1,socket2,socket3,socket4,socket5,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6,applytype0,applyvalue0,applytype1,applyvalue1,applytype2,applyvalue2,applytype3,applyvalue3,applytype4,applyvalue4,applytype5,applyvalue5,applytype6,applyvalue6,applytype7,applyvalue7 FROM %soffline_shop_item WHERE owner_id = %u and status = 0", get_table_postfix(), ch->GetPlayerID()));
#else			
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT pos,count,vnum,type,subtype,price_gold,price_cheque,socket0,socket1,socket2,socket3,socket4,socket5,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6 FROM %soffline_shop_item WHERE owner_id = %u and status = 0", get_table_postfix(), ch->GetPlayerID()));
#endif		
// 		else if(g_bOfflineShopSocketMax == 4)
// #ifdef USE_LENTS_SHOULDER_SASH
// 			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT pos,count,vnum,price,socket0,socket1,socket2,socket3,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6,applytype0,applyvalue0,applytype1,applyvalue1,applytype2,applyvalue2,applytype3,applyvalue3,applytype4,applyvalue4,applytype5,applyvalue5,applytype6,applyvalue6,applytype7,applyvalue7 FROM %soffline_shop_item WHERE owner_id = %u and status = 0", get_table_postfix(), ch->GetPlayerID()));
// #else			
// 			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT pos,count,vnum,price,socket0,socket1,socket2,socket3,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6 FROM %soffline_shop_item WHERE owner_id = %u and status = 0", get_table_postfix(), ch->GetPlayerID()));
// #endif		
// 		else if(g_bOfflineShopSocketMax == 5)
// #ifdef USE_LENTS_SHOULDER_SASH
// 			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT pos,count,vnum,price,socket0,socket1,socket2,socket3,socket4,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6,applytype0,applyvalue0,applytype1,applyvalue1,applytype2,applyvalue2,applytype3,applyvalue3,applytype4,applyvalue4,applytype5,applyvalue5,applytype6,applyvalue6,applytype7,applyvalue7 FROM %soffline_shop_item WHERE owner_id = %u and status = 0", get_table_postfix(), ch->GetPlayerID()));
// #else			
// 			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT pos,count,vnum,price,socket0,socket1,socket2,socket3,socket4,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6 FROM %soffline_shop_item WHERE owner_id = %u and status = 0", get_table_postfix(), ch->GetPlayerID()));
// #endif		
// 		else if(g_bOfflineShopSocketMax == 6)
// #ifdef USE_LENTS_SHOULDER_SASH
// 			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT pos,count,vnum,price,socket0,socket1,socket2,socket3,socket4,socket5,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6,applytype0,applyvalue0,applytype1,applyvalue1,applytype2,applyvalue2,applytype3,applyvalue3,applytype4,applyvalue4,applytype5,applyvalue5,applytype6,applyvalue6,applytype7,applyvalue7 FROM %soffline_shop_item WHERE owner_id = %u and status = 0", get_table_postfix(), ch->GetPlayerID()));
// #else			
// 			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT pos,count,vnum,price,socket0,socket1,socket2,socket3,socket4,socket5,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6 FROM %soffline_shop_item WHERE owner_id = %u and status = 0", get_table_postfix(), ch->GetPlayerID()));
// #endif		

		MYSQL_ROW row;
		while (NULL != (row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
		{
			int cur = 0;
			BYTE bPos = 0;
			
			str_to_number(bPos, row[cur++]);
			str_to_number(pack2.items[bPos].count, row[cur++]);
			str_to_number(pack2.items[bPos].vnum, row[cur++]);
			TItemTable* proto = ITEM_MANAGER::instance().GetTable(pack2.items[bPos].vnum);
			str_to_number(proto->bType, row[cur++]);
			str_to_number(proto->bSubType, row[cur++]);
			str_to_number(pack2.items[bPos].price, row[cur++]);
			str_to_number(pack2.items[bPos].cheque, row[cur++]);

			DWORD alSockets[ITEM_SOCKET_MAX_NUM];
			for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
				str_to_number(alSockets[i], row[cur++]);

			TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
			for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
			{
				str_to_number(aAttr[i].bType, row[cur++]);
				str_to_number(aAttr[i].sValue, row[cur++]);
			}

			thecore_memcpy(pack2.items[bPos].alSockets, alSockets, sizeof(pack2.items[bPos].alSockets));
			thecore_memcpy(pack2.items[bPos].aAttr, aAttr, sizeof(pack2.items[bPos].aAttr));
		}

		pack.size = sizeof(pack) + sizeof(pack2);
		ch->GetDesc()->BufferedPacket(&pack, sizeof(TPacketGCShop));
		ch->GetDesc()->Packet(&pack2, sizeof(TPacketGCOfflineShopStart));
	}
	else
	{
		LPOFFLINESHOP pkOfflineShop = npc->GetOfflineShop();
		if (!pkOfflineShop)
			return;

		pkOfflineShop->Refresh(ch);
	}
}

void COfflineShopManager::RefreshMoney(LPCHARACTER ch)
{
	if (!ch)
		return;

	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT shop_money_gold FROM srv1_player.player WHERE id = %u", ch->GetPlayerID()));
	
	TPacketGCShop p{};
	TPacketGCOfflineShopMoney p2{};

	p.header = HEADER_GC_OFFLINE_SHOP;
	p.subheader = SHOP_SUBHEADER_GC_REFRESH_MONEY;
	
	//Przewidywalny zarobek
	std::unique_ptr<SQLMsg> pMsg2(DBManager::instance().DirectQuery("SELECT SUM(price_gold) FROM srv1_player.offline_shop_item WHERE owner_id = %u", ch->GetPlayerID()));
	if (pMsg2->Get()->uiNumRows == 0)
		return;

	int64_t ZarobekYang = 0;
	MYSQL_ROW row1 = mysql_fetch_row(pMsg2->Get()->pSQLResult);
	str_to_number(ZarobekYang, row1[0]);

	p2.zarobekYang = ZarobekYang;
	//Koniec

	if (pMsg->Get()->uiNumRows == 0)
	{
		p2.dwMoney = 0;
		p.size = sizeof(p) + sizeof(p2);
		ch->GetDesc()->BufferedPacket(&p, sizeof(TPacketGCShop));
		ch->GetDesc()->Packet(&p2, sizeof(TPacketGCOfflineShopMoney));
	}
	else
	{
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		str_to_number(p2.dwMoney, row[0]);
		p.size = sizeof(p) + sizeof(p2);
		ch->GetDesc()->BufferedPacket(&p, sizeof(TPacketGCShop));
		ch->GetDesc()->Packet(&p2, sizeof(TPacketGCOfflineShopMoney));
	}
}

void COfflineShopManager::RefreshCheque(LPCHARACTER ch)
{
	if (!ch)
		return;
	
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT shop_money_cheque FROM srv1_player.player WHERE id = %u", ch->GetPlayerID()));
	
	TPacketGCShop p{};
	TPacketGCOfflineShopMoney p2{};

	p.header = HEADER_GC_OFFLINE_SHOP;
	p.subheader = SHOP_SUBHEADER_GC_REFRESH_CHEQUE;
	
	//Przewidywalny zarobek
	std::unique_ptr<SQLMsg> pMsg2(DBManager::instance().DirectQuery("SELECT SUM(price_cheque) FROM srv1_player.offline_shop_item WHERE owner_id = %u", ch->GetPlayerID()));
	if (pMsg2->Get()->uiNumRows == 0)
		return;

	int64_t ZarobekCheque = 0;
	MYSQL_ROW row1 = mysql_fetch_row(pMsg2->Get()->pSQLResult);
	str_to_number(ZarobekCheque, row1[0]);

	p2.zarobekCheque = ZarobekCheque;
	//Koniec

	if (pMsg->Get()->uiNumRows == 0)
	{
		p2.dwCheque = 0;
		p.size = sizeof(p) + sizeof(p2);
		ch->GetDesc()->BufferedPacket(&p, sizeof(TPacketGCShop));
		ch->GetDesc()->Packet(&p2, sizeof(TPacketGCOfflineShopMoney));
	}
	else
	{
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		str_to_number(p2.dwCheque, row[0]);
		p.size = sizeof(p) + sizeof(p2);
		ch->GetDesc()->BufferedPacket(&p, sizeof(TPacketGCShop));
		ch->GetDesc()->Packet(&p2, sizeof(TPacketGCOfflineShopMoney));
	}
}

DWORD COfflineShopManager::FindMyOfflineShop(DWORD dwPID)
{
	TOfflineShopMap::iterator it = m_Map_pkOfflineShopByNPC2.find(dwPID);
	if (m_Map_pkOfflineShopByNPC2.end() == it)
		return 0;
	
	return it->second;
}

DWORD COfflineShopManager::GetPidByVid(DWORD dwVID)
{
	for (TOfflineShopMap::iterator it = m_Map_pkOfflineShopByNPC2.begin(); it != m_Map_pkOfflineShopByNPC2.end(); it++)
	{
		if(it->second != dwVID)
			continue;
		
		return it->first;
	}
	
	return 0;
}

void COfflineShopManager::StopShopping(LPCHARACTER ch)
{
	LPOFFLINESHOP pkOfflineShop;

	if (!(pkOfflineShop = ch->GetOfflineShop()))
		return;

	pkOfflineShop->RemoveGuest(ch);
	sys_log(0, "OFFLINE_SHOP: END: %s", ch->GetName());
}

void COfflineShopManager::Buy(LPCHARACTER ch, BYTE pos)
{
	if (!ch->GetOfflineShop())
		return;

	if (!ch->GetOfflineShopOwner())
		return;
	
	if (quest::CQuestManager::instance().GetEventFlag("block_offline_shop_buy_item"))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2404]");
		return;
	}

	if (DISTANCE_SQRT(ch->GetX() - ch->GetOfflineShopOwner()->GetX(), ch->GetY() - ch->GetOfflineShopOwner()->GetY()) > 1500)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2407]");
		return;
	}
	
	LPOFFLINESHOP pkOfflineShop = ch->GetOfflineShop();

	if (!pkOfflineShop)
		return;
	
	if ((unsigned)get_global_time() < pkOfflineShop->GetActionPulse())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2400]");
		return;
	}

	int ret = pkOfflineShop->Buy(ch, pos);

	// The result is not equal to SHOP_SUBHEADER_GC_OK, send the error to the character.
	if (SHOP_SUBHEADER_GC_OK != ret)
	{
		TPacketGCShop pack{};
		pack.header = HEADER_GC_OFFLINE_SHOP;
		pack.subheader	= ret;
		pack.size = sizeof(TPacketGCShop);
		
		if (ch->GetDesc())		
			ch->GetDesc()->Packet(&pack, sizeof(pack));
	}
}

void COfflineShopManager::WithdrawMoney(LPCHARACTER ch, int64_t dwRequiredMoney)
{
	if (g_bOfflineShopMapAllowLimit)
	{
		if (!offlineshop_map_allow_find(ch->GetMapIndex()))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2380]");
			return;
		}
	}

	if (!ch)
		return;

	if (dwRequiredMoney < 0)
		return;

	if (ch->IsObserverMode() || ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen() || ch->GetOfflineShopOwner())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2378]");
		return;
	}

	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT shop_money_gold FROM srv1_player.player WHERE id = %u", ch->GetPlayerID()));
	if (pMsg->Get()->uiNumRows == 0)
		return;

	int64_t dwCurrentMoney = 0;
	MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
	str_to_number(dwCurrentMoney, row[0]);

	if (dwRequiredMoney >= dwCurrentMoney)
	{
		if (test_server)
			ch->ChatPacket(CHAT_TYPE_INFO, "dwCurrentMoney(%lu) - dwRequiredMoney(%lu)", dwCurrentMoney, dwRequiredMoney);
		
		dwRequiredMoney = dwCurrentMoney;
	}

	bool isOverFlow = ch->GetGold() + dwRequiredMoney > GOLD_MAX - 1 ? true : false;
	if (isOverFlow)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2408]");
		return;
	}	

	if (dwRequiredMoney == 1)
		dwRequiredMoney = dwCurrentMoney;	
	
	ch->PointChange(POINT_GOLD, dwRequiredMoney, false);

	DBManager::instance().DirectQuery("UPDATE srv1_player.player SET shop_money_gold = shop_money_gold - "
										" %lld "
										" WHERE id = %u", dwRequiredMoney, ch->GetPlayerID());
	LogManager::instance().CharLog(ch, 0, "OFFLINE SHOP", "WITHDRAW MONEY");
}

void COfflineShopManager::WithdrawCheque(LPCHARACTER ch, int64_t dwRequiredCheque)
{
	if (g_bOfflineShopMapAllowLimit)
	{
		if (!offlineshop_map_allow_find(ch->GetMapIndex()))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2380]");
			return;
		}
	}

	if (!ch)
		return;

	if (dwRequiredCheque < 0)
		return;

	if (ch->IsObserverMode() || ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen() || ch->GetOfflineShopOwner())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2378]");
		return;
	}

	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT shop_money_cheque FROM srv1_player.player WHERE id = %u", ch->GetPlayerID()));
	if (pMsg->Get()->uiNumRows == 0)
		return;

	int64_t dwCurrentCheque = 0;
	MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
	str_to_number(dwCurrentCheque, row[0]);

	if (dwRequiredCheque >= dwCurrentCheque)
	{
		if (test_server)
			ch->ChatPacket(CHAT_TYPE_INFO, "dwCurrentCheque(%lu) - dwRequiredCheque(%lu)", dwCurrentCheque, dwRequiredCheque);
		
		dwRequiredCheque = dwCurrentCheque;
	}

	bool isOverFlow = ch->GetCheque() + dwRequiredCheque > CHEQUE_MAX - 1 ? true : false;
	if (isOverFlow)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2408]");
		return;
	}	

	if (dwRequiredCheque == 1)
		dwRequiredCheque = dwCurrentCheque;	
	
	ch->PointChange(POINT_CHEQUE, dwRequiredCheque, false);

	DBManager::instance().DirectQuery("UPDATE srv1_player.player SET shop_money_cheque = shop_money_cheque - "
										" %lld "
										" WHERE id = %u", dwRequiredCheque, ch->GetPlayerID());
	LogManager::instance().CharLog(ch, 0, "OFFLINE SHOP", "WITHDRAW CHEQUE");
}

BYTE COfflineShopManager::LeftItemCount(LPCHARACTER ch)
{
	if (!ch)
		return -1;
	
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT COUNT(*) FROM %soffline_shop_item WHERE owner_id = %u", get_table_postfix(), ch->GetPlayerID()));
	if (pMsg->Get()->uiNumRows == 0)
		return 0;

	MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
	BYTE bCount = 0;
	str_to_number(bCount, row[0]);
	return bCount;
}

#ifdef ENABLE_VS_SHOP_SEARCH
struct FSearch
{
	FSearch(LPCHARACTER ch, int searchType, int Vnum, const std::string& vnumString, int itemType, int itemSubtype) 
		: m_ch(ch), search_type(searchType), itemVnum(Vnum), m_vnumString(vnumString), type(itemType), subtype(itemSubtype)
	{
	}
	void operator () (LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER ch = (LPCHARACTER) ent;

			if (ch->GetRaceNum() != 30000)
				return;

			char szQuery[1024];

			if (search_type == 0)
			{
				snprintf(szQuery, sizeof(szQuery), "SELECT * FROM %soffline_shop_item WHERE owner_id = %u AND vnum = %d", get_table_postfix(), ch->GetOfflineShopRealOwner(), itemVnum);
			}
			else if (search_type == 1)
			{
				snprintf(szQuery, sizeof(szQuery), "SELECT * FROM %soffline_shop_item WHERE owner_id = %u AND type = %d AND subtype = %d", get_table_postfix(), ch->GetOfflineShopRealOwner(), type, subtype);
			}
			else if (search_type == 2)
			{
                snprintf(szQuery, sizeof(szQuery), "SELECT * FROM %soffline_shop_item WHERE owner_id = %u AND vnum IN (%s)", get_table_postfix(), ch->GetOfflineShopRealOwner(), m_vnumString.c_str());
            }
			else
			{
				return;
			}

			std::unique_ptr<SQLMsg> pSearchQuery(DBManager::instance().DirectQuery(szQuery));

			if (pSearchQuery->uiSQLErrno != 0) {
				sys_err("Item Search Query Failed, Error code: %ld", pSearchQuery->uiSQLErrno);
				return;
			}
			if (!pSearchQuery->Get()->uiNumRows) {
				return;
			}

			CTargetManager::Instance().CreateTarget(m_ch->GetPlayerID(), 9999, ch->GetName(), 3, ch->GetVID(), 0, m_ch->GetMapIndex(), NULL, 1);
			m_ch->SetQuestFlag("shop.search", m_ch->GetQuestFlag("shop.search") + 1);
		}
	}
	
	LPCHARACTER m_ch;
	int search_type;
	int itemVnum;
	std::string m_vnumString;
	int type;
	int subtype;
};

struct FClearSearch
{
	FClearSearch(LPCHARACTER ch) : m_ch(ch)
	{
	}
	void operator () (LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER ch = (LPCHARACTER) ent;

			if (ch->GetRaceNum() != 30000)
				return;

			CTargetManager::Instance().DeleteTarget(m_ch->GetPlayerID(), 9999, ch->GetName());
		}
	}
	LPCHARACTER m_ch;
};

void COfflineShopManager::ClearShopSearch(LPCHARACTER ch)
{
	if (!ch)
		return;
	LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(ch->GetMapIndex());
	if (!pSecMap)
		return;
	FClearSearch f(ch);
	pSecMap->for_each(f);
	ch->SetQuestFlag("clear_search.flood_check", get_global_time());
}

void COfflineShopManager::SearchInShops(LPCHARACTER ch, int search_type, int vnum, const std::string& vnumString, int type, int subtype)
{
    if (!ch)
        return;

    LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(ch->GetMapIndex());
    if (!pSecMap)
        return;

    FSearch f(ch, search_type, vnum, vnumString, type, subtype);
    pSecMap->for_each(f);

    int c = ch->GetQuestFlag("shop.search");
    if (c > 0)
    {
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2409;%d]", c);
        ch->SetQuestFlag("shop.search", 0);
    }
    
    ch->SetQuestFlag("item_search.flood_check", get_global_time());
}
#endif

#endif




