#include "stdafx.h"
#include "Inventory.hpp"
#include "char.h"
#include "config.h"
#include "item.h"
#include "item_manager.h"

Inventory::Inventory(CHARACTER& owner) : owner_(owner) {}

Inventory::EStatus Inventory::Add(CItem*& item) {
    if (!item)
		return EStatus::ERROR_UNDEFINED;

    if (Stack(item))
		return EStatus::SUCCESS;

    TItemPos position;
    if (!FindEmptyPosition(*item, position.cell))
			return EStatus::ERROR_NO_SPACE;

    if (item->IsDragonSoul())
        position.window_type = DRAGON_SOUL_INVENTORY;
    else
        position.window_type = INVENTORY;

    item->RemoveFromGround();
    item->AddToCharacter(GetOwnerPtr(), std::move(position));
	return EStatus::SUCCESS;
}

CItem* Inventory::Get(const uint16_t& cell) const {
    if (cell < INVENTORY_MAX_NUM ||
        (cell >= BELT_INVENTORY_SLOT_START && cell < CICEK_INVENTORY_SLOT_END))
        return GetOwner().GetItem(TItemPos(INVENTORY, cell));
    else if (cell < DRAGON_SOUL_INVENTORY_MAX_NUM)
        return GetOwner().GetItem(TItemPos(DRAGON_SOUL_INVENTORY, cell));

    return nullptr;
}

bool Inventory::FindEmptyPosition(CItem& item, uint16_t& cell) const {
    int32_t dummyCell;
    if (item.IsDragonSoul())
        dummyCell = GetOwner().GetEmptyDragonSoulInventory(&item);
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
    else if (item.IsSkillBook())
        dummyCell = GetOwner().GetEmptySkillBookInventory(item.GetSize());
    else if (item.IsUpgradeItem())
        dummyCell = GetOwner().GetEmptyUpgradeItemsInventory(item.GetSize());
    else if (item.IsStone())
        dummyCell = GetOwner().GetEmptyStoneInventory(item.GetSize());
    else if (item.IsBox())
        dummyCell = GetOwner().GetEmptyBoxInventory(item.GetSize());
    else if (item.IsEfsun())
        dummyCell = GetOwner().GetEmptyEfsunInventory(item.GetSize());
    else if (item.IsCicek())
        dummyCell = GetOwner().GetEmptyCicekInventory(item.GetSize());
#endif
    else
        dummyCell = GetOwner().GetEmptyInventory(item.GetSize());

    if (dummyCell < 0)
        return false;

    cell = static_cast<uint16_t>(dummyCell);
    return true;
}

void Inventory::Sort() {
    std::vector<CItem*> items;

    for (uint16_t i = 0; i < INVENTORY_MAX_NUM; ++i) {
        auto item = Get(i);
        if (!item || item->IsLocked())
            continue;

        item->RemoveFromCharacter();
        items.push_back(item);
    }

    std::sort(std::begin(items), std::end(items),
              [](CItem* a, CItem* b) { return a->GetVnum() < b->GetVnum(); });

    TItemPos position;
    for (auto item : items) {
        auto status = Add(OUT item);
        if (status != EStatus::SUCCESS) {
            sys_err(
                "Couldn't find an empty inventory position for item %u, "
                "player %u.",
                item->GetVnum(), GetOwner().GetPlayerID());

            item->AddToGround(GetOwner().GetMapIndex(), GetOwner().GetXYZ());
            item->SetOwnership(&GetOwner(), 300);
            item->StartDestroyEvent();
        }
    }
}
void Inventory::SortStack() {
    if (GetOwner().IsDead()) {
        GetOwner().ChatPacket(CHAT_TYPE_INFO, ("Cant stack if dead."));
        return;
    }
    if (GetOwner().GetExchange() || GetOwner().IsOpenSafebox() ||
        GetOwner().IsCubeOpen()) {
        GetOwner().ChatPacket(CHAT_TYPE_INFO,
                              ("Cant stack with these windows open."));
        return;
    }

    for (uint16_t i = 0; i < INVENTORY_MAX_NUM; ++i) {
        auto item = Get(i);
        if (!item || item->IsLocked())
            continue;

        if (item->GetCount() == ITEM_MAX_COUNT)
            continue;

        if (item->IsStackable() &&
            !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK)) {
            for (int16_t j = INVENTORY_MAX_NUM; j >= 0; --j) {
                auto item2 = Get(j);
                if (!item2 || item2->IsLocked())
                    continue;

                if (item2->GetCount() == ITEM_MAX_COUNT)
                    continue;

                if (item2->GetID() == item->GetID())
                    continue;

                if (item2->GetVnum() == item->GetVnum()) {
                    bool bStopSockets = false;

                    for (int k = 0; k < ITEM_SOCKET_MAX_NUM; ++k) {
                        if (item2->GetSocket(k) != item->GetSocket(k)) {
                            bStopSockets = true;
                            break;
                        }
                    }

                    if (bStopSockets)
                        continue;

                    auto bAddCount = MIN(ITEM_MAX_COUNT - item->GetCount(),
                                         item2->GetCount());

                    item->SetCount(item->GetCount() + bAddCount);
                    item2->SetCount(item2->GetCount() - bAddCount);
                }
            }
        }
    }
}

bool Inventory::Stack(CItem*& item) {
    if (!item)
        return false;

    if (!item->IsStackable())
        return false;

    auto count = static_cast<uint16_t>(item->GetCount());

    CItem* match;
    TryStack(*item, BELT_INVENTORY_SLOT_START, BELT_INVENTORY_SLOT_END, count,
             match);
    TryStack(*item, 0, INVENTORY_MAX_NUM, count, match);
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
    TryStack(*item, SKILL_BOOK_INVENTORY_SLOT_START, SKILL_BOOK_INVENTORY_SLOT_END, count, match);
    TryStack(*item, UPGRADE_ITEMS_INVENTORY_SLOT_START, UPGRADE_ITEMS_INVENTORY_SLOT_END, count, match);
    TryStack(*item, STONE_INVENTORY_SLOT_START, STONE_INVENTORY_SLOT_END, count, match);
    TryStack(*item, BOX_INVENTORY_SLOT_START, BOX_INVENTORY_SLOT_END, count, match);
    TryStack(*item, EFSUN_INVENTORY_SLOT_START, EFSUN_INVENTORY_SLOT_END, count, match);
    TryStack(*item, CICEK_INVENTORY_SLOT_START, CICEK_INVENTORY_SLOT_END, count, match);
#endif
    if (count > 0) {
        item->SetCount(count);
        return false;
    }

    ITEM_MANAGER::Instance().DestroyItem(item);
    item = match;
    return true;
}

void Inventory::TryStack(CItem& item,
                         uint16_t startCell,
                         uint16_t endCell,
                         uint16_t& count,
                         CItem*& match) {
    while (count > 0 && (match = FindStackableItem(item, startCell, endCell))) {
        startCell = match->GetCell() + 1;

#ifdef ENABLE_SWITCHER_CHEST
        if (item.IsChangeAttributeChest()) {
            auto switcherCount =
                item.GetSocket(CHANGE_ATTRIBUTE_CHEST_SOCKET_INDEX);
            switcherCount -=
                owner_.AddAttributeChangerToChest(switcherCount, match);

            if (switcherCount == 0)
                count = 0;

            continue;
        }
#endif

        auto haveCount = static_cast<uint16_t>(match->GetCount());
        auto newCount = static_cast<uint16_t>(MIN(g_bItemCountLimit, haveCount + count));

        if (newCount > haveCount) {
            count -= newCount - haveCount;
            match->SetCount(newCount);
        }
    }
}

CItem* Inventory::FindStackableItem(CItem& originItem,
                                    uint16_t startCell,
                                    uint16_t endCell) const {
	for (auto i = startCell; i < endCell; ++i) {
        auto item = Get(i);
		
        if (!item || !item->IsStackable(&originItem))
            continue;

        return item;
    }

    return nullptr;
}

