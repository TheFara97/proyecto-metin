#include "stdafx.h"
#include "../../libgame/include/grid.h"
#include "utils.h"
#include "desc.h"
#include "desc_client.h"
#include "char.h"
#include "item.h"
#include "item_manager.h"
#include "packet.h"
#include "log.h"
#include "db.h"
#include "locale_service.h"
#include "../../common/length.h"
#include "exchange.h"
#include "DragonSoul.h"
#include "questmanager.h" // @fixme150
#ifdef ENABLE_IN_GAME_LOG_SYSTEM
#include "InGameLogManager.h"
#endif
#ifdef ENABLE_MESSENGER_BLOCK
#include "messenger_manager.h"
#endif

#ifdef ENABLE_LONG_LONG
void exchange_packet(LPCHARACTER ch, BYTE sub_header, bool is_me, int64_t arg1, TItemPos arg2, DWORD arg3, void * pvData = NULL);
#else
void exchange_packet(LPCHARACTER ch, BYTE sub_header, bool is_me, DWORD arg1, TItemPos arg2, DWORD arg3, void * pvData = NULL);
#endif


#ifdef ENABLE_LONG_LONG
void exchange_packet(LPCHARACTER ch, BYTE sub_header, bool is_me, int64_t arg1, TItemPos arg2, DWORD arg3, void * pvData)
#else
void exchange_packet(LPCHARACTER ch, BYTE sub_header, bool is_me, DWORD arg1, TItemPos arg2, DWORD arg3, void * pvData)
#endif
{
	if (!ch->GetDesc())
		return;

	struct packet_exchange pack_exchg;
	pack_exchg.header 		= HEADER_GC_EXCHANGE;
	pack_exchg.sub_header 	= sub_header;
	pack_exchg.is_me		= is_me;
	pack_exchg.arg1			= arg1;
	pack_exchg.arg2			= arg2;
	pack_exchg.arg3			= arg3;

	if (sub_header == EXCHANGE_SUBHEADER_GC_ITEM_ADD && pvData)
	{
#ifdef WJ_ENABLE_TRADABLE_ICON
		pack_exchg.arg4 = TItemPos(((LPITEM) pvData)->GetWindow(), ((LPITEM) pvData)->GetCell());
#endif
		thecore_memcpy(&pack_exchg.alSockets, ((LPITEM) pvData)->GetSockets(), sizeof(pack_exchg.alSockets));
		thecore_memcpy(&pack_exchg.aAttr, ((LPITEM) pvData)->GetAttributes(), sizeof(pack_exchg.aAttr));
	}
	else
	{
#ifdef WJ_ENABLE_TRADABLE_ICON
		pack_exchg.arg4 = TItemPos(RESERVED_WINDOW, 0);
#endif
		memset(&pack_exchg.alSockets, 0, sizeof(pack_exchg.alSockets));
		memset(&pack_exchg.aAttr, 0, sizeof(pack_exchg.aAttr));
	}

	ch->GetDesc()->Packet(&pack_exchg, sizeof(pack_exchg));
}

bool CHARACTER::ExchangeStart(LPCHARACTER victim)
{
	if (this == victim)
		return false;

	if (IsObserverMode())
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;893]");
		return false;
	}

	if (victim->IsNPC())
		return false;

	//PREVENT_TRADE_WINDOW
	if ( IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen()
#ifdef __ENABLE_POLYMORPH_SYSTEM__
		|| IsPolyShopping()
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		|| GetOfflineShopOwner()
#endif
#ifdef ENABLE_BOT_CONTROL
		|| IsBotControl()
#endif
	)
	{
		ChatPacket( CHAT_TYPE_INFO, "[LS;894]" );
		return false;
	}

	if ( victim->IsOpenSafebox() || victim->GetShopOwner() || victim->GetMyShop() || victim->IsCubeOpen()
#ifdef __ENABLE_POLYMORPH_SYSTEM__
		|| victim->IsPolyShopping()
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		|| victim->GetOfflineShopOwner()
#endif
#ifdef ENABLE_BOT_CONTROL
		|| victim->IsBotControl()
#endif
	)
	{
		ChatPacket( CHAT_TYPE_INFO, "[LS;895]" );
		return false;
	}
	//END_PREVENT_TRADE_WINDOW
	int iDist = DISTANCE_SQRT(GetX() - victim->GetX(), GetY() - victim->GetY());

	if (iDist >= EXCHANGE_MAX_DISTANCE)
		return false;

	if (GetExchange())
		return false;
	
#ifdef ENABLE_MESSENGER_BLOCK
	if (MessengerManager::instance().IsBlocked(GetName(), victim->GetName()))
	{
		ChatPacket(CHAT_TYPE_INFO, "You need to unblock %s do to that.", victim->GetName());
		return false;
	}
	if (MessengerManager::instance().IsBlocked(victim->GetName(), GetName()))
	{
		ChatPacket(CHAT_TYPE_INFO, "%s has blocked you.", victim->GetName());
		return false;
	}
#endif

	if (victim->GetExchange())
	{
		exchange_packet(this, EXCHANGE_SUBHEADER_GC_ALREADY, 0, 0, NPOS, 0);
		return false;
	}

	if (victim->IsBlockMode(BLOCK_EXCHANGE))
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;896]");
		return false;
	}

	SetExchange(M2_NEW CExchange(this));
	victim->SetExchange(M2_NEW CExchange(victim));

	victim->GetExchange()->SetCompany(GetExchange());
	GetExchange()->SetCompany(victim->GetExchange());

	SetExchangeTime();
	victim->SetExchangeTime();

	exchange_packet(victim, EXCHANGE_SUBHEADER_GC_START, 0, GetVID(), NPOS, 0);
	exchange_packet(this, EXCHANGE_SUBHEADER_GC_START, 0, victim->GetVID(), NPOS, 0);

	return true;
}

CExchange::CExchange(LPCHARACTER pOwner)
{
	m_pCompany = NULL;

	m_bAccept = false;

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		m_apItems[i] = NULL;
		m_aItemPos[i] = NPOS;
		m_abItemDisplayPos[i] = 0;
	}

	m_lGold = 0;
#ifdef ENABLE_CHEQUE_SYSTEM
	m_lCheque = 0;
#endif

	m_pOwner = pOwner;
	pOwner->SetExchange(this);

	m_pGrid = M2_NEW CGrid(6,4);
}

CExchange::~CExchange()
{
	M2_DELETE(m_pGrid);
}

#if defined(ITEM_CHECKINOUT_UPDATE)
int CExchange::GetEmptyExchange(BYTE size)
{
	for (unsigned int i = 0; m_pGrid && i < m_pGrid->GetSize(); i++)
		if (m_pGrid->IsEmpty(i, 1, size))
			return i;

	return -1;
}
bool CExchange::AddItem(TItemPos item_pos, BYTE display_pos, bool SelectPosAuto)
#else
bool CExchange::AddItem(TItemPos item_pos, BYTE display_pos)
#endif
{
	assert(m_pOwner != NULL && GetCompany());

	if (!item_pos.IsValidItemPosition())
		return false;

	if (item_pos.IsEquipPosition())
		return false;

	LPITEM item;

	if (!(item = m_pOwner->GetItem(item_pos)))
		return false;

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_GIVE))
	{
		m_pOwner->ChatPacket(CHAT_TYPE_INFO, "[LS;710]");
		return false;
	}

	if (true == item->isLocked())
	{
		return false;
	}

	if (item->IsExchanging())
	{
		sys_log(0, "EXCHANGE under exchanging");
		return false;
	}

#if defined(ITEM_CHECKINOUT_UPDATE)
	if (SelectPosAuto)
	{
		int AutoPos = GetEmptyExchange(item->GetSize());
		if (AutoPos == -1)
		{
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, "<EXCHANGE> You don't have enough space.");
			return false;
		}
		display_pos = AutoPos;
	}
#endif

	if (!m_pGrid->IsEmpty(display_pos, 1, item->GetSize()))
	{
		sys_log(0, "EXCHANGE not empty item_pos %d %d %d", display_pos, 1, item->GetSize());
		return false;
	}

	Accept(false);
	GetCompany()->Accept(false);

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (m_apItems[i])
			continue;

		m_apItems[i]		= item;
		m_aItemPos[i]		= item_pos;
		m_abItemDisplayPos[i]	= display_pos;
		m_pGrid->Put(display_pos, 1, item->GetSize());

		item->SetExchanging(true);

		exchange_packet(m_pOwner,
				EXCHANGE_SUBHEADER_GC_ITEM_ADD,
				true,
				item->GetVnum(),
				TItemPos(RESERVED_WINDOW, display_pos),
				item->GetCount(),
				item);

		exchange_packet(GetCompany()->GetOwner(),
				EXCHANGE_SUBHEADER_GC_ITEM_ADD,
				false,
				item->GetVnum(),
				TItemPos(RESERVED_WINDOW, display_pos),
				item->GetCount(),
				item);

		sys_log(0, "EXCHANGE AddItem success %s pos(%d, %d) %d", item->GetName(), item_pos.window_type, item_pos.cell, display_pos);

		return true;
	}

	return false;
}

bool CExchange::RemoveItem(BYTE pos)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
		return false;

	if (!m_apItems[pos])
		return false;

	TItemPos PosOfInventory = m_aItemPos[pos];
	m_apItems[pos]->SetExchanging(false);

	m_pGrid->Get(m_abItemDisplayPos[pos], 1, m_apItems[pos]->GetSize());

	exchange_packet(GetOwner(),	EXCHANGE_SUBHEADER_GC_ITEM_DEL, true, pos, NPOS, 0);
	exchange_packet(GetCompany()->GetOwner(), EXCHANGE_SUBHEADER_GC_ITEM_DEL, false, pos, PosOfInventory, 0);

	Accept(false);
	GetCompany()->Accept(false);

	m_apItems[pos]	    = NULL;
	m_aItemPos[pos]	    = NPOS;
	m_abItemDisplayPos[pos] = 0;
	return true;
}

#ifdef ENABLE_LONG_LONG
bool CExchange::AddGold(int64_t gold)
#else
bool CExchange::AddGold(int32_t gold)
#endif
{
	if (gold <= 0)
		return false;

	if (GetOwner()->GetGold() < gold)
	{
		exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_LESS_GOLD, 0, 0, NPOS, 0);
		return false;
	}

	if (m_lGold > 0)
		return false;

	Accept(false);
	GetCompany()->Accept(false);

	m_lGold = gold;

	exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_GOLD_ADD, true, m_lGold, NPOS, 0);
	exchange_packet(GetCompany()->GetOwner(), EXCHANGE_SUBHEADER_GC_GOLD_ADD, false, m_lGold, NPOS, 0);
	return true;
}

#ifdef ENABLE_CHEQUE_SYSTEM
bool CExchange::AddCheque(int64_t cheque)
{
	if (cheque <= 0)
		return false;

	if (GetOwner()->GetCheque() < cheque)
	{
		exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_LESS_CHEQUE, 0, 0, NPOS, 0);
		return false;
	}

	LPCHARACTER victim = GetCompany()->GetOwner();

	if (m_lCheque)
	{
		int32_t vic_cheque = victim->GetCheque() + m_lCheque;
		if (vic_cheque > CHEQUE_MAX)
		{
			exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_LESS_CHEQUE, 0, 0, NPOS, 0);
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, "This user can not accept that much cheque.");
			return false;
		}
	}

	if (LC_IsCanada() == true || LC_IsEurope() == true)
	{
		if (m_lCheque > 0)
		{
			return false;
		}
	}

	Accept(false);
	GetCompany()->Accept(false);

	m_lCheque = cheque;

	exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_CHEQUE_ADD, true, m_lCheque, NPOS, 0);
	exchange_packet(GetCompany()->GetOwner(), EXCHANGE_SUBHEADER_GC_CHEQUE_ADD, false, m_lCheque, NPOS, 0);
	return true;
}
#endif

bool CExchange::Check(int * piItemCount)
{
	if (GetOwner()->GetGold() < m_lGold)
		return false;

#ifdef ENABLE_CHEQUE_SYSTEM
	if (GetOwner()->GetCheque() < m_lCheque)
		return false;
#endif

	int item_count = 0;

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (!m_apItems[i])
			continue;

		if (!m_aItemPos[i].IsValidItemPosition())
			return false;

		if (m_apItems[i] != GetOwner()->GetItem(m_aItemPos[i]))
			return false;

		++item_count;
	}

	*piItemCount = item_count;
	return true;
}

bool CExchange::CheckSpace()
{
	static CGrid s_grid1(INVENTORY_PAGE_COLUMN, INVENTORY_PAGE_ROW); // inven page 1
	static CGrid s_grid2(INVENTORY_PAGE_COLUMN, INVENTORY_PAGE_ROW); // inven page 2
#ifdef ENABLE_EXTEND_INVEN_SYSTEM
	static CGrid s_grid3(INVENTORY_PAGE_COLUMN, INVENTORY_PAGE_ROW); // inven page 3
	static CGrid s_grid4(INVENTORY_PAGE_COLUMN, INVENTORY_PAGE_ROW); // inven page 4
#endif

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	LPCHARACTER	me = GetOwner();
	static CGrid s_grid5(5, SKILL_BOOK_INVENTORY_MAX_NUM / 5 / 3);
	static CGrid s_grid6(5, SKILL_BOOK_INVENTORY_MAX_NUM / 5 / 3);
	static CGrid s_grid7(5, SKILL_BOOK_INVENTORY_MAX_NUM / 5 / 3);
	static CGrid s_grid8(5, UPGRADE_ITEMS_INVENTORY_MAX_NUM / 5 / 3);
	static CGrid s_grid9(5, UPGRADE_ITEMS_INVENTORY_MAX_NUM / 5 / 3);
	static CGrid s_grid10(5, UPGRADE_ITEMS_INVENTORY_MAX_NUM / 5 / 3);
	static CGrid s_grid11(5, STONE_INVENTORY_MAX_NUM / 5 / 3);
	static CGrid s_grid12(5, STONE_INVENTORY_MAX_NUM / 5 / 3);
	static CGrid s_grid13(5, STONE_INVENTORY_MAX_NUM / 5 / 3);
	static CGrid s_grid14(5, BOX_INVENTORY_MAX_NUM / 5 / 3);
	static CGrid s_grid15(5, BOX_INVENTORY_MAX_NUM / 5 / 3);
	static CGrid s_grid16(5, BOX_INVENTORY_MAX_NUM / 5 / 3);
	static CGrid s_grid17(5, EFSUN_INVENTORY_MAX_NUM / 5 / 3);
	static CGrid s_grid18(5, EFSUN_INVENTORY_MAX_NUM / 5 / 3);
	static CGrid s_grid19(5, EFSUN_INVENTORY_MAX_NUM / 5 / 3);
	static CGrid s_grid20(5, CICEK_INVENTORY_MAX_NUM / 5 / 3);
	static CGrid s_grid21(5, CICEK_INVENTORY_MAX_NUM / 5 / 3);
	static CGrid s_grid22(5, CICEK_INVENTORY_MAX_NUM / 5 / 3);
#endif

	s_grid1.Clear();
	s_grid2.Clear();
#ifdef ENABLE_EXTEND_INVEN_SYSTEM
	s_grid3.Clear();
	s_grid4.Clear();
#endif

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	s_grid5.Clear();
	s_grid6.Clear();
	s_grid7.Clear();
	s_grid8.Clear();
	s_grid9.Clear();
	s_grid10.Clear();
	s_grid11.Clear();
	s_grid12.Clear();
	s_grid13.Clear();
	s_grid14.Clear();
	s_grid15.Clear();
	s_grid16.Clear();
	s_grid17.Clear();
	s_grid18.Clear();
	s_grid19.Clear();
	s_grid20.Clear();
	s_grid21.Clear();
	s_grid22.Clear();
#endif

	LPCHARACTER	victim = GetCompany()->GetOwner();
	LPITEM item;

	int i;

	for (i = 0; i < INVENTORY_PAGE_SIZE*1; ++i)
	{
		if (!(item = victim->GetInventoryItem(i)))
			continue;

		s_grid1.Put(i, 1, item->GetSize());
	}
	for (i = INVENTORY_PAGE_SIZE*1; i < INVENTORY_PAGE_SIZE*2; ++i)
	{
		if (!(item = victim->GetInventoryItem(i)))
			continue;

		s_grid2.Put(i - INVENTORY_PAGE_SIZE*1, 1, item->GetSize());
	}
#ifdef ENABLE_EXTEND_INVEN_SYSTEM
	for (i = INVENTORY_PAGE_SIZE*2; i < INVENTORY_PAGE_SIZE*3; ++i)
	{
		if (!(item = victim->GetInventoryItem(i)))
			continue;

		s_grid3.Put(i - INVENTORY_PAGE_SIZE*2, 1, item->GetSize());
	}
	for (i = INVENTORY_PAGE_SIZE*3; i < INVENTORY_PAGE_SIZE*4; ++i)
	{
		if (!(item = victim->GetInventoryItem(i)))
			continue;

		s_grid4.Put(i - INVENTORY_PAGE_SIZE*3, 1, item->GetSize());
	}
#endif

	static std::vector <WORD> s_vDSGrid(DRAGON_SOUL_INVENTORY_MAX_NUM);

	bool bDSInitialized = false;

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	int b;
	int v;
	int u;
	int r;
	int a;
	int l;
	const int perPageSkillBookSlotCount = 45;
	const int perPageUpgradeItemsSlotCount = 45;
	const int perPageStoneSlotCount = 45;
	const int perPageBoxSlotCount = 45;
	const int perPageEfsunSlotCount = 45;
	const int perPageCicekSlotCount = 45;
	for (b = 0; b < SKILL_BOOK_INVENTORY_MAX_NUM; ++b) {
		if (!(item = me->GetItem(TItemPos(INVENTORY, b))))
			continue;

		BYTE itemSize = item->GetSize();

		if (b < perPageSkillBookSlotCount) // Notice: This is adjusted for 3 Pages only!
			s_grid5.Put(b, 1, itemSize);
		else if (b < perPageSkillBookSlotCount * 2)
			s_grid6.Put(b - perPageSkillBookSlotCount, 1, itemSize);
		else if (b < perPageSkillBookSlotCount * 3)
			s_grid7.Put(b - perPageSkillBookSlotCount * 2, 1, itemSize);
	}

	for (v = 0; v < UPGRADE_ITEMS_INVENTORY_MAX_NUM; ++v) {
		if (!(item = me->GetItem(TItemPos(INVENTORY, v))))
			continue;

		BYTE itemSize = item->GetSize();

		if (v < perPageUpgradeItemsSlotCount) // Notice: This is adjusted for 3 Pages only!
			s_grid8.Put(v, 1, itemSize);
		else if (v < perPageUpgradeItemsSlotCount * 2)
			s_grid9.Put(v - perPageUpgradeItemsSlotCount, 1, itemSize);
		else if (v < perPageUpgradeItemsSlotCount * 3)
			s_grid10.Put(v - perPageUpgradeItemsSlotCount * 2, 1, itemSize);
	}

	for (u = 0; u < STONE_INVENTORY_MAX_NUM; ++u) {
		if (!(item = me->GetItem(TItemPos(INVENTORY, u))))
			continue;

		BYTE itemSize = item->GetSize();

		if (u < perPageStoneSlotCount) // Notice: This is adjusted for 3 Pages only!
			s_grid11.Put(u, 1, itemSize);
		else if (u < perPageStoneSlotCount * 2)
			s_grid12.Put(u - perPageStoneSlotCount, 1, itemSize);
		else if (u < perPageStoneSlotCount * 3)
			s_grid13.Put(u - perPageStoneSlotCount * 2, 1, itemSize);
	}

	for (r = 0; r < BOX_INVENTORY_MAX_NUM; ++r) {
		if (!(item = me->GetItem(TItemPos(INVENTORY, r))))
			continue;

		BYTE itemSize = item->GetSize();

		if (r < perPageBoxSlotCount) // Notice: This is adjusted for 3 Pages only!
			s_grid14.Put(r, 1, itemSize);
		else if (r < perPageBoxSlotCount * 2)
			s_grid15.Put(r - perPageBoxSlotCount, 1, itemSize);
		else if (r < perPageBoxSlotCount * 3)
			s_grid16.Put(r - perPageBoxSlotCount * 2, 1, itemSize);
	}

	for (a = 0; a < EFSUN_INVENTORY_MAX_NUM; ++a) {
		if (!(item = me->GetItem(TItemPos(INVENTORY, a))))
			continue;

		BYTE itemSize = item->GetSize();

		if (a < perPageEfsunSlotCount) // Notice: This is adjusted for 3 Pages only!
			s_grid17.Put(a, 1, itemSize);
		else if (a < perPageEfsunSlotCount * 2)
			s_grid18.Put(a - perPageEfsunSlotCount, 1, itemSize);
		else if (a < perPageEfsunSlotCount * 3)
			s_grid19.Put(a - perPageEfsunSlotCount * 2, 1, itemSize);
	}

	for (l = 0; l < CICEK_INVENTORY_MAX_NUM; ++l) {
		if (!(item = me->GetItem(TItemPos(INVENTORY, l))))
			continue;

		BYTE itemSize = item->GetSize();

		if (l < perPageCicekSlotCount) // Notice: This is adjusted for 3 Pages only!
			s_grid20.Put(l, 1, itemSize);
		else if (l < perPageCicekSlotCount * 2)
			s_grid21.Put(l - perPageCicekSlotCount, 1, itemSize);
		else if (l < perPageCicekSlotCount * 3)
			s_grid22.Put(l - perPageCicekSlotCount * 2, 1, itemSize);
	}
#endif

	for (i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (!(item = m_apItems[i]))
			continue;

		if (item->IsDragonSoul())
		{
			if (!victim->DragonSoul_IsQualified())
			{
				return false;
			}

			if (!bDSInitialized)
			{
				bDSInitialized = true;
				victim->CopyDragonSoulItemGrid(s_vDSGrid);
			}

			bool bExistEmptySpace = false;
			WORD wBasePos = DSManager::instance().GetBasePosition(item);
			if (wBasePos >= DRAGON_SOUL_INVENTORY_MAX_NUM)
				return false;

			for (int i = 0; i < DRAGON_SOUL_BOX_SIZE; i++)
			{
				WORD wPos = wBasePos + i;
				if (0 == s_vDSGrid[wPos]) // @fixme159 (wBasePos to wPos)
				{
					bool bEmpty = true;
					for (int j = 1; j < item->GetSize(); j++)
					{
						if (s_vDSGrid[wPos + j * DRAGON_SOUL_BOX_COLUMN_NUM])
						{
							bEmpty = false;
							break;
						}
					}
					if (bEmpty)
					{
						for (int j = 0; j < item->GetSize(); j++)
						{
							s_vDSGrid[wPos + j * DRAGON_SOUL_BOX_COLUMN_NUM] =  wPos + 1;
						}
						bExistEmptySpace = true;
						break;
					}
				}
				if (bExistEmptySpace)
					break;
			}
			if (!bExistEmptySpace)
				return false;
		}
		else
		{
			int iPos;

			if ((iPos = s_grid1.FindBlank(1, item->GetSize())) >= 0)
			{
				s_grid1.Put(iPos, 1, item->GetSize());
			}
			else if ((iPos = s_grid2.FindBlank(1, item->GetSize())) >= 0)
			{
				s_grid2.Put(iPos, 1, item->GetSize());
			}
#ifdef ENABLE_EXTEND_INVEN_SYSTEM
			else if ((iPos = s_grid3.FindBlank(1, item->GetSize())) >= 0)
			{
				s_grid3.Put(iPos, 1, item->GetSize());
			}
			else if ((iPos = s_grid4.FindBlank(1, item->GetSize())) >= 0)
			{
				s_grid4.Put(iPos, 1, item->GetSize());
			}
#endif
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
			else if (item->IsSkillBook())
			{
				BYTE itemSize = item->GetSize();
				int iPos = s_grid5.FindBlank(1, itemSize);
				if (iPos >= 0) {
					s_grid5.Put(iPos, 1, itemSize);
					continue;
				}
				
				iPos = s_grid6.FindBlank(1, itemSize);
				if (iPos >= 0) {
					s_grid6.Put(iPos, 1, itemSize);
					continue;
				}
				
				iPos = s_grid7.FindBlank(1, itemSize);
				if (iPos >= 0) {
					s_grid7.Put(iPos, 1, itemSize);
					continue;
				}
				
				return false;
			}
			else if (item->IsUpgradeItem())
			{
				BYTE itemSize = item->GetSize();
				int iPos = s_grid8.FindBlank(1, itemSize);
				if (iPos >= 0) {
					s_grid8.Put(iPos, 1, itemSize);
					continue;
				}
				
				iPos = s_grid9.FindBlank(1, itemSize);
				if (iPos >= 0) {
					s_grid9.Put(iPos, 1, itemSize);
					continue;
				}
				
				iPos = s_grid10.FindBlank(1, itemSize);
				if (iPos >= 0) {
					s_grid10.Put(iPos, 1, itemSize);
					continue;
				}
				
				return false;
			}
			else if (item->IsStone())
			{
				BYTE itemSize = item->GetSize();
				int iPos = s_grid11.FindBlank(1, itemSize);
				if (iPos >= 0) {
					s_grid11.Put(iPos, 1, itemSize);
					continue;
				}
				
				iPos = s_grid12.FindBlank(1, itemSize);
				if (iPos >= 0) {
					s_grid12.Put(iPos, 1, itemSize);
					continue;
				}
				
				iPos = s_grid13.FindBlank(1, itemSize);
				if (iPos >= 0) {
					s_grid13.Put(iPos, 1, itemSize);
					continue;
				}
				
				return false;
			}
			else if (item->IsBox())
			{
				BYTE itemSize = item->GetSize();
				int iPos = s_grid14.FindBlank(1, itemSize);
				if (iPos >= 0) {
					s_grid14.Put(iPos, 1, itemSize);
					continue;
				}
				
				iPos = s_grid15.FindBlank(1, itemSize);
				if (iPos >= 0) {
					s_grid15.Put(iPos, 1, itemSize);
					continue;
				}
				
				iPos = s_grid16.FindBlank(1, itemSize);
				if (iPos >= 0) {
					s_grid16.Put(iPos, 1, itemSize);
					continue;
				}
				
				return false;
			}
			else if (item->IsEfsun())
			{
				BYTE itemSize = item->GetSize();
				int iPos = s_grid17.FindBlank(1, itemSize);
				if (iPos >= 0) {
					s_grid17.Put(iPos, 1, itemSize);
					continue;
				}
				
				iPos = s_grid18.FindBlank(1, itemSize);
				if (iPos >= 0) {
					s_grid18.Put(iPos, 1, itemSize);
					continue;
				}
				
				iPos = s_grid19.FindBlank(1, itemSize);
				if (iPos >= 0) {
					s_grid19.Put(iPos, 1, itemSize);
					continue;
				}
				
				return false;
			}
			else if (item->IsCicek())
			{
				BYTE itemSize = item->GetSize();
				int iPos = s_grid20.FindBlank(1, itemSize);
				if (iPos >= 0) {
					s_grid20.Put(iPos, 1, itemSize);
					continue;
				}
				
				iPos = s_grid21.FindBlank(1, itemSize);
				if (iPos >= 0) {
					s_grid21.Put(iPos, 1, itemSize);
					continue;
				}
				
				iPos = s_grid22.FindBlank(1, itemSize);
				if (iPos >= 0) {
					s_grid22.Put(iPos, 1, itemSize);
					continue;
				}
				
				return false;
			}
#endif
			else
				return false;
		}
	}

	return true;
}

#ifdef ENABLE_IN_GAME_LOG_SYSTEM
bool CExchange::Done(std::vector<InGameLog::TTradeLogItemInfo>& logitem)
#else
bool CExchange::Done()
#endif
{
	int		empty_pos, i;
	LPITEM	item;

	LPCHARACTER	victim = GetCompany()->GetOwner();
#ifdef ENABLE_IN_GAME_LOG_SYSTEM
	InGameLog::TTradeLogItemInfo loginfo;
#endif

	for (i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (!(item = m_apItems[i]))
			continue;

		if (item->IsDragonSoul())
			empty_pos = victim->GetEmptyDragonSoulInventory(item);
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
		else if (item->IsSkillBook())
			empty_pos = victim->GetEmptySkillBookInventory(item->GetSize());
		else if (item->IsUpgradeItem())
			empty_pos = victim->GetEmptyUpgradeItemsInventory(item->GetSize());
		else if (item->IsStone())
			empty_pos = victim->GetEmptyStoneInventory(item->GetSize());
		else if (item->IsBox())
			empty_pos = victim->GetEmptyBoxInventory(item->GetSize());
		else if (item->IsEfsun())
			empty_pos = victim->GetEmptyEfsunInventory(item->GetSize());
		else if (item->IsCicek())
			empty_pos = victim->GetEmptyCicekInventory(item->GetSize());
#endif
		else
			empty_pos = victim->GetEmptyInventory(item->GetSize());

		if (empty_pos < 0)
		{
			sys_err("Exchange::Done : Cannot find blank position in inventory %s <-> %s item %s",
					m_pOwner->GetName(), victim->GetName(), item->GetName());
			continue;
		}

		assert(empty_pos >= 0);

		if (item->GetVnum() == 90008 || item->GetVnum() == 90009) // VCARD
		{
			VCardUse(m_pOwner, victim, item);
			continue;
		}

		m_pOwner->SyncQuickslot(QUICKSLOT_TYPE_ITEM, item->GetCell(), 255);

		item->RemoveFromCharacter();
		if (item->IsDragonSoul())
			item->AddToCharacter(victim, TItemPos(DRAGON_SOUL_INVENTORY, empty_pos));
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
		else if (item->IsSkillBook())
			item->AddToCharacter(victim, TItemPos(INVENTORY, empty_pos));
		else if (item->IsUpgradeItem())
			item->AddToCharacter(victim, TItemPos(INVENTORY, empty_pos));
		else if (item->IsStone())
			item->AddToCharacter(victim, TItemPos(INVENTORY, empty_pos));
		else if (item->IsBox())
			item->AddToCharacter(victim, TItemPos(INVENTORY, empty_pos));
		else if (item->IsEfsun())
			item->AddToCharacter(victim, TItemPos(INVENTORY, empty_pos));
		else if (item->IsCicek())
			item->AddToCharacter(victim, TItemPos(INVENTORY, empty_pos));
#endif
		else
			item->AddToCharacter(victim, TItemPos(INVENTORY, empty_pos));
		ITEM_MANAGER::instance().FlushDelayedSave(item);

		item->SetExchanging(false);
		{
			char exchange_buf[51];

			snprintf(exchange_buf, sizeof(exchange_buf), "%s %u %u", item->GetName(), GetOwner()->GetPlayerID(), item->GetCount());
			LogManager::instance().ItemLog(victim, item, "EXCHANGE_TAKE", exchange_buf);

			snprintf(exchange_buf, sizeof(exchange_buf), "%s %u %u", item->GetName(), victim->GetPlayerID(), item->GetCount());
			LogManager::instance().ItemLog(GetOwner(), item, "EXCHANGE_GIVE", exchange_buf);

			if (item->GetVnum() >= 80003 && item->GetVnum() <= 80007)
			{
				LogManager::instance().GoldBarLog(victim->GetPlayerID(), item->GetID(), EXCHANGE_TAKE, "");
				LogManager::instance().GoldBarLog(GetOwner()->GetPlayerID(), item->GetID(), EXCHANGE_GIVE, "");
			}
		}

#ifdef ENABLE_IN_GAME_LOG_SYSTEM
		loginfo.ownerID = m_pOwner->GetPlayerID();
		memcpy(loginfo.item.aAttr, item->GetAttributes(), sizeof(loginfo.item.aAttr));
		loginfo.item.dwVnum = item->GetVnum();
		loginfo.item.dwCount = item->GetCount();
		memcpy(loginfo.item.alSockets, item->GetSockets(), sizeof(loginfo.item.alSockets));
		loginfo.pos = m_abItemDisplayPos[i];
		logitem.push_back(loginfo);
#endif

		m_apItems[i] = NULL;
	}

	if (m_lGold)
	{
		GetOwner()->PointChange(POINT_GOLD, -m_lGold, true);
		victim->PointChange(POINT_GOLD, m_lGold, true);

		if (m_lGold > 1000)
		{
			char exchange_buf[51];
			snprintf(exchange_buf, sizeof(exchange_buf), "%u %s", GetOwner()->GetPlayerID(), GetOwner()->GetName());
			LogManager::instance().CharLog(victim, m_lGold, "EXCHANGE_GOLD_TAKE", exchange_buf);

			snprintf(exchange_buf, sizeof(exchange_buf), "%u %s", victim->GetPlayerID(), victim->GetName());
			LogManager::instance().CharLog(GetOwner(), m_lGold, "EXCHANGE_GOLD_GIVE", exchange_buf);
		}
	}

#ifdef ENABLE_CHEQUE_SYSTEM
	if (m_lCheque)
	{
		GetOwner()->PointChange(POINT_CHEQUE, -m_lCheque, true);
		victim->PointChange(POINT_CHEQUE, m_lCheque, true);

		if (m_lCheque > 1000)
		{
			char exchange_buf[51];
			snprintf(exchange_buf, sizeof(exchange_buf), "%u %s", GetOwner()->GetPlayerID(), GetOwner()->GetName());
			LogManager::instance().CharLog(victim, m_lCheque, "EXCHANGE_GOLD_TAKE", exchange_buf);

			snprintf(exchange_buf, sizeof(exchange_buf), "%u %s", victim->GetPlayerID(), victim->GetName());
			LogManager::instance().CharLog(GetOwner(), m_lCheque, "EXCHANGE_GOLD_GIVE", exchange_buf);
		}
	}
#endif

	m_pGrid->Clear();
	return true;
}

bool CExchange::Accept(bool bAccept)
{
	if (m_bAccept == bAccept)
		return true;

	m_bAccept = bAccept;

	if (m_bAccept && GetCompany()->m_bAccept)
	{
		int	iItemCount;

		LPCHARACTER victim = GetCompany()->GetOwner();

		//PREVENT_PORTAL_AFTER_EXCHANGE
		GetOwner()->SetExchangeTime();
		victim->SetExchangeTime();
		//END_PREVENT_PORTAL_AFTER_EXCHANGE

#ifdef ENABLE_IN_GAME_LOG_SYSTEM
		std::vector<InGameLog::TTradeLogItemInfo> v_itemLog;
		v_itemLog.clear();
		InGameLog::TSubPackTradeAddGD logpacket;
#endif

		// @fixme150 BEGIN
		if (quest::CQuestManager::instance().GetPCForce(GetOwner()->GetPlayerID())->IsRunning() == true)
		{
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, "You cannot trade if you're using quests");
			victim->ChatPacket(CHAT_TYPE_INFO, "You cannot trade if the other part using quests");
			goto EXCHANGE_END;
		}
		else if (quest::CQuestManager::instance().GetPCForce(victim->GetPlayerID())->IsRunning() == true)
		{
			victim->ChatPacket(CHAT_TYPE_INFO, "You cannot trade if you're using quests");
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, "You cannot trade if the other part using quests");
			goto EXCHANGE_END;
		}
		// @fixme150 END

		if (!Check(&iItemCount))
		{
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, "[LS;897]");
			victim->ChatPacket(CHAT_TYPE_INFO, "[LS;898]");
			goto EXCHANGE_END;
		}

		if (!CheckSpace())
		{
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, "[LS;899]");
			victim->ChatPacket(CHAT_TYPE_INFO, "[LS;1130]");
			goto EXCHANGE_END;
		}

		if (!GetCompany()->Check(&iItemCount))
		{
			victim->ChatPacket(CHAT_TYPE_INFO, "[LS;897]");
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, "[LS;898]");
			goto EXCHANGE_END;
		}

		if (!GetCompany()->CheckSpace())
		{
			victim->ChatPacket(CHAT_TYPE_INFO, "[LS;899]");
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, "[LS;1130]");
			goto EXCHANGE_END;
		}

		if (db_clientdesc->GetSocket() == INVALID_SOCKET)
		{
			sys_err("Cannot use exchange feature while DB cache connection is dead.");
			victim->ChatPacket(CHAT_TYPE_INFO, "Unknown error");
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, "Unknown error");
			goto EXCHANGE_END;
		}

#ifdef ENABLE_CHEQUE_SYSTEM
		// Pre-validate cheque limits for both sides before any items are moved
		if (m_lCheque)
		{
			int64_t vic_cheque = victim->GetCheque() + m_lCheque;
			if (vic_cheque > CHEQUE_MAX)
			{
				GetOwner()->ChatPacket(CHAT_TYPE_INFO, "The other party has reached cheque limit.");
				victim->ChatPacket(CHAT_TYPE_INFO, "You have reached cheque limit.");
				goto EXCHANGE_END;
			}
		}
		if (GetCompany()->m_lCheque)
		{
			int64_t own_cheque = GetOwner()->GetCheque() + GetCompany()->m_lCheque;
			if (own_cheque > CHEQUE_MAX)
			{
				victim->ChatPacket(CHAT_TYPE_INFO, "The other party has reached cheque limit.");
				GetOwner()->ChatPacket(CHAT_TYPE_INFO, "You have reached cheque limit.");
				goto EXCHANGE_END;
			}
		}
#endif

#ifdef ENABLE_IN_GAME_LOG_SYSTEM
		if (Done(v_itemLog))
#else
		if (Done())
#endif
		{
			if (m_lGold)
				GetOwner()->Save();

#ifdef ENABLE_CHEQUE_SYSTEM
			if (m_lCheque)
				GetOwner()->Save();
#endif

#ifdef ENABLE_IN_GAME_LOG_SYSTEM
			if (GetCompany()->Done(v_itemLog))
#else
			if (GetCompany()->Done())
#endif
			{
				if (GetCompany()->m_lGold)
					victim->Save();

#ifdef ENABLE_IN_GAME_LOG_SYSTEM
				if (v_itemLog.size() > 0 || m_lGold > 0 || GetCompany()->m_lGold > 0)
				{
					logpacket.itemCount = v_itemLog.size();
					logpacket.owner.pid = m_pOwner->GetPlayerID();
					snprintf(logpacket.owner.name, sizeof(logpacket.owner.name), m_pOwner->GetName());
					logpacket.owner.gold = m_lGold;

					logpacket.victim.pid = GetCompany()->m_pOwner->GetPlayerID();
					snprintf(logpacket.victim.name, sizeof(logpacket.victim.name), GetCompany()->m_pOwner->GetName());
					logpacket.victim.gold = GetCompany()->m_lGold;
					InGameLog::InGameLogManager& rIglMgr = InGameLog::GetManager();
					rIglMgr.SendTradeLogAddGD(v_itemLog, logpacket);
				}
#endif

#ifdef ENABLE_CHEQUE_SYSTEM
				if (GetCompany()->m_lCheque)
					victim->Save();
#endif

				// INTERNATIONAL_VERSION
				GetOwner()->ChatPacket(CHAT_TYPE_INFO, "[LS;900;%s]", victim->GetName());
				victim->ChatPacket(CHAT_TYPE_INFO, "[LS;900;%s]", GetOwner()->GetName());
				// END_OF_INTERNATIONAL_VERSION
			}
		}

	EXCHANGE_END:
		Cancel();
		return false;
	}
	else
	{
		exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_ACCEPT, true, m_bAccept, NPOS, 0);
		exchange_packet(GetCompany()->GetOwner(), EXCHANGE_SUBHEADER_GC_ACCEPT, false, m_bAccept, NPOS, 0);
		return true;
	}
}

void CExchange::Cancel()
{
	exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_END, 0, 0, NPOS, 0);
	GetOwner()->SetExchange(NULL);

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (m_apItems[i])
			m_apItems[i]->SetExchanging(false);
	}

	if (GetCompany())
	{
		GetCompany()->SetCompany(NULL);
		GetCompany()->Cancel();
	}

	M2_DELETE(this);
}
//martysama0134's ceqyqttoaf71vasf9t71218
