#include "stdafx.h"
#include "constants.h"
#include "fishing.h"
#include "locale_service.h"
#include "skill.h"

#include "item_manager.h"
#include "config.h"
#include "packet.h"
#include "sectree_manager.h"
#include "char.h"
#include "char_manager.h"
#include "log.h"
#include "questmanager.h"
#include "buffer_manager.h"
#include "desc_client.h"
#include "locale_service.h"

#include "affect.h"
#include "fish_wiki.h"
#include "unique_item.h"

#ifdef RANKING_SYSTEM
#include "server_ranking_manager.h"
#include "ranking_manager.h"
#endif
#ifdef BATTLE_PASS
#include "CBattlePass.h"
#endif
#include "utils.h"
#define FISH_LENGTH_MULTIPLIER_FROM_BONUS 0.5f // jak wejdzie bonus szansy na wieksza rybke to o taki mnoznik zostanie zwiekszona jej dlugosc

namespace fishing
{
	enum
	{
		ROD_MAX_LEVEL = 21,	// ilosc wedek, od +0 do +15 jest ich 16
		FISHING_EXTRA_ITEMS_COUNT = 3,
		MANWOO_EVENT_ITEMS_COUNT = 1,
		MAX_FISH_NUM = 31,
	};

	int fishing_passive_skill_bonus_values[SKILL_MAX_LEVEL + 1][5] = {
		// natychmiastowy_polow, wieksza_ryba, rzadsza_ryba, wiecej_yang, wiecej_punktow
		{ 0, 0, 0, 0, 0 },		// 0
		{ 0, 1, 1, 0, 0 },		// 1
		{ 0, 1, 1, 0, 0 },		// 2
		{ 0, 2, 2, 0, 0 },		// 3
		{ 0, 2, 2, 0, 0 },		// 4
		{ 0, 3, 3, 0, 0 },		// 5
		{ 0, 3, 3, 0, 0 },		// 6
		{ 0, 4, 4, 0, 0 },		// 7
		{ 0, 4, 4, 0, 0 },		// 8
		{ 0, 5, 5, 0, 0 },		// 9
		{ 0, 5, 5, 0, 0 },		// 10
		{ 0, 6, 6, 0, 0 },		// 11
		{ 0, 6, 6, 0, 0 },		// 12
		{ 0, 7, 7, 0, 0 },		// 13
		{ 0, 7, 7, 0, 0 },		// 14
		{ 0, 8, 8, 0, 0 },		// 15
		{ 0, 8, 8, 0, 0 },		// 16
		{ 0, 9, 9, 0, 0 },		// 17
		{ 0, 9, 9, 0, 0 },		// 18
		{ 0, 10, 10, 0, 0 },		// 19
		{ 1, 10, 10, 10, 0 },		// M1
		{ 1, 11, 11, 11, 0 },		// M2
		{ 2, 11, 11, 12, 0 },		// M3
		{ 2, 12, 12, 13, 0 },		// M4
		{ 3, 12, 12, 14, 0 },		// M5
		{ 3, 13, 13, 15, 0 },		// M6
		{ 4, 13, 13, 16, 0 },		// M7
		{ 4, 14, 14, 17, 0 },		// M8
		{ 5, 14, 14, 18, 0 },		// M9
		{ 5, 15, 15, 19, 0 },		// M10
		{ 6, 15, 15, 20, 10 },		// G1
		{ 6, 16, 16, 21, 11 },		// G2
		{ 7, 16, 16, 22, 12 },		// G3
		{ 7, 17, 17, 23, 13 },		// G4
		{ 8, 17, 17, 24, 14 },		// G5
		{ 8, 18, 18, 25, 15 },		// G6
		{ 9, 18, 18, 26, 16 },		// G7
		{ 10, 19, 19, 27, 17 },		// G8
		{ 11, 19, 19, 28, 18 },		// G9
		{ 13, 20, 20, 29, 19 },		// G10
		{ 15, 20, 20, 30, 20 },		// P
	};

	struct SFishData
	{
		int vnum;
		int dead_vnum;
		int grilled_vnum;
		int	length;
		int price;
	}
	FISH_DATA[MAX_FISH_NUM] = {
		// vnum, martwy_vnum, ugrilowany_vnum, dlugosc_w_cm, cena_bazowa
		{27802,	27893,	27799,	20,	4000000	},
		{27803,	27833,	27799,	40,	4000000	},
		{27804,	27834,	27799,	10,	4000000	},
		{27805,	27835,	27865,	50,	4000000	},
		{27806,	27836,	27799,	60,	4000000	},
		{27807,	27837,	27799,	150,	4000000	},
		{27808,	27838,	27868,	150,	4000000	},
		{27809,	27839,	27799,	80,	4000000	},
		{27810,	27840,	27870,	70,	4000000	},
		{27811,	27841,	27799,	50,	4000000	},
		{27812,	27842,	27799,	60,	4000000	},
		{27813,	27843,	27873,	30,	4000000	},
		{27814,	27844,	27874,	60,	4000000	},
		{27815,	27845,	27799,	40,	4000000	},
		{27816,	27846,	27876,	180,	4000000	},
		{27817,	27847,	27877,	30,	4000000	},
		{27818,	27848,	27799,	50,	4000000	},
		{27819,	27849,	27799,	40,	4000000	},
		{27820,	27850,	27799,	20,	4000000	},
		{27821,	27851,	27881,	100,	4000000	},
		{27822,	27852,	27799,	60,	4000000	},
		{27823,	27853,	27883,	60,	4000000	},
		{27824,	27854,	27799,	10,	4000000	},
		{27825,	27855,	27799,	20,	4000000	},
		{27826,	27856,	27886,	30,	4000000	},
		{27827,	27857,	27887,	10,	4000000	},
		{27828,	27858,	27888,	70,	4000000	},
		{27829,	27859,	27799,	20,	4000000	},
		{27830,	27860,	27890,	120,	4000000	},
		{27831,	27861,	27891,	10,	4000000	},
		{27832,	27862,	27892,	10,	4000000	},
	};

	int fishing_extra_drop_items[FISHING_EXTRA_ITEMS_COUNT][4] =
	{
		// poziom pasywki, przedmiot, ilosc, szansa
		{ 0, 203009, 1, 500 },
		{ 30, 203009, 1, 600 },
		{ 40, 203009, 1, 700 },
		//{ 0, 29050, 1, 500 },
		//{ 0, 29051, 1, 500 },
		//{ 40, 20000, 1, 70 },
	};

	int fishing_extra_drop_items_summer[FISHING_EXTRA_ITEMS_COUNT][4] =
	{
		// poziom pasywki, przedmiot, ilosc, szansa
		{ 0, 201218, 1, 1200 },   // Summer fishing item 1 - 4% chance, no skill requirement
		{ 30, 201218, 1, 1200 },  // Summer fishing item 2 - 5% chance, requires level 30 fishing skill
		{ 40, 201218, 1, 1200 },  // Summer fishing item 3 - 6% chance, requires level 40 fishing skill
	};

	int manwoo_event_extra_drop_items[MANWOO_EVENT_ITEMS_COUNT][3] =
	{
		// przedmiot, ilosc, szansa
		{ 7910, 1, 1 },
	};

	struct SRodData
	{
		int extra_length_pct;
		std::vector<uint32_t> fishes[3];
	} ROD_DATA[ROD_MAX_LEVEL];
	
	void Initialize()
	{
		for (int i = 0; i < ROD_MAX_LEVEL; i++)
		{
			ROD_DATA[i].extra_length_pct = 2 * i;	// kazda wedka daje 2% dluzsza rybe
			for (int j = 0; j < 3; j++)
				ROD_DATA[i].fishes[j].clear();
		}
		// Rybki na Wedce +0, 3 przedzialy
		ROD_DATA[0].fishes[0] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[0].fishes[1] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[0].fishes[2] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };

		// Rybki na Wedce +1, 3 przedzialy
		ROD_DATA[1].fishes[0] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[1].fishes[1] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[1].fishes[2] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };

		// Rybki na Wedce +2, 3 przedzialy
		ROD_DATA[2].fishes[0] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[2].fishes[1] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[2].fishes[2] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
 
		// Rybki na Wedce +3, 3 przedzialy
		ROD_DATA[3].fishes[0] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[3].fishes[1] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[3].fishes[2] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
 
		// Rybki na Wedce +4, 3 przedzialy
		ROD_DATA[4].fishes[0] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[4].fishes[1] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[4].fishes[2] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
 
		// Rybki na Wedce +5, 3 przedzialy
		ROD_DATA[5].fishes[0] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[5].fishes[1] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[5].fishes[2] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };

		// Rybki na Wedce +6, 3 przedzialy
		ROD_DATA[6].fishes[0] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[6].fishes[1] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[6].fishes[2] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };

		// Rybki na Wedce +7, 3 przedzialy
		ROD_DATA[7].fishes[0] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[7].fishes[1] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[7].fishes[2] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };

		// Rybki na Wedce +8, 3 przedzialy
		ROD_DATA[8].fishes[0] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[8].fishes[1] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[8].fishes[2] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };

		// Rybki na Wedce +9, 3 przedzialy
		ROD_DATA[9].fishes[0] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[9].fishes[1] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[9].fishes[2] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };

		// Rybki na Wedce +10, 3 przedzialy
		ROD_DATA[10].fishes[0] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[10].fishes[1] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[10].fishes[2] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };

		// Rybki na Wedce +11, 3 przedzialy
		ROD_DATA[11].fishes[0] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[11].fishes[1] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[11].fishes[2] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };

		// Rybki na Wedce +12, 3 przedzialy
		ROD_DATA[12].fishes[0] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[12].fishes[1] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[12].fishes[2] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };

		// Rybki na Wedce +13, 3 przedzialy
		ROD_DATA[13].fishes[0] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[13].fishes[1] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[13].fishes[2] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };

		// Rybki na Wedce +14, 3 przedzialy
		ROD_DATA[14].fishes[0] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[14].fishes[1] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[14].fishes[2] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };

		// Rybki na Wedce +15, 3 przedzialy
		ROD_DATA[15].fishes[0] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[15].fishes[1] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[15].fishes[2] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };

		// Rybki na Wedce +15, 3 przedzialy
		ROD_DATA[16].fishes[0] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[16].fishes[1] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[16].fishes[2] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };

		// Rybki na Wedce +15, 3 przedzialy
		ROD_DATA[17].fishes[0] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[17].fishes[1] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[17].fishes[2] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };

		// Rybki na Wedce +15, 3 przedzialy
		ROD_DATA[18].fishes[0] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[18].fishes[1] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[18].fishes[2] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };

		// Rybki na Wedce +15, 3 przedzialy
		ROD_DATA[19].fishes[0] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[19].fishes[1] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[19].fishes[2] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };

		// Rybki na Wedce +15, 3 przedzialy
		ROD_DATA[20].fishes[0] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[20].fishes[1] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		ROD_DATA[20].fishes[2] = { 27802, 27803, 27804, 27805, 27806, 27807, 27808, 27809, 27810, 27811, 27812, 27813, 27814, 27815, 27816, 27817, 27818, 27819, 27820, 27821, 27822, 27823 };
		// Rybki na Wedce +15, 3 przedzialy
		// ROD_DATA[15].fishes[0] = { 27832 };
		// ROD_DATA[15].fishes[1] = { 27832 };
		// ROD_DATA[15].fishes[2] = { 27832 };	
	}

	// built in + from bonuses
	int GetFishingRodBonusValue(LPITEM rod, uint8_t attrType)
	{
		int value = 0;
		if (rod)
		{
			value = rod->FindApplyValue(attrType);
			for (int i = 0; i < 3; i++)
			{
				const TPlayerItemAttribute& attr = rod->GetAttribute(i);
				if (attr.bType == attrType)
				{
					value += attr.sValue;
					break;
				}
			}
		}

		return value;
	}

	int GetFishingBonusValue(LPCHARACTER ch, uint8_t attrType)
	{
		// 1 -> Built in bonuses in rod
		// 2 -> Added bonuses in rod
		// 3 -> Bonuses from points (for ex. builtin in artifact)
		// 4 -> Bonuses from passive skill
		// 5 -> Bonuses from premium packet

		/*
		{ POINT_FISHING_INSTANT_CATCH_CHANCE, },	// APPLY_FISHING_INSTANT_CATCH_CHANCE,	114
		{ POINT_FISHING_BIGGER_FISH_CHANCE, },		// APPLY_FISHING_BIGGER_FISH_CHANCE,115
		{ POINT_FISHING_NEXT_SECTION_CHANCE, },		// APPLY_FISHING_NEXT_SECTION_CHANCE,	116
		{ POINT_FISHING_MORE_YANG_MULTIPLIER, },	// APPLY_FISHING_MORE_YANG_MULTIPLIER,	117
		{ POINT_FISHING_MORE_POINTS_CHANCE, },		// APPLY_FISHING_MORE_POINTS_CHANCE,	118
		{ POINT_FISHING_SUCCESS_CHANCE, },			// APPLY_FISHING_SUCCESS_CHANCE,	119
		*/

		int value = GetFishingRodBonusValue(ch->GetWear(WEAR_WEAPON), attrType);
		value += static_cast<int>(ch->GetPoint(aApplyInfo[attrType].bPointType));
		if (attrType >= APPLY_FISHING_INSTANT_CATCH_CHANCE && attrType <= APPLY_FISHING_MORE_POINTS_CHANCE)
			value += fishing_passive_skill_bonus_values[ch->GetSkillLevel(SKILL_FISHING)][attrType - APPLY_FISHING_INSTANT_CATCH_CHANCE];

		if (attrType == APPLY_FISHING_INSTANT_CATCH_CHANCE && ch->IsVIP())
			value += 10;
		else if (attrType == APPLY_FISHING_BIGGER_FISH_CHANCE && ch->IsVIP())
			value += 10;
		else if (attrType == APPLY_FISHING_MORE_YANG_MULTIPLIER && ch->IsVIP())
			value += 25;
		else if (attrType == APPLY_FISHING_MORE_POINTS_CHANCE && ch->IsVIP())
			value += 10;
		else if (attrType == APPLY_FISHING_SUCCESS_CHANCE && ch->IsVIP())
			value += 10;

		return value;
	}

	void ExtraFishingEventItemDrop(LPCHARACTER ch)
	{
		if (CHARACTER_MANAGER::Instance().CheckEventIsActive(BIGGER_STAR_CHANCE, 0, ch))
		{
			int chance = 20;
	
			//if (test_server)
			//{
			//	chance = 100;
			//}

			if (number(1, 100) <= chance)
			{
				ch->AutoGiveItem(203009, 1);
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10059]");
			}
		}
	}

	void BroadcastManwoo(CHARACTER* pChar, bool bStatus, int32_t completeStamp)
	{
		if (!pChar || !pChar->IsPC())
			return;
		
		pChar->ChatPacket(CHAT_TYPE_COMMAND, "ManwooEvent %d %d", static_cast<int32_t>(bStatus), completeStamp);
	}
	
	void BroadcastManwooLogin(CHARACTER* pChar)
	{
		if (!pChar || !pChar->IsPC())
			return;

		bool isActive = quest::CQuestManager::instance().GetEventFlag("manwoo_item_chance") > 0;
		int32_t completeTime = isActive ? get_global_time() + (60*60*2) : 0; // CompleteTime = CEventManager::GetEndTime() - instead of a static time.
		pChar->ChatPacket(CHAT_TYPE_COMMAND, "ManwooEvent %d %d", static_cast<int32_t>(isActive), completeTime);
	}
	
	void ExtraFishingItemDrop(LPCHARACTER ch)
	{
		// losuje 7 razy tylko jezeli wymagany poziom pasywki jest wiekszy, jezeli poziom pasywki bedzie pasowal to sprawdzam tylko szanse
		const int passiveLevel = ch->GetSkillLevel(SKILL_FISHING);

		for (int i = 0; i < 6; i++)
		{
			const int randomIdx = number(0, FISHING_EXTRA_ITEMS_COUNT - 1);
			if (passiveLevel < fishing_extra_drop_items[randomIdx][0])
				continue;

			if (number(1, 10000) <= fishing_extra_drop_items[randomIdx][3])
				ch->AutoGiveItem(fishing_extra_drop_items[randomIdx][1], fishing_extra_drop_items[randomIdx][2]);
			
			break;
		}
	}
	
	void ExtraSummerFishingItemDrop(LPCHARACTER ch)
	{
	
		const int passiveLevel = ch->GetSkillLevel(SKILL_FISHING);
	
		for (int i = 0; i < 6; i++)  // Same logic as original - try 6 times
		{
			const int randomIdx = number(0, FISHING_EXTRA_ITEMS_COUNT - 1);
			if (passiveLevel < fishing_extra_drop_items_summer[randomIdx][0])
				continue;
	
			if (test_server)
				ch->ChatPacket(CHAT_TYPE_INFO, "Summer Fishing Drop %d: chance=%d", 
					randomIdx, fishing_extra_drop_items_summer[randomIdx][3]);
	
			if (number(1, 10000) <= fishing_extra_drop_items_summer[randomIdx][3])
			{
				ch->AutoGiveItem(fishing_extra_drop_items_summer[randomIdx][1], fishing_extra_drop_items_summer[randomIdx][2]);
				
				//ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10094]");
			}
			
			break;
		}
	}

	uint8_t GetFishIdx(uint32_t fishVnum)
	{
		for (int i = 0; i < MAX_FISH_NUM; i++)
		{
			if (FISH_DATA[i].vnum == fishVnum)
				return i;
		}

		return UINT8_MAX;
	}

	int GetFishLength(uint32_t fishVnum)
	{
		const uint8_t fishIdx = GetFishIdx(fishVnum);
		if (fishIdx == UINT8_MAX)
			return 0;

		return FISH_DATA[fishIdx].length;
	}

	std::pair<uint8_t, uint8_t> DetermineFish(LPCHARACTER ch, int rodLevel)
	{
		const int manwooFishChance = quest::CQuestManager::instance().GetEventFlag("manwoo_fish_chance");
		if (manwooFishChance > 0 && number(1, 100) <= manwooFishChance)
			rodLevel = ROD_MAX_LEVEL-1;

		int sectionChances[3] = { 85, 10, 5 };
		int extraSectionChance = MINMAX(0, GetFishingBonusValue(ch, APPLY_FISHING_NEXT_SECTION_CHANCE), 85);
		if (extraSectionChance > 0)
		{
			sectionChances[0] -= extraSectionChance;

			extraSectionChance /= 2;
			sectionChances[1] += extraSectionChance;
			sectionChances[2] += extraSectionChance;
		}

		int section = 0;
		const int chance = number(1, 100);
		if (chance > sectionChances[0] + sectionChances[1])
			section = 2;
		else if (chance > sectionChances[0])
			section = 1;

		return std::make_pair(section, GetFishIdx(ROD_DATA[rodLevel].fishes[section][number(0, ROD_DATA[rodLevel].fishes[section].size() - 1)]));
	}

	void FishingPractice(LPCHARACTER ch)
	{
		LPITEM rod = ch->GetWear(WEAR_WEAPON);
		if (rod && rod->GetType() == ITEM_ROD)
		{
			if (rod->GetSocket(0) < rod->GetValue(2) && number(1, 10) <= 10)
			{
				int addCount = 2;
				if (number(1, 100) <= GetFishingBonusValue(ch, APPLY_FISHING_MORE_POINTS_CHANCE))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2432]");
					rod->SetSocket(0, MINMAX(0, rod->GetSocket(0) + 1 + addCount, rod->GetValue(2)));
					// addCount++;
				}
				else
				{
					rod->SetSocket(0, MINMAX(0, rod->GetSocket(0) + 1, rod->GetValue(2)));
				}
				
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2433;%d;%d]", rod->GetSocket(0), rod->GetValue(2));
				if (rod->GetSocket(0) == rod->GetValue(2))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2434]");
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2435]");
				}
			}

			rod->SetSocket(2, 0);
		}
	}

	void FishingReact(LPCHARACTER ch)
	{
		TPacketGCFishing p;
		p.header = HEADER_GC_FISHING;
		p.subheader = FISHING_SUBHEADER_GC_REACT;
		p.info = ch->GetVID();
		ch->PacketAround(&p, sizeof(p));
	}

	void FishingSuccess(LPCHARACTER ch)
	{
		FishingPractice(ch);
		ExtraFishingItemDrop(ch);
		ExtraSummerFishingItemDrop(ch);
		ExtraFishingEventItemDrop(ch);

		TPacketGCFishing p;
		p.header = HEADER_GC_FISHING;
		p.subheader = FISHING_SUBHEADER_GC_SUCCESS;
		p.info = ch->GetVID();
		ch->PacketAround(&p, sizeof(p));
		
		const int po = number(50, 70);
		if (ch->IsVIP() && number(1, 100) <= 50)
		{
			ch->PointChange(POINT_PKT_OSIAG, po*2);
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2436;%d]", po);
		}
		else
		{
			ch->PointChange(POINT_PKT_OSIAG, po);
		}

#ifdef RANKING_SYSTEM
		CServerRankingManager::instance().IncServerRankValue(ch, SERVER_RANK_TYPE_FISHING);
		CRankingManager::instance().IncRankValue(ch, RANK_TYPE_FISHING);
#endif
#ifdef BATTLE_PASS
		ch->GetBattlePass()->IncrementDailyMission(CBattlePass::DAILY_MISSION_FISHING);
#endif
	}

	void FishingFail(LPCHARACTER ch)
	{
		TPacketGCFishing p;
		p.header = HEADER_GC_FISHING;
		p.subheader = FISHING_SUBHEADER_GC_FAIL;
		p.info = ch->GetVID();
		ch->PacketAround(&p, sizeof(p));
	}

	int CalculateFishLength(int baseLength, uint8_t sectionIdx, int extraLengthBonusChance, int rodExtraLength)
	{
		// od 80 do 120 procent bazowej dlugosci
		// 15 procent dluzsze dla drugiego przedzialu i 30 procent na trzeciego przedzialu
		// dodatkowo dluzsze o LENGTH_MULTIPLIER_FROM_BONUS procent jezeli wejdzie bonus
		// dodatkowo dluzsze w zaleznosci od poziomu wedki
		float multiplier = number(80, 120) / 100.0f;

		if (sectionIdx == 1)
			multiplier += 0.15f;
		else if (sectionIdx == 2)
			multiplier += 0.3f;

		if (number(1, 100) <= extraLengthBonusChance)
			multiplier += FISH_LENGTH_MULTIPLIER_FROM_BONUS;

		if (rodExtraLength > 0)
			multiplier += static_cast<float>(rodExtraLength) / 100.0f;

		if (test_server)
			sys_log(0, "[TEST_SERVER] length multiplier %f", multiplier);

		return static_cast<int>(static_cast<float>(baseLength) * multiplier) * 100 + number(1, 99);
	}

	int GetFishPrice(int fishBasePrice, int fishLength, int rodLevel, uint8_t fishSection, int extraPriceBonusMultiplier)
	{
		// podstawowy mnoznik to dlugosc ryby / 100
		// kazdy poziom wedki daje +0.1 mnoznika
		// 0.5 mnozika dla rybki z przedzialu 2 i 1.0 mnoznika dla rybki z przedzialu 3
		// dodatkowo mnoznik yang z bonusu zwiekszenia yang z rybek
		float multiplier = 1.0f;
		multiplier += static_cast<float>(fishLength) / 10000.0f;	// /100 dla uzyskania cm i /100 dla uzyskania mnoznika
		multiplier += (rodLevel + 1) * 0.1f;

		if (fishSection == 1)
			multiplier += 0.5f;
		else if (fishSection == 2)
			multiplier += 1.0f;

		multiplier += static_cast<float>(extraPriceBonusMultiplier) / 100.0f;

		if (test_server)
			sys_log(0, "[TEST_SERVER] price multiplier %f", multiplier);

		return static_cast<int>(static_cast<float>(fishBasePrice) * multiplier);
	}

	int GetSuccessFishCatchChance(LPCHARACTER ch, int extraChanceFromBait)
	{
		if (test_server)
			ch->ChatPacket(CHAT_TYPE_INFO, "[TEST_SERVER] extraChanceFromBait %d bonus success chance %d", extraChanceFromBait, GetFishingBonusValue(ch, APPLY_FISHING_SUCCESS_CHANCE));
		return 0 + extraChanceFromBait + GetFishingBonusValue(ch, APPLY_FISHING_SUCCESS_CHANCE);
	}

	void DecreaseRodBonuses(LPITEM rod)
	{
		int toSetIndex = 0;
		for (int i = 0; i < 3; i++)
		{
			const TPlayerItemAttribute& attr = rod->GetAttribute(i);
			if (attr.bType == 0)
				break;

			const int restCount = rod->GetAttribute(3 + i).sValue - 1;
			if (restCount > 0)
			{
				rod->SetForceAttribute(toSetIndex, attr.bType, attr.sValue, false);
				rod->SetForceAttribute(3 + toSetIndex, 1, restCount, false);
				if (i != toSetIndex)
				{
					rod->SetForceAttribute(i, 0, 0, false);
					rod->SetForceAttribute(3 + i, 0, 0, false);
				}
				toSetIndex++;
			}
			else
			{
				rod->SetForceAttribute(i, 0, 0, false);
				rod->SetForceAttribute(3 + i, 0, 0, false);
			}
		}

		rod->UpdatePacket();
		rod->Save();
	}

	EVENTFUNC(fishing_event)
	{
		fishing_event_info* info = dynamic_cast<fishing_event_info*>(event->info);
		if (info == NULL)
		{
			sys_err("fishing_event> <Factor> Null pointer");
			return 0;
		}

		LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(info->pid);
		if (!ch)
			return 0;

		LPITEM rod = ch->GetWear(WEAR_WEAPON);
		if (!(rod && rod->GetType() == ITEM_ROD))
		{
			ch->m_pkFishingEvent = NULL;
			return 0;
		}

		if (info->step == 0)
		{
			++info->step;

			//info->ch->Motion(MOTION_FISHING_SIGN);
			info->hang_time = get_dword_time();
			info->need_take_count = number(2, 3);
			info->my_take_count = 0;
			info->my_response_time = 0;

			// ch->ChatPacket(CHAT_TYPE_INFO, "Aby wylowic rybe kliknij spacje %u razy!", info->need_take_count);
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2437;%d]", info->need_take_count);

			FishingReact(ch);

#ifdef SYSTEM_EFFECT_FISH
			// Efekt nad hlavou podle potřeby zmáčknutí mezerníku
			switch (info->need_take_count)
			{
				case 1:
					ch->EffectPacket(SE_EFFECT_FISH1);
					break;
				case 2:
					ch->EffectPacket(SE_EFFECT_FISH2);
					break;
				case 3:
					ch->EffectPacket(SE_EFFECT_FISH3);
					break;
				case 4:
					ch->EffectPacket(SE_EFFECT_FISH4);
					break;
			}
#endif

			return (PASSES_PER_SEC(3));

		}
		else if (info->step == 1)
		{
			++info->step;

			if (info->step > 5)
				info->step = 5;
			
			if (test_server)
				ch->ChatPacket(CHAT_TYPE_INFO, "[TEST_SERVER] %u ms", info->my_response_time);

			LPITEM rod = ch->GetWear(WEAR_WEAPON);
			if (!(rod && rod->GetType() == ITEM_ROD) || info->my_response_time > 2999)
			{
				LogManager::instance().FishLog(ch->GetPlayerID(), 0, 0, get_dword_time() - info->hang_time, 251);
				FishingFail(ch);
			}
			else
			{
				const int baitExtraChance = rod->GetSocket(2);
				const int rodLevel = rod->GetRefineLevel();

				if (test_server && info->my_take_count != info->need_take_count)
					ch->ChatPacket(CHAT_TYPE_INFO, "[TEST_SERVER] Bledna ilosc spacji mordko!");

				if (info->my_take_count < info->need_take_count)
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2438]");
				else if (info->my_take_count > info->need_take_count)
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2439]");

				const int los = number(1, 100);
				const int chance = GetSuccessFishCatchChance(ch, baitExtraChance);
				if (test_server)
					ch->ChatPacket(CHAT_TYPE_INFO, "Random: %d Chance: %d Chance from bait: %d Success: %d Need click %d Clicked %d", los, chance, baitExtraChance, los <= chance, info->need_take_count, info->my_take_count);
				if (info->my_take_count == info->need_take_count && los <= chance)
				//if (info->my_take_count == info->need_take_count && number(1, 100) <= GetSuccessFishCatchChance(ch, baitExtraChance))
				{
					const std::pair<uint8_t, uint8_t> rewardFish = DetermineFish(ch, rodLevel);
					SFishData* rewardFishData = &FISH_DATA[rewardFish.second];

					FishingSuccess(ch);

					TPacketGCFishing p;
					p.header = HEADER_GC_FISHING;
					p.subheader = FISHING_SUBHEADER_GC_FISH;
					p.info = rewardFishData->vnum;
					ch->GetDesc()->Packet(&p, sizeof(TPacketGCFishing));

					const int newFishCount = ch->GetQuestFlag("fishing.my_fish_count") + 1;
					ch->SetQuestFlag("fishing.my_fish_count", newFishCount);
					ch->ChatPacket(CHAT_TYPE_COMMAND, "my_fish %d", newFishCount);

					LPITEM item = ch->AutoGiveItem(rewardFishData->vnum, 1, false);
					if (item)
					{
						item->SetSocket(0, CalculateFishLength(rewardFishData->length, rewardFish.first, GetFishingBonusValue(ch, APPLY_FISHING_BIGGER_FISH_CHANCE), ROD_DATA[rodLevel].extra_length_pct));
						item->SetSocket(1, GetFishPrice(rewardFishData->price, item->GetSocket(0), rodLevel, rewardFish.first, GetFishingBonusValue(ch, APPLY_FISHING_MORE_YANG_MULTIPLIER)));

						LogManager::instance().FishLog(ch->GetPlayerID(), baitExtraChance, rodLevel, info->my_response_time, rewardFishData->vnum, item->GetSocket(0), item->GetSocket(1));

						ch->GetFishWiki()->CatchFish(rewardFishData->vnum, item->GetSocket(0), item->GetSocket(1), rodLevel);
					}
					else
						LogManager::instance().FishLog(ch->GetPlayerID(), baitExtraChance, rodLevel, info->my_response_time, 252);
				}
				else
				{
					LogManager::instance().FishLog(ch->GetPlayerID(), baitExtraChance, rodLevel, info->my_response_time, 253);
					FishingFail(ch);
				}

				DecreaseRodBonuses(rod);
				rod->SetSocket(2, 0);
			}
		}

		ch->m_pkFishingEvent = NULL;
		return 0;
	}

	LPEVENT CreateFishingEvent(LPCHARACTER ch)
	{
		fishing_event_info* info = AllocEventInfo<fishing_event_info>();
		info->pid = ch->GetPlayerID();
		info->step = 0;
		info->hang_time = 0;
		
		if (test_server)
			ch->ChatPacket(CHAT_TYPE_INFO, "[TEST_SERVER] instant catch chance %d", GetFishingBonusValue(ch, APPLY_FISHING_INSTANT_CATCH_CHANCE));

		int time = 2 + 7;	// daje 2 sekundy startowego czasu na pelne zarzucenie wedki, nie chcesz to wyjeb i zostaw samo 5
		if (number(1, 100) <= GetFishingBonusValue(ch, APPLY_FISHING_INSTANT_CATCH_CHANCE))
			time -= 7;

		TPacketGCFishing p;
		p.header = HEADER_GC_FISHING;
		p.subheader = FISHING_SUBHEADER_GC_START;
		p.info = ch->GetVID();
		p.dir = static_cast<BYTE>(ch->GetRotation() / 5);
		ch->PacketAround(&p, sizeof(TPacketGCFishing));

		return event_create(fishing_event, info, PASSES_PER_SEC(time));
	}

	bool Take(fishing_event_info* info, LPCHARACTER ch)
	{
		if (info->step == 1)
		{
			LPITEM rod = ch->GetWear(WEAR_WEAPON);
			if (!(rod && rod->GetType() == ITEM_ROD))
			{
				LogManager::instance().FishLog(ch->GetPlayerID(), 0, 0, get_dword_time() - info->hang_time, 251);
				FishingFail(ch);

				return true;
			}

			info->my_take_count++;

			if (info->need_take_count == info->my_take_count)
				info->my_response_time = get_dword_time() - info->hang_time;

			if (test_server)
			{
				if (info->my_take_count < info->need_take_count)
					ch->ChatPacket(CHAT_TYPE_INFO, "[TEST_SERVER] Click %u", info->need_take_count - info->my_take_count);
				else if (info->my_take_count > info->need_take_count)
					ch->ChatPacket(CHAT_TYPE_INFO, "[TEST_SERVER] Too many clicks boy!");
			}

			return false;
		}
		else if (info->step > 1)
		{
			LPITEM rod = ch->GetWear(WEAR_WEAPON);
			if (rod && rod->GetType() == ITEM_ROD)
			{
				LogManager::instance().FishLog(ch->GetPlayerID(), rod->GetSocket(0), rod->GetRefineLevel(), get_dword_time() - info->hang_time, 254);
				rod->SetSocket(2, 0);
			}
			else
				LogManager::instance().FishLog(ch->GetPlayerID(), 0, 0, get_dword_time() - info->hang_time, 255);
			FishingFail(ch);
		}
		else
		{
			TPacketGCFishing p;
			p.header = HEADER_GC_FISHING;
			p.subheader = FISHING_SUBHEADER_GC_STOP;
			p.info = ch->GetVID();
			ch->PacketAround(&p, sizeof(p));
		}

		return true;
	}

	void UseFish(LPCHARACTER ch, LPITEM item)
	{
		const int idx = GetFishIdx(item->GetVnum());
		if (idx == UINT8_MAX)
			return;

		const int32_t fishLength = item->GetSocket(0);
		const int64_t fishGold = item->GetSocket(1);
		item->SetCount(item->GetCount() - 1);
		// LPITEM newItem = ch->AutoGiveItem(FISH_DATA[idx].dead_vnum);
		// if (newItem)
			// newItem->SetSocket(0, fishLength);

		if (fishGold <= 0)
			return;
		
		if (fishGold + ch->GetGold() < 0)
			sys_err("FISH USE: too many gold on character(%u, %s) try add %lld (now %lld)", ch->GetPlayerID(), ch->GetName(), fishGold, ch->GetGold());
		else
		{
			ch->PointChange(POINT_GOLD, fishGold, true);
		}
	}

	void Grill(LPCHARACTER ch, LPITEM item)
	{
		const int idx = GetFishIdx(item->GetVnum());
		if (idx == UINT8_MAX)
			return;

		const int count = item->GetCount();

		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2440;%s]", item->GetName());
		item->SetCount(0);
		ch->AutoGiveItem(FISH_DATA[idx].grilled_vnum, count);
	}

	bool RefinableRod(LPITEM rod)
	{
		if (rod->GetType() != ITEM_ROD)
			return false;

		if (rod->IsEquipped())
			return false;

		return (rod->GetSocket(0) == rod->GetValue(2));
	}

	int RealRefineRod(LPCHARACTER ch, LPITEM item, bool isRefineWithExtraItem)
	{
		if (!ch || !item)
			return 2;
		
		LogManager& rkLogMgr = LogManager::instance();
		ITEM_MANAGER& rkItemMgr = ITEM_MANAGER::instance();

		if (!RefinableRod(item))
		{
			sys_err("REFINE_ROD_HACK pid(%u) item(%s:%d) type(%d)", ch->GetPlayerID(), item->GetName(), item->GetID(), item->GetType());
			rkLogMgr.RefineLog(ch->GetPlayerID(), item->GetName(), item->GetID(), -1, 1, "ROD_HACK");
			return 2;
		}

		CItem& rkOldRod = *item;

		const int iAdv = rkOldRod.GetValue(0) / 10;

		if (number(1, 100) <= rkOldRod.GetValue(3))
		{
			rkLogMgr.RefineLog(ch->GetPlayerID(), rkOldRod.GetName(), rkOldRod.GetID(), iAdv, 1, "ROD");

			LPITEM pkNewRod = ITEM_MANAGER::instance().CreateItem(rkOldRod.GetRefinedVnum(), 1);
			if (pkNewRod)
			{
				item->CopyAttributeTo(pkNewRod);

				const WORD bCell = rkOldRod.GetCell();
				rkItemMgr.RemoveItem(item, "REMOVE (REFINE ROD)");
				pkNewRod->AddToCharacter(ch, TItemPos(INVENTORY, bCell));
				LogManager::instance().ItemLog(ch, pkNewRod, "REFINE ROD SUCCESS", pkNewRod->GetName());
				return 1;
			}

			return 2;
		}
		else
		{
			rkLogMgr.RefineLog(ch->GetPlayerID(), rkOldRod.GetName(), rkOldRod.GetID(), iAdv, 0, "ROD");

			rkOldRod.SetSocket(0, isRefineWithExtraItem ? rkOldRod.GetSocket(0) / 2 : 0);

			rkLogMgr.ItemLog(ch, item, "REFINE ROD FAIL", item->GetName());
		}

		return 0;
	}
}
