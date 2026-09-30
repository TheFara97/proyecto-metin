#include "stdafx.h"
#ifdef __PICKUPER_HELPER__
#include "utils.h"
#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "MonsterPickuper.hpp"

namespace MonsterPickuper
{
	const std::set<DWORD> s_blacklist { 19 };

	// Pickuper
	bool PerformPickup(LPCHARACTER ch, LPITEM item, LPCHARACTER enemy)
	{
		if (!ch)
			return false;

		if (s_blacklist.count(item->GetVnum()))
		{
			return false;
		}

		if (ch->GetPremiumRemainSeconds(PREMIUM_PICKUP) <= 0)
			return false;

		if (ch->GetEmptyInventory(item->GetSize()) == -1)
			return false;

		ch->AutoGiveItem(item->GetVnum(), item->GetCount());
		return true;
	}
}
#endif
