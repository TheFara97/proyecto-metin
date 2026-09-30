#include "stdafx.h"
#ifdef __ENABLE_ITEM_TOGGLE__
#include "ItemToggle.hpp"

#include "char.h"
#include "item_manager.h"

namespace NSToggle {
#ifdef __ENABLE_AUTO_PICKUP__
	bool PerformPickup(CHARACTER* ch, CItem* item, CHARACTER* enemy) {
		if (!ch) return false;
		if (!item) return false;
		if (!enemy) return false;

		if (!ch->FindToggleItem(true, TOGGLE_PICKUP)) return false;

		if (ch->GetEmptyInventory(item->GetSize()) == -1) return false;

		ch->AutoGiveItem(item->GetVnum(), item->GetCount(), -1, true);

#ifdef __ENABLE_CHAT_LOGS_CONTROLLER__
		ch->SendChatLogsPacket(CHAT_LOGS_TYPE_ITEM, item->GetVnum(), item->GetCount());
#endif

		return true;
	}
#endif
};

namespace item {

	ItemToggle::ItemToggle(uint32_t vnum) : CItem(vnum) {}

	ItemToggle::~ItemToggle() {}

	bool ItemToggle::CanActivate(bool byLoad) const {
		switch (GetSubType()) {
			case TOGGLE_AUTO_RECOVERY_HP:
			case TOGGLE_AUTO_RECOVERY_SP: {
			} break;

			case TOGGLE_AFFECT: {
				if (FindLimit(LIMIT_TIMER_BASED_ON_WEAR)) {
					if (GetSocket(ITEM_SOCKET_REMAIN_SEC) <= 0) return false;
				}

				const auto group = GetGroup();

				if (GetOwner()->FindAffect(AFFECT_TOGGLE_BLOCK, group)) {
					GetOwner()->ChatPacket(CHAT_TYPE_INFO, "This effect is already activated. ");
					return false;
				}
			} break;
		}

		return true;
	}

	bool ItemToggle::Activate(bool byLoad) {
		if (!CanActivate(byLoad)) {
			return false;
		}

		// Unstack if required.
		if (GetCount() > 1) {
			auto pos = GetOwner()->GetEmptyInventory(GetSize());
			if (pos == -1) {
				GetOwner()->ChatPacket(CHAT_TYPE_INFO, "Unstack the item first.");
				return false;
			}

			SetCount(GetCount() - 1);

			auto item = ITEM_MANAGER::Instance().CreateItem(GetVnum());
			item->AddToCharacter(GetOwner(), TItemPos(INVENTORY, pos));

			return static_cast<ItemToggle*>(item)->Activate();
		}

		// Update states.
		SetActive(true);
		Lock(true);

		// Start expire event if necessary.
		StartExpire();

		// Handle different sub types.
		switch (GetSubType()) {
			case TOGGLE_AUTO_RECOVERY_HP:
			case TOGGLE_AUTO_RECOVERY_SP: {
				GetOwner()->StartAffectEvent(); 
			} break;

			case TOGGLE_AFFECT: {
				auto type = GetValue(ITEM_VALUE_TOGGLE_AFFECT_TYPE);
				auto affType = GetValue(ITEM_VALUE_TOGGLE_AFFECT_AFFTYPE);

				if (type)
					GetOwner()->AddAffect(type, APPLY_NONE, 0, affType, INFINITE_AFFECT_DURATION, 0, false);

				ModifyPoints(true); 
			} break;
		}

		OnActivate();

		return true;
	}

	void ItemToggle::Deactivate() {
		// Stop expire event if required.
		StopExpire();

		// Unset states.
		Lock(false);
		SetActive(false);

		// Handle sub types.
		switch (GetSubType()) {
			case TOGGLE_AFFECT: {
				ModifyPoints(false);

				auto type = GetValue(ITEM_VALUE_TOGGLE_AFFECT_TYPE);
				if (type)
					GetOwner()->RemoveAffect(type);
			} break;
		}

		OnDeactivate();
	}

	void ItemToggle::SetActive(bool active) {
		SetSocket(ITEM_SOCKET_TOGGLE_ACTIVE, active ? 1 : 0);
	}

	bool ItemToggle::IsActive() const {
		return GetSocket(ITEM_SOCKET_TOGGLE_ACTIVE) == 1;
	}

	int32_t ItemToggle::GetGroup() const {
		return GetValue(ITEM_VALUE_TOGGLE_GROUP);
	}

	void ItemToggle::StartExpire() {
		auto limit = FindLimit(LIMIT_TIMER_BASED_ON_WEAR);
		if (limit) {
			StartTimerBasedOnWearExpireEvent();
			return;
		}

		limit = FindLimit(LIMIT_REAL_TIME_START_FIRST_USE);
		if (limit) {
			if (GetSocket(1) == 0) {
				auto duration = GetSocket(0);
				if (duration == 0)
					duration = limit->lValue;

				if (duration == 0)
					duration = 60 * 60 * 24 * 7;

				SetSocket(0, time(0) + duration);
				StartRealTimeExpireEvent();

				SetSocket(1, 1);
			}
		}
	}

	void ItemToggle::StopExpire() {
		if (FindLimit(LIMIT_TIMER_BASED_ON_WEAR)) {
			StopTimerBasedOnWearExpireEvent();
		}
	}

	void ItemToggle::ModifyPoints(bool add) {
		switch (GetSubType()) {

		}

		CItem::ModifyPoints(add);
	}

	void ItemToggle::OnComputePoints() {
		if (!IsActive())
			return;

		switch (GetSubType()) {
			case TOGGLE_AFFECT:
				ModifyPoints(true);
				break;
		}
	}

	void ItemToggle::OnCreate() {
		switch (GetSubType()) {
			case TOGGLE_AUTO_RECOVERY_HP:
			case TOGGLE_AUTO_RECOVERY_SP: {
				SetSocket(ITEM_SOCKET_AUTORECOVERY_FULL, GetValue(ITEM_VALUE_TOGGLE_AUTORECOVERY_AMOUNT), false);
			} break;

			case TOGGLE_AFFECT: {

			} break;
		}
	}

	void ItemToggle::OnLoad() {
		if (!GetOwner() || !IsActive())
			return;

		// If the item has a unique toggle group, check if another item of the same
		// group is already activated.
		const auto group = GetGroup();
		if (-1 != group &&
			GetOwner()->FindToggleItem(true, GetSubType(), group, this)) {
			// Found, deactivate this item.
			SetActive(false);
			return;
		}

		Activate(true);
	}

	void ItemToggle::OnRemove() {
		if (!IsActive()) {
			return;
		}

		Deactivate();
	}

	bool ItemToggle::OnUse() {
		if (IsActive()) {
			Deactivate();
			return true;
		}

		// If the item has a unique toggle group, check if another item of the same
		// group is already activated.
		const auto group = GetGroup();
		if (-1 != group &&
			GetOwner()->FindToggleItem(true, GetSubType(), group, this)) {
			// Found, we can't active this item.
			GetOwner()->ChatPacket(
				CHAT_TYPE_INFO, "Another item of the same type is already active.");
			return false;
		}

		return Activate();
	}

	void ItemToggle::OnActivate() {
	}

	void ItemToggle::OnDeactivate() {
	}

	bool ItemToggle::Is(const CItem& item) {
		return item.GetType() == ITEM_TOGGLE;
	}
}
#endif
