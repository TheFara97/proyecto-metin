#ifndef __INC_FISHING_H
#define __INC_FISHING_H

#include "item.h"

namespace fishing
{
	enum
	{
		CAMPFIRE_MOB = 12000,
		FISHER_MOB = 9009,
		FISH_MIND_PILL_VNUM = 27610,

		ONLY_POSSIBLE_MAP_INDEX = 26,
	};

	EVENTINFO(fishing_event_info)
	{
		DWORD		pid;
		int			step;
		DWORD		hang_time;
		uint8_t		need_take_count;
		uint8_t		my_take_count;
		uint32_t	my_response_time;

		fishing_event_info()
		: pid(0)
		, step(0)
		, hang_time(0)
		, need_take_count(0)
		, my_take_count(0)
		, my_response_time(0)
		{
		}
	};

	extern void Initialize();
	extern LPEVENT CreateFishingEvent(LPCHARACTER ch);
	extern bool Take(fishing_event_info* info, LPCHARACTER ch);
	extern void UseFish(LPCHARACTER ch, LPITEM item);
	extern void Grill(LPCHARACTER ch, LPITEM item);
	extern int GetFishLength(uint32_t fishVnum);

	extern bool RefinableRod(LPITEM rod);
	extern int RealRefineRod(LPCHARACTER ch, LPITEM rod, bool isRefineWithExtraItem=false);
	
	extern void BroadcastManwoo(CHARACTER* pChar, bool bStatus, int32_t completeStamp);
	extern void BroadcastManwooLogin(CHARACTER* pChar);

}
#endif
//martysama0134's ac258f174820b7674fa39b8b3a817d16
