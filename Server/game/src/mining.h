#ifndef __MINING_H
#define __MINING_H

namespace mining
{
	LPEVENT CreateMiningEvent(LPCHARACTER ch, LPCHARACTER load, int seconds);
	DWORD GetRawOreFromLoad(DWORD dwLoadVnum);
	bool OreRefine(LPCHARACTER ch, LPCHARACTER npc, LPITEM item, int cost, int pct, LPITEM metinstone_item);
	int GetMiningBonusValue(LPCHARACTER ch, uint8_t attrType);

	// REFINE_PICK
	int RealRefinePick(LPCHARACTER ch, LPITEM item, bool isRefineWithExtraItem);
	void CHEAT_MAX_PICK(LPCHARACTER ch, LPITEM item);
	// END_OF_REFINE_PICK

	bool IsVeinOfOre (DWORD vnum);
}

#endif
//martysama0134's ac258f174820b7674fa39b8b3a817d16
