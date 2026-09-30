#include "StdAfx.h"
#include "PythonNetworkStream.h"
#include "Packet.h"
#include "PythonOfflineshop.h"
#ifdef ENABLE_AVERAGE_PRICE
#include "OfflineShopAveragePrice.h"
#endif
#ifdef ENABLE_OFFLINE_SHOP
#define SendObj(obj) (Send(sizeof(obj) , &obj))

bool CPythonNetworkStream::RecvOfflineshopPacket()
{
	TPacketGCNewOfflineshop pack;
	if (!Recv(sizeof(pack), &pack))
	{
		return false;
	}

	switch (pack.bSubHeader)
	{
	case offlineshop::SUBHEADER_GC_SHOP_LIST:					return RecvOfflineshopShopList();
	case offlineshop::SUBHEADER_GC_SHOP_OPEN:					return RecvOfflineshopShopOpen();
	case offlineshop::SUBHEADER_GC_SHOP_OPEN_OWNER:				return RecvOfflineshopShopOpenOwner();
	case offlineshop::SUBHEADER_GC_SHOP_OPEN_OWNER_NO_SHOP:		return RecvOfflineshopShopOpenOwnerNoShop();
	case offlineshop::SUBHEADER_GC_SHOP_CLOSE:					return RecvOfflineshopShopClose();
	case offlineshop::SUBHEADER_GC_SHOP_BUY_ITEM_FROM_SEARCH:	return RecvOfflineshopShopBuyItemFromSearch();
	case offlineshop::SUBHEADER_GC_SHOP_FILTER_RESULT:			return RecvOfflineshopShopFilterResult();
	case offlineshop::SUBHEADER_GC_SHOP_SAFEBOX_REFRESH:		return RecvOfflineshopShopSafeboxRefresh();
#ifdef ENABLE_SHOP_IN_CITIES
	case offlineshop::SUBHEADER_GC_INSERT_SHOP_ENTITY:			return RecvOfflineshopInsertEntity();
	case offlineshop::SUBHEADER_GC_REMOVE_SHOP_ENTITY:			return RecvOfflineshopRemoveEntity();
#endif
#ifdef ENABLE_OFFLINESHOP_NOTIFICATION
	case offlineshop::SUBHEADER_GC_NOTIFICATION:				return RecvOfflineshopNotification();
#endif
#ifdef ENABLE_AVERAGE_PRICE
	case offlineshop::SUBHEADER_GC_AVERAGE_PRICE:				return RecvOfflineshopAveragePrice();
#endif
	default:
		TraceError("UNKNOWN OFFLINESHOP SUBHEADER : %d ", pack.bSubHeader);
		return false;
	}
}

bool CPythonNetworkStream::RecvOfflineshopShopList()
{
	offlineshop::TSubPacketGCShopList subpack;
	if (!Recv(sizeof(subpack), &subpack))
	{
		return false;
	}

	CPythonOfflineshop::instance().ShopListClear();

	offlineshop::TShopInfo shop;
	for (DWORD i = 0; i < subpack.dwShopCount; i++)
	{
		if (!Recv(sizeof(shop), &shop))
		{
			return false;
		}

		CPythonOfflineshop::instance().ShopListAddItem(shop);
	}

	CPythonOfflineshop::instance().ShopListShow();
	return true;
}

bool CPythonNetworkStream::RecvOfflineshopShopOpen()
{
	offlineshop::TSubPacketGCShopOpen subpack;
	if (!Recv(sizeof(subpack), &subpack))
	{
		return false;
	}

	offlineshop::TItemInfo itemInfo;
	std::vector<offlineshop::TItemInfo> items;

	for (DWORD i = 0; i < subpack.shop.dwCount; i++)
	{
		if (!Recv(sizeof(itemInfo), &itemInfo))
		{
			return false;
		}

		items.push_back(itemInfo);
	}

	CPythonOfflineshop::Instance().OpenShop(subpack.shop, items);
	return true;
}

bool CPythonNetworkStream::RecvOfflineshopShopOpenOwner()
{
	offlineshop::TSubPacketGCShopOpenOwner subpack;
	if (!Recv(sizeof(subpack), &subpack))
	{
		return false;
	}

	std::vector<offlineshop::TItemInfo> items;
	items.resize(subpack.shop.dwCount);

	if (subpack.shop.dwCount != 0)
	{
		if (!Recv(sizeof(offlineshop::TItemInfo) * subpack.shop.dwCount, &items[0]))
		{
			return false;
		}
	}

	CPythonOfflineshop::Instance().OpenShopOwner(subpack.shop, items);
	return true;
}

bool CPythonNetworkStream::RecvOfflineshopShopOpenOwnerNoShop()
{
	CPythonOfflineshop::instance().OpenShopOwnerNoShop();
	return true;
}

bool CPythonNetworkStream::RecvOfflineshopShopBuyItemFromSearch()
{
	offlineshop::TSubPacketGCShopBuyItemFromSearch subpack;
	if (!Recv(sizeof(subpack), &subpack))
	{
		return false;
	}

	CPythonOfflineshop::Instance().BuyFromSearch(subpack.dwOwnerID, subpack.dwItemID);
	return true;
}

bool CPythonNetworkStream::RecvOfflineshopShopClose()
{
	CPythonOfflineshop::instance().ShopClose();
	return true;
}

bool CPythonNetworkStream::RecvOfflineshopShopFilterResult()
{
	offlineshop::TSubPacketGCShopFilterResult subpack;
	if (!Recv(sizeof(subpack), &subpack))
	{
		return false;
	}

	offlineshop::TItemInfo itemInfo;
	std::vector<offlineshop::TItemInfo> items;

	for (DWORD i = 0; i < subpack.dwCount; i++)
	{
		if (!Recv(sizeof(itemInfo), &itemInfo))
		{
			return false;
		}
		items.push_back(itemInfo);
	}

	CPythonOfflineshop::Instance().ShopFilterResult(items);
	return true;
}

bool CPythonNetworkStream::RecvOfflineshopShopSafeboxRefresh()
{
	offlineshop::TSubPacketGCShopSafeboxRefresh subpack;
	if (!Recv(sizeof(subpack), &subpack))
	{
		return false;
	}

	std::vector<DWORD> ids;
	std::vector<offlineshop::TItemInfoEx> items;
	ids.resize(subpack.dwItemCount);
	items.resize(subpack.dwItemCount);

	for (DWORD i = 0; i < subpack.dwItemCount; i++)
	{
		if (!Recv(sizeof(DWORD), &ids[i]))
		{
			return false;
		}

		if (!Recv(sizeof(offlineshop::TItemInfoEx), &items[i]))
		{
			return false;
		}
	}

	CPythonOfflineshop::instance().SafeboxRefresh(subpack.valute, ids, items);
	return true;
}

#ifdef ENABLE_SHOP_IN_CITIES
bool CPythonNetworkStream::RecvOfflineshopInsertEntity()
{
	offlineshop::TSubPacketGCInsertShopEntity subpack;
	if (!Recv(sizeof(subpack), &subpack))
		return false;

	CPythonOfflineshop::Instance().InsertEntity(subpack.dwVID, subpack.iType, subpack.szName, subpack.x, subpack.y, subpack.z
#ifdef ENABLE_SHOP_DECORATION
	, subpack.dwShopDecoration
#endif
	);
	return true;
}

bool CPythonNetworkStream::RecvOfflineshopRemoveEntity()
{
	offlineshop::TSubPacketGCRemoveShopEntity subpack;
	if (!Recv(sizeof(subpack), &subpack))
		return false;

	CPythonOfflineshop::Instance().RemoveEntity(subpack.dwVID);
	return true;
}

void CPythonNetworkStream::SendOfflineshopOnClickShopEntity(DWORD dwPickedShopVID)
{
	TPacketCGNewOfflineShop pack;
	pack.bHeader = HEADER_CG_NEW_OFFLINESHOP;
	pack.bSubHeader = offlineshop::SUBHEADER_CG_CLICK_ENTITY;
	pack.wSize = sizeof(pack) + sizeof(offlineshop::TSubPacketCGShopClickEntity);

	offlineshop::TSubPacketCGShopClickEntity subpack;
	subpack.dwShopVID = dwPickedShopVID;

	if (!SendObj(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.bSubHeader);
		return;
	}

	if (!SendObj(subpack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - subpack ", pack.bSubHeader);
		return;
	}
}
#endif

void CPythonNetworkStream::SendOfflineshopShopCreate(const offlineshop::TShopInfo& shopInfo, const std::vector<offlineshop::TShopItemInfo>& items)
{
	TPacketCGNewOfflineShop pack;
	pack.bHeader = HEADER_CG_NEW_OFFLINESHOP;
	pack.bSubHeader = offlineshop::SUBHEADER_CG_SHOP_CREATE_NEW;
	pack.wSize = (WORD)(sizeof(pack) + sizeof(offlineshop::TSubPacketCGShopCreate) + items.size() * sizeof(offlineshop::TShopItemInfo));
	offlineshop::TSubPacketCGShopCreate subpack;
	offlineshop::CopyObject(subpack.shop, shopInfo);
	subpack.shop.dwCount = items.size();

	if (!SendObj(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.bSubHeader);
		return;
	}

	if (!SendObj(subpack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - subpack ", pack.bSubHeader);
		return;
	}

	for (DWORD i = 0; i < items.size(); i++)
	{
		const offlineshop::TShopItemInfo& info = items[i];
		if (!SendObj(info))
		{
			TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - items forloop", pack.bSubHeader);
			return;
		}
	}
}

void CPythonNetworkStream::SendOfflineshopChangeName(const char* szName)
{
	TPacketCGNewOfflineShop pack;
	pack.bHeader = HEADER_CG_NEW_OFFLINESHOP;
	pack.bSubHeader = offlineshop::SUBHEADER_CG_SHOP_CHANGE_NAME;
	pack.wSize = sizeof(pack) + sizeof(offlineshop::TSubPacketCGShopChangeName);
	offlineshop::TSubPacketCGShopChangeName subpack;
	strncpy(subpack.szName, szName, sizeof(subpack.szName));

	if (!SendObj(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.bSubHeader);
		return;
	}

	if (!SendObj(subpack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - subpack ", pack.bSubHeader);
		return;
	}
}

#ifdef ENABLE_OFFLINESHOP_EXTEND_TIME
void CPythonNetworkStream::SendOfflineshopExtendTime(DWORD dwTime)
{
	TPacketCGNewOfflineShop pack;
	pack.bHeader = HEADER_CG_NEW_OFFLINESHOP;
	pack.bSubHeader = offlineshop::SUBHEADER_CG_SHOP_EXTEND_TIME;
	pack.wSize = sizeof(pack) + sizeof(offlineshop::TSubPacketCGShopExtendTime);

	offlineshop::TSubPacketCGShopExtendTime subpack;
	subpack.dwTime = dwTime;

	if (!SendObj(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.bSubHeader);
		return;
	}

	if (!SendObj(subpack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - subpack ", pack.bSubHeader);
		return;
	}
}

#ifdef ENABLE_AVERAGE_PRICE
void CPythonNetworkStream::SendOfflineShopAveragePrice(DWORD vnum)
{
	COfflineShopAveragePrice& average = COfflineShopAveragePrice().Instance();
	long long price = average.GetAveragePrice(vnum);

	if (price == -1)
	{
		TPacketCGNewOfflineShop pack;
		pack.bHeader = HEADER_CG_NEW_OFFLINESHOP;
		pack.bSubHeader = offlineshop::SUBHEADER_CG_AVERAGE_PRICE;
		pack.wSize = sizeof(pack) + sizeof(DWORD);

		DWORD subPack = vnum;

		if (!SendObj(pack))
		{
			TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.bSubHeader);
			return;
		}

		if (!SendObj(subPack))
		{
			TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - subpack ", pack.bSubHeader);
			return;
		}
	}
	else
	{
		CPythonOfflineshop& offlineshop = CPythonOfflineshop::instance();
		offlineshop.SetAveragePrice(price);
	}
}
#endif


#endif

void CPythonNetworkStream::SendOfflineshopForceCloseShop()
{
	TPacketCGNewOfflineShop pack;
	pack.bHeader = HEADER_CG_NEW_OFFLINESHOP;
	pack.bSubHeader = offlineshop::SUBHEADER_CG_SHOP_FORCE_CLOSE;
	pack.wSize = sizeof(pack);

	if (!SendObj(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.bSubHeader);
		return;
	}
}

void CPythonNetworkStream::SendOfflineshopRequestShopList()
{
	TPacketCGNewOfflineShop pack;
	pack.bHeader = HEADER_CG_NEW_OFFLINESHOP;
	pack.bSubHeader = offlineshop::SUBHEADER_CG_SHOP_REQUEST_SHOPLIST;
	pack.wSize = sizeof(pack);

	if (!SendObj(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.bSubHeader);
		return;
	}
}

void CPythonNetworkStream::SendOfflineshopOpenShop(DWORD dwOwnerID)
{
	TPacketCGNewOfflineShop pack;
	pack.bHeader = HEADER_CG_NEW_OFFLINESHOP;
	pack.bSubHeader = offlineshop::SUBHEADER_CG_SHOP_OPEN;
	pack.wSize = sizeof(pack) + sizeof(offlineshop::TSubPacketCGShopOpen);
	offlineshop::TSubPacketCGShopOpen subpack;
	subpack.dwOwnerID = dwOwnerID;

	if (!SendObj(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.bSubHeader);
		return;
	}

	if (!SendObj(subpack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - subpack ", pack.bSubHeader);
		return;
	}
}

void CPythonNetworkStream::SendOfflineshopOpenShopOwner()
{
	TPacketCGNewOfflineShop pack;
	pack.bHeader = HEADER_CG_NEW_OFFLINESHOP;
	pack.bSubHeader = offlineshop::SUBHEADER_CG_SHOP_OPEN_OWNER;
	pack.wSize = sizeof(pack);

	if (!SendObj(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.bSubHeader);
		return;
	}
}

void CPythonNetworkStream::SendOfflineshopBuyItem(DWORD dwOwnerID, DWORD dwItemID, bool isSearch)
{
	TPacketCGNewOfflineShop pack;
	pack.bHeader = HEADER_CG_NEW_OFFLINESHOP;
	pack.bSubHeader = offlineshop::SUBHEADER_CG_SHOP_BUY_ITEM;
	pack.wSize = sizeof(pack) + sizeof(offlineshop::TSubPacketCGShopBuyItem);
	offlineshop::TSubPacketCGShopBuyItem subpack;
	subpack.dwItemID = dwItemID;
	subpack.dwOwnerID = dwOwnerID;
	subpack.bIsSearch = isSearch;

	if (!SendObj(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.bSubHeader);
		return;
	}

	if (!SendObj(subpack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - subpack ", pack.bSubHeader);
		return;
	}
}

void CPythonNetworkStream::SendOfflineshopAddItem(offlineshop::TShopItemInfo& itemInfo)
{
	TPacketCGNewOfflineShop pack;
	pack.bHeader = HEADER_CG_NEW_OFFLINESHOP;
	pack.bSubHeader = offlineshop::SUBHEADER_CG_SHOP_ADD_ITEM;
	pack.wSize = sizeof(pack) + sizeof(offlineshop::TSubPacketCGAddItem);
	offlineshop::TSubPacketCGAddItem subpack;
	offlineshop::CopyObject(subpack.pos, itemInfo.pos);
	offlineshop::CopyObject(subpack.price, itemInfo.price);
#ifdef ENABLE_OFFLINE_SHOP_GRID
	offlineshop::CopyObject(subpack.display_pos, itemInfo.display_pos);
#endif
	if (!SendObj(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.bSubHeader);
		return;
	}

	if (!SendObj(subpack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - subpack ", pack.bSubHeader);
		return;
	}
}

void CPythonNetworkStream::SendOfflineshopRemoveItem(DWORD dwItemID)
{
	TPacketCGNewOfflineShop pack;
	pack.bHeader = HEADER_CG_NEW_OFFLINESHOP;
	pack.bSubHeader = offlineshop::SUBHEADER_CG_SHOP_REMOVE_ITEM;
	pack.wSize = sizeof(pack) + sizeof(offlineshop::TSubPacketCGRemoveItem);
	offlineshop::TSubPacketCGRemoveItem subpack;
	subpack.dwItemID = dwItemID;
	if (!SendObj(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.bSubHeader);
		return;
	}

	if (!SendObj(subpack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - subpack ", pack.bSubHeader);
		return;
	}
}

void CPythonNetworkStream::SendOfflineShopEditItem(DWORD dwItemID, const offlineshop::TPriceInfo& price, bool allEdit)
{
	TPacketCGNewOfflineShop pack;
	pack.bHeader = HEADER_CG_NEW_OFFLINESHOP;
	pack.bSubHeader = offlineshop::SUBHEADER_CG_SHOP_EDIT_ITEM;
	pack.wSize = sizeof(pack) + sizeof(offlineshop::TSubPacketCGEditItem);
	offlineshop::TSubPacketCGEditItem subpack;
	subpack.dwItemID = dwItemID;
	offlineshop::CopyObject(subpack.price, price);
	subpack.allEdit = allEdit;

	if (!SendObj(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.bSubHeader);
		return;
	}

	if (!SendObj(subpack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - subpack ", pack.bSubHeader);
		return;
	}
}

void CPythonNetworkStream::SendOfflineshopFilterRequest(const offlineshop::TFilterInfo& filter)
{
	TPacketCGNewOfflineShop pack;
	pack.bHeader = HEADER_CG_NEW_OFFLINESHOP;
	pack.bSubHeader = offlineshop::SUBHEADER_CG_SHOP_FILTER_REQUEST;
	pack.wSize = sizeof(pack) + sizeof(offlineshop::TSubPacketCGFilterRequest);
	offlineshop::TSubPacketCGFilterRequest subpack;
	offlineshop::CopyObject(subpack.filter, filter);

	if (!SendObj(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.bSubHeader);
		return;
	}

	if (!SendObj(subpack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - subpack ", pack.bSubHeader);
		return;
	}
}

void CPythonNetworkStream::SendOfflineshopSafeboxOpen()
{
	TPacketCGNewOfflineShop pack;
	pack.bHeader = HEADER_CG_NEW_OFFLINESHOP;
	pack.bSubHeader = offlineshop::SUBHEADER_CG_SHOP_SAFEBOX_OPEN;
	pack.wSize = sizeof(pack);

	if (!SendObj(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.bSubHeader);
		return;
	}
}

void CPythonNetworkStream::SendOfflineshopSafeboxGetItem(DWORD dwItemID)
{
	TPacketCGNewOfflineShop pack;
	pack.bHeader = HEADER_CG_NEW_OFFLINESHOP;
	pack.bSubHeader = offlineshop::SUBHEADER_CG_SHOP_SAFEBOX_GET_ITEM;
	offlineshop::TSubPacketCGShopSafeboxGetItem subpack;
	subpack.dwItemID = dwItemID;

	if (!SendObj(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.bSubHeader);
		return;
	}

	if (!SendObj(subpack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - subpack ", pack.bSubHeader);
		return;
	}
}

void CPythonNetworkStream::SendOfflineshopSafeboxGetValutes(const unsigned long long& valutes)
{
	TPacketCGNewOfflineShop pack;
	pack.bHeader = HEADER_CG_NEW_OFFLINESHOP;
	pack.bSubHeader = offlineshop::SUBHEADER_CG_SHOP_SAFEBOX_GET_VALUTES;
	pack.wSize = sizeof(pack) + sizeof(offlineshop::TSubPacketCGShopSafeboxGetValutes);
	offlineshop::TSubPacketCGShopSafeboxGetValutes subpack;
	offlineshop::CopyObject(subpack.gold, valutes);

	if (!SendObj(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.bSubHeader);
		return;
	}

	if (!SendObj(subpack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - subpack ", pack.bSubHeader);
		return;
	}
}

void CPythonNetworkStream::SendOfflineshopSafeboxClose()
{
	TPacketCGNewOfflineShop pack;
	pack.bHeader = HEADER_CG_NEW_OFFLINESHOP;
	pack.bSubHeader = offlineshop::SUBHEADER_CG_SHOP_SAFEBOX_CLOSE;
	pack.wSize = sizeof(pack);

	if (!SendObj(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.bSubHeader);
		return;
	}
}

#ifdef ENABLE_OFFLINESHOP_NOTIFICATION
bool CPythonNetworkStream::RecvOfflineshopNotification()
{
	offlineshop::TSubPacketGCNotification subpack;

	if (!Recv(sizeof(subpack), &subpack))
		return false;
	CPythonOfflineshop::Instance().SendNotification(subpack.dwItemID, subpack.dwItemPrice, subpack.dwItemCount);
	return true;
}
#endif

#ifdef ENABLE_AVERAGE_PRICE
bool CPythonNetworkStream::RecvOfflineshopAveragePrice()
{
	offlineshop::TSubPacketAveragePrice subpack;

	if (!Recv(sizeof(subpack), &subpack))
		return false;
	
	COfflineShopAveragePrice& avg = COfflineShopAveragePrice().Instance();
	avg.SetAveragePrice(subpack.vnum, subpack.price);

	CPythonOfflineshop::Instance().SetAveragePrice(subpack.price);
	return true;
}
#endif

void CPythonNetworkStream::SendOfflineshopCloseBoard()
{
	if (m_gamePhase != GAME_PHASE_GAME)
		return;

	TPacketCGNewOfflineShop pack;
	pack.bHeader = HEADER_CG_NEW_OFFLINESHOP;
	pack.bSubHeader = offlineshop::SUBHEADER_CG_CLOSE_BOARD;
	pack.wSize = sizeof(pack);

	if (!SendObj(pack))
	{
		TraceError("CANNOT SEND OFFLINESHOP PACKET : SUBHEADER %d - pack", pack.bSubHeader);
		return;
	}
}
#endif