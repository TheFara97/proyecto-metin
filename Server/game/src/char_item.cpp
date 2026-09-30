#include "stdafx.h"

#include <stack>

#include "utils.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "item_manager.h"
#include "desc.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "packet.h"
#include "affect.h"
#include "skill.h"
#include "start_position.h"
#include "mob_manager.h"
#include "db.h"
#include "log.h"
#include "vector.h"
#include "buffer_manager.h"
#include "questmanager.h"
#include "locale_item_manager.h"
#include "fishing.h"
#include "party.h"
#include "dungeon.h"
#include "refine.h"
#include "unique_item.h"
#include "war_map.h"
#include "xmas_event.h"
#include "marriage.h"
#include "monarch.h"
#include "polymorph.h"
#include "blend_item.h"
#include "castle.h"
#include "arena.h"
#include "config.h"
#include "threeway_war.h"
#ifdef ENABLE_SWITCHBOT
#include "switchbot.h"
#endif
#include "safebox.h"
#include "shop.h"
#include "guild_manager.h"

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
#include "buff_npc_system.h"
#endif

#ifdef ENABLE_GLOVE_SYSTEM
#include "glove_bonus_manager.h"
#endif

#ifdef ENABLE_NEWSTUFF
#include "pvp.h"
#include "../../common/PulseManager.h"
#endif

#include "../../common/item_length.h"

#include "../../common/VnumHelper.h"
#include "DragonSoul.h"
#include "buff_on_attributes.h"
#include "belt_inventory_helper.h"
#include "../../common/CommonDefines.h"
#ifdef ENABLE_NEW_PET_SYSTEM
#include "PetSystem.h"
#endif
#ifdef __ENABLE_POLYMORPH_SYSTEM__
	#include "PolySystem.h"
#endif

#ifdef __ENABLE_ITEM_TOGGLE__
#include "ItemToggle.hpp"
#endif
#ifdef RANKING_SYSTEM
#include "server_ranking_manager.h"
#include "ranking_manager.h"
#endif
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
#include "AchievementTitle.h"
#endif
#ifdef _ENABLE_BATTLEPASS_
#include "BattlePassManager.h"
#endif

#define ENABLE_EFFECT_EXTRAPOT
#define ENABLE_BOOKS_STACKFIX
#define ENABLE_ITEM_RARE_ATTR_LEVEL_PCT

enum { ITEM_BROKEN_METIN_VNUM = 28960 };

// CHANGE_ITEM_ATTRIBUTES
const char CHARACTER::msc_szLastChangeItemAttrFlag[] = "Item.LastChangeItemAttr";
// END_OF_CHANGE_ITEM_ATTRIBUTES
const BYTE g_aBuffOnAttrPoints[] = { POINT_ENERGY, POINT_COSTUME_ATTR_BONUS };

struct FFindStone
{
	std::map<DWORD, LPCHARACTER> m_mapStone;

	void operator()(LPENTITY pEnt)
	{
		if (pEnt->IsType(ENTITY_CHARACTER) == true)
		{
			LPCHARACTER pChar = (LPCHARACTER)pEnt;

			if (pChar->IsStone() == true)
			{
				m_mapStone[(DWORD)pChar->GetVID()] = pChar;
			}
		}
	}
};

static bool IS_SUMMON_ITEM(int vnum)
{
	switch (vnum)
	{
	case 22000:
	case 22010:
	case 22011:
	case 22020:
	case ITEM_MARRIAGE_RING:
		return true;
	}

	return false;
}

static bool IS_MONKEY_DUNGEON(int map_index)
{
	switch (map_index)
	{
	case 5:
	case 25:
	case 45:
	case 108:
	case 109:
		return true;;
	}

	return false;
}

bool IS_SUMMONABLE_ZONE(int map_index)
{
	if (IS_MONKEY_DUNGEON(map_index))
		return false;
	if (IS_CASTLE_MAP(map_index))
		return false;

	switch (map_index)
	{
	case 66:
	case 71:
	case 72:
	case 73:
	case 193:
#if 0
	case 184:
	case 185:
	case 186:
	case 187:
	case 188:
	case 189:
#endif

	case 216:
	case 217:
	case 208:

	case 113:
		return false;
	}

	if (map_index > 10000) return false;

	return true;
}

bool IS_BOTARYABLE_ZONE(int nMapIndex)
{
	if (!g_bEnableBootaryCheck) return true;

	switch (nMapIndex)
	{
	case 1:
	case 2:
		return true;
	}

	return false;
}

static bool FN_check_item_socket(LPITEM item)
{
#ifdef ENABLE_SORT_INVENTORY_ITEMS
	if (item->GetType() == ITEM_USE && item->GetSubType() == USE_AFFECT)
		return true;
#endif

	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		if (item->GetSocket(i) != item->GetProto()->alSockets[i])
			return false;
	}

	return true;
}

static void FN_copy_item_socket(LPITEM dest, LPITEM src)
{
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		dest->SetSocket(i, src->GetSocket(i));
	}
}
static bool FN_check_item_sex(LPCHARACTER ch, LPITEM item)
{
	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_MALE))
	{
		if (SEX_MALE == GET_SEX(ch))
			return false;
	}

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_FEMALE))
	{
		if (SEX_FEMALE == GET_SEX(ch))
			return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// ITEM HANDLING
/////////////////////////////////////////////////////////////////////////////
bool CHARACTER::CanHandleItem(bool bSkipCheckRefine, bool bSkipObserver)
{
	if (!bSkipObserver)
		if (m_bIsObserver)
			return false;

	if (GetMyShop())
		return false;

	if (!bSkipCheckRefine)
		if (m_bUnderRefine)
			return false;

	if (IsCubeOpen() || NULL != DragonSoul_RefineWindow_GetOpener())
		return false;

	if (IsWarping())
		return false;

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	if ((m_bAcceCombination) || (m_bAcceAbsorption))
		return false;
#endif
#ifdef ENABLE_AURA_SYSTEM
	if ((m_bAuraRefine) || (m_bAuraAbsorption))
		return false;
#endif
#ifdef __ENABLE_POLYMORPH_SYSTEM__
	if (IsPolyShopping())
		return false;
#endif

	return true;
}

LPITEM CHARACTER::GetInventoryItem(WORD wCell) const
{
	return GetItem(TItemPos(INVENTORY, wCell));
}
LPITEM CHARACTER::GetItem(TItemPos Cell) const
{
	if (!m_PlayerSlots)
		return nullptr;

	if (!IsValidItemPosition(Cell))
		return NULL;

	WORD wCell = Cell.cell;
	BYTE window_type = Cell.window_type;
	switch (window_type)
	{
	case INVENTORY:
	case EQUIPMENT:
		if (wCell >= INVENTORY_AND_EQUIP_SLOT_MAX)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid item cell %d", wCell);
			return NULL;
		}
		return m_PlayerSlots->pItems[wCell];
	case DRAGON_SOUL_INVENTORY:
		if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid DS item cell %d", wCell);
			return NULL;
		}
		return m_PlayerSlots->pDSItems[wCell];
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	case BUFF_EQUIPMENT:
		if (wCell >= BUFF_WINDOW_SLOT_MAX_NUM)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid Buff-EQ item cell %d >= %d", wCell, BUFF_WINDOW_SLOT_MAX_NUM);
			return NULL;
		}
		return m_pointsInstant.pBuffEquipmentItem[wCell];
#endif
#ifdef ENABLE_SWITCHBOT
	case SWITCHBOT:
		if (wCell >= SWITCHBOT_SLOT_COUNT)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid switchbot item cell %d", wCell);
			return NULL;
		}
		return m_PlayerSlots->pSwitchbotItems[wCell];
#endif
	default:
		return NULL;
	}
	return NULL;
}

void CHARACTER::SetItem(TItemPos Cell, LPITEM pItem
#ifdef ENABLE_HIGHLIGHT_NEW_ITEM
	, bool bWereMine
#endif
)
{
	if (!m_PlayerSlots)
		return;

	WORD wCell = Cell.cell;
	BYTE window_type = Cell.window_type;

	if (pItem && pItem->GetOwner())
	{
		assert(!"GetOwner exist");
		return;
	}

	switch (window_type)
	{
	case INVENTORY:
	case EQUIPMENT:
	{
		if (wCell >= INVENTORY_AND_EQUIP_SLOT_MAX)
		{
			sys_err("CHARACTER::SetItem: invalid item cell %d", wCell);
			return;
		}

		LPITEM pOld = m_PlayerSlots->pItems[wCell];

		if (pOld)
		{
			if (wCell < INVENTORY_MAX_NUM)
			{
				for (int i = 0; i < pOld->GetSize(); ++i)
				{
					int p = wCell + (i * 5);

					if (p >= INVENTORY_MAX_NUM)
						continue;

					if (m_PlayerSlots->pItems[p] && m_PlayerSlots->pItems[p] != pOld)
						continue;

					m_PlayerSlots->bItemGrid[p] = 0;
				}
			}
			else
				m_PlayerSlots->bItemGrid[wCell] = 0;
		}

		if (pItem)
		{
			if (wCell < INVENTORY_MAX_NUM)
			{
				for (int i = 0; i < pItem->GetSize(); ++i)
				{
					int p = wCell + (i * 5);

					if (p >= INVENTORY_MAX_NUM)
						continue;

					m_PlayerSlots->bItemGrid[p] = wCell + 1;
				}
			}
			else
				m_PlayerSlots->bItemGrid[wCell] = wCell + 1;
		}

		m_PlayerSlots->pItems[wCell] = pItem;
	}
	break;

	case DRAGON_SOUL_INVENTORY:
	{
		LPITEM pOld = m_PlayerSlots->pDSItems[wCell];

		if (pOld)
		{
			if (wCell < DRAGON_SOUL_INVENTORY_MAX_NUM)
			{
				for (int i = 0; i < pOld->GetSize(); ++i)
				{
					int p = wCell + (i * DRAGON_SOUL_BOX_COLUMN_NUM);

					if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
						continue;

					if (m_PlayerSlots->pDSItems[p] && m_PlayerSlots->pDSItems[p] != pOld)
						continue;

					m_PlayerSlots->wDSItemGrid[p] = 0;
				}
			}
			else
				m_PlayerSlots->wDSItemGrid[wCell] = 0;
		}

		if (pItem)
		{
			if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
			{
				sys_err("CHARACTER::SetItem: invalid DS item cell %d", wCell);
				return;
			}

			if (wCell < DRAGON_SOUL_INVENTORY_MAX_NUM)
			{
				for (int i = 0; i < pItem->GetSize(); ++i)
				{
					int p = wCell + (i * DRAGON_SOUL_BOX_COLUMN_NUM);

					if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
						continue;

					m_PlayerSlots->wDSItemGrid[p] = wCell + 1;
				}
			}
			else
				m_PlayerSlots->wDSItemGrid[wCell] = wCell + 1;
		}

		m_PlayerSlots->pDSItems[wCell] = pItem;
	}
	break;
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	case BUFF_EQUIPMENT:
	{
		if (wCell >= BUFF_WINDOW_SLOT_MAX_NUM) {
			sys_err("CHARACTER::SetItem: invalid BUFF_EQUIPMENT item cell %d", wCell);
			return;
		}
		LPITEM pOld = m_pointsInstant.pBuffEquipmentItem[wCell];

		if (pOld && pItem) {
			return;
		}

		m_pointsInstant.pBuffEquipmentItem[wCell] = pItem;
		if (GetBuffNPCSystem() != NULL) {
			if (GetBuffNPCSystem()->IsSummoned()) {
				GetBuffNPCSystem()->UpdateBuffEquipment();
			}
		}
	}
	break;
#endif
#ifdef ENABLE_SWITCHBOT
	case SWITCHBOT:
	{
		LPITEM pOld = m_PlayerSlots->pSwitchbotItems[wCell];
		if (pItem && pOld)
		{
			return;
		}

		if (wCell >= SWITCHBOT_SLOT_COUNT)
		{
			sys_err("CHARACTER::SetItem: invalid switchbot item cell %d", wCell);
			return;
		}

		if (pItem)
		{
			CSwitchbotManager::Instance().RegisterItem(GetPlayerID(), pItem->GetID(), wCell);
		}
		else
		{
			CSwitchbotManager::Instance().UnregisterItem(GetPlayerID(), wCell);
		}

		m_PlayerSlots->pSwitchbotItems[wCell] = pItem;
	}
	break;
#endif

	default:
		sys_err("Invalid Inventory type %d", window_type);
		return;
	}

	if (GetDesc())
	{
		if (pItem)
		{
			TPacketGCItemSet pack;
			pack.header = HEADER_GC_ITEM_SET;
			pack.Cell = Cell;

			pack.count = pItem->GetCount();
			pack.vnum = pItem->GetVnum();
			pack.flags = pItem->GetFlag();
			pack.anti_flags = pItem->GetAntiFlag();
#ifdef ENABLE_HIGHLIGHT_NEW_ITEM
			pack.highlight = !bWereMine || (Cell.window_type == DRAGON_SOUL_INVENTORY);
#else
			pack.highlight = (Cell.window_type == DRAGON_SOUL_INVENTORY);
#endif

			thecore_memcpy(pack.alSockets, pItem->GetSockets(), sizeof(pack.alSockets));
#if defined(__ITEM_APPLY_RANDOM__)
			thecore_memcpy(pack.aApplyRandom, pItem->GetRandomApplies(), sizeof(pack.aApplyRandom));
#endif
			thecore_memcpy(pack.aAttr, pItem->GetAttributes(), sizeof(pack.aAttr));

#ifndef __INVENTORY_BUFFERING__
				GetDesc()->Packet(&pack, sizeof(TPacketGCItemSet));
#else
				if (!bInvBuff)
					GetDesc()->Packet(&pack, sizeof(TPacketGCItemSet));
				else
					// Put item into set
					AddItemToInvBuff(pItem);
#endif
			if (pItem->GetType() == ITEM_GUILD_GUARD && pItem->GetSocket(0) != 0)
				SendGuildName(pItem->GetSocket(0));
		}
		else
		{
			TPacketGCItemDelDeprecated pack;
			pack.header = HEADER_GC_ITEM_DEL;
			pack.Cell = Cell;
			pack.count = 0;
			pack.vnum = 0;
			memset(pack.alSockets, 0, sizeof(pack.alSockets));
#if defined(__ITEM_APPLY_RANDOM__)
			memset(pack.aApplyRandom, 0, sizeof(pack.aApplyRandom));
#endif
			memset(pack.aAttr, 0, sizeof(pack.aAttr));

			GetDesc()->Packet(&pack, sizeof(TPacketGCItemDelDeprecated));
		}
	}

	if (pItem)
	{
		pItem->SetCell(this, wCell);
		switch (window_type)
		{
		case INVENTORY:
		case EQUIPMENT:
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
			if ((wCell < INVENTORY_MAX_NUM) || (BELT_INVENTORY_SLOT_START <= wCell && BELT_INVENTORY_SLOT_END > wCell) || (SKILL_BOOK_INVENTORY_SLOT_START <= wCell && SKILL_BOOK_INVENTORY_SLOT_END > wCell) || (UPGRADE_ITEMS_INVENTORY_SLOT_START <= wCell && UPGRADE_ITEMS_INVENTORY_SLOT_END > wCell) || (STONE_INVENTORY_SLOT_START <= wCell && STONE_INVENTORY_SLOT_END > wCell) || (BOX_INVENTORY_SLOT_START <= wCell && BOX_INVENTORY_SLOT_END > wCell) || (EFSUN_INVENTORY_SLOT_START <= wCell && EFSUN_INVENTORY_SLOT_END > wCell) || (CICEK_INVENTORY_SLOT_START <= wCell && CICEK_INVENTORY_SLOT_END > wCell))
				pItem->SetWindow(INVENTORY);
#else
			if ((wCell < INVENTORY_MAX_NUM) || (BELT_INVENTORY_SLOT_START <= wCell && BELT_INVENTORY_SLOT_END > wCell))
				pItem->SetWindow(INVENTORY);
#endif
			else
				pItem->SetWindow(EQUIPMENT);
			break;
		case DRAGON_SOUL_INVENTORY:
			pItem->SetWindow(DRAGON_SOUL_INVENTORY);
			break;
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
		case BUFF_EQUIPMENT:
			pItem->SetWindow(BUFF_EQUIPMENT);
			break;
#endif
#ifdef ENABLE_SWITCHBOT
		case SWITCHBOT:
			pItem->SetWindow(SWITCHBOT);
			break;
#endif		
		}
	}
}

#ifdef ENABLE_SYSTEM_RUNE
void CHARACTER::CheckRuneExtraBonus(LPITEM item, BYTE type) {
    switch (type) {
        case 0: {
#ifdef ENABLE_EXTRABONUS_SYSTEM
            if (auto affectType = RUNE_AFFECT_START + item->GetType(); FindAffect(affectType)) {
                RemoveAffect(affectType);
            }

            if (item->GetSocket(0) > 0) {
                for (const auto& apply : item->GetProto()->aApplies) {
                    if (apply.bType == APPLY_NONE) {
                        continue;
                    }

                    int reinforcement = (apply.lValue * RUNE_EXTRABONUS_VALUE) / 100;
                    AddAffect(
                        RUNE_AFFECT_START + item->GetType(),
                        aApplyInfo[apply.bType].bPointType,
                        reinforcement,
                        AFF_NONE,
                        60 * 60 * 60 * 365,
                        0,
                        false
                    );
                }
            }
#endif
            auto allRunesEquipped = std::array{WEAR_RUNE, WEAR_RUNE_RED, WEAR_RUNE_BLUE, WEAR_RUNE_GREEN, WEAR_RUNE_YELLOW, WEAR_RUNE_BLACK};
            bool allEquipped = std::all_of(allRunesEquipped.begin(), allRunesEquipped.end(), [this](int wearSlot) {
                return GetWear(wearSlot) != nullptr;
            });

            if (allEquipped) {
                bool allRunesHaveValue5 = true;
                bool allRunesHaveValue9 = true;

                for (int slot : allRunesEquipped) {
                    if (auto* pItem = GetWear(slot)) {
                        int value1 = pItem->GetValue(1);

                        if (value1 != 5) {
                            allRunesHaveValue5 = false;
                        }
                        if (value1 != 9) {
                            allRunesHaveValue9 = false;
                        }
                    }
                }

                if (allRunesHaveValue5) {
                    if (!FindAffect(AFFECT_RUNE_DECK5)) {
                        AddAffect(AFFECT_RUNE_DECK5, POINT_FINAL_DMG_BONUS, 5, AFF_NONE, 60 * 60 * 60 * 365, 0, false);
                        AddAffect(AFFECT_RUNE_DECK5, POINT_NORMAL_HIT_DAMAGE_BONUS, 15, AFF_NONE, 60 * 60 * 60 * 365, 0, false);
                        AddAffect(AFFECT_RUNE_DECK5, POINT_ATTBONUS_HUMAN, 25, AFF_NONE, 60 * 60 * 60 * 365, 0, false);
                        AddAffect(AFFECT_RUNE_DECK5, POINT_ATTBONUS_KLASY, 10, AFF_NONE, 60 * 60 * 60 * 365, 0, false);
                    }
                } else if (FindAffect(AFFECT_RUNE_DECK5)) {
                    RemoveAffect(AFFECT_RUNE_DECK5);
                }

                if (allRunesHaveValue9) {
                    if (!FindAffect(AFFECT_RUNE_DECK9)) {
                        AddAffect(AFFECT_RUNE_DECK9, POINT_FINAL_DMG_BONUS, 10, AFF_NONE, 60 * 60 * 60 * 365, 0, false);
                        AddAffect(AFFECT_RUNE_DECK9, POINT_NORMAL_HIT_DAMAGE_BONUS, 25, AFF_NONE, 60 * 60 * 60 * 365, 0, false);
                        AddAffect(AFFECT_RUNE_DECK9, POINT_ATTBONUS_HUMAN, 40, AFF_NONE, 60 * 60 * 60 * 365, 0, false);
                    }
                } else if (FindAffect(AFFECT_RUNE_DECK9)) {
                    RemoveAffect(AFFECT_RUNE_DECK9);
                }
            }
            break;
        }

        case 1: {
#ifdef ENABLE_EXTRABONUS_SYSTEM
            if (auto affectType = RUNE_AFFECT_START + item->GetType(); FindAffect(affectType)) {
                RemoveAffect(affectType);
            }
#endif
            if (FindAffect(AFFECT_RUNE_DECK5)) {
                RemoveAffect(AFFECT_RUNE_DECK5);
            }

            if (FindAffect(AFFECT_RUNE_DECK9)) {
                RemoveAffect(AFFECT_RUNE_DECK9);
            }
            break;
        }

        default:
            break;
    }
}
#endif

#ifdef ENABLE_NEW_PET_SYSTEM
void CHARACTER::CheckPetExtraBonus(LPITEM item, BYTE type) {
    switch (type) {
        case 0: {
            if (auto affectType = PET_AFFECT_START + item->GetValue(0); FindAffect(affectType)) {
                RemoveAffect(affectType);
            }

            if (item->GetSocket(0) > 0) {
                for (const auto& apply : item->GetProto()->aApplies) {
                    if (apply.bType == APPLY_NONE) {
                        continue;
                    }

                    int reinforcement = (apply.lValue * PET_EXTRABONUS_VALUE) / 100;
                    AddAffect(
                        PET_AFFECT_START + item->GetValue(0),
                        aApplyInfo[apply.bType].bPointType,
                        reinforcement,
                        AFF_NONE,
                        60 * 60 * 60 * 365,
                        0,
                        false
                    );
                }
            }
            break;
        }

        case 1: {
            if (auto affectType = RUNE_AFFECT_START + item->GetValue(0); FindAffect(affectType)) {
                RemoveAffect(affectType);
            }
            break;
        }

        default:
            break;
    }
}
#endif

LPITEM CHARACTER::GetWear(BYTE bCell) const
{
	if (!m_PlayerSlots)
		return nullptr;

	if (bCell >= WEAR_MAX_NUM + DRAGON_SOUL_DECK_MAX_NUM * DS_SLOT_MAX)
	{
		sys_err("CHARACTER::GetWear: invalid wear cell %d", bCell);
		return NULL;
	}

	return m_PlayerSlots->pItems[INVENTORY_MAX_NUM + bCell];
}

void CHARACTER::SetWear(BYTE bCell, LPITEM item)
{
	if (bCell >= WEAR_MAX_NUM + DRAGON_SOUL_DECK_MAX_NUM * DS_SLOT_MAX)
	{
		sys_err("CHARACTER::SetItem: invalid item cell %d", bCell);
		return;
	}

	SetItem(TItemPos(INVENTORY, INVENTORY_MAX_NUM + bCell), item);
}

void CHARACTER::ClearItem()
{
	int		i;
	LPITEM	item;

	for (i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
	{
		if ((item = GetInventoryItem(i)))
		{
			if (!item->GetProto())
			{
				sys_err("ClearItem: slot %d has item with null proto (dangling pointer?) for char %s, nulling slot", i, GetName());
				m_PlayerSlots->pItems[i] = NULL;
				continue;
			}

			if (item->GetOwner() != this)
			{
				sys_err("ClearItem: slot %d item owner mismatch (use-after-free/exploit?) char %s vnum %u, nulling slot", i, GetName(), item->GetVnum());
				m_PlayerSlots->pItems[i] = NULL;
				continue;
			}

			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			// Force-null the slot: Unequip() may fail its self-consistency check
			// and return false without clearing the slot, leaving a dangling
			// pointer that causes ComputePoints to crash later (e.g. ClearAffect).
			m_PlayerSlots->pItems[i] = nullptr;
			M2_DESTROY_ITEM(item);

			SyncQuickslot(QUICKSLOT_TYPE_ITEM, i, 255);
		}
	}
	for (i = 0; i < DRAGON_SOUL_INVENTORY_MAX_NUM; ++i)
	{
		if ((item = GetItem(TItemPos(DRAGON_SOUL_INVENTORY, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			m_PlayerSlots->pDSItems[i] = nullptr;
			M2_DESTROY_ITEM(item);
		}
	}
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	for (i = 0; i < BUFF_WINDOW_SLOT_MAX_NUM; ++i)
	{
		if ((item = GetItem(TItemPos(BUFF_EQUIPMENT, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}
#endif
#ifdef ENABLE_SWITCHBOT
	for (i = 0; i < SWITCHBOT_SLOT_COUNT; ++i)
	{
		if ((item = GetItem(TItemPos(SWITCHBOT, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}
#endif
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	for (i = 0; i < SKILL_BOOK_INVENTORY_MAX_NUM; ++i)
	{
		if ((item = GetItem(TItemPos(SKILL_BOOK_INVENTORY, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}

	for (i = 0; i < UPGRADE_ITEMS_INVENTORY_MAX_NUM; ++i)
	{
		if ((item = GetItem(TItemPos(UPGRADE_ITEMS_INVENTORY, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}

	for (i = 0; i < STONE_INVENTORY_MAX_NUM; ++i)
	{
		if ((item = GetItem(TItemPos(STONE_INVENTORY, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}

	for (i = 0; i < BOX_INVENTORY_MAX_NUM; ++i)
	{
		if ((item = GetItem(TItemPos(BOX_INVENTORY, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}

	for (i = 0; i < EFSUN_INVENTORY_MAX_NUM; ++i)
	{
		if ((item = GetItem(TItemPos(EFSUN_INVENTORY, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}

	for (i = 0; i < CICEK_INVENTORY_MAX_NUM; ++i)
	{
		if ((item = GetItem(TItemPos(CICEK_INVENTORY, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}
#endif
}

bool CHARACTER::IsEmptyItemGrid(TItemPos Cell, BYTE bSize, int iExceptionCell) const
{
	if (!m_PlayerSlots)
		return false;

	switch (Cell.window_type)
	{
	case INVENTORY:
	{
		WORD bCell = Cell.cell;

		++iExceptionCell;

		if (Cell.IsBeltInventoryPosition())
		{
			LPITEM beltItem = GetWear(WEAR_BELT);

			if (NULL == beltItem)
				return false;

			if (false == CBeltInventoryHelper::IsAvailableCell(bCell - BELT_INVENTORY_SLOT_START, beltItem->GetValue(0)))
				return false;

			if (m_PlayerSlots->bItemGrid[bCell])
			{
				if (m_PlayerSlots->bItemGrid[bCell] == iExceptionCell)
					return true;

				return false;
			}

			if (bSize == 1)
				return true;

		}
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
		else if (Cell.IsSkillBookInventoryPosition())
		{
			if (bCell < SKILL_BOOK_INVENTORY_SLOT_START)
				return false;

			if (bCell > SKILL_BOOK_INVENTORY_SLOT_END)
				return false;

			if (m_PlayerSlots->bItemGrid[bCell] == (UINT)iExceptionCell)
			{
				if (bSize == 1)
					return true;

				int j = 1;
				UINT bPage = bCell / (SKILL_BOOK_INVENTORY_MAX_NUM / 3);

				do
				{
					UINT p = bCell + (5 * j);

					if (p >= SKILL_BOOK_INVENTORY_MAX_NUM)
						return false;

					if (p / (SKILL_BOOK_INVENTORY_MAX_NUM / 3) != bPage)
						return false;

					if (m_PlayerSlots->bItemGrid[p])
						if (m_PlayerSlots->bItemGrid[p] != iExceptionCell)
							return false;
				} while (++j < bSize);

				return true;
			}
		}
		else if (Cell.IsUpgradeItemsInventoryPosition())
		{
			if (bCell < UPGRADE_ITEMS_INVENTORY_SLOT_START)
				return false;

			if (bCell > UPGRADE_ITEMS_INVENTORY_SLOT_END)
				return false;

			if (m_PlayerSlots->bItemGrid[bCell] == (UINT)iExceptionCell)
			{
				if (bSize == 1)
					return true;

				int j = 1;
				UINT bPage = bCell / (UPGRADE_ITEMS_INVENTORY_MAX_NUM / 3);

				do
				{
					UINT p = bCell + (5 * j);

					if (p >= UPGRADE_ITEMS_INVENTORY_MAX_NUM)
						return false;

					if (p / (UPGRADE_ITEMS_INVENTORY_MAX_NUM / 3) != bPage)
						return false;

					if (m_PlayerSlots->bItemGrid[p])
						if (m_PlayerSlots->bItemGrid[p] != iExceptionCell)
							return false;
				} while (++j < bSize);

				return true;
			}
		}
		else if (Cell.IsStoneInventoryPosition())
		{
			if (bCell < STONE_INVENTORY_SLOT_START)
				return false;

			if (bCell > STONE_INVENTORY_SLOT_END)
				return false;

			if (m_PlayerSlots->bItemGrid[bCell] == (UINT)iExceptionCell)
			{
				if (bSize == 1)
					return true;

				int j = 1;
				UINT bPage = bCell / (STONE_INVENTORY_MAX_NUM / 3);

				do
				{
					UINT p = bCell + (5 * j);

					if (p >= STONE_INVENTORY_MAX_NUM)
						return false;

					if (p / (STONE_INVENTORY_MAX_NUM / 3) != bPage)
						return false;

					if (m_PlayerSlots->bItemGrid[p])
						if (m_PlayerSlots->bItemGrid[p] != iExceptionCell)
							return false;
				} while (++j < bSize);

				return true;
			}
		}
		else if (Cell.IsBoxInventoryPosition())
		{
			if (bCell < BOX_INVENTORY_SLOT_START)
				return false;

			if (bCell > BOX_INVENTORY_SLOT_END)
				return false;

			if (m_PlayerSlots->bItemGrid[bCell] == (UINT)iExceptionCell)
			{
				if (bSize == 1)
					return true;

				int j = 1;
				UINT bPage = bCell / (BOX_INVENTORY_MAX_NUM / 3);

				do
				{
					UINT p = bCell + (5 * j);

					if (p >= BOX_INVENTORY_MAX_NUM)
						return false;

					if (p / (BOX_INVENTORY_MAX_NUM / 3) != bPage)
						return false;

					if (m_PlayerSlots->bItemGrid[p])
						if (m_PlayerSlots->bItemGrid[p] != iExceptionCell)
							return false;
				} while (++j < bSize);

				return true;
			}
		}
		else if (Cell.IsEfsunInventoryPosition())
		{
			if (bCell < EFSUN_INVENTORY_SLOT_START)
				return false;

			if (bCell > EFSUN_INVENTORY_SLOT_END)
				return false;

			if (m_PlayerSlots->bItemGrid[bCell] == (UINT)iExceptionCell)
			{
				if (bSize == 1)
					return true;

				int j = 1;
				UINT bPage = bCell / (EFSUN_INVENTORY_MAX_NUM / 3);

				do
				{
					UINT p = bCell + (5 * j);

					if (p >= EFSUN_INVENTORY_MAX_NUM)
						return false;

					if (p / (EFSUN_INVENTORY_MAX_NUM / 3) != bPage)
						return false;

					if (m_PlayerSlots->bItemGrid[p])
						if (m_PlayerSlots->bItemGrid[p] != iExceptionCell)
							return false;
				} while (++j < bSize);

				return true;
			}
		}
		else if (Cell.IsCicekInventoryPosition())
		{
			if (bCell < CICEK_INVENTORY_SLOT_START)
				return false;

			if (bCell > CICEK_INVENTORY_SLOT_END)
				return false;

			if (m_PlayerSlots->bItemGrid[bCell] == (UINT)iExceptionCell)
			{
				if (bSize == 1)
					return true;

				int j = 1;
				UINT bPage = bCell / (CICEK_INVENTORY_MAX_NUM / 3);

				do
				{
					UINT p = bCell + (5 * j);

					if (p >= CICEK_INVENTORY_MAX_NUM)
						return false;

					if (p / (CICEK_INVENTORY_MAX_NUM / 3) != bPage)
						return false;

					if (m_PlayerSlots->bItemGrid[p])
						if (m_PlayerSlots->bItemGrid[p] != iExceptionCell)
							return false;
				} while (++j < bSize);

				return true;
			}
		}
#endif
		else if (bCell >= INVENTORY_MAX_NUM)
			return false;

		if (m_PlayerSlots->bItemGrid[bCell])
		{
			if (m_PlayerSlots->bItemGrid[bCell] == iExceptionCell)
			{
				if (bSize == 1)
					return true;

				int j = 1;
				BYTE bPage = bCell / (INVENTORY_MAX_NUM / INVENTORY_PAGE_COUNT);

				do
				{
					BYTE p = bCell + (5 * j);

					if (p >= INVENTORY_MAX_NUM)
						return false;

					if (p / (INVENTORY_MAX_NUM / INVENTORY_PAGE_COUNT) != bPage)
						return false;

					if (m_PlayerSlots->bItemGrid[p])
						if (m_PlayerSlots->bItemGrid[p] != iExceptionCell)
							return false;
				} while (++j < bSize);

				return true;
			}
			else
				return false;
		}

		if (1 == bSize)
			return true;
		else
		{
			int j = 1;
			BYTE bPage = bCell / (INVENTORY_MAX_NUM / INVENTORY_PAGE_COUNT);

			do
			{
				BYTE p = bCell + (5 * j);

				if (p >= INVENTORY_MAX_NUM)
					return false;

				if (p / (INVENTORY_MAX_NUM / INVENTORY_PAGE_COUNT) != bPage)
					return false;

				if (m_PlayerSlots->bItemGrid[p])
					if (m_PlayerSlots->bItemGrid[p] != iExceptionCell)
						return false;
			} while (++j < bSize);

			return true;
		}
	}
	break;
	case DRAGON_SOUL_INVENTORY:
	{
		WORD wCell = Cell.cell;
		if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
			return false;

		iExceptionCell++;

		if (m_PlayerSlots->wDSItemGrid[wCell])
		{
			if (m_PlayerSlots->wDSItemGrid[wCell] == iExceptionCell)
			{
				if (bSize == 1)
					return true;

				int j = 1;

				do
				{
					int p = wCell + (DRAGON_SOUL_BOX_COLUMN_NUM * j);

					if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
						return false;

					if (m_PlayerSlots->wDSItemGrid[p])
						if (m_PlayerSlots->wDSItemGrid[p] != iExceptionCell)
							return false;
				} while (++j < bSize);

				return true;
			}
			else
				return false;
		}

		if (1 == bSize)
			return true;
		else
		{
			int j = 1;

			do
			{
				int p = wCell + (DRAGON_SOUL_BOX_COLUMN_NUM * j);

				if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
					return false;

				if (m_PlayerSlots->bItemGrid[p])
					if (m_PlayerSlots->wDSItemGrid[p] != iExceptionCell)
						return false;
			} while (++j < bSize);

			return true;
		}
	}
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	case BUFF_EQUIPMENT:
	{
		UINT wCell = Cell.cell;
		if (wCell >= BUFF_WINDOW_SLOT_MAX_NUM)
		{
			return false;
		}

		if (m_pointsInstant.pBuffEquipmentItem[wCell])
		{
			return false;
		}

		return true;
	}
#endif
#ifdef ENABLE_SWITCHBOT
	case SWITCHBOT:
	{
		WORD wCell = Cell.cell;
		if (wCell >= SWITCHBOT_SLOT_COUNT)
		{
			return false;
		}

		if (m_PlayerSlots->pSwitchbotItems[wCell])
		{
			return false;
		}

		return true;
	}
#endif
	}

	return false;
}

int CHARACTER::GetEmptyInventory(BYTE size) const
{
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
		if (IsEmptyItemGrid(TItemPos(INVENTORY, i), size))
			return i;
	return -1;
}

int CHARACTER::GetEmptyInventoryFromIndex(WORD index, BYTE itemSize) const //SPLIT ITEMS
{
	if (index > INVENTORY_MAX_NUM)
		return -1;
	
	for (WORD i = index; i < INVENTORY_MAX_NUM; ++i)
		if (IsEmptyItemGrid(TItemPos (INVENTORY, i), itemSize))
			return i;
	return -1;
}

int CHARACTER::GetEmptyUpgradeItemsInventoryFromIndex(WORD index, BYTE itemSize) const //SPLIT ITEMS
{
	if (index > 548)
		return -1;

	for (WORD i = index; i < 548; ++i)
		if (IsEmptyItemGrid(TItemPos (INVENTORY, i), itemSize))
			return i;
	return -1;
}

int CHARACTER::GetEmptySkillBookInventoryFromIndex(WORD index, BYTE itemSize) const //SPLIT ITEMS
{
	if (index > 413)
		return -1;
	
	for (WORD i = index; i < 413; ++i)
		if (IsEmptyItemGrid(TItemPos (INVENTORY, i), itemSize))
			return i;
	return -1;
}

int CHARACTER::GetEmptyStoneInventoryFromIndex(WORD index, BYTE itemSize) const //SPLIT ITEMS
{
	if (index > 683)
		return -1;
	
	for (WORD i = index; i < 683; ++i)
		if (IsEmptyItemGrid(TItemPos (INVENTORY, i), itemSize))
			return i;
	return -1;
}

int CHARACTER::GetEmptyBoxInventoryFromIndex(WORD index, BYTE itemSize) const //SPLIT ITEMS
{
	if (index > 818)
		return -1;
	
	for (WORD i = index; i < 818; ++i)
		if (IsEmptyItemGrid(TItemPos (INVENTORY, i), itemSize))
			return i;
	return -1;
}

int CHARACTER::GetEmptyEfsunInventoryFromIndex(WORD index, BYTE itemSize) const //SPLIT ITEMS
{
	if (index > 953)
		return -1;
	
	for (WORD i = index; i < 953; ++i)
		if (IsEmptyItemGrid(TItemPos (INVENTORY, i), itemSize))
			return i;
	return -1;
}

int CHARACTER::GetEmptyCicekInventoryFromIndex(WORD index, BYTE itemSize) const //SPLIT ITEMS
{
	if (index > 1088)
		return -1;
	
	for (WORD i = index; i < 1088; ++i)
		if (IsEmptyItemGrid(TItemPos (INVENTORY, i), itemSize))
			return i;
	return -1;
}

int CHARACTER::GetEmptyDragonSoulInventory(LPITEM pItem) const
{
	if (NULL == pItem || !pItem->IsDragonSoul())
		return -1;
	if (!DragonSoul_IsQualified())
	{
		return -1;
	}
	BYTE bSize = pItem->GetSize();
	WORD wBaseCell = DSManager::instance().GetBasePosition(pItem);

	if (WORD_MAX == wBaseCell)
		return -1;

	for (int i = 0; i < DRAGON_SOUL_BOX_SIZE; ++i)
		if (IsEmptyItemGrid(TItemPos(DRAGON_SOUL_INVENTORY, i + wBaseCell), bSize))
			return i + wBaseCell;

	return -1;
}

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
int CHARACTER::GetEmptySkillBookInventory(BYTE size) const
{
	for (int i = SKILL_BOOK_INVENTORY_SLOT_START; i < SKILL_BOOK_INVENTORY_SLOT_END; ++i)
		if (IsEmptyItemGrid(TItemPos(INVENTORY, i), size))
			return i;

	return -1;
}

int CHARACTER::GetEmptyUpgradeItemsInventory(BYTE size) const
{
	for (int i = UPGRADE_ITEMS_INVENTORY_SLOT_START; i < UPGRADE_ITEMS_INVENTORY_SLOT_END; ++i)
		if (IsEmptyItemGrid(TItemPos(INVENTORY, i), size))
			return i;

	return -1;
}

int CHARACTER::GetEmptyStoneInventory(BYTE size) const
{
	for (int i = STONE_INVENTORY_SLOT_START; i < STONE_INVENTORY_SLOT_END; ++i)
		if (IsEmptyItemGrid(TItemPos(INVENTORY, i), size))
			return i;

	return -1;
}

int CHARACTER::GetEmptyBoxInventory(BYTE size) const
{
	for (int i = BOX_INVENTORY_SLOT_START; i < BOX_INVENTORY_SLOT_END; ++i)
		if (IsEmptyItemGrid(TItemPos(INVENTORY, i), size))
			return i;

	return -1;
}

int CHARACTER::GetEmptyEfsunInventory(BYTE size) const
{
	for (int i = EFSUN_INVENTORY_SLOT_START; i < EFSUN_INVENTORY_SLOT_END; ++i)
		if (IsEmptyItemGrid(TItemPos(INVENTORY, i), size))
			return i;

	return -1;
}

int CHARACTER::GetEmptyCicekInventory(BYTE size) const
{
	for (int i = CICEK_INVENTORY_SLOT_START; i < CICEK_INVENTORY_SLOT_END; ++i)
		if (IsEmptyItemGrid(TItemPos(INVENTORY, i), size))
			return i;

	return -1;
}
#endif

void CHARACTER::CopyDragonSoulItemGrid(std::vector<WORD>& vDragonSoulItemGrid) const
{
	vDragonSoulItemGrid.resize(DRAGON_SOUL_INVENTORY_MAX_NUM);

	std::copy(m_PlayerSlots->wDSItemGrid, m_PlayerSlots->wDSItemGrid + DRAGON_SOUL_INVENTORY_MAX_NUM, vDragonSoulItemGrid.begin());
}

int CHARACTER::CountEmptyInventory() const
{
	int	count = 0;

// #ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
// 	for (int i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
// #else
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
// #endif
		if (GetInventoryItem(i))
			count += GetInventoryItem(i)->GetSize();

	return (INVENTORY_MAX_NUM - count);
}

void TransformRefineItem(LPITEM pkOldItem, LPITEM pkNewItem)
{
	// ACCESSORY_REFINE
	if (pkOldItem->IsAccessoryForSocket())
	{
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			pkNewItem->SetSocket(i, pkOldItem->GetSocket(i));
		}
		//pkNewItem->StartAccessorySocketExpireEvent();
	}
	// END_OF_ACCESSORY_REFINE
	else
	{
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			if (!pkOldItem->GetSocket(i))
				break;
			else
				pkNewItem->SetSocket(i, 1);
		}

		int slot = 0;

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			int32_t socket = pkOldItem->GetSocket(i);

			if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
				pkNewItem->SetSocket(slot++, socket);
		}

	}

	pkOldItem->CopyAttributeTo(pkNewItem);
}

void NotifyRefineSuccess(LPCHARACTER ch, LPITEM item, const char* way)
{
	if (NULL != ch && item != NULL)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "RefineSuceeded");

		LogManager::instance().RefineLog(ch->GetPlayerID(), item->GetName(), item->GetID(), item->GetRefineLevel(), 1, way);
	}
}

void NotifyRefineFail(LPCHARACTER ch, LPITEM item, const char* way, int success = 0)
{
	if (NULL != ch && NULL != item)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "RefineFailed");

		LogManager::instance().RefineLog(ch->GetPlayerID(), item->GetName(), item->GetID(), item->GetRefineLevel(), success, way);
	}
}

void CHARACTER::SetRefineNPC(LPCHARACTER ch)
{
	if (ch != NULL)
	{
		m_dwRefineNPCVID = ch->GetVID();
	}
	else
	{
		m_dwRefineNPCVID = 0;
	}
}

// Add this helper function to check item level limit
bool CHARACTER::CanRefineItemByLevel(LPITEM item, int refineType)
{
    if (!item)
        return false;
    
    TItemTable* pProto = ITEM_MANAGER::instance().GetTable(item->GetVnum());
    if (!pProto)
        return false;
    
    // Find level limit from aLimits array
    int itemLevelLimit = 0;
    for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
    {
        if (pProto->aLimits[i].bType == LIMIT_LEVEL)
        {
            itemLevelLimit = pProto->aLimits[i].lValue;
            break;
        }
    }
    
    // METAL_25 can only be used on items with level limit >= 201
    if (refineType == REFINE_TYPE_METAL_25)
    {
        if (itemLevelLimit < 201)
        {
			ChatPacket(CHAT_TYPE_INFO, "[LS;10070]");
            return false;
        }
    }
    // All other refine types cannot be used on items with level limit >= 201
    else if (refineType != REFINE_TYPE_METAL_25)
    {
        if (itemLevelLimit >= 201)
        {
			ChatPacket(CHAT_TYPE_INFO, "[LS;10071]");
            return false;
        }
    }
    
    return true;
}

bool CHARACTER::DoRefine(LPITEM item, bool bMoneyOnly)
{
    if (!CanHandleItem(true))
    {
        ClearRefineMode();
        return false;
    }

    // Add level restriction check for normal refine
    if (!CanRefineItemByLevel(item, REFINE_TYPE_NORMAL))
    {
        ClearRefineMode();
        return false;
    }

	if (quest::CQuestManager::instance().GetEventFlag("update_refine_time") != 0)
	{
		if (get_global_time() < quest::CQuestManager::instance().GetEventFlag("update_refine_time") + (60 * 5))
		{
			sys_log(0, "can't refine %d %s", GetPlayerID(), GetName());
			return false;
		}
	}

	const TRefineTable* prt = CRefineManager::instance().GetRefineRecipe(item->GetRefineSet());

	if (!prt)
		return false;

	if (item->GetType() == ITEM_GUILD_GUARD)
	{
		if (!GetGuild())
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;10118]");
			return false;
		}
		if (!GetGuild()->HasGradeAuth(GetGuild()->GetMember(GetPlayerID())->grade, GUILD_AUTH_MANAGE_GUARD))
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;10119]");
			return false;
		}
		//if (!CGuildManager::instance().CanFullManageGuildOnThisMap(this))
		//{
		//	ChatPacket(CHAT_TYPE_INFO, "[LS;10120]");
		//	return false;
		//}
	}

	DWORD result_vnum = item->GetRefinedVnum();

	// REFINE_COST
	int cost = ComputeRefineFee(prt->cost);
	int cost2 = ComputeRefineFee(prt->cost2);
	int cost3 = ComputeRefineFee(prt->cost3);

	int RefineChance = GetQuestFlag("main_quest_lv7.refine_chance");

	if (RefineChance > 0)
	{
		if (!item->CheckItemUseLevel(20) || item->GetType() != ITEM_WEAPON)
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;1003]");
			return false;
		}

		cost = 0;
		SetQuestFlag("main_quest_lv7.refine_chance", RefineChance - 1);
	}
	// END_OF_REFINE_COST

	if (result_vnum == 0)
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;991]");
		return false;
	}

	if (item->GetType() == ITEM_USE && item->GetSubType() == USE_TUNING)
		return false;

	TItemTable* pProto = ITEM_MANAGER::instance().GetTable(item->GetRefinedVnum());

	if (!pProto)
	{
		sys_err("DoRefine NOT GET ITEM PROTO %d", item->GetRefinedVnum());
		ChatPacket(CHAT_TYPE_INFO, "[LS;1002]");
		return false;
	}

	// REFINE_COST
	bool isRefineGuildGuardItem = false;
	if (item->GetType() == ITEM_GUILD_GUARD)
	{
		if (GetGuild()->GetGuildMoney() < cost)
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;10121]");
			return false;
		}
		isRefineGuildGuardItem = true;
	}
	else
	{
		if (GetGold() < cost)
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;67]");
			return false;
		}
		
		if (GetCheque() < cost2)
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;2203]");
			return false;
		}
		
		if (GetPktOsiag() < cost3)
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;2204]");
			return false;
		}
	}

	if (!bMoneyOnly && !RefineChance)
	{
		for (int i = 0; i < prt->material_count; ++i)
		{
			if (CountSpecifyItem(prt->materials[i].vnum) < prt->materials[i].count)
			{
				if (test_server)
				{
					ChatPacket(CHAT_TYPE_INFO, "Find %d, count %d, require %d", prt->materials[i].vnum, CountSpecifyItem(prt->materials[i].vnum), prt->materials[i].count);
				}
				ChatPacket(CHAT_TYPE_INFO, "[LS;1035]");
				return false;
			}
		}

		for (int i = 0; i < prt->material_count; ++i)
			RemoveSpecifyItem(prt->materials[i].vnum, prt->materials[i].count);
	}

	int prob = number(1, 100);
	
	bool isHighLevelItem = false;
	TItemTable* pItemProto = ITEM_MANAGER::instance().GetTable(item->GetVnum());
	if (pItemProto)
	{
		for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
		{
			if (pItemProto->aLimits[i].bType == LIMIT_LEVEL && pItemProto->aLimits[i].lValue >= 201)
			{
				isHighLevelItem = true;
				break;
			}
		}
	}
	
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
	if (GetTitleAchievement() == 11)
	{
		prob -= 5;
	}
	else if (GetTitleAchievementPremium() == 11)
	{
		prob -= 5;
	}
	else if (GetTitleAchievement() == 21)
	{
		prob -= 7;
	}
	else if (GetTitleAchievementPremium() == 21)
	{
		prob -= 7;
	}
#endif
	if (IsRefineThroughGuild() || bMoneyOnly)
		prob -= 10;

	// END_OF_REFINE_COST

	if (prob <= prt->prob)
	{
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(result_vnum, 1, 0, false);

		if (pkNewItem)
		{
			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
			LogManager::instance().ItemLog(this, pkNewItem, "REFINE SUCCESS", pkNewItem->GetName());

			BYTE bCell = item->GetCell();

			// DETAIL_REFINE_LOG
			NotifyRefineSuccess(this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -cost);
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");
			// END_OF_DETAIL_REFINE_LOG

			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell));
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);

			sys_log(0, "Refine Success %d", cost);
			pkNewItem->AttrLog();
			//PointChange(POINT_GOLD, -cost);
			sys_log(0, "PayPee %d", cost);
			
			if (isRefineGuildGuardItem)
				GetGuild()->ChangeMoney(-cost);
			else
				PayRefineFee(cost);
		
			PointChange(POINT_CHEQUE, -cost2);
			PointChange(POINT_PKT_OSIAG, -cost3);
			sys_log(0, "PayPee End %d", cost);
#ifdef WEEKLY_RANK_BYLUZER
			PointChange(POINT_WEEKLY6, 1);
			CheckWeekly();
#endif
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
			const int itemRefined = GetQuestFlag("AchievementTitle.refine_succes") + 1;
			SetQuestFlag("AchievementTitle.refine_succes", itemRefined);
					
			CTitleAchievementTitle::instance().CheckAndUnlockTitles(this);
					
			// Send progress update to client
			ChatPacket(CHAT_TYPE_COMMAND, "title_progress_refine_success %d", itemRefined);
#endif
#ifdef _ENABLE_BATTLEPASS_
			BattlePassManager::Instance().Notify(MISSION_TYPE_REFINE, this, item->GetVnum(), 1);
#endif
#ifdef RANKING_SYSTEM
			CServerRankingManager::instance().IncServerRankValue(this, SERVER_RANK_TYPE_REFINE);
			CRankingManager::instance().IncRankValue(this, RANK_TYPE_REFINE, 0);
#endif
		}
		else
		{
			// DETAIL_REFINE_LOG

			sys_err("cannot create item %u", result_vnum);
			NotifyRefineFail(this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
			// END_OF_DETAIL_REFINE_LOG
		}
	}
	else
	{
		DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -cost);
		NotifyRefineFail(this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
		item->AttrLog();
		ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE FAIL)");

		//PointChange(POINT_GOLD, -cost);
		if (isRefineGuildGuardItem)
			GetGuild()->ChangeMoney(-cost);
		else
			PayRefineFee(cost);
		
		PointChange(POINT_CHEQUE, -cost2);
		PointChange(POINT_PKT_OSIAG, -cost3);

#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
		const int itemRefineFailed = GetQuestFlag("AchievementTitle.refine_fail") + 1;
		SetQuestFlag("AchievementTitle.refine_fail", itemRefineFailed);
		
		CTitleAchievementTitle::instance().CheckAndUnlockTitles(this);
		
		// Send progress update to client
		ChatPacket(CHAT_TYPE_COMMAND, "title_progress_refine_fail %d", itemRefineFailed);
#endif
	}

	return true;
}

enum enum_RefineScrolls
{
	CHUKBOK_SCROLL = 0,
	HYUNIRON_CHN = 1,
	YONGSIN_SCROLL = 2,
	MUSIN_SCROLL = 3,
	YAGONG_SCROLL = 4,
	MEMO_SCROLL = 5,
	BDRAGON_SCROLL = 6,
	METAL_10 = 9,
	METAL_20 = 8,
	METAL_25 = 10,
};

bool CHARACTER::DoRefineWithScroll(LPITEM item)
{
	if (!CanHandleItem(true))
	{
		ClearRefineMode();
		return false;
	}

	ClearRefineMode();

	if (quest::CQuestManager::instance().GetEventFlag("update_refine_time") != 0)
	{
		if (get_global_time() < quest::CQuestManager::instance().GetEventFlag("update_refine_time") + (60 * 5))
		{
			sys_log(0, "can't refine %d %s", GetPlayerID(), GetName());
			return false;
		}
	}

	// Get the scroll to determine refine type and check level restrictions
	LPITEM pkItemScroll;
	if (m_iRefineAdditionalCell < 0)
		return false;

	pkItemScroll = GetInventoryItem(m_iRefineAdditionalCell);
	if (!pkItemScroll)
		return false;

	if (!(pkItemScroll->GetType() == ITEM_USE && pkItemScroll->GetSubType() == USE_TUNING))
		return false;

	if (pkItemScroll->GetVnum() == item->GetVnum())
		return false;

	// Determine refine type based on scroll and check level restrictions
	int refineType = REFINE_TYPE_SCROLL; // default
	
	if (pkItemScroll->GetValue(0) == HYUNIRON_CHN) // 1
		refineType = REFINE_TYPE_HYUNIRON;
	else if (pkItemScroll->GetValue(0) == MUSIN_SCROLL) // 3
		refineType = REFINE_TYPE_MUSIN;
	else if (pkItemScroll->GetValue(0) == BDRAGON_SCROLL) // 6
		refineType = REFINE_TYPE_BDRAGON;
	else if (pkItemScroll->GetValue(0) == METAL_10) // 9
		refineType = REFINE_TYPE_METAL_10;
	else if (pkItemScroll->GetValue(0) == METAL_20) // 8
		refineType = REFINE_TYPE_METAL_20;
	else if (pkItemScroll->GetValue(0) == METAL_25) // 10
		refineType = REFINE_TYPE_METAL_25;
	else if (pkItemScroll->GetValue(0) == YONGSIN_SCROLL) // 2
		refineType = REFINE_TYPE_SCROLL;
	else if (pkItemScroll->GetValue(0) == YAGONG_SCROLL) // 4
		refineType = REFINE_TYPE_SCROLL;

	// Check level restrictions
	if (!CanRefineItemByLevel(item, refineType))
	{
		return false;
	}

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	LPITEM absorbedItem = NULL;
	bool isSashAbsorbedUpgrade = false;
	DWORD result_vnum = 0, result_fail_vnum = 0;
	if ((item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_ACCE) && item->GetSocket(ACCE_ABSORBED_SOCKET) != 0)
	{
		absorbedItem = ITEM_MANAGER::instance().CreateItem(item->GetSocket(ACCE_ABSORBED_SOCKET), 1);
		result_vnum = absorbedItem->GetRefinedVnum();
		result_fail_vnum = absorbedItem->GetRefineFromVnum();

		isSashAbsorbedUpgrade = true;

		if (test_server)
			ChatPacket(CHAT_TYPE_INFO, "<test_server>SASH DoRefineWithScroll: vnum: %d / result: %d / result_fail: %d / refine_set: %d", absorbedItem->GetVnum(), result_vnum, result_fail_vnum, absorbedItem->GetRefineSet());
	}
	else
	{
		result_vnum = item->GetRefinedVnum();
		result_fail_vnum = item->GetRefineFromVnum();
	}

	TItemTable* pProto;
	const TRefineTable* prt;
	if (absorbedItem && isSashAbsorbedUpgrade)
	{
		pProto = ITEM_MANAGER::instance().GetTable(absorbedItem->GetRefinedVnum());
		prt = CRefineManager::instance().GetRefineRecipe(absorbedItem->GetRefineSet());
	}
	else
	{
		pProto = ITEM_MANAGER::instance().GetTable(item->GetRefinedVnum());
		prt = CRefineManager::instance().GetRefineRecipe(item->GetRefineSet());
	}
#else
	TItemTable* pProto = ITEM_MANAGER::instance().GetTable(item->GetRefinedVnum());
	const TRefineTable* prt = CRefineManager::instance().GetRefineRecipe(item->GetRefineSet());

	DWORD result_vnum = item->GetRefinedVnum();
	DWORD result_fail_vnum = item->GetRefineFromVnum();
#endif
	if (!pProto)
	{
		sys_err("DoRefineWithScroll NOT GET ITEM PROTO %d", item->GetRefinedVnum());
		ChatPacket(CHAT_TYPE_INFO, "[LS;1002]");
		return false;
	}

	if (!prt)
		return false;

	if (item->GetType() == ITEM_GUILD_GUARD)
	{
		if (!GetGuild())
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;10118]");
			return false;
		}
		if (!GetGuild()->HasGradeAuth(GetGuild()->GetMember(GetPlayerID())->grade, GUILD_AUTH_MANAGE_GUARD))
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;10119]");
			return false;
		}
		//if (!CGuildManager::instance().CanFullManageGuildOnThisMap(this))
		//{
		//	ChatPacket(CHAT_TYPE_INFO, "[LS;10120]");
		//	return false;
		//}
	}

	if (result_vnum == 0)
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;991]");
		return false;
	}
	// MUSIN_SCROLL
	if (pkItemScroll->GetValue(0) == MUSIN_SCROLL)
	{
		if (item->GetRefineLevel() >= 4)
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;1056]");
			return false;
		}
	}
	// END_OF_MUSIC_SCROLL

	else if (pkItemScroll->GetValue(0) == MEMO_SCROLL)
	{
		if (item->GetRefineLevel() != pkItemScroll->GetValue(1))
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;162]");
			return false;
		}
	}
	else if (pkItemScroll->GetValue(0) == BDRAGON_SCROLL)
	{
		if (item->GetType() != ITEM_METIN || item->GetRefineLevel() != 4)
		{
			return false;
		}
	}

	bool isRefineGuildGuardItem = false;
	if (item->GetType() == ITEM_GUILD_GUARD)
	{
		if (GetGuild()->GetGuildMoney() < prt->cost)
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;10121]");
			return false;
		}
		isRefineGuildGuardItem = true;
	}
	else
	{
		if (GetGold() < prt->cost)
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;67]");
			return false;
		}
	
		if (GetCheque() < prt->cost2)
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;2203]");
			return false;
		}
	
		if (GetPktOsiag() < prt->cost3)
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;2204]");
			return false;
		}
	}

	for (int i = 0; i < prt->material_count; ++i)
	{
		if (CountSpecifyItem(prt->materials[i].vnum) < prt->materials[i].count)
		{
			if (test_server)
			{
				ChatPacket(CHAT_TYPE_INFO, "Find %d, count %d, require %d", prt->materials[i].vnum, CountSpecifyItem(prt->materials[i].vnum), prt->materials[i].count);
			}
			ChatPacket(CHAT_TYPE_INFO, "[LS;1035]");
			return false;
		}
	}

	for (int i = 0; i < prt->material_count; ++i)
		RemoveSpecifyItem(prt->materials[i].vnum, prt->materials[i].count);

	int prob = number(1, 100);
	int success_prob = prt->prob;
	bool bDestroyWhenFail = false;
	
	// Check if item level is 201+ to disable title bonuses
	bool isHighLevelItem = false;
	TItemTable* pItemProto = ITEM_MANAGER::instance().GetTable(item->GetVnum());
	if (pItemProto)
	{
		for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
		{
			if (pItemProto->aLimits[i].bType == LIMIT_LEVEL && pItemProto->aLimits[i].lValue >= 201)
			{
				isHighLevelItem = true;
				break;
			}
		}
	}
	
//#ifdef TITLE_SYSTEM_BYLUZER
//	// Only apply title bonuses for items below level 201
//	//if (!isHighLevelItem)
//	//{
//	if (m_pTitle[17].active == 2)
//		success_prob += 10;
//	else if (m_pTitle[16].active == 2)
//		success_prob += 5;
//	else if (m_pTitle[15].active == 2)
//		success_prob += 3;
//	//}
//#endif
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
	if (GetTitleAchievement() == 11)
	{
		success_prob += 5;
	}
	else if (GetTitleAchievementPremium() == 11)
	{
		success_prob += 5;
	}
	else if (GetTitleAchievement() == 21)
	{
		success_prob += 7;
	}
	else if (GetTitleAchievementPremium() == 21)
	{
		success_prob += 7;
	}
#endif
	const char* szRefineType = "SCROLL";

	if (pkItemScroll->GetValue(0) == HYUNIRON_CHN ||
		pkItemScroll->GetValue(0) == YONGSIN_SCROLL ||
		pkItemScroll->GetValue(0) == METAL_10 ||
		pkItemScroll->GetValue(0) == METAL_20 ||
		pkItemScroll->GetValue(0) == METAL_25 ||
		pkItemScroll->GetValue(0) == YAGONG_SCROLL)
	{
		const char hyuniron_prob[9] = { 100, 75, 65, 55, 45, 40, 35, 25, 20 };
		const char yagong_prob[9] = { 100, 100, 90, 80, 70, 60, 50, 30, 20 };

		if (pkItemScroll->GetValue(0) == YONGSIN_SCROLL)
		{
			success_prob = hyuniron_prob[MINMAX(0, item->GetRefineLevel(), 8)];
		}
		else if (pkItemScroll->GetValue(0) == YAGONG_SCROLL)
		{
			success_prob = yagong_prob[MINMAX(0, item->GetRefineLevel(), 8)];
		}
		else if (pkItemScroll->GetValue(0) == HYUNIRON_CHN) {} // @fixme121
		else
		{
			sys_err("REFINE : Unknown refine scroll item. Value0: %d", pkItemScroll->GetValue(0));
		}

		if (test_server)
		{
			ChatPacket(CHAT_TYPE_INFO, "[Only Test] Success_Prob %d, RefineLevel %d ", success_prob, item->GetRefineLevel());
		}
		if (pkItemScroll->GetValue(0) == HYUNIRON_CHN)
			bDestroyWhenFail = true;
	
		if (pkItemScroll->GetValue(0) == METAL_10)
			bDestroyWhenFail = true;
		
		if (pkItemScroll->GetValue(0) == METAL_20)
			bDestroyWhenFail = true;
		
		if (pkItemScroll->GetValue(0) == METAL_25)
			bDestroyWhenFail = true;

		// DETAIL_REFINE_LOG
		if (pkItemScroll->GetValue(0) == HYUNIRON_CHN)
		{
			szRefineType = "HYUNIRON";
		}
		else if (pkItemScroll->GetValue(0) == YONGSIN_SCROLL)
		{
			szRefineType = "GOD_SCROLL";
		}
		else if (pkItemScroll->GetValue(0) == YAGONG_SCROLL)
		{
			szRefineType = "YAGONG_SCROLL";
		}
		else if (pkItemScroll->GetValue(0) == METAL_10) // 9
		{
			szRefineType = "METAL_10";
		}
		else if (pkItemScroll->GetValue(0) == METAL_20) // 8
		{
			szRefineType = "METAL_20";
		}
		else if (pkItemScroll->GetValue(0) == METAL_25) // 10
		{
			szRefineType = "METAL_25";
		}
		// END_OF_DETAIL_REFINE_LOG
	}

	// DETAIL_REFINE_LOG
	if (pkItemScroll->GetValue(0) == MUSIN_SCROLL)
	{
		success_prob = 100;

		szRefineType = "MUSIN_SCROLL";
	}
	// END_OF_DETAIL_REFINE_LOG
	else if (pkItemScroll->GetValue(0) == MEMO_SCROLL)
	{
		success_prob = 100;
		szRefineType = "MEMO_SCROLL";
	}
	else if (pkItemScroll->GetValue(0) == BDRAGON_SCROLL)
	{
		success_prob = 80;
		szRefineType = "BDRAGON_SCROLL";
	}
	else if (pkItemScroll->GetValue(0) == METAL_10)
	{
		success_prob = MIN(100, success_prob+10);
	}
	else if (pkItemScroll->GetValue(0) == METAL_20)
	{
		success_prob = MIN(100, success_prob+20);
	}
	else if (pkItemScroll->GetValue(0) == METAL_25)
	{
		success_prob = MIN(100, success_prob+25);
	}

	if (szRefineType != "HYUNIRON") {
		pkItemScroll->SetCount(pkItemScroll->GetCount() - 1);
	}

	if (prob <= success_prob)
	{
#ifdef _ENABLE_BATTLEPASS_
		BattlePassManager::Instance().Notify(MISSION_TYPE_REFINE, this, item->GetVnum(), 1);
#endif
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(result_vnum, 1, 0, false);
		if (pkNewItem)
		{
			BYTE bCell = item->GetCell();
			NotifyRefineSuccess(this, item, szRefineType);
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -prt->cost);

			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			if (isSashAbsorbedUpgrade)
			{
				LogManager::instance().ItemLog(this, item, "SASH ITEM REFINE SUCCESS", pkNewItem->GetName());

				item->SetSocket(ACCE_ABSORBED_SOCKET, pkNewItem->GetVnum());
				ITEM_MANAGER::instance().FlushDelayedSave(item);
			}
			else
			{
				LogManager::instance().ItemLog(this, pkNewItem, "REFINE SUCCESS", pkNewItem->GetName());
				ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");
				pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell));
				
#ifdef ENABLE_GLOVE_SYSTEM
				if (item != NULL) {
					if (item->GetType() == ITEM_ARMOR && item->GetSubType() == ARMOR_GLOVE) {
						for (int i = 0; i < 3; i++) {
							int adjustedValue = CGloveBonusManager::GetRefineNewGloveBonus(item->GetAttributeType(i), item->GetAttributeValue(i));
							pkNewItem->SetForceAttribute(i, item->GetAttributeType(i), adjustedValue);
						}
					}
				} else {
					return false;
				}
#endif
				ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);
				pkNewItem->AttrLog();
			}
#else
			LogManager::instance().ItemLog(this, pkNewItem, "REFINE SUCCESS", pkNewItem->GetName());
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");
			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell));
			
#ifdef ENABLE_GLOVE_SYSTEM
			if (item != NULL) {
				if (item->GetType() == ITEM_ARMOR && item->GetSubType() == ARMOR_GLOVE) {
					for (int i = 0; i < 3; i++) {
						int adjustedValue = CGloveBonusManager::GetRefineNewGloveBonus(item->GetAttributeType(i), item->GetAttributeValue(i));
						pkNewItem->SetForceAttribute(i, item->GetAttributeType(i), adjustedValue);
					}
				}
			} else {
				return false;
			}
#endif
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);
			pkNewItem->AttrLog();
#endif


// 			int refine_vnum = item->GetVnum() - 23000;
// 			for (int i = 0; i < 3; ++i) {
// 				AddAffect(AFFECT_GLOVE_BONUS, aApplyInfo[item->GetAttributeType(i)].bPointType, (GLOVE_BONUS_TABLE[item->GetAttributeType(i)]/9)*refine_vnum, AFF_NONE, 60*60*60*365, 0, false);

			//PointChange(POINT_GOLD, -prt->cost);
			PointChange(POINT_CHEQUE, -prt->cost2);
			PointChange(POINT_PKT_OSIAG, -prt->cost3);
			
			if (isRefineGuildGuardItem)
				GetGuild()->ChangeMoney(-(prt->cost));
			else
				PayRefineFee(prt->cost);
			
#ifdef WEEKLY_RANK_BYLUZER
			PointChange(POINT_WEEKLY6, 1);
			CheckWeekly();
#endif
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
			const int itemRefined = GetQuestFlag("AchievementTitle.refine_succes") + 1;
			SetQuestFlag("AchievementTitle.refine_succes", itemRefined);
					
			CTitleAchievementTitle::instance().CheckAndUnlockTitles(this);
					
			// Send progress update to client
			ChatPacket(CHAT_TYPE_COMMAND, "title_progress_refine_success %d", itemRefined);
#endif
		}
		else
		{
			sys_err("cannot create item %u", result_vnum);
			NotifyRefineFail(this, item, szRefineType);
		}
	}
	else if (!bDestroyWhenFail && result_fail_vnum)
	{
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(result_fail_vnum, 1, 0, false);

		if (pkNewItem)
		{
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			if (!isSashAbsorbedUpgrade)
			{
				ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
				LogManager::instance().ItemLog(this, pkNewItem, "REFINE FAIL", pkNewItem->GetName());

				BYTE bCell = item->GetCell();

				DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -prt->cost);
				NotifyRefineFail(this, item, szRefineType, -1);
				ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE FAIL)");

				pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell));
				ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);

				pkNewItem->AttrLog();
			}
			else
			{
				LogManager::instance().ItemLog(this, pkNewItem, "SASH ITEM REFINE FAIL", pkNewItem->GetName());
				DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -prt->cost);

				NotifyRefineFail(this, pkNewItem, szRefineType, -1);

				item->SetSocket(ACCE_ABSORBED_SOCKET, pkNewItem->GetVnum());
				ITEM_MANAGER::instance().FlushDelayedSave(item);

				M2_DESTROY_ITEM(pkNewItem);
				M2_DESTROY_ITEM(absorbedItem);
			}
#else
			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
			LogManager::instance().ItemLog(this, pkNewItem, "REFINE FAIL", pkNewItem->GetName());

			BYTE bCell = item->GetCell();

			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -prt->cost);
			NotifyRefineFail(this, item, szRefineType, -1);
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE FAIL)");
			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell));
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);

			pkNewItem->AttrLog();
#endif

			if (isRefineGuildGuardItem)
				GetGuild()->ChangeMoney(-(prt->cost));
			else
				PayRefineFee(prt->cost);
			
			PointChange(POINT_CHEQUE, -prt->cost2);
			PointChange(POINT_PKT_OSIAG, -prt->cost3);

#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
			const int itemRefineFailed = GetQuestFlag("AchievementTitle.refine_fail") + 1;
			SetQuestFlag("AchievementTitle.refine_fail", itemRefineFailed);
			
			CTitleAchievementTitle::instance().CheckAndUnlockTitles(this);
			
			// Send progress update to client
			ChatPacket(CHAT_TYPE_COMMAND, "title_progress_refine_fail %d", itemRefineFailed);
#endif
		}
		else
		{
			sys_err("cannot create item %u", result_fail_vnum);
			NotifyRefineFail(this, item, szRefineType);
		}
	}
	else
	{
		NotifyRefineFail(this, item, szRefineType);

		if (isRefineGuildGuardItem)
			GetGuild()->ChangeMoney(-(prt->cost));
		else
			PayRefineFee(prt->cost);
		
		PointChange(POINT_CHEQUE, -prt->cost2);
		PointChange(POINT_PKT_OSIAG, -prt->cost3);
		
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
		const int itemRefineFailed = GetQuestFlag("AchievementTitle.refine_fail") + 1;
		SetQuestFlag("AchievementTitle.refine_fail", itemRefineFailed);
		
		CTitleAchievementTitle::instance().CheckAndUnlockTitles(this);
		
		// Send progress update to client
		ChatPacket(CHAT_TYPE_COMMAND, "title_progress_refine_fail %d", itemRefineFailed);
#endif
	}

	return true;
}

bool CHARACTER::RefineInformation(BYTE bCell, BYTE bType, int iAdditionalCell)
{
    if (bCell > INVENTORY_MAX_NUM)
        return false;

    LPITEM item = GetInventoryItem(bCell);

    if (!item)
        return false;

    // Add level restriction check for refine information
    if (!CanRefineItemByLevel(item, bType))
    {
        return false;
    }

	// REFINE_COST
	if (bType == REFINE_TYPE_MONEY_ONLY && !GetQuestFlag("deviltower_zone.can_refine"))
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;1067]");
		return false;
	}
	// END_OF_REFINE_COST

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	LPITEM absorbedItem = NULL;
	bool isSashAbsorbedUpgrade = false;
	if ((item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_ACCE) && item->GetSocket(ACCE_ABSORBED_SOCKET) != 0)
	{
		absorbedItem = ITEM_MANAGER::instance().CreateItem(item->GetSocket(ACCE_ABSORBED_SOCKET), 1);

		isSashAbsorbedUpgrade = true;

		if (test_server)
			ChatPacket(CHAT_TYPE_INFO, "<test_server> SASH RefineInformation: vnum: %d / result_vnum: %d / refine_set: %d", absorbedItem->GetVnum(), absorbedItem->GetRefinedVnum(), absorbedItem->GetRefineSet());
	}
#endif

	if (item->GetType() == ITEM_GUILD_GUARD)
	{
		if (!GetGuild())
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;10118]");
			return false;
		}
		if (!GetGuild()->HasGradeAuth(GetGuild()->GetMember(GetPlayerID())->grade, GUILD_AUTH_MANAGE_GUARD))
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;10119]");
			return false;
		}
		//if (!CGuildManager::instance().CanFullManageGuildOnThisMap(this))
		//{
		//	ChatPacket(CHAT_TYPE_INFO, "[LS;10120]");
		//	return false;
		//}
	}

	TPacketGCRefineInformation p;

	p.header = HEADER_GC_REFINE_INFORMATION;
	p.pos = bCell;
	p.src_vnum = item->GetVnum();
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	if (absorbedItem && isSashAbsorbedUpgrade)
	{
		p.src_vnum = absorbedItem->GetVnum();
		p.result_vnum = absorbedItem->GetRefinedVnum();
	}
	else
	{
		p.src_vnum = item->GetVnum();
		p.result_vnum = item->GetRefinedVnum();
	}
#else
	p.result_vnum = item->GetRefinedVnum();
#endif
	p.type = bType;
#if defined(__ITEM_APPLY_RANDOM__)
	thecore_memcpy(&p.aApplyRandom, item->GetNextRandomApplies(), sizeof(p.aApplyRandom));
#endif

	if (p.result_vnum == 0)
	{
		sys_err("RefineInformation p.result_vnum == 0");
		ChatPacket(CHAT_TYPE_INFO, "[LS;1002]");
		return false;
	}

	if (item->GetType() == ITEM_USE && item->GetSubType() == USE_TUNING)
	{
		if (bType == 0)
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;1077]");
			return false;
		}
		else
		{
			LPITEM itemScroll = GetInventoryItem(iAdditionalCell);
			if (!itemScroll || item->GetVnum() == itemScroll->GetVnum())
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;1106]");
				ChatPacket(CHAT_TYPE_INFO, "[LS;1096]");
				return false;
			}
		}
	}

	CRefineManager& rm = CRefineManager::instance();

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	const TRefineTable* prt;
	if (absorbedItem && isSashAbsorbedUpgrade)
		prt = rm.GetRefineRecipe(absorbedItem->GetRefineSet());
	else
		prt = rm.GetRefineRecipe(item->GetRefineSet());
#else
	const TRefineTable* prt = rm.GetRefineRecipe(item->GetRefineSet());
#endif

	if (!prt)
	{
		sys_err("RefineInformation NOT GET REFINE SET %d", item->GetRefineSet());
		ChatPacket(CHAT_TYPE_INFO, "[LS;1002]");
		return false;
	}

	// REFINE_COST

	//MAIN_QUEST_LV7
	if (GetQuestFlag("main_quest_lv7.refine_chance") > 0)
	{
		if (!item->CheckItemUseLevel(20) || item->GetType() != ITEM_WEAPON)
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;1003]");
			return false;
		}
		p.cost = 0;
		p.cost2 = 0;
		p.cost3 = 0;
	}
	else {
		p.cost = ComputeRefineFee(prt->cost);
		p.cost2 = ComputeRefineFee(prt->cost2);
		p.cost3 = ComputeRefineFee(prt->cost3);
	}

	//END_MAIN_QUEST_LV7
	p.prob = prt->prob;
	p.probExtra = 0;
	
//#ifdef TITLE_SYSTEM_BYLUZER
//	if (m_pTitle[17].active == 2)
//		p.probExtra += 10;
//	else if (m_pTitle[16].active == 2)
//		p.probExtra += 5;
//	else if (m_pTitle[15].active == 2)
//		p.probExtra += 3;
//#endif
	
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
	if (GetTitleAchievement() == 11)
	{
		p.probExtra += 5;
	}
	else if (GetTitleAchievementPremium() == 11)
	{
		p.probExtra += 5;
	}
	else if (GetTitleAchievement() == 21)
	{
		p.probExtra += 7;
	}
	else if (GetTitleAchievementPremium() == 21)
	{
		p.probExtra += 7;
	}
#endif

	if (bType == REFINE_TYPE_METAL_10)
	{
		p.probExtra += 10;  // Add refine bonus to probExtra
		//p.prob = MIN(100, prt->prob+10);
	}
	
	if (bType == REFINE_TYPE_METAL_20)
	{
		p.probExtra += 20;  // Add refine bonus to probExtra
	}
	
	if (bType == REFINE_TYPE_METAL_25)
	{
		p.probExtra += 25;  // Add refine bonus to probExtra
	}
	
	// Ensure the total chance does not exceed 100%
	p.probExtra = MIN(100 - p.prob, p.probExtra);
	
	if (bType == REFINE_TYPE_MONEY_ONLY)
	{
		p.material_count = 0;
		memset(p.materials, 0, sizeof(p.materials));
	}
	else
	{
		p.material_count = prt->material_count;
		thecore_memcpy(&p.materials, prt->materials, sizeof(prt->materials));
	}
	// END_OF_REFINE_COST

	GetDesc()->Packet(&p, sizeof(TPacketGCRefineInformation));

	SetRefineMode(iAdditionalCell);
	return true;
}

bool CHARACTER::RefineItem(LPITEM pkItem, LPITEM pkTarget)
{
    if (!CanHandleItem())
        return false;

    if (pkItem->GetSubType() == USE_TUNING)
    {
        // Determine refine type and check restrictions before calling RefineInformation
        int refineType = REFINE_TYPE_SCROLL; // default
        
        if (pkItem->GetValue(0) == MUSIN_SCROLL) // 3
            refineType = REFINE_TYPE_MUSIN;
        else if (pkItem->GetValue(0) == HYUNIRON_CHN) // 1
            refineType = REFINE_TYPE_HYUNIRON;
        else if (pkItem->GetValue(0) == METAL_10) // 9
            refineType = REFINE_TYPE_METAL_10;
        else if (pkItem->GetValue(0) == METAL_20) // 8
            refineType = REFINE_TYPE_METAL_20;
        else if (pkItem->GetValue(0) == METAL_25) // 10
            refineType = REFINE_TYPE_METAL_25;
        else if (pkItem->GetValue(0) == BDRAGON_SCROLL) // 6
            refineType = REFINE_TYPE_BDRAGON;

        // Check level restrictions before proceeding
        if (!CanRefineItemByLevel(pkTarget, refineType))
        {
            return false;
        }

        // MUSIN_SCROLL
        if (pkItem->GetValue(0) == MUSIN_SCROLL) // 3
            RefineInformation(pkTarget->GetCell(), REFINE_TYPE_MUSIN, pkItem->GetCell());
        // END_OF_MUSIN_SCROLL
        else if (pkItem->GetValue(0) == HYUNIRON_CHN) // 1
        {
            RefineInformation(pkTarget->GetCell(), REFINE_TYPE_HYUNIRON, pkItem->GetCell());
        }
        else if (pkItem->GetValue(0) == METAL_10) // 9
        {
            RefineInformation(pkTarget->GetCell(), REFINE_TYPE_METAL_10, pkItem->GetCell());
        }
        else if (pkItem->GetValue(0) == METAL_20) // 8
        {
            RefineInformation(pkTarget->GetCell(), REFINE_TYPE_METAL_20, pkItem->GetCell());
        }
        else if (pkItem->GetValue(0) == METAL_25) // 10
        {
            RefineInformation(pkTarget->GetCell(), REFINE_TYPE_METAL_25, pkItem->GetCell());
        }
        else if (pkItem->GetValue(0) == BDRAGON_SCROLL) // 6
        {
            RefineInformation(pkTarget->GetCell(), REFINE_TYPE_BDRAGON, pkItem->GetCell());
        }
        else
        {
            RefineInformation(pkTarget->GetCell(), REFINE_TYPE_SCROLL, pkItem->GetCell());
        }
    }
    else if (pkItem->GetSubType() == USE_DETACHMENT && IS_SET(pkTarget->GetFlag(), ITEM_FLAG_REFINEABLE))
    {
        // ... your existing detachment code remains the same ...
        LogManager::instance().ItemLog(this, pkTarget, "USE_DETACHMENT", pkTarget->GetName());

        bool bHasMetinStone = false;

#ifdef ENABLE_EXTENDED_SOCKETS
        for (int i = 0; i < ITEM_STONES_MAX_NUM; i++)
#else
        for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
#endif
        {
            int32_t socket = pkTarget->GetSocket(i);
            if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
            {
                bHasMetinStone = true;
                break;
            }
        }

        if (bHasMetinStone)
        {
#ifdef ENABLE_EXTENDED_SOCKETS
            for (int i = 0; i < ITEM_STONES_MAX_NUM; i++)
#else
            for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
#endif
            {
                int32_t socket = pkTarget->GetSocket(i);
                if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
                {
                    AutoGiveItem(socket);
                    pkTarget->SetSocket(i, ITEM_BROKEN_METIN_VNUM);
                }
            }
            pkItem->SetCount(pkItem->GetCount() - 1);
            return true;
        }
        else
        {
            ChatPacket(CHAT_TYPE_INFO, "[LS;1108]");
            return false;
        }
    }

    return false;
}

EVENTFUNC(kill_campfire_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>(event->info);

	if (info == NULL)
	{
		sys_err("kill_campfire_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (ch == NULL) { // <Factor>
		return 0;
	}
	ch->m_pkMiningEvent = NULL;
	M2_DESTROY_CHARACTER(ch);
	return 0;
}

bool CHARACTER::GiveRecallItem(LPITEM item)
{
	int idx = GetMapIndex();
	int iEmpireByMapIndex = -1;

	if (idx < 20)
		iEmpireByMapIndex = 1;
	else if (idx < 40)
		iEmpireByMapIndex = 2;
	else if (idx < 60)
		iEmpireByMapIndex = 3;
	else if (idx < 10000)
		iEmpireByMapIndex = 0;

	switch (idx)
	{
	case 66:
	case 216:
		iEmpireByMapIndex = -1;
		break;
	}

	if (iEmpireByMapIndex && GetEmpire() != iEmpireByMapIndex)
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;1119]");
		return false;
	}

	int pos;

	if (item->GetCount() == 1)
	{
		item->SetSocket(0, GetX());
		item->SetSocket(1, GetY());
	}
	else if ((pos = GetEmptyInventory(item->GetSize())) != -1)
	{
		LPITEM item2 = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), 1);

		if (NULL != item2)
		{
			item2->SetSocket(0, GetX());
			item2->SetSocket(1, GetY());
			item2->AddToCharacter(this, TItemPos(INVENTORY, pos));

			item->SetCount(item->GetCount() - 1);
		}
	}
	else
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;1130]");
		return false;
	}

	return true;
}

void CHARACTER::ProcessRecallItem(LPITEM item)
{
	int idx;

	if ((idx = SECTREE_MANAGER::instance().GetMapIndex(item->GetSocket(0), item->GetSocket(1))) == 0)
		return;

	int iEmpireByMapIndex = -1;

	if (idx < 20)
		iEmpireByMapIndex = 1;
	else if (idx < 40)
		iEmpireByMapIndex = 2;
	else if (idx < 60)
		iEmpireByMapIndex = 3;
	else if (idx < 10000)
		iEmpireByMapIndex = 0;

	switch (idx)
	{
	case 66:
	case 216:
		iEmpireByMapIndex = -1;
		break;

	case 301:
	case 302:
	case 303:
	case 304:
		if (GetLevel() < 90)
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;1013]");
			return;
		}
		else
			break;
	}

	if (iEmpireByMapIndex && GetEmpire() != iEmpireByMapIndex)
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;1141]");
		item->SetSocket(0, 0);
		item->SetSocket(1, 0);
	}
	else
	{
		sys_log(1, "Recall: %s %d %d -> %d %d", GetName(), GetX(), GetY(), item->GetSocket(0), item->GetSocket(1));
		WarpSet(item->GetSocket(0), item->GetSocket(1));
		item->SetCount(item->GetCount() - 1);
	}
}

void CHARACTER::__OpenPrivateShop()
{
#ifdef ENABLE_OPEN_SHOP_WITH_ARMOR
	ChatPacket(CHAT_TYPE_COMMAND, "OpenPrivateShop");
#else
	unsigned bodyPart = GetPart(PART_MAIN);
	switch (bodyPart)
	{
	case 0:
	case 1:
	case 2:
		ChatPacket(CHAT_TYPE_COMMAND, "OpenPrivateShop");
		break;
	default:
		ChatPacket(CHAT_TYPE_INFO, "[LS;1025]");
		break;
	}
#endif
}

// MYSHOP_PRICE_LIST
#ifdef ENABLE_LONG_LONG
void CHARACTER::SendMyShopPriceListCmd(DWORD dwItemVnum, int64_t dwItemPrice)
{
	char szLine[256];
	snprintf(szLine, sizeof(szLine), "MyShopPriceList %u %lld", dwItemVnum, dwItemPrice);
	ChatPacket(CHAT_TYPE_COMMAND, szLine);
	sys_log(0, szLine);
}
#else
void CHARACTER::SendMyShopPriceListCmd(DWORD dwItemVnum, DWORD dwItemPrice)
{
	char szLine[256];
	snprintf(szLine, sizeof(szLine), "MyShopPriceList %u %u", dwItemVnum, dwItemPrice);
	ChatPacket(CHAT_TYPE_COMMAND, szLine);
	sys_log(0, szLine);
}
#endif

//

//
void CHARACTER::UseSilkBotaryReal(const TPacketMyshopPricelistHeader* p)
{
	const TItemPriceInfo* pInfo = (const TItemPriceInfo*)(p + 1);

	if (!p->byCount)

		SendMyShopPriceListCmd(1, 0);
	else {
		for (int idx = 0; idx < p->byCount; idx++)
			SendMyShopPriceListCmd(pInfo[idx].dwVnum, pInfo[idx].dwPrice);
	}

	__OpenPrivateShop();
}

//

//
void CHARACTER::UseSilkBotary(void)
{
	if (m_bNoOpenedShop) {
		DWORD dwPlayerID = GetPlayerID();
		db_clientdesc->DBPacket(HEADER_GD_MYSHOP_PRICELIST_REQ, GetDesc()->GetHandle(), &dwPlayerID, sizeof(DWORD));
		m_bNoOpenedShop = false;
	}
	else {
		__OpenPrivateShop();
	}
}
// END_OF_MYSHOP_PRICE_LIST

int CalculateConsume(LPCHARACTER ch)
{
	static const int WARP_NEED_LIFE_PERCENT = 30;
	static const int WARP_MIN_LIFE_PERCENT = 10;
	// CONSUME_LIFE_WHEN_USE_WARP_ITEM
	int consumeLife = 0;
	{
		// CheckNeedLifeForWarp
		const int curLife = ch->GetHP();
		const int needPercent = WARP_NEED_LIFE_PERCENT;
		const int needLife = ch->GetMaxHP() * needPercent / 100;
		if (curLife < needLife)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1152]");
			return -1;
		}

		consumeLife = needLife;

		const int minPercent = WARP_MIN_LIFE_PERCENT;
		const int minLife = ch->GetMaxHP() * minPercent / 100;
		if (curLife - needLife < minLife)
			consumeLife = curLife - minLife;

		if (consumeLife < 0)
			consumeLife = 0;
	}
	// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM
	return consumeLife;
}

int CalculateConsumeSP(LPCHARACTER lpChar)
{
	static const int NEED_WARP_SP_PERCENT = 30;

	const int curSP = lpChar->GetSP();
	const int needSP = lpChar->GetMaxSP() * NEED_WARP_SP_PERCENT / 100;

	if (curSP < needSP)
	{
		lpChar->ChatPacket(CHAT_TYPE_INFO, "[LS;1162]");
		return -1;
	}

	return needSP;
}

struct TGachaItems {
	short itemCount;
	char szName[ITEM_NAME_MAX_LEN + 1];
};

// #define ENABLE_FIREWORK_STUN
#define ENABLE_ADDSTONE_FAILURE
bool CHARACTER::UseItemEx(LPITEM item, TItemPos DestCell)
{
	int iLimitRealtimeStartFirstUseFlagIndex = -1;
	//int iLimitTimerBasedOnWearFlagIndex = -1;

	WORD wDestCell = DestCell.cell;
	BYTE bDestInven = DestCell.window_type;
	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		int32_t limitValue = item->GetProto()->aLimits[i].lValue;

		switch (item->GetProto()->aLimits[i].bType)
		{
		case LIMIT_LEVEL:
			if (item->GetType() != ITEM_GUILD_GUARD && GetLevel() < limitValue)
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;1013]");
				return false;
			}
			break;
				
#ifdef ENABLE_SECONDARY_LEVEL
		case LIMIT_SECONDARY_LEVEL:
			if (GetSecondaryLevel() < limitValue)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("TOO_LOW_SECONDARY_LEVEL"));
				return false;
			}
			break;
#endif
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
		case LIMIT_ENLIGHT_LEVEL:
			if (GetEnlightLevel() < limitValue)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("TOO_LOW_ENLIGHTENMENT_LEVEL"));
				return false;
			}
			break;
#endif

#ifdef ENABLE_MOUNT_SYSTEM
		case LIMIT_MOUNT_LEVEL:
			if (GetMountLevel() < limitValue)
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;10013]");
				return false;
			}
			break;
#endif

		case LIMIT_REAL_TIME_START_FIRST_USE:
			iLimitRealtimeStartFirstUseFlagIndex = i;
			break;

		case LIMIT_TIMER_BASED_ON_WEAR:
			//iLimitTimerBasedOnWearFlagIndex = i;
			break;
		}
	}

	if (test_server)
	{
		sys_log(0, "USE_ITEM %s, Inven %d, Cell %d, ItemType %d, SubType %d", item->GetName(), bDestInven, wDestCell, item->GetType(), item->GetSubType());
	}

	if (CArenaManager::instance().IsLimitedItem(GetMapIndex(), item->GetVnum()) == true)
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;1205]");
		return false;
	}
#ifdef ENABLE_NEWSTUFF
	else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && IsLimitedPotionOnPVP(item->GetVnum()))
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;1205]");
		return false;
	}
#endif

	// @fixme402 (IsLoadedAffect to block affect hacking)
	if (!IsLoadedAffect())
	{
		ChatPacket(CHAT_TYPE_INFO, "Affects are not loaded yet!");
		return false;
	}

	// @fixme141 BEGIN
	if (TItemPos(item->GetWindow(), item->GetCell()).IsBeltInventoryPosition())
	{
		LPITEM beltItem = GetWear(WEAR_BELT);

		if (NULL == beltItem)
		{
			ChatPacket(CHAT_TYPE_INFO, "<Belt> You can't use this item if you have no equipped belt.");
			return false;
		}

		if (false == CBeltInventoryHelper::IsAvailableCell(item->GetCell() - BELT_INVENTORY_SLOT_START, beltItem->GetValue(0)))
		{
			ChatPacket(CHAT_TYPE_INFO, "<Belt> You can't use this item if you don't upgrade your belt.");
			return false;
		}
	}
	// @fixme141 END
	
#ifdef _ENABLE_BATTLEPASS_
	BattlePassManager::Instance().Notify(MISSION_TYPE_USE_ITEM, this, item->GetVnum(), 1);

	if (item->GetVnum() == BattlePassManager::Instance().GetConf().premiumItem)
	{
		UpgradeToPremium();
		return true;
	}
#endif

	if (-1 != iLimitRealtimeStartFirstUseFlagIndex)
	{
		if (0 == item->GetSocket(1))
		{
			int32_t duration = (0 != item->GetSocket(0)) ? item->GetSocket(0) : item->GetProto()->aLimits[iLimitRealtimeStartFirstUseFlagIndex].lValue;

			if (item->GetSocket(0) > 0)
				duration += item->GetSocket(0);

			if (0 == duration)
				duration = 60 * 60 * 24 * 7;

			item->SetSocket(0, time(0) + duration);
			item->StartRealTimeExpireEvent();
		}

		if (false == item->IsEquipped())
			item->SetSocket(1, item->GetSocket(1) + 1);
	}

	switch (item->GetType())
	{
	case ITEM_HAIR:
		return ItemProcess_Hair(item, wDestCell);
#ifdef ENABLE_NEW_PET_SYSTEM
	case ITEM_NEW_PET:
	case ITEM_NEW_PET_EQ:
#ifdef ENABLE_EXTRABONUS_SYSTEM
		if (!item->IsEquipped()) {
			EquipItem(item);
			CheckPetExtraBonus(item, 0);
		}
		else {
			UnequipItem(item);
			CheckPetExtraBonus(item, 1);
		}
		break;
#else
		if (!item->IsEquipped()) {
			EquipItem(item);
		}
		else {
			UnequipItem(item);
		}
		break;
#endif
#endif
	case ITEM_POLYMORPH:
		return ItemProcess_Polymorph(item);

#ifdef __VIP_SYSTEM__
	case EItemTypes::ITEM_VIP:
		return ItemProcessVIP(item);
#endif

	case ITEM_QUEST:
#ifdef ENABLE_QUEST_DND_EVENT
		if (IS_SET(item->GetFlag(), ITEM_FLAG_APPLICABLE))
		{
			LPITEM item2;

			if (!GetItem(DestCell) || !(item2 = GetItem(DestCell)))
				return false;

			if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
				return false;

			quest::CQuestManager::instance().DND(GetPlayerID(), item, item2, false);
			return true;
		}
#endif


#ifdef ENABLE_AUTONAME_SYSTEM
		if (item->GetVnum() == AUTONAME_SCROLL_VNUM)
			ChatPacket(CHAT_TYPE_COMMAND, "OpenAutoNameSystem");
#endif

#ifdef __AUTO_QUQUE_ATTACK__
		if (item->GetVnum() == 61400 || item->GetVnum() == 61401 || item->GetVnum() == 61402 || item->GetVnum() == 61403)
		{
			if (item->isLocked() || item->IsExchanging())
				return false;
			else if (FindAffect(AFFECT_AUTO_METIN_FARM)) {
				ChatPacket(CHAT_TYPE_INFO, "[LS;2205]");
				return false;
			}
			ChatPacket(CHAT_TYPE_INFO, "[LS;2206]");
			AddAffect(AFFECT_AUTO_METIN_FARM, 0, 0, AFF_NONE, item->GetValue(0), 0, false);
			item->SetCount(item->GetCount() - 1);
			return true;
		}
#endif


		if (GetArena() != NULL || IsObserverMode() == true)
		{
			if (item->GetVnum() == 50051 || item->GetVnum() == 50052 || item->GetVnum() == 50053)
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;1205]");
				return false;
			}
		}

		if (!IS_SET(item->GetFlag(), ITEM_FLAG_QUEST_USE | ITEM_FLAG_QUEST_USE_MULTIPLE))
		{
			if (item->GetSIGVnum() == 0)
			{
				quest::CQuestManager::instance().UseItem(GetPlayerID(), item, false);
			}
			else
			{
				quest::CQuestManager::instance().SIGUse(GetPlayerID(), item->GetSIGVnum(), item, false);
			}
		}
		break;

	case ITEM_CAMPFIRE:
	{
		float fx, fy;
		GetDeltaByDegree(GetRotation(), 100.0f, &fx, &fy);

		LPSECTREE tree = SECTREE_MANAGER::instance().Get(GetMapIndex(), (int32_t)(GetX() + fx), (int32_t)(GetY() + fy));

		if (!tree)
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;1217]");
			return false;
		}

		if (tree->IsAttr((int32_t)(GetX() + fx), (int32_t)(GetY() + fy), ATTR_WATER))
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;1228]");
			return false;
		}

		LPCHARACTER campfire = CHARACTER_MANAGER::instance().SpawnMob(fishing::CAMPFIRE_MOB, GetMapIndex(), (int32_t)(GetX() + fx), (int32_t)(GetY() + fy), 0, false, number(0, 359));

		char_event_info* info = AllocEventInfo<char_event_info>();

		info->ch = campfire;

		campfire->m_pkMiningEvent = event_create(kill_campfire_event, info, PASSES_PER_SEC(40));

		item->SetCount(item->GetCount() - 1);
	}
	break;

	case ITEM_UNIQUE:
	{
		switch (item->GetSubType())
		{
		case USE_ABILITY_UP:
		{
			switch (item->GetValue(0))
			{
			case APPLY_MOV_SPEED:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_MOV_SPEED, item->GetValue(2), AFF_MOV_SPEED_POTION, item->GetValue(1), 0, true, true);
				break;

			case APPLY_ATT_SPEED:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_ATT_SPEED, item->GetValue(2), AFF_ATT_SPEED_POTION, item->GetValue(1), 0, true, true);
				break;

			case APPLY_STR:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_ST, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
				break;

			case APPLY_DEX:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_DX, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
				break;

			case APPLY_CON:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_HT, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
				break;

			case APPLY_INT:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_IQ, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
				break;

			case APPLY_CAST_SPEED:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_CASTING_SPEED, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
				break;

			case APPLY_RESIST_MAGIC:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_RESIST_MAGIC, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
				break;

			case APPLY_ATT_GRADE_BONUS:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_ATT_GRADE_BONUS,
					item->GetValue(2), 0, item->GetValue(1), 0, true, true);
				break;

			case APPLY_DEF_GRADE_BONUS:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_DEF_GRADE_BONUS,
					item->GetValue(2), 0, item->GetValue(1), 0, true, true);
				break;
			}
		}

		if (GetDungeon())
			GetDungeon()->UsePotion(this);

		if (GetWarMap())
			GetWarMap()->UsePotion(this, item);

		item->SetCount(item->GetCount() - 1);
		break;

		default:
		{
			if (item->GetSubType() == USE_SPECIAL)
			{
#ifdef ENABLE_MULTI_FARM_BLOCK
				if (item->GetVnum() >= 55610 && item->GetVnum() <= 55615)
				{
					if (FindAffect(AFFECT_MULTI_FARM_PREMIUM))
					{
						ChatPacket(CHAT_TYPE_INFO, "You have already this affect!");
						return false;
					}
					else
					{
						AddAffect(AFFECT_MULTI_FARM_PREMIUM, POINT_NONE, item->GetValue(1), AFF_NONE, item->GetValue(0), 0, false, false);
						item->SetCount(item->GetCount() - 1);
						CHARACTER_MANAGER::Instance().CheckMultiFarmAccount(GetCompositeHWID().c_str(), GetPlayerID(), GetName(), GetRewardStatus());
						ChatPacket(CHAT_TYPE_INFO, "Affect succesfully added on your character!");
						ChatPacket(CHAT_TYPE_INFO, "If you want use this affect this character need active drop status!");
					}
				}
#endif

				switch (item->GetVnum())
				{
				case 71049:
					if (g_bEnableBootaryCheck)
					{
						if (IS_BOTARYABLE_ZONE(GetMapIndex()) == true)
						{
							UseSilkBotary();
						}
					}
					else
					{
						UseSilkBotary();
					}
					break;
				}
			}
			else
			{
				if (!item->IsEquipped())
					EquipItem(item);
				else
					UnequipItem(item);
			}
		}
		break;
		}
	}
	break;

	case ITEM_COSTUME:
	case ITEM_WEAPON:
	case ITEM_ARMOR:
	case ITEM_ROD:
	case ITEM_RING:
	case ITEM_BELT:
#ifdef ENABLE_ARTEFAKT_SYSTEM
	case ITEM_ARTEFAKT:
#endif
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	case ITEM_MOUNT_EQUIPMENT:
#endif
#ifdef __SKILL_COSTUME__
	case ITEM_SKILL_COSTUME:
#endif
		// MINING
	case ITEM_PICK:
#ifdef ENABLE_MOUNT_SYSTEM
	case ITEM_MOUNT:
#endif
		// END_OF_MINING

		if (item->GetType() == ITEM_WEAPON 
		|| (item->GetType() == ITEM_ARMOR && item->GetSubType() == ARMOR_BODY) 
		|| (item->GetType() == ITEM_COSTUME && (item->GetSubType() == COSTUME_ACCE || item->GetSubType() == COSTUME_BODY || item->GetSubType() == COSTUME_HAIR)))
		{
			int iPulse = thecore_pulse();
			if (iPulse - GetEquipItemsTime() < PASSES_PER_SEC(3))
			{
				int time = (3 - (iPulse - GetEquipItemsTime()) / PASSES_PER_SEC(1));
				ChatPacket(CHAT_TYPE_INFO, "[LS;2207]");
				return false;
			}

			m_iEquipItemsCount++;
			if (get_global_time() < m_iEquipItemsLastTime)
			{
				if (GetEquipItemsCount() > 5)
				{
					SetEquipItemsTime();
					m_iEquipItemsCount = 0;
				}
			}
			else
				m_iEquipItemsCount = 0;

			m_iEquipItemsLastTime = get_global_time() + 2;
		}
		
#ifdef ENABLE_MOUNT_SYSTEM
		if (item->GetSubType() == COSTUME_MOUNT)
		{
			if (IsFishing() || IsMining())
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;10014]");
				return false;
			}
		}
#endif

		if (!item->IsEquipped()) {
			EquipItem(item);
		}
		else {
			UnequipItem(item);
		}
		break;

#ifdef ENABLE_SYSTEM_RUNE
	case ITEM_RUNE:
	case ITEM_RUNE_RED:
	case ITEM_RUNE_BLUE:
	case ITEM_RUNE_GREEN:
	case ITEM_RUNE_YELLOW:
	case ITEM_RUNE_BLACK:
#ifdef ENABLE_EXTRABONUS_SYSTEM
		if (!item->IsEquipped()) {
			EquipItem(item);
			CheckRuneExtraBonus(item, 0);
		}
		else {
			UnequipItem(item);
			CheckRuneExtraBonus(item, 1);
		}
		break;
#else
		if (!item->IsEquipped()) {
			EquipItem(item);
		}
		else {
			UnequipItem(item);
		}
		break;
#endif
#endif
#ifdef TITLE_SYSTEM_BYLUZER
	case ITEM_TITLE:
	{
		int title_id = item->GetValue(0);
		quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(GetPlayerID());
		if (pPC->IsRunning())
			return false;
		if (GetExchange() || IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen() || GetShop())
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;2208]");
			return false;
		}
		if (!CanWarp())
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;2208]");
			return false;
		}
		if (m_pTitle[title_id].active != 0)
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;2209]");
			return false;
		}
		m_pTitle[title_id].active = 1;
		item->SetCount(item->GetCount() - 1);
		TitleSendGeneralInfo();
		ChatPacket(CHAT_TYPE_INFO, "[LS;2210]");
	}
	break;
#endif

	case ITEM_GUILD_GUARD:
	{
		sys_log(0, "GUILD_GUARD: UseItemEx called by %s, item vnum=%d", GetName(), item->GetVnum());
		
		if (!GetGuild())
		{
			sys_log(0, "GUILD_GUARD: Player %s has no guild!", GetName());
			ChatPacket(CHAT_TYPE_INFO, "[LS;10118]");
			return false;
		}
		
		sys_log(0, "GUILD_GUARD: Player %s in guild %s, calling EquipItem with bDestInven=%d", 
				GetName(), GetGuild()->GetName(), bDestInven);
		
		GetGuild()->EquipItem(this, bDestInven, item);
		
		sys_log(0, "GUILD_GUARD: EquipItem completed successfully");
	}
	break;
		
	case ITEM_DS:
	{
		if (!item->IsEquipped())
			return false;
		return DSManager::instance().PullOut(this, NPOS, item);
		break;
	}
	case ITEM_SPECIAL_DS:
		if (!item->IsEquipped())
			EquipItem(item);
		else
			UnequipItem(item);
		break;

	case ITEM_FISH:
	{
		if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;1205]");
			return false;
		}
#ifdef ENABLE_NEWSTUFF
		else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;1205]");
			return false;
		}
#endif

		if (item->GetSubType() == FISH_ALIVE)
			fishing::UseFish(this, item);
	}
	break;

	case ITEM_TREASURE_BOX:
	{
		return false;

	}
	break;
	
	case ITEM_GIFTBOX:
	{
		DWORD dwBoxVnum = item->GetVnum();
		std::vector <DWORD> dwVnums;
		std::vector <DWORD> dwCounts;
		std::vector <LPITEM> item_gets(0);
		int count = 0;
		int size = item->GetSize() ; 

		if (dwBoxVnum >= 50255 && dwBoxVnum <= 50259)
		{
			if (!(this->DragonSoul_IsQualified()))
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;1004]");
				return false;
			}

			// Block opening if any DS inventory page is full to prevent
			// cherry-picking the desired DS alchemy type.
			const int iTotalPages = DRAGON_SOUL_INVENTORY_MAX_NUM / DRAGON_SOUL_BOX_SIZE;
			for (int page = 0; page < iTotalPages; ++page)
			{
				WORD wBase = (WORD)(page * DRAGON_SOUL_BOX_SIZE);
				bool bPageFull = true;
				for (int i = 0; i < DRAGON_SOUL_BOX_SIZE && bPageFull; ++i)
				{
					if (IsEmptyItemGrid(TItemPos(DRAGON_SOUL_INVENTORY, wBase + i), 1))
						bPageFull = false;
				}
				if (bPageFull)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;1229]");
					return false;
				}
			}
		}
		
		if (item->IsDragonSoul())
		{
			if (GetEmptyDragonSoulInventory(item) == -1)
			{
				ChatPacket(CHAT_TYPE_INFO, "You haven't empty slots to get open this item");
				return false;
			}
		}
		else
		{
			if (GetEmptyInventory(size) == -1)
			{
				ChatPacket(CHAT_TYPE_INFO, "You haven't empty inventory slots to open this item.");
				return false;
			}
		}

//#ifdef ENABLE_NEWSTUFF
//		if (!PulseManager::Instance().CheckClock(GetPlayerID(), ePulse::BoxOpening))
//		{
//			ChatPacket(CHAT_TYPE_INFO, "[LS;2200;%.2f]", PULSEMANAGER_CLOCK_TO_SEC2(GetPlayerID(), ePulse::BoxOpening));
//			return false;
//		}
//#endif

		if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
		{
			item->SetCount(item->GetCount() - 1);
#ifdef _ENABLE_BATTLEPASS_
			BattlePassManager::Instance().Notify(MISSION_TYPE_CHEST_OPEN, this, item->GetVnum(), 1);
#endif

			for (int i = 0; i < count; i++)
			{
				switch (dwVnums[i])
				{
				case CSpecialItemGroup::GOLD:
					break;

				case CSpecialItemGroup::EXP:
					ChatPacket(CHAT_TYPE_INFO, "[LS;2359]");
					ChatPacket(CHAT_TYPE_INFO, "[LS;2360;%d]", dwCounts[i]);
					break;

				case CSpecialItemGroup::MOB:
					ChatPacket(CHAT_TYPE_INFO, "[LS;2361]");
					break;

				case CSpecialItemGroup::SLOW:
					ChatPacket(CHAT_TYPE_INFO, "[LS;2362]");
					break;

				case CSpecialItemGroup::DRAIN_HP:
					ChatPacket(CHAT_TYPE_INFO, "[LS;2363]");
					break;

				case CSpecialItemGroup::POISON:
					ChatPacket(CHAT_TYPE_INFO, "[LS;2364]");
					break;

				case CSpecialItemGroup::MOB_GROUP:
					ChatPacket(CHAT_TYPE_INFO, "[LS;2361]");
					break;

				default:
					if (item_gets[i])
					{
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
						const std::string& localName = CLocaleItemManager::instance().Find(item_gets[i]->GetVnum(), GetLanguage());
						const char* itemName = !localName.empty() ? localName.c_str() : item_gets[i]->GetName();
#else
						const char* itemName = item_gets[i]->GetName();
#endif
						if (dwCounts[i] > 1)
							ChatPacket(CHAT_TYPE_INFO, "[LS;24;%s;%d]", itemName, dwCounts[i]);
						else
							ChatPacket(CHAT_TYPE_INFO, "[LS;212;%s]", itemName);
					}
				}
			}

#ifdef ENABLE_NEWSTUFF
			{
				bool bDroppedToGround = false;
				for (int i = 0; i < count; i++)
				{
					if (item_gets[i] && !item_gets[i]->GetOwner())
					{
						bDroppedToGround = true;
						break;
					}
				}
				PulseManager::Instance().SetClock(GetPlayerID(), ePulse::BoxOpening,
					std::chrono::milliseconds(bDroppedToGround ? 3000 : 0));
			}
#endif
		}
		else
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;2365]");
			return false;
		}
	}
	break;

	case ITEM_TREASURE_KEY:
	{
		LPITEM item2;

		if (!GetItem(DestCell) || !(item2 = GetItem(DestCell)))
			return false;

		if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
			return false;

		if (item2->GetType() != ITEM_TREASURE_BOX)
		{
			ChatPacket(CHAT_TYPE_TALKING, "[LS;1248]");
			return false;
		}

		if (item->GetValue(0) == item2->GetValue(0))
		{
			DWORD dwBoxVnum = item2->GetVnum();
			std::vector <DWORD> dwVnums;
			std::vector <DWORD> dwCounts;
			std::vector <LPITEM> item_gets(0);
			int count = 0;

			if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
			{
				ITEM_MANAGER::instance().RemoveItem(item);
				ITEM_MANAGER::instance().RemoveItem(item2);

				for (int i = 0; i < count; i++) {
					switch (dwVnums[i])
					{
					case CSpecialItemGroup::GOLD:
						#if defined(__CHATTING_WINDOW_RENEWAL__)
						ChatPacket(CHAT_TYPE_MONEY_INFO, "[LS;1269;%d]", dwCounts[i]);
						#else
						ChatPacket(CHAT_TYPE_INFO, "[LS;1269;%d]", dwCounts[i]);
						#endif
						break;
					case CSpecialItemGroup::EXP:
						#if defined(__CHATTING_WINDOW_RENEWAL__)
						ChatPacket(CHAT_TYPE_EXP_INFO, "[LS;1279]");
						ChatPacket(CHAT_TYPE_EXP_INFO, "[LS;1290;%d]", dwCounts[i]);
						#else
						ChatPacket(CHAT_TYPE_INFO, "[LS;1279]");
						ChatPacket(CHAT_TYPE_INFO, "[LS;1290;%d]", dwCounts[i]);
						#endif
						break;
					case CSpecialItemGroup::MOB:
						ChatPacket(CHAT_TYPE_INFO, "[LS;1299]");
						break;
					case CSpecialItemGroup::SLOW:
						ChatPacket(CHAT_TYPE_INFO, "[LS;1310]");
						break;
					case CSpecialItemGroup::DRAIN_HP:
						ChatPacket(CHAT_TYPE_INFO, "[LS;3]");
						break;
					case CSpecialItemGroup::POISON:
						ChatPacket(CHAT_TYPE_INFO, "[LS;13]");
						break;
#ifdef ENABLE_WOLFMAN_CHARACTER
					case CSpecialItemGroup::BLEEDING:
						ChatPacket(CHAT_TYPE_INFO, "[LS;13]");
						break;
#endif
					case CSpecialItemGroup::MOB_GROUP:
						ChatPacket(CHAT_TYPE_INFO, "[LS;1299]");
						break;
					default:
						if (item_gets[i])
						{
							#if defined(__CHATTING_WINDOW_RENEWAL__)
							if (dwCounts[i] > 1)
								ChatPacket(CHAT_TYPE_ITEM_INFO, "[LS;204;%s;%d]", item_gets[i]->GetName(), dwCounts[i]);
							else
								ChatPacket(CHAT_TYPE_ITEM_INFO, "[LS;35;%s]", item_gets[i]->GetName());
							#else
							if (dwCounts[i] > 1)
								ChatPacket(CHAT_TYPE_INFO, "[LS;204;%s;%d]", item_gets[i]->GetName(), dwCounts[i]);
							else
								ChatPacket(CHAT_TYPE_INFO, "[LS;35;%s]", item_gets[i]->GetName());
							#endif
						}
					}
				}
			}
			else
			{
				ChatPacket(CHAT_TYPE_TALKING, "[LS;46]");
				return false;
			}
		}
		else
		{
			ChatPacket(CHAT_TYPE_TALKING, "[LS;46]");
			return false;
		}
	}
	break;

	case ITEM_SKILLFORGET:
	{
		if (!item->GetSocket(0))
		{
			ITEM_MANAGER::instance().RemoveItem(item);
			return false;
		}

		DWORD dwVnum = item->GetSocket(0);

		if (SkillLevelDown(dwVnum))
		{
			ITEM_MANAGER::instance().RemoveItem(item);
			ChatPacket(CHAT_TYPE_INFO, "[LS;78]");
		}
		else
			ChatPacket(CHAT_TYPE_INFO, "[LS;88]");
	}
	break;

	case ITEM_SKILLBOOK:
	{
		if (IsPolymorphed())
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;458]");
			return false;
		}
		DWORD dwVnum = 0;

		if (item->GetVnum() == 50300)
		{
			dwVnum = item->GetSocket(0);
		}
		else
		{
			dwVnum = item->GetValue(0);
		}

		if (0 == dwVnum)
		{
			ITEM_MANAGER::instance().RemoveItem(item);

			return false;
		}

		if (true == LearnSkillByBook(dwVnum))
		{
#ifdef ENABLE_BOOKS_STACKFIX
			item->SetCount(item->GetCount() - 1);
#else
			ITEM_MANAGER::instance().RemoveItem(item);
#endif

			int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);

			if (distribution_test_server)
				iReadDelay /= 3;

			SetSkillNextReadTime(dwVnum, get_global_time() + iReadDelay);
		}
	}
	break;

	case ITEM_USE:
	{
#ifdef ENABLE_PUNKTY_OSIAGNIEC
		switch(item->GetVnum())
		{
			case 80030:
			case 80031:
			case 80032:
			case 80034:
			case 80035:
			case 80036:
			{
				if (GetExchange() || IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen() || GetShop())
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2208]");
					return false;
				}
				
				int pkt_osiag = item->GetValue(0);
				PointChange(POINT_PKT_OSIAG, pkt_osiag );
				
				ChatPacket(CHAT_TYPE_INFO, "[LS;2213;%d]", pkt_osiag);
				item->SetCount(item->GetCount() - 1);
				break;
			}
		}
#endif
		switch (item->GetVnum())
		{
			case 80005:
			case 80006:
			case 80007:
			case 80008:
			case 80009:
			{
				static const int64_t sGold[5] =
				{
					500000,
					1000000,
					2000000,
					10000000,
					5000000,
				};
			
				if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen()
			#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
					|| GetOfflineShopOwner()
			#endif
				)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2369]");
					return false;
				}
			
				const int64_t amount = sGold[item->GetVnum() - 80005];
				const int64_t currentGold = static_cast<int64_t>(GetGold());
				const int64_t allowedGold = static_cast<int64_t>(GOLD_MAX);
				int itemCount = item->GetCount();
			
				if (itemCount <= 0)
					return false;
			
				int maxUsableItems = (allowedGold - currentGold) / amount;
			
				if (maxUsableItems < 1)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2369]");
					return false;
				}
			
				// Zajisti, že vždy zůstane alespoň 1 kus, pokud jich je víc než 1
				if (itemCount > 1 && maxUsableItems >= itemCount)
					maxUsableItems = itemCount - 1;
			
				// Použij tolik, kolik můžeš
				int usedItems = std::min(itemCount, maxUsableItems);
			
				if (usedItems > 0)
				{
					int64_t totalGoldObtained = static_cast<int64_t>(usedItems) * amount;
					item->SetCount(itemCount - usedItems);
					PointChange(POINT_GOLD, totalGoldObtained);
				}
			
				// Pokud po použití nějaké kusy zůstaly a hráč není na limitu, může použít ještě jeden (ten poslední)
				//if (item->GetCount() == 1)
				//{
				//	const int64_t nTotalMoney = static_cast<int64_t>(GetGold()) + static_cast<int64_t>(amount);
				//	if (nTotalMoney <= allowedGold)
				//	{
				//		item->SetCount(0);
				//		PointChange(POINT_GOLD, amount);
				//	}
				//}
			}
			break;

		default:
			break;
		}
		switch(item->GetVnum())
		{
			case 80037:
			case 80038:
			case 80039:
			case 80040:
			case 201215:
			{
				if (GetExchange() || IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen() || GetShop())
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2208]");
					return false;
				}
				
				int SM = item->GetValue(0);
				if (SM == 0)
					return false;
		
#ifdef USE_ITEMSHOP_RENEWED
				SetAccountMoney(SM, 0, true);
#else
				SetAccountMoney(SM, true);
#endif

				ChatPacket(CHAT_TYPE_INFO, "[LS;2214;%d]", SM);
				item->SetCount(item->GetCount() - 1);
				break;
			}
		}
		switch (item->GetVnum())
		{
			case 80033:
				{
					if (GetExchange() || IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen() || GetShop())
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;2208]");
						return false;
					}
					int Exp = item->GetValue(0);
					if (PetGetLevel() >= 40 && PetGetLevel() < 70) {
						if (GetWear(WEAR_NEW_PET)) {
							PetSetExpPercent(Exp);
							item->SetCount(item->GetCount() - 1);
						}
						else {
							ChatPacket(CHAT_TYPE_INFO, "[LS;2215]");
							return false;
						}
					} else {
						ChatPacket(CHAT_TYPE_INFO, "[LS;2216]");
						return false;
					}
					break;
				}
			case 80056:
				{
					if (GetExchange() || IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen() || GetShop())
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;2208]");
						return false;
					}
					int Exp = item->GetValue(0);
					if (PetGetLevel() >= 70 && PetGetLevel() < 110) {
						if (GetWear(WEAR_NEW_PET)) {
							PetSetExpPercent(Exp);
							item->SetCount(item->GetCount() - 1);
						}
						else {
							ChatPacket(CHAT_TYPE_INFO, "[LS;2215]");
							return false;
						}
					} else {
						ChatPacket(CHAT_TYPE_INFO, "[LS;2217]");

						return false;
					}
					break;
				}
			case 80057:
				{
					if (GetExchange() || IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen() || GetShop())
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;2208]");
						return false;
					}
					int Exp = item->GetValue(0);
					if (PetGetLevel() >= 110 && PetGetLevel() < 150) {
						if (GetWear(WEAR_NEW_PET)) {
							PetSetExpPercent(Exp);
							item->SetCount(item->GetCount() - 1);
						}
						else {
							ChatPacket(CHAT_TYPE_INFO, "[LS;2215]");
							return false;
						}
					} else {
						ChatPacket(CHAT_TYPE_INFO, "[LS;2217]");

						return false;
					}
					break;
				}
		}
#ifdef ENABLE_COLLECT_WINDOW
		switch(item->GetVnum())
		{
			case 72348: // 75% reduction
			case 72349: // 100% reduction
			{
				if (GetExchange() || IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen() || GetShop())
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2208]");
					return false;
				}
			
				int val = item->GetValue(0); // Reduction percentage (75 for 72348, 100 for 72349)
				int type = item->GetValue(1);
			
				if (type == 1) { // Only Collector cooldown is affected
					int collector_state = GetQuestFlag("collector.state");
					std::string delay_flag_key = "collector.item_delay" + std::to_string(collector_state);
			
					time_t item_delay = GetQuestFlag(delay_flag_key);
					time_t current_time = get_global_time();
					time_t remaining_time = item_delay - current_time;
			
					if (remaining_time > 0) {
						// Calculate reduced time
						time_t reduced_time = remaining_time - (remaining_time * val / 100);
			
						// Ensure reduced time is at least zero
						if (reduced_time < 0) {
							reduced_time = 0;
						}
			
						// Update cooldown
						SetQuestFlag(delay_flag_key, current_time + reduced_time);
						ChatPacket(CHAT_TYPE_COMMAND, "UpdateTime 1 %d", reduced_time);
						item->SetCount(item->GetCount() - 1);
						ChatPacket(CHAT_TYPE_INFO, "[LS;2328]");
						ConvertTime(reduced_time);
#ifdef ENABLE_NOTIFICATION_SYSTEM
						if (reduced_time == 0)
							StopCollectNotifyTimer(this, true);
#endif
					} else {
						// No cooldown to reduce
						ChatPacket(CHAT_TYPE_INFO, "[LS;2329]");
						return false;
					}
				}
			
				break;
			}

			case 71036:
			case 71037:
				{
					if (GetExchange() || IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen() || GetShop())
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;2208]");
						return false;
					}
					int val = item->GetValue(0);
					int type = item->GetValue(1);
					int max_increase = item->GetValue(2);
					if (type == 0) {
						int take_chance = GetQuestFlag("biologist.take_chance");
						if (take_chance == max_increase || take_chance > max_increase) {
							ChatPacket(CHAT_TYPE_INFO, "[LS;2327]");
							return false;
						}
						SetQuestFlag("biologist.take_chance", take_chance+val);
						item->SetCount(item->GetCount()-1);

						ChatPacket(CHAT_TYPE_INFO, "[LS;2330]");

						int new_chance = GetQuestFlag("biologist.take_chance");

						ChatPacket(CHAT_TYPE_INFO, "[LS;2331;%d]", new_chance);
						ChatPacket(CHAT_TYPE_COMMAND, "UpdateChance 0 %d", new_chance);
					} else if (type == 1) {
						int take_chance = GetQuestFlag("collector.take_chance");
						if (take_chance == max_increase || take_chance > max_increase) {
							ChatPacket(CHAT_TYPE_INFO, "[LS;2327]");
							return false;
						}
						SetQuestFlag("collector.take_chance", take_chance+val);
						item->SetCount(item->GetCount()-1);

						ChatPacket(CHAT_TYPE_INFO, "[LS;2330]");

						int new_chance = GetQuestFlag("collector.take_chance");
						
						ChatPacket(CHAT_TYPE_INFO, "[LS;2331;%d]", new_chance);
						ChatPacket(CHAT_TYPE_COMMAND, "UpdateChance 1 %d", new_chance);
					}
					break;
				}
		}
#endif

		if (item->GetVnum() > 50800 && item->GetVnum() <= 50820)
		{
			if (test_server)
				sys_log(0, "ADD addtional effect : vnum(%d) subtype(%d)", item->GetOriginalVnum(), item->GetSubType());

			int affect_type = AFFECT_EXP_BONUS_EURO_FREE;
			int apply_type = aApplyInfo[item->GetValue(0)].bPointType;
			int apply_value = item->GetValue(2);
			int apply_duration = item->GetValue(1);

			switch (item->GetSubType())
			{
			case USE_ABILITY_UP:
				if (FindAffect(affect_type, apply_type))
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;99]");
					return false;
				}

				{
					switch (item->GetValue(0))
					{
					case APPLY_MOV_SPEED:
						AddAffect(affect_type, apply_type, apply_value, AFF_MOV_SPEED_POTION, apply_duration, 0, true, true);
						break;

					case APPLY_ATT_SPEED:
						AddAffect(affect_type, apply_type, apply_value, AFF_ATT_SPEED_POTION, apply_duration, 0, true, true);
						break;

					case APPLY_CRITICAL_PCT:
					case APPLY_PENETRATE_PCT:
					case APPLY_STR:

					case APPLY_DEX:
					case APPLY_CON:
					case APPLY_INT:
					case APPLY_CAST_SPEED:
					case APPLY_RESIST_MAGIC:
					case APPLY_ATT_GRADE_BONUS:
					case APPLY_DEF_GRADE_BONUS:
						AddAffect(affect_type, apply_type, apply_value, 0, apply_duration, 0, true, true);
						break;
					}
				}

				if (GetDungeon())
					GetDungeon()->UsePotion(this);

				if (GetWarMap())
					GetWarMap()->UsePotion(this, item);

				item->SetCount(item->GetCount() - 1);
				break;

			case USE_AFFECT:
			{
				if (FindAffect(AFFECT_EXP_BONUS_EURO_FREE, aApplyInfo[item->GetValue(1)].bPointType))
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;99]");
				}
				else
				{
					AddAffect(AFFECT_EXP_BONUS_EURO_FREE, aApplyInfo[item->GetValue(1)].bPointType, item->GetValue(2), 0, item->GetValue(3), 0, false, true);
					item->SetCount(item->GetCount() - 1);
				}
			}
			break;

			case USE_POTION_NODELAY:
			{
				if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
				{
					if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit") > 0)
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;403]");
						return false;
					}

					switch (item->GetVnum())
					{
					case 70020:
					case 71018:
					case 71019:
					case 71020:
						if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count") < 10000)
						{
							if (m_nPotionLimit <= 0)
							{
								ChatPacket(CHAT_TYPE_INFO, "[LS;122]");
								return false;
							}
						}
						break;

					default:
						ChatPacket(CHAT_TYPE_INFO, "[LS;403]");
						return false;
						break;
					}
				}
#ifdef ENABLE_NEWSTUFF
				else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;1205]");
					return false;
				}
#endif
				bool used = false;

				if (item->GetValue(0) != 0)
				{
					if (GetHP() < GetMaxHP())
					{
						PointChange(POINT_HP, item->GetValue(0) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
						EffectPacket(SE_HPUP_RED);
						used = TRUE;
					}
				}

				if (item->GetValue(1) != 0)
				{
					if (GetSP() < GetMaxSP())
					{
						PointChange(POINT_SP, item->GetValue(1) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
						EffectPacket(SE_SPUP_BLUE);
						used = TRUE;
					}
				}

				if (item->GetValue(3) != 0)
				{
					if (GetHP() < GetMaxHP())
					{
						PointChange(POINT_HP, item->GetValue(3) * GetMaxHP() / 100);
						EffectPacket(SE_HPUP_RED);
						used = TRUE;
					}
				}

				if (item->GetValue(4) != 0)
				{
					if (GetSP() < GetMaxSP())
					{
						PointChange(POINT_SP, item->GetValue(4) * GetMaxSP() / 100);
						EffectPacket(SE_SPUP_BLUE);
						used = TRUE;
					}
				}

				if (used)
				{
					if (item->GetVnum() == 50085 || item->GetVnum() == 50086)
					{
						if (test_server)
							ChatPacket(CHAT_TYPE_INFO, "[LS;132]");
						SetUseSeedOrMoonBottleTime();
					}
					if (GetDungeon())
						GetDungeon()->UsePotion(this);

					if (GetWarMap())
						GetWarMap()->UsePotion(this, item);

					m_nPotionLimit--;

					//RESTRICT_USE_SEED_OR_MOONBOTTLE
					item->SetCount(item->GetCount() - 1);
					//END_RESTRICT_USE_SEED_OR_MOONBOTTLE
				}
#ifdef ENABLE_NEWSTUFF
				if (!PulseManager::Instance().IncreaseClock(GetPlayerID(), ePulse::Blogo, std::chrono::milliseconds(1000)))
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2200;%.2f]", PULSEMANAGER_CLOCK_TO_SEC2(GetPlayerID(), ePulse::Blogo));
					return false;
				}
#endif
			}
			break;
			}

			return true;
		}

		if (item->GetVnum() >= 27863 && item->GetVnum() <= 27883)
		{
			if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;1205]");
				return false;
			}
#ifdef ENABLE_NEWSTUFF
			else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;1205]");
				return false;
			}
#endif
		}

		if (test_server)
		{
			sys_log(0, "USE_ITEM %s Type %d SubType %d vnum %d", item->GetName(), item->GetType(), item->GetSubType(), item->GetOriginalVnum());
		}

		switch (item->GetSubType())
		{
#ifdef ENABLE_MOUNT_SYSTEM
		case USE_COMPANION_EXP_VOUCHER:
		{
			int iType = item->GetValue(0);
			int iValue = item->GetValue(1);
			int iMinCompanionLevel = item->GetValue(2);
			int iMaxCompanionLevel = item->GetValue(3);
			
			int used = 0;
			
			WORD wItemCount = 1;
			int64_t llAmount = 0;

			switch (iType)
			{
				case 1:
				{
					if (GetMountLevel() < iMinCompanionLevel)
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;10015;%d]", iMinCompanionLevel);
						break;
					}
					
					if (iMaxCompanionLevel > 0 && GetMountLevel() >= iMaxCompanionLevel)
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;10016;%d]", iMaxCompanionLevel);
						break;
					}
					
					wItemCount = (item->GetCount() >= 200) ? 200 : item->GetCount();
					for (used = 0; used < wItemCount; used++)
					{
						if (GetMountLevel() >= 150)
							break;
						if (GetMountLevel() < iMinCompanionLevel)
							break;
						if (iMaxCompanionLevel > 0 && GetMountLevel() >= iMaxCompanionLevel)
							break;
		
						llAmount += iValue;
						UpdateMountExp(iValue);
					}

					ChatPacket(CHAT_TYPE_INFO, "[LS;10020;%d]", llAmount);

				} break;
				default:
					break;
			}
			
			item->SetCount(item->GetCount() - used);
		} break;
#endif
		case USE_TIME_CHARGE_PER:
		{
			LPITEM pDestItem = GetItem(DestCell);
			if (NULL == pDestItem)
			{
				return false;
			}

			if (pDestItem->IsDragonSoul())
			{
				int ret;
				char buf[128];
				if (item->GetVnum() == DRAGON_HEART_VNUM)
				{
					ret = pDestItem->GiveMoreTime_Per((float)item->GetSocket(ITEM_SOCKET_CHARGING_AMOUNT_IDX));
				}
				else
				{
					ret = pDestItem->GiveMoreTime_Per((float)item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
				}
				if (ret > 0)
				{
					if (item->GetVnum() == DRAGON_HEART_VNUM)
					{
						sprintf(buf, "Inc %ds by item{VN:%d SOC%d:%d}", ret, item->GetVnum(), ITEM_SOCKET_CHARGING_AMOUNT_IDX, item->GetSocket(ITEM_SOCKET_CHARGING_AMOUNT_IDX));
					}
					else
					{
						sprintf(buf, "Inc %ds by item{VN:%d VAL%d:%d}", ret, item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
					}

					ChatPacket(CHAT_TYPE_INFO, "[LS;1093;%d]", ret);
					item->SetCount(item->GetCount() - 1);
					LogManager::instance().ItemLog(this, item, "DS_CHARGING_SUCCESS", buf);
					return true;
				}
				else
				{
					if (item->GetVnum() == DRAGON_HEART_VNUM)
					{
						sprintf(buf, "No change by item{VN:%d SOC%d:%d}", item->GetVnum(), ITEM_SOCKET_CHARGING_AMOUNT_IDX, item->GetSocket(ITEM_SOCKET_CHARGING_AMOUNT_IDX));
					}
					else
					{
						sprintf(buf, "No change by item{VN:%d VAL%d:%d}", item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
					}

					ChatPacket(CHAT_TYPE_INFO, "[LS;1066]");
					LogManager::instance().ItemLog(this, item, "DS_CHARGING_FAILED", buf);
					return false;
				}
			}
			else
				return false;
		}
		break;
		case USE_TIME_CHARGE_FIX:
		{
			LPITEM pDestItem = GetItem(DestCell);
			if (NULL == pDestItem)
			{
				return false;
			}

			if (pDestItem->IsDragonSoul())
			{
				int ret = pDestItem->GiveMoreTime_Fix(item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
				char buf[128];
				if (ret)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;1093;%d]", ret);
					sprintf(buf, "Increase %ds by item{VN:%d VAL%d:%d}", ret, item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
					LogManager::instance().ItemLog(this, item, "DS_CHARGING_SUCCESS", buf);
					item->SetCount(item->GetCount() - 1);
					return true;
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;1066]");
					sprintf(buf, "No change by item{VN:%d VAL%d:%d}", item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
					LogManager::instance().ItemLog(this, item, "DS_CHARGING_FAILED", buf);
					return false;
				}
			}
			else
				return false;
		}
		break;

		case USE_SPECIAL:
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
			if (item->GetVnum() == 71999)
			{
				if (GetBuffNPCSystem()->IsActive()) {
					ChatPacket(CHAT_TYPE_INFO, "[LS;2218]");
					return false;
				}

				if (item->GetSocket(0) == 0) {
					SetBuffSealVID(item->GetCell());

					TPacketGCBuffNPCAction packet;
					packet.bHeader = HEADER_GC_BUFF_NPC_ACTION;
					packet.bAction = 0;
					packet.dValue0 = 0;
					packet.dValue1 = 0;
					GetDesc()->Packet(&packet, sizeof(packet));
				}
				else
				{
					{
						char szEscName[CHARACTER_NAME_MAX_LEN * 2 + 1];
						DBManager::instance().EscapeString(szEscName, sizeof(szEscName), GetName(), strlen(GetName()));
						DBManager::instance().DirectQuery("UPDATE srv1_player.player_buff_npc SET player_name='%s', player_id=%d WHERE id=%d ", szEscName, GetPlayerID(), item->GetSocket(0));
					}
					item->RemoveFromCharacter();
					GetBuffNPCSystem()->LoadBuffNPC();
					SetQuestFlag("buff_npc.is_summon", 1);
					GetBuffNPCSystem()->Summon();
					ChatPacket(CHAT_TYPE_INFO, "[LS;2219]");
				}
			}
#endif

			if (item->GetVnum() >= 91201 && item->GetVnum() <= 91205)
			{
				LPITEM item2;


				if (!IsValidItemPosition(DestCell) || !(item2 = GetInventoryItem(wDestCell)))
					return false;

				if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
					return false;

				if (item2->GetType() != ITEM_ROD)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2445]");
					return false;
				}

				int newBonusIdx = -1;
				for (int i = 0; i < 3; i++)
				{
					if (item2->GetAttributeType(i) == 0)
					{
						newBonusIdx = i;
						break;
					}
				}

				if (newBonusIdx == -1)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2446]");
					return false;
				}

				uint8_t bonusTypeToAdd = static_cast<uint8_t>(item->GetValue(0));

				bool hasThisBonusAlready = false;
				for (int i = 0; i < 3; i++)
				{
					if (item2->GetAttributeType(i) == bonusTypeToAdd)
					{
						hasThisBonusAlready = true;
						break;
					}
				}

				if (hasThisBonusAlready)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2447]");
					return false;
				}

				item2->SetForceAttribute(newBonusIdx, bonusTypeToAdd, static_cast<short>(item->GetValue(1)));
				item2->SetForceAttribute(3 + newBonusIdx, 1, static_cast<short>(item->GetSocket(0)));
				ChatPacket(CHAT_TYPE_INFO, "[LS;2448]");

				item->SetCount(item->GetCount() - 1);

				return false;
			}

			if (item->GetVnum() == 79015)
			{
				LPITEM item2;
				std::set<int> allowedSubTypes = { ARMOR_HEAD, ARMOR_FOOTS, ARMOR_SHIELD };
				
				auto isAllowedSubType = [&allowedSubTypes](int subType) {
					return allowedSubTypes.find(subType) != allowedSubTypes.end();
				};
				
				if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
				{
					return false;
				}
		
				if (item2->IsExchanging() || item2->IsEquipped())
				{
					return false;
				}
				if (item2->GetType() != ITEM_ARMOR || !isAllowedSubType(item2->GetSubType()))
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2456]");
					return false;
				}
				
				struct Rune {
					int vnum;
					int probability; // probability out of 1000
				};
				
				std::vector<Rune> runes = {
					{89500, 500}, // Rune I
					{89501, 300}, // Rune II
					{89502, 200}, // Rune III
					{89503, 500}, // Rune I
					{89504, 300}, // Rune II
					{89505, 200}, // Rune III
					{89506, 500}, // Rune I
					{89507, 300}, // Rune II
					{89508, 200}, // Rune III
					{89509, 500}, // Rune I
					{89510, 300}, // Rune II
					{89511, 200}, // Rune III
					{89512, 500}, // Rune I
					{89513, 300}, // Rune II
					{89514, 200}, // Rune III
					{89515, 500}, // Rune I
					{89516, 300}, // Rune II
					{89517, 200}, // Rune III
					{89518, 500}, // Rune I
					{89519, 300}, // Rune II
					{89520, 200}, // Rune III
					{89521, 500}, // Rune I
					{89522, 300}, // Rune II
					{89523, 200}, // Rune III
					{89524, 500}, // Rune I
					{89525, 300}, // Rune II
					{89526, 200}, // Rune III
					{89527, 500}, // Rune I
					{89528, 300}, // Rune II
					{89529, 200}, // Rune III
					{89530, 500}, // Rune I
					{89531, 300}, // Rune II
					{89532, 200}, // Rune III
					{89533, 500}, // Rune I
					{89534, 300}, // Rune II
					{89535, 200}, // Rune III
					{89536, 500}, // Rune I
					{89537, 300}, // Rune II
					{89538, 200}, // Rune III
					{89539, 500}, // Rune I
					{89540, 300}, // Rune II
					{89541, 200}, // Rune III
					{89542, 500}, // Rune I
					{89543, 300}, // Rune II
					{89544, 200}, // Rune III
					{89545, 500}, // Rune I
					{89546, 300}, // Rune II
					{89547, 200}, // Rune III
					{89548, 500}, // Rune I
					{89549, 300}, // Rune II
					{89550, 200}  // Rune III
				};
				
				// Function to select a rune based on probability
				auto selectRune = [&runes]() -> int {
					int totalProbability = 0;
					for (const auto& rune : runes) {
						totalProbability += rune.probability;
					}
				
					int randomValue = rand() % totalProbability;
					int cumulativeProbability = 0;
					for (const auto& rune : runes) {
						cumulativeProbability += rune.probability;
						if (randomValue < cumulativeProbability) {
							return rune.vnum;
						}
					}
					return 89500; // Default return in case of error
				};
				
				// Seed random number generator
				srand(time(nullptr));

				int selectedRuneVnum = selectRune();
		
				// Attempt to add the selected rune as a socket
				int i;
				for (i = 0; i < 3; ++i)
				{
					if (item2->GetSocket(i) >= 0 && item2->GetSocket(i) <= 2 && item2->GetSocket(i) >= item->GetValue(2))
					{
						if (number(1, 100) <= 75) // 75% success rate
						{
							ChatPacket(CHAT_TYPE_INFO, "[LS;2457]");
							item2->SetSocket(i, selectedRuneVnum);
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, "[LS;2458]");
						}
			
						// Log the item modification
						// LogManager::instance().ItemLog(this, item2, "SOCKET", item->GetName());
						item->SetCount(item->GetCount() - 1);
						break;
					}
				}
		
				if (i == 3)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2459]");
				}
			}

			if (item->GetVnum() == 79016)
			{
				LPITEM item2;
				std::set<int> allowedSubTypes = { ARMOR_HEAD, ARMOR_FOOTS, ARMOR_SHIELD };
				
				auto isAllowedSubType = [&allowedSubTypes](int subType) {
					return allowedSubTypes.find(subType) != allowedSubTypes.end();
				};
				
				if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
				{
					return false;
				}
		
				if (item2->IsExchanging() || item2->IsEquipped())
				{
					return false;
				}
				
				if (item2->GetType() != ITEM_ARMOR || !isAllowedSubType(item2->GetSubType()))
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2456]");
					return false;
				}
				
				struct Rune {
					int vnum;
					int probability; // probability out of 1000
				};
				
				// Normal rune probabilitie
				std::vector<Rune> normalRunes = {
					{89500, 500}, // Rune I
					{89501, 300}, // Rune II
					{89502, 200}, // Rune III
					{89503, 500}, // Rune I
					{89504, 300}, // Rune II
					{89505, 200}, // Rune III
					{89506, 500}, // Rune I
					{89507, 300}, // Rune II
					{89508, 200}, // Rune III
					{89509, 500}, // Rune I
					{89510, 300}, // Rune II
					{89511, 200}, // Rune III
					{89512, 500}, // Rune I
					{89513, 300}, // Rune II
					{89514, 200}, // Rune III
					{89515, 500}, // Rune I
					{89516, 300}, // Rune II
					{89517, 200}, // Rune III
					{89518, 500}, // Rune I
					{89519, 300}, // Rune II
					{89520, 200}, // Rune III
					{89521, 500}, // Rune I
					{89522, 300}, // Rune II
					{89523, 200}, // Rune III
					{89524, 500}, // Rune I
					{89525, 300}, // Rune II
					{89526, 200}, // Rune III
					{89527, 500}, // Rune I
					{89528, 300}, // Rune II
					{89529, 200}, // Rune III
					{89530, 500}, // Rune I
					{89531, 300}, // Rune II
					{89532, 200}, // Rune III
					{89533, 500}, // Rune I
					{89534, 300}, // Rune II
					{89535, 200}, // Rune III
					{89536, 500}, // Rune I
					{89537, 300}, // Rune II
					{89538, 200}, // Rune III
					{89539, 500}, // Rune I
					{89540, 300}, // Rune II
					{89541, 200}, // Rune III
					{89542, 500}, // Rune I
					{89543, 300}, // Rune II
					{89544, 200}, // Rune III
					{89545, 500}, // Rune I
					{89546, 300}, // Rune II
					{89547, 200}, // Rune III
					{89548, 500}, // Rune I
					{89549, 300}, // Rune II
					{89550, 200}  // Rune III
				};
								
				// Event rune probabilities (easier to get stage III)
				std::vector<Rune> eventRunes = {
					{89500, 0}, // Rune I
					{89501, 300}, // Rune II
					{89502, 500}, // Rune III
					{89503, 0}, // Rune I
					{89504, 300}, // Rune II
					{89505, 500}, // Rune III
					{89506, 0}, // Rune I
					{89507, 300}, // Rune II
					{89508, 500}, // Rune III
					{89509, 0}, // Rune I
					{89510, 300}, // Rune II
					{89511, 500}, // Rune III
					{89512, 0}, // Rune I
					{89513, 300}, // Rune II
					{89514, 500}, // Rune III
					{89515, 0}, // Rune I
					{89516, 300}, // Rune II
					{89517, 500}, // Rune III
					{89518, 0}, // Rune I
					{89519, 300}, // Rune II
					{89520, 500}, // Rune III
					{89521, 0}, // Rune I
					{89522, 300}, // Rune II
					{89523, 500}, // Rune III
					{89524, 0}, // Rune I
					{89525, 300}, // Rune II
					{89526, 500}, // Rune III
					{89527, 0}, // Rune I
					{89528, 300}, // Rune II
					{89529, 500}, // Rune III
					{89530, 0}, // Rune I
					{89531, 300}, // Rune II
					{89532, 500}, // Rune III
					{89533, 0}, // Rune I
					{89534, 300}, // Rune II
					{89535, 500}, // Rune III
					{89536, 0}, // Rune I
					{89537, 300}, // Rune II
					{89538, 500}, // Rune III
					{89539, 0}, // Rune I
					{89540, 300}, // Rune II
					{89541, 500}, // Rune III
					{89542, 0}, // Rune I
					{89543, 300}, // Rune II
					{89544, 500}, // Rune III
					{89545, 0}, // Rune I
					{89546, 300}, // Rune II
					{89547, 500}, // Rune III
					{89548, 0}, // Rune I
					{89549, 300}, // Rune II
					{89550, 500}  // Rune III
				};
				
				
				const auto isRuneEventActive = CHARACTER_MANAGER::Instance().CheckEventIsActive(SWITCH_RUNE_EVENT, 0, this);
				auto& runes = isRuneEventActive ? eventRunes : normalRunes;
				
				// Function to select a rune based on probability
				auto selectRune = [&runes]() -> int {
					int totalProbability = 0;
					for (const auto& rune : runes) {
						totalProbability += rune.probability;
					}
				
					int randomValue = rand() % totalProbability;
					int cumulativeProbability = 0;
					for (const auto& rune : runes) {
						cumulativeProbability += rune.probability;
						if (randomValue < cumulativeProbability) {
							return rune.vnum;
						}
					}
					return 89500; // Default return in case of error
				};
				
				// Needed so the runes are always different
				srand(time(nullptr));
		
				// Check if the item has exactly 3 runes
				int runeCount = 0;
				for (int i = 0; i < 3; ++i) {
					if (item2->GetSocket(i) != 0) {
						++runeCount;
					}
				}
				
				if (runeCount != 3)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2460]");
					return false;
				}
				else
				{
					// Change all runes in the sockets
					for (int i = 0; i < 3; ++i)
					{
						int selectedRuneVnum = selectRune();
						item2->SetSocket(i, selectedRuneVnum);
					}
				}
				
				bool shouldConsumeItem = true;
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
				if (GetTitleAchievement() == 10 || GetTitleAchievementPremium() == 10)
				{
					// 5% chance to not consume the item
					if (number(0, 100) < 5)
					{
						shouldConsumeItem = false;
						ChatPacket(CHAT_TYPE_INFO, "[LS;10116]");
						ChatPacket(CHAT_TYPE_INFO, "[LS;2461]");
					}
				}
#endif
				
				if (shouldConsumeItem)
				{
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
					const int RareChangersCount = GetQuestFlag("AchievementTitle.relict_used") + 1;
					SetQuestFlag("AchievementTitle.relict_used", RareChangersCount);
							
					CTitleAchievementTitle::instance().CheckAndUnlockTitles(this);
							
					// Send progress update to client
					ChatPacket(CHAT_TYPE_COMMAND, "title_progress_relict_count %d", RareChangersCount);
#endif
					item->SetCount(item->GetCount() - 1);
					ChatPacket(CHAT_TYPE_INFO, "[LS;2461]");
				}
			}

			if (item->GetVnum() == 79017)
			{
				LPITEM item2;
				std::set<int> allowedSubTypes = { ARMOR_HEAD, ARMOR_FOOTS, ARMOR_SHIELD };
				
				auto isAllowedSubType = [&allowedSubTypes](int subType) {
					return allowedSubTypes.find(subType) != allowedSubTypes.end();
				};
				
				if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
				{
					return false;
				}
		
				if (item2->IsExchanging() || item2->IsEquipped())
				{
					return false;
				}
				
				if (item2->GetType() != ITEM_ARMOR || !isAllowedSubType(item2->GetSubType()))
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2456]");
					return false;
				}
				
				struct Rune {
					int vnum;
					int probability; // probability out of 1000
				};
				
				// Normal rune probabilitie
				std::vector<Rune> normalRunes = {
					{89544, 500}, // Rune III
				};
								
				// Event rune probabilities (easier to get stage III)
				std::vector<Rune> eventRunes = {
					{89544, 500}, // Rune III
				};
				
				
				const auto isRuneEventActive = CHARACTER_MANAGER::Instance().CheckEventIsActive(SWITCH_RUNE_EVENT, 0, this);
				auto& runes = isRuneEventActive ? eventRunes : normalRunes;
				
				// Function to select a rune based on probability
				auto selectRune = [&runes]() -> int {
					int totalProbability = 0;
					for (const auto& rune : runes) {
						totalProbability += rune.probability;
					}
				
					int randomValue = rand() % totalProbability;
					int cumulativeProbability = 0;
					for (const auto& rune : runes) {
						cumulativeProbability += rune.probability;
						if (randomValue < cumulativeProbability) {
							return rune.vnum;
						}
					}
					return 89500; // Default return in case of error
				};
				
				// Needed so the runes are always different
				srand(time(nullptr));
		
				// Check if the item has exactly 3 runes
				int runeCount = 0;
				for (int i = 0; i < 3; ++i) {
					if (item2->GetSocket(i) != 0) {
						++runeCount;
					}
				}
				
				if (runeCount != 3)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2460]");
					return false;
				}
				else
				{
					// Change all runes in the sockets
					for (int i = 0; i < 3; ++i)
					{
						int selectedRuneVnum = selectRune();
						item2->SetSocket(i, selectedRuneVnum);
					}
				}
				
				item->SetCount(item->GetCount() - 1);
				ChatPacket(CHAT_TYPE_INFO, "[LS;2461]");
			}

			if (item->GetVnum() == 79018)
			{
				LPITEM item2;
				std::set<int> allowedSubTypes = { ARMOR_HEAD, ARMOR_FOOTS, ARMOR_SHIELD };
				
				auto isAllowedSubType = [&allowedSubTypes](int subType) {
					return allowedSubTypes.find(subType) != allowedSubTypes.end();
				};
				
				if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
				{
					return false;
				}
		
				if (item2->IsExchanging() || item2->IsEquipped())
				{
					return false;
				}
				
				if (item2->GetType() != ITEM_ARMOR || !isAllowedSubType(item2->GetSubType()))
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2456]");
					return false;
				}
				
				struct Rune {
					int vnum;
					int probability; // probability out of 1000
				};
				
				// Normal rune probabilitie
				std::vector<Rune> normalRunes = {
					{89505, 500}, // Rune III
				};
								
				// Event rune probabilities (easier to get stage III)
				std::vector<Rune> eventRunes = {
					{89505, 500}, // Rune III
				};
				
				
				const auto isRuneEventActive = CHARACTER_MANAGER::Instance().CheckEventIsActive(SWITCH_RUNE_EVENT, 0, this);
				auto& runes = isRuneEventActive ? eventRunes : normalRunes;
				
				// Function to select a rune based on probability
				auto selectRune = [&runes]() -> int {
					int totalProbability = 0;
					for (const auto& rune : runes) {
						totalProbability += rune.probability;
					}
				
					int randomValue = rand() % totalProbability;
					int cumulativeProbability = 0;
					for (const auto& rune : runes) {
						cumulativeProbability += rune.probability;
						if (randomValue < cumulativeProbability) {
							return rune.vnum;
						}
					}
					return 89500; // Default return in case of error
				};
				
				// Needed so the runes are always different
				srand(time(nullptr));
		
				// Check if the item has exactly 3 runes
				int runeCount = 0;
				for (int i = 0; i < 3; ++i) {
					if (item2->GetSocket(i) != 0) {
						++runeCount;
					}
				}
				
				if (runeCount != 3)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2460]");
					return false;
				}
				else
				{
					// Change all runes in the sockets
					for (int i = 0; i < 3; ++i)
					{
						int selectedRuneVnum = selectRune();
						item2->SetSocket(i, selectedRuneVnum);
					}
				}
				
				item->SetCount(item->GetCount() - 1);
				ChatPacket(CHAT_TYPE_INFO, "[LS;2461]");
			}

			if (item->GetVnum() == 79019)
			{
				LPITEM item2;
				std::set<int> allowedSubTypes = { ARMOR_HEAD, ARMOR_FOOTS, ARMOR_SHIELD };
				
				auto isAllowedSubType = [&allowedSubTypes](int subType) {
					return allowedSubTypes.find(subType) != allowedSubTypes.end();
				};
				
				if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
				{
					return false;
				}
		
				if (item2->IsExchanging() || item2->IsEquipped())
				{
					return false;
				}
				
				if (item2->GetType() != ITEM_ARMOR || !isAllowedSubType(item2->GetSubType()))
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2456]");
					return false;
				}
				
				struct Rune {
					int vnum;
					int probability; // probability out of 1000
				};
				
				// Normal rune probabilitie
				std::vector<Rune> normalRunes = {
					{89511, 500}, // Rune III
				};
								
				// Event rune probabilities (easier to get stage III)
				std::vector<Rune> eventRunes = {
					{89511, 500}, // Rune III
				};
				
				
				const auto isRuneEventActive = CHARACTER_MANAGER::Instance().CheckEventIsActive(SWITCH_RUNE_EVENT, 0, this);
				auto& runes = isRuneEventActive ? eventRunes : normalRunes;
				
				// Function to select a rune based on probability
				auto selectRune = [&runes]() -> int {
					int totalProbability = 0;
					for (const auto& rune : runes) {
						totalProbability += rune.probability;
					}
				
					int randomValue = rand() % totalProbability;
					int cumulativeProbability = 0;
					for (const auto& rune : runes) {
						cumulativeProbability += rune.probability;
						if (randomValue < cumulativeProbability) {
							return rune.vnum;
						}
					}
					return 89500; // Default return in case of error
				};
				
				// Needed so the runes are always different
				srand(time(nullptr));
		
				// Check if the item has exactly 3 runes
				int runeCount = 0;
				for (int i = 0; i < 3; ++i) {
					if (item2->GetSocket(i) != 0) {
						++runeCount;
					}
				}
				
				if (runeCount != 3)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2460]");
					return false;
				}
				else
				{
					// Change all runes in the sockets
					for (int i = 0; i < 3; ++i)
					{
						int selectedRuneVnum = selectRune();
						item2->SetSocket(i, selectedRuneVnum);
					}
				}
				
				item->SetCount(item->GetCount() - 1);
				ChatPacket(CHAT_TYPE_INFO, "[LS;2461]");
			}

			if (item->GetVnum() == 79020)
			{
				LPITEM item2;
				std::set<int> allowedSubTypes = { ARMOR_HEAD, ARMOR_FOOTS, ARMOR_SHIELD };
				
				auto isAllowedSubType = [&allowedSubTypes](int subType) {
					return allowedSubTypes.find(subType) != allowedSubTypes.end();
				};
				
				if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
				{
					return false;
				}
		
				if (item2->IsExchanging() || item2->IsEquipped())
				{
					return false;
				}
				
				if (item2->GetType() != ITEM_ARMOR || !isAllowedSubType(item2->GetSubType()))
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2456]");
					return false;
				}
				
				struct Rune {
					int vnum;
					int probability; // probability out of 1000
				};
				
				// Normal rune probabilitie
				std::vector<Rune> normalRunes = {
					{89502, 500}, // Rune III
				};
								
				// Event rune probabilities (easier to get stage III)
				std::vector<Rune> eventRunes = {
					{89502, 500}, // Rune III
				};
				
				
				const auto isRuneEventActive = CHARACTER_MANAGER::Instance().CheckEventIsActive(SWITCH_RUNE_EVENT, 0, this);
				auto& runes = isRuneEventActive ? eventRunes : normalRunes;
				
				// Function to select a rune based on probability
				auto selectRune = [&runes]() -> int {
					int totalProbability = 0;
					for (const auto& rune : runes) {
						totalProbability += rune.probability;
					}
				
					int randomValue = rand() % totalProbability;
					int cumulativeProbability = 0;
					for (const auto& rune : runes) {
						cumulativeProbability += rune.probability;
						if (randomValue < cumulativeProbability) {
							return rune.vnum;
						}
					}
					return 89500; // Default return in case of error
				};
				
				// Needed so the runes are always different
				srand(time(nullptr));
		
				// Check if the item has exactly 3 runes
				int runeCount = 0;
				for (int i = 0; i < 3; ++i) {
					if (item2->GetSocket(i) != 0) {
						++runeCount;
					}
				}
				
				if (runeCount != 3)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2460]");
					return false;
				}
				else
				{
					// Change all runes in the sockets
					for (int i = 0; i < 3; ++i)
					{
						int selectedRuneVnum = selectRune();
						item2->SetSocket(i, selectedRuneVnum);
					}
				}
				
				item->SetCount(item->GetCount() - 1);
				ChatPacket(CHAT_TYPE_INFO, "[LS;2461]");
			}

			if (item->GetVnum() == 79021)
			{
				LPITEM item2;
				std::set<int> allowedSubTypes = { ARMOR_HEAD, ARMOR_FOOTS, ARMOR_SHIELD };
				
				auto isAllowedSubType = [&allowedSubTypes](int subType) {
					return allowedSubTypes.find(subType) != allowedSubTypes.end();
				};
				
				if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
				{
					return false;
				}
		
				if (item2->IsExchanging() || item2->IsEquipped())
				{
					return false;
				}
				
				if (item2->GetType() != ITEM_ARMOR || !isAllowedSubType(item2->GetSubType()))
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2456]");
					return false;
				}
				
				struct Rune {
					int vnum;
					int probability; // probability out of 1000
				};
				
				// Normal rune probabilitie
				std::vector<Rune> normalRunes = {
					{89547, 500}, // Rune III
				};
								
				// Event rune probabilities (easier to get stage III)
				std::vector<Rune> eventRunes = {
					{89547, 500}, // Rune III
				};
				
				
				const auto isRuneEventActive = CHARACTER_MANAGER::Instance().CheckEventIsActive(SWITCH_RUNE_EVENT, 0, this);
				auto& runes = isRuneEventActive ? eventRunes : normalRunes;
				
				// Function to select a rune based on probability
				auto selectRune = [&runes]() -> int {
					int totalProbability = 0;
					for (const auto& rune : runes) {
						totalProbability += rune.probability;
					}
				
					int randomValue = rand() % totalProbability;
					int cumulativeProbability = 0;
					for (const auto& rune : runes) {
						cumulativeProbability += rune.probability;
						if (randomValue < cumulativeProbability) {
							return rune.vnum;
						}
					}
					return 89500; // Default return in case of error
				};
				
				// Needed so the runes are always different
				srand(time(nullptr));
		
				// Check if the item has exactly 3 runes
				int runeCount = 0;
				for (int i = 0; i < 3; ++i) {
					if (item2->GetSocket(i) != 0) {
						++runeCount;
					}
				}
				
				if (runeCount != 3)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2460]");
					return false;
				}
				else
				{
					// Change all runes in the sockets
					for (int i = 0; i < 3; ++i)
					{
						int selectedRuneVnum = selectRune();
						item2->SetSocket(i, selectedRuneVnum);
					}
				}
				
				item->SetCount(item->GetCount() - 1);
				ChatPacket(CHAT_TYPE_INFO, "[LS;2461]");
			}

			if (item->GetVnum() == 79022)
			{
				LPITEM item2;
				std::set<int> allowedSubTypes = { ARMOR_HEAD, ARMOR_FOOTS, ARMOR_SHIELD };
				
				auto isAllowedSubType = [&allowedSubTypes](int subType) {
					return allowedSubTypes.find(subType) != allowedSubTypes.end();
				};
				
				if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
				{
					return false;
				}
		
				if (item2->IsExchanging() || item2->IsEquipped())
				{
					return false;
				}
				
				if (item2->GetType() != ITEM_ARMOR || !isAllowedSubType(item2->GetSubType()))
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2456]");
					return false;
				}
				
				struct Rune {
					int vnum;
					int probability; // probability out of 1000
				};
				
				// Normal rune probabilitie
				std::vector<Rune> normalRunes = {
					{89517, 500}, // Rune III
				};
								
				// Event rune probabilities (easier to get stage III)
				std::vector<Rune> eventRunes = {
					{89517, 500}, // Rune III
				};
				
				
				const auto isRuneEventActive = CHARACTER_MANAGER::Instance().CheckEventIsActive(SWITCH_RUNE_EVENT, 0, this);
				auto& runes = isRuneEventActive ? eventRunes : normalRunes;
				
				// Function to select a rune based on probability
				auto selectRune = [&runes]() -> int {
					int totalProbability = 0;
					for (const auto& rune : runes) {
						totalProbability += rune.probability;
					}
				
					int randomValue = rand() % totalProbability;
					int cumulativeProbability = 0;
					for (const auto& rune : runes) {
						cumulativeProbability += rune.probability;
						if (randomValue < cumulativeProbability) {
							return rune.vnum;
						}
					}
					return 89500; // Default return in case of error
				};
				
				// Needed so the runes are always different
				srand(time(nullptr));
		
				// Check if the item has exactly 3 runes
				int runeCount = 0;
				for (int i = 0; i < 3; ++i) {
					if (item2->GetSocket(i) != 0) {
						++runeCount;
					}
				}
				
				if (runeCount != 3)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2460]");
					return false;
				}
				else
				{
					// Change all runes in the sockets
					for (int i = 0; i < 3; ++i)
					{
						int selectedRuneVnum = selectRune();
						item2->SetSocket(i, selectedRuneVnum);
					}
				}
				
				item->SetCount(item->GetCount() - 1);
				ChatPacket(CHAT_TYPE_INFO, "[LS;2461]");
			}

			if (item->GetVnum() == 79023)
			{
				LPITEM item2;
				std::set<int> allowedSubTypes = { ARMOR_HEAD, ARMOR_FOOTS, ARMOR_SHIELD };
				
				auto isAllowedSubType = [&allowedSubTypes](int subType) {
					return allowedSubTypes.find(subType) != allowedSubTypes.end();
				};
				
				if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
				{
					return false;
				}
		
				if (item2->IsExchanging() || item2->IsEquipped())
				{
					return false;
				}
				
				if (item2->GetType() != ITEM_ARMOR || !isAllowedSubType(item2->GetSubType()))
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2456]");
					return false;
				}
				
				struct Rune {
					int vnum;
					int probability; // probability out of 1000
				};
				
				// Normal rune probabilitie
				std::vector<Rune> normalRunes = {
					{89550, 500}, // Rune III
				};
								
				// Event rune probabilities (easier to get stage III)
				std::vector<Rune> eventRunes = {
					{89550, 500}, // Rune III
				};
				
				
				const auto isRuneEventActive = CHARACTER_MANAGER::Instance().CheckEventIsActive(SWITCH_RUNE_EVENT, 0, this);
				auto& runes = isRuneEventActive ? eventRunes : normalRunes;
				
				// Function to select a rune based on probability
				auto selectRune = [&runes]() -> int {
					int totalProbability = 0;
					for (const auto& rune : runes) {
						totalProbability += rune.probability;
					}
				
					int randomValue = rand() % totalProbability;
					int cumulativeProbability = 0;
					for (const auto& rune : runes) {
						cumulativeProbability += rune.probability;
						if (randomValue < cumulativeProbability) {
							return rune.vnum;
						}
					}
					return 89500; // Default return in case of error
				};
				
				// Needed so the runes are always different
				srand(time(nullptr));
		
				// Check if the item has exactly 3 runes
				int runeCount = 0;
				for (int i = 0; i < 3; ++i) {
					if (item2->GetSocket(i) != 0) {
						++runeCount;
					}
				}
				
				if (runeCount != 3)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2460]");
					return false;
				}
				else
				{
					// Change all runes in the sockets
					for (int i = 0; i < 3; ++i)
					{
						int selectedRuneVnum = selectRune();
						item2->SetSocket(i, selectedRuneVnum);
					}
				}
				
				item->SetCount(item->GetCount() - 1);
				ChatPacket(CHAT_TYPE_INFO, "[LS;2461]");
			}

			if (item->GetVnum() == 79024)
			{
				LPITEM item2;
				std::set<int> allowedSubTypes = { ARMOR_HEAD, ARMOR_FOOTS, ARMOR_SHIELD };
				
				auto isAllowedSubType = [&allowedSubTypes](int subType) {
					return allowedSubTypes.find(subType) != allowedSubTypes.end();
				};
				
				if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
				{
					return false;
				}
		
				if (item2->IsExchanging() || item2->IsEquipped())
				{
					return false;
				}
				
				if (item2->GetType() != ITEM_ARMOR || !isAllowedSubType(item2->GetSubType()))
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2456]");
					return false;
				}
				
				struct Rune {
					int vnum;
					int probability; // probability out of 1000
				};
				
				// Normal rune probabilitie
				std::vector<Rune> normalRunes = {
					{89526, 500}, // Rune III
				};
								
				// Event rune probabilities (easier to get stage III)
				std::vector<Rune> eventRunes = {
					{89526, 500}, // Rune III
				};
				
				
				const auto isRuneEventActive = CHARACTER_MANAGER::Instance().CheckEventIsActive(SWITCH_RUNE_EVENT, 0, this);
				auto& runes = isRuneEventActive ? eventRunes : normalRunes;
				
				// Function to select a rune based on probability
				auto selectRune = [&runes]() -> int {
					int totalProbability = 0;
					for (const auto& rune : runes) {
						totalProbability += rune.probability;
					}
				
					int randomValue = rand() % totalProbability;
					int cumulativeProbability = 0;
					for (const auto& rune : runes) {
						cumulativeProbability += rune.probability;
						if (randomValue < cumulativeProbability) {
							return rune.vnum;
						}
					}
					return 89500; // Default return in case of error
				};
				
				// Needed so the runes are always different
				srand(time(nullptr));
		
				// Check if the item has exactly 3 runes
				int runeCount = 0;
				for (int i = 0; i < 3; ++i) {
					if (item2->GetSocket(i) != 0) {
						++runeCount;
					}
				}
				
				if (runeCount != 3)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2460]");
					return false;
				}
				else
				{
					// Change all runes in the sockets
					for (int i = 0; i < 3; ++i)
					{
						int selectedRuneVnum = selectRune();
						item2->SetSocket(i, selectedRuneVnum);
					}
				}
				
				item->SetCount(item->GetCount() - 1);
				ChatPacket(CHAT_TYPE_INFO, "[LS;2461]");
			}

			if (item->GetVnum() == 79025)
			{
				LPITEM item2;
				std::set<int> allowedSubTypes = { ARMOR_HEAD, ARMOR_FOOTS, ARMOR_SHIELD };
				
				auto isAllowedSubType = [&allowedSubTypes](int subType) {
					return allowedSubTypes.find(subType) != allowedSubTypes.end();
				};
				
				if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
				{
					return false;
				}
		
				if (item2->IsExchanging() || item2->IsEquipped())
				{
					return false;
				}
				
				if (item2->GetType() != ITEM_ARMOR || !isAllowedSubType(item2->GetSubType()))
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2456]");
					return false;
				}
				
				struct Rune {
					int vnum;
					int probability; // probability out of 1000
				};
				
				// Normal rune probabilitie
				std::vector<Rune> normalRunes = {
					{89514, 500}, // Rune III
				};
								
				// Event rune probabilities (easier to get stage III)
				std::vector<Rune> eventRunes = {
					{89514, 500}, // Rune III
				};
				
				
				const auto isRuneEventActive = CHARACTER_MANAGER::Instance().CheckEventIsActive(SWITCH_RUNE_EVENT, 0, this);
				auto& runes = isRuneEventActive ? eventRunes : normalRunes;
				
				// Function to select a rune based on probability
				auto selectRune = [&runes]() -> int {
					int totalProbability = 0;
					for (const auto& rune : runes) {
						totalProbability += rune.probability;
					}
				
					int randomValue = rand() % totalProbability;
					int cumulativeProbability = 0;
					for (const auto& rune : runes) {
						cumulativeProbability += rune.probability;
						if (randomValue < cumulativeProbability) {
							return rune.vnum;
						}
					}
					return 89500; // Default return in case of error
				};
				
				// Needed so the runes are always different
				srand(time(nullptr));
		
				// Check if the item has exactly 3 runes
				int runeCount = 0;
				for (int i = 0; i < 3; ++i) {
					if (item2->GetSocket(i) != 0) {
						++runeCount;
					}
				}
				
				if (runeCount != 3)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2460]");
					return false;
				}
				else
				{
					// Change all runes in the sockets
					for (int i = 0; i < 3; ++i)
					{
						int selectedRuneVnum = selectRune();
						item2->SetSocket(i, selectedRuneVnum);
					}
				}
				
				item->SetCount(item->GetCount() - 1);
				ChatPacket(CHAT_TYPE_INFO, "[LS;2461]");
			}
		
			if (item->GetVnum() == 79014)
			{
				LPITEM item2;
				if (!(item2 = GetItem(DestCell)))
				{
					ChatPacket(CHAT_TYPE_INFO, "Target item not found at destination cell");
					return false;
				}
				
				// Additional check: make sure the destination is actually dragon soul inventory
				if (DestCell.window_type != DRAGON_SOUL_INVENTORY)
				{
					ChatPacket(CHAT_TYPE_INFO, "Can only be used on items in dragon soul inventory");
					return false;
				}
				if (item2->IsExchanging() == true)
				{
					return false;
				}
				if (item2->IsDragonSoul())
				{
					ChatPacket(CHAT_TYPE_INFO, "Bruh I am changing atttributes in this fucking dragon rune.");
					//item2->ClearAttribute(true);
					ChatPacket(CHAT_TYPE_INFO, "ClearAttribute now");
					DSManager::instance().PutAttributes(item2);
					ChatPacket(CHAT_TYPE_INFO, "PutAttributes now");
					item->SetCount(item->GetCount() - 1);
					ChatPacket(CHAT_TYPE_INFO, "SetCount -1 now");
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, "This item can be used only on alchemy stones");
					return false;
				}
			}
			
#ifdef _ENABLE_BATTLEPASS_
			if (item->GetVnum() == 203012)
			{
				if (!CanHandleItem() || item->IsExchanging())
					return false;

				const auto& cfg = BattlePassManager::Instance().GetConf();

				if (static_cast<uint64_t>(get_global_time()) >= cfg.seasonEnd)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;10063]");
					return false;
				}

				if (GetLevel() < 30)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;10064]");
					return false;
				}

				uint32_t currentPoints = GetQuestFlag("battlepass.points");
				uint32_t currentLevel  = currentPoints / cfg.pointsPerLevel;

				if (currentLevel >= cfg.maxLevel)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;10065]");
					return false;
				}

				uint32_t battlepassUsage = GetQuestFlag("battlepass.boost_usage");
				if (battlepassUsage >= 7)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;10066]");
					return false;
				}

				uint32_t pointsToAdd = std::min(cfg.pointsPerLevel, cfg.maxLevel * cfg.pointsPerLevel - currentPoints);
				uint32_t finalPoints  = currentPoints + pointsToAdd;
				uint32_t finalLevel   = finalPoints / cfg.pointsPerLevel;

				SetQuestFlag("battlepass.points", finalPoints);

				for (uint32_t i = currentLevel; i < finalLevel; ++i)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2325]");
					if (i < cfg.rewardsFree.size())
						AutoGiveItem(cfg.rewardsFree[i].first, cfg.rewardsFree[i].second);
					if (GetQuestFlag("battlepass.isPremium") && i < cfg.rewardsPremium.size())
						AutoGiveItem(cfg.rewardsPremium[i].first, cfg.rewardsPremium[i].second);
				}

				SetQuestFlag("battlepass.boost_usage", battlepassUsage + 1);
				SendBPState(BATTLEPASS_GC_SUB_UPDATE);
				ChatPacket(CHAT_TYPE_INFO, "[LS;10067;%d;%d;%d]", pointsToAdd, finalLevel, battlepassUsage + 1);
				item->SetCount(item->GetCount() - 1);
			}
#endif
			
			if (item->GetVnum() == 203034)
			{
				if (!CanHandleItem() || item->IsExchanging())
				{
					return false;
				}
				
				if (FindAffect(AFFECT_DOUBLE_DROP))
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;99]");
					return false;
				}
				
				int32_t time = item->GetValue(0);
				int32_t bonus = item->GetValue(1);
				int32_t value = item->GetValue(2);
				AddAffect(AFFECT_DOUBLE_DROP, POINT_DOUBLE_LOOT_DROP, value, AFF_NONE, time, 0, true, true);
				item->SetCount(item->GetCount() - 1);
			}
			
			if (item->GetVnum() == 203035)
			{
				if (!CanHandleItem() || item->IsExchanging())
				{
					return false;
				}
				
				SetQuestFlag("fish_wiki.missionc27802", 25);
				SetQuestFlag("fish_wiki.missionc27803", 25);
				SetQuestFlag("fish_wiki.missionc27804", 25);
				SetQuestFlag("fish_wiki.missionc27805", 25);
				SetQuestFlag("fish_wiki.missionc27806", 25);
				SetQuestFlag("fish_wiki.missionc27807", 25);
				SetQuestFlag("fish_wiki.missionc27808", 25);
				SetQuestFlag("fish_wiki.missionc27809", 25);
				SetQuestFlag("fish_wiki.missionc27810", 25);
				SetQuestFlag("fish_wiki.missionc27811", 25);
				SetQuestFlag("fish_wiki.missionc27812", 25);
				SetQuestFlag("fish_wiki.missionc27813", 25);
				SetQuestFlag("fish_wiki.missionc27814", 25);
				SetQuestFlag("fish_wiki.missionc27815", 25);
				SetQuestFlag("fish_wiki.missionc27816", 25);
				SetQuestFlag("fish_wiki.missionc27817", 25);
				SetQuestFlag("fish_wiki.missionc27818", 25);
				SetQuestFlag("fish_wiki.missionc27819", 25);
				SetQuestFlag("fish_wiki.missionc27820", 25);
				SetQuestFlag("fish_wiki.missionc27821", 25);
				SetQuestFlag("fish_wiki.missionc27822", 25);
				SetQuestFlag("fish_wiki.missionc27823", 25);
				SetQuestFlag("fish_wiki.unlocks0", 4194303);
			}

			switch (item->GetVnum())
			{
			case ITEM_NOG_POCKET:
			{
				if (FindAffect(AFFECT_NOG_ABILITY))
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;99]");
					return false;
				}
				int32_t time = item->GetValue(0);
				//int32_t bonus_dungeon = item->GetValue(1);
				int32_t srednia = item->GetValue(2);
				//AddAffect(AFFECT_NOG_ABILITY, POINT_ATTBONUS_DUNGEON, bonus_dungeon, AFF_MOV_SPEED_POTION, time, 0, true, true);
				AddAffect(AFFECT_NOG_ABILITY, POINT_NORMAL_HIT_DAMAGE_BONUS, srednia, AFF_NONE, time, 0, true, true);
				item->SetCount(item->GetCount() - 1);
			}
			break;

			case ITEM_RAMADAN_CANDY:
			{
				// @fixme147 BEGIN
				if (FindAffect(AFFECT_RAMADAN_ABILITY))
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;99]");
					return false;
				}
				// @fixme147 END
				int32_t time = item->GetValue(0);
				int32_t moveSpeedPer = item->GetValue(1);
				int32_t attPer = item->GetValue(2);
				int32_t expPer = item->GetValue(3);
				AddAffect(AFFECT_RAMADAN_ABILITY, POINT_MOV_SPEED, moveSpeedPer, AFF_MOV_SPEED_POTION, time, 0, true, true);
				AddAffect(AFFECT_RAMADAN_ABILITY, POINT_MALL_ATTBONUS, attPer, AFF_NONE, time, 0, true, true);
				AddAffect(AFFECT_RAMADAN_ABILITY, POINT_MALL_EXPBONUS, expPer, AFF_NONE, time, 0, true, true);
				item->SetCount(item->GetCount() - 1);
			}
			break;
			case ITEM_MARRIAGE_RING:
			{
				marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(GetPlayerID());
				if (pMarriage)
				{
					if (pMarriage->ch1 != NULL)
					{
						if (CArenaManager::instance().IsArenaMap(pMarriage->ch1->GetMapIndex()) == true)
						{
							ChatPacket(CHAT_TYPE_INFO, "[LS;1205]");
							break;
						}
					}

					if (pMarriage->ch2 != NULL)
					{
						if (CArenaManager::instance().IsArenaMap(pMarriage->ch2->GetMapIndex()) == true)
						{
							ChatPacket(CHAT_TYPE_INFO, "[LS;1205]");
							break;
						}
					}

					int consumeSP = CalculateConsumeSP(this);

					if (consumeSP < 0)
						return false;

					PointChange(POINT_SP, -consumeSP, false);

					WarpToPID(pMarriage->GetOther(GetPlayerID()));
				}
				else
					ChatPacket(CHAT_TYPE_INFO, "[LS;143]");
			}
			break;

#ifdef ENABLE_VS_PELERYNKA_VIP
			case UNIQUE_ITEM_CAPE_OF_COURAGE:
			case 70057:
			case REWARD_BOX_UNIQUE_ITEM_CAPE_OF_COURAGE:
				AggregateMonster(0);
				break;
				
			case 70069:
				AggregateMonster(1);
				break;
#else
			case UNIQUE_ITEM_CAPE_OF_COURAGE:
			case 70057:
			case REWARD_BOX_UNIQUE_ITEM_CAPE_OF_COURAGE:
				AggregateMonster();
				// item->SetCount(item->GetCount()-1);
				break;
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			case ACCE_REVERSAL_VNUM_1:
			case ACCE_REVERSAL_VNUM_2:
			{
				LPITEM item2;
				if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
					return false;

				if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
					return false;

				if (!CleanAcceAttr(item, item2))
					return false;
				item->SetCount(item->GetCount()-1);
				break;
			}
			break;
#endif

#ifdef ENABLE_VIP_ITEMS
		case VIP_PD_1_DAY:
		case VIP_PD_3_DAY:
		case VIP_PD_7_DAY:
		case VIP_PD_14_DAY:
		case VIP_PD_30_DAY:
			if (GetPremiumRemainSeconds(PREMIUM_EXP) > 0)
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;2220]");
				return false;
			}
			else
			{
				if (item->GetValue(0) > 0) {
					ITEM_MANAGER::instance().RemoveItem(item);
					std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("UPDATE srv1_account.account SET vip_pd_expire = DATE_ADD(NOW(), INTERVAL '%d' DAY) WHERE id = '%d';", item->GetValue(0), GetDesc()->GetAccountTable().id));

					ChatPacket(CHAT_TYPE_INFO, "[LS;2221]");
				} else
					ChatPacket(CHAT_TYPE_INFO, "[LS;2222]");
			}
			break;

		case VIP_GLOVE_1_DAY:
		case VIP_GLOVE_3_DAY:
		case VIP_GLOVE_7_DAY:
		case VIP_GLOVE_14_DAY:
		case VIP_GLOVE_30_DAY:
			if (GetPremiumRemainSeconds(PREMIUM_ITEM) > 0)
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;2223]");
				return false;
			}
			else
			{
				if (item->GetValue(0) > 0) {
					ITEM_MANAGER::instance().RemoveItem(item);
					std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("UPDATE srv1_account.account SET vip_glove_expire = DATE_ADD(NOW(), INTERVAL '%d' DAY) WHERE id = '%d';", item->GetValue(0), GetDesc()->GetAccountTable().id));

					ChatPacket(CHAT_TYPE_INFO, "[LS;2224]");
				} else
					ChatPacket(CHAT_TYPE_INFO, "[LS;2225]");
			}
			break;

		case VIP_AUTOPODNOSZENIE_1_DAY:
		case VIP_AUTOPODNOSZENIE_3_DAY:
		case VIP_AUTOPODNOSZENIE_7_DAY:
		case VIP_AUTOPODNOSZENIE_14_DAY:
		case VIP_AUTOPODNOSZENIE_30_DAY:
			if (item->GetValue(0) > 0) {
				long days = item->GetValue(0);
				ITEM_MANAGER::instance().RemoveItem(item);
				std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("UPDATE srv1_account.account SET pickup_expire = DATE_ADD(GREATEST(IFNULL(pickup_expire, NOW()), NOW()), INTERVAL '%d' DAY) WHERE id = '%d';", days, GetDesc()->GetAccountTable().id));
				m_aiPremiumTimes[PREMIUM_PICKUP] = std::max(m_aiPremiumTimes[PREMIUM_PICKUP], (int)get_global_time()) + days * 86400L;
				ChatPacket(CHAT_TYPE_INFO, "[LS;2227]");
			} else
				ChatPacket(CHAT_TYPE_INFO, "[LS;2228]");
			break;
#endif


			case UNIQUE_ITEM_WHITE_FLAG:
				ForgetMyAttacker();
				item->SetCount(item->GetCount() - 1);
				break;

			case UNIQUE_ITEM_TREASURE_BOX:
				break;

			case 30093:
			case 30094:
			case 30095:
			case 30096:

			{
				const int MAX_BAG_INFO = 26;
				static struct LuckyBagInfo
				{
					DWORD count;
					int prob;
					DWORD vnum;
				} b1[MAX_BAG_INFO] =
				{
					{ 1000,	302,	1 },
					{ 10,	150,	27002 },
					{ 10,	75,	27003 },
					{ 10,	100,	27005 },
					{ 10,	50,	27006 },
					{ 10,	80,	27001 },
					{ 10,	50,	27002 },
					{ 10,	80,	27004 },
					{ 10,	50,	27005 },
					{ 1,	10,	50300 },
					{ 1,	6,	92 },
					{ 1,	2,	132 },
					{ 1,	6,	1052 },
					{ 1,	2,	1092 },
					{ 1,	6,	2082 },
					{ 1,	2,	2122 },
					{ 1,	6,	3082 },
					{ 1,	2,	3122 },
					{ 1,	6,	5052 },
					{ 1,	2,	5082 },
					{ 1,	6,	7082 },
					{ 1,	2,	7122 },
					{ 1,	1,	11282 },
					{ 1,	1,	11482 },
					{ 1,	1,	11682 },
					{ 1,	1,	11882 },
				};

				LuckyBagInfo* bi = NULL;
				bi = b1;

				int pct = number(1, 1000);

				int i;
				for (i = 0; i < MAX_BAG_INFO; i++)
				{
					if (pct <= bi[i].prob)
						break;
					pct -= bi[i].prob;
				}
				if (i >= MAX_BAG_INFO)
					return false;

				if (bi[i].vnum == 50300)
				{
					GiveRandomSkillBook();
				}
				else if (bi[i].vnum == 1)
				{
					PointChange(POINT_GOLD, 1000, true);
				}
				else
				{
					AutoGiveItem(bi[i].vnum, bi[i].count);
				}
				ITEM_MANAGER::instance().RemoveItem(item);
			}
			break;

			case 50004:
			{
				if (item->GetSocket(0))
				{
					item->SetSocket(0, item->GetSocket(0) + 1);
				}
				else
				{
					int iMapIndex = GetMapIndex();

					PIXEL_POSITION pos;

					if (SECTREE_MANAGER::instance().GetRandomLocation(iMapIndex, pos, 700))
					{
						item->SetSocket(0, 1);
						item->SetSocket(1, pos.x);
						item->SetSocket(2, pos.y);
					}
					else
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;154]");
						return false;
					}
				}

				int dist = 0;
				float distance = (DISTANCE_SQRT(GetX() - item->GetSocket(1), GetY() - item->GetSocket(2)));

				if (distance < 1000.0f)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;165]");

					struct TEventStoneInfo
					{
						DWORD dwVnum;
						int count;
						int prob;
					};
					const int EVENT_STONE_MAX_INFO = 15;
					TEventStoneInfo info_10[EVENT_STONE_MAX_INFO] =
					{
						{ 27001, 10,  8 },
						{ 27004, 10,  6 },
						{ 27002, 10, 12 },
						{ 27005, 10, 12 },
						{ 27100,  1,  9 },
						{ 27103,  1,  9 },
						{ 27101,  1, 10 },
						{ 27104,  1, 10 },
						{ 27999,  1, 12 },

						{ 25040,  1,  4 },

						{ 27410,  1,  0 },
						{ 27600,  1,  0 },
						{ 25100,  1,  0 },

						{ 50001,  1,  0 },
						{ 50003,  1,  1 },
					};
					TEventStoneInfo info_7[EVENT_STONE_MAX_INFO] =
					{
						{ 27001, 10,  1 },
						{ 27004, 10,  1 },
						{ 27004, 10,  9 },
						{ 27005, 10,  9 },
						{ 27100,  1,  5 },
						{ 27103,  1,  5 },
						{ 27101,  1, 10 },
						{ 27104,  1, 10 },
						{ 27999,  1, 14 },

						{ 25040,  1,  5 },

						{ 27410,  1,  5 },
						{ 27600,  1,  5 },
						{ 25100,  1,  5 },

						{ 50001,  1,  0 },
						{ 50003,  1,  5 },

					};
					TEventStoneInfo info_4[EVENT_STONE_MAX_INFO] =
					{
						{ 27001, 10,  0 },
						{ 27004, 10,  0 },
						{ 27002, 10,  0 },
						{ 27005, 10,  0 },
						{ 27100,  1,  0 },
						{ 27103,  1,  0 },
						{ 27101,  1,  0 },
						{ 27104,  1,  0 },
						{ 27999,  1, 25 },

						{ 25040,  1,  0 },

						{ 27410,  1,  0 },
						{ 27600,  1,  0 },
						{ 25100,  1, 15 },

						{ 50001,  1, 10 },
						{ 50003,  1, 50 },

					};

					{
						TEventStoneInfo* info;
						if (item->GetSocket(0) <= 4)
							info = info_4;
						else if (item->GetSocket(0) <= 7)
							info = info_7;
						else
							info = info_10;

						int prob = number(1, 100);

						for (int i = 0; i < EVENT_STONE_MAX_INFO; ++i)
						{
							if (!info[i].prob)
								continue;

							if (prob <= info[i].prob)
							{
								if (info[i].dwVnum == 50001)
								{
									DWORD* pdw = M2_NEW DWORD[2];

									pdw[0] = info[i].dwVnum;
									pdw[1] = info[i].count;

									DBManager::instance().ReturnQuery(QID_LOTTO, GetPlayerID(), pdw,
										"INSERT INTO lotto_list VALUES(0, 'server%s', %u, NOW())",
										get_table_postfix(), GetPlayerID());
								}
								else
									AutoGiveItem(info[i].dwVnum, info[i].count);

								break;
							}
							prob -= info[i].prob;
						}
					}

					char chatbuf[CHAT_MAX_LEN + 1];
					int len = snprintf(chatbuf, sizeof(chatbuf), "StoneDetect %u 0 0", (DWORD)GetVID());

					if (len < 0 || len >= (int)sizeof(chatbuf))
						len = sizeof(chatbuf) - 1;

					++len;

					TPacketGCChat pack_chat;
					pack_chat.header = HEADER_GC_CHAT;
					pack_chat.size = sizeof(TPacketGCChat) + len;
					pack_chat.type = CHAT_TYPE_COMMAND;
					pack_chat.id = 0;
					pack_chat.bEmpire = GetDesc()->GetEmpire();
					//pack_chat.id	= vid;

					TEMP_BUFFER buf;
					buf.write(&pack_chat, sizeof(TPacketGCChat));
					buf.write(chatbuf, len);

					PacketAround(buf.read_peek(), buf.size());

					ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (DETECT_EVENT_STONE) 1");
					return true;
				}
				else if (distance < 20000)
					dist = 1;
				else if (distance < 70000)
					dist = 2;
				else
					dist = 3;

				const int STONE_DETECT_MAX_TRY = 10;
				if (item->GetSocket(0) >= STONE_DETECT_MAX_TRY)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;176]");
					ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (DETECT_EVENT_STONE) 0");
					AutoGiveItem(27002);
					return true;
				}

				if (dist)
				{
					char chatbuf[CHAT_MAX_LEN + 1];
					int len = snprintf(chatbuf, sizeof(chatbuf),
						"StoneDetect %u %d %d",
						(DWORD)GetVID(), dist, (int)GetDegreeFromPositionXY(GetX(), item->GetSocket(2), item->GetSocket(1), GetY()));

					if (len < 0 || len >= (int)sizeof(chatbuf))
						len = sizeof(chatbuf) - 1;

					++len;

					TPacketGCChat pack_chat;
					pack_chat.header = HEADER_GC_CHAT;
					pack_chat.size = sizeof(TPacketGCChat) + len;
					pack_chat.type = CHAT_TYPE_COMMAND;
					pack_chat.id = 0;
					pack_chat.bEmpire = GetDesc()->GetEmpire();
					//pack_chat.id		= vid;

					TEMP_BUFFER buf;
					buf.write(&pack_chat, sizeof(TPacketGCChat));
					buf.write(chatbuf, len);

					PacketAround(buf.read_peek(), buf.size());
				}

			}
			break;

			case 27989:
			case 76006:
			{
				LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());

				if (pMap != NULL)
				{
					item->SetSocket(0, item->GetSocket(0) + 1);

					FFindStone f;

					// <Factor> SECTREE::for_each -> SECTREE::for_each_entity
					pMap->for_each(f);

					if (f.m_mapStone.size() > 0)
					{
						std::map<DWORD, LPCHARACTER>::iterator stone = f.m_mapStone.begin();

						DWORD max = UINT_MAX;
						LPCHARACTER pTarget = stone->second;

						while (stone != f.m_mapStone.end())
						{
							DWORD dist = (DWORD)DISTANCE_SQRT(GetX() - stone->second->GetX(), GetY() - stone->second->GetY());

							if (dist != 0 && max > dist)
							{
								max = dist;
								pTarget = stone->second;
							}
							stone++;
						}

						if (pTarget != NULL)
						{
							int val = 3;

							if (max < 10000) val = 2;
							else if (max < 70000) val = 1;

							ChatPacket(CHAT_TYPE_COMMAND, "StoneDetect %u %d %d", (DWORD)GetVID(), val,
								(int)GetDegreeFromPositionXY(GetX(), pTarget->GetY(), pTarget->GetX(), GetY()));
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, "[LS;1071]");
						}
					}
					else
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;1071]");
					}

					if (item->GetSocket(0) >= 6)
					{
						ChatPacket(CHAT_TYPE_COMMAND, "StoneDetect %u 0 0", (DWORD)GetVID());
						ITEM_MANAGER::instance().RemoveItem(item);
					}
				}
				break;
			}
			break;

			case 27996:
				item->SetCount(item->GetCount() - 1);
				AttackedByPoison(NULL); // @warme008
				break;

			case 27987:

			{
				item->SetCount(item->GetCount() - 1);

				int r = number(1, 100);

				if (r <= 50)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;221]");
					AutoGiveItem(27990);
				}
				else
				{
					const int prob_table_gb2312[] =
					{
						95, 97, 99
					};

					const int* prob_table = prob_table_gb2312;

					if (r <= prob_table[0])
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;232]");
					}
					else if (r <= prob_table[1])
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;243]");
						AutoGiveItem(27992);
					}
					else if (r <= prob_table[2])
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;253]");
						AutoGiveItem(27993);
					}
					else
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;264]");
						AutoGiveItem(27994);
					}
				}
			}
			break;

			case 71013:
				CreateFly(number(FLY_FIREWORK1, FLY_FIREWORK6), this);
				item->SetCount(item->GetCount() - 1);
				break;

			case 50100:
			case 50101:
			case 50102:
			case 50103:
			case 50104:
			case 50105:
			case 50106:
				CreateFly(item->GetVnum() - 50100 + FLY_FIREWORK1, this);
				item->SetCount(item->GetCount() - 1);
				break;

			case 50200:
			{
				if (IS_BOTARYABLE_ZONE(GetMapIndex()) == true)
				{
#ifdef __PREMIUM_PRIVATE_SHOP__
					if (IsPrivateShopOwner())
					{
						if (GetPrivateShopTable()->llGold > 0 || GetPrivateShopTable()->dwCheque > 0)
						{
							OpenPrivateShopPanel();
							return true;
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, "[LS;2229]");
							return false;
						}
					}

					OpenPrivateShopPanel();
#else
					__OpenPrivateShop();
#endif
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2230]");
				}
			}
			break;

#ifdef __PREMIUM_PRIVATE_SHOP__
			case 71221:
			{
				if (IsPrivateShopOwner())
				{
					if (GetPrivateShopTable()->llGold > 0 || GetPrivateShopTable()->dwCheque > 0)
					{
						OpenPrivateShopPanel();
						return true;
					}
					else
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;2229]");
						return false;
					}
				}

				OpenPrivateShopPanel();
				ChatPacket(CHAT_TYPE_COMMAND, "SetPrivateShopPremiumBuild");
			} break;

			case 60004:
			{
				OpenShopSearch(MODE_LOOKING);
			} break;

			case 60005:
			case 60006:
			case 60007:
			{
				OpenShopSearch(MODE_TRADING);
			} break;
#endif

			case fishing::FISH_MIND_PILL_VNUM:
				AddAffect(AFFECT_FISH_MIND_PILL, POINT_NONE, 0, AFF_FISH_MIND, 20 * 60, 0, true);
				item->SetCount(item->GetCount() - 1);
				break;

			case 50301:
			case 50302:
			case 50303:
			{
				if (IsPolymorphed() == true)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;521]");
					return false;
				}

				int lv = GetSkillLevel(SKILL_LEADERSHIP);

				if (lv < item->GetValue(0))
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;274]");
					return false;
				}

				if (lv >= item->GetValue(1))
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;284]");
					return false;
				}

				if (LearnSkillByBook(SKILL_LEADERSHIP))
				{
#ifdef ENABLE_BOOKS_STACKFIX
					item->SetCount(item->GetCount() - 1);
#else
					ITEM_MANAGER::instance().RemoveItem(item);
#endif

					int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
					if (distribution_test_server) iReadDelay /= 3;

					SetSkillNextReadTime(SKILL_LEADERSHIP, get_global_time() + iReadDelay);
				}
			}
			break;

			case 50304:
			case 50305:
			case 50306:
			{
				if (IsPolymorphed())
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;1041]");
					return false;

				}
				if (GetSkillLevel(SKILL_COMBO) == 0 && GetLevel() < 30)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;295]");
					return false;
				}

				if (GetSkillLevel(SKILL_COMBO) == 1 && GetLevel() < 50)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;305]");
					return false;
				}

				if (GetSkillLevel(SKILL_COMBO) >= 2)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;316]");
					return false;
				}

				int iPct = item->GetValue(0);

				if (LearnSkillByBook(SKILL_COMBO, iPct))
				{
#ifdef ENABLE_BOOKS_STACKFIX
					item->SetCount(item->GetCount() - 1);
#else
					ITEM_MANAGER::instance().RemoveItem(item);
#endif

					int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
					if (distribution_test_server) iReadDelay /= 3;

					SetSkillNextReadTime(SKILL_COMBO, get_global_time() + iReadDelay);
				}
			}
			break;
			case 50311:
			case 50312:
			case 50313:
			{
				if (IsPolymorphed())
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;1041]");
					return false;

				}
				DWORD dwSkillVnum = item->GetValue(0);
				int iPct = MINMAX(0, item->GetValue(1), 100);
				if (GetSkillLevel(dwSkillVnum) >= 20 || dwSkillVnum - SKILL_LANGUAGE1 + 1 == GetEmpire())
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;328]");
					return false;
				}

				if (LearnSkillByBook(dwSkillVnum, iPct))
				{
#ifdef ENABLE_BOOKS_STACKFIX
					item->SetCount(item->GetCount() - 1);
#else
					ITEM_MANAGER::instance().RemoveItem(item);
#endif

					int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
					if (distribution_test_server) iReadDelay /= 3;

					SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
				}
			}
			break;

			case 50061:
			{
				if (IsPolymorphed())
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;1041]");
					return false;

				}
				DWORD dwSkillVnum = item->GetValue(0);
				int iPct = MINMAX(0, item->GetValue(1), 100);

				if (GetSkillLevel(dwSkillVnum) >= 10)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;948]");
					return false;
				}

				if (LearnSkillByBook(dwSkillVnum, iPct))
				{
#ifdef ENABLE_BOOKS_STACKFIX
					item->SetCount(item->GetCount() - 1);
#else
					ITEM_MANAGER::instance().RemoveItem(item);
#endif

					int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
					if (distribution_test_server) iReadDelay /= 3;

					SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
				}
			}
			break;

			case 50495:
			{
				if (IsPolymorphed())
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;1041]");
					return false;
				}
				
				int iSkillID = GetQuestFlag("buff.trainskill_vnum");
				if (iSkillID < 0 && iSkillID > 2) {
					ChatPacket(CHAT_TYPE_INFO, "[LS;2231]");
					return false;
				}

				int iSkillLevel = GetBuffNPCSystem()->GetSkillLevel(iSkillID);
				int iSkillGrade = GetBuffNPCSystem()->GetSkillGrade(iSkillLevel);

				if (iSkillID == -1 || iSkillGrade == -1 || iSkillGrade != 2)
					return false;

				char flag_next_read[128+1];
				memset(flag_next_read, 0, sizeof(flag_next_read));
				snprintf(flag_next_read, sizeof(flag_next_read), "buff_npc_learn2.%u.next_read", iSkillID);

				int iNextReadLastTime = GetQuestFlag(flag_next_read);
				if(get_global_time() < iNextReadLastTime)
				{
					if (FindAffect(AFFECT_SKILE_BUFF_NO_BOOK_DELAY))
					{
						RemoveAffect(AFFECT_SKILE_BUFF_NO_BOOK_DELAY);
						ChatPacket(CHAT_TYPE_INFO, "[LS;377]");
					}
					else 
					{
						int sec_new = iNextReadLastTime-get_global_time();
						ConvertTime(sec_new);
						return false;
					}
				}

				int percent = item->GetValue(5);

				if (FindAffect(AFFECT_SKILE_BUFF_BOOK_BONUS))
					RemoveAffect(AFFECT_SKILE_BUFF_BOOK_BONUS);
					percent += 20;

				item->SetCount(item->GetCount() - 1);
				if(number(1, 100) < percent)
				{
					DWORD nextTime = get_global_time() + item->GetValue(4);
					SetQuestFlag(flag_next_read, nextTime);

					// m_dwSkill[iSkillID] = this->m_dwSkill[iSkillID] + 1;
					GetBuffNPCSystem()->SetSkill(iSkillID, iSkillLevel + 1);

					ChatPacket(CHAT_TYPE_INFO, "[LS;2232]");
						
					ChatPacket(CHAT_TYPE_COMMAND, "BuffNPCSkillInfo %d %d %d", 
					GetBuffNPCSystem()->GetSkillLevel(0), 
					GetBuffNPCSystem()->GetSkillLevel(1), 
					GetBuffNPCSystem()->GetSkillLevel(2)
					);

					if (test_server) {
						sys_log(0, "TrainBuffSkill_50495: percent: %d, skillVnum: %d iSkillGrade: %d", percent, iSkillID, iSkillGrade);
					}
				}
				else {
					ChatPacket(CHAT_TYPE_INFO, "[LS;2233]");
					return false;
				}
			}
			break;

			case 50511:
			case 50494:
			case 50496:
			{
				if (IsPolymorphed())
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;1041]");
					return false;
				}
				if (!GetBuffNPCSystem()->IsSummoned())
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;10057]");
					return false;
				}

				int iSkillID = item->GetValue(0);
				int iSkillLevel = GetBuffNPCSystem()->GetSkillLevel(item->GetValue(1));
				int iFakeSkillID = item->GetValue(1);
				int iSkillGrade = GetBuffNPCSystem()->GetSkillGrade(iSkillLevel);

				if(iSkillID == -1 || iSkillGrade == -1)
					return false;
					
				if(iSkillGrade != 1) {
					if (iSkillGrade > 1)
						ChatPacket(CHAT_TYPE_INFO, "[LS;959]");
					else
						ChatPacket(CHAT_TYPE_INFO, "[LS;960]");
					return false;
				}

				char flag_next_read[128+1];
				memset(flag_next_read, 0, sizeof(flag_next_read));
				snprintf(flag_next_read, sizeof(flag_next_read), "buff_npc_learn.%u.next_read", iSkillID);
				char flag_read_count[128+1];
				memset(flag_read_count, 0, sizeof(flag_read_count));
				snprintf(flag_read_count, sizeof(flag_read_count), "buff_npc_learn.%u.read_count", iSkillID);

				int iNextReadLastTime = GetQuestFlag(flag_next_read);
				if(get_global_time() < iNextReadLastTime)
				{
					if (FindAffect(AFFECT_SKILE_BUFF_NO_BOOK_DELAY))
					{
						RemoveAffect(AFFECT_SKILE_BUFF_NO_BOOK_DELAY);
						ChatPacket(CHAT_TYPE_INFO, "[LS;377]");
					}
					else 
					{
						int sec_new = iNextReadLastTime-get_global_time();
						ConvertTime(sec_new);
						return false;
					}
				}

				int iReadCount = GetQuestFlag(flag_read_count);
					
				int need_bookcount = 1;
				int percent = item->GetValue(5);

				if (FindAffect(AFFECT_SKILE_BUFF_BOOK_BONUS))
					RemoveAffect(AFFECT_SKILE_BUFF_BOOK_BONUS);
					percent += 30;

				item->SetCount(item->GetCount() - 1);
				if(number(1, 100) < percent)
				{
					
					DWORD nextTime = get_global_time() + item->GetValue(4);
					SetQuestFlag(flag_next_read, nextTime);

					if (iReadCount >= need_bookcount)
					{
						// m_dwSkill[iSkillID] = this->m_dwSkill[iSkillID] + 1;
						GetBuffNPCSystem()->SetSkill(iFakeSkillID, iSkillLevel + 1);
						SetQuestFlag(flag_read_count, 0);

						ChatPacket(CHAT_TYPE_INFO, "[LS;964]");
						
						ChatPacket(CHAT_TYPE_COMMAND, "BuffNPCSkillInfo %d %d %d", 
						GetBuffNPCSystem()->GetSkillLevel(0), 
						GetBuffNPCSystem()->GetSkillLevel(1), 
						GetBuffNPCSystem()->GetSkillLevel(2)
						);

						if (test_server) {
							sys_log(0, "TrainBuffSkill: percent: %d, skillVnum: %d, iSkillGrade: %d", percent, iSkillID, iSkillGrade);
						}
					}
					else
					{
						SetQuestFlag(flag_read_count, iReadCount + 1);

						switch (number(1, 3))
						{
							case 1:
								ChatPacket(CHAT_TYPE_TALKING, "[LS;961]");
								break;
											
							case 2:
								ChatPacket(CHAT_TYPE_TALKING, "[LS;962]");
								break;

							case 3:
							default:
								ChatPacket(CHAT_TYPE_TALKING, "[LS;963]");
								break;
						}

						ChatPacket(CHAT_TYPE_INFO, "[LS;995;%d]", need_bookcount - iReadCount);
						break;
					}
				} else {
					ChatPacket(CHAT_TYPE_INFO, "[LS;2233]");
					return false;
				}
			}
			break;


			case 50314: case 50315: case 50316:
			case 50323: case 50324:
			case 50325: case 50326:
			{
				if (IsPolymorphed() == true)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;1041]");
					return false;
				}

				int iSkillLevelLowLimit = item->GetValue(0);
				int iSkillLevelHighLimit = item->GetValue(1);
				int iPct = MINMAX(0, item->GetValue(2), 100);
				int iLevelLimit = item->GetValue(3);
				DWORD dwSkillVnum = 0;

				switch (item->GetVnum())
				{
				case 50314: case 50315: case 50316:
					dwSkillVnum = SKILL_POLYMORPH;
					break;

				case 50323: case 50324:
					dwSkillVnum = SKILL_ADD_HP;
					break;

				case 50325: case 50326:
					dwSkillVnum = SKILL_RESIST_PENETRATE;
					break;
					


				default:
					return false;
				}

				if (0 == dwSkillVnum)
					return false;

				if (GetLevel() < iLevelLimit)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2234]");
					return false;
				}

				if (GetSkillLevel(dwSkillVnum) >= 40)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2235]");
					return false;
				}

				if (GetSkillLevel(dwSkillVnum) < iSkillLevelLowLimit)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2236]");
					return false;
				}

				if (GetSkillLevel(dwSkillVnum) >= iSkillLevelHighLimit)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2237]");
					return false;
				}

				if (LearnSkillByBook(dwSkillVnum, iPct))
				{
#ifdef ENABLE_BOOKS_STACKFIX
					item->SetCount(item->GetCount() - 1);
#else
					ITEM_MANAGER::instance().RemoveItem(item);
#endif

					int iReadDelay = item->GetValue(4);
					SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
				}
			}
			break;

#ifdef ENABLE_NEW_PET_SYSTEM_BOOK
			case 90501:
			case 90502:
			case 90503:
			case 90504:
			case 90505:
			case 90506:
			case 90507:
			case 90508:
			case 90509:
			case 90510:
			case 90511:
			case 90512:
			case 90513:
			case 90514:
			case 90515:
			{
				// Dodanie wspomagania dla peta
				auto bSkillGrade = item->GetValue(2);
				auto rBookConfig = item->GetValue(0);
				
				if (NewPetSystemHelper::GetSkillLevel(this, rBookConfig) >= NewPetSystemHelper::PET_MAX_SKILL_LV)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2238]");
					return false;
				}

				if (NewPetSystemHelper::GetSkillGrade(NewPetSystemHelper::GetSkillLevel(this, rBookConfig)) != bSkillGrade)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2239]");
					return false;
				}
				if (NewPetSystemHelper::GetSkillDelay(this, rBookConfig, NewPetSystemHelper::GetSkillLevel(this, rBookConfig)) > get_global_time())
				{
					if (FindAffect(AFFECT_SKILE_PET_NO_BOOK_DELAY))
					{
						RemoveAffect(AFFECT_SKILE_PET_NO_BOOK_DELAY);
					}
					else
					{
						int sec_new = (NewPetSystemHelper::GetSkillDelay(this, rBookConfig, NewPetSystemHelper::GetSkillLevel(this, rBookConfig))-get_global_time());
						ConvertTime(sec_new);
						return false; // nie dopuszczam do ponownego uzycia
						if (test_server)
						{
							ChatPacket(CHAT_TYPE_INFO, "<Test Server> Hm cos tu nie gra, jak to widzisz to nie usunal sie Affect czytania ksiegi.");
						}
						else
						{
							return false; // nie dopuszczam do ponownego uzycia
						}
					}
				}
				BYTE bChance = item->GetValue(1);
				if (FindAffect(AFFECT_SKILE_PET_BOOK_BONUS))
				{
					bChance += 40;
					RemoveAffect(AFFECT_SKILE_PET_BOOK_BONUS);
				}

				item->SetCount(item->GetCount() - 1);

				time_t ttDelay = item->GetValue(3);

				if (bChance >= number(1, 100))
				{
					int iRes = NewPetSystemHelper::TrainSkill(this, rBookConfig);
					NewPetSystemHelper::SetSkillDelay(this, rBookConfig, NewPetSystemHelper::GetSkillLevel(this, rBookConfig), ttDelay);

					ChatPacket(CHAT_TYPE_INFO, "[LS;2240]");

					if (test_server) {
						sys_log(0, "TrainPetSkill: bChance: %d, skillVnum: %d", bChance, rBookConfig);
					}

					if (iRes > 0)
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;2241] %d", iRes);
					}
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2242]");
				}

				return true;
			}
			case 90500:
			{
				// Dodanie wspomagania dla peta
				auto bSkillGrade = item->GetValue(2);
				auto vnum = GetQuestFlag("pet.trainskill_vnum");
				
				if (vnum == 0) {
					ChatPacket(CHAT_TYPE_INFO, "[LS;2243]");
					return false;
				}

				if (NewPetSystemHelper::GetSkillLevel(this, vnum) >= NewPetSystemHelper::PET_MAX_SKILL_LV)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2238]");
					return false;
				}
				if (NewPetSystemHelper::GetSkillGrade(NewPetSystemHelper::GetSkillLevel(this, vnum)) != bSkillGrade)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2239]");
					return false;
				}
				if (NewPetSystemHelper::GetSkillDelay(this, vnum, NewPetSystemHelper::GetSkillLevel(this, vnum)) > get_global_time())
				{
					if (FindAffect(AFFECT_SKILE_PET_NO_BOOK_DELAY))
					{
						RemoveAffect(AFFECT_SKILE_PET_NO_BOOK_DELAY);
					}
					else
					{
						int sec_new = (NewPetSystemHelper::GetSkillDelay(this, vnum, NewPetSystemHelper::GetSkillLevel(this, vnum))-get_global_time());
						ConvertTime(sec_new);
						return false; // nie dopuszczam do ponownego uzycia
						if (test_server)
						{
							ChatPacket(CHAT_TYPE_INFO, "<Test Server> Hm cos tu nie gra, jak to widzisz to nie usunal sie Affect czytania ksiegi.");
						}
						else
						{
							return false; // nie dopuszczam do ponownego uzycia
						}
					}
				}
				BYTE bChance = item->GetValue(1);
				if (FindAffect(AFFECT_SKILE_PET_BOOK_BONUS))
				{
					bChance += 20;
					RemoveAffect(AFFECT_SKILE_PET_BOOK_BONUS);
				}

				item->SetCount(item->GetCount() - 1);

				time_t ttDelay = item->GetValue(3);

				if (bChance >= number(1, 100))
				{
					int iRes = NewPetSystemHelper::TrainSkill(this, vnum);
					NewPetSystemHelper::SetSkillDelay(this, vnum, NewPetSystemHelper::GetSkillLevel(this, vnum), ttDelay);

					ChatPacket(CHAT_TYPE_INFO, "[LS;2240]");

					if (test_server) {
						sys_log(0, "TrainPetSkill - 90500: bChance: %d, skillVnum: %d", bChance, vnum);
					}

					if (iRes > 0)
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;2241;%d]", iRes);
					}
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2242]");
				}

				return true;
			}
#endif

#ifdef ENABLE_SKILE_PASYWNE
			case 50335: case 50336: //SKILL_HUMAN_BONUS
			case 50337: case 50338: //SKILL_MONSTER_BONUS
			case 50339: case 50340: //SKILL_STONE_BONUS
			case 50341: case 50342: //SKILL_DUNG_BONUS
			case 50343: case 50344: //SKILL_BOSS_BONUS
			case 50345: case 50346: //SKILL_EXP_BONUS
			case 50347: case 50348: //SKILL_ODP_UM
			case 50349: case 50350: //SKILL_SREDNIE_BONUS
			case 50351: case 50352: //SKILL_STAT_BONUS
			case 50353: case 50354: //SKILL_HP_BONUS
			case 50355: case 50356: //SKILL_DO_PZ_BONUS
			case 50357: case 50358:
			case 50359: case 50360:
			case 50361: case 50362:
			case 50363: case 50364:
			case 50365: case 50366: 
			case 50367:
			{
				if (IsPolymorphed() == true)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;1041]");
					return false;
				}

				int iSkillLevelLowLimit = item->GetValue(0);
				int iSkillLevelHighLimit = item->GetValue(1);
				int iPct = MINMAX(0, item->GetValue(2), 100);
				int iLevelLimit = item->GetValue(3);
				DWORD dwSkillVnum = 0;

				switch (item->GetVnum())
				{
					case 50335: case 50336: case 50337:  //SKILL_HUMAN_BONUS
						dwSkillVnum = SKILL_HUMAN_BONUS;
							break;
					case 50338: case 50339: case 50340: //SKILL_MONSTER_BONUS
						dwSkillVnum = SKILL_MONSTER_BONUS;
							break;
					case 50341: case 50342: case 50343: //SKILL_STONE_BONUS
						dwSkillVnum = SKILL_STONE_BONUS;
							break;
					case 50344: case 50345: case 50346: //SKILL_DUNG_BONUS
						dwSkillVnum = SKILL_DUNG_BONUS;
							break;
					case 50347: case 50348: case 50349://SKILL_BOSS_BONUS
						dwSkillVnum = SKILL_BOSS_BONUS;
							break;
					case 50350: case 50351: case 50352: //SKILL_EXP_BONUS
						dwSkillVnum = SKILL_EXP_BONUS;
							break;
					case 50353: case 50354: case 50355: //SKILL_ODP_UM
						dwSkillVnum = SKILL_ODP_UM;
							break;
					case 50356: case 50357: case 50358: //SKILL_SREDNIE_BONUS
						dwSkillVnum = SKILL_SREDNIE_BONUS;
							break;
					case 50359: case 50360: case 50361: //SKILL_STAT_BONUS
						dwSkillVnum = SKILL_STAT_BONUS;
							break;
					case 50362: case 50363: case 50364: //SKILL_HP_BONUS
						dwSkillVnum = SKILL_HP_BONUS;
							break;
					case 50365: case 50366: case 50367: //SKILL_DO_PZ_BONUS
						dwSkillVnum = SKILL_DO_PZ_BONUS;
							break;

					default:
						return false;
				}

				if (0 == dwSkillVnum)
					return false;

				if (GetLevel() < iLevelLimit)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2234]");
					return false;
				}

				if (GetSkillLevel(dwSkillVnum) >= 40)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2235]");
					return false;
				}

				if (GetSkillLevel(dwSkillVnum) < iSkillLevelLowLimit)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2236]");
					return false;
				}

				if (GetSkillLevel(dwSkillVnum) >= iSkillLevelHighLimit)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2237]");
					return false;
				}
				
				if (LearnSkillByNewBook(dwSkillVnum, iPct))
				{
#ifdef ENABLE_BOOKS_STACKFIX
					item->SetCount(item->GetCount() - 1);
#else
					ITEM_MANAGER::instance().RemoveItem(item);
#endif

					int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
					if (distribution_test_server) iReadDelay /= 3;

					SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
				}
			}
			break;
#endif
			case 50902:
			case 50903:
			case 50904:
			{
				if (IsPolymorphed())
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;10021]");
					return false;

				}
				if (GetSkillGroup() == 0)
				{
					ChatPacket(CHAT_TYPE_INFO, "Musis mit zvolene schopnosti");
					return false;

				}
				DWORD dwSkillVnum = SKILL_FISHING;
				int iPct = MINMAX(0, item->GetValue(1), 100);

				if (GetSkillLevel(dwSkillVnum) >= 40)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;10022]");
					return false;
				}

				if (LearnSkillByBook(dwSkillVnum, iPct))
				{
#ifdef ENABLE_BOOKS_STACKFIX
					item->SetCount(item->GetCount() - 1);
#else
					ITEM_MANAGER::instance().RemoveItem(item);
#endif

					int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
					if (distribution_test_server) iReadDelay /= 3;

					SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);

					if (test_server)
					{
						ChatPacket(CHAT_TYPE_INFO, "[TEST_SERVER] Success to learn skill ");
					}
				}
				else
				{
					if (test_server)
					{
						ChatPacket(CHAT_TYPE_INFO, "[TEST_SERVER] Failed to learn skill ");
					}
				}
			}
			break;

			// MINING
			case ITEM_MINING_SKILL_TRAIN_BOOK:
			{
				if (IsPolymorphed())
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;10021]");
					return false;

				}
				if (GetSkillGroup() == 0)
				{
					ChatPacket(CHAT_TYPE_INFO, "Musis mit zvolene schopnosti");
					return false;

				}
				DWORD dwSkillVnum = SKILL_MINING;
				int iPct = MINMAX(0, item->GetValue(1), 100);

				if (GetSkillLevel(dwSkillVnum) >= 40)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;10022]");
					return false;
				}

				if (LearnSkillByBook(dwSkillVnum, iPct))
				{
#ifdef ENABLE_BOOKS_STACKFIX
					item->SetCount(item->GetCount() - 1);
#else
					ITEM_MANAGER::instance().RemoveItem(item);
#endif

					int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
					if (distribution_test_server) iReadDelay /= 3;

					SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
				}
			}
			break;
			// END_OF_MINING

			case ITEM_HORSE_SKILL_TRAIN_BOOK:
			{
				if (IsPolymorphed())
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;1041]");
					return false;

				}
				DWORD dwSkillVnum = SKILL_HORSE;
				int iPct = MINMAX(0, item->GetValue(1), 100);

				if (GetLevel() < 50)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;376]");
					return false;
				}

				if (!test_server && get_global_time() < GetSkillNextReadTime(dwSkillVnum))
				{
					if (FindAffect(AFFECT_SKILL_NO_BOOK_DELAY))
					{
						RemoveAffect(AFFECT_SKILL_NO_BOOK_DELAY);
						ChatPacket(CHAT_TYPE_INFO, "[LS;377]");
					}
					else
					{
						SkillLearnWaitMoreTimeMessage(GetSkillNextReadTime(dwSkillVnum) - get_global_time());
						return false;
					}
				}

				if (GetPoint(POINT_HORSE_SKILL) >= 20 ||
					GetSkillLevel(SKILL_HORSE_WILDATTACK) + GetSkillLevel(SKILL_HORSE_CHARGE) + GetSkillLevel(SKILL_HORSE_ESCAPE) >= 60 ||
					GetSkillLevel(SKILL_HORSE_WILDATTACK_RANGE) + GetSkillLevel(SKILL_HORSE_CHARGE) + GetSkillLevel(SKILL_HORSE_ESCAPE) >= 60)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;378]");
					return false;
				}

				if (number(1, 100) <= iPct)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;379]");
					ChatPacket(CHAT_TYPE_INFO, "[LS;380]");
					PointChange(POINT_HORSE_SKILL, 1);

					int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
					if (distribution_test_server) iReadDelay /= 3;

					if (!test_server)
						SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;382]");
				}
#ifdef ENABLE_BOOKS_STACKFIX
				item->SetCount(item->GetCount() - 1);
#else
				ITEM_MANAGER::instance().RemoveItem(item);
#endif
			}
			break;

			case 70102:
			case 70103:
			{
				if (GetAlignment() >= 0)
					return false;

				int delta = MIN(-GetAlignment(), item->GetValue(0));

				sys_log(0, "%s ALIGNMENT ITEM %d", GetName(), delta);

				UpdateAlignment(delta);
				item->SetCount(item->GetCount() - 1);

				if (delta / 10 > 0)
				{
					ChatPacket(CHAT_TYPE_TALKING, "[LS;383]");
					ChatPacket(CHAT_TYPE_INFO, "[LS;384;%d]", delta / 10);
				}
			}
			break;

			case 71107:
			case 39032: // @fixme169 mythical peach alternative vnum
			{
				int val = item->GetValue(0);
				int interval = item->GetValue(1);
				quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());
				if (!pPC) // @fixme169 missing check
					return false;
				if (GetAlignment() == 200000)
				{
					return false;
				}

				if (200000 - GetAlignment() < val * 10)
				{
					val = (200000 - GetAlignment()) / 10;
				}

				int old_alignment = GetAlignment() / 10;

				UpdateAlignment(val * 10);

				item->SetCount(item->GetCount() - 1);

				ChatPacket(CHAT_TYPE_TALKING, "[LS;383]");
				ChatPacket(CHAT_TYPE_INFO, "[LS;384;%d]", val);

				char buf[256 + 1];
				snprintf(buf, sizeof(buf), "%d %lld", old_alignment, GetAlignment() / 10);
				LogManager::instance().CharLog(this, val, "MYTHICAL_PEACH", buf);
			}
			break;
			
			case 80050: // Jab?o 
			{
				int val = item->GetValue(0);
				quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());
				if (!pPC) // @fixme169 missing check
					return false;

				if (GetAlignment() >= 50000)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2244]");
					return false;
				}

				int old_alignment = GetAlignment();

				UpdateAlignment(val);

				item->SetCount(item->GetCount() - 1);

				ChatPacket(CHAT_TYPE_INFO, "[LS;2245;%d]", val);

				char buf[256 + 1];
				snprintf(buf, sizeof(buf), "%d %lld", old_alignment, GetAlignment());
				LogManager::instance().CharLog(this, val, "MYTHICAL_PEACH", buf);
			}
			break;
			case 80051: // gruszka
			{
				if (GetAlignment() > 49999 && GetAlignment() < 100000)
				{
					int val = item->GetValue(0);
					quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());
					if (!pPC) // @fixme169 missing check
						return false;

					int old_alignment = GetAlignment();

					UpdateAlignment(val);

					item->SetCount(item->GetCount() - 1);

					ChatPacket(CHAT_TYPE_INFO, "[LS;2245;%d]", val);

					char buf[256 + 1];
					snprintf(buf, sizeof(buf), "%d %lld", old_alignment, GetAlignment());
					LogManager::instance().CharLog(this, val, "MYTHICAL_PEACH", buf);
				} 
				else 
				{
					return false;
				}
			}
			break;
			
			case 80052: // winogron
			{
				if (GetAlignment() > 99999 && GetAlignment() < 200000)
				{
					int val = item->GetValue(0);
					quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());
					if (!pPC) // @fixme169 missing check
						return false;

					int old_alignment = GetAlignment();

					UpdateAlignment(val);

					item->SetCount(item->GetCount() - 1);

					ChatPacket(CHAT_TYPE_INFO, "[LS;2245;%d]", val);

					char buf[256 + 1];
					snprintf(buf, sizeof(buf), "%d %lld", old_alignment, GetAlignment());
					LogManager::instance().CharLog(this, val, "MYTHICAL_PEACH", buf);
				} 
				else 
				{
					return false;
				}
			}
			break;
			
			case 80053: // Arbuz
			{
				if (GetAlignment() > 199999 && GetAlignment() < 300000)
				{
					int val = item->GetValue(0);
					quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());
					if (!pPC) // @fixme169 missing check
						return false;

					int old_alignment = GetAlignment();

					UpdateAlignment(val);

					item->SetCount(item->GetCount() - 1);

					ChatPacket(CHAT_TYPE_INFO, "[LS;2245;%d]", val);

					char buf[256 + 1];
					snprintf(buf, sizeof(buf), "%d %lld", old_alignment, GetAlignment());
					LogManager::instance().CharLog(this, val, "MYTHICAL_PEACH", buf);
				} 
				else 
				{
					return false;
				}
			}
			break;
			
			case 80054: // Melon
			{
				if (GetAlignment() > 299999 && GetAlignment() < 400000)
				{
					int val = item->GetValue(0);
					quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());
					if (!pPC) // @fixme169 missing check
						return false;

					int old_alignment = GetAlignment();

					UpdateAlignment(val);

					item->SetCount(item->GetCount() - 1);

					ChatPacket(CHAT_TYPE_INFO, "[LS;2245;%d]", val);

					char buf[256 + 1];
					snprintf(buf, sizeof(buf), "%d %lld", old_alignment, GetAlignment());
					LogManager::instance().CharLog(this, val, "MYTHICAL_PEACH", buf);
				} 
				else 
				{
					return false;
				}
			}
			break;
			
			case 80055: // Truskawka
			{
				if (GetAlignment() > 399999 && GetAlignment() < 500000)
				{
					int val = item->GetValue(0);
					quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());
					if (!pPC) // @fixme169 missing check
						return false;

					int old_alignment = GetAlignment();

					UpdateAlignment(val);

					item->SetCount(item->GetCount() - 1);

					ChatPacket(CHAT_TYPE_INFO, "[LS;2245;%d]", val);

					char buf[256 + 1];
					snprintf(buf, sizeof(buf), "%d %lld", old_alignment, GetAlignment());
					LogManager::instance().CharLog(this, val, "MYTHICAL_PEACH", buf);
				} 
				else 
				{
					return false;
				}
			}
			break;
			
			case 203028:
			{
				if (GetAlignment() > 499999 && GetAlignment() < 600000)
				{
					int val = item->GetValue(0);
					quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());
					if (!pPC) // @fixme169 missing check
						return false;

					int old_alignment = GetAlignment();

					UpdateAlignment(val);

					item->SetCount(item->GetCount() - 1);

					ChatPacket(CHAT_TYPE_INFO, "[LS;2245;%d]", val);

					char buf[256 + 1];
					snprintf(buf, sizeof(buf), "%d %lld", old_alignment, GetAlignment());
					LogManager::instance().CharLog(this, val, "MYTHICAL_PEACH", buf);
				} 
				else 
				{
					return false;
				}
			}
			break;
			
			case 203027:
			{
				if (GetAlignment() > 599999 && GetAlignment() < 700000)
				{
					int val = item->GetValue(0);
					quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());
					if (!pPC) // @fixme169 missing check
						return false;

					int old_alignment = GetAlignment();

					UpdateAlignment(val);

					item->SetCount(item->GetCount() - 1);

					ChatPacket(CHAT_TYPE_INFO, "[LS;2245;%d]", val);

					char buf[256 + 1];
					snprintf(buf, sizeof(buf), "%d %lld", old_alignment, GetAlignment());
					LogManager::instance().CharLog(this, val, "MYTHICAL_PEACH", buf);
				} 
				else 
				{
					return false;
				}
			}
			break;

			case 71109:
			case 72719:
			{
				LPITEM item2;

				if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
					return false;

				if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
					return false;

				if (item2->GetSocketCount() == 0)
					return false;

				switch (item2->GetType())
				{
				case ITEM_WEAPON:
					break;
				case ITEM_ARMOR:
					switch (item2->GetSubType())
					{
					case ARMOR_EAR:
					case ARMOR_WRIST:
					case ARMOR_NECK:
						ChatPacket(CHAT_TYPE_INFO, "[LS;1108]");
						return false;
					}
					break;

				default:
					return false;
				}

				std::stack<int32_t> socket;

				for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
					socket.push(item2->GetSocket(i));

				int idx = ITEM_SOCKET_MAX_NUM - 1;

				while (socket.size() > 0)
				{
					if (socket.top() > 2 && socket.top() != ITEM_BROKEN_METIN_VNUM)
						break;

					idx--;
					socket.pop();
				}

				if (socket.size() == 0)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;1032]");
					return false;
				}

				LPITEM pItemReward = AutoGiveItem(socket.top());

				if (pItemReward != NULL)
				{
					item2->SetSocket(idx, 1);

					char buf[256 + 1];
					snprintf(buf, sizeof(buf), "%s(%u) %s(%u)",
						item2->GetName(), item2->GetID(), pItemReward->GetName(), pItemReward->GetID());
					LogManager::instance().ItemLog(this, item, "USE_DETACHMENT_ONE", buf);

					item->SetCount(item->GetCount() - 1);
				}
			}
			break;

			case 70201:
			case 70202:
			case 70203:
			case 70204:
			case 70205:
			case 70206:
			{
				// NEW_HAIR_STYLE_ADD
				if (GetPart(PART_HAIR) >= 1001)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;385]");
				}
				// END_NEW_HAIR_STYLE_ADD
				else
				{
					quest::CQuestManager& q = quest::CQuestManager::instance();
					quest::PC* pPC = q.GetPC(GetPlayerID());

					if (pPC)
					{
						int last_dye_level = pPC->GetFlag("dyeing_hair.last_dye_level");

						if (last_dye_level == 0 ||
							last_dye_level + 3 <= GetLevel() ||
							item->GetVnum() == 70201)
						{
							SetPart(PART_HAIR, item->GetVnum() - 70201);

							if (item->GetVnum() == 70201)
								pPC->SetFlag("dyeing_hair.last_dye_level", 0);
							else
								pPC->SetFlag("dyeing_hair.last_dye_level", GetLevel());

							item->SetCount(item->GetCount() - 1);
							UpdatePacket();
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, "[LS;386;%d]", last_dye_level + 3);
						}
					}
				}
			}
			break;

			case ITEM_NEW_YEAR_GREETING_VNUM:
			{
				DWORD dwBoxVnum = ITEM_NEW_YEAR_GREETING_VNUM;
				std::vector <DWORD> dwVnums;
				std::vector <DWORD> dwCounts;
				std::vector <LPITEM> item_gets;
				int count = 0;

				if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
				{
					for (int i = 0; i < count; i++)
					{
						if (dwVnums[i] == CSpecialItemGroup::GOLD)
							ChatPacket(CHAT_TYPE_INFO, "[LS;1269;%d]", dwCounts[i]);
					}

					item->SetCount(item->GetCount() - 1);
				}
			}
			break;

			case ITEM_VALENTINE_ROSE:
			case ITEM_VALENTINE_CHOCOLATE:
			{
				DWORD dwBoxVnum = item->GetVnum();
				std::vector <DWORD> dwVnums;
				std::vector <DWORD> dwCounts;
				std::vector <LPITEM> item_gets(0);
				int count = 0;

				if (((item->GetVnum() == ITEM_VALENTINE_ROSE) && (SEX_MALE == GET_SEX(this))) ||
					((item->GetVnum() == ITEM_VALENTINE_CHOCOLATE) && (SEX_FEMALE == GET_SEX(this))))
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;387]");
					return false;
				}

				if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
					item->SetCount(item->GetCount() - 1);
			}
			break;

			case ITEM_WHITEDAY_CANDY:
			case ITEM_WHITEDAY_ROSE:
			{
				DWORD dwBoxVnum = item->GetVnum();
				std::vector <DWORD> dwVnums;
				std::vector <DWORD> dwCounts;
				std::vector <LPITEM> item_gets(0);
				int count = 0;

				if (((item->GetVnum() == ITEM_WHITEDAY_CANDY) && (SEX_MALE == GET_SEX(this))) ||
					((item->GetVnum() == ITEM_WHITEDAY_ROSE) && (SEX_FEMALE == GET_SEX(this))))
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;387]");
					return false;
				}

				if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
					item->SetCount(item->GetCount() - 1);
			}
			break;

			case 50011:
			{
				DWORD dwBoxVnum = 50011;
				std::vector <DWORD> dwVnums;
				std::vector <DWORD> dwCounts;
				std::vector <LPITEM> item_gets(0);
				int count = 0;
				
				auto size = item->GetSize();
				
				if (item->IsDragonSoul())
				{
					if (GetEmptyDragonSoulInventory(item) == -1)
					{
						ChatPacket(CHAT_TYPE_INFO, "You haven't empty slots to get open this item");
						return false;
					}
				}
				else
				{
					if (GetEmptyInventory(size) == -1)
					{
						ChatPacket(CHAT_TYPE_INFO, "You haven't empty inventory slots to open this item.");
						return false;
					}
				}
		
				if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
				{
					for (int i = 0; i < count; i++)
					{
						char buf[50 + 1];
						snprintf(buf, sizeof(buf), "%u %u", dwVnums[i], dwCounts[i]);
						LogManager::instance().ItemLog(this, item, "MOONLIGHT_GET", buf);

						//ITEM_MANAGER::instance().RemoveItem(item);
						item->SetCount(item->GetCount() - 1);

						switch (dwVnums[i])
						{
						case CSpecialItemGroup::GOLD:
							ChatPacket(CHAT_TYPE_INFO, "[LS;1269;%d]", dwCounts[i]);
							break;

						case CSpecialItemGroup::EXP:
							ChatPacket(CHAT_TYPE_INFO, "[LS;1279]");
							ChatPacket(CHAT_TYPE_INFO, "[LS;1290;%d]", dwCounts[i]);
							break;

						case CSpecialItemGroup::MOB:
							ChatPacket(CHAT_TYPE_INFO, "[LS;1299]");
							break;

						case CSpecialItemGroup::SLOW:
							ChatPacket(CHAT_TYPE_INFO, "[LS;1310]");
							break;

						case CSpecialItemGroup::DRAIN_HP:
							ChatPacket(CHAT_TYPE_INFO, "[LS;3]");
							break;

						case CSpecialItemGroup::POISON:
							ChatPacket(CHAT_TYPE_INFO, "[LS;13]");
							break;
#ifdef ENABLE_WOLFMAN_CHARACTER
						case CSpecialItemGroup::BLEEDING:
							ChatPacket(CHAT_TYPE_INFO, "[LS;13]");
							break;
#endif
						case CSpecialItemGroup::MOB_GROUP:
							ChatPacket(CHAT_TYPE_INFO, "[LS;1299]");
							break;

						default:
							if (item_gets[i])
							{
								#if defined(__CHATTING_WINDOW_RENEWAL__)
								if (dwCounts[i] > 1)
									ChatPacket(CHAT_TYPE_ITEM_INFO, "[LS;204;%s;%d]", item_gets[i]->GetName(), dwCounts[i]);
								else
									ChatPacket(CHAT_TYPE_ITEM_INFO, "[LS;35;%s]", item_gets[i]->GetName());
								#else
								if (dwCounts[i] > 1)
									ChatPacket(CHAT_TYPE_INFO, "[LS;204;%s;%d]", item_gets[i]->GetName(), dwCounts[i]);
								else
									ChatPacket(CHAT_TYPE_INFO, "[LS;35;%s]", item_gets[i]->GetName());
								#endif
							}
							break;
						}
					}
				}
				else
				{
					ChatPacket(CHAT_TYPE_TALKING, "[LS;56]");
					return false;
				}
			}
			break;

			case ITEM_GIVE_STAT_RESET_COUNT_VNUM:
			{
				//PointChange(POINT_GOLD, -iCost);
				PointChange(POINT_STAT_RESET_COUNT, 1);
				item->SetCount(item->GetCount() - 1);
			}
			break;

			case 50107:
			{
				if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;1205]");
					return false;
				}
#ifdef ENABLE_NEWSTUFF
				else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;1205]");
					return false;
				}
#endif

				EffectPacket(SE_CHINA_FIREWORK);
#ifdef ENABLE_FIREWORK_STUN

				AddAffect(AFFECT_CHINA_FIREWORK, POINT_STUN_PCT, 30, AFF_CHINA_FIREWORK, 5 * 60, 0, true);
#endif
				item->SetCount(item->GetCount() - 1);
			}
			break;

			case 50108:
			{
				if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;1205]");
					return false;
				}
#ifdef ENABLE_NEWSTUFF
				else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;1205]");
					return false;
				}
#endif

				EffectPacket(SE_SPIN_TOP);
#ifdef ENABLE_FIREWORK_STUN

				AddAffect(AFFECT_CHINA_FIREWORK, POINT_STUN_PCT, 30, AFF_CHINA_FIREWORK, 5 * 60, 0, true);
#endif
				item->SetCount(item->GetCount() - 1);
			}
			break;

			case ITEM_WONSO_BEAN_VNUM:
				PointChange(POINT_HP, GetMaxHP() - GetHP());
				item->SetCount(item->GetCount() - 1);
				break;

			case ITEM_WONSO_SUGAR_VNUM:
				PointChange(POINT_SP, GetMaxSP() - GetSP());
				item->SetCount(item->GetCount() - 1);
				break;

			case ITEM_WONSO_FRUIT_VNUM:
				PointChange(POINT_STAMINA, GetMaxStamina() - GetStamina());
				item->SetCount(item->GetCount() - 1);
				break;

			//case 90008: // VCARD
			//case 90009: // VCARD
			//	//VCardUse(this, this, item);
			//	break;

			case ITEM_ELK_VNUM:
			{
				int iGold = item->GetSocket(0);
				ITEM_MANAGER::instance().RemoveItem(item);
				ChatPacket(CHAT_TYPE_INFO, "[LS;1269;%d]", iGold);
				PointChange(POINT_GOLD, iGold);
			}
			break;
			

			case 70021:
			{
				int HealPrice = quest::CQuestManager::instance().GetEventFlag("MonarchHealGold");
				if (HealPrice == 0)
					HealPrice = 2000000;

				if (CMonarch::instance().HealMyEmpire(this, HealPrice))
				{
					char szNotice[256];
					snprintf(szNotice, sizeof(szNotice), "[LS;559;%s]", EMPIRE_NAME(GetEmpire()));
					SendNoticeMap(szNotice, GetMapIndex(), false);

					ChatPacket(CHAT_TYPE_INFO, "[LS;570]");
				}
			}
			break;

			case 27995:
			{
			}
			break;

			case 71092:
			{
				if (m_pkChrTarget != NULL)
				{
					if (m_pkChrTarget->IsPolymorphed())
					{
						m_pkChrTarget->SetPolymorph(0);
						m_pkChrTarget->RemoveAffect(AFFECT_POLYMORPH);
					}
				}
				else
				{
					if (IsPolymorphed())
					{
						SetPolymorph(0);
						RemoveAffect(AFFECT_POLYMORPH);
					}
				}
			}
			break;

			case 204013 :
				{
					LPITEM item2;

					if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
					{
						return false;
					}
					
					if (item2->IsEquipped() || item2->IsExchanging() || item2->IsEquipped())
					{
						return false;
					}
					
					//if(item2-> GetType() != ITEM_WEAPON || item2-> GetType() != ITEM_ARMOR)
					//{
					//	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("POUZITIPOUZENAZBRANABRNENI!"));
					//	return false;
					//}
					
					if(item2->GetType() == ITEM_COSTUME)
					{
						ChatPacket(CHAT_TYPE_INFO, "This item cannot be used on this item type.");
						return false;
					}
					
					if(item2->GetSocketCount() >= 5)
					{
						ChatPacket(CHAT_TYPE_INFO, "This item already have maximum stone slots.");
						return false;
					}
					/*int count = item2->GetSocketCount(); // Druha mo?nost
						
					if (count == ITEM_SOCKET_MAX_NUM)
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("...!"));
						return false;*/
					if(item2-> GetType() == ITEM_WEAPON || item2-> GetType() == ITEM_ARMOR && item2-> GetSubType() == ARMOR_BODY )
					{
						item2->AddSocket();
						item2->UpdatePacket();
						ChatPacket(CHAT_TYPE_INFO, "Successfully added stone slot to item.");
						item->SetCount(item->GetCount() - 1);
						Save();
					}
					else
					{
						ChatPacket(CHAT_TYPE_INFO, "This item already have maximum stone slots.");
						return false;
					}
					//item2->SetSocket(3, true);
					//item2->UpdatePacket();
					//ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Uspesnepridano!"));
					//item->SetCount(item->GetCount() - 1);
					//Save();
				}
				break;

			case 71051:
			{
				LPITEM item2;

				if (!IsValidItemPosition(DestCell) || !(item2 = GetInventoryItem(wDestCell)))
					return false;

				if (ITEM_COSTUME == item2->GetType()) // @fixme124
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;1959]");
					return false;
				}
				
				if (ITEM_BELT == item2->GetType()) // @fixme124
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;1959]");
					return false;
				}
				
				if (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_GLOVE) // @fixme124
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;1959]");
					return false;
				}

				if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
					return false;

				if (item2->GetAttributeSetIndex() == -1)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;1959]");
					return false;
				}

#ifdef ENABLE_ITEM_RARE_ATTR_LEVEL_PCT
				if (item2->AddRareAttribute2())
#else
				if (item2->AddRareAttribute())
#endif
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;1958]");

					int iAddedIdx = item2->GetRareAttrCount() + 4;
					char buf[21];
					snprintf(buf, sizeof(buf), "%u", item2->GetID());

					LogManager::instance().ItemLog(
						GetPlayerID(),
						item2->GetAttributeType(iAddedIdx),
						item2->GetAttributeValue(iAddedIdx),
						item->GetID(),
						"ADD_RARE_ATTR",
						buf,
						GetDesc()->GetHostName(),
						item->GetOriginalVnum());

					item->SetCount(item->GetCount() - 1);
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;1957]");
				}
			}
			break;

			case 71052:
			{
				LPITEM item2;

				if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
					return false;

				if (ITEM_COSTUME == item2->GetType()) // @fixme124
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;1959]");
					return false;
				}

				if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
					return false;

				if (item2->GetAttributeSetIndex() == -1)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;1959]");
					return false;
				}

				const auto SwitchRareEventActive = CHARACTER_MANAGER::Instance().CheckEventIsActive(SWITCH_RARE_EVENT, 0, this);
#ifdef ENABLE_ITEM_RARE_ATTR_LEVEL_PCT
				if (SwitchRareEventActive)
				{
					if (item2->ChangeRareAttributeEvent()) 
#else
				if (SwitchRareEventActive)
				{
					if (item2->ChangeRareAttributeEvent()) 
#endif
					{
						char buf[21];
						snprintf(buf, sizeof(buf), "%u", item2->GetID());
						LogManager::instance().ItemLog(this, item, "CHANGE_RARE_ATTR", buf);
						bool shouldConsumeItem = true;
				
//#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
//						if (GetTitleAchievement() == 4)
//						{
//							// 5% chance to not consume the item
//							if (number(0, 100) < 5)
//							{
//								shouldConsumeItem = false;
//								ChatPacket(CHAT_TYPE_INFO, "Your title saved the rare changer from being consumed!");
//							}
//						}
//#endif
				
						if (shouldConsumeItem)
						{
//#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
//							const int RareChangersCount = GetQuestFlag("AchievementTitle.rarechangers_used") + 1;
//							SetQuestFlag("AchievementTitle.rarechangers_used", RareChangersCount);
//									
//							CTitleAchievementTitle::instance().CheckAndUnlockTitles(this);
//									
//							// Send progress update to client
//							ChatPacket(CHAT_TYPE_COMMAND, "title_progress_rare_count %d", RareChangersCount);
//#endif
							item->SetCount(item->GetCount() - 1);
						}
					}
				}
				else {
					
				#ifdef ENABLE_ITEM_RARE_ATTR_LEVEL_PCT
					if (item2->ChangeRareAttribute2())
				#else
					if (item2->ChangeRareAttribute())
				#endif
					{
						char buf[21];
						snprintf(buf, sizeof(buf), "%u", item2->GetID());
						LogManager::instance().ItemLog(this, item, "CHANGE_RARE_ATTR", buf);
						bool shouldConsumeItem = true;
				
//#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
//						if (GetTitleAchievement() == 4)
//						{
//							// 5% chance to not consume the item
//							if (number(0, 100) < 5)
//							{
//								shouldConsumeItem = false;
//								ChatPacket(CHAT_TYPE_INFO, "Your title saved the rare changer from being consumed!");
//							}
//						}
//#endif
				
						if (shouldConsumeItem)
						{
//#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
//							const int RareChangersCount = GetQuestFlag("AchievementTitle.rarechangers_used") + 1;
//							SetQuestFlag("AchievementTitle.rarechangers_used", RareChangersCount);
//									
//							CTitleAchievementTitle::instance().CheckAndUnlockTitles(this);
//									
//							// Send progress update to client
//							ChatPacket(CHAT_TYPE_COMMAND, "title_progress_rare_count %d", RareChangersCount);
//#endif
							item->SetCount(item->GetCount() - 1);
						}
					}
				}
			}
			break;

			case ITEM_AUTO_HP_RECOVERY_S:
			case ITEM_AUTO_HP_RECOVERY_M:
			case ITEM_AUTO_HP_RECOVERY_L:
			case ITEM_AUTO_HP_RECOVERY_X:
			case ITEM_AUTO_SP_RECOVERY_S:
			case ITEM_AUTO_SP_RECOVERY_M:
			case ITEM_AUTO_SP_RECOVERY_L:
			case ITEM_AUTO_SP_RECOVERY_X:

			case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_XS:
			case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_S:
			case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_XS:
			case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_S:
			case FUCKING_BRAZIL_ITEM_AUTO_SP_RECOVERY_S:
			case FUCKING_BRAZIL_ITEM_AUTO_HP_RECOVERY_S:
			{
#ifdef ENABLE_NEWSTUFF
				if (!PulseManager::Instance().IncreaseClock(GetPlayerID(), ePulse::Potions, std::chrono::milliseconds(1500)))
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2200;%f]", PULSEMANAGER_CLOCK_TO_SEC2(GetPlayerID(), ePulse::Potions));
					return false;
				}
#endif
				if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;403]");
					return false;
				}
#ifdef ENABLE_NEWSTUFF
				else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;1205]");
					return false;
				}
#endif

				EAffectTypes type = AFFECT_NONE;
				bool isSpecialPotion = false;

				switch (item->GetVnum())
				{
				case ITEM_AUTO_HP_RECOVERY_X:
					isSpecialPotion = true;

				case ITEM_AUTO_HP_RECOVERY_S:
				case ITEM_AUTO_HP_RECOVERY_M:
				case ITEM_AUTO_HP_RECOVERY_L:
				case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_XS:
				case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_S:
				case FUCKING_BRAZIL_ITEM_AUTO_HP_RECOVERY_S:
					type = AFFECT_AUTO_HP_RECOVERY;
					break;

				case ITEM_AUTO_SP_RECOVERY_X:
					isSpecialPotion = true;

				case ITEM_AUTO_SP_RECOVERY_S:
				case ITEM_AUTO_SP_RECOVERY_M:
				case ITEM_AUTO_SP_RECOVERY_L:
				case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_XS:
				case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_S:
				case FUCKING_BRAZIL_ITEM_AUTO_SP_RECOVERY_S:
					type = AFFECT_AUTO_SP_RECOVERY;
					break;
				}

				if (AFFECT_NONE == type)
					break;

				if (item->GetCount() > 1)
				{
					int pos = GetEmptyInventory(item->GetSize());

					if (-1 == pos)
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;1130]");
						break;
					}

					item->SetCount(item->GetCount() - 1);

					LPITEM item2 = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), 1);
					item2->AddToCharacter(this, TItemPos(INVENTORY, pos));

					if (item->GetSocket(1) != 0)
					{
						item2->SetSocket(1, item->GetSocket(1));
					}

					item = item2;
				}

				CAffect* pAffect = FindAffect(type);

				if (NULL == pAffect)
				{
					EPointTypes bonus = POINT_NONE;

					if (true == isSpecialPotion)
					{
						if (type == AFFECT_AUTO_HP_RECOVERY)
						{
							bonus = POINT_MAX_HP_PCT;
						}
						else if (type == AFFECT_AUTO_SP_RECOVERY)
						{
							bonus = POINT_MAX_SP_PCT;
						}
					}

					AddAffect(type, bonus, 4, item->GetID(), INFINITE_AFFECT_DURATION, 0, true, false);

					item->Lock(true);
					item->SetSocket(0, true);

					AutoRecoveryItemProcess(type);
				}
				else
				{
					if (item->GetID() == pAffect->dwFlag)
					{
						RemoveAffect(pAffect);

						item->Lock(false);
						item->SetSocket(0, false);
					}
					else
					{
						LPITEM old = FindItemByID(pAffect->dwFlag);

						if (NULL != old)
						{
							old->Lock(false);
							old->SetSocket(0, false);
						}

						RemoveAffect(pAffect);

						EPointTypes bonus = POINT_NONE;

						if (true == isSpecialPotion)
						{
							if (type == AFFECT_AUTO_HP_RECOVERY)
							{
								bonus = POINT_MAX_HP_PCT;
							}
							else if (type == AFFECT_AUTO_SP_RECOVERY)
							{
								bonus = POINT_MAX_SP_PCT;
							}
						}

						AddAffect(type, bonus, 4, item->GetID(), INFINITE_AFFECT_DURATION, 0, true, false);

						item->Lock(true);
						item->SetSocket(0, true);

						AutoRecoveryItemProcess(type);
					}
				}
			}
			break;
			}
			break;

		case USE_CLEAR:
		{
			switch (item->GetVnum())
			{
#ifdef ENABLE_WOLFMAN_CHARACTER
			case 27124: // Bandage
				RemoveBleeding();
				break;
#endif
			case 27874: // Grilled Perch
			default:
				RemoveBadAffect();
				break;
			}
			item->SetCount(item->GetCount() - 1);
		}
		break;

		case USE_INVISIBILITY:
		{
			if (item->GetVnum() == 70026)
			{
				quest::CQuestManager& q = quest::CQuestManager::instance();
				quest::PC* pPC = q.GetPC(GetPlayerID());

				if (pPC != NULL)
				{
					int last_use_time = pPC->GetFlag("mirror_of_disapper.last_use_time");

					if (get_global_time() - last_use_time < 10 * 60)
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;217]");
						return false;
					}

					pPC->SetFlag("mirror_of_disapper.last_use_time", get_global_time());
				}
			}

			AddAffect(AFFECT_INVISIBILITY, POINT_NONE, 0, AFF_INVISIBILITY, 300, 0, true);
			item->SetCount(item->GetCount() - 1);
		}
		break;

		case USE_POTION_NODELAY:
		{
			if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
			{
				if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit") > 0)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;403]");
					return false;
				}

				switch (item->GetVnum())
				{
				case 70020:
				case 71018:
				case 71019:
				case 71020:
					if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count") < 10000)
					{
						if (m_nPotionLimit <= 0)
						{
							ChatPacket(CHAT_TYPE_INFO, "[LS;122]");
							return false;
						}
					}
					break;

				default:
					ChatPacket(CHAT_TYPE_INFO, "[LS;403]");
					return false;
				}
			}
#ifdef ENABLE_NEWSTUFF
			else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;1205]");
				return false;
			}
#endif

			bool used = false;

			if (item->GetValue(0) != 0)
			{
				if (GetHP() < GetMaxHP())
				{
					PointChange(POINT_HP, item->GetValue(0) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
					EffectPacket(SE_HPUP_RED);
					used = TRUE;
				}
			}

			if (item->GetValue(1) != 0)
			{
				if (GetSP() < GetMaxSP())
				{
					PointChange(POINT_SP, item->GetValue(1) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
					EffectPacket(SE_SPUP_BLUE);
					used = TRUE;
				}
			}

			if (item->GetValue(3) != 0)
			{
				if (GetHP() < GetMaxHP())
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;10068]");
					//PointChange(POINT_HP, item->GetValue(3) * GetMaxHP() / 100);
					//EffectPacket(SE_HPUP_RED);
					//used = TRUE;
				}
			}

			if (item->GetValue(4) != 0)
			{
				if (GetSP() < GetMaxSP())
				{
					PointChange(POINT_SP, item->GetValue(4) * GetMaxSP() / 100);
					EffectPacket(SE_SPUP_BLUE);
					used = TRUE;
				}
			}

			if (used)
			{
				if (item->GetVnum() == 50085 || item->GetVnum() == 50086)
				{
					if (test_server)
						ChatPacket(CHAT_TYPE_INFO, "[LS;132]");
					SetUseSeedOrMoonBottleTime();
				}
				if (GetDungeon())
					GetDungeon()->UsePotion(this);

				if (GetWarMap())
					GetWarMap()->UsePotion(this, item);

				m_nPotionLimit--;

				//RESTRICT_USE_SEED_OR_MOONBOTTLE
				item->SetCount(item->GetCount() - 1);
				//END_RESTRICT_USE_SEED_OR_MOONBOTTLE
			}
#ifdef ENABLE_NEWSTUFF
			if (!PulseManager::Instance().IncreaseClock(GetPlayerID(), ePulse::Blogo, std::chrono::milliseconds(1000)))
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;2200;%f]", PULSEMANAGER_CLOCK_TO_SEC2(GetPlayerID(), ePulse::Blogo));
				return false;
			}
#endif
		}
		break;

		case USE_POTION:
			if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
			{
				if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit") > 0)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;403]");
					return false;
				}

				switch (item->GetVnum())
				{
				case 27001:
				case 27002:
				case 27003:
				case 27004:
				case 27005:
				case 27006:
					if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count") < 10000)
					{
						if (m_nPotionLimit <= 0)
						{
							ChatPacket(CHAT_TYPE_INFO, "[LS;122]");
							return false;
						}
					}
					break;

				default:
					ChatPacket(CHAT_TYPE_INFO, "[LS;403]");
					return false;
				}
			}
#ifdef ENABLE_NEWSTUFF
			else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;1205]");
				return false;
			}
#endif

			if (item->GetValue(1) != 0)
			{
				if (GetPoint(POINT_SP_RECOVERY) + GetSP() >= GetMaxSP())
				{
					return false;
				}

				PointChange(POINT_SP_RECOVERY, item->GetValue(1) * MIN(200, (100 + GetPoint(POINT_POTION_BONUS))) / 100);
				StartAffectEvent();
				EffectPacket(SE_SPUP_BLUE);
			}

			if (item->GetValue(0) != 0)
			{
				if (GetPoint(POINT_HP_RECOVERY) + GetHP() >= GetMaxHP())
				{
					return false;
				}

				PointChange(POINT_HP_RECOVERY, item->GetValue(0) * MIN(200, (100 + GetPoint(POINT_POTION_BONUS))) / 100);
				StartAffectEvent();
				EffectPacket(SE_HPUP_RED);
			}

			if (GetDungeon())
				GetDungeon()->UsePotion(this);

			if (GetWarMap())
				GetWarMap()->UsePotion(this, item);

			// item->SetCount(item->GetCount() - 1);
			m_nPotionLimit--;
			break;

		case USE_POTION_CONTINUE:
		{
			if (item->GetValue(0) != 0)
			{
				AddAffect(AFFECT_HP_RECOVER_CONTINUE, POINT_HP_RECOVER_CONTINUE, item->GetValue(0), 0, item->GetValue(2), 0, true);
			}
			else if (item->GetValue(1) != 0)
			{
				AddAffect(AFFECT_SP_RECOVER_CONTINUE, POINT_SP_RECOVER_CONTINUE, item->GetValue(1), 0, item->GetValue(2), 0, true);
			}
			else
				return false;
		}

		if (GetDungeon())
			GetDungeon()->UsePotion(this);

		if (GetWarMap())
			GetWarMap()->UsePotion(this, item);

		item->SetCount(item->GetCount() - 1);
		break;

		case USE_ABILITY_UP:
		{
#ifdef USE_FISH_SYSTEM
			if (item->GetVnum() > FISH_ITEMID_START-1 && item->GetVnum() < FISH_ITEMID_END+1) {
				DWORD pointid = (aApplyInfo[item->GetValue(0)].bPointType);
				if (FindAffect(AFFECT_BLEND_FISH, pointid)) {
					ChatPacket(CHAT_TYPE_INFO, "This effect is already activated.");
				} else {
					AddAffect(AFFECT_BLEND_FISH, pointid, item->GetValue(2), 0, item->GetValue(1), 0, false);
						item->SetCount(item->GetCount() - 1);
				}
			}
#endif
			switch (item->GetValue(0))
			{
				case APPLY_MOV_SPEED:
					AddAffect(AFFECT_MOV_SPEED, POINT_MOV_SPEED, item->GetValue(2), AFF_MOV_SPEED_POTION, item->GetValue(1), 0, true);
#ifdef ENABLE_EFFECT_EXTRAPOT
					EffectPacket(SE_DXUP_PURPLE);
#endif
					break;

				case APPLY_ATT_SPEED:
					AddAffect(AFFECT_ATT_SPEED, POINT_ATT_SPEED, item->GetValue(2), AFF_ATT_SPEED_POTION, item->GetValue(1), 0, true);
#ifdef ENABLE_EFFECT_EXTRAPOT
					EffectPacket(SE_SPEEDUP_GREEN);
#endif
					break;
			}
		}

		if (GetDungeon())
			GetDungeon()->UsePotion(this);

		if (GetWarMap())
			GetWarMap()->UsePotion(this, item);

		// item->SetCount(item->GetCount() - 1);
		break;

		case USE_TALISMAN:
		{
			const int TOWN_PORTAL = 1;
			const int MEMORY_PORTAL = 2;

			if (GetMapIndex() == 200 || GetMapIndex() == 113)
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;388]");
				return false;
			}

			if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;1205]");
				return false;
			}
#ifdef ENABLE_NEWSTUFF
			else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;1205]");
				return false;
			}
#endif

			if (m_pkWarpEvent)
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;389]");
				return false;
			}

			// CONSUME_LIFE_WHEN_USE_WARP_ITEM
			int consumeLife = CalculateConsume(this);

			if (consumeLife < 0)
				return false;
			// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM

			if (item->GetValue(0) == TOWN_PORTAL)
			{
				if (item->GetSocket(0) == 0)
				{
					if (!GetDungeon())
						if (!GiveRecallItem(item))
							return false;

					PIXEL_POSITION posWarp;

					if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(GetMapIndex(), GetEmpire(), posWarp))
					{
						// CONSUME_LIFE_WHEN_USE_WARP_ITEM
						PointChange(POINT_HP, -consumeLife, false);
						// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM

						WarpSet(posWarp.x, posWarp.y);
					}
					else
					{
						sys_err("CHARACTER::UseItem : cannot find spawn position (name %s, %d x %d)", GetName(), GetX(), GetY());
					}
				}
				else
				{
					if (test_server)
						ChatPacket(CHAT_TYPE_INFO, "[LS;390]");

					ProcessRecallItem(item);
				}
			}
			else if (item->GetValue(0) == MEMORY_PORTAL)
			{
				if (item->GetSocket(0) == 0)
				{
					if (GetDungeon())
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;391;%s;%s]",
							item->GetName(),
							"");
						return false;
					}

					if (!GiveRecallItem(item))
						return false;
				}
				else
				{
					// CONSUME_LIFE_WHEN_USE_WARP_ITEM
					PointChange(POINT_HP, -consumeLife, false);
					// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM

					ProcessRecallItem(item);
				}
			}
		}
		break;
		
		case USE_TUNING:
		case USE_DETACHMENT:
		{
			LPITEM item2;

			if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
				return false;

			if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
				return false;

			if (item2->GetVnum() >= 28330 && item2->GetVnum() <= 28343)
			{
				return false;
			}

			if (item2->GetVnum() >= 28430 && item2->GetVnum() <= 28443)
			{
				if (item->GetVnum() == 71056)
				{
					RefineItem(item, item2);
				}
			}
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			if (item->GetValue(0) == ACCE_CLEAN_ATTR_VALUE0 && item->GetVnum() != 51003 || item->GetVnum() == ACCE_REVERSAL_VNUM_1 || item->GetVnum() == ACCE_REVERSAL_VNUM_2)
			{
				if (!CleanAcceAttr(item, item2))
					return false;
					
				item->SetCount(item->GetCount()-1);
				return true;
			}
#endif
			else
			{
				RefineItem(item, item2);
			}
		}
		break;
		
		case USE_SASH_CLEANER:
		{
			LPITEM item2;
			if (!IsValidItemPosition(DestCell) || !(item2 = GetInventoryItem(wDestCell)))
			{
				return false;
			}
			if (item2->IsExchanging() == true)
			{
				return false;
			}
			if (item2->GetType() != ITEM_COSTUME || item2->GetSubType() != COSTUME_ACCE)
			{
				return false;
			}
			if (GetEmptyInventory(3) == -1)
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;5002]");
				return false;
			}
			if (!item2->GetSocket(1))
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;5003]");
				return false;
			}
			auto pItem = ITEM_MANAGER::instance().CreateItem(item2->GetSocket(1), 1);
			if (pItem)
			{
				item2->CopyAttributeTo(pItem);
				item2->ClearAttribute();
				item2->SetForceAttribute(5, 0, 0);
				item2->SetForceAttribute(6, 0, 0);
				for (auto i = ACCE_ABSORPTION_SOCKET + 1; i < ITEM_SOCKET_MAX_NUM; i++)
				{
					item2->SetSocket(i, 0);
				}
				item->SetCount(item->GetCount() - 1); 
				AutoGiveItem(pItem);
			}
		}
		break;
		
		//case USE_ALCHEMY_CHANGER:
		//{
		//	if (wDestCell >= INVENTORY_MAX_NUM)
		//		DestCell.window_type = DRAGON_SOUL_INVENTORY;
		//	LPITEM item2;
		//	if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
		//	{
		//		ChatPacket(CHAT_TYPE_INFO, "!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)");
		//		return false;
		//	}
		//	if (item2->IsExchanging() == true)
		//	{
		//		return false;
		//	}
		//	if (item2->IsDragonSoul())
		//	{
		//		//item2->ClearAttribute(true);
		//		ChatPacket(CHAT_TYPE_INFO, "ClearAttribute now");
		//		DSManager::instance().PutAttributes(item2);
		//		ChatPacket(CHAT_TYPE_INFO, "PutAttributes now");
		//		item->SetCount(item->GetCount() - 1);
		//		ChatPacket(CHAT_TYPE_INFO, "SetCount -1 now");
		//	}
		//	else
		//	{
		//		ChatPacket(CHAT_TYPE_INFO, "This item can be used only on alchemy stones");
		//		return false;
		//	}
		//}
		//break;

		case USE_CHANGE_COSTUME_ATTR:
		case USE_RESET_COSTUME_ATTR:
		{
			LPITEM item2;
			if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
				return false;

			if (item2->IsEquipped())
			{
				BuffOnAttr_RemoveBuffsFromItem(item2);
			}

			if (ITEM_COSTUME != item2->GetType())
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;396]");
				return false;
			}

			if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
				return false;

			if (item2->GetAttributeSetIndex() == -1)
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;396]");
				return false;
			}

			if (item2->GetAttributeCount() == 0)
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;397]");
				return false;
			}

			switch (item->GetSubType())
			{
			case USE_CHANGE_COSTUME_ATTR:
				item2->ChangeAttribute();
				{
					char buf[21];
					snprintf(buf, sizeof(buf), "%u", item2->GetID());
					LogManager::instance().ItemLog(this, item, "CHANGE_COSTUME_ATTR", buf);
				}
				break;
			case USE_RESET_COSTUME_ATTR:
				item2->ClearAttribute();
				item2->AlterToMagicItem();
				{
					char buf[21];
					snprintf(buf, sizeof(buf), "%u", item2->GetID());
					LogManager::instance().ItemLog(this, item, "RESET_COSTUME_ATTR", buf);
				}
				break;
			}

			ChatPacket(CHAT_TYPE_INFO, "[LS;399]");

			item->SetCount(item->GetCount() - 1);
			break;
		}

		//  ACCESSORY_REFINE & ADD/CHANGE_ATTRIBUTES
		case USE_PUT_INTO_BELT_SOCKET:
		case USE_PUT_INTO_RING_SOCKET:
		case USE_PUT_INTO_ACCESSORY_SOCKET:
		case USE_ADD_ACCESSORY_SOCKET:
		case USE_CLEAN_SOCKET:
		case USE_CHANGE_ATTRIBUTE:
		case USE_CHANGE_ATTRIBUTE2:
		case USE_ADD_ATTRIBUTE:
		case USE_ADD_ATTRIBUTE2:
#ifdef ENABLE_SPECIAL_BONUS
		case USE_ADD_ATTRIBUTE_SPECIAL_1:
#endif
		case USE_ADD_IS_BONUS:
		case USE_ADD_NEW_BONUS:
		case USE_ADD_EXTRA_BONUS:
		{
			LPITEM item2;
			if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
				return false;

			if (item2->GetVnum() == 53012) {  // blokada dodawania do startowego peta
				return false;
			}
			// kostiumy - tylko dodania pvp i exp // blokada innych
			if (item2->GetVnum() >= 41001 && item2->GetVnum() <= 41999) {
				if (item->GetVnum() != 33052 && item->GetVnum() != 33053) {
					return false;
				}
			}
			// fryzury - tylko dodania pvp i exp // blokada innych
			if (item2->GetVnum() >= 45001 && item2->GetVnum() <= 45999) {
				if (item->GetVnum() != 33054 && item->GetVnum() != 33055) {
					return false;
				}
			}
			// pasy - tylko dodania i zmianki dla pasa // blokada innych
			if (item2->GetType() == ITEM_BELT) {
				if (item->GetVnum() != 79010 && item->GetVnum() != 79011 && item->GetVnum() != 50649) {
					return false;
				}
			}
			// relawice - tylko dodania i zmianki dla rekawic, extrabonus // blokada innych
			if (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_GLOVE) {
				if (item->GetVnum() != 79012 && item->GetVnum() != 79013 && item->GetVnum() != 34000 && item->GetVnum() != 203011) {
					return false;
				}
			}
			// talisman
			if (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_PENDANT_SOUL) {
				if (item->GetVnum() != 10781 && item->GetVnum() != 10782 && item->GetVnum() != 10783 && item->GetVnum() != 10784) {
					return false;
				}
			}
			// extra bonus
			if (item2->GetVnum() >= 85001 && item2->GetVnum() <= 85024 ||
				item2->GetVnum() == 80109 || item2->GetVnum() == 80119 || item2->GetVnum() == 80129 ||
				item2->GetVnum() == 55045 || item2->GetVnum() == 55055 || item2->GetVnum() == 55065 || 
				item2->GetVnum() == 55075 || item2->GetVnum() == 55085 || item2->GetVnum() == 55095
			) {
				if (item->GetVnum() != 34000) {
					return false;
				}
			}

			if (item2->IsEquipped())
			{
				BuffOnAttr_RemoveBuffsFromItem(item2);
			}

			if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
				return false;

			switch (item->GetSubType())
			{

			case USE_CLEAN_SOCKET:
			{
				int i;
#ifdef ENABLE_EXTENDED_SOCKETS
				for (i = 0; i < ITEM_STONES_MAX_NUM; ++i)
#else
				for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
#endif
				{
					if (item2->GetSocket(i) == ITEM_BROKEN_METIN_VNUM)
						break;
				}

#ifdef ENABLE_EXTENDED_SOCKETS
				if (i == ITEM_STONES_MAX_NUM)
#else
				if (i == ITEM_SOCKET_MAX_NUM)
#endif
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;395]");
					return false;
				}

				int j = 0;

#ifdef ENABLE_EXTENDED_SOCKETS
				for (i = 0; i < ITEM_STONES_MAX_NUM; ++i)
#else
				for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
#endif
				{
					if (item2->GetSocket(i) != ITEM_BROKEN_METIN_VNUM && item2->GetSocket(i) != 0)
						item2->SetSocket(j++, item2->GetSocket(i));
				}

#ifdef ENABLE_EXTENDED_SOCKETS
				for (; j < ITEM_STONES_MAX_NUM; ++j)
#else
				for (; j < ITEM_SOCKET_MAX_NUM; ++j)
#endif
				{
					if (item2->GetSocket(j) > 0)
						item2->SetSocket(j, 1);
				}

				{
					char buf[21];
					snprintf(buf, sizeof(buf), "%u", item2->GetID());
					LogManager::instance().ItemLog(this, item, "CLEAN_SOCKET", buf);
				}

				item->SetCount(item->GetCount() - 1);

			}
			break;

			case USE_CHANGE_ATTRIBUTE:
			case USE_CHANGE_ATTRIBUTE2: // @fixme123
				if (item->GetVnum() >= 33050 && item->GetVnum() <= 33061) {
					return false;
				}

				if (item2->GetAttributeSetIndex() == -1)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;396]");
					return false;
				}

				if (item2->GetAttributeCount() == 0)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;397]");
					return false;
				}
				if ((GM_PLAYER == GetGMLevel()) && (false == test_server) && (g_dwItemBonusChangeTime > 0))
				{
					DWORD dwChangeItemAttrCycle = g_dwItemBonusChangeTime;

					quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());

					if (pPC)
					{
						DWORD dwNowSec = get_global_time();

						DWORD dwLastChangeItemAttrSec = pPC->GetFlag(msc_szLastChangeItemAttrFlag);

						if (dwLastChangeItemAttrSec + dwChangeItemAttrCycle > dwNowSec)
						{
							ChatPacket(CHAT_TYPE_INFO, "[LS;398;%d;%d]",
								dwChangeItemAttrCycle, dwChangeItemAttrCycle - (dwNowSec - dwLastChangeItemAttrSec));
							return false;
						}

						pPC->SetFlag(msc_szLastChangeItemAttrFlag, dwNowSec);
					}
				}

				if (item->GetSubType() == USE_CHANGE_ATTRIBUTE2)
				{
					int aiChangeProb[ITEM_ATTRIBUTE_MAX_LEVEL] =
					{
						0, 0, 30, 40, 3
					};

					item2->ChangeAttribute(aiChangeProb);
				}
				else if (item->GetVnum() == 76014)
				{
					int aiChangeProb[ITEM_ATTRIBUTE_MAX_LEVEL] =
					{
						0, 10, 50, 39, 1
					};

					item2->ChangeAttribute(aiChangeProb);
				}
				else
				{
					if (item->GetVnum() == 71151 || item->GetVnum() == 76023)
					{
						if ((item2->GetType() == ITEM_WEAPON)
							|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_BODY))
						{
							bool bCanUse = true;
							for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
							{
								if (item2->GetLimitType(i) == LIMIT_LEVEL && item2->GetLimitValue(i) > 40)
								{
									bCanUse = false;
									break;
								}
							}
							if (false == bCanUse)
							{
								break;
							}
						}
						else
						{
							break;
						}
					}
					
					item2->ChangeAttribute();
				}

				{
					char buf[21];
					snprintf(buf, sizeof(buf), "%u", item2->GetID());
					LogManager::instance().ItemLog(this, item, "CHANGE_ATTRIBUTE", buf);
				}
				break;

			case USE_ADD_ATTRIBUTE:
				if (ITEM_COSTUME == item2->GetType())
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;396]");
					return false;
				}

				// else if ((item2->GetType() == ITEM_COSTUME && item2->GetSubType() == COSTUME_HAIR && item2->GetVnum() < 45007) || (item2->GetType() == ITEM_COSTUME && item2->GetSubType() == COSTUME_HAIR && item2->GetVnum() > 45999))
				if (item->IsPremiumBonusItem() && !item2->bonusOnlyByPremiumItem())
					return false;
				
				if (item2->GetAttributeSetIndex() == -1)
				{
					return false;
				}

				if (item2->GetAttributeCount() < 5)
				{
					char buf[21];
					snprintf(buf, sizeof(buf), "%u", item2->GetID());

					//if (number(1, 100) <= aiItemAttributeAddPercent[item2->GetAttributeCount()])
					for (int i =0 ; i<5; i++)
					{	
						item2->AddAttribute();

						int iAddedIdx = item2->GetAttributeCount() - 1;
						LogManager::instance().ItemLog(
								GetPlayerID(),
								item2->GetAttributeType(iAddedIdx),
								item2->GetAttributeValue(iAddedIdx),
								item->GetID(),
								"ADD_ATTRIBUTE_SUCCESS",
								buf,
								GetDesc()->GetHostName(),
								item->GetOriginalVnum()); // mzoe bd dzialc xd 
					}
					//xitem->SetCount(item->GetCount() - 1);
				}
				break;

#ifdef ENABLE_SPECIAL_BONUS
			case USE_ADD_ATTRIBUTE_SPECIAL_1:
			{
				const int CHANCE_ADDING = 25; // szansa na wejscie

				if (item2->GetAttributeCount() < 5)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;2246]");
					return false;
				}

				if ((item2->GetVnum() >= 0 && item2->GetVnum() <= 0) || 
					(item2->GetVnum() >= 0 && item2->GetVnum() <= 0) || 
					(item2->GetVnum() >= 0 && item2->GetVnum() <= 0) || 
					(item2->GetVnum() >= 0 && item2->GetVnum() <= 0) || 
					(item2->GetVnum() >= 0 && item2->GetVnum() <= 0) || 
					(item2->GetVnum() >= 0 && item2->GetVnum() <= 0)) // Zbroje
				{
					
					if (item->GetVnum() != 70602)
						return false;
					
					if (item2->GetAttributeValue(5) >= 15)
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;2247]");
						return false;
					}

					if (number(1, 100) > CHANCE_ADDING)
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;2248]"); 
					}
					else
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;2249]");
						item2->SetForceAttribute(5, 105, item2->GetAttributeValue(5) + 1);
					}

					item->SetCount(item->GetCount() - 1);

				}
				else if ((item2->GetVnum() >= 180 && item2->GetVnum() <= 189) || 
					(item2->GetVnum() >= 190 && item2->GetVnum() <= 199) || 
					(item2->GetVnum() >= 1130 && item2->GetVnum() <= 1139) || 
					(item2->GetVnum() >= 3160 && item2->GetVnum() <= 3169) || 
					(item2->GetVnum() >= 2170 && item2->GetVnum() <= 2179)  || 
					(item2->GetVnum() >= 5120 && item2->GetVnum() <= 5129)) // bronie
				{
					if (item->GetVnum() != 70603)
						return false;

					if (item2->GetAttributeValue(5) >= 15)
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;2247]");
						return false;
					}

					BYTE value = item2->GetAttributeValue(5);

					value += 1;

					if (number(1, 100) > CHANCE_ADDING)
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;2248]"); 
					}
					else
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;2249]");
						item2->SetForceAttribute(5, 63, value);
					}
				
					item->SetCount(item->GetCount() - 1);
				}
				else
				{
					return false;
				}
			}
			break;
#endif
			case USE_ADD_NEW_BONUS:
			{
#ifdef ENABLE_BELT_ATTRIBUTES
				if (item2->GetType() == ITEM_BELT) {
					if (item->GetVnum() == 79010) {
						if (item2->GetAttributeCount() < 4) {
							if (number(1, 100) > 80) {
								item2->AddBeltAttribute();
								ChatPacket(CHAT_TYPE_INFO, "[LS;2249]");
							} else {
								ChatPacket(CHAT_TYPE_INFO, "[LS;2248]");

							}
							
							item->SetCount(item->GetCount() - 1);
						} else {
							ChatPacket(CHAT_TYPE_INFO, "[LS;2247]");

						}
					} if (item->GetVnum() == 79011) {
						if (item2->GetAttributeCount() > 0) {
							item2->ChangeBeltAttribute();
							ChatPacket(CHAT_TYPE_INFO, "[LS;2249]");
							item->SetCount(item->GetCount() - 1);
						} else {
							ChatPacket(CHAT_TYPE_INFO, "[LS;2250]"); 
						}
					}
				}
#endif
#ifdef ENABLE_GLOVE_SYSTEM
				if (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_GLOVE) {
					if (item->GetVnum() == 79013) {
						if (item2->GetAttributeCount() < 5) {
							if (number(1, 100) <= 50) {
								item2->AddGloveAttribute();
								ChatPacket(CHAT_TYPE_INFO, "[LS;2251]"); 
								if (item2->GetSocket(0) > 0) {
									for (int i = 3; i < 5; i++) {
										item2->SetForceAttribute(i, item2->GetAttributeType(i), item2->GetAttributeValue(i) + (item2->GetAttributeValue(i) * 20) / 100);
									}
								}
							} else {
								ChatPacket(CHAT_TYPE_INFO, "[LS;2252]");
							}
							item->SetCount(item->GetCount() - 1);
						} else {
							ChatPacket(CHAT_TYPE_INFO, "[LS;2253]"); 
							return false;
						}
					}
					if (item->GetVnum() == 79012) {
						if (item2->GetSocket(0) == 0) {
							if (item2->GetAttributeCount() > 3) {
								item2->ChangeGloveAttribute();
								ChatPacket(CHAT_TYPE_INFO, "[LS;2254]"); 
								item->SetCount(item->GetCount() - 1);
							} else {
								ChatPacket(CHAT_TYPE_INFO, "[LS;2255]");
								return false;
							}
						} else {
							ChatPacket(CHAT_TYPE_INFO, "[LS;2256]"); 
							return false;
						}
					}
					if (item->GetVnum() == 203011)
					{
						if (item2->GetSocket(0) == 1) // Check if extra bonus is applied
						{
							// Remove the extra bonus by reversing the 20% increase
							for (int i = 3; i < 5; i++) 
							{
								if (item2->GetAttributeType(i) != 0) // Make sure attribute exists
								{
									//int original_value = (item2->GetAttributeValue(i) * 100) / 120; // Reverse the 20% increase
									item2->SetForceAttribute(i, 0, 0);
								}
							}
							
							// Reset the socket to indicate no extra bonus
							item2->SetSocket(0, 0);
							
							// Consume the cleaner item
							item->SetCount(item->GetCount() - 1);
							
							ChatPacket(CHAT_TYPE_INFO, "[LS;10060]"); // "Extra bonus removed successfully"
						} 
						else 
						{
							ChatPacket(CHAT_TYPE_INFO, "[LS;10061]"); // "This item doesn't have extra bonus"
							return false;
						}
					}
				}
	
#endif

				if (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_PENDANT_SOUL) {
					//if (item->GetVnum() == 10781) {
					//	if (item2->GetAttributeCount() < 5) {
					//		if (number(1, 100) <= 50) {
					//			item2->AddTalismanAttribute();
					//			ChatPacket(CHAT_TYPE_INFO, "[LS;2251]"); 
					//			if (item2->GetSocket(0) > 0) {
					//				for (int i = 3; i < 5; i++) {
					//					item2->SetForceAttribute(i, item2->GetAttributeType(i), item2->GetAttributeValue(i) + (item2->GetAttributeValue(i) * 20) / 100);
					//				}
					//			}
					//		} else {
					//			ChatPacket(CHAT_TYPE_INFO, "[LS;2252]");
					//		}
					//		item->SetCount(item->GetCount() - 1);
					//	} else {
					//		ChatPacket(CHAT_TYPE_INFO, "[LS;2253]"); 
					//		return false;
					//	}
					//}
					if (item->GetVnum() == 10781) {
						if (item2->GetVnum() >= 10860 && item2->GetVnum() <= 10880) {
							if (item2->GetAttributeCount() < 5)
							{
								item2->AddTalismanAttribute();
								ChatPacket(CHAT_TYPE_INFO, "[LS;10104]");
								item->SetCount(item->GetCount() - 1);
							}
							else
							{
								ChatPacket(CHAT_TYPE_INFO, "[LS;10105]"); 
								return false; // Just return, don't modify anything
							}
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, "[LS;10106]"); 
							return false; // Just return, don't modify anything
						}
					}
					if (item->GetVnum() == 10782)
					{
						if (item2->GetVnum() >= 10860 && item2->GetVnum() <= 10880) {
							if (item2->GetAttributeCount() > 0)
							{
								item2->ChangeTalismanAttribute();
								ChatPacket(CHAT_TYPE_INFO, "[LS;10107]"); 
								item->SetCount(item->GetCount() - 1);
							}
							else
							{
								ChatPacket(CHAT_TYPE_INFO, "[LS;10108]");
								return false;
							}
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, "[LS;10106]"); 
							return false; // Just return, don't modify anything
						}
					}
					if (item->GetVnum() == 10783) {
						if (item2->GetVnum() >= 10870 && item2->GetVnum() <= 10880) {
							if (item2->GetTalismanRareAttrCount() >= 3)
							{
								ChatPacket(CHAT_TYPE_INFO, "[LS;10109]"); // "Cannot add more rare attributes."
								return false;
							}
							
							if (item2->AddTalismanRareAttribute())
							{
								ChatPacket(CHAT_TYPE_INFO, "[LS;10110]"); 
								item->SetCount(item->GetCount() - 1);
							}
							else
							{
								ChatPacket(CHAT_TYPE_INFO, "[LS;10111]"); 
								return false; // Just return, don't modify anything
							}
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, "[LS;10112]"); 
							return false; // Just return, don't modify anything
						}
					}
					if (item->GetVnum() == 10784)
					{
						if (item2->GetVnum() >= 10870 && item2->GetVnum() <= 10880) {
							if (item2->GetTalismanRareAttrCount() == 0)
							{
								ChatPacket(CHAT_TYPE_INFO, "[LS;10113]"); // "No rare attributes to change."
								return false;
							}
							
							if (item2->ChangeTalismanRareAttribute())
							{
								ChatPacket(CHAT_TYPE_INFO, "[LS;10114]"); // "Rare attributes changed successfully."
								item->SetCount(item->GetCount() - 1);
							}
							else
							{
								ChatPacket(CHAT_TYPE_INFO, "[LS;10111]");
								return false;
							}
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, "[LS;10115]"); 
							return false; // Just return, don't modify anything
						}
					}
					//if (item->GetVnum() == 10785)
					//{
					//	if (item2->GetAttributeCount() == 5)
					//	{
					//		item2->AddTalismanAttribute();
					//		ChatPacket(CHAT_TYPE_INFO, "[LS;2251]");
					//		item->SetCount(item->GetCount() - 1);
					//	}
					//	else
					//	{
					//		ChatPacket(CHAT_TYPE_INFO, "[LS;2253]"); 
					//		return false;
					//	}
					//}
				}
			}
			break;

			case USE_ADD_EXTRA_BONUS:
			{
				if (item->GetVnum() == 34000) {
					if (item2->GetVnum() >= 85001 && item2->GetVnum() <= 85024) {
						if (item2->GetSocket(0) > 29) {
							if (item2->GetSocket(2) == 0) {
								item2->SetSocket(0, 45);
								item2->SetSocket(2, 1);
								ChatPacket(CHAT_TYPE_INFO, "[LS;2261]");
								item->SetCount(item->GetCount() - 1);
							} else {
								ChatPacket(CHAT_TYPE_INFO, "[LS;2262]"); 
								return false;
							}
						} else {
							ChatPacket(CHAT_TYPE_INFO, "[LS;2263]");
							return false;
						}
					}
					if (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_GLOVE) {
						if (item2->GetVnum() == 23005) {
							if (item2->GetAttributeCount() > 4) {
								if (item2->GetSocket(0) == 0) {
									item2->SetSocket(0, 1);
									item->SetCount(item->GetCount() - 1);
									for (int i = 3; i < 5; i++) {
										item2->SetForceAttribute(i, item2->GetAttributeType(i), item2->GetAttributeValue(i) + (item2->GetAttributeValue(i) * 20) / 100);
									}
									ChatPacket(CHAT_TYPE_INFO, "[LS;2264]");
								} else {
									ChatPacket(CHAT_TYPE_INFO, "[LS;2265]");
									return false;
								}
							} else {
								ChatPacket(CHAT_TYPE_INFO, "[LS;2266]");
								return false;
							}
						} else {
							ChatPacket(CHAT_TYPE_INFO, "[LS;2267]"); 
							return false;
						}
					}
					if (item2->GetType() == ITEM_NEW_PET_EQ) {
						if (item2->GetVnum() == 80109 || item2->GetVnum() == 80119 || item2->GetVnum() == 80129) {
							if (item2->GetSocket(0) == 0) {
								item2->SetSocket(0, 1);
								item->SetCount(item->GetCount() - 1);
								ChatPacket(CHAT_TYPE_INFO, "[LS;2268]");
							} else {
								ChatPacket(CHAT_TYPE_INFO, "[LS;2269]"); 
								return false;
							}
						} else {
							ChatPacket(CHAT_TYPE_INFO, "[LS;2270]");
							return false;
						}
					}
					if (item2->GetType() == ITEM_RUNE || item2->GetType() == ITEM_RUNE_RED || item2->GetType() == ITEM_RUNE_BLUE || item2->GetType() == ITEM_RUNE_GREEN || item2->GetType() == ITEM_RUNE_YELLOW || item2->GetType() == ITEM_RUNE_BLACK) {
						if (item2->GetVnum() == 55045 || item2->GetVnum() == 55055 || item2->GetVnum() == 55065 || item2->GetVnum() == 55075 || item2->GetVnum() == 55085 || item2->GetVnum() == 55095) {
							if (item2->GetSocket(0) == 0) {
								item2->SetSocket(0, 1);
								ChatPacket(CHAT_TYPE_INFO, "[LS;2271]");
								item->SetCount(item->GetCount() - 1);
							} else {
								ChatPacket(CHAT_TYPE_INFO, "[LS;2272]");
								return false;
							}
						} else {
							ChatPacket(CHAT_TYPE_INFO, "[LS;2273]");
							return false;
						}
					}
				}
			}
			break;


			case USE_ADD_IS_BONUS:
			{
				// 33060: Skill Costume - Strong Against Monsters [1-5]
				if (item->GetVnum() == 33060)
				{
					if (!(item2->GetType() == ITEM_SKILL_COSTUME))
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;396]");
						return false;
					}

					int foundIdx = -1, freeIdx = -1;
					for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
					{
						if (item2->GetAttributeType(i) == APPLY_ATTBONUS_MONSTER)
						{ foundIdx = i; break; }
						if (freeIdx == -1 && item2->GetAttributeType(i) == APPLY_NONE)
							freeIdx = i;
					}

					int targetIdx = (foundIdx != -1) ? foundIdx : freeIdx;
					if (targetIdx == -1)
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;2253]");
						return false;
					}

					{
						int roll = number(1, 100);
						int bonusVal;
						if (roll <= 15)       bonusVal = 1;
						else if (roll <= 30)  bonusVal = 2;
						else if (roll <= 45)  bonusVal = 3;
						else if (roll <= 58)  bonusVal = 4;
						else if (roll <= 70)  bonusVal = 5;
						else if (roll <= 80)  bonusVal = 6;
						else if (roll <= 88)  bonusVal = 7;
						else if (roll <= 93)  bonusVal = 8;
						else if (roll <= 97)  bonusVal = 9;
						else                  bonusVal = 10;
						item2->SetForceAttribute(targetIdx, APPLY_ATTBONUS_MONSTER, bonusVal);
					}
					item->SetCount(item->GetCount() - 1);
					break;
				}

				// 33061: Autobuff Costume - Buff NPC Intelligence [1-10]
				if (item->GetVnum() == 33061)
				{
					if (!(item2->GetType() == ITEM_COSTUME &&
						(item2->GetSubType() == BUFF_COSTUME_BODY ||
						 item2->GetSubType() == BUFF_COSTUME_HAIR ||
						 item2->GetSubType() == BUFF_COSTUME_WEAPON)))
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;396]");
						return false;
					}

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
					item2->SetSocket(0, (long long)number(1, 10));
#endif
					item->SetCount(item->GetCount() - 1);
					break;
				}

				if (item2->GetVnum() == 53012 || item2->GetVnum() == 45924 || item2->GetVnum() == 45925 || item2->GetVnum() == 41924 || item2->GetVnum() == 41925
				|| item2->GetVnum() == 40930 || item2->GetVnum() == 40931 || item2->GetVnum() == 40932 || item2->GetVnum() == 40933 || item2->GetVnum() == 40934
				|| item2->GetVnum() == 40935 || item2->GetVnum() == 40936 || item2->GetVnum() == 45966 || item2->GetVnum() == 45967 || item2->GetVnum() == 41970 || item2->GetVnum() == 41971
				|| item2->GetVnum() == 40738 || item2->GetVnum() == 40739 || item2->GetVnum() == 40740 || item2->GetVnum() == 40741 || item2->GetVnum() == 40742
				|| item2->GetVnum() == 40743 || item2->GetVnum() == 40744 || item2->GetVnum() == 85524 || item2->GetVnum() == 45968 || item2->GetVnum() == 45969 || item2->GetVnum() == 41972 || item2->GetVnum() == 41973
				|| item2->GetVnum() == 40745 || item2->GetVnum() == 40746 || item2->GetVnum() == 40747 || item2->GetVnum() == 40748 || item2->GetVnum() == 40749
				|| item2->GetVnum() == 40750 || item2->GetVnum() == 40751 || item2->GetVnum() == 85525
				|| item2->GetVnum() == 40682 || item2->GetVnum() == 40683 || item2->GetVnum() == 40684 || item2->GetVnum() == 40685 || item2->GetVnum() == 40686 || item2->GetVnum() == 40687
				|| item2->GetVnum() == 41726 || item2->GetVnum() == 41727 || item2->GetVnum() == 46022 || item2->GetVnum() == 46023 || item2->GetVnum() == 85552 || item2->GetVnum() == 85553
				|| item2->GetVnum() == 41756 || item2->GetVnum() == 41757 || item2->GetVnum() == 46052 || item2->GetVnum() == 46053 || item2->GetVnum() == 85568 || item2->GetVnum() == 40398
				|| item2->GetVnum() == 40399 || item2->GetVnum() == 40400 || item2->GetVnum() == 40401 || item2->GetVnum() == 40402 || item2->GetVnum() == 40403 || item2->GetVnum() == 40404
				|| item2->GetVnum() == 40412 || item2->GetVnum() == 40413 || item2->GetVnum() == 40414 || item2->GetVnum() == 40415 || item2->GetVnum() == 40416 || item2->GetVnum() == 40417
				|| item2->GetVnum() == 40418 || item2->GetVnum() == 40419 || item2->GetVnum() == 40420 || item2->GetVnum() == 40421 || item2->GetVnum() == 40422 || item2->GetVnum() == 40423
				|| item2->GetVnum() == 40424 || item2->GetVnum() == 40425 || item2->GetVnum() == 41760 || item2->GetVnum() == 41761 || item2->GetVnum() == 41762 || item2->GetVnum() == 41763
				|| item2->GetVnum() == 46056 || item2->GetVnum() == 46057 || item2->GetVnum() == 46058 || item2->GetVnum() == 46059 || item2->GetVnum() == 85570 || item2->GetVnum() == 85571
				|| item2->GetVnum() == 53933 || item2->GetVnum() == 53935 || item2->GetVnum() == 53936) {
					return false;
				}

				if (CItemVnumHelper::IsBlockedCostumes(item2->GetVnum())) return false;

				LPITEM item2;
				if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
					return false;

				if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
					return false;

				if (!item2->IsItemShopBonusExpireItem())
				{
					ChatPacket(CHAT_TYPE_INFO, "Wrong type");
					return false;
				}

				if (item2->GetSocket(2) != 0 && (item->GetVnum() == item2->GetSocket(2)))
				{
					ChatPacket(CHAT_TYPE_INFO, "This item already have a bonus");
					return false;
				}

				if (item->GetValue(0) == COSTUME_BODY && !(item2->GetType() == ITEM_COSTUME && item2->GetSubType() == COSTUME_BODY))
				{
					return false;
				}

				if (item->GetValue(0) == COSTUME_HAIR && !(item2->GetType() == ITEM_COSTUME && item2->GetSubType() == COSTUME_HAIR))
				{
					return false;
				}

				if (item->GetValue(0) == COSTUME_WEAPON && !(item2->GetType() == ITEM_COSTUME && item2->GetSubType() == COSTUME_WEAPON))
				{
					return false;
				}

				if (item->GetValue(0) == COSTUME_STOLE && !(item2->GetType() == ITEM_COSTUME && item2->GetSubType() == COSTUME_STOLE))
				{
					return false;
				}

				if (item->GetValue(0) == 10 && !(item2->GetType() == ITEM_NEW_PET))
				{
					return false;
				}

				item->SetCount(item->GetCount() - 1);
				item2->SetSocket(1, get_global_time() + item->GetValue(2));
				item2->SetSocket(2, item->GetVnum());
				item2->StartItemshopBonusTimeExpireEvent();
			}
			break;

			case USE_ADD_ACCESSORY_SOCKET:
			{
				char buf[21];
				snprintf(buf, sizeof(buf), "%u", item2->GetID());

				if (item2->IsAccessoryForSocket())
				{
					if (item2->GetAccessorySocketMaxGrade() < ITEM_ACCESSORY_SOCKET_MAX_NUM)
					{
#ifdef ENABLE_ADDSTONE_FAILURE
						if (number(1, 100) <= 50)
#else
						if (1)
#endif
						{
							item2->SetAccessorySocketMaxGrade(item2->GetAccessorySocketMaxGrade() + 1);
							ChatPacket(CHAT_TYPE_INFO, "[LS;406]");
							LogManager::instance().ItemLog(this, item, "ADD_SOCKET_SUCCESS", buf);
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, "[LS;407]");
							LogManager::instance().ItemLog(this, item, "ADD_SOCKET_FAIL", buf);
						}

						item->SetCount(item->GetCount() - 1);
					}
					else
					{
						ChatPacket(CHAT_TYPE_INFO, "[LS;408]");
					}
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;409]");
				}
			}
			break;

			case USE_PUT_INTO_BELT_SOCKET:
			case USE_PUT_INTO_ACCESSORY_SOCKET:
				if (item2->IsAccessoryForSocket() && item->CanPutInto(item2))
				{
					char buf[21];
					snprintf(buf, sizeof(buf), "%u", item2->GetID());

					if (item2->GetAccessorySocketGrade() < item2->GetAccessorySocketMaxGrade())
					{
						if (number(1, 100) <= aiAccessorySocketPutPct[item2->GetAccessorySocketGrade()])
						{
							item2->SetAccessorySocketGrade(item2->GetAccessorySocketGrade() + 1);
							ChatPacket(CHAT_TYPE_INFO, "[LS;410]");
							LogManager::instance().ItemLog(this, item, "PUT_SOCKET_SUCCESS", buf);
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, "[LS;411]");
							LogManager::instance().ItemLog(this, item, "PUT_SOCKET_FAIL", buf);
						}

						item->SetCount(item->GetCount() - 1);
					}
					else
					{
						// For belt melt (50649): if socket slots can still be added, add one instead of failing
						if (item->GetVnum() == 50649 && item2->GetType() == ITEM_BELT &&
							item2->GetAccessorySocketMaxGrade() < ITEM_ACCESSORY_SOCKET_MAX_NUM)
						{
							char buf2[21];
							snprintf(buf2, sizeof(buf2), "%u", item2->GetID());
#ifdef ENABLE_ADDSTONE_FAILURE
							if (number(1, 100) <= 50)
#else
							if (1)
#endif
							{
								item2->SetAccessorySocketMaxGrade(item2->GetAccessorySocketMaxGrade() + 1);
								ChatPacket(CHAT_TYPE_INFO, "[LS;406]");
								LogManager::instance().ItemLog(this, item, "ADD_SOCKET_SUCCESS", buf2);
							}
							else
							{
								ChatPacket(CHAT_TYPE_INFO, "[LS;407]");
								LogManager::instance().ItemLog(this, item, "ADD_SOCKET_FAIL", buf2);
							}
							item->SetCount(item->GetCount() - 1);
						}
						else if (item2->GetAccessorySocketMaxGrade() == 0)
							ChatPacket(CHAT_TYPE_INFO, "[LS;412]");
						else if (item2->GetAccessorySocketMaxGrade() < ITEM_ACCESSORY_SOCKET_MAX_NUM)
						{
							ChatPacket(CHAT_TYPE_INFO, "[LS;413]");
							ChatPacket(CHAT_TYPE_INFO, "[LS;415]");
						}
						else
							ChatPacket(CHAT_TYPE_INFO, "[LS;416]");
					}
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;417]");
				}
				break;
			}
			if (item2->IsEquipped())
			{
				BuffOnAttr_AddBuffsFromItem(item2);
			}
		}
		break;
		//  END_OF_ACCESSORY_REFINE & END_OF_ADD_ATTRIBUTES & END_OF_CHANGE_ATTRIBUTES

		case USE_BAIT:
		{
			if (m_pkFishingEvent)
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;418]");
				return false;
			}

			LPITEM weapon = GetWear(WEAR_WEAPON);

			if (!weapon || weapon->GetType() != ITEM_ROD)
				return false;

			if (weapon->GetSocket(2))
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;419;%s]", item->GetName());
			}
			else
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;420;%s]", item->GetName());
			}

			weapon->SetSocket(2, item->GetValue(0));
			item->SetCount(item->GetCount() - 1);
		}
		break;

		case USE_MOVE:
		case USE_TREASURE_BOX:
		case USE_MONEYBAG:
			break;

#ifdef __ENABLE_COLLECTIONS_SYSTEM__
		case USE_COLLECTION_SCROLL:
		{
			if (!CanHandleItem() || item->IsExchanging())
			{
				return false;
			}

			if (GetQuestFlag("collection.percent_up"))
			{
				ChatPacket(CHAT_TYPE_INFO, "You already increased your perecent.");
				return false;
			}

			SetQuestFlag("collection.percent_up", 1);
			ChatPacket(CHAT_TYPE_COMMAND, "RECV_CollectionIncrease %d", 1);
			item->SetCount(item->GetCount() - 1);
		}
		break;
#endif

		case USE_AFFECT:
		{
	        const auto affectType = item->GetValue(0);

	        const auto affectDuration = item->GetValue(3);

#ifdef __PREMIUM_PRIVATE_SHOP__
			if (affectType == AFFECT_PREMIUM_PRIVATE_SHOP)
			{
				if (SetPremiumPrivateShopBonus(affectDuration))
					item->SetCount(item->GetCount() - 1);
				return true;
			}
#endif

			if (FindAffect(item->GetValue(0), aApplyInfo[item->GetValue(1)].bPointType))
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;99]");
			}
			else
			{
				AddAffect(item->GetValue(0), aApplyInfo[item->GetValue(1)].bPointType, item->GetValue(2), 0, item->GetValue(3), 0, false);
				if (item->GetVnum() == 39024 || item->GetVnum() == 39025 || item->GetVnum() == 71031)
				{
				}
				else {
					item->SetCount(item->GetCount() - 1);
				}
			}
		}
		break;

		case USE_CREATE_STONE:
			AutoGiveItem(number(28000, 28013));
			item->SetCount(item->GetCount() - 1);
			break;

#ifdef __ENABLE_POLYMORPH_SYSTEM__
		case USE_POLY_SKIN:
		{
			if (!CanWarp() || !CanHandleItem())
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;2356]");
				return false;
			}

			uint8_t bSkinIndex = static_cast<uint8_t>(item->GetValue(0));
			if (!CPolymorphMgr::instance().UnlockSkin(this, bSkinIndex))
			{
				return false;
			}

			ChatPacket(CHAT_TYPE_INFO, "[LS;2430]");
			item->SetCount(item->GetCount() - 1);
			return true;
		}
		break;
#endif

		case USE_RECIPE:
		{
			LPITEM pSource1 = FindSpecifyItem(item->GetValue(1));
			DWORD dwSourceCount1 = item->GetValue(2);

			LPITEM pSource2 = FindSpecifyItem(item->GetValue(3));
			DWORD dwSourceCount2 = item->GetValue(4);

			if (dwSourceCount1 != 0)
			{
				if (pSource1 == NULL)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;421]");
					return false;
				}
			}

			if (dwSourceCount2 != 0)
			{
				if (pSource2 == NULL)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;421]");
					return false;
				}
			}

			if (pSource1 != NULL)
			{
				if (pSource1->GetCount() < dwSourceCount1)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;422;%s]", pSource1->GetName());
					return false;
				}

				pSource1->SetCount(pSource1->GetCount() - dwSourceCount1);
			}

			if (pSource2 != NULL)
			{
				if (pSource2->GetCount() < dwSourceCount2)
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;422;%s]", pSource2->GetName());
					return false;
				}

				pSource2->SetCount(pSource2->GetCount() - dwSourceCount2);
			}

			LPITEM pBottle = FindSpecifyItem(50901);

			if (!pBottle || pBottle->GetCount() < 1)
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;423]");
				return false;
			}

			pBottle->SetCount(pBottle->GetCount() - 1);

			if (number(1, 100) > item->GetValue(5))
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;424]");
				return false;
			}

			AutoGiveItem(item->GetValue(0));
		}
		break;
		}
	}
	break;

	case ITEM_METIN:
	{
		LPITEM item2;

		if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
			return false;

		if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
			return false;

		if (item2->GetType() == ITEM_PICK) return false;
		if (item2->GetType() == ITEM_ROD) return false;

		int i;

		for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			DWORD dwVnum;

			if ((dwVnum = item2->GetSocket(i)) <= 2)
				continue;

			TItemTable* p = ITEM_MANAGER::instance().GetTable(dwVnum);

			if (!p)
				continue;

			if (item->GetValue(5) == p->alValues[5])
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;426]");
				return false;
			}
		}

		if (item2->GetType() == ITEM_ARMOR)
		{
			if (!IS_SET(item->GetWearFlag(), WEARABLE_BODY) || !IS_SET(item2->GetWearFlag(), WEARABLE_BODY))
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;427]");
				return false;
			}
		}
		else if (item2->GetType() == ITEM_WEAPON)
		{
			if (!IS_SET(item->GetWearFlag(), WEARABLE_WEAPON))
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;428]");
				return false;
			}
		}
		else
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;431]");
			return false;
		}

		for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
			if (item2->GetSocket(i) >= 1 && item2->GetSocket(i) <= 2 && item2->GetSocket(i) >= item->GetValue(2))
			{
        		int successRate = 45;
				
				if (item->GetVnum() >= 28930 && item->GetVnum() <= 28943)
				{
					successRate = 100;
				}

#ifdef ENABLE_ADDSTONE_FAILURE
        		if (number(1, 100) <= successRate)
#else
        		if (1)
#endif
        		{
					ChatPacket(CHAT_TYPE_INFO, "[LS;429]");
					item2->SetSocket(i, item->GetVnum());
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, "[LS;430]");
#ifndef ENABLE_NEW_STONE_DETACH
					item2->SetSocket(i, ITEM_BROKEN_METIN_VNUM);
#endif
				}

				LogManager::instance().ItemLog(this, item2, "SOCKET", item->GetName());
				item->SetCount(item->GetCount() - 1);
				// ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (METIN)");
				break;
			}

		if (i == ITEM_SOCKET_MAX_NUM)
			ChatPacket(CHAT_TYPE_INFO, "[LS;431]");
	}
	break;

	case ITEM_AUTOUSE:
	case ITEM_MATERIAL:
	case ITEM_SPECIAL:
	case ITEM_TOOL:
	case ITEM_LOTTERY:
		break;

	case ITEM_TOTEM:
	{
		if (!item->IsEquipped())
			EquipItem(item);
	}
	break;

#ifdef ENABLE_EXTENDED_BLEND
	case ITEM_BLEND:
		if (Blend_Item_find(item->GetVnum()))
		{
			int     affect_type = GetAffectType(item);
			int     apply_type = aApplyInfo[item->GetValue(0)].bPointType;
			int     apply_value = item->GetSocket(0);
			bool    active_blend = (item->GetSocket(1) == 1) ? true : false;
			int     apply_duration = item->GetSocket(2);
	
// Items 40024/40025 (BLEND) may coexist with 50821/50822 (WATER) despite sharing apply_type
			const DWORD dwItemVnum = item->GetVnum();
			const bool bSkipWaterConflict = (dwItemVnum == 40024 || dwItemVnum == 40025);
			const bool bSkipBlendConflict = (dwItemVnum == 50821 || dwItemVnum == 50822);

			if (!active_blend && (
				((affect_type == AFFECT_BLEND || affect_type == AFFECT_BLEND_EX) &&
					(FindAffect(AFFECT_BLEND, apply_type) || FindAffect(AFFECT_BLEND_EX, apply_type) ||
					(!bSkipWaterConflict && FindAffect(AFFECT_WATER, apply_type)) || FindAffect(AFFECT_MALL_EX, apply_type)))
				|| ((affect_type == AFFECT_WATER) &&
					(FindAffect(AFFECT_EXP_BONUS_EURO_FREE, apply_type) || FindAffect(AFFECT_WATER, apply_type) ||
					(!bSkipBlendConflict && FindAffect(AFFECT_BLEND, apply_type)) || FindAffect(AFFECT_BLEND_EX, apply_type)))
				|| ((affect_type == AFFECT_MALL_EX) &&
					(FindAffect(AFFECT_MALL, apply_type) || FindAffect(AFFECT_MALL_EX, apply_type) ||
					((dwItemVnum == 71044 || dwItemVnum == 71045 ||
					  dwItemVnum == 71027 || dwItemVnum == 71028 || dwItemVnum == 71029 || dwItemVnum == 71030) && FindAffect(AFFECT_BLEND, apply_type))))
			))
			{
				ChatPacket(CHAT_TYPE_INFO, "This effect is already activated.");
				return false;
			}
	
			if ((affect_type == AFFECT_BLEND_EX || affect_type == AFFECT_WATER || affect_type == AFFECT_MALL_EX))
			{
				if (active_blend)
				{
					RemoveAffect(FindAffect(affect_type, apply_type));
					item->Lock(false);
					item->SetSocket(1, 0);
				}
				else
				{
					AddAffect(affect_type, apply_type, apply_value, 0, INFINITE_AFFECT_DURATION, 0, false);
					item->Lock(true);
					item->SetSocket(1, 1);
				}
			}
			else
			{
				AddAffect(affect_type, apply_type, apply_value, 0, apply_duration, 0, false);
				if (item->GetVnum() == 50821 || item->GetVnum() == 50822 || item->GetVnum() == 50823
				|| item->GetVnum() == 50824 || item->GetVnum() == 50825 || item->GetVnum() == 50826) {
					return false;
				} else {
					item->SetCount(item->GetCount() - 1);
				}
			}
		}
		break;
#else
	// Your old case here
	case ITEM_BLEND:

		sys_log(0, "ITEM_BLEND!!");
		if (Blend_Item_find(item->GetVnum()))
		{
			int		affect_type = AFFECT_BLEND;
			int		apply_type = aApplyInfo[item->GetSocket(0)].bPointType;
			int		apply_value = item->GetSocket(1);
			int		apply_duration = item->GetSocket(2);

			if (FindAffect(affect_type, apply_type))
			{
				ChatPacket(CHAT_TYPE_INFO, "This effect is already activated.");
			}
			else
			{
				// if (FindAffect(AFFECT_EXP_BONUS_EURO_FREE, POINT_RESIST_MAGIC))
				// {
					// ChatPacket(CHAT_TYPE_INFO, "This effect is already activated.");
				// }
				// else
				// {
				AddAffect(affect_type, apply_type, apply_value, 0, apply_duration, 0, false);
				item->SetCount(item->GetCount() - 1);
				// }
			}
		}
		break;
#endif
	case ITEM_EXTRACT:
	{
		LPITEM pDestItem = GetItem(DestCell);
		if (NULL == pDestItem)
		{
			return false;
		}
		switch (item->GetSubType())
		{
		case EXTRACT_DRAGON_SOUL:
			if (pDestItem->IsDragonSoul())
			{
				return DSManager::instance().PullOut(this, NPOS, pDestItem, item);
			}
			return false;
		case EXTRACT_DRAGON_HEART:
			if (pDestItem->IsDragonSoul())
			{
				return DSManager::instance().ExtractDragonHeart(this, pDestItem, item);
			}
			return false;
		default:
			return false;
		}
	}
	break;

#ifdef __ENABLE_ITEM_TOGGLE__
	case ITEM_TOGGLE:
		if (!item->OnUse()) {
			return false;
		}
		break;
#endif

	case ITEM_NONE:
		sys_err("Item type NONE %s", item->GetName());
		break;

	default:
		sys_log(0, "UseItemEx: Unknown type %s %d", item->GetName(), item->GetType());
		return false;
	}

	return true;
}

int g_nPortalLimitTime = 10;

bool CHARACTER::UseItem(TItemPos Cell, TItemPos DestCell)
{
	WORD wCell = Cell.cell;
	BYTE window_type = Cell.window_type;
	//WORD wDestCell = DestCell.cell;
	//BYTE bDestInven = DestCell.window_type;
	LPITEM item;

	if (!CanHandleItem())
		return false;

	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
		return false;

	//We don't want to use it if we are dragging it over another item of the same type...
	CItem* destItem = GetItem(DestCell);
	if (destItem && item != destItem && destItem->IsStackable() && !IS_SET(destItem->GetAntiFlag(), ITEM_ANTIFLAG_STACK) && destItem->GetVnum() == item->GetVnum())
	{
		if (MoveItem(Cell, DestCell, 0))
			return false;
	}

	sys_log(0, "%s: USE_ITEM %s (inven %d, cell: %d)", GetName(), item->GetName(), window_type, wCell);

	if (item->IsExchanging())
		return false;

#ifdef ENABLE_SWITCHBOT
	if (Cell.IsSwitchbotPosition())
	{
		CSwitchbot* pkSwitchbot = CSwitchbotManager::Instance().FindSwitchbot(GetPlayerID());
		if (pkSwitchbot && pkSwitchbot->IsActive(Cell.cell))
		{
			return false;
		}

		int iEmptyCell = GetEmptyInventory(item->GetSize());

		if (iEmptyCell == -1)
		{
			ChatPacket(CHAT_TYPE_INFO, "Cannot remove item from switchbot. Inventory is full.");
			return false;
		}

		MoveItem(Cell, TItemPos(INVENTORY, iEmptyCell), item->GetCount());
		return true;
	}
	// else
	// {
		// if(item->GetSubType() != USE_CHANGE_ATTRIBUTE)
		// {
			// if (thecore_pulse() > use_item_anti_flood_pulse() + PASSES_PER_SEC(1))
			// {
				// set_use_item_anti_flood_count(0);
				// set_use_item_anti_flood_pulse(thecore_pulse());
			// }

			// if (increase_use_item_anti_flood_count() >= 10)
			// {
				// GetDesc()->DelayedDisconnect(0);
				// return false;
			// }
		// }
	// }
#endif
	if (Cell.IsBuffEquipmentPosition())
	{
	}
	else
		
		if (!item->CanUsedBy(this))
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;1004]");
			return false;
		}

	if (IsStun())
		return false;

	if (false == FN_check_item_sex(this, item))
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;1005]");
		return false;
	}

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	if (Cell.IsBuffEquipmentPosition())
	{
		int iEmptyCell = GetEmptyInventory(item->GetSize());

		if (iEmptyCell == -1)
		{
			ChatPacket(CHAT_TYPE_INFO, "Inventory is full.");
			return false;
		}

		MoveItem(Cell, TItemPos(INVENTORY, iEmptyCell), item->GetCount());
		return true;
	}
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
	if (IsEditingPrivateShop())
	{
		ChatPacket(CHAT_TYPE_INFO, "You cannot use items while editing your personal shop.");
		return false;
	}
#endif

	//PREVENT_TRADE_WINDOW
	if (IS_SUMMON_ITEM(item->GetVnum()))
	{
		if (false == IS_SUMMONABLE_ZONE(GetMapIndex()))
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;432]");
			return false;
		}

		if (CThreeWayWar::instance().IsThreeWayWarMapIndex(GetMapIndex()))
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;433]");
			return false;
		}
		int iPulse = thecore_pulse();

		if (iPulse - GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;434;%d]", g_nPortalLimitTime);

			if (test_server)
				ChatPacket(CHAT_TYPE_INFO, "[TestOnly]Pulse %d LoadTime %d PASS %d", iPulse, GetSafeboxLoadTime(), PASSES_PER_SEC(g_nPortalLimitTime));
			return false;
		}


		if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen()
#ifdef __ENABLE_POLYMORPH_SYSTEM__
			|| IsPolyShopping()
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
			|| GetOfflineShopOwner()
#endif
		)
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;435]");
			return false;
		}

		{
			if (iPulse - GetRefineTime() < PASSES_PER_SEC(g_nPortalLimitTime))
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;437;%d]", g_nPortalLimitTime);
				return false;
			}
		}

		{
			if (iPulse - GetMyShopTime() < PASSES_PER_SEC(g_nPortalLimitTime))
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;438;%d]", g_nPortalLimitTime);
				return false;
			}

		}

		if (item->GetVnum() != 70302)
		{
			PIXEL_POSITION posWarp;

			int x = 0;
			int y = 0;

			double nDist = 0;
			const double nDistant = 5000.0;

			if (item->GetVnum() == 22010)
			{
				x = item->GetSocket(0) - GetX();
				y = item->GetSocket(1) - GetY();
			}

			else if (item->GetVnum() == 22000)
			{
				SECTREE_MANAGER::instance().GetRecallPositionByEmpire(GetMapIndex(), GetEmpire(), posWarp);

				if (item->GetSocket(0) == 0)
				{
					x = posWarp.x - GetX();
					y = posWarp.y - GetY();
				}
				else
				{
					x = item->GetSocket(0) - GetX();
					y = item->GetSocket(1) - GetY();
				}
			}

			nDist = sqrt(pow((float)x, 2) + pow((float)y, 2));

			if (nDistant > nDist)
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;439]");
				if (test_server)
					ChatPacket(CHAT_TYPE_INFO, "PossibleDistant %f nNowDist %f", nDistant, nDist);
				return false;
			}
		}

		//PREVENT_PORTAL_AFTER_EXCHANGE

		if (iPulse - GetExchangeTime() < PASSES_PER_SEC(g_nPortalLimitTime))
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;440;%d]", g_nPortalLimitTime);
			return false;
		}
		//END_PREVENT_PORTAL_AFTER_EXCHANGE

	}

	if ((item->GetVnum() == 50200) || (item->GetVnum() == 71049))
	{
		if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen()
#ifdef __ENABLE_POLYMORPH_SYSTEM__
			|| IsPolyShopping()
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
			|| GetOfflineShopOwner()
#endif
		)
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;441]");
			return false;
		}

	}

	//END_PREVENT_TRADE_WINDOW

#ifdef ENABLE_EXTENDED_BLEND
	// Again
	// This is unnecessary if you aren't going to use both items(I mean use only the blends for an example).
	if (item->GetType() == ITEM_USE && (item->GetSubType() == USE_AFFECT || item->GetSubType() == USE_ABILITY_UP))
	{
		int apply_type;
		if ((item->GetType() == ITEM_USE && item->GetSubType() == USE_AFFECT))
			apply_type = aApplyInfo[item->GetValue(1)].bPointType;
		else
			apply_type = aApplyInfo[item->GetValue(0)].bPointType;

		if ((item->IsWaterItem() && (FindAffect(AFFECT_WATER, apply_type) || FindAffect(AFFECT_BLEND, apply_type) || FindAffect(AFFECT_BLEND_EX, apply_type))))
		{
			ChatPacket(CHAT_TYPE_INFO, "This effect is already activated.");
			return false;
		}

		if ((item->IsDragonGodItem() && FindAffect(AFFECT_MALL_EX, apply_type)))
		{
			ChatPacket(CHAT_TYPE_INFO, "This effect is already activated.");
			return false;
		}
	}
#endif

	// @fixme150 BEGIN
	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;2413]");
		return false;
	}
	// @fixme150 END

	if (IS_SET(item->GetFlag(), ITEM_FLAG_LOG))
	{
		DWORD vid = item->GetVID();
		DWORD oldCount = item->GetCount();
		DWORD vnum = item->GetVnum();

		char hint[ITEM_NAME_MAX_LEN + 32 + 1];
		int len = snprintf(hint, sizeof(hint) - 32, "%s", item->GetName());

		if (len < 0 || len >= (int)sizeof(hint) - 32)
			len = (sizeof(hint) - 32) - 1;

		bool ret = UseItemEx(item, DestCell);
		if (NULL == ITEM_MANAGER::instance().FindByVID(vid))
		{
			LogManager::instance().ItemLog(this, vid, vnum, "REMOVE", hint);
		}
		else if (oldCount != item->GetCount())
		{
			snprintf(hint + len, sizeof(hint) - len, " %u", oldCount - 1);
			LogManager::instance().ItemLog(this, vid, vnum, "USE_ITEM", hint);
		}
		return (ret);
	}
	else
		return UseItemEx(item, DestCell);
}

#ifdef __EXTENDED_ITEM_COUNT__
bool CHARACTER::DropItem(TItemPos Cell, uint16_t bCount)
#else
bool CHARACTER::DropItem(TItemPos Cell, BYTE bCount)
#endif
{
	LPITEM item = NULL;

	if (!CanHandleItem())
	{
		if (NULL != DragonSoul_RefineWindow_GetOpener())
			ChatPacket(CHAT_TYPE_INFO, "[LS;1069]");
		return false;
	}
#ifdef ENABLE_NEWSTUFF
	if (g_ItemDropTimeLimitValue && !PulseManager::Instance().IncreaseClock(GetPlayerID(), ePulse::ItemDrop, std::chrono::milliseconds(g_ItemDropTimeLimitValue)))
	{
		return false;
	}
#endif
	if (IsDead())
		return false;

	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
		return false;

	if (item->IsExchanging())
		return false;

	if (true == item->isLocked())
		return false;

	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
		return false;

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_DROP | ITEM_ANTIFLAG_GIVE))
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;442]");
		return false;
	}

	if (bCount == 0 || bCount > item->GetCount())
#ifdef __EXTENDED_ITEM_COUNT__
		bCount = (uint16_t)item->GetCount();
#else
		bCount = (BYTE)item->GetCount();
#endif

	SyncQuickslot(QUICKSLOT_TYPE_ITEM, Cell.cell, 255);

	LPITEM pkItemToDrop;

	if (bCount == item->GetCount())
	{
		item->RemoveFromCharacter();
		pkItemToDrop = item;
	}
	else
	{
		if (bCount == 0)
		{
			if (test_server)
				sys_log(0, "[DROP_ITEM] drop item count == 0");
			return false;
		}

		item->SetCount(item->GetCount() - bCount);
		ITEM_MANAGER::instance().FlushDelayedSave(item);

		pkItemToDrop = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), bCount);

		// copy item socket -- by mhh
		FN_copy_item_socket(pkItemToDrop, item);

		char szBuf[51 + 1];
		snprintf(szBuf, sizeof(szBuf), "%u %u", pkItemToDrop->GetID(), pkItemToDrop->GetCount());
		LogManager::instance().ItemLog(this, item, "ITEM_SPLIT", szBuf);
	}

	PIXEL_POSITION pxPos = GetXYZ();

	if (pkItemToDrop->AddToGround(GetMapIndex(), pxPos))
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;443]");
#ifdef ENABLE_NEWSTUFF
		pkItemToDrop->StartDestroyEvent(10);
#else
		pkItemToDrop->StartDestroyEvent();
#endif

		ITEM_MANAGER::instance().FlushDelayedSave(pkItemToDrop);

		char szHint[32 + 1];
		snprintf(szHint, sizeof(szHint), "%s %u %u", pkItemToDrop->GetName(), pkItemToDrop->GetCount(), pkItemToDrop->GetOriginalVnum());
		LogManager::instance().ItemLog(this, pkItemToDrop, "DROP", szHint);
		//Motion(MOTION_PICKUP);
	}

	return true;
}

bool CHARACTER::DropGold(int gold)
{
	return false;
}

#ifdef ENABLE_CHEQUE_SYSTEM
bool CHARACTER::DropCheque(int64_t cheque)
{
	if (cheque <= 0 || (int64_t)cheque > GetCheque())
		return false;

	if (!CanHandleItem())
		return false;

	if (0 != 30)
	{
		if (get_dword_time() < m_dwLastChequeDropTime + 30)
		{
			return false;
		}
	}

	m_dwLastChequeDropTime = get_dword_time();

	LPITEM item = ITEM_MANAGER::instance().CreateItem(80020, cheque);

	if (item)
	{
		PIXEL_POSITION pos = GetXYZ();

		if (item->AddToGround(GetMapIndex(), pos))
		{
			PointChange(POINT_CHEQUE, -cheque, true);

			if (cheque > 1000)
				LogManager::instance().CharLog(this, cheque, "DROP_CHEQUE", "");


			item->StartDestroyEvent(60);
		}

		Save();
		return true;
	}

	return false;
}
#endif

#ifdef __EXTENDED_ITEM_COUNT__
bool CHARACTER::MoveItem(TItemPos Cell, TItemPos DestCell, uint16_t count)
#else
bool CHARACTER::MoveItem(TItemPos Cell, TItemPos DestCell, BYTE count)
#endif
{
	{
        TItemPos tempSrc = Cell;
        TItemPos tempDst = DestCell;
        if (tempSrc.window_type == EQUIPMENT)
            tempSrc.window_type = INVENTORY;

        if (tempDst.window_type == EQUIPMENT)
            tempDst.window_type = INVENTORY;

        if (tempSrc == tempDst)
            return false;
    }
	
	if (Cell == DestCell) // @fixme196
		return false;

	if (!IsValidItemPosition(Cell))
		return false;

	LPITEM item = NULL;
	if (!(item = GetItem(Cell)))
		return false;

	if (item->IsExchanging())
		return false;

	if (item->GetCount() < count)
		return false;

// #ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	// if (INVENTORY == Cell.window_type && Cell.cell >= INVENTORY_AND_EQUIP_SLOT_MAX && IS_SET(item->GetFlag(), ITEM_FLAG_IRREMOVABLE))
		// return false;
// #else
	if (INVENTORY == Cell.window_type && Cell.cell >= INVENTORY_MAX_NUM && IS_SET(item->GetFlag(), ITEM_FLAG_IRREMOVABLE))
		return false;
// #endif

	if (true == item->isLocked())
		return false;

	if (!IsValidItemPosition(DestCell))
	{
		return false;
	}

	if (!CanHandleItem())
	{
		if (NULL != DragonSoul_RefineWindow_GetOpener())
			ChatPacket(CHAT_TYPE_INFO, "[LS;1069]");
		return false;
	}

#ifdef __PREMIUM_PRIVATE_SHOP__
	if (IsEditingPrivateShop())
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;2274]");
		return false;
	}
#endif

	if (DestCell.IsBeltInventoryPosition() && false == CBeltInventoryHelper::CanMoveIntoBeltInventory(item))
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;1097]");
		return false;
	}
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	if (DestCell.IsBuffEquipmentPosition())
	{
		if (DestCell.IsBuffEquipmentPosition()) {
			if (!EquipBuffItem(DestCell.cell, item)) {
				return false;
			}
		}
		else if (Cell.IsBuffEquipmentPosition() && INVENTORY == DestCell.window_type && DestCell.cell >= INVENTORY_MAX_NUM) {
			if (!UnequipBuffItem(Cell.cell, item)) {
				return false;
			}
		}
	}
#endif
#ifdef ENABLE_SWITCHBOT
	if (Cell.IsSwitchbotPosition() && CSwitchbotManager::Instance().IsActive(GetPlayerID(), Cell.cell))
	{
		ChatPacket(CHAT_TYPE_INFO, "Cannot move active switchbot item.");
		return false;
	}

	if ((DestCell.IsSwitchbotPosition() && item->IsEquipped()) || (Cell.IsSwitchbotPosition() && DestCell.IsEquipPosition()))
	{
		return false;
	}

	if (DestCell.IsSwitchbotPosition() && !SwitchbotHelper::IsValidItem(item))
	{
		ChatPacket(CHAT_TYPE_INFO, "Invalid item type for switchbot.");
		return false;
	}

	if (Cell.IsSwitchbotPosition() && DestCell.IsEquipPosition())
	{
		ChatPacket(CHAT_TYPE_INFO, "Cannot equip items directly from switchbot.");
		return false;
	}

	if (DestCell.IsSwitchbotPosition() && Cell.IsEquipPosition())
	{
		ChatPacket(CHAT_TYPE_INFO, "Cannot move equipped items to switchbot.");
		return false;
	}
#endif


#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	if (DestCell.IsSkillBookInventoryPosition() && (item->IsEquipped() || Cell.IsBeltInventoryPosition()))
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;10087]");
		return false;
	}

	if (DestCell.IsUpgradeItemsInventoryPosition() && (item->IsEquipped() || Cell.IsBeltInventoryPosition()))
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;10087]");
		return false;
	}

	if (DestCell.IsStoneInventoryPosition() && (item->IsEquipped() || Cell.IsBeltInventoryPosition()))
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;10087]");
		return false;
	}

	if (DestCell.IsBoxInventoryPosition() && (item->IsEquipped() || Cell.IsBeltInventoryPosition()))
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;10087]");
		return false;
	}

	if (DestCell.IsEfsunInventoryPosition() && (item->IsEquipped() || Cell.IsBeltInventoryPosition()))
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;10087]");
		return false;
	}

	if (DestCell.IsCicekInventoryPosition() && (item->IsEquipped() || Cell.IsBeltInventoryPosition()))
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;10087]");
		return false;
	}

	if ((Cell.IsSkillBookInventoryPosition() && !DestCell.IsSkillBookInventoryPosition() && !DestCell.IsDefaultInventoryPosition()))
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;10087]");
		return false;
	}

	if (Cell.IsUpgradeItemsInventoryPosition() && !DestCell.IsUpgradeItemsInventoryPosition() && !DestCell.IsDefaultInventoryPosition())
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;10087]");
		return false;
	}

	if (Cell.IsStoneInventoryPosition() && !DestCell.IsStoneInventoryPosition() && !DestCell.IsDefaultInventoryPosition())
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;10087]");
		return false;
	}

	if (Cell.IsBoxInventoryPosition() && !DestCell.IsBoxInventoryPosition() && !DestCell.IsDefaultInventoryPosition())
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;10087]");
		return false;
	}

	if (Cell.IsEfsunInventoryPosition() && !DestCell.IsEfsunInventoryPosition() && !DestCell.IsDefaultInventoryPosition())
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;10087]");
		return false;
	}

	if (Cell.IsCicekInventoryPosition() && !DestCell.IsCicekInventoryPosition() && !DestCell.IsDefaultInventoryPosition())
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;10087]");
		return false;
	}

	if (Cell.IsDefaultInventoryPosition() && DestCell.IsSkillBookInventoryPosition())
	{
		if (!item->IsSkillBook())
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;10087]");
			return false;
		}
	}

	if (Cell.IsDefaultInventoryPosition() && DestCell.IsUpgradeItemsInventoryPosition())
	{
		if (!item->IsUpgradeItem())
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;10087]");
			return false;
		}
	}

	if (Cell.IsDefaultInventoryPosition() && DestCell.IsStoneInventoryPosition())
	{
		if (!item->IsStone())
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;10087]");
			return false;
		}
	}

	if (Cell.IsDefaultInventoryPosition() && DestCell.IsBoxInventoryPosition())
	{
		if (!item->IsBox())
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;10087]");
			return false;
		}
	}

	if (Cell.IsDefaultInventoryPosition() && DestCell.IsEfsunInventoryPosition())
	{
		if (!item->IsEfsun())
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;10087]");
			return false;
		}
	}

	if (Cell.IsDefaultInventoryPosition() && DestCell.IsCicekInventoryPosition())
	{
		if (!item->IsCicek())
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;10087]");
			return false;
		}
	}
#endif
#ifdef __RENEWAL_MOUNT__
	if (DestCell.IsDefaultInventoryPosition() && Cell.IsEquipPosition())
	{
		if (item->IsCostumeMountItem() && GetHorse())
		{
			HorseSummon(false);
		}
	}
#endif
	if (Cell.IsEquipPosition())
	{
		if (!CanUnequipNow(item))
			return false;

#ifdef ENABLE_MOUNT_SYSTEM
		if (item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_MOUNT && IsRiding())
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;10017]");
			return false;
		}
#endif

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
		int iWearCell = item->FindEquipCell(this);
		if (iWearCell == WEAR_WEAPON)
		{
			LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && !UnequipItem(costumeWeapon))
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;2415]");
				return false;
			}

			if (!IsEmptyItemGrid(DestCell, item->GetSize(), Cell.cell))
				return UnequipItem(item);
		}
#endif
	}

	if (DestCell.IsEquipPosition())
	{
		if (GetItem(DestCell))
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;1092]");

			return false;
		}

		EquipItem(item, DestCell.cell - INVENTORY_MAX_NUM);
	}
	else
	{
		if (item->IsDragonSoul())
		{
			if (item->IsEquipped())
			{
				return DSManager::instance().PullOut(this, DestCell, item);
			}
			else
			{
				if (DestCell.window_type != DRAGON_SOUL_INVENTORY)
				{
					return false;
				}

				if (!DSManager::instance().IsValidCellForThisItem(item, DestCell))
					return false;
			}
		}

		else if (DRAGON_SOUL_INVENTORY == DestCell.window_type)
			return false;

		LPITEM item2;

		if ((item2 = GetItem(DestCell)) && item != item2 && item2->IsStackable() &&
			!IS_SET(item2->GetAntiFlag(), ITEM_ANTIFLAG_STACK) &&
			item2->GetVnum() == item->GetVnum())
		{
			// for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i) {
			// 	if (item2->GetSocket(i) != item->GetSocket(i)) {
			// 		if (item2->GetType() != ITEM_GACHA && item->GetType() != ITEM_GACHA) {
			// 			return false;
			// 		}
			// 	}
			// }

			if (count == 0)
#ifdef __EXTENDED_ITEM_COUNT__
				count = (uint16_t)item->GetCount();
#else
				count = (BYTE)item->GetCount();
#endif

			sys_log(0, "%s: ITEM_STACK %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d", GetName(), item->GetName(), Cell.window_type, Cell.cell,
				DestCell.window_type, DestCell.cell, count);

			count = MIN(g_bItemCountLimit - item2->GetCount(), count);

			// if (item->GetType() == ITEM_GACHA && item2->GetType() == ITEM_GACHA) {
			// 	int newSocketValue = MIN(g_bItemCountLimit - item2->GetSocket(0), item->GetSocket(0));
			// 	item->SetSocket(0, item->GetSocket(0) - newSocketValue);
			// 	item2->SetSocket(0, item2->GetSocket(0) + newSocketValue);
			// }

			item->SetCount(item->GetCount() - count);
			item2->SetCount(item2->GetCount() + count);
			return true;
		}

		if (!IsEmptyItemGrid(DestCell, item->GetSize(), Cell.cell))
			return false;

		if (count == 0 || count >= item->GetCount() || !item->IsStackable() || IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
		{
			sys_log(0, "%s: ITEM_MOVE %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d", GetName(), item->GetName(), Cell.window_type, Cell.cell,
				DestCell.window_type, DestCell.cell, count);

			item->RemoveFromCharacter();
#ifdef ENABLE_HIGHLIGHT_NEW_ITEM
			SetItem(DestCell, item, true);
#else
			SetItem(DestCell, item);
#endif
			if (INVENTORY == Cell.window_type && INVENTORY == DestCell.window_type)
				SyncQuickslot(QUICKSLOT_TYPE_ITEM, Cell.cell, DestCell.cell);
		}
		else if (count < item->GetCount())
		{
			sys_log(0, "%s: ITEM_SPLIT %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d", GetName(), item->GetName(), Cell.window_type, Cell.cell,
				DestCell.window_type, DestCell.cell, count);

			item->SetCount(item->GetCount() - count);
			LPITEM item2 = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), count);

			// copy socket -- by mhh
			FN_copy_item_socket(item2, item);

			item2->AddToCharacter(this, DestCell);

			char szBuf[51 + 1];
			snprintf(szBuf, sizeof(szBuf), "%u %u %u %u ", item2->GetID(), item2->GetCount(), item->GetCount(), item->GetCount() + item2->GetCount());
			LogManager::instance().ItemLog(this, item, "ITEM_SPLIT", szBuf);
		}
	}

	return true;
}

namespace NPartyPickupDistribute
{
	struct FFindOwnership
	{
		LPITEM item;
		LPCHARACTER owner;

		FFindOwnership(LPITEM item)
			: item(item), owner(NULL)
		{
		}

		void operator () (LPCHARACTER ch)
		{
			if (item->IsOwnership(ch))
				owner = ch;
		}
	};

	struct FCountNearMember
	{
		int		total;
		int		x, y;

		FCountNearMember(LPCHARACTER center)
			: total(0), x(center->GetX()), y(center->GetY())
		{
		}

		void operator () (LPCHARACTER ch)
		{
			if (DISTANCE_SQRT(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
				total += 1;
		}
	};

	struct FMoneyDistributor
	{
		int		total;
		LPCHARACTER	c;
		int		x, y;
		int		iMoney;

		FMoneyDistributor(LPCHARACTER center, int iMoney)
			: total(0), c(center), x(center->GetX()), y(center->GetY()), iMoney(iMoney)
		{
		}

		void operator ()(LPCHARACTER ch)
		{
			if (ch != c)
				if (DISTANCE_SQRT(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
				{
					ch->PointChange(POINT_GOLD, iMoney, true);

					if (iMoney > 1000)
					{
						LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().CharLog(ch, iMoney, "GET_GOLD", ""));
					}
				}
		}
	};

#ifdef ENABLE_CHEQUE_SYSTEM
	struct FChequeDistributor
	{
		int total;
		LPCHARACTER c;
		int x, y;
		int iCheque;

		FChequeDistributor(LPCHARACTER center, int iCheque)
			: total(0), c(center), x(center->GetX()), y(center->GetY()), iCheque(iCheque)
		{
		}

		void operator ()(LPCHARACTER ch)
		{
			if (ch != c)
				if (DISTANCE_SQRT(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
				{
					ch->PointChange(POINT_CHEQUE, iCheque, true);

					if (iCheque > 1000)
						LogManager::instance().CharLog(ch, iCheque, "GET_CHEQUE", "");
				}
		}
	};
#endif

#ifdef ENABLE_PUNKTY_OSIAGNIEC
	struct FPktOsiagDistributor
	{
		int		total;
		LPCHARACTER	c;
		int		x, y;
		int		iPktOsiag;

		FPktOsiagDistributor(LPCHARACTER center, int iPktOsiag)
			: total(0), c(center), x(center->GetX()), y(center->GetY()), iPktOsiag(iPktOsiag)
		{
		}

		void operator ()(LPCHARACTER ch)
		{
			if (ch != c)
				if (DISTANCE_SQRT(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
				{
					ch->PointChange(POINT_PKT_OSIAG, iPktOsiag, true);

					if (iPktOsiag > 1000)
						LogManager::instance().CharLog(ch, iPktOsiag, "GET_PktOsiag", "");
				}
		}
	};
#endif

}

#ifdef ENABLE_LONG_LONG
void CHARACTER::GiveGold(int64_t iAmount)
#else
void CHARACTER::GiveGold(int iAmount)
#endif
{
	if (iAmount <= 0)
		return;

#ifdef ENABLE_LONG_LONG
	sys_log(0, "GIVE_GOLD: %s %lld", GetName(), iAmount);
#else
	sys_log(0, "GIVE_GOLD: %s %d", GetName(), iAmount);
#endif

	if (GetParty())
	{
		LPPARTY pParty = GetParty();

#ifdef ENABLE_LONG_LONG
		int64_t dwTotal = iAmount;
		int64_t dwMyAmount = dwTotal;
#else
		DWORD dwTotal = iAmount;
		DWORD dwMyAmount = dwTotal;
#endif

		NPartyPickupDistribute::FCountNearMember funcCountNearMember(this);
		pParty->ForEachOnlineMember(funcCountNearMember);

		if (funcCountNearMember.total > 1)
		{
			DWORD dwShare = dwTotal / funcCountNearMember.total;
			dwMyAmount -= dwShare * (funcCountNearMember.total - 1);

			NPartyPickupDistribute::FMoneyDistributor funcMoneyDist(this, dwShare);

			pParty->ForEachOnlineMember(funcMoneyDist);
		}

		PointChange(POINT_GOLD, dwMyAmount, true);
		if (dwMyAmount > 1000)
		{
			LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().CharLog(this, dwMyAmount, "GET_GOLD", ""));
		}
	}
	else
	{
		PointChange(POINT_GOLD, iAmount, true);
		if (iAmount > 1000)
		{
			LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().CharLog(this, iAmount, "GET_GOLD", ""));
		}
	}
}

#ifdef ENABLE_CHEQUE_SYSTEM
void CHARACTER::GiveCheque(int64_t iAmount)
{
	if (iAmount <= 0)
		return;

	sys_log(0, "GIVE_CHEQUE: %s %lld", GetName(), iAmount);

	if (GetParty())
	{
		LPPARTY pParty = GetParty();

		DWORD dwTotal = iAmount;
		DWORD dwMyAmount = dwTotal;

		NPartyPickupDistribute::FCountNearMember funcCountNearMember(this);
		pParty->ForEachOnlineMember(funcCountNearMember);

		if (funcCountNearMember.total > 1)
		{
			DWORD dwShare = dwTotal / funcCountNearMember.total;
			dwMyAmount -= dwShare * (funcCountNearMember.total - 1);

			NPartyPickupDistribute::FChequeDistributor funcChequeDist(this, dwShare);

			pParty->ForEachOnlineMember(funcChequeDist);
		}

		PointChange(POINT_CHEQUE, dwMyAmount, true);

		if (dwMyAmount > 1000)
			LogManager::instance().CharLog(this, dwMyAmount, "GET_CHEQUE", "");
	}
	else
	{
		PointChange(POINT_CHEQUE, iAmount, true);

		if (LC_IsBrazil() == true)
		{
			if (iAmount >= 213)
				LogManager::instance().CharLog(this, iAmount, "GET_CHEQUE", "");
		}
		else
		{
			if (iAmount > 1000)
				LogManager::instance().CharLog(this, iAmount, "GET_CHEQUE", "");
		}
	}
}
#endif

#ifdef ENABLE_STONE_POINT_SYSTEM
void CHARACTER::GivePktOsiag(int iAmount)
{
	if (iAmount <= 0)
		return;

	sys_log(0, "GIVE_PktOsiag: %s %lld", GetName(), iAmount);

	if (GetParty())
	{
		LPPARTY pParty = GetParty();

		DWORD dwTotal = iAmount;
		DWORD dwMyAmount = dwTotal;

		NPartyPickupDistribute::FCountNearMember funcCountNearMember(this);
		pParty->ForEachOnlineMember(funcCountNearMember);

		if (funcCountNearMember.total > 1)
		{
			DWORD dwShare = dwTotal / funcCountNearMember.total;
			dwMyAmount -= dwShare * (funcCountNearMember.total - 1);

			NPartyPickupDistribute::FPktOsiagDistributor funcPktOsiagDist(this, dwShare);

			pParty->ForEachOnlineMember(funcPktOsiagDist);
		}

		PointChange(POINT_PKT_OSIAG, dwMyAmount, true);

		if (dwMyAmount > 1000)
			LogManager::instance().CharLog(this, dwMyAmount, "GET_PktOsiag", "");
	}
	else
	{
		PointChange(POINT_PKT_OSIAG, iAmount, true);

		if (LC_IsBrazil() == true)
		{
			if (iAmount >= 213)
				LogManager::instance().CharLog(this, iAmount, "GET_PktOsiag", "");
		}
		else
		{
			if (iAmount > 1000)
				LogManager::instance().CharLog(this, iAmount, "GET_PktOsiag", "");
		}
	}
}
#endif

#include "Inventory.hpp"
bool CHARACTER::PickupItem(DWORD dwVID, bool isPremiumPickUp)
{
	LPITEM item = ITEM_MANAGER::instance().FindByVID(dwVID);

	if (IsObserverMode())
		return false;

	if (!item || !item->GetSectree())
		return false;

	if (item->DistanceValid(this))
	{
		LPCHARACTER picker = this; //For clearer interactions/comparisons right below

		LPCHARACTER owner = item->IsOwnership(picker) ? picker : nullptr;
		if (!owner && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_DROP) && GetParty())
		{
			NPartyPickupDistribute::FFindOwnership funcFindOwnership(item);
			GetParty()->ForEachOnlineMember(funcFindOwnership);
			owner = funcFindOwnership.owner;
		}

		if (owner)
		{
			// If it is Yang, just add it up and destroy the item
			if (item->GetType() == ITEM_ELK)
			{
				owner->GiveGold(item->GetCount());
				item->RemoveFromGround();

				M2_DESTROY_ITEM(item);
				owner->Save();
			}
			else //Otherwise, add it to the player's inventory
			{
				const uint16_t origCount = (uint16_t)item->GetCount();
				auto status = owner->GetInventory().Add(OUT item);
				if (status == Inventory::EStatus::ERROR_NO_SPACE && owner != picker)
				{
					// If there was no space, and the owner was not the picker, let's try again
					// but with the picker as the owner.
					owner = picker;
					status = owner->GetInventory().Add(OUT item);
				}

				switch (status)
				{
					case Inventory::EStatus::SUCCESS:
						// Ping quests
						quest::CQuestManager::instance().PickupItem(owner->GetPlayerID(), item);

						
						// Log eet
						char szHint[64 + 1];
						snprintf(szHint, sizeof(szHint), "%s %lu %lu", item->GetName(), item->GetCount(), item->GetOriginalVnum());
						LogManager::instance().ItemLog(owner, item, "GET", szHint);

						if (owner == picker)
						{
							// Notify player
							if (origCount > 1) 
							{
								picker->ChatPacket(CHAT_TYPE_INFO, "[LS;24;[IN;%d];%d]", item->GetVnum(), origCount);
							} 
							else 
							{
								picker->ChatPacket(CHAT_TYPE_INFO, "[LS;444;[IN;%d]]", item->GetVnum());
							}
						}
						else
						{
							// Party pickup. Notify both players
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
							const std::string& ownerLocalName = CLocaleItemManager::instance().Find(item->GetVnum(), owner->GetLanguage());
							const char* ownerItemName = !ownerLocalName.empty() ? ownerLocalName.c_str() : item->GetName();
							const std::string& pickerLocalName = CLocaleItemManager::instance().Find(item->GetVnum(), picker->GetLanguage());
							const char* pickerItemName = !pickerLocalName.empty() ? pickerLocalName.c_str() : item->GetName();
#else
							const char* ownerItemName = item->GetName();
							const char* pickerItemName = item->GetName();
#endif
							owner->ChatPacket(CHAT_TYPE_INFO, "[LS;10055;%s;%s]", picker->GetName(), ownerItemName);
							picker->ChatPacket(CHAT_TYPE_INFO, "[LS;10056;%s;%s]", owner->GetName(), pickerItemName);
						}

						break;

					case Inventory::EStatus::ERROR_NO_SPACE:
						//Here, owner == picker, since if it had not been, we'd retried already
						owner->ChatPacket(CHAT_TYPE_INFO, "[LS;1466]");
						break;

					default: break; //Don't do anything
				}
			}

			return true;
		}
	}

	return false;
}

bool CHARACTER::SwapItem(BYTE bCell, BYTE bDestCell)
{
	if (!CanHandleItem())
		return false;

	TItemPos srcCell(INVENTORY, bCell), destCell(INVENTORY, bDestCell);

	//if (bCell >= INVENTORY_MAX_NUM + WEAR_MAX_NUM || bDestCell >= INVENTORY_MAX_NUM + WEAR_MAX_NUM)
	if (srcCell.IsDragonSoulEquipPosition() || destCell.IsDragonSoulEquipPosition())
		return false;

	if (bCell == bDestCell)
		return false;

	if (srcCell.IsEquipPosition() && destCell.IsEquipPosition())
		return false;

	LPITEM item1, item2;

	if (srcCell.IsEquipPosition())
	{
		item1 = GetInventoryItem(bDestCell);
		item2 = GetInventoryItem(bCell);
	}
	else
	{
		item1 = GetInventoryItem(bCell);
		item2 = GetInventoryItem(bDestCell);
	}

	if (!item1 || !item2)
		return false;

	if (item1 == item2)
	{
		sys_log(0, "[WARNING][WARNING][HACK USER!] : %s %d %d", m_stName.c_str(), bCell, bDestCell);
		return false;
	}

	if (!IsEmptyItemGrid(TItemPos(INVENTORY, item1->GetCell()), item2->GetSize(), item1->GetCell()))
		return false;

	if (TItemPos(EQUIPMENT, item2->GetCell()).IsEquipPosition())
	{
		BYTE bEquipCell = item2->GetCell() - INVENTORY_MAX_NUM;
		BYTE bInvenCell = item1->GetCell();

		if (item2->IsDragonSoul() || item2->GetType() == ITEM_BELT) // @fixme117
		{
			if (false == CanUnequipNow(item2) || false == CanEquipNow(item1))
				return false;
		}

		if (bEquipCell != item1->FindEquipCell(this))
			return false;

		item2->RemoveFromCharacter();

		if (item1->EquipTo(this, bEquipCell))
			item2->AddToCharacter(this, TItemPos(INVENTORY, bInvenCell));
		else
			sys_err("SwapItem cannot equip %s! item1 %s", item2->GetName(), item1->GetName());
	}
	else
	{
		BYTE bCell1 = item1->GetCell();
		BYTE bCell2 = item2->GetCell();

		item1->RemoveFromCharacter();
		item2->RemoveFromCharacter();

		item1->AddToCharacter(this, TItemPos(INVENTORY, bCell2));
		item2->AddToCharacter(this, TItemPos(INVENTORY, bCell1));
	}

	return true;
}

bool CHARACTER::UnequipItem(LPITEM item)
{
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	int iWearCell = item->FindEquipCell(this);
	if (iWearCell == WEAR_WEAPON)
	{
		LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
		if (costumeWeapon && !UnequipItem(costumeWeapon))
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;2415]");
			return false;
		}
	}
#endif

	if (false == CanUnequipNow(item))
		return false;
	
#ifdef __SKILL_COSTUME__
	if (item->GetType() == ITEM_SKILL_COSTUME)
		UseSkillCostumeItem(item, false);
#endif

#ifdef __RENEWAL_MOUNT__
	if (item->IsCostumeMountItem() && GetHorse()){
		HorseSummon(false);
	}
#endif

	int pos;
	if (item->IsDragonSoul())
		pos = GetEmptyDragonSoulInventory(item);
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	else if (item->IsSkillBook())
		pos = GetEmptySkillBookInventory(item->GetSize());
	else if (item->IsUpgradeItem())
		pos = GetEmptyUpgradeItemsInventory(item->GetSize());
	else if (item->IsStone())
		pos = GetEmptyStoneInventory(item->GetSize());
	else if (item->IsBox())
		pos = GetEmptyBoxInventory(item->GetSize());
	else if (item->IsEfsun())
		pos = GetEmptyEfsunInventory(item->GetSize());
	else if (item->IsCicek())
		pos = GetEmptyCicekInventory(item->GetSize());
#endif
	else
		pos = GetEmptyInventory(item->GetSize());

	// HARD CODING
	if (item->GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
		ShowAlignment(true);
	
#ifdef ENABLE_MOUNT_SYSTEM
	if (item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_MOUNT && IsRiding())
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;10002]");
		return false;
	}
#endif

	item->RemoveFromCharacter();
	if (item->IsDragonSoul())
		item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, pos));
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	else if (item->IsSkillBook())
		item->AddToCharacter(this, TItemPos(SKILL_BOOK_INVENTORY, pos));
	else if (item->IsUpgradeItem())
		item->AddToCharacter(this, TItemPos(UPGRADE_ITEMS_INVENTORY, pos));
	else if (item->IsStone())
		item->AddToCharacter(this, TItemPos(STONE_INVENTORY, pos));
	else if (item->IsBox())
		item->AddToCharacter(this, TItemPos(BOX_INVENTORY, pos));
	else if (item->IsEfsun())
		item->AddToCharacter(this, TItemPos(EFSUN_INVENTORY, pos));
	else if (item->IsCicek())
		item->AddToCharacter(this, TItemPos(CICEK_INVENTORY, pos));
#endif
	else
		item->AddToCharacter(this, TItemPos(INVENTORY, pos));

	CheckMaximumPoints();

#ifdef ENABLE_MOUNT_SYSTEM
	if (item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_MOUNT)
	{
		SendMountInfoPacket();
		if (m_chHorse)
		{
			HorseSummon(false, true);
			HorseSummon(true, true);
		}
	}
#endif

	return true;
}

bool CHARACTER::FastStack(TItemPos originPos)
{
	if (!IsValidItemPosition(originPos))
		return false;

	if (!CanHandleItem())
		return false;

	LPITEM item = GetItem(originPos);

	if (item == nullptr)
		return false;

	if (item->GetCount() >= ITEM_MAX_COUNT)
	{
		ChatPacket(CHAT_TYPE_INFO, "ITEM MAX");
		return false;
	}

	int currentPos = originPos.cell + 1;

	while (item->GetCount() < ITEM_MAX_COUNT && (originPos.cell != currentPos))
	{
		LPITEM item2 = GetInventoryItem(currentPos);
		if (item2)
		{
			if (item2->GetVnum() == item->GetVnum())
			{
				if (item2->GetCount() < ITEM_MAX_COUNT)
				{
					int itemCount = item->GetCount();
					int item2Count = item2->GetCount();

					if (itemCount + item2Count > ITEM_MAX_COUNT)
					{
						int requiredCountToMax = (ITEM_MAX_COUNT - itemCount);
						
						MoveItem(TItemPos(INVENTORY, item2->GetCell()), originPos, requiredCountToMax);
					}
					else
					{
						MoveItem(TItemPos(INVENTORY, item2->GetCell()), originPos, item2->GetCount());
					}
				}
			}
		}

		currentPos++;

		if (currentPos >= ITEM_MAX_COUNT)
		{
			currentPos = 0;
		}
	}

	return true;
}

//
bool CHARACTER::EquipItem(LPITEM item, int iCandidateCell)
{
	if (item->IsExchanging())
		return false;

	if (false == item->IsEquipable())
		return false;

	if (false == CanEquipNow(item))
		return false;
	
#ifdef ENABLE_MOUNT_SYSTEM
	if (item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_MOUNT && IsRiding())
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;10003]");
		return false;
	}
#endif

	int iWearCell = item->FindEquipCell(this, iCandidateCell);

	if (iWearCell < 0)
		return false;
	
#ifdef __SKILL_COSTUME__
	if (item->GetType() == ITEM_SKILL_COSTUME)
	{
		if (m_pSkillLevels[item->GetValue(3)].bLevel < SKILL_MAX_LEVEL)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cant use this because your skillgroup or skill level is invalid"));
			return false;
		}
	}
#endif

	if (item->GetCount() > 1) 
	{
		return false;
	}

	if (iWearCell == WEAR_BODY && IsRiding() && (item->GetVnum() >= 11901 && item->GetVnum() <= 11904))
	{
		return false;
	}

	if (FN_check_item_sex(this, item) == false)
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;1005]");
		return false;
	}

	DWORD dwCurTime = get_dword_time();

	if (iWearCell != WEAR_ARROW
		&& (dwCurTime - GetLastAttackTime() <= 500 || dwCurTime - m_dwLastSkillTime <= 250))
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;451]");
		return false;
	}

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	if (iWearCell == WEAR_WEAPON)
	{
		if (item->GetType() == ITEM_WEAPON)
		{
			LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && costumeWeapon->GetValue(3) != item->GetSubType() && !UnequipItem(costumeWeapon))
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;2415]");
				return false;
			}
		}
		else //fishrod/pickaxe
		{
			LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && !UnequipItem(costumeWeapon))
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;2415]");
				return false;
			}
		}
	}
	else if (iWearCell == WEAR_COSTUME_WEAPON)
	{
		if (item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_WEAPON)
		{
			LPITEM pkWeapon = GetWear(WEAR_WEAPON);
			if (!pkWeapon || pkWeapon->GetType() != ITEM_WEAPON || item->GetValue(3) != pkWeapon->GetSubType())
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;2414]");
				return false;
			}
		}
	}
#endif

	if (item->IsDragonSoul())
	{
		if (GetInventoryItem(INVENTORY_MAX_NUM + iWearCell))
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;1090]");
			return false;
		}

		if (!item->EquipTo(this, iWearCell))
		{
			return false;
		}
	}

	else
	{
		if (GetWear(iWearCell) && !IS_SET(GetWear(iWearCell)->GetFlag(), ITEM_FLAG_IRREMOVABLE))
		{
			if (item->GetWearFlag() == WEARABLE_ABILITY)
				return false;

			if (false == SwapItem(item->GetCell(), INVENTORY_MAX_NUM + iWearCell))
			{
				return false;
			}
		}
		else
		{
			BYTE bOldCell = item->GetCell();

			if (item->EquipTo(this, iWearCell))
			{
				SyncQuickslot(QUICKSLOT_TYPE_ITEM, bOldCell, iWearCell);
			}
		}
	}

	if (true == item->IsEquipped())
	{
		if (-1 != item->GetProto()->cLimitRealTimeFirstUseIndex)
		{
			if (0 == item->GetSocket(1))
			{
				int32_t duration = (0 != item->GetSocket(0)) ? item->GetSocket(0) : item->GetProto()->aLimits[(unsigned char)(item->GetProto()->cLimitRealTimeFirstUseIndex)].lValue;

				if (item->GetSocket(0) > 0)
					duration += item->GetSocket(0);

				if (0 == duration)
					duration = 60 * 60 * 24 * 7;

				item->SetSocket(0, time(0) + duration);
				item->StartRealTimeExpireEvent();
			}

			item->SetSocket(1, item->GetSocket(1) + 1);
		}

		if (item->GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
			ShowAlignment(false);

		const DWORD& dwVnum = item->GetVnum();

		if (true == CItemVnumHelper::IsRamadanMoonRing(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_RAMADAN_RING);
		}

		else if (true == CItemVnumHelper::IsHalloweenCandy(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_HALLOWEEN_CANDY);
		}

#ifdef ENABLE_SYSTEM_RUNE
		// WHITE
		else if (true == CItemVnumHelper::IsRunaWhite(dwVnum))
		{
			this->EffectPacket(SE_RUNA_WHITE_EFFECT);
		}
		// RED
		else if (true == CItemVnumHelper::IsRunaRed(dwVnum))
		{
			this->EffectPacket(SE_RUNA_RED_EFFECT);
		}
		// BLUE
		else if (true == CItemVnumHelper::IsRunaBlue(dwVnum))
		{
			this->EffectPacket(SE_RUNA_BLUE_EFFECT);
		}
		// GREEN
		else if (true == CItemVnumHelper::IsRunaGreen(dwVnum))
		{
			this->EffectPacket(SE_RUNA_GREEN_EFFECT);
		}
		// BLACK
		else if (true == CItemVnumHelper::IsRunaBlack(dwVnum))
		{
			this->EffectPacket(SE_RUNA_BLACK_EFFECT);
		}
		// YELLOW
		else if (true == CItemVnumHelper::IsRunaYellow(dwVnum))
		{
			this->EffectPacket(SE_RUNA_YELLOW_EFFECT);
		}
#endif


		else if (true == CItemVnumHelper::IsHappinessRing(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_HAPPINESS_RING);
		}

		else if (true == CItemVnumHelper::IsLovePendant(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_LOVE_PENDANT);
		}
		else if (ITEM_UNIQUE == item->GetType() && 0 != item->GetSIGVnum())
		{
			const CSpecialItemGroup* pGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(item->GetSIGVnum());
			if (NULL != pGroup)
			{
				const CSpecialAttrGroup* pAttrGroup = ITEM_MANAGER::instance().GetSpecialAttrGroup(pGroup->GetAttrVnum(item->GetVnum()));
				if (NULL != pAttrGroup)
				{
					const std::string& std = pAttrGroup->m_stEffectFileName;
					SpecificEffectPacket(std);
				}
			}
		}
		else if (
			(ITEM_UNIQUE == item->GetType() && UNIQUE_SPECIAL_RIDE == item->GetSubType() && IS_SET(item->GetFlag(), ITEM_FLAG_QUEST_USE))
			|| (ITEM_UNIQUE == item->GetType() && UNIQUE_SPECIAL_MOUNT_RIDE == item->GetSubType() && IS_SET(item->GetFlag(), ITEM_FLAG_QUEST_USE))
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
			|| (ITEM_COSTUME == item->GetType() && COSTUME_MOUNT == item->GetSubType())
#endif
		)
		{
			quest::CQuestManager::instance().UseItem(GetPlayerID(), item, false);
		}
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		else if ((item->GetType() == ITEM_COSTUME) && (item->GetSubType() == COSTUME_ACCE))
			this->EffectPacket(SE_EFFECT_ACCE_EQUIP);
#endif
#ifdef ENABLE_STOLE_COSTUME
		else if ((item->GetType() == ITEM_COSTUME) && (item->GetSubType() == COSTUME_STOLE))
			this->EffectPacket(SE_EFFECT_ACCE_EQUIP);
#endif

		// if (item->IsNewMountItem()) // @fixme152
		// 	quest::CQuestManager::instance().SIGUse(GetPlayerID(), quest::QUEST_NO_NPC, item, false);
#ifdef __SKILL_COSTUME__
		if (item->GetType() == ITEM_SKILL_COSTUME)
			UseSkillCostumeItem(item, true);
#endif
	}

#ifdef __RENEWAL_MOUNT__
	if (item->IsCostumeMountItem())
	{
		if (!PulseManager::Instance().IncreaseCount(GetPlayerID(), ePulse::RideMount, std::chrono::milliseconds(1500), 2)) {
			return false;
		}

		LPITEM costumeItem = GetWear(WEAR_COSTUME_MOUNT);
		if (!costumeItem)
		{
			StopRiding();
			HorseSummon(false);
		}
		else
		{
			StopRiding();
			if (GetHorse())
				HorseSummon(false);
			HorseSummon(true, false, costumeItem->GetValue(1));
			StartRiding();
		}
	}
#endif

#ifdef ENABLE_MOUNT_SYSTEM
	if (iWearCell == WEAR_COSTUME_MOUNT || iWearCell == WEAR_MOUNT_EQUIPMENT_SHOE || iWearCell == WEAR_MOUNT_EQUIPMENT_SADDLE || iWearCell == WEAR_MOUNT_EQUIPMENT_ARMOR)
	{
		if (m_dwLastMountUseTime + 1000 > get_dword_time())
			return false;
		
		m_dwLastMountUseTime = get_dword_time();
	}
#endif

#ifdef ENABLE_MOUNT_SYSTEM
	if (iWearCell == WEAR_COSTUME_MOUNT && IsRiding())
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;10017]");
		//ChatInfoTrans(("You can't change mount appearance while mounted."));
		return false;
	}
#endif

#ifdef ENABLE_MOUNT_SYSTEM
	if (item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_MOUNT)
	{
		SendMountInfoPacket();
		if (m_chHorse)
		{
			HorseSummon(false, true);
			HorseSummon(true, true);
		}
	}
#endif

	return true;
}

void CHARACTER::BuffOnAttr_AddBuffsFromItem(LPITEM pItem)
{
	for (size_t i = 0; i < sizeof(g_aBuffOnAttrPoints) / sizeof(g_aBuffOnAttrPoints[0]); i++)
	{
		TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.find(g_aBuffOnAttrPoints[i]);
		if (it != m_map_buff_on_attrs.end())
		{
			it->second->AddBuffFromItem(pItem);
		}
	}
}

void CHARACTER::BuffOnAttr_RemoveBuffsFromItem(LPITEM pItem)
{
	for (size_t i = 0; i < sizeof(g_aBuffOnAttrPoints) / sizeof(g_aBuffOnAttrPoints[0]); i++)
	{
		TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.find(g_aBuffOnAttrPoints[i]);
		if (it != m_map_buff_on_attrs.end())
		{
			it->second->RemoveBuffFromItem(pItem);
		}
	}
}

void CHARACTER::BuffOnAttr_ClearAll()
{
	for (TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.begin(); it != m_map_buff_on_attrs.end(); it++)
	{
		CBuffOnAttributes* pBuff = it->second;
		if (pBuff)
		{
			pBuff->Initialize();
		}
	}
}

void CHARACTER::BuffOnAttr_ValueChange(BYTE bType, BYTE bOldValue, BYTE bNewValue)
{
	TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.find(bType);

	if (0 == bNewValue)
	{
		if (m_map_buff_on_attrs.end() == it)
			return;
		else
			it->second->Off();
	}
	else if (0 == bOldValue)
	{
		CBuffOnAttributes* pBuff = NULL;
		if (m_map_buff_on_attrs.end() == it)
		{
			switch (bType)
			{
			case POINT_ENERGY:
			{
				static BYTE abSlot[] = { WEAR_BODY, WEAR_HEAD, WEAR_FOOTS, WEAR_WRIST, WEAR_WEAPON, WEAR_NECK, WEAR_EAR, WEAR_SHIELD };
				static std::vector <BYTE> vec_slots(abSlot, abSlot + _countof(abSlot));
				pBuff = M2_NEW CBuffOnAttributes(this, bType, &vec_slots);
			}
			break;
			case POINT_COSTUME_ATTR_BONUS:
			{
				static BYTE abSlot[] = {
					WEAR_COSTUME_BODY,
					WEAR_COSTUME_HAIR,
					#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
					WEAR_COSTUME_WEAPON,
					#endif
					#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
					WEAR_COSTUME_MOUNT,
					#endif
					#ifdef ENABLE_NEW_MOUNT_SYSTEM
					WEAR_COSTUME_MOUNT,
					#endif
				};
				static std::vector <BYTE> vec_slots(abSlot, abSlot + _countof(abSlot));
				pBuff = M2_NEW CBuffOnAttributes(this, bType, &vec_slots);
			}
			break;
			default:
				break;
			}
			m_map_buff_on_attrs.emplace(bType, pBuff);

		}
		else
			pBuff = it->second;
		if (pBuff != NULL)
			pBuff->On(bNewValue);
	}
	else
	{
		assert(m_map_buff_on_attrs.end() != it);
		it->second->ChangeBuffValue(bNewValue);
	}
}

LPITEM CHARACTER::FindSpecifyItem(DWORD vnum) const
{
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	for (int i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
#else
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
#endif
		if (GetInventoryItem(i) && GetInventoryItem(i)->GetVnum() == vnum)
			return GetInventoryItem(i);

	return NULL;
}

LPITEM CHARACTER::FindItemByID(DWORD id) const
{
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	for (int i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
#else
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
#endif
	{
		if (NULL != GetInventoryItem(i) && GetInventoryItem(i)->GetID() == id)
			return GetInventoryItem(i);
	}

	for (int i = BELT_INVENTORY_SLOT_START; i < BELT_INVENTORY_SLOT_END; ++i)
	{
		if (NULL != GetInventoryItem(i) && GetInventoryItem(i)->GetID() == id)
			return GetInventoryItem(i);
	}

	return NULL;
}

int CHARACTER::CountSpecifyItem(DWORD vnum) const
{
	int	count = 0;
	LPITEM item;

	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		item = GetInventoryItem(i);
		if (NULL != item && item->GetVnum() == vnum)
		{
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}
			else
			{
				count += item->GetCount();
			}
		}
	}

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	for (int i = SKILL_BOOK_INVENTORY_SLOT_START; i < SKILL_BOOK_INVENTORY_SLOT_END; ++i)
	{
		item = GetItem(TItemPos(INVENTORY, i));
		if (NULL != item && item->GetVnum() == vnum)
		{
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}
			else
			{
				count += item->GetCount();
			}
		}
	}

	for (int i = UPGRADE_ITEMS_INVENTORY_SLOT_START; i < UPGRADE_ITEMS_INVENTORY_SLOT_END; ++i)
	{
		item = GetItem(TItemPos(INVENTORY, i));
		if (NULL != item && item->GetVnum() == vnum)
		{
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}
			else
			{
				count += item->GetCount();
			}
		}
	}

	for (int i = STONE_INVENTORY_SLOT_START; i < STONE_INVENTORY_SLOT_END; ++i)
	{
		item = GetItem(TItemPos(INVENTORY, i));
		if (NULL != item && item->GetVnum() == vnum)
		{
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}
			else
			{
				count += item->GetCount();
			}
		}
	}

	for (int i = BOX_INVENTORY_SLOT_START; i < BOX_INVENTORY_SLOT_END; ++i)
	{
		item = GetItem(TItemPos(INVENTORY, i));
		if (NULL != item && item->GetVnum() == vnum)
		{
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}
			else
			{
				count += item->GetCount();
			}
		}
	}

	for (int i = EFSUN_INVENTORY_SLOT_START; i < EFSUN_INVENTORY_SLOT_END; ++i)
	{
		item = GetItem(TItemPos(INVENTORY, i));
		if (NULL != item && item->GetVnum() == vnum)
		{
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}
			else
			{
				count += item->GetCount();
			}
		}
	}

	for (int i = CICEK_INVENTORY_SLOT_START; i < CICEK_INVENTORY_SLOT_END; ++i)
	{
		item = GetItem(TItemPos(INVENTORY, i));
		if (NULL != item && item->GetVnum() == vnum)
		{
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}
			else
			{
				count += item->GetCount();
			}
		}
	}
#endif

	return count;
}

void CHARACTER::RemoveSpecifyItem(DWORD vnum, DWORD count)
{
	if (0 == count)
		return;

	for (UINT i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		if (NULL == GetInventoryItem(i))
			continue;

		if (GetInventoryItem(i)->GetVnum() != vnum)
			continue;

		if (m_pkMyShop)
		{
			bool isItemSelling = m_pkMyShop->IsSellingItem(GetInventoryItem(i)->GetID());
			if (isItemSelling)
				continue;
		}

		if (vnum >= 80003 && vnum <= 80007)
			LogManager::instance().GoldBarLog(GetPlayerID(), GetInventoryItem(i)->GetID(), QUEST, "RemoveSpecifyItem");

		if (count >= GetInventoryItem(i)->GetCount())
		{
			count -= GetInventoryItem(i)->GetCount();
			GetInventoryItem(i)->SetCount(0);

			if (0 == count)
				return;
		}
		else
		{
			GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
			return;
		}
	}

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	for (UINT i = SKILL_BOOK_INVENTORY_SLOT_START; i < CICEK_INVENTORY_SLOT_END; ++i)
	{
		if (NULL == GetInventoryItem(i))
			continue;

		if (GetInventoryItem(i)->GetVnum() != vnum)
			continue;

		if (m_pkMyShop)
		{
			bool isItemSelling = m_pkMyShop->IsSellingItem(GetInventoryItem(i)->GetID());
			if (isItemSelling)
				continue;
		}

		if (vnum >= 80003 && vnum <= 80007)
			LogManager::instance().GoldBarLog(GetPlayerID(), GetInventoryItem(i)->GetID(), QUEST, "RemoveSpecifyItem");

		if (count >= GetInventoryItem(i)->GetCount())
		{
			count -= GetInventoryItem(i)->GetCount();
			GetInventoryItem(i)->SetCount(0);

			if (0 == count)
				return;
		}
		else
		{
			GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
			return;
		}
	}
#endif

	if (count)
		sys_log(0, "CHARACTER::RemoveSpecifyItem cannot remove enough item vnum %u, still remain %d", vnum, count);
}

int CHARACTER::CountSpecifyTypeItem(BYTE type) const
{
	int	count = 0;

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	for (int i = SKILL_BOOK_INVENTORY_SLOT_START; i < STONE_INVENTORY_SLOT_END; ++i)
#else
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
#endif
	{
		LPITEM pItem = GetInventoryItem(i);
		if (pItem != NULL && pItem->GetType() == type)
		{
			count += pItem->GetCount();
		}
	}

	return count;
}

void CHARACTER::RemoveSpecifyTypeItem(BYTE type, DWORD count)
{
	if (0 == count)
		return;

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	for (UINT i = SKILL_BOOK_INVENTORY_SLOT_START; i < STONE_INVENTORY_SLOT_END; ++i)
#else
	for (UINT i = 0; i < INVENTORY_MAX_NUM; ++i)
#endif
	{
		if (NULL == GetInventoryItem(i))
			continue;

		if (GetInventoryItem(i)->GetType() != type)
			continue;

		if (m_pkMyShop)
		{
			bool isItemSelling = m_pkMyShop->IsSellingItem(GetInventoryItem(i)->GetID());
			if (isItemSelling)
				continue;
		}

		if (count >= GetInventoryItem(i)->GetCount())
		{
			count -= GetInventoryItem(i)->GetCount();
			GetInventoryItem(i)->SetCount(0);

			if (0 == count)
				return;
		}
		else
		{
			GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
			return;
		}
	}
}

void CHARACTER::AutoGiveItem(LPITEM item, bool longOwnerShip)
{
	if (NULL == item)
	{
		sys_err("NULL point.");
		return;
	}
	if (item->GetOwner())
	{
		sys_err("item %d 's owner exists!", item->GetID());
		return;
	}

	int cell;
	if (item->IsDragonSoul())
	{
		cell = GetEmptyDragonSoulInventory(item);
	}
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	else if (item->IsSkillBook())
	{
		cell = GetEmptySkillBookInventory(item->GetSize());
	}
	else if (item->IsUpgradeItem())
	{
		cell = GetEmptyUpgradeItemsInventory(item->GetSize());
	}
	else if (item->IsStone())
	{
		cell = GetEmptyStoneInventory(item->GetSize());
	}
	else if (item->IsBox())
	{
		cell = GetEmptyBoxInventory(item->GetSize());
	}
	else if (item->IsEfsun())
	{
		cell = GetEmptyEfsunInventory(item->GetSize());
	}
	else if (item->IsCicek())
	{
		cell = GetEmptyCicekInventory(item->GetSize());
	}
#endif
	else
	{
		cell = GetEmptyInventory(item->GetSize());
	}

	if (cell != -1)
	{
		if (item->IsDragonSoul())
			item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, cell));
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
		else if (item->IsSkillBook())
			item->AddToCharacter(this, TItemPos(INVENTORY, cell));
		else if (item->IsUpgradeItem())
			item->AddToCharacter(this, TItemPos(INVENTORY, cell));
		else if (item->IsStone())
			item->AddToCharacter(this, TItemPos(INVENTORY, cell));
		else if (item->IsBox())
			item->AddToCharacter(this, TItemPos(INVENTORY, cell));
		else if (item->IsEfsun())
			item->AddToCharacter(this, TItemPos(INVENTORY, cell));
		else if (item->IsCicek())
			item->AddToCharacter(this, TItemPos(INVENTORY, cell));
#endif
		else
			item->AddToCharacter(this, TItemPos(INVENTORY, cell));

		LogManager::instance().ItemLog(this, item, "SYSTEM", item->GetName());

		if (item->GetType() == ITEM_USE && item->GetSubType() == USE_POTION)
		{
			TQuickslot* pSlot;

			if (GetQuickslot(0, &pSlot) && pSlot->type == QUICKSLOT_TYPE_NONE)
			{
				TQuickslot slot;
				slot.type = QUICKSLOT_TYPE_ITEM;
				slot.pos = cell;
				SetQuickslot(0, slot);
			}
		}
	}
	else
	{
		item->AddToGround(GetMapIndex(), GetXYZ());
#ifdef ENABLE_NEWSTUFF
		item->StartDestroyEvent(g_aiItemDestroyTime[ITEM_DESTROY_TIME_AUTOGIVE]);
#else
		item->StartDestroyEvent();
#endif

		if (longOwnerShip)
			item->SetOwnership(this, 300);
		else
			item->SetOwnership(this, 60);
		LogManager::instance().ItemLog(this, item, "SYSTEM_DROP", item->GetName());
	}
}

#ifdef __EXTENDED_ITEM_COUNT__
LPITEM CHARACTER::AutoGiveItem(DWORD dwItemVnum, uint16_t bCount, int iRarePct, bool bMsg, bool bDrop)
#else
LPITEM CHARACTER::AutoGiveItem(DWORD dwItemVnum, BYTE bCount, int iRarePct, bool bMsg)
#endif
{
	TItemTable* p = ITEM_MANAGER::instance().GetTable(dwItemVnum);

	if (!p)
		return NULL;

	DBManager::instance().SendMoneyLog(MONEY_LOG_DROP, dwItemVnum, bCount);

	if (p->dwFlags & ITEM_FLAG_STACKABLE && p->bType != ITEM_BLEND)
	{
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
		for (int i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
#else
		for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
#endif
		{
			LPITEM item = GetInventoryItem(i);

			if (!item)
				continue;

#ifdef ENABLE_SORT_INVENTORY_ITEMS
			if (item->GetOriginalVnum() == dwItemVnum && FN_check_item_socket(item))
#else
			if (item->GetVnum() == dwItemVnum && FN_check_item_socket(item))
#endif
			{
				if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (bCount < p->alValues[1])
						bCount = p->alValues[1];
				}

#ifdef __EXTENDED_ITEM_COUNT__
				uint16_t bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#else
				BYTE bCount2 = MIN(200 - item->GetCount(), bCount);
#endif
				bCount -= bCount2;

				item->SetCount(item->GetCount() + bCount2);

				if (bCount == 0)
				{
					return item;
				}
			}
		}
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
		for (int i = SKILL_BOOK_INVENTORY_SLOT_START; i < SKILL_BOOK_INVENTORY_SLOT_END; ++i)
		{
			LPITEM item = GetItem(TItemPos(INVENTORY, i));

			if (!item)
				continue;

			if (item->GetVnum() == dwItemVnum && FN_check_item_socket(item))
			{
				if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (bCount < p->alValues[1])
						bCount = p->alValues[1];
				}

#ifdef __EXTENDED_ITEM_COUNT__
				uint16_t bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#else
				BYTE bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#endif
				bCount -= bCount2;

				item->SetCount(item->GetCount() + bCount2);

				if (bCount == 0)
				{
					if (bMsg)
						ChatPacket(CHAT_TYPE_INFO, "[LS;444;[IN;%d]]", item->GetVnum());

					return item;
				}
			}
		}

		for (int i = UPGRADE_ITEMS_INVENTORY_SLOT_START; i < UPGRADE_ITEMS_INVENTORY_SLOT_END; ++i)
		{
			LPITEM item = GetItem(TItemPos(INVENTORY, i));

			if (!item)
				continue;

#ifdef ENABLE_SORT_INVENTORY_ITEMS
			if (item->GetOriginalVnum() == dwItemVnum && FN_check_item_socket(item))
#else
			if (item->GetVnum() == dwItemVnum && FN_check_item_socket(item))
#endif
			{
				if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (bCount < p->alValues[1])
						bCount = p->alValues[1];
				}

#ifdef __EXTENDED_ITEM_COUNT__
				uint16_t bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#else
				BYTE bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#endif
				bCount -= bCount2;

				item->SetCount(item->GetCount() + bCount2);

				if (bCount == 0)
				{
					if (bMsg)
						ChatPacket(CHAT_TYPE_INFO, "[LS;444;[IN;%d]]", item->GetVnum());

					return item;
				}
			}
		}

		for (int i = STONE_INVENTORY_SLOT_START; i < STONE_INVENTORY_SLOT_END; ++i)
		{
			LPITEM item = GetItem(TItemPos(INVENTORY, i));

			if (!item)
				continue;

#ifdef ENABLE_SORT_INVENTORY_ITEMS
			if (item->GetOriginalVnum() == dwItemVnum && FN_check_item_socket(item))
#else
			if (item->GetVnum() == dwItemVnum && FN_check_item_socket(item))
#endif
			{
				if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (bCount < p->alValues[1])
						bCount = p->alValues[1];
				}

#ifdef __EXTENDED_ITEM_COUNT__
				uint16_t bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#else
				BYTE bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#endif
				bCount -= bCount2;

				item->SetCount(item->GetCount() + bCount2);

				if (bCount == 0)
				{
					if (bMsg)
						ChatPacket(CHAT_TYPE_INFO, "[LS;444;[IN;%d]]", item->GetVnum());

					return item;
				}
			}
		}

		for (int i = BOX_INVENTORY_SLOT_START; i < BOX_INVENTORY_SLOT_END; ++i)
		{
			LPITEM item = GetItem(TItemPos(INVENTORY, i));

			if (!item)
				continue;

#ifdef ENABLE_SORT_INVENTORY_ITEMS
			if (item->GetOriginalVnum() == dwItemVnum && FN_check_item_socket(item))
#else
			if (item->GetVnum() == dwItemVnum && FN_check_item_socket(item))
#endif
			{
				if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (bCount < p->alValues[1])
						bCount = p->alValues[1];
				}

#ifdef __EXTENDED_ITEM_COUNT__
				uint16_t bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#else
				BYTE bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#endif
				bCount -= bCount2;

				item->SetCount(item->GetCount() + bCount2);

				if (bCount == 0)
				{
					if (bMsg)
						ChatPacket(CHAT_TYPE_INFO, "[LS;444;[IN;%d]]", item->GetVnum());

					return item;
				}
			}
		}

		for (int i = EFSUN_INVENTORY_SLOT_START; i < EFSUN_INVENTORY_SLOT_END; ++i)
		{
			LPITEM item = GetItem(TItemPos(INVENTORY, i));

			if (!item)
				continue;

#ifdef ENABLE_SORT_INVENTORY_ITEMS
			if (item->GetOriginalVnum() == dwItemVnum && FN_check_item_socket(item))
#else
			if (item->GetVnum() == dwItemVnum && FN_check_item_socket(item))
#endif
			{
				if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (bCount < p->alValues[1])
						bCount = p->alValues[1];
				}

#ifdef __EXTENDED_ITEM_COUNT__
				uint16_t bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#else
				BYTE bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#endif
				bCount -= bCount2;

				item->SetCount(item->GetCount() + bCount2);

				if (bCount == 0)
				{
					if (bMsg)
						ChatPacket(CHAT_TYPE_INFO, "[LS;444;%[IN;%d]", item->GetVnum());

					return item;
				}
			}
		}

		for (int i = CICEK_INVENTORY_SLOT_START; i < CICEK_INVENTORY_SLOT_END; ++i)
		{
			LPITEM item = GetItem(TItemPos(INVENTORY, i));

			if (!item)
				continue;

#ifdef ENABLE_SORT_INVENTORY_ITEMS
			if (item->GetOriginalVnum() == dwItemVnum && FN_check_item_socket(item))
#else
			if (item->GetVnum() == dwItemVnum && FN_check_item_socket(item))
#endif
			{
				if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (bCount < p->alValues[1])
						bCount = p->alValues[1];
				}

#ifdef __EXTENDED_ITEM_COUNT__
				uint16_t bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#else
				BYTE bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#endif
				bCount -= bCount2;

				item->SetCount(item->GetCount() + bCount2);

				if (bCount == 0)
				{
					if (bMsg)
						ChatPacket(CHAT_TYPE_INFO, "[LS;444;[IN;%d]]", item->GetVnum());

					return item;
				}
			}
		}
#endif
	}

	LPITEM item = ITEM_MANAGER::instance().CreateItem(dwItemVnum, bCount, 0, true);

	if (!item)
	{
		sys_err("cannot create item by vnum %u (name: %s)", dwItemVnum, GetName());
		return NULL;
	}

	if (item->GetType() == ITEM_BLEND)
	{
		for (int i = 0; i < INVENTORY_MAX_NUM; i++)
		{
			LPITEM inv_item = GetInventoryItem(i);

			if (inv_item == NULL) continue;

			if (inv_item->GetType() == ITEM_BLEND)
			{
				if (inv_item->GetVnum() == item->GetVnum())
				{
					if (inv_item->GetSocket(0) == item->GetSocket(0) &&
						inv_item->GetSocket(1) == item->GetSocket(1) &&
						inv_item->GetSocket(2) == item->GetSocket(2) &&
						inv_item->GetCount() < g_bItemCountLimit)
					{
						inv_item->SetCount(inv_item->GetCount() + item->GetCount());
						M2_DESTROY_ITEM(item);
						return inv_item;
					}
				}
			}
		}
	}

	int iEmptyCell;
	if (item->IsDragonSoul())
	{
		iEmptyCell = GetEmptyDragonSoulInventory(item);
	}
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	else if (item->IsSkillBook())
	{
		iEmptyCell = GetEmptySkillBookInventory(item->GetSize());
	}
	else if (item->IsUpgradeItem())
	{
		iEmptyCell = GetEmptyUpgradeItemsInventory(item->GetSize());
	}
	else if (item->IsStone())
	{
		iEmptyCell = GetEmptyStoneInventory(item->GetSize());
	}
	else if (item->IsBox())
	{
		iEmptyCell = GetEmptyBoxInventory(item->GetSize());
	}
	else if (item->IsEfsun())
	{
		iEmptyCell = GetEmptyEfsunInventory(item->GetSize());
	}
	else if (item->IsCicek())
	{
		iEmptyCell = GetEmptyCicekInventory(item->GetSize());
	}
#endif
	else
		iEmptyCell = GetEmptyInventory(item->GetSize());

	if (iEmptyCell != -1)
	{

		if (bMsg)
		{
			#if defined(__CHATTING_WINDOW_RENEWAL__)
			ChatPacket(CHAT_TYPE_ITEM_INFO, "[LS;444;[IN;%d]]", item->GetVnum());
			#else
			ChatPacket(CHAT_TYPE_INFO, "[LS;444;[IN;%d]]", item->GetVnum());
			#endif
		}

		if (item->IsDragonSoul())
			item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyCell));
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
		else if (item->IsSkillBook())
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
		else if (item->IsUpgradeItem())
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
		else if (item->IsStone())
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
		else if (item->IsBox())
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
		else if (item->IsEfsun())
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
		else if (item->IsCicek())
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
#endif
		else
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
		LogManager::instance().ItemLog(this, item, "SYSTEM", item->GetName());

		if (item->GetType() == ITEM_USE && item->GetSubType() == USE_POTION)
		{
			TQuickslot* pSlot;

			if (GetQuickslot(0, &pSlot) && pSlot->type == QUICKSLOT_TYPE_NONE)
			{
				TQuickslot slot;
				slot.type = QUICKSLOT_TYPE_ITEM;
				slot.pos = iEmptyCell;
				SetQuickslot(0, slot);
			}
		}
	}
	else
	{
		if (!bDrop
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
			&& (m_bQuickOpenInProgress || (!item->IsEfsun() && !item->IsCicek()))
#endif
		)
		{
			M2_DESTROY_ITEM(item);
			return nullptr;
		}
		item->AddToGround(GetMapIndex(), GetXYZ());
#ifdef ENABLE_NEWSTUFF
		item->StartDestroyEvent(g_aiItemDestroyTime[ITEM_DESTROY_TIME_AUTOGIVE]);
#else
		item->StartDestroyEvent();
#endif

		if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_DROP))
			item->SetOwnership(this, 300);
		else
			item->SetOwnership(this, 60);
		LogManager::instance().ItemLog(this, item, "SYSTEM_DROP", item->GetName());
	}

	sys_log(0,
		"7: %d %d", dwItemVnum, bCount);
	return item;
}

bool CHARACTER::GiveItem(LPCHARACTER victim, TItemPos Cell)
{
	if (!CanHandleItem())
		return false;

	// @fixme150 BEGIN
	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;2413]");
		return false;
	}
	// @fixme150 END

	LPITEM item = GetItem(Cell);

	if (item && !item->IsExchanging())
	{
		if (victim->CanReceiveItem(this, item))
		{
			victim->ReceiveItem(this, item);
			return true;
		}
	}

	return false;
}

bool CHARACTER::CanReceiveItem(LPCHARACTER from, LPITEM item) const
{
	if (IsPC())
		return false;

	// TOO_LONG_DISTANCE_EXCHANGE_BUG_FIX
	if (DISTANCE_SQRT(GetX() - from->GetX(), GetY() - from->GetY()) > 2000)
		return false;
	// END_OF_TOO_LONG_DISTANCE_EXCHANGE_BUG_FIX

	switch (GetRaceNum())
	{
	case fishing::CAMPFIRE_MOB:
		if (item->GetType() == ITEM_FISH &&
			(item->GetSubType() == FISH_ALIVE || item->GetSubType() == FISH_DEAD))
			return true;
		break;

	case fishing::FISHER_MOB:
		if (item->GetType() == ITEM_ROD)
			return true;
		break;

		// BUILDING_NPC
	case BLACKSMITH_WEAPON_MOB:
	case DEVILTOWER_BLACKSMITH_WEAPON_MOB:
		if (item->GetType() == ITEM_WEAPON &&
			item->GetRefinedVnum())
			return true;
		else
			return false;
		break;

	case BLACKSMITH_ARMOR_MOB:
	case DEVILTOWER_BLACKSMITH_ARMOR_MOB:
		if (item->GetType() == ITEM_ARMOR &&
			(item->GetSubType() == ARMOR_BODY || item->GetSubType() == ARMOR_SHIELD || item->GetSubType() == ARMOR_HEAD) &&
			item->GetRefinedVnum())
			return true;
		else
			return false;
		break;

	case BLACKSMITH_ACCESSORY_MOB:
	case DEVILTOWER_BLACKSMITH_ACCESSORY_MOB:
		if (item->GetType() == ITEM_ARMOR &&
			!(item->GetSubType() == ARMOR_BODY || item->GetSubType() == ARMOR_SHIELD || item->GetSubType() == ARMOR_HEAD) &&
			item->GetRefinedVnum())
			return true;
		else
			return false;
		break;
		// END_OF_BUILDING_NPC

	case BLACKSMITH_MOB:
		if (item->GetRefinedVnum() && item->GetRefineSet() < 500)
		{
			return true;
		}
		else
		{
			return false;
		}

	case BLACKSMITH2_MOB:
		if (item->GetRefineSet() >= 500)
		{
			return true;
		}
		else
		{
			return false;
		}

	case ALCHEMIST_MOB:
		if (item->GetRefinedVnum())
			return true;
		break;

	case 20101:
	case 20102:
	case 20103:

		if (item->GetVnum() == ITEM_REVIVE_HORSE_1)
		{
			if (!IsDead())
			{
				from->ChatPacket(CHAT_TYPE_INFO, "[LS;452]");
				return false;
			}
			return true;
		}
		else if (item->GetVnum() == ITEM_HORSE_FOOD_1)
		{
			if (IsDead())
			{
				from->ChatPacket(CHAT_TYPE_INFO, "[LS;453]");
				return false;
			}
			return true;
		}
		else if (item->GetVnum() == ITEM_HORSE_FOOD_2 || item->GetVnum() == ITEM_HORSE_FOOD_3)
		{
			return false;
		}
		break;
	case 20104:
	case 20105:
	case 20106:

		if (item->GetVnum() == ITEM_REVIVE_HORSE_2)
		{
			if (!IsDead())
			{
				from->ChatPacket(CHAT_TYPE_INFO, "[LS;452]");
				return false;
			}
			return true;
		}
		else if (item->GetVnum() == ITEM_HORSE_FOOD_2)
		{
			if (IsDead())
			{
				from->ChatPacket(CHAT_TYPE_INFO, "[LS;453]");
				return false;
			}
			return true;
		}
		else if (item->GetVnum() == ITEM_HORSE_FOOD_1 || item->GetVnum() == ITEM_HORSE_FOOD_3)
		{
			return false;
		}
		break;
	case 20107:
	case 20108:
	case 20109:

		if (item->GetVnum() == ITEM_REVIVE_HORSE_3)
		{
			if (!IsDead())
			{
				from->ChatPacket(CHAT_TYPE_INFO, "[LS;452]");
				return false;
			}
			return true;
		}
		else if (item->GetVnum() == ITEM_HORSE_FOOD_3)
		{
			if (IsDead())
			{
				from->ChatPacket(CHAT_TYPE_INFO, "[LS;453]");
				return false;
			}
			return true;
		}
		else if (item->GetVnum() == ITEM_HORSE_FOOD_1 || item->GetVnum() == ITEM_HORSE_FOOD_2)
		{
			return false;
		}
		break;
	}

	//if (IS_SET(item->GetFlag(), ITEM_FLAG_QUEST_GIVE))
	{
		return true;
	}

	return false;
}

void CHARACTER::ReceiveItem(LPCHARACTER from, LPITEM item)
{
	if (IsPC())
		return;

	switch (GetRaceNum())
	{
	case fishing::CAMPFIRE_MOB:
		if (item->GetType() == ITEM_FISH && (item->GetSubType() == FISH_ALIVE || item->GetSubType() == FISH_DEAD))
			fishing::Grill(from, item);
		else
		{
			// TAKE_ITEM_BUG_FIX
			from->SetQuestNPCID(GetVID());
			// END_OF_TAKE_ITEM_BUG_FIX
			quest::CQuestManager::instance().TakeItem(from->GetPlayerID(), GetRaceNum(), item);
		}
		break;

		// DEVILTOWER_NPC
	case DEVILTOWER_BLACKSMITH_WEAPON_MOB:
	case DEVILTOWER_BLACKSMITH_ARMOR_MOB:
	case DEVILTOWER_BLACKSMITH_ACCESSORY_MOB:
		if (item->GetRefinedVnum() != 0 && item->GetRefineSet() != 0 && item->GetRefineSet() < 500)
		{
			from->SetRefineNPC(this);
			from->RefineInformation(item->GetCell(), REFINE_TYPE_MONEY_ONLY);
		}
		else
		{
			from->ChatPacket(CHAT_TYPE_INFO, "[LS;1002]");
		}
		break;
		// END_OF_DEVILTOWER_NPC

	case BLACKSMITH_MOB:
	case BLACKSMITH2_MOB:
	case BLACKSMITH_WEAPON_MOB:
	case BLACKSMITH_ARMOR_MOB:
	case BLACKSMITH_ACCESSORY_MOB:
		if (item->GetRefinedVnum())
		{
			from->SetRefineNPC(this);
			from->RefineInformation(item->GetCell(), REFINE_TYPE_NORMAL);
		}
		else
		{
			from->ChatPacket(CHAT_TYPE_INFO, "[LS;1002]");
		}
		break;

	case 20101:
	case 20102:
	case 20103:
	case 20104:
	case 20105:
	case 20106:
	case 20107:
	case 20108:
	case 20109:
		if (item->GetVnum() == ITEM_REVIVE_HORSE_1 ||
			item->GetVnum() == ITEM_REVIVE_HORSE_2 ||
			item->GetVnum() == ITEM_REVIVE_HORSE_3)
		{
			from->ReviveHorse();
			item->SetCount(item->GetCount() - 1);
			from->ChatPacket(CHAT_TYPE_INFO, "[LS;454]");
		}
		else if (item->GetVnum() == ITEM_HORSE_FOOD_1 ||
			item->GetVnum() == ITEM_HORSE_FOOD_2 ||
			item->GetVnum() == ITEM_HORSE_FOOD_3)
		{
			from->FeedHorse();
			from->ChatPacket(CHAT_TYPE_INFO, "[LS;455]");
			item->SetCount(item->GetCount() - 1);
			EffectPacket(SE_HPUP_RED);
		}
		break;

	default:
		sys_log(0, "TakeItem %s %d %s", from->GetName(), GetRaceNum(), item->GetName());
		from->SetQuestNPCID(GetVID());
		quest::CQuestManager::instance().TakeItem(from->GetPlayerID(), GetRaceNum(), item);
		break;
	}
}

bool CHARACTER::IsEquipUniqueItem(DWORD dwItemVnum) const
{
	{
		LPITEM u = GetWear(WEAR_UNIQUE1);

		if (u && u->GetVnum() == dwItemVnum)
			return true;
	}

	{
		LPITEM u = GetWear(WEAR_UNIQUE2);

		if (u && u->GetVnum() == dwItemVnum)
			return true;
	}

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	{
		LPITEM u = GetWear(WEAR_COSTUME_MOUNT);

		if (u && u->GetVnum() == dwItemVnum)
			return true;
	}
#endif

	if (dwItemVnum == UNIQUE_ITEM_RING_OF_LANGUAGE)
		return IsEquipUniqueItem(UNIQUE_ITEM_RING_OF_LANGUAGE_SAMPLE);

	return false;
}

// CHECK_UNIQUE_GROUP
bool CHARACTER::IsEquipUniqueGroup(DWORD dwGroupVnum) const
{
	{
		LPITEM u = GetWear(WEAR_UNIQUE1);

		if (u && u->GetSpecialGroup() == (int)dwGroupVnum)
			return true;
	}

	{
		LPITEM u = GetWear(WEAR_UNIQUE2);

		if (u && u->GetSpecialGroup() == (int)dwGroupVnum)
			return true;
	}

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	{
		LPITEM u = GetWear(WEAR_COSTUME_MOUNT);

		if (u && u->GetSpecialGroup() == (int)dwGroupVnum)
			return true;
	}
#endif

	return false;
}
// END_OF_CHECK_UNIQUE_GROUP

void CHARACTER::SetRefineMode(int iAdditionalCell)
{
	m_iRefineAdditionalCell = iAdditionalCell;
	m_bUnderRefine = true;
}

void CHARACTER::ClearRefineMode()
{
	m_bUnderRefine = false;
	SetRefineNPC(NULL);
}

bool CHARACTER::GiveItemFromSpecialItemGroup(DWORD dwGroupNum, std::vector<DWORD> &dwItemVnums,
                                            std::vector<DWORD> &dwItemCounts, std::vector <LPITEM> &item_gets, int &count, bool sendMessage)
{
	const CSpecialItemGroup* pGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(dwGroupNum);

	if (!pGroup)
	{
		sys_err("cannot find special item group %d", dwGroupNum);
		return false;
	}

	std::vector <int> idxes;
	int n = pGroup->GetMultiIndex(idxes);

	bool bSuccess;

	for (int i = 0; i < n; i++)
	{
		bSuccess = false;
		int idx = idxes[i];
		DWORD dwVnum = pGroup->GetVnum(idx);
		DWORD dwCount = pGroup->GetCount(idx);
		int	iRarePct = pGroup->GetRarePct(idx);
		LPITEM item_get = NULL;
		switch (dwVnum)
		{
			case CSpecialItemGroup::GOLD:
				PointChange(POINT_GOLD, dwCount);
				LogManager::instance().CharLog(this, dwCount, "TREASURE_GOLD", "");

				bSuccess = true;
				break;
			case CSpecialItemGroup::EXP:
				{
					PointChange(POINT_EXP, dwCount);
					LogManager::instance().CharLog(this, dwCount, "TREASURE_EXP", "");

					bSuccess = true;
				}
				break;

			case CSpecialItemGroup::MOB:
				{
					sys_log(0, "CSpecialItemGroup::MOB %d", dwCount);
					int x = GetX() + number(-500, 500);
					int y = GetY() + number(-500, 500);

					LPCHARACTER ch = CHARACTER_MANAGER::instance().SpawnMob(dwCount, GetMapIndex(), x, y, 0, true, -1);
					if (ch)
						ch->SetAggressive();
					bSuccess = true;
				}
				break;
			case CSpecialItemGroup::SLOW:
				{
					sys_log(0, "CSpecialItemGroup::SLOW %d", -(int)dwCount);
					AddAffect(AFFECT_SLOW, POINT_MOV_SPEED, -(int)dwCount, AFF_SLOW, 300, 0, true);
					bSuccess = true;
				}
				break;
			case CSpecialItemGroup::DRAIN_HP:
				{
					int iDropHP = GetMaxHP() * dwCount / 100;
					sys_log(0, "CSpecialItemGroup::DRAIN_HP %d", -iDropHP);
					iDropHP = MIN(iDropHP, GetHP() - 1);
					sys_log(0, "CSpecialItemGroup::DRAIN_HP %d", -iDropHP);
					PointChange(POINT_HP, -iDropHP);
					bSuccess = true;
				}
				break;
			case CSpecialItemGroup::POISON:
				{
					AttackedByPoison(NULL);
					bSuccess = true;
				}
				break;
	#ifdef ENABLE_WOLFMAN_CHARACTER
			case CSpecialItemGroup::BLEEDING:
				{
					AttackedByBleeding(NULL);
					bSuccess = true;
				}
				break;
	#endif
			case CSpecialItemGroup::MOB_GROUP:
				{
					int sx = GetX() - number(300, 500);
					int sy = GetY() - number(300, 500);
					int ex = GetX() + number(300, 500);
					int ey = GetY() + number(300, 500);
					CHARACTER_MANAGER::instance().SpawnGroup(dwCount, GetMapIndex(), sx, sy, ex, ey, NULL, true);

					bSuccess = true;
				}
				break;
			default:
				{
					TItemTable* pTable = ITEM_MANAGER::instance().GetTable(dwVnum);
					const bool bIsDS = (pTable && pTable->bType == ITEM_DS);
					// DS items must never drop to ground from a chest (causes lag, enables
					// cherry-picking). Pass bDrop=false so AutoGiveItem either places the
					// item in DS inventory or destroys it cleanly.
					item_get = AutoGiveItem(dwVnum, dwCount, iRarePct, true, !m_bQuickOpenInProgress && !bIsDS);

					if (item_get)
					{
						bSuccess = true;
					}
					else if (bIsDS)
					{
						// DS inventory was full for this item's type - chest is still
						// consumed but no item is given.
						bSuccess = true;
					}
				}
				break;
		}

		if (bSuccess)
		{
			dwItemVnums.emplace_back(dwVnum);
			dwItemCounts.emplace_back(dwCount);
			item_gets.emplace_back(item_get);
			count++;

		}
		else
		{
			return false;
		}
	}
	return bSuccess;
}

// NEW_HAIR_STYLE_ADD
bool CHARACTER::ItemProcess_Hair(LPITEM item, int iDestCell)
{
	if (item->CheckItemUseLevel(GetLevel()) == false)
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;456]");
		return false;
	}

	DWORD hair = item->GetVnum();

	switch (GetJob())
	{
	case JOB_WARRIOR:
		hair -= 72000;
		break;

	case JOB_ASSASSIN:
		hair -= 71250;
		break;

	case JOB_SURA:
		hair -= 70500;
		break;

	case JOB_SHAMAN:
		hair -= 69750;
		break;
#ifdef ENABLE_WOLFMAN_CHARACTER
	case JOB_WOLFMAN:
		break;
#endif
	default:
		return false;
		break;
	}

	if (hair == GetPart(PART_HAIR))
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;457]");
		return true;
	}

	item->SetCount(item->GetCount() - 1);

	SetPart(PART_HAIR, hair);
	UpdatePacket();

	return true;
}
// END_NEW_HAIR_STYLE_ADD


static int POLYMORPH_AFFECT[1][40 + 1] = {
	{1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10/*20*/,11,11,12,12,13,13,14,14,15,15/*30*/,16,17,18,19,20,21,22,23,24,25,25 },
};

bool CHARACTER::CanProcessPolymorph() const
{
	int map_table[2] = { 12, 39 };
	
	for (BYTE i = 0; i < 2; ++i)
	{
		if (GetMapIndex() >= map_table[i] * 10000 && GetMapIndex() < (map_table[i] + 1) * 10000)
			return true;
	}
	
	return false;
}

bool CHARACTER::ItemProcess_Polymorph(LPITEM item)
{
	if (!CanProcessPolymorph())
	{
		return false;
	}
	
	if (IsPolymorphed())
	{
		SetPolymorph(0);
		RemoveAffect(AFFECT_POLYMORPH);
		return false;
	}
	if (IsHorseRiding())
		StopRiding();

	DWORD dwVnum = item->GetSocket(0);

	if (item->GetVnum() == 71093)
	{
		dwVnum = 200104;
	}

	if (dwVnum == 0)
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;460]");
		item->SetCount(item->GetCount() - 1);
		return false;
	}

#ifdef __ENABLE_POLYMORPH_SYSTEM__
	if (GetPolySkin())
	{
		DWORD dwSkinVnum = CPolymorphMgr::instance().GetSkinVnum(GetPolySkin());
		if (dwSkinVnum)
			dwVnum = dwSkinVnum;
	}
#endif

	const CMob* pMob = CMobManager::instance().Get(dwVnum);

	if (pMob == NULL)
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;460]");
		item->SetCount(item->GetCount() - 1);
		return false;
	}

	switch (item->GetVnum())
	{
	case 70104:
	case 70105:
	case 70106:
	case 70107:
	case 71093:
	{

		sys_log(0, "USE_POLYMORPH_BALL PID(%d) vnum(%d)", GetPlayerID(), dwVnum);


		int iPolymorphLevelLimit = MAX(0, 20 - GetLevel() * 3 / 10);
		if (pMob->m_table.bLevel >= GetLevel() + iPolymorphLevelLimit)
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;461]");
			return false;
		}

		int iDuration = GetSkillLevel(POLYMORPH_SKILL_ID) == 0 ? 5 : (5 + (5 + GetSkillLevel(POLYMORPH_SKILL_ID) / 40 * 50));
		iDuration *= 60;

#ifdef __ENABLE_POLYMORPH_SYSTEM__
		RemoveAffect(AFFECT_POLYMORPH); // Remove Old Bonuses.
#endif

		DWORD dwBonus = 0;

		if (true == LC_IsYMIR() || true == LC_IsKorea())
		{
			dwBonus = GetSkillLevel(POLYMORPH_SKILL_ID) + 60;
		}
		else
		{
			dwBonus = (2 + GetSkillLevel(POLYMORPH_SKILL_ID) / 40) * 100;
		}

		AddAffect(AFFECT_POLYMORPH, POINT_POLYMORPH, dwVnum, AFF_POLYMORPH, iDuration, 0, true);
		//AddAffect(AFFECT_POLYMORPH, POINT_ATT_BONUS, dwBonus, AFF_POLYMORPH, iDuration, 0, false);

		AddAffect(AFFECT_POLYMORPH, POINT_MOV_SPEED, 200, AFF_POLYMORPH, iDuration, 0, false);

		DWORD skillLevel = GetSkillLevel(POLYMORPH_SKILL_ID);
		//AddAffect(AFFECT_POLYMORPH, POINT_NORMAL_HIT_DAMAGE_BONUS, POLYMORPH_AFFECT[0][skillLevel], AFF_POLYMORPH, iDuration, 0, false);

#ifdef __ENABLE_POLYMORPH_SYSTEM__
		for (uint8_t i = 0; i < MAX_POLY_BONUS; i++)
		{
			const auto& rkAttribute = item->GetAttribute(i);
			if (rkAttribute.bType && rkAttribute.sValue)
			{
				AddAffect(AFFECT_POLYMORPH, aApplyInfo[rkAttribute.bType].bPointType, rkAttribute.sValue, AFF_POLYMORPH, iDuration, 0, false);
			}
		}
#endif
#ifdef _ENABLE_BATTLEPASS_
		BattlePassManager::Instance().Notify(MISSION_TYPE_POLYMORPH, this, 0, 1);
#endif

		item->SetCount(item->GetCount() - 1);
	}
	break;

	case 50322:
	{
		sys_log(0, "USE_POLYMORPH_BOOK: %s(%u) vnum(%u)", GetName(), GetPlayerID(), dwVnum);

		if (CPolymorphUtils::instance().PolymorphCharacter(this, item, pMob) == true)
		{
			CPolymorphUtils::instance().UpdateBookPracticeGrade(this, item);
		}
		else
		{
		}
	}
	break;

	default:
		sys_err("POLYMORPH invalid item passed PID(%d) vnum(%d)", GetPlayerID(), item->GetOriginalVnum());
		return false;
	}

	return true;
}

bool CHARACTER::CanDoCube() const
{
	if (m_bIsObserver)	return false;
	if (GetShop())		return false;
	if (GetMyShop())	return false;
	if (m_bUnderRefine)	return false;
	if (IsWarping())	return false;
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	if (GetOfflineShop()) return false;
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
	if (IsEditingPrivateShop() || IsShopSearch() || GetMyPrivateShop()) return false;
#endif

	return true;
}

bool CHARACTER::UnEquipSpecialRideUniqueItem()
{
	LPITEM Unique1 = GetWear(WEAR_UNIQUE1);
	LPITEM Unique2 = GetWear(WEAR_UNIQUE2);
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	LPITEM Unique3 = GetWear(WEAR_COSTUME_MOUNT);
#endif
#ifdef ENABLE_NEW_MOUNT_SYSTEM
	LPITEM MountCostume = GetWear(WEAR_COSTUME_MOUNT);
#endif

	if (NULL != Unique1)
	{
		if (UNIQUE_GROUP_SPECIAL_RIDE == Unique1->GetSpecialGroup())
		{
			return UnequipItem(Unique1);
		}
	}

	if (NULL != Unique2)
	{
		if (UNIQUE_GROUP_SPECIAL_RIDE == Unique2->GetSpecialGroup())
		{
			return UnequipItem(Unique2);
		}
	}

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	if (NULL != Unique3)
	{
		if (UNIQUE_GROUP_SPECIAL_RIDE == Unique3->GetSpecialGroup())
		{
			return UnequipItem(Unique3);
		}
	}
#endif

#ifdef ENABLE_NEW_MOUNT_SYSTEM
	if (MountCostume)
		return UnequipItem(MountCostume);
#endif

	return true;
}

void CHARACTER::AutoRecoveryItemProcess(const EAffectTypes type)
{
	if (true == IsDead() || true == IsStun())
		return;

	if (false == IsPC())
		return;

	if (AFFECT_AUTO_HP_RECOVERY != type && AFFECT_AUTO_SP_RECOVERY != type)
		return;

	if (NULL != FindAffect(AFFECT_STUN))
		return;

	{
		const DWORD stunSkills[] = { SKILL_TANHWAN, SKILL_GEOMPUNG, SKILL_BYEURAK, SKILL_GIGUNG };

		for (size_t i = 0; i < sizeof(stunSkills) / sizeof(DWORD); ++i)
		{
			const CAffect* p = FindAffect(stunSkills[i]);

			if (NULL != p && AFF_STUN == p->dwFlag)
				return;
		}
	}

	const CAffect* pAffect = FindAffect(type);

	if (pAffect)
	{
		LPITEM pItem = FindItemByID(pAffect->dwFlag);

		if (NULL != pItem && true == pItem->GetSocket(0))
		{
			if (!CArenaManager::instance().IsArenaMap(GetMapIndex())
#ifdef ENABLE_NEWSTUFF
				&& !(g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(pItem->GetVnum()))
#endif
				)
			{
				int32_t amount = 0;

				if (AFFECT_AUTO_HP_RECOVERY == type)
				{
					amount = GetMaxHP() - (GetHP() + GetPoint(POINT_HP_RECOVERY));
				}
				else if (AFFECT_AUTO_SP_RECOVERY == type)
				{
					amount = GetMaxSP() - (GetSP() + GetPoint(POINT_SP_RECOVERY));
				}

				if (amount > 0)
				{
					if (AFFECT_AUTO_HP_RECOVERY == type)
					{
						PointChange(POINT_HP_RECOVERY, amount);
						EffectPacket(SE_AUTO_HPUP);
					}
					else if (AFFECT_AUTO_SP_RECOVERY == type)
					{
						PointChange(POINT_SP_RECOVERY, amount);
						EffectPacket(SE_AUTO_SPUP);
					}
				}
			}
			else
			{
				pItem->Lock(false);
				pItem->SetSocket(0, false);
				RemoveAffect(const_cast<CAffect*>(pAffect));
			}
		}
		else
		{
			RemoveAffect(const_cast<CAffect*>(pAffect));
		}
	}
}

bool CHARACTER::IsValidItemPosition(TItemPos Pos) const
{
	BYTE window_type = Pos.window_type;
	WORD cell = Pos.cell;

	switch (window_type)
	{
	case RESERVED_WINDOW:
		return false;

	case INVENTORY:
	case EQUIPMENT:
		return cell < (INVENTORY_AND_EQUIP_SLOT_MAX);

	case DRAGON_SOUL_INVENTORY:
		return cell < (DRAGON_SOUL_INVENTORY_MAX_NUM);

	case SAFEBOX:
		if (NULL != m_pkSafebox)
			return m_pkSafebox->IsValidPosition(cell);
		else
			return false;

	case MALL:
		if (NULL != m_pkMall)
			return m_pkMall->IsValidPosition(cell);
		else
			return false;
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	case BUFF_EQUIPMENT:
		return cell < (BUFF_WINDOW_SLOT_MAX_NUM);
#endif
#ifdef ENABLE_SWITCHBOT
	case SWITCHBOT:
		return cell < SWITCHBOT_SLOT_COUNT;
#endif
	default:
		return false;
	}
}

#define VERIFY_MSG(exp, msg)  \
	if (true == (exp)) { \
			ChatPacket(CHAT_TYPE_INFO, msg); \
			return false; \
	}

bool CHARACTER::CanEquipNow(const LPITEM item, const TItemPos& srcCell, const TItemPos& destCell) /*const*/
{
	const TItemTable* itemTable = item->GetProto();
	//BYTE itemType = item->GetType();
	//BYTE itemSubType = item->GetSubType();

	switch (GetJob())
	{
	case JOB_WARRIOR:
		if (item->GetAntiFlag() & ITEM_ANTIFLAG_WARRIOR)
			return false;
		break;

	case JOB_ASSASSIN:
		if (item->GetAntiFlag() & ITEM_ANTIFLAG_ASSASSIN)
			return false;
		break;

	case JOB_SHAMAN:
		if (item->GetAntiFlag() & ITEM_ANTIFLAG_SHAMAN)
			return false;
		break;

	case JOB_SURA:
		if (item->GetAntiFlag() & ITEM_ANTIFLAG_SURA)
			return false;
		break;
#ifdef ENABLE_WOLFMAN_CHARACTER
	case JOB_WOLFMAN:
		if (item->GetAntiFlag() & ITEM_ANTIFLAG_WOLFMAN)
			return false;
		break;
#endif
	}

	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		int32_t limit = itemTable->aLimits[i].lValue;
		switch (itemTable->aLimits[i].bType)
		{
		case LIMIT_LEVEL:
			if (GetLevel() < limit)
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;462]");
				return false;
			}
			break;
				
#ifdef ENABLE_SECONDARY_LEVEL
		case LIMIT_SECONDARY_LEVEL:
			if (GetSecondaryLevel() < limit)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("TOO_LOW_SECONDARY_LEVEL"));
				return false;
			}
			break;
#endif
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
		case LIMIT_ENLIGHT_LEVEL:
			if (GetEnlightLevel() < limit)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("TOO_LOW_ENLIGHTENMENT_LEVEL"));
				return false;
			}
			break;
#endif

#ifdef ENABLE_MOUNT_SYSTEM
		case LIMIT_MOUNT_LEVEL:
		{
			if (GetMountLevel() < limit)
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;10013]");
				return false;
			}
		} break;
#endif

		case LIMIT_STR:
			if (GetPoint(POINT_ST) < limit)
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;463]");
				return false;
			}
			break;

		case LIMIT_INT:
			if (GetPoint(POINT_IQ) < limit)
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;464]");
				return false;
			}
			break;

		case LIMIT_DEX:
			if (GetPoint(POINT_DX) < limit)
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;465]");
				return false;
			}
			break;

		case LIMIT_CON:
			if (GetPoint(POINT_HT) < limit)
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;466]");
				return false;
			}
			break;
		}
	}

	if (item->GetWearFlag() & WEARABLE_UNIQUE)
	{
		if ((GetWear(WEAR_UNIQUE1) && GetWear(WEAR_UNIQUE1)->IsSameSpecialGroup(item)) ||
			(GetWear(WEAR_UNIQUE2) && GetWear(WEAR_UNIQUE2)->IsSameSpecialGroup(item))
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
			|| (GetWear(WEAR_COSTUME_MOUNT) && GetWear(WEAR_COSTUME_MOUNT)->IsSameSpecialGroup(item))
#endif
			)
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;468]");
			return false;
		}

		if (marriage::CManager::instance().IsMarriageUniqueItem(item->GetVnum()) &&
			!marriage::CManager::instance().IsMarried(GetPlayerID()))
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;467]");
			return false;
		}

	}

	if (item->GetType() == ITEM_RING)
	{
		if ((GetWear(WEAR_RING1) && GetWear(WEAR_RING1)->IsRingSameGroup(item)) || (GetWear(WEAR_RING1) && GetWear(WEAR_RING1)->IsSameSubType(item)) 
		|| (GetWear(WEAR_RING2) && GetWear(WEAR_RING2)->IsRingSameGroup(item)) || (GetWear(WEAR_RING2) && GetWear(WEAR_RING2)->IsSameSubType(item)))
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;468]");
			return false;
		}
	}

#ifdef ENABLE_NEW_PET_SYSTEM
	if (item->GetType() == ITEM_NEW_PET_EQ)
	{
		if ((GetWear(WEAR_NEW_PET_EQ0) && GetWear(WEAR_NEW_PET_EQ0)->IsSameSubType(item)) ||
			(GetWear(WEAR_NEW_PET_EQ1) && GetWear(WEAR_NEW_PET_EQ1)->IsSameSubType(item)) ||
			(GetWear(WEAR_NEW_PET_EQ2) && GetWear(WEAR_NEW_PET_EQ2)->IsSameSubType(item)) ||
			(GetWear(WEAR_NEW_PET_EQ3) && GetWear(WEAR_NEW_PET_EQ3)->IsSameSubType(item)) ||
			(GetWear(WEAR_NEW_PET_EQ4) && GetWear(WEAR_NEW_PET_EQ4)->IsSameSubType(item)))
		{
			return false;
		}
	}
#endif
	return true;
}

bool CHARACTER::CanUnequipNow(const LPITEM item, const TItemPos& srcCell, const TItemPos& destCell) /*const*/
{
	if (ITEM_BELT == item->GetType())
		VERIFY_MSG(CBeltInventoryHelper::IsExistItemInBeltInventory(this), "��Ʈ �κ��丮�� �������� �����ϸ� ������ �� �����ϴ�.");

	if (IS_SET(item->GetFlag(), ITEM_FLAG_IRREMOVABLE))
		return false;

	{
		int pos = -1;

		if (item->IsDragonSoul())
			pos = GetEmptyDragonSoulInventory(item);
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
		else if (item->IsSkillBook())
			pos = GetEmptySkillBookInventory(item->GetSize());
		else if (item->IsUpgradeItem())
			pos = GetEmptyUpgradeItemsInventory(item->GetSize());
		else if (item->IsStone())
			pos = GetEmptyStoneInventory(item->GetSize());
		else if (item->IsBox())
			pos = GetEmptyBoxInventory(item->GetSize());
		else if (item->IsEfsun())
			pos = GetEmptyEfsunInventory(item->GetSize());
		else if (item->IsCicek())
			pos = GetEmptyCicekInventory(item->GetSize());
#endif
		else
			pos = GetEmptyInventory(item->GetSize());

		VERIFY_MSG(-1 == pos, "There isn't enough space in the inventory.");
	}
	
#ifdef __RENEWAL_MOUNT__
	if (item->IsCostumeMountItem())
	{
		if(IsRiding())
		{
			return false;
		}
	}
#endif

	if (item->GetType() == ITEM_WEAPON)
	{
		if (IsAffectFlag(AFF_GWIGUM))
			RemoveAffect(SKILL_GWIGEOM);

		if (IsAffectFlag(AFF_GEOMGYEONG))
			RemoveAffect(SKILL_GEOMKYUNG);
	}

	return true;
}

#ifdef ENABLE_EXTENDED_BLEND
int CHARACTER::GetAffectType(LPITEM item)
{
	switch (item->GetValue(1))
	{
	case 1: return AFFECT_BLEND_EX;
	case 2: return AFFECT_WATER;
	case 3: return AFFECT_MALL_EX;
	default: return AFFECT_BLEND;
	}
}
#endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
static bool FN_check_item_sex_buff(DWORD dwSex, LPITEM item)
{
	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_MALE)) {
		if (0 == dwSex)
			return false;
	}

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_FEMALE)) {
		if (1 == dwSex)
			return false;
	}

	return true;
}

static bool FN_check_item_level_buff(DWORD dwLevel, LPITEM item)
{
	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		int32_t limitValue = item->GetProto()->aLimits[i].lValue;

		switch (item->GetProto()->aLimits[i].bType)
		{
		case LIMIT_LEVEL:
			if (dwLevel < limitValue)
				return false;
			break;
		}
	}
	return true;
}

LPITEM CHARACTER::GetBuffWear(UINT bCell) const
{
	if (bCell >= BUFF_WINDOW_SLOT_MAX_NUM)
	{
		sys_err("CHARACTER::GetBuffWear: invalid wear cell %d", bCell);
		return NULL;
	}

	return m_pointsInstant.pBuffEquipmentItem[bCell];
}

bool CHARACTER::IsBuffEquipEmpty()
{
	for (int i = 0; i < 3; ++i) {
		if (m_pointsInstant.pBuffEquipmentItem[i])
			return false;
	}
	return true;
}

bool CHARACTER::CheckBuffEquipmentPositionAvailable(int iWearCell)
{
	if (iWearCell < 0) {
		return false;
	}
	return false;
}

LPITEM CHARACTER::GetBuffEquipmentItem(WORD wCell) const
{
	return GetItem(TItemPos(BUFF_EQUIPMENT, wCell));
}

bool CHARACTER::IsBuffEquipUniqueItem(DWORD dwItemVnum) const
{
	LPITEM item = GetBuffWear(BUFF_WEAR_UNIQUE);

	if (item && item->GetVnum() == dwItemVnum)
		return true;

	return false;
}

bool CHARACTER::EquipBuffItem(BYTE cell, LPITEM item)
{
	if (item->IsExchanging())
		return false;

	int iWearCell = item->FindBuffEquipCell(this);

	if (iWearCell != cell) {
		ChatPacket(CHAT_TYPE_INFO, "[LS;2275]");
		return false;
	}

	if (!GetBuffNPCSystem()->IsActive()) {
		ChatPacket(CHAT_TYPE_INFO, "[LS;2276]");
		return false;
	}

	if (item->GetAntiFlag() & ITEM_ANTIFLAG_SHAMAN) {
		ChatPacket(CHAT_TYPE_INFO, "[LS;2277]");
		return false;
	}

	if (false == FN_check_item_sex_buff(GetBuffNPCSystem()->GetSex(), item)) {
		ChatPacket(CHAT_TYPE_INFO, "[LS;2278]");
		return false;
	}

	if (GetBuffNPCSystem() != NULL) {
		if (GetBuffNPCSystem()->IsSummoned()) {
			item->StartUniqueExpireEvent();
		}
	}

	return true;
}

bool CHARACTER::UnequipBuffItem(BYTE cell, LPITEM item)
{
	if (IS_SET(item->GetFlag(), ITEM_FLAG_IRREMOVABLE)) {
		ChatPacket(CHAT_TYPE_INFO, "[LS;2279]");
		return false;
	}

	item->StopUniqueExpireEvent();

	return true;
}
#endif

#ifdef __INVENTORY_BUFFERING__
void CHARACTER::SendBufferedInventoryPacket()
{
	if (!GetDesc())
	{
		// Well, no desc
		// Just disable it
		us_buffered_items.clear();
		bInvBuff = false;
	}

	// Intializing buffer
	TEMP_BUFFER buf{1024 * 1024, false};
	TPacketGCInventoryHeader packHdr{};
	packHdr.bHeader = HEADER_GC_ITEM_BUFFERED;
	packHdr.wSize = sizeof(packHdr) + sizeof(TPacketGCItemSet)*us_buffered_items.size();

	for (const auto & pItem : us_buffered_items)
	{
		TPacketGCItemSet pack{};
		pack.header = HEADER_GC_ITEM_SET;
		pack.Cell = TItemPos(pItem->GetWindow(), pItem->GetCell());
		pack.count = pItem->GetCount();

		pack.vnum = pItem->GetVnum();
		pack.flags = pItem->GetFlag();
		pack.anti_flags = pItem->GetAntiFlag();
		pack.highlight = (pack.Cell.window_type == DRAGON_SOUL_INVENTORY);

		std::memcpy(pack.alSockets, pItem->GetSockets(), sizeof(pack.alSockets));
		std::memcpy(pack.aAttr, pItem->GetAttributes(), sizeof(pack.aAttr));
			
		buf.write(&pack, sizeof(pack));
	}

	// Sending data
	GetDesc()->BufferedPacket(&packHdr, sizeof(packHdr));
	GetDesc()->LargePacket(buf.read_peek(), buf.size());

	// Data sent, switching off buffer
	us_buffered_items.clear();
	bInvBuff = false;
}

void CHARACTER::QuickOpenStack(LPITEM item)
{
	if (m_bQuickOpenInProgress)
	{
		sys_log(0, "QuickOpenStack: Recursion prevented for player %s", GetName());
		return;
	}

///#ifdef ENABLE_NEWSTUFF
///	if (!PulseManager::Instance().IncreaseClock(GetPlayerID(), ePulse::BoxOpening, std::chrono::milliseconds(3000)))
///	{
///		ChatPacket(CHAT_TYPE_INFO, "[LS;2200;%.2f]", PULSEMANAGER_CLOCK_TO_SEC2(GetPlayerID(), ePulse::BoxOpening));
///		return;
///	}
///#endif

	switch (item->GetType())
	{
		case ITEM_GIFTBOX:
			break;
		case ITEM_USE:
			if (item->GetSubType() != USE_SPECIAL)
				return;
			// Exclude skill books from quick opening - they should be used individually
			if (item->GetVnum() >= 50335 && item->GetVnum() <= 50369)
			{
				ChatPacket(CHAT_TYPE_INFO, "You cant use fast opening for this item.");
				return;
			}
		break;
		default:
			return;
	}

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	// Block quick open if any sub-inventory is completely full.
	// Prevents cherry-picking by keeping specific inventories intentionally full.
	if (GetEmptyInventory(1) == -1
		|| GetEmptySkillBookInventory(1) == -1
		|| GetEmptyUpgradeItemsInventory(1) == -1
		|| GetEmptyStoneInventory(1) == -1
		|| GetEmptyBoxInventory(1) == -1
		|| GetEmptyEfsunInventory(1) == -1
		|| GetEmptyCicekInventory(1) == -1)
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;1229]");
		return;
	}
#endif

	// Block quick open of DS chests if any DS inventory page is full.
	if (item->GetType() == ITEM_GIFTBOX && item->GetVnum() >= 50255 && item->GetVnum() <= 50259
		&& DragonSoul_IsQualified())
	{
		const int iTotalPages = DRAGON_SOUL_INVENTORY_MAX_NUM / DRAGON_SOUL_BOX_SIZE;
		for (int page = 0; page < iTotalPages; ++page)
		{
			WORD wBase = (WORD)(page * DRAGON_SOUL_BOX_SIZE);
			bool bPageFull = true;
			for (int i = 0; i < DRAGON_SOUL_BOX_SIZE && bPageFull; ++i)
			{
				if (IsEmptyItemGrid(TItemPos(DRAGON_SOUL_INVENTORY, wBase + i), 1))
					bPageFull = false;
			}
			if (bPageFull)
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;1229]");
				return;
			}
		}
	}

	// Set recursion guard
    m_bQuickOpenInProgress = true;

	// Setup buffering
	SetInventoryBuffer(true);

	DWORD dwVID = item->GetVID();
	WORD wInitialCount = item->GetCount();
	int iMaxIterations = 500;

	while (item->GetCount() && iMaxIterations > 0)
	{
		if (!UseItemEx(item, NPOS))
			break;

		if (!ITEM_MANAGER::instance().FindByVID(dwVID))
			break;

		if (item->GetCount() == wInitialCount)
			break;

		iMaxIterations--;
	}

	// Release buffer
	SendBufferedInventoryPacket();
	
	// Clear recursion guard
	m_bQuickOpenInProgress = false;

	if (iMaxIterations <= 0)
		sys_log(0, "QuickOpenStack: Hit iteration limit for player %s", GetName());
}
#endif

#ifdef __ENABLE_ITEM_TOGGLE__
item::ItemToggle* CHARACTER::FindToggleItem(bool active,
	uint8_t subType,
	int32_t group,
	CItem* except) {
	for (int32_t i = 0; i < INVENTORY_MAX_NUM; ++i) {
		const auto item = GetInventoryItem(i);
		if (!item || item == except || !item::ItemToggle::Is(*item)) {
			continue;
		}

		const auto toggleItem = static_cast<item::ItemToggle*>(item);

		if (subType != -1 && toggleItem->GetSubType() != subType) {
			continue;
		}

		if (active != toggleItem->IsActive()) {
			continue;
		}

		if (group != -1 && group != toggleItem->GetGroup()) {
			continue;
		}

		return toggleItem;
	}

	return nullptr;
}

void CHARACTER::ProcessAutoRecoveryItem(item::ItemToggle* item) {
	if (IsDead() || IsStun()) {
		return;
	}
	
    if (IsAffectFlag(AFF_POISON)) {
        return;
	}

	static const uint32_t STUN_SKILLS[] = { AFFECT_STUN, AFFECT_POISON, SKILL_TANHWAN,
										   SKILL_GEOMPUNG, SKILL_BYEURAK,
										   SKILL_GIGUNG };
	for (auto skill : STUN_SKILLS) {
		if (FindAffect(skill)) {
			return;
		}
	}

	const int32_t amountUsed = item->GetSocket(ITEM_SOCKET_AUTORECOVERY_USED);
	const int32_t amountFull = item->GetSocket(ITEM_SOCKET_AUTORECOVERY_FULL);
	const int32_t avail = amountFull - amountUsed;
	const bool isUnlimited = item->GetValue(ITEM_VALUE_TOGGLE_AUTORECOVERY_UNLIMITED);

	int32_t amount = 0;

	if (TOGGLE_AUTO_RECOVERY_HP == item->GetSubType()) {
		amount = GetMaxHP() - (GetHP() + GetPoint(POINT_HP_RECOVERY));
	}
	else if (TOGGLE_AUTO_RECOVERY_SP == item->GetSubType()) {
		amount = GetMaxSP() - (GetSP() + GetPoint(POINT_SP_RECOVERY));
	}

	if (amount <= 0)
		return;

	if (!isUnlimited) {
		if (avail > amount) {
			const int pct_of_used = amountUsed * 100 / amountFull;
			const int pct_of_will_used =
				(amountUsed + amount) * 100 / amountFull;

			bool log = false;
			if ((pct_of_will_used / 10) - (pct_of_used / 10) >= 1)
				log = true;

			item->SetSocket(ITEM_SOCKET_AUTORECOVERY_USED,
				amountUsed + amount,
				log);
		}
		else {
			amount = avail;
		}
	}

	if (TOGGLE_AUTO_RECOVERY_HP == item->GetSubType()) {
		PointChange(POINT_HP_RECOVERY, amount);
		EffectPacket(SE_AUTO_HPUP);
	}
	else if (TOGGLE_AUTO_RECOVERY_SP == item->GetSubType()) {
		PointChange(POINT_SP_RECOVERY, amount);
		EffectPacket(SE_AUTO_SPUP);
	}

	if (!isUnlimited && amount == avail) {
		ITEM_MANAGER::instance().RemoveItem(item);
	}
}
#endif
