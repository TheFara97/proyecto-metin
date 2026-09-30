#include "stdafx.h"
#include "../../libgame/include/grid.h"
#include "constants.h"
#include "utils.h"
#include "config.h"
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
#include "offline_shop.h"
#include "p2p.h"
#include "target.h"

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
COfflineShop::COfflineShop() : m_pkOfflineShopNPC(NULL)
{
	m_pGrid = M2_NEW CGrid(12, 10);
	dwActionPulse = get_global_time() + 10;
}


COfflineShop::~COfflineShop()
{
	TPacketGCShop pack;
	pack.header = HEADER_GC_OFFLINE_SHOP;
	pack.subheader = SHOP_SUBHEADER_GC_END;
	pack.size = sizeof(TPacketGCShop);

	Broadcast(&pack, sizeof(pack));

	for (const auto & it : m_map_guest)
	{
		auto * ch = it.first;
		if (ch)
		{
			ch->SetOfflineShop(NULL);
		}
	}

	M2_DELETE(m_pGrid);
	M2_DESTROY_CHARACTER(m_pkOfflineShopNPC);
}

void COfflineShop::SetOfflineShopNPC(LPCHARACTER npc)
{
	m_pkOfflineShopNPC = npc;
}

bool COfflineShop::AddGuest(LPCHARACTER ch, LPCHARACTER npc)
{
	// If there is no ch, return false
	if (!ch)
		return false;

	// If ch is exchanging, return false
	if (ch->GetExchange())
		return false;

	// If target is shopping, return false
	if (ch->GetShop())
		return false;

	// If target is look at private shop, return false
	if (ch->GetMyShop())
		return false;

	// If target is look at offline shop, return false
	// if (ch->GetOfflineShop())
	// 	return false;

	ch->SetOfflineShop(this);
	m_map_guest.insert(GuestMapType::value_type(ch, false));

	TPacketGCShop pack;
	pack.header = HEADER_GC_OFFLINE_SHOP;
	pack.subheader = SHOP_SUBHEADER_GC_START;

	TPacketGCOfflineShopStart pack2;
	memset(&pack2, 0, sizeof(pack2));
	pack2.owner_vid = npc->GetVID();

	pack2.IsOnline = 0;
	const char* name = npc->GetName();
	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(name);
	if (tch)
	{
	 	pack2.IsOnline = 1;
	}

#ifdef USE_LENTS_SHOULDER_SASH
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT pos,count,vnum,price_gold,price_cheque,socket0,socket1,socket2,socket3,socket4,socket5,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6,applytype0,applyvalue0,applytype1,applyvalue1,applytype2,applyvalue2,applytype3,applyvalue3,applytype4,applyvalue4,applytype5,applyvalue5,applytype6,applyvalue6,applytype7,applyvalue7 FROM %soffline_shop_item WHERE owner_id = %u", get_table_postfix(), npc ? npc->GetOfflineShopRealOwner() : 0));
#else
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT pos,count,vnum,price_gold,price_cheque,socket0,socket1,socket2,socket3,socket4,socket5,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6 FROM %soffline_shop_item WHERE owner_id = %u", get_table_postfix(), npc ? npc->GetOfflineShopRealOwner() : 0));
#endif

	LPEVENT pkEvent = CTargetManager::instance().GetTargetEvent(ch->GetPlayerID(), npc->GetOfflineShopRealOwner(), "SHOP_ITEM_SEARCH");
	if (pkEvent)
	{
		CTargetManager::instance().DeleteTarget(ch->GetPlayerID(), npc->GetOfflineShopRealOwner(), "SHOP_ITEM_SEARCH");
	}

	if (pMsg->Get()->uiNumRows == 0)
	{
		DBManager::instance().DirectQuery("DELETE FROM srv1_player.offline_shop_npc WHERE owner_id = %u", npc->GetOfflineShopRealOwner());
		ch->SetOfflineShop(NULL);
		ch->SetOfflineShopOwner(NULL);
		M2_DESTROY_CHARACTER(npc);
		return false;
	}

	MYSQL_ROW row;
	while (NULL != (row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
	{
		int cur = 0;
		BYTE bPos = 0;
		
		str_to_number(bPos, row[cur++]);
		str_to_number(pack2.items[bPos].count, row[cur++]);
		str_to_number(pack2.items[bPos].vnum, row[cur++]);
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
	return true;
}

void COfflineShop::RemoveGuest(LPCHARACTER ch)
{
	// If this offline shop is not equal to this, break it
	if (ch->GetOfflineShop() != this)
		return;

	m_map_guest.erase(ch);
	ch->SetOfflineShop(NULL);

	TPacketGCShop pack;
	pack.header = HEADER_GC_OFFLINE_SHOP;
	pack.subheader = SHOP_SUBHEADER_GC_END;
	pack.size = sizeof(TPacketGCShop);

	ch->GetDesc()->Packet(&pack, sizeof(pack));
}

void COfflineShop::RemoveAllGuest()
{
	TPacketGCShop pack;
	pack.header = HEADER_GC_OFFLINE_SHOP;
	pack.subheader = SHOP_SUBHEADER_GC_END;
	pack.size = sizeof(TPacketGCShop);

	Broadcast(&pack, sizeof(pack));

	for (const auto& it : m_map_guest)
	{
		auto* ch = it.first;
		if (ch)
		{
			ch->SetOfflineShop(NULL);
		}
	}
}

void COfflineShop::Destroy(LPCHARACTER npc)
{
	DBManager::instance().DirectQuery("DELETE FROM %soffline_shop_npc WHERE owner_id = %u", get_table_postfix(), npc->GetOfflineShopRealOwner());
	RemoveAllGuest();
	if (npc != NULL)
	{
		--g_shopCount[npc->GetMapIndex()];
	}
	M2_DESTROY_CHARACTER(npc);
}

int64_t COfflineShop::Buy(LPCHARACTER ch, BYTE bPos)
{

	if (ch->GetOfflineShopOwner()->GetOfflineShopRealOwner() == ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2398]");
		return SHOP_SUBHEADER_GC_OK;
	}
	
	if (ch->GetGMLevel() > GM_LOW_WIZARD && ch->GetGMLevel() < GM_IMPLEMENTOR)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2399]");
		return SHOP_SUBHEADER_GC_OK;
	}

	if (bPos >= OFFLINE_SHOP_HOST_ITEM_MAX_NUM)
	{
		sys_log(0, "OfflineShop::Buy : invalid position %d : %s", bPos, ch->GetName());
		return SHOP_SUBHEADER_GC_INVALID_POS;
	}
	
	if ((unsigned)get_global_time() < GetActionPulse())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2400]");
		return SHOP_SUBHEADER_GC_END;
	}

	sys_log(0, "OfflineShop::Buy : name %s pos %d", ch->GetName(), bPos);

	GuestMapType::iterator it = m_map_guest.find(ch);
	if (it == m_map_guest.end())
		return SHOP_SUBHEADER_GC_END;
	
	char szQuery[2048];
#ifdef USE_LENTS_SHOULDER_SASH
	snprintf(szQuery, sizeof(szQuery), "SELECT pos,count,vnum,price_gold,price_cheque,socket0,socket1,socket2,socket3,socket4,socket5,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6,applytype0,applyvalue0,applytype1,applyvalue1,applytype2,applyvalue2,applytype3,applyvalue3,applytype4,applyvalue4,applytype5,applyvalue5,applytype6,applyvalue6,applytype7,applyvalue7 FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), ch->GetOfflineShopOwner()->GetOfflineShopRealOwner(), bPos);
#else
	snprintf(szQuery, sizeof(szQuery), "SELECT pos,count,vnum,price_gold,price_cheque,socket0,socket1,socket2,socket3,socket4,socket5,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6 FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), ch->GetOfflineShopOwner()->GetOfflineShopRealOwner(), bPos);
#endif

	std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));

	MYSQL_ROW row;
	int64_t dwPrice = 0;
	int64_t dwPrice2 = 0;
	DWORD dwItemVnum = 0;
	int bCount = 0;
	DWORD alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];

	while (NULL != (row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
	{
		int cur = 1;
		str_to_number(bCount, row[cur++]);
		str_to_number(dwItemVnum, row[cur++]);
		str_to_number(dwPrice, row[cur++]);
		str_to_number(dwPrice2, row[cur++]);

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
			str_to_number(alSockets[i], row[cur++]);
		
		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		{
			str_to_number(aAttr[i].bType, row[cur++]);
			str_to_number(aAttr[i].sValue, row[cur++]);
		}
	}
	
	TItemTable * itemTable = ITEM_MANAGER::instance().GetTable(dwItemVnum);
	if (itemTable != NULL)
	{
		for (BYTE b = 0; b < ITEM_LIMIT_MAX_NUM; ++b)
		{
			if (itemTable->aLimits[b].bType == LIMIT_REAL_TIME)
			{
				if ((unsigned)get_global_time() >= alSockets[0])
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2400]");
					return SHOP_SUBHEADER_GC_OK;
				}
			}
			
			if (itemTable->aLimits[b].bType == LIMIT_REAL_TIME_START_FIRST_USE)
			{
				if ((alSockets[1] == 0) || (alSockets[0] == 0))
				{
					continue;
				}
				
				if ((unsigned)get_global_time() >= alSockets[0])
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2400]");
					return SHOP_SUBHEADER_GC_OK;
				}
			}
		}
	}

	if (ch->GetGold() < static_cast<int64_t>(dwPrice))
	{
		sys_log(1, "OfflineShop::Buy : Not enough money : %s has %u, price %lld", ch->GetName(), ch->GetGold(), dwPrice);
		return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY;
	}

	if (ch->GetCheque() < static_cast<int64_t>(dwPrice2))
	{
		sys_log(1, "OfflineShop::Buy : Not enough cheque : %s has %u, price %lld", ch->GetName(), ch->GetCheque(), dwPrice2);
		return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY;
	}

	LPITEM pItem = ITEM_MANAGER::Instance().CreateItem(dwItemVnum, bCount);
	if (!pItem)
		return SHOP_SUBHEADER_GC_SOLD_OUT;
	
	// Set Attributes, Sockets
	pItem->SetAttributes(aAttr);
	for (BYTE i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		pItem->SetSocket(i, alSockets[i]);
	// End Of Set Attributes, Sockets
	
	// If item is a dragon soul item or normal item
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

	// If iEmptyPos is less than 0, return inventory is full
	if (iEmptyPos < 0)
		return SHOP_SUBHEADER_GC_INVENTORY_FULL;

	// If item is a dragon soul, add this item in dragon soul inventory
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

	if (pItem)
		sys_log(0, "OFFLINE_SHOP: BUY: name %s %s(x %u):%u price_gold %lld price_cheque %lld", ch->GetName(), pItem->GetName(), pItem->GetCount(), pItem->GetID(), dwPrice, dwPrice2);

	DBManager::instance().DirectQuery("UPDATE srv1_player.player SET shop_money_gold = shop_money_gold + %lld, shop_money_cheque = shop_money_cheque + %lld WHERE id = %u", dwPrice, dwPrice2, ch->GetOfflineShopOwner()->GetOfflineShopRealOwner());
	// DBManager::instance().DirectQuery("UPDATE srv1_player.player SET shop_money_cheque = shop_money_cheque + %lld WHERE id = %u", dwPrice2, ch->GetOfflineShopOwner()->GetOfflineShopRealOwner());

	RemoveItem(ch->GetOfflineShopOwner()->GetOfflineShopRealOwner(), bPos);
	BroadcastUpdateItem(bPos, ch->GetOfflineShopOwner()->GetOfflineShopRealOwner(), true);

	ch->PointChange(POINT_GOLD, -dwPrice);
	ch->PointChange(POINT_CHEQUE, -dwPrice2);
	ch->Save();
	
	LogManager::instance().ItemLog(ch, pItem, "BUY ITEM FROM OFFLINE SHOP", "");
	DBManager::instance().DirectQuery("INSERT INTO srv1_log.offlineshop_log (pid,action,item,count,price_gold,price_cheque,time) VALUES (%d, 'BUY', %d, %d, %lld, %lld, NOW())", ch->GetPlayerID(), pItem->GetVnum(), bCount, dwPrice, dwPrice2);
	DBManager::instance().DirectQuery("INSERT INTO srv1_log.offlineshop_log (pid,action,item,count,price_gold,price_cheque,time) VALUES (%d, 'SELL', %d, %d, %lld, %lld, NOW())", ch->GetOfflineShopOwner()->GetOfflineShopRealOwner(), pItem->GetVnum(), bCount, dwPrice, dwPrice2);
	int bLeftItemCount = GetLeftItemCount(ch->GetOfflineShopOwner()->GetOfflineShopRealOwner());
	if (bLeftItemCount == 0)
		Destroy(ch->GetOfflineShopOwner());

	return (SHOP_SUBHEADER_GC_OK);
}

void COfflineShop::BroadcastUpdateItem(BYTE bPos, DWORD dwPID, bool bDestroy)
{
	TPacketGCShop pack{};
	TPacketGCShopUpdateItem pack2{};

	TEMP_BUFFER buf;

	pack.header = HEADER_GC_OFFLINE_SHOP;
	pack.subheader = SHOP_SUBHEADER_GC_UPDATE_ITEM;
	pack.size = sizeof(pack) + sizeof(pack2);
	pack2.pos = bPos;

	if (bDestroy)
	{
		pack2.item.vnum = 0;
		pack2.item.count = 0;
		pack2.item.price = 0;
		pack2.item.cheque = 0;
		memset(pack2.item.alSockets, 0, sizeof(pack2.item.alSockets));
		memset(pack2.item.aAttr, 0, sizeof(pack2.item.aAttr));
	}
	else
	{
		char szQuery[2048];
#ifdef USE_LENTS_SHOULDER_SASH
		snprintf(szQuery, sizeof(szQuery), "SELECT pos,count,vnum,price_gold,price_cheque,socket0,socket1,socket2,socket3,socket4,socket5,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6,applytype0,applyvalue0,applytype1,applyvalue1,applytype2,applyvalue2,applytype3,applyvalue3,applytype4,applyvalue4,applytype5,applyvalue5,applytype6,applyvalue6,applytype7,applyvalue7 FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), dwPID, bPos);
#else		
		snprintf(szQuery, sizeof(szQuery), "SELECT pos,count,vnum,price_gold,price_cheque,socket0,socket1,socket2,socket3,socket4,socket5,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6 FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), dwPID, bPos);
#endif

		std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
		MYSQL_ROW row;
		while (NULL != (row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
		{
			int cur = 1;
			
			str_to_number(pack2.item.count, row[cur++]);
			str_to_number(pack2.item.vnum, row[cur++]);
			str_to_number(pack2.item.price, row[cur++]);
			str_to_number(pack2.item.cheque, row[cur++]);
			
			for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
				str_to_number(pack2.item.alSockets[i], row[cur++]);
			
			for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
			{
				str_to_number(pack2.item.aAttr[i].bType, row[cur++]);
				str_to_number(pack2.item.aAttr[i].sValue, row[cur++]);
			}
		}
	}

	buf.write(&pack, sizeof(pack));
	buf.write(&pack2, sizeof(pack2));
	Broadcast(buf.read_peek(), buf.size());
}

void COfflineShop::BroadcastUpdatePrice(BYTE bPos, int64_t dwPrice, int64_t dwPrice2)
{
	TPacketGCShop pack{};
	TPacketGCShopUpdatePrice pack2{};

	TEMP_BUFFER buf;
	
	pack.header = HEADER_GC_OFFLINE_SHOP;
	pack.subheader = SHOP_SUBHEADER_GC_UPDATE_PRICE;
	pack.size = sizeof(pack) + sizeof(pack2);

	pack2.bPos = bPos;
	pack2.iPrice = dwPrice;
	pack2.iPrice2 = dwPrice2;

	buf.write(&pack, sizeof(pack));
	buf.write(&pack2, sizeof(pack2));

	Broadcast(buf.read_peek(), buf.size());
}

void COfflineShop::Refresh(LPCHARACTER ch)
{
	TPacketGCShop pack{};
	pack.header = HEADER_GC_OFFLINE_SHOP;
	pack.subheader = SHOP_SUBHEADER_GC_UPDATE_ITEM2;

	TPacketGCOfflineShopStart pack2{};
	
	pack2.owner_vid = 0;

#ifdef USE_LENTS_SHOULDER_SASH
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT pos,count,vnum,price_gold,price_cheque,socket0,socket1,socket2,socket3,socket4,socket5,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6,applytype0,applyvalue0,applytype1,applyvalue1,applytype2,applyvalue2,applytype3,applyvalue3,applytype4,applyvalue4,applytype5,applyvalue5,applytype6,applyvalue6,applytype7,applyvalue7 FROM %soffline_shop_item WHERE owner_id = %u", get_table_postfix(), ch->GetPlayerID()));
#else
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT pos,count,vnum,price_gold,price_cheque,socket0,socket1,socket2,socket3,socket4,socket5,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6 FROM %soffline_shop_item WHERE owner_id = %u", get_table_postfix(), ch->GetPlayerID()));
#endif

	MYSQL_ROW row;
	while (NULL != (row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
	{
		int cur = 0;
		BYTE bPos = 0;
		
		str_to_number(bPos, row[cur++]);
		str_to_number(pack2.items[bPos].count, row[cur++]);
		str_to_number(pack2.items[bPos].vnum, row[cur++]);
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

bool COfflineShop::RemoveItem(DWORD dwVID, BYTE bPos)
{
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("DELETE FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), dwVID, bPos));
	return pMsg->Get()->uiAffectedRows > 0;
}

BYTE COfflineShop::GetLeftItemCount(DWORD dwPID)
{
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT COUNT(*) FROM %soffline_shop_item WHERE owner_id = %u", get_table_postfix(), dwPID));
	if (pMsg->Get()->uiNumRows == 0)
		return 0;

	MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
	BYTE bCount = 0;
	str_to_number(bCount, row[0]);
	return bCount;
}

void COfflineShop::Broadcast(const void * data, int bytes)
{
	sys_log(1, "OfflineShop::Broadcast %p %d", data, bytes);
	
	for (const auto & it : m_map_guest)
	{
		auto * ch = it.first;
		if (ch && ch->GetDesc())
			ch->GetDesc()->Packet(data, bytes);			
	}
}
#endif