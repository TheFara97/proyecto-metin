#ifndef __INC_METIN_II_GAME_CONSTANTS_H__
#define __INC_METIN_II_GAME_CONSTANTS_H__

#include "../../common/tables.h"

enum EMonsterChatState
{
	MONSTER_CHAT_WAIT,
	MONSTER_CHAT_ATTACK,
	MONSTER_CHAT_CHASE,
	MONSTER_CHAT_ATTACKED,
};

typedef struct SMobRankStat
{
	int iGoldPercent;
} TMobRankStat;

typedef struct SMobStat
{
	BYTE	byLevel;
	WORD	HP;
	DWORD	dwExp;
	WORD	wDefGrade;
} TMobStat;

typedef struct SBattleTypeStat
{
	int		AttGradeBias;
	int		DefGradeBias;
	int		MagicAttGradeBias;
	int		MagicDefGradeBias;
} TBattleTypeStat;

typedef struct SJobInitialPoints
{
	int		st, ht, dx, iq;
	int		max_hp, max_sp;
	int		hp_per_ht, sp_per_iq;
	int		hp_per_lv_begin, hp_per_lv_end;
	int		sp_per_lv_begin, sp_per_lv_end;
	int		max_stamina;
	int		stamina_per_con;
	int		stamina_per_lv_begin, stamina_per_lv_end;
} TJobInitialPoints;

typedef struct __coord
{
	int		x, y;
} Coord;

typedef struct SApplyInfo
{
	BYTE	bPointType;                          // APPLY -> POINT
} TApplyInfo;

enum {
	FORTUNE_BIG_LUCK,
	FORTUNE_LUCK,
	FORTUNE_SMALL_LUCK,
	FORTUNE_NORMAL,
	FORTUNE_SMALL_BAD_LUCK,
	FORTUNE_BAD_LUCK,
	FORTUNE_BIG_BAD_LUCK,
	FORTUNE_MAX_NUM,
};

const int STONE_INFO_MAX_NUM = 10;
const int STONE_LEVEL_MAX_NUM = 4;

struct SStoneDropInfo
{
	DWORD dwMobVnum;
	int iDropPct;
	int iLevelPct[STONE_LEVEL_MAX_NUM+1];
};

inline bool operator < (const SStoneDropInfo& l, DWORD r)
{
	return l.dwMobVnum < r;
}

inline bool operator < (DWORD l, const SStoneDropInfo& r)
{
	return l < r.dwMobVnum;
}

inline bool operator < (const SStoneDropInfo& l, const SStoneDropInfo& r)
{
	return l.dwMobVnum < r.dwMobVnum;
}

extern const TApplyInfo		aApplyInfo[MAX_APPLY_NUM];
extern const TMobRankStat       MobRankStats[MOB_RANK_MAX_NUM];

extern TBattleTypeStat		BattleTypeStats[BATTLE_TYPE_MAX_NUM];

extern const DWORD		party_exp_distribute_table[PLAYER_EXP_TABLE_MAX + 1];

extern const DWORD		exp_table_common[PLAYER_MAX_LEVEL_CONST + 1];

extern const DWORD*		exp_table;

extern const DWORD		guild_exp_table[GUILD_MAX_LEVEL + 1];
extern const DWORD		guild_exp_table2[GUILD_MAX_LEVEL + 1];

#define MAX_EXP_DELTA_OF_LEV	67
#define PERCENT_LVDELTA(me, victim) aiPercentByDeltaLev_euckr[MINMAX(0, (victim + 35) - me, MAX_EXP_DELTA_OF_LEV - 1)]
#define PERCENT_LVDELTA_BOSS(me, victim) aiPercentByDeltaLevForBoss_euckr[MINMAX(0, (victim + 35) - me, MAX_EXP_DELTA_OF_LEV - 1)]
#define CALCULATE_VALUE_LVDELTA(me, victim, val) ((val * PERCENT_LVDELTA(me, victim)) / 100)
extern const int		aiPercentByDeltaLev_euckr[MAX_EXP_DELTA_OF_LEV];
extern const int		aiPercentByDeltaLevForBoss_euckr[MAX_EXP_DELTA_OF_LEV];
extern const int *		aiPercentByDeltaLev;
extern const int *		aiPercentByDeltaLevForBoss;

#define ARROUND_COORD_MAX_NUM	161
extern Coord			aArroundCoords[ARROUND_COORD_MAX_NUM];
extern TJobInitialPoints	JobInitialPoints[JOB_MAX_NUM];

extern const int		aiMobEnchantApplyIdx[MOB_ENCHANTS_MAX_NUM];
extern const int		aiMobResistsApplyIdx[MOB_RESISTS_MAX_NUM];

extern const int		aSkillAttackAffectProbByRank[MOB_RANK_MAX_NUM];

extern const int aiItemMagicAttributePercentHigh[ITEM_ATTRIBUTE_MAX_LEVEL];
extern const int aiItemMagicAttributePercentLow[ITEM_ATTRIBUTE_MAX_LEVEL];

extern const int aiItemAttributeAddPercent[ITEM_ATTRIBUTE_MAX_NUM];

extern const int aiWeaponSocketQty[WEAPON_NUM_TYPES];
extern const int aiArmorSocketQty[ARMOR_NUM_TYPES];
extern const int aiSocketPercentByQty[5][4];

extern const int aiExpLossPercents[PLAYER_EXP_TABLE_MAX + 1];

extern const int * aiSkillPowerByLevel;
extern const int aiSkillPowerByLevel_euckr[SKILL_MAX_LEVEL + 1];

extern const int aiPolymorphPowerByLevel[SKILL_MAX_LEVEL + 1];

extern const int aiSkillBookCountForLevelUp[10];
extern const int aiGrandMasterSkillBookCountForLevelUp[10];
extern const int aiGrandMasterSkillBookMinCount[10];
extern const int aiGrandMasterSkillBookMaxCount[10];
extern const int CHN_aiPartyBonusExpPercentByMemberCount[9];
extern const int KOR_aiPartyBonusExpPercentByMemberCount[9];
extern const int KOR_aiUniqueItemPartyBonusExpPercentByMemberCount[9];

extern const std::vector<DWORD> v_dss_chest_items;

typedef std::map<DWORD, TItemAttrTable> TItemAttrMap;
extern TItemAttrMap g_map_itemAttr;
extern TItemAttrMap g_map_itemRare;
#ifdef ENABLE_BELT_ATTRIBUTES
extern TItemAttrMap g_map_beltAttr;
#endif
#ifdef ENABLE_GLOVE_ATTRIBUTES
extern TItemAttrMap g_map_gloveAttr;
#endif
#ifdef ENABLE_TALISMAN_ATTRIBUTES
extern TItemAttrMap g_map_talismanAttr;
#endif
#ifdef ENABLE_TALISMAN_ATTRIBUTES_RARE
extern TItemAttrMap g_map_talismanAttrRare;
#endif

extern const int * aiChainLightningCountBySkillLevel;
extern const int aiChainLightningCountBySkillLevel_euckr[SKILL_MAX_LEVEL + 1];

extern const char * c_apszEmpireNames[EMPIRE_MAX_NUM];
extern const char * c_apszPrivNames[MAX_PRIV_NUM];
extern const SStoneDropInfo aStoneDrop[STONE_INFO_MAX_NUM];

typedef struct SGuildWarInfo
{
	int32_t lMapIndex;
	int iWarPrice;
	int iWinnerPotionRewardPctToWinner;
	int iLoserPotionRewardPctToWinner;
	int iInitialScore;
	int iEndScore;
} TGuildWarInfo;

extern TGuildWarInfo KOR_aGuildWarInfo[GUILD_WAR_TYPE_MAX_NUM];

// ACCESSORY_REFINE
enum
{
	ITEM_ACCESSORY_SOCKET_MAX_NUM = 3
};

extern const int aiAccessorySocketAddPct[ITEM_ACCESSORY_SOCKET_MAX_NUM];
extern const int aiAccessorySocketEffectivePct[ITEM_ACCESSORY_SOCKET_MAX_NUM + 1];
extern const int aiAccessorySocketDegradeTime[ITEM_ACCESSORY_SOCKET_MAX_NUM + 1];
extern const int aiAccessorySocketPutPct[ITEM_ACCESSORY_SOCKET_MAX_NUM + 1];
int32_t FN_get_apply_type(const char *apply_type_string);
#ifdef ENABLE_RESP_SYSTEM
extern std::set<uint32_t> g_setRespAllowedMap;
#endif
#ifdef ENABLE_MOUNT_SYSTEM
extern const uint32_t	mount_exp_table[150 + 1];
#endif
#ifdef __CASKET_PREVIEW_ENABLE__
extern std::map<DWORD, std::vector<std::pair<DWORD, WORD>>> m_casket_preview;
#endif
// END_OF_ACCESSORY_REFINE
	// Teleport data: {index, mapIndex, x, y, min_level, max_level}
static const std::vector<std::tuple<int, int, int, int, int, int>> TELEPORT_DATA = {
	// Index 0 reserved
	std::make_tuple(0, 0, 0, 0, 1, 999),

	// Basic locations (1-4)
	std::make_tuple(1, 1, 969600, 278400, 1, 999),
	std::make_tuple(2, 1, 873100, 242600, 1, 999),
	std::make_tuple(3, 41, 469300, 964200, 1, 999),
	std::make_tuple(4, 41, 360800, 877600, 1, 999),

	// General locations (5-9)
	std::make_tuple(5, 1, 1075900, 267800, 1, 999),
	std::make_tuple(6, 3, 118900, 98700, 30, 999),
	std::make_tuple(7, 3, 614900, 289900, 30, 999),
	std::make_tuple(8, 3, 86400, 338400, 40, 999),
	std::make_tuple(9, 3, 576100, 385900, 40, 999),

	// Level-restricted locations (10-14)
	std::make_tuple(10, 3, 990900, 1245300, 75, 999),
	std::make_tuple(11, 4, 303200, 473300, 100, 210),
	std::make_tuple(12, 61, 372900, 304300, 125, 210),
	std::make_tuple(13, 62, 598100, 622600, 150, 210),
	std::make_tuple(14, 4, 425000, 413700, 175, 210),

	// Activity locations (15-16)
	std::make_tuple(15, 3, 158000, 569800, 1, 999),
	std::make_tuple(16, 3, 256000, 665600, 1, 999),

	// End-game locations (17-18)
	std::make_tuple(17, 107, 1085500, 423200, 1, 999),
	std::make_tuple(18, 50, 701200, 1115500, 200, 999),

	// Easter Map
	//std::make_tuple(19, 30, 865500, 1216900, 1, 999),
};

int32_t FN_get_apply_type(const char *apply_type_string);
#endif
#ifdef __DUNGEON_INFO__
extern const std::map<DWORD, std::pair<std::pair<BYTE, BYTE>, std::string>> m_mapDungeonList;
extern const std::map<DWORD, uint16_t> m_mapDungeonIndexList;
#endif
//martysama0134's ceqyqttoaf71vasf9t71218
