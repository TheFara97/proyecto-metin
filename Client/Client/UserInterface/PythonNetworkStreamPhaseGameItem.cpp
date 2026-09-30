#include "StdAfx.h"
#include "PythonNetworkStream.h"
#include "PythonItem.h"
#include "PythonShop.h"
#include "PythonExchange.h"
#include "PythonSafeBox.h"
#include "PythonCharacterManager.h"
#include "PythonSystem.h"

#include "AbstractPlayer.h"

//////////////////////////////////////////////////////////////////////////
// SafeBox

bool CPythonNetworkStream::SendSafeBoxMoneyPacket(BYTE byState, DWORD dwMoney)
{
	assert(!"CPythonNetworkStream::SendSafeBoxMoneyPacket - Don't use this function");
	return false;

	//	TPacketCGSafeboxMoney kSafeboxMoney;
	//	kSafeboxMoney.bHeader = HEADER_CG_SAFEBOX_MONEY;
	//	kSafeboxMoney.bState = byState;
	//	kSafeboxMoney.dwMoney = dwMoney;
	//	if (!Send(sizeof(kSafeboxMoney), &kSafeboxMoney))
	//		return false;
	//
	//	return SendSequence();
}

bool CPythonNetworkStream::SendSafeBoxCheckinPacket(TItemPos InventoryPos, BYTE bySafeBoxPos)
{
	__PlayInventoryItemDropSound(InventoryPos);

	TPacketCGSafeboxCheckin kSafeboxCheckin;
	kSafeboxCheckin.bHeader = HEADER_CG_SAFEBOX_CHECKIN;
	kSafeboxCheckin.ItemPos = InventoryPos;
	kSafeboxCheckin.bSafePos = bySafeBoxPos;
	if (!Send(sizeof(kSafeboxCheckin), &kSafeboxCheckin))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendSafeBoxCheckoutPacket(BYTE bySafeBoxPos, TItemPos InventoryPos)
{
	__PlaySafeBoxItemDropSound(bySafeBoxPos);

	TPacketCGSafeboxCheckout kSafeboxCheckout;
	kSafeboxCheckout.bHeader = HEADER_CG_SAFEBOX_CHECKOUT;
	kSafeboxCheckout.bSafePos = bySafeBoxPos;
	kSafeboxCheckout.ItemPos = InventoryPos;
	if (!Send(sizeof(kSafeboxCheckout), &kSafeboxCheckout))
		return false;

	return SendSequence();
}

#ifdef ENABLE_EXTENDED_ITEM_COUNT
bool CPythonNetworkStream::SendSafeBoxItemMovePacket(BYTE bySourcePos, BYTE byTargetPos, uint16_t byCount)
#else
bool CPythonNetworkStream::SendSafeBoxItemMovePacket(BYTE bySourcePos, BYTE byTargetPos, BYTE byCount)
#endif
{
	__PlaySafeBoxItemDropSound(bySourcePos);

	TPacketCGItemMove kItemMove;
	kItemMove.header = HEADER_CG_SAFEBOX_ITEM_MOVE;
	kItemMove.pos = TItemPos(INVENTORY, bySourcePos);
	kItemMove.num = byCount;
	kItemMove.change_pos = TItemPos(INVENTORY, byTargetPos);
	if (!Send(sizeof(kItemMove), &kItemMove))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvSafeBoxSetPacket()
{
	TPacketGCItemSet2 kItemSet;
	if (!Recv(sizeof(kItemSet), &kItemSet))
		return false;

	TItemData kItemData;
	kItemData.vnum = kItemSet.vnum;
	kItemData.count = kItemSet.count;
	kItemData.flags = kItemSet.flags;
	kItemData.anti_flags = kItemSet.anti_flags;
	for (int isocket = 0; isocket < ITEM_SOCKET_SLOT_MAX_NUM; ++isocket)
		kItemData.alSockets[isocket] = kItemSet.alSockets[isocket];
	for (int iattr = 0; iattr < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++iattr)
		kItemData.aAttr[iattr] = kItemSet.aAttr[iattr];

	CPythonSafeBox::Instance().SetItemData(kItemSet.Cell.cell, kItemData);

	__RefreshSafeboxWindow();

	return true;
}

bool CPythonNetworkStream::RecvSafeBoxDelPacket()
{
	TPacketGCItemDel kItemDel;
	if (!Recv(sizeof(kItemDel), &kItemDel))
		return false;

	CPythonSafeBox::Instance().DelItemData(kItemDel.pos);

	__RefreshSafeboxWindow();

	return true;
}

bool CPythonNetworkStream::RecvSafeBoxWrongPasswordPacket()
{
	TPacketGCSafeboxWrongPassword kSafeboxWrongPassword;

	if (!Recv(sizeof(kSafeboxWrongPassword), &kSafeboxWrongPassword))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnSafeBoxError", Py_BuildValue("()"));

	return true;
}

bool CPythonNetworkStream::RecvSafeBoxSizePacket()
{
	TPacketGCSafeboxSize kSafeBoxSize;
	if (!Recv(sizeof(kSafeBoxSize), &kSafeBoxSize))
		return false;

	CPythonSafeBox::Instance().OpenSafeBox(kSafeBoxSize.bSize);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenSafeboxWindow", Py_BuildValue("(i)", kSafeBoxSize.bSize));

	return true;
}

bool CPythonNetworkStream::RecvSafeBoxMoneyChangePacket()
{
	TPacketGCSafeboxMoneyChange kMoneyChange;
	if (!Recv(sizeof(kMoneyChange), &kMoneyChange))
		return false;

	CPythonSafeBox::Instance().SetMoney(kMoneyChange.dwMoney);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSafeboxMoney", Py_BuildValue("()"));

	return true;
}

// SafeBox
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Mall
bool CPythonNetworkStream::SendMallCheckoutPacket(BYTE byMallPos, TItemPos InventoryPos)
{
	__PlayMallItemDropSound(byMallPos);

	TPacketCGMallCheckout kMallCheckoutPacket;
	kMallCheckoutPacket.bHeader = HEADER_CG_MALL_CHECKOUT;
	kMallCheckoutPacket.bMallPos = byMallPos;
	kMallCheckoutPacket.ItemPos = InventoryPos;
	if (!Send(sizeof(kMallCheckoutPacket), &kMallCheckoutPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvMallOpenPacket()
{
	TPacketGCMallOpen kMallOpen;
	if (!Recv(sizeof(kMallOpen), &kMallOpen))
		return false;

	CPythonSafeBox::Instance().OpenMall(kMallOpen.bSize);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenMallWindow", Py_BuildValue("(i)", kMallOpen.bSize));

	return true;
}
bool CPythonNetworkStream::RecvMallItemSetPacket()
{
	TPacketGCItemSet2 kItemSet;
	if (!Recv(sizeof(kItemSet), &kItemSet))
		return false;

	TItemData kItemData;
	kItemData.vnum = kItemSet.vnum;
	kItemData.count = kItemSet.count;
	kItemData.flags = kItemSet.flags;
	kItemData.anti_flags = kItemSet.anti_flags;
	for (int isocket = 0; isocket < ITEM_SOCKET_SLOT_MAX_NUM; ++isocket)
		kItemData.alSockets[isocket] = kItemSet.alSockets[isocket];
	for (int iattr = 0; iattr < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++iattr)
		kItemData.aAttr[iattr] = kItemSet.aAttr[iattr];

	CPythonSafeBox::Instance().SetMallItemData(kItemSet.Cell.cell, kItemData);

	__RefreshMallWindow();

	return true;
}
bool CPythonNetworkStream::RecvMallItemDelPacket()
{
	TPacketGCItemDel kItemDel;
	if (!Recv(sizeof(kItemDel), &kItemDel))
		return false;

	CPythonSafeBox::Instance().DelMallItemData(kItemDel.pos);

	__RefreshMallWindow();
	Tracef(" >> CPythonNetworkStream::RecvMallItemDelPacket\n");

	return true;
}
// Mall
//////////////////////////////////////////////////////////////////////////

// Item
// Recieve
bool CPythonNetworkStream::RecvItemSetPacket()
{
	TPacketGCItemSet packet_item_set;

	if (!Recv(sizeof(TPacketGCItemSet), &packet_item_set))
		return false;

	TItemData kItemData;
	kItemData.vnum = packet_item_set.vnum;
	kItemData.count = packet_item_set.count;
	kItemData.flags = 0;
	for (int i = 0; i < ITEM_SOCKET_SLOT_MAX_NUM; ++i)
		kItemData.alSockets[i] = packet_item_set.alSockets[i];
	for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
		kItemData.aAttr[j] = packet_item_set.aAttr[j];


	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();

	rkPlayer.SetItemData(packet_item_set.Cell, kItemData);
#ifdef ENABLE_SWITCHBOT
	if (packet_item_set.Cell.window_type == SWITCHBOT)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSwitchbotWindow", Py_BuildValue("()"));
		return true;
	}
#endif
	__RefreshInventoryWindow();
	return true;
}

bool CPythonNetworkStream::RecvItemSetPacket2()
{
	TPacketGCItemSet2 packet_item_set;

	if (!Recv(sizeof(TPacketGCItemSet2), &packet_item_set))
		return false;

	TItemData kItemData;
	kItemData.vnum = packet_item_set.vnum;
	kItemData.count = packet_item_set.count;
	kItemData.flags = packet_item_set.flags;
	kItemData.anti_flags = packet_item_set.anti_flags;

	for (int i = 0; i < ITEM_SOCKET_SLOT_MAX_NUM; ++i)
		kItemData.alSockets[i] = packet_item_set.alSockets[i];
	for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
		kItemData.aAttr[j] = packet_item_set.aAttr[j];

	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	rkPlayer.SetItemData(packet_item_set.Cell, kItemData);
#ifdef ENABLE_SWITCHBOT
	if (packet_item_set.Cell.window_type == SWITCHBOT)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSwitchbotWindow", Py_BuildValue("()"));
		return true;
	}
#endif
	if (packet_item_set.highlight)
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Highlight_Item", Py_BuildValue("(ii)", packet_item_set.Cell.window_type, packet_item_set.Cell.cell));

	__RefreshInventoryWindow();
	return true;
}

bool CPythonNetworkStream::RecvItemUsePacket()
{
	TPacketGCItemUse packet_item_use;

	if (!Recv(sizeof(TPacketGCItemUse), &packet_item_use))
		return false;

	__RefreshInventoryWindow();
	return true;
}

bool CPythonNetworkStream::RecvItemUpdatePacket()
{
	TPacketGCItemUpdate packet_item_update;

	if (!Recv(sizeof(TPacketGCItemUpdate), &packet_item_update))
		return false;

	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	rkPlayer.SetItemCount(packet_item_update.Cell, packet_item_update.count);
	for (int i = 0; i < ITEM_SOCKET_SLOT_MAX_NUM; ++i)
		rkPlayer.SetItemMetinSocket(packet_item_update.Cell, i, packet_item_update.alSockets[i]);
	for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
		rkPlayer.SetItemAttribute(packet_item_update.Cell, j, packet_item_update.aAttr[j].bType, packet_item_update.aAttr[j].sValue);


	__RefreshInventoryWindow();
	return true;
}

bool CPythonNetworkStream::RecvItemGroundAddPacket()
{
	TPacketGCItemGroundAdd recv;
	if (!Recv(sizeof(TPacketGCItemGroundAdd), &recv))
		return false;

	__GlobalPositionToLocalPosition(recv.lX, recv.lY);

#ifdef ENABLE_EXTENDED_ITEMNAME_ON_GROUND
	CPythonItem::Instance().CreateItem(recv.dwVID, recv.dwVnum, recv.lX, recv.lY, recv.lZ, recv.count, true, recv.alSockets, recv.aAttrs);
#else
	CPythonItem::Instance().CreateItem(recv.dwVID, recv.dwVnum, recv.lX, recv.lY, recv.lZ, recv.count);
#endif
	return true;
}

bool CPythonNetworkStream::RecvItemOwnership()
{
	TPacketGCItemOwnership p;

	if (!Recv(sizeof(TPacketGCItemOwnership), &p))
		return false;

	CPythonItem::Instance().SetOwnership(p.dwVID, p.szName);
	return true;
}

bool CPythonNetworkStream::RecvItemGroundDelPacket()
{
	TPacketGCItemGroundDel	packet_item_ground_del;

	if (!Recv(sizeof(TPacketGCItemGroundDel), &packet_item_ground_del))
		return false;

	CPythonItem::Instance().DeleteItem(packet_item_ground_del.vid);
	return true;
}

bool CPythonNetworkStream::RecvQuickSlotAddPacket()
{
	TPacketGCQuickSlotAdd packet_quick_slot_add;

	if (!Recv(sizeof(TPacketGCQuickSlotAdd), &packet_quick_slot_add))
		return false;

	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	rkPlayer.AddQuickSlot(packet_quick_slot_add.pos, packet_quick_slot_add.slot.Type, packet_quick_slot_add.slot.Position);

	__RefreshInventoryWindow();

	return true;
}

bool CPythonNetworkStream::RecvQuickSlotDelPacket()
{
	TPacketGCQuickSlotDel packet_quick_slot_del;

	if (!Recv(sizeof(TPacketGCQuickSlotDel), &packet_quick_slot_del))
		return false;

	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	rkPlayer.DeleteQuickSlot(packet_quick_slot_del.pos);

	__RefreshInventoryWindow();

	return true;
}

bool CPythonNetworkStream::RecvQuickSlotMovePacket()
{
	TPacketGCQuickSlotSwap packet_quick_slot_swap;

	if (!Recv(sizeof(TPacketGCQuickSlotSwap), &packet_quick_slot_swap))
		return false;

	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	rkPlayer.MoveQuickSlot(packet_quick_slot_swap.pos, packet_quick_slot_swap.change_pos);

	__RefreshInventoryWindow();

	return true;
}

bool CPythonNetworkStream::SendShopEndPacket()
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGShop packet_shop;
	packet_shop.header = HEADER_CG_SHOP;
	packet_shop.subheader = SHOP_SUBHEADER_CG_END;

	if (!Send(sizeof(packet_shop), &packet_shop))
	{
		Tracef("SendShopEndPacket Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendShopBuyPacket(BYTE bPos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_BUY;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendShopBuyPacket Error\n");
		return false;
	}

	BYTE bCount = 1;
	if (!Send(sizeof(BYTE), &bCount))
	{
		Tracef("SendShopBuyPacket Error\n");
		return false;
	}

	if (!Send(sizeof(BYTE), &bPos))
	{
		Tracef("SendShopBuyPacket Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendShopSellPacket(BYTE bySlot)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_SELL;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendShopSellPacket Error\n");
		return false;
	}
	if (!Send(sizeof(BYTE), &bySlot))
	{
		Tracef("SendShopAddSellPacket Error\n");
		return false;
	}

	return SendSequence();
}


#ifdef ENABLE_EXTENDED_ITEM_COUNT
bool CPythonNetworkStream::SendShopSellPacketNew(WORD wSlot, uint16_t byCount, BYTE byType)
#else
bool CPythonNetworkStream::SendShopSellPacketNew(BYTE bySlot, BYTE byCount)
#endif
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_SELL2;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendShopSellPacket Error\n");
		return false;
	}
	if (!Send(sizeof(WORD), &wSlot))
	{
		Tracef("SendShopAddSellPacket Error\n");
		return false;
	}
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	if (!Send(sizeof(uint16_t), &byCount))
#else
	if (!Send(sizeof(BYTE), &byCount))
#endif
	{
		Tracef("SendShopAddSellPacket Error\n");
		return false;
	}

#ifdef WJ_SPLIT_INVENTORY_SYSTEM
	if (!Send(sizeof(BYTE), &byType))
	{
		Tracef("SendShopAddSellPacket Error\n");
		return false;
	}
#endif
	Tracef(" SendShopSellPacketNew(bySlot=%d, byCount=%d)\n", wSlot, byCount);

	return SendSequence();
}


#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
bool CPythonNetworkStream::SendOfflineShopEndPacket()
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGShop packet_shop;
	packet_shop.header = HEADER_CG_OFFLINE_SHOP;
	packet_shop.subheader = SHOP_SUBHEADER_CG_END;

	if (!Send(sizeof(packet_shop), &packet_shop))
	{
		Tracef("SendOfflineShopEndPacket Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendOfflineShopBuyPacket(BYTE bPos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_OFFLINE_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_BUY;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendOfflineShopBuyPacket Error\n");
		return false;
	}

	BYTE bCount = 1;
	if (!Send(sizeof(BYTE), &bCount))
	{
		Tracef("SendOfflineShopBuyPacket Error\n");
		return false;
	}

	if (!Send(sizeof(BYTE), &bPos))
	{
		Tracef("SendOfflineShopBuyPacket Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendAddOfflineShopItem(BYTE bDisplayPos, TItemPos itemPos, long long lPrice, long long lPrice2)
{
	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_OFFLINE_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_ADD_ITEM;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendAddOfflineShopItem Error\n");
		return false;
	}

	TOfflineShopItemTable pTable;

	pTable.bDisplayPos = bDisplayPos;
	pTable.itemPos = itemPos;
	pTable.lPrice = lPrice;
	pTable.lPrice2 = lPrice2;

	if (!Send(sizeof(TOfflineShopItemTable), &pTable))
	{
		Tracef("SendAddOfflineShopItem Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendRemoveOfflineShopItem(BYTE bPos)
{
	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_OFFLINE_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_REMOVE_ITEM;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendRemoveOfflineShopItem Error\n");
		return false;
	}

	if (!Send(sizeof(BYTE), &bPos))
	{
		Tracef("SendRemoveOfflineShopItem Packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendDestroyOfflineShop()
{
	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_OFFLINE_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_DESTROY_OFFLINE_SHOP;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendDestroyOfflineShop Packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendRefreshOfflineShop()
{
	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_OFFLINE_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_REFRESH;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendRefreshOfflineShop Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendRefreshOfflineShopMoney()
{
	static DWORD s_LastTime = timeGetTime() - 5001;

	if (timeGetTime() - s_LastTime < 5000)
		return true;

	s_LastTime = timeGetTime();

	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_OFFLINE_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_REFRESH_MONEY;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendRefreshOfflineShopMoney Packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendRefreshOfflineShopChequeMoney()
{
	static DWORD s_LastTime = timeGetTime() - 5001;

	if (timeGetTime() - s_LastTime < 5000)
		return true;

	s_LastTime = timeGetTime();

	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_OFFLINE_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_REFRESH_CHEQUE;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendRefreshOfflineShopCheque Packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendOfflineShopWithdrawMoney(long long dwMoney)
{
	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_OFFLINE_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_WITHDRAW_MONEY;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendOfflineShopWithdrawMoney Packet Error\n");
		return false;
	}

	if (!Send(sizeof(dwMoney), &dwMoney))
	{
		Tracef("SendOfflineShopWithdrawMoney Packet Error \n");
		return false;
	}

	return SendSequence();
}
bool CPythonNetworkStream::SendOfflineShopWithdrawCheque(long long dwCheque)
{
	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_OFFLINE_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_WITHDRAW_CHEQUE;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendOfflineShopWithdrawCheque Packet Error\n");
		return false;
	}

	if (!Send(sizeof(dwCheque), &dwCheque))
	{
		Tracef("SendOfflineShopWithdrawCheque Packet Error \n");
		return false;
	}

	return SendSequence();
}
#endif


// Send
bool CPythonNetworkStream::SendItemUsePacket(TItemPos pos)
{
	if (!__CanActMainInstance())
		return true;

	if (__IsEquipItemInSlot(pos))
	{
		if (CPythonExchange::Instance().isTrading())
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_EQUIP_EXCHANGE"));
			return true;
		}

		if (CPythonShop::Instance().IsOpen())
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_EQUIP_SHOP"));
			return true;
		}

		if (__IsPlayerAttacking())
			return true;
	}

	__PlayInventoryItemUseSound(pos);

	TPacketCGItemUse itemUsePacket;
	itemUsePacket.header = HEADER_CG_ITEM_USE;
	itemUsePacket.pos = pos;

	if (!Send(sizeof(TPacketCGItemUse), &itemUsePacket))
	{
		Tracen("SendItemUsePacket Error");
		return false;
	}

	return SendSequence();
}

#ifdef ENABLE_INSTANT_CHEST_OPEN
bool CPythonNetworkStream::SendOpenChestPacket(TItemPos pos, WORD wOpenCount)
{
	if (!__CanActMainInstance())
		return true;
	__PlayInventoryItemUseSound(pos);

	TPacketCGOpenChest chestUsePacket;
	chestUsePacket.header = HEADER_CG_CHEST_OPEN;
	chestUsePacket.pos = pos;
	chestUsePacket.wOpenCount = wOpenCount;

	if (!Send(sizeof(TPacketCGOpenChest), &chestUsePacket))
	{
		Tracen("SendOpenChestPacket Error");
		return false;
	}

	return SendSequence();
}
#endif

#ifdef ENABLE_INSTANT_INGOT_OPEN
bool CPythonNetworkStream::SendOpenIngotPacket(TItemPos pos, WORD wOpenCount)
{
	if (!__CanActMainInstance())
		return true;
	__PlayInventoryItemUseSound(pos);

	TPacketCGOpenIngot ingotUsePacket;
	ingotUsePacket.header = HEADER_CG_INGOT_OPEN;
	ingotUsePacket.pos = pos;
	ingotUsePacket.wOpenCount = wOpenCount;

	TraceError("test");

	if (!Send(sizeof(TPacketCGOpenIngot), &ingotUsePacket))
	{
		Tracen("SendOpenIngotPacket Error");
		return false;
	}

	return SendSequence();
}
#endif

bool CPythonNetworkStream::SendItemUseToItemPacket(TItemPos source_pos, TItemPos target_pos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGItemUseToItem itemUseToItemPacket;
	itemUseToItemPacket.header = HEADER_CG_ITEM_USE_TO_ITEM;
	itemUseToItemPacket.source_pos = source_pos;
	itemUseToItemPacket.target_pos = target_pos;

	if (!Send(sizeof(TPacketCGItemUseToItem), &itemUseToItemPacket))
	{
		Tracen("SendItemUseToItemPacket Error");
		return false;
	}

#ifdef _DEBUG
	Tracef(" << SendItemUseToItemPacket(src=%d, dst=%d)\n", source_pos, target_pos);
#endif

	return SendSequence();
}

bool CPythonNetworkStream::SendItemDropPacket(TItemPos pos, DWORD elk)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGItemDrop itemDropPacket;
	itemDropPacket.header = HEADER_CG_ITEM_DROP;
	itemDropPacket.pos = pos;
	itemDropPacket.elk = elk;

	if (!Send(sizeof(TPacketCGItemDrop), &itemDropPacket))
	{
		Tracen("SendItemDropPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendItemDropPacketNew(TItemPos pos, DWORD elk, DWORD count)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGItemDrop2 itemDropPacket;
	itemDropPacket.header = HEADER_CG_ITEM_DROP2;
	itemDropPacket.pos = pos;
	itemDropPacket.gold = elk;
	itemDropPacket.count = count;

	if (!Send(sizeof(itemDropPacket), &itemDropPacket))
	{
		Tracen("SendItemDropPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::__IsEquipItemInSlot(TItemPos uSlotPos)
{
	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	return rkPlayer.IsEquipItemInSlot(uSlotPos);
}

void CPythonNetworkStream::__PlayInventoryItemUseSound(TItemPos uSlotPos)
{
	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	DWORD dwItemID = rkPlayer.GetItemIndex(uSlotPos);

	CPythonItem& rkItem = CPythonItem::Instance();
	rkItem.PlayUseSound(dwItemID);
}

void CPythonNetworkStream::__PlayInventoryItemDropSound(TItemPos uSlotPos)
{
	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	DWORD dwItemID = rkPlayer.GetItemIndex(uSlotPos);

	CPythonItem& rkItem = CPythonItem::Instance();
	rkItem.PlayDropSound(dwItemID);
}

//void CPythonNetworkStream::__PlayShopItemDropSound(UINT uSlotPos)
//{
//	DWORD dwItemID;
//	CPythonShop& rkShop=CPythonShop::Instance();
//	if (!rkShop.GetSlotItemID(uSlotPos, &dwItemID))
//		return;
//
//	CPythonItem& rkItem=CPythonItem::Instance();
//	rkItem.PlayDropSound(dwItemID);
//}

void CPythonNetworkStream::__PlaySafeBoxItemDropSound(UINT uSlotPos)
{
	DWORD dwItemID;
	CPythonSafeBox& rkSafeBox = CPythonSafeBox::Instance();
	if (!rkSafeBox.GetSlotItemID(uSlotPos, &dwItemID))
		return;

	CPythonItem& rkItem = CPythonItem::Instance();
	rkItem.PlayDropSound(dwItemID);
}

void CPythonNetworkStream::__PlayMallItemDropSound(UINT uSlotPos)
{
	DWORD dwItemID;
	CPythonSafeBox& rkSafeBox = CPythonSafeBox::Instance();
	if (!rkSafeBox.GetSlotMallItemID(uSlotPos, &dwItemID))
		return;

	CPythonItem& rkItem = CPythonItem::Instance();
	rkItem.PlayDropSound(dwItemID);
}

#ifdef ENABLE_EXTENDED_ITEM_COUNT
bool CPythonNetworkStream::SendItemMovePacket(TItemPos pos, TItemPos change_pos, uint16_t num)
#else
bool CPythonNetworkStream::SendItemMovePacket(TItemPos pos, TItemPos change_pos, BYTE num)
#endif
{
	if (!__CanActMainInstance())
		return true;

	if (__IsEquipItemInSlot(pos))
	{
		if (CPythonExchange::Instance().isTrading())
		{
			if (pos.IsEquipCell() || change_pos.IsEquipCell())
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_EQUIP_EXCHANGE"));
				return true;
			}
		}

		if (CPythonShop::Instance().IsOpen())
		{
			if (pos.IsEquipCell() || change_pos.IsEquipCell())
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_EQUIP_SHOP"));
				return true;
			}
		}

		if (__IsPlayerAttacking())
			return true;
	}

	__PlayInventoryItemDropSound(pos);

	TPacketCGItemMove	itemMovePacket;
	itemMovePacket.header = HEADER_CG_ITEM_MOVE;
	itemMovePacket.pos = pos;
	itemMovePacket.change_pos = change_pos;
	itemMovePacket.num = num;

	if (!Send(sizeof(TPacketCGItemMove), &itemMovePacket))
	{
		Tracen("SendItemMovePacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendItemPickUpPacket(DWORD vid)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGItemPickUp	itemPickUpPacket;
	itemPickUpPacket.header = HEADER_CG_ITEM_PICKUP;
	itemPickUpPacket.vid = vid;

	if (!Send(sizeof(TPacketCGItemPickUp), &itemPickUpPacket))
	{
		Tracen("SendItemPickUpPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendFastStackPacket(TItemPos itemPos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGFastStack fastStackPacket;
	fastStackPacket.header = HEADER_CG_FAST_STACK;
	fastStackPacket.pos = itemPos;

	if (!Send(sizeof(TPacketCGFastStack), &fastStackPacket))
	{
		Tracen("SendFastStackPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendQuickSlotAddPacket(BYTE wpos, BYTE type, BYTE pos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGQuickSlotAdd quickSlotAddPacket;

	quickSlotAddPacket.header = HEADER_CG_QUICKSLOT_ADD;
	quickSlotAddPacket.pos = wpos;
	quickSlotAddPacket.slot.Type = type;
	quickSlotAddPacket.slot.Position = pos;

	if (!Send(sizeof(TPacketCGQuickSlotAdd), &quickSlotAddPacket))
	{
		Tracen("SendQuickSlotAddPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendQuickSlotDelPacket(BYTE pos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGQuickSlotDel quickSlotDelPacket;

	quickSlotDelPacket.header = HEADER_CG_QUICKSLOT_DEL;
	quickSlotDelPacket.pos = pos;

	if (!Send(sizeof(TPacketCGQuickSlotDel), &quickSlotDelPacket))
	{
		Tracen("SendQuickSlotDelPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendQuickSlotMovePacket(BYTE pos, BYTE change_pos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGQuickSlotSwap quickSlotSwapPacket;

	quickSlotSwapPacket.header = HEADER_CG_QUICKSLOT_SWAP;
	quickSlotSwapPacket.pos = pos;
	quickSlotSwapPacket.change_pos = change_pos;

	if (!Send(sizeof(TPacketCGQuickSlotSwap), &quickSlotSwapPacket))
	{
		Tracen("SendQuickSlotSwapPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvSpecialEffect()
{
	TPacketGCSpecialEffect kSpecialEffect;
	if (!Recv(sizeof(kSpecialEffect), &kSpecialEffect))
		return false;

	DWORD effect = -1;
	bool bPlayPotionSound = false;
	bool bAttachEffect = true;
	switch (kSpecialEffect.type)
	{
	case SE_HPUP_RED:
		effect = CInstanceBase::EFFECT_HPUP_RED;
		bPlayPotionSound = true;
		break;
	case SE_SPUP_BLUE:
		effect = CInstanceBase::EFFECT_SPUP_BLUE;
		bPlayPotionSound = true;
		break;
	case SE_SPEEDUP_GREEN:
		effect = CInstanceBase::EFFECT_SPEEDUP_GREEN;
		bPlayPotionSound = true;
		break;
	case SE_DXUP_PURPLE:
		effect = CInstanceBase::EFFECT_DXUP_PURPLE;
		bPlayPotionSound = true;
		break;
	case SE_CRITICAL:
		effect = CInstanceBase::EFFECT_CRITICAL;
		break;
	case SE_PENETRATE:
		effect = CInstanceBase::EFFECT_PENETRATE;
		break;
	case SE_BLOCK:
		effect = CInstanceBase::EFFECT_BLOCK;
		break;
	case SE_DODGE:
		effect = CInstanceBase::EFFECT_DODGE;
		break;
	case SE_CHINA_FIREWORK:
		effect = CInstanceBase::EFFECT_FIRECRACKER;
		bAttachEffect = false;
		break;
	case SE_SPIN_TOP:
		effect = CInstanceBase::EFFECT_SPIN_TOP;
		bAttachEffect = false;
		break;
	case SE_SUCCESS:
		effect = CInstanceBase::EFFECT_SUCCESS;
		bAttachEffect = false;
		break;
	case SE_FAIL:
		effect = CInstanceBase::EFFECT_FAIL;
		break;
	case SE_FR_SUCCESS:
		effect = CInstanceBase::EFFECT_FR_SUCCESS;
		bAttachEffect = false;
		break;
	case SE_LEVELUP_ON_14_FOR_GERMANY:
		effect = CInstanceBase::EFFECT_LEVELUP_ON_14_FOR_GERMANY;
		bAttachEffect = false;
		break;
	case SE_LEVELUP_UNDER_15_FOR_GERMANY:
		effect = CInstanceBase::EFFECT_LEVELUP_UNDER_15_FOR_GERMANY;
		bAttachEffect = false;
		break;
	case SE_PERCENT_DAMAGE1:
		effect = CInstanceBase::EFFECT_PERCENT_DAMAGE1;
		break;
	case SE_PERCENT_DAMAGE2:
		effect = CInstanceBase::EFFECT_PERCENT_DAMAGE2;
		break;
	case SE_PERCENT_DAMAGE3:
		effect = CInstanceBase::EFFECT_PERCENT_DAMAGE3;
		break;
	case SE_AUTO_HPUP:
		effect = CInstanceBase::EFFECT_AUTO_HPUP;
		break;
	case SE_AUTO_SPUP:
		effect = CInstanceBase::EFFECT_AUTO_SPUP;
		break;
	case SE_EQUIP_RAMADAN_RING:
		effect = CInstanceBase::EFFECT_RAMADAN_RING_EQUIP;
		break;
	case SE_EQUIP_HALLOWEEN_CANDY:
		effect = CInstanceBase::EFFECT_HALLOWEEN_CANDY_EQUIP;
		break;
	case SE_EQUIP_HAPPINESS_RING:
		effect = CInstanceBase::EFFECT_HAPPINESS_RING_EQUIP;
		break;
	case SE_EQUIP_LOVE_PENDANT:
		effect = CInstanceBase::EFFECT_LOVE_PENDANT_EQUIP;
		break;
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	case SE_EFFECT_ACCE_SUCCEDED:
		effect = CInstanceBase::EFFECT_ACCE_SUCCEDED;
		break;
	case SE_EFFECT_ACCE_EQUIP:
		effect = CInstanceBase::EFFECT_ACCE_EQUIP;
		break;
#endif

	case SE_EFFECT_FISH1:
		effect = CInstanceBase::EFFECT_FISH1;
		break;

	case SE_EFFECT_FISH2:
		effect = CInstanceBase::EFFECT_FISH2;
		break;
		
	case SE_EFFECT_FISH3:
		effect = CInstanceBase::EFFECT_FISH3;
		break;
		
	case SE_EFFECT_FISH4:
		effect = CInstanceBase::EFFECT_FISH4;
		break;
#ifdef ENABLE_NEW_PET_SYSTEM
	case SE_PET_LV_UP:
		effect = CInstanceBase::SE_PET_LV_UP;
		break;
#endif
#ifdef ENABLE_SYSTEM_RUNE
	case SE_RUNA_WHITE_EFFECT:
		effect = CInstanceBase::EFFECT_RUNA_WHITE_EQUIP;
		break;
	case SE_RUNA_RED_EFFECT:
		effect = CInstanceBase::EFFECT_RUNA_RED_EQUIP;
		break;
	case SE_RUNA_BLUE_EFFECT:
		effect = CInstanceBase::EFFECT_RUNA_BLUE_EQUIP;
		break;
	case SE_RUNA_YELLOW_EFFECT:
		effect = CInstanceBase::EFFECT_RUNA_YELLOW_EQUIP;
		break;
	case SE_RUNA_BLACK_EFFECT:
		effect = CInstanceBase::EFFECT_RUNA_BLACK_EQUIP;
		break;
	case SE_RUNA_GREEN_EFFECT:
		effect = CInstanceBase::EFFECT_RUNA_GREEN_EQUIP;
		break;
#endif
	default:
		TraceError("%d is not a special effect number. TPacketGCSpecialEffect", kSpecialEffect.type);
		break;
	}

	if (bPlayPotionSound)
	{
		IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
		if (rkPlayer.IsMainCharacterIndex(kSpecialEffect.vid))
		{
			CPythonItem& rkItem = CPythonItem::Instance();
			rkItem.PlayUsePotionSound();
		}
	}

#ifdef ENABLE_GRAPHIC_ON_OFF
	if (-1 != effect && CPythonSystem::Instance().GetEffectLevel() != 4)
	{
		if (CPythonSystem::Instance().GetEffectLevel() == 0 || CPythonSystem::Instance().GetEffectLevel() == 2)
		{
			CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(kSpecialEffect.vid);
			if (pInstance)
			{
				if (bAttachEffect)
					pInstance->AttachSpecialEffect(effect);
				else
					pInstance->CreateSpecialEffect(effect);
			}
		}
		else if (CPythonSystem::Instance().GetEffectLevel() == 1 || CPythonSystem::Instance().GetEffectLevel() == 3)
		{
			IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
			if (rkPlayer.IsMainCharacterIndex(kSpecialEffect.vid))
			{
				CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(kSpecialEffect.vid);
				if (pInstance)
				{
					if (bAttachEffect)
						pInstance->AttachSpecialEffect(effect);
					else
						pInstance->CreateSpecialEffect(effect);
				}
			}
		}
	}
#else
	if (-1 != effect)
	{
		CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(kSpecialEffect.vid);
		if (pInstance)
		{
			if (bAttachEffect)
				pInstance->AttachSpecialEffect(effect);
			else
				pInstance->CreateSpecialEffect(effect);
		}
	}
#endif

	return true;
}

bool CPythonNetworkStream::RecvSpecificEffect()
{
	TPacketGCSpecificEffect kSpecificEffect;
	if (!Recv(sizeof(kSpecificEffect), &kSpecificEffect))
		return false;

	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(kSpecificEffect.vid);
	//EFFECT_TEMP
	if (pInstance)
	{
		CInstanceBase::RegisterEffect(CInstanceBase::EFFECT_TEMP, "", kSpecificEffect.effect_file, false);
		pInstance->AttachSpecialEffect(CInstanceBase::EFFECT_TEMP);
	}

	return true;
}

bool CPythonNetworkStream::RecvDragonSoulRefine()
{
	TPacketGCDragonSoulRefine kDragonSoul;

	if (!Recv(sizeof(kDragonSoul), &kDragonSoul))
		return false;

	switch (kDragonSoul.bSubType)
	{
#if defined(ENABLE_DS_CHANGE_ATTR)
	case DS_SUB_HEADER_OPEN:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DragonSoulRefineWindow_Open", Py_BuildValue("(i)", DS_REFINE_TYPE_GRADE));
		break;

	case DS_SUB_HEADER_OPEN_CHANGE_ATTR:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DragonSoulRefineWindow_Open", Py_BuildValue("(i)", DS_REFINE_TYPE_CHANGE_ATTR));
		break;
#else
	case DS_SUB_HEADER_OPEN:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DragonSoulRefineWindow_Open", Py_BuildValue("()"));
		break;
#endif
	case DS_SUB_HEADER_REFINE_FAIL:
	case DS_SUB_HEADER_REFINE_FAIL_MAX_REFINE:
	case DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL:
	case DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MONEY:
	case DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MATERIAL:
	case DS_SUB_HEADER_REFINE_FAIL_TOO_MUCH_MATERIAL:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DragonSoulRefineWindow_RefineFail", Py_BuildValue("(iii)",
			kDragonSoul.bSubType, kDragonSoul.Pos.window_type, kDragonSoul.Pos.cell));
		break;
	case DS_SUB_HEADER_REFINE_SUCCEED:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DragonSoulRefineWindow_RefineSucceed",
			Py_BuildValue("(ii)", kDragonSoul.Pos.window_type, kDragonSoul.Pos.cell));
		break;
	}

	return true;
}

#ifdef TITLE_SYSTEM_BYLUZER
bool CPythonNetworkStream::SendSelectTitle(int id)
{
	TPacketCGTitleActive p;
	p.bHeader = HEADER_CG_TITLE_ACTIVE;
	p.id = id;

	if (!Send(sizeof(TPacketCGTitleActive), &p))
		return false;

	return SendSequence();
}
#endif

#ifdef WEEKLY_RANK_BYLUZER
bool CPythonNetworkStream::SelectWeeklyRankPage(int id)
{
	TPacketCGSelectWeeklyRank p;
	p.bHeader = HEADER_CG_WEEKLY_RANK;
	p.page = id;

	if (!Send(sizeof(TPacketCGSelectWeeklyRank), &p))
		return false;

	return SendSequence();
}
#endif

//martysama0134's ceqyqttoaf71vasf9t71218
