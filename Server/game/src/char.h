#ifndef __INC_METIN_II_CHAR_H__
#define __INC_METIN_II_CHAR_H__

#include <boost/unordered_map.hpp>
#include "../../common/stl.h"
#include "entity.h"
#include "FSM.h"
#include "horse_rider.h"
#include "vid.h"
#include "constants.h"
#include "affect.h"
#include "affect_flag.h"
#include "mining.h"
#include "../../common/CommonDefines.h"
#include "../../common/item_length.h"
#ifdef _ENABLE_BATTLEPASS_
#include "BattlePassManager.h"
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
#include <vector>
#endif
#if defined(__DUNGEON_INFO_SYSTEM__)
#	include "dungeon_info.h"
#endif

#include "cube.h"
#include "abuse.h"
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
class CMountSystem;
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
#include "packet.h"
class CPrivateShop;
#endif

#include "packet.h"

#include "Inventory.hpp"
#include <boost/signals2.hpp>

namespace item {
#ifdef __ENABLE_ITEM_TOGGLE__
	class ItemToggle;
#endif
}

#ifdef ENABLE_OFFLINE_SHOP
namespace offlineshop
{
	class CShop;
	class CShopSafebox;
}
#endif

#define ENABLE_ANTI_CMD_FLOOD
#define ENABLE_OPEN_SHOP_WITH_ARMOR
#define ENABLE_SKILL_COOLDOWN_CHECK

enum eMountType {MOUNT_TYPE_NONE=0, MOUNT_TYPE_NORMAL=1, MOUNT_TYPE_COMBAT=2, MOUNT_TYPE_MILITARY=3};
eMountType GetMountLevelByVnum(DWORD dwMountVnum, bool IsNew);
const DWORD GetRandomSkillVnum(BYTE bJob = JOB_MAX_NUM);
namespace cheat {
	class Controller;
};

#ifdef ENABLE_VOTE4BUFF
// Bonuses must be set with its APPLY_ declaration.
// You can add as many bonuses as you want per slot.
static std::vector<std::vector<std::pair<int, int>>> s_VOTE4BUFF_BONUSES =
{
	{ // Bonus button 1
		{ APPLY_ATTBONUS_BOSS, 5 },
		//{ APPLY_DEF_GRADE_BONUS, 50 },
		// Add here more bonuses if wanted
	},

	{ // Bonus button 2
		{ APPLY_ATTBONUS_STONE, 5 },
		//{ APPLY_ATTBONUS_INSECT, 5 },
		// Add here more bonuses if wanted
	},

	{ // Bonus button 3
		{ APPLY_EXP_DOUBLE_BONUS, 50 },
		//{ APPLY_EXP_DOUBLE_BONUS, 10 },
		// Add here more bonuses if wanted
	},
};

static std::vector<std::pair<DWORD, int>> s_VOTE4BUFF_ITEM_REWARDS =
{
	//{ vnum, count },
	{ 201234, 1 },
};

// Cooldown when you change the bonus on the UI (it does not apply when you choose a bonus right after voting) in seconds, 0 = no cooldown
static const int s_VOTE4BUFF_CHANGE_BONUS_COOLDOWN = 300; // seconds

// Price when you change the bonus on the UI (it does not apply when you choose a bonus right after voting)
static const int s_VOTE4BUFF_CHANGE_BONUS_PRICE = 15000; // 0 = free

// API SETTINGS from metin2pserver.info
static const std::string s_VOTE4BUFF_SERVER_ID = "elvion";        // server slug on metin2pserver.info
static const std::string s_VOTE4BUFF_OWNER_EMAIL = "jaxpokepro@gmail.com"; // YOUR metin2pserver.info account email

static const int s_VOTE4BUFF_DURATION = 86400; // seconds (1 day) DO NOT MODIFY THIS !!!
#endif

class CBuffOnAttributes;
class CPetSystem;
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
class CBuffNPCActor;
#endif
class CFishWiki;

#define INSTANT_FLAG_DEATH_PENALTY		(1 << 0)
#define INSTANT_FLAG_SHOP			(1 << 1)
#define INSTANT_FLAG_EXCHANGE			(1 << 2)
#define INSTANT_FLAG_STUN			(1 << 3)
#define INSTANT_FLAG_NO_REWARD			(1 << 4)
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
#define INSTANT_FLAG_OFFLINE_SHOP		(1 << 5)
#endif

#define AI_FLAG_NPC				(1 << 0)
#define AI_FLAG_AGGRESSIVE			(1 << 1)
#define AI_FLAG_HELPER				(1 << 2)
#define AI_FLAG_STAYZONE			(1 << 3)
#ifdef ENABLE_SYSTEMY_KARTY_OKEY
#define MAX_CARDS_IN_HAND	5
#define MAX_CARDS_IN_FIELD	3
#endif

#define SET_OVER_TIME(ch, time)	(ch)->SetOverTime(time)

extern int g_nPortalLimitTime;

enum
{
	MAIN_RACE_WARRIOR_M,
	MAIN_RACE_ASSASSIN_W,
	MAIN_RACE_SURA_M,
	MAIN_RACE_SHAMAN_W,
	MAIN_RACE_WARRIOR_W,
	MAIN_RACE_ASSASSIN_M,
	MAIN_RACE_SURA_W,
	MAIN_RACE_SHAMAN_M,
#ifdef ENABLE_WOLFMAN_CHARACTER
	MAIN_RACE_WOLFMAN_M,
#endif
	MAIN_RACE_MAX_NUM,
};

enum
{
	POISON_LENGTH = 30,
#ifdef ENABLE_WOLFMAN_CHARACTER
	BLEEDING_LENGTH = 30,
#endif
	STAMINA_PER_STEP = 1,
	SAFEBOX_PAGE_SIZE = 9,
	AI_CHANGE_ATTACK_POISITION_TIME_NEAR = 10000,
	AI_CHANGE_ATTACK_POISITION_TIME_FAR = 1000,
	AI_CHANGE_ATTACK_POISITION_DISTANCE = 100,
	SUMMON_MONSTER_COUNT = 3,
};

enum
{
	FLY_NONE,
	FLY_EXP,
	FLY_HP_MEDIUM,
	FLY_HP_BIG,
	FLY_SP_SMALL,
	FLY_SP_MEDIUM,
	FLY_SP_BIG,
	FLY_FIREWORK1,
	FLY_FIREWORK2,
	FLY_FIREWORK3,
	FLY_FIREWORK4,
	FLY_FIREWORK5,
	FLY_FIREWORK6,
	FLY_FIREWORK_CHRISTMAS,
	FLY_CHAIN_LIGHTNING,
	FLY_HP_SMALL,
	FLY_SKILL_MUYEONG,
#ifdef ENABLE_QUIVER_SYSTEM
	FLY_QUIVER_ATTACK_NORMAL,
#endif
};

enum EDamageType
{
	DAMAGE_TYPE_NONE,
	DAMAGE_TYPE_NORMAL,
	DAMAGE_TYPE_NORMAL_RANGE,
	DAMAGE_TYPE_MELEE,
	DAMAGE_TYPE_RANGE,
	DAMAGE_TYPE_FIRE,
	DAMAGE_TYPE_ICE,
	DAMAGE_TYPE_ELEC,
	DAMAGE_TYPE_MAGIC,
	DAMAGE_TYPE_POISON,
	DAMAGE_TYPE_SPECIAL,
#ifdef ENABLE_WOLFMAN_CHARACTER
	DAMAGE_TYPE_BLEEDING,
#endif
};

enum DamageFlag
{
	DAMAGE_NORMAL	= (1 << 0),
	DAMAGE_POISON	= (1 << 1),
	DAMAGE_DODGE	= (1 << 2),
	DAMAGE_BLOCK	= (1 << 3),
	DAMAGE_PENETRATE= (1 << 4),
	DAMAGE_CRITICAL = (1 << 5),
#if defined(ENABLE_WOLFMAN_CHARACTER) && !defined(USE_MOB_BLEEDING_AS_POISON)
	DAMAGE_BLEEDING	= (1 << 6),
#endif
};

enum EPointTypes
{
	POINT_NONE,                 // 0
	POINT_LEVEL,                // 1
	POINT_VOICE,                // 2
	POINT_EXP,                  // 3
	POINT_NEXT_EXP,             // 4
	POINT_HP,                   // 5
	POINT_MAX_HP,               // 6
	POINT_SP,                   // 7
	POINT_MAX_SP,               // 8
	POINT_STAMINA,              // 9
	POINT_MAX_STAMINA,          // 10

	POINT_GOLD,                 // 11
	POINT_ST,                   // 12
	POINT_HT,                   // 13
	POINT_DX,                   // 14
	POINT_IQ,                   // 15
	POINT_DEF_GRADE,		// 16 ...
	POINT_ATT_SPEED,            // 17
	POINT_ATT_GRADE,		// 18
	POINT_MOV_SPEED,            // 19
	POINT_CLIENT_DEF_GRADE,	// 20
	POINT_CASTING_SPEED,        // 21
	POINT_MAGIC_ATT_GRADE,      // 22
	POINT_MAGIC_DEF_GRADE,      // 23
	POINT_EMPIRE_POINT,         // 24
	POINT_LEVEL_STEP,           // 25
	POINT_STAT,                 // 26
	POINT_SUB_SKILL,		// 27
	POINT_SKILL,		// 28
	POINT_WEAPON_MIN,		// 29
	POINT_WEAPON_MAX,		// 30
	POINT_PLAYTIME,             // 31
	POINT_HP_REGEN,             // 32
	POINT_SP_REGEN,             // 33

	POINT_BOW_DISTANCE,         // 34

	POINT_HP_RECOVERY,          // 35
	POINT_SP_RECOVERY,          // 36

	POINT_POISON_PCT,           // 37
	POINT_STUN_PCT,             // 38
	POINT_SLOW_PCT,             // 39
	POINT_CRITICAL_PCT,         // 40
	POINT_PENETRATE_PCT,        // 41
	POINT_CURSE_PCT,            // 42

	POINT_ATTBONUS_HUMAN,       // 43
	POINT_ATTBONUS_ANIMAL,      // 44
	POINT_ATTBONUS_ORC,         // 45
	POINT_ATTBONUS_MILGYO,      // 46
	POINT_ATTBONUS_UNDEAD,      // 47
	POINT_ATTBONUS_DEVIL,       // 48
	POINT_ATTBONUS_INSECT,      // 49
	POINT_ATTBONUS_FIRE,        // 50
	POINT_ATTBONUS_ICE,         // 51
	POINT_ATTBONUS_DESERT,      // 52
	POINT_ATTBONUS_MONSTER,     // 53
	POINT_ATTBONUS_WARRIOR,     // 54
	POINT_ATTBONUS_ASSASSIN,	// 55
	POINT_ATTBONUS_SURA,		// 56
	POINT_ATTBONUS_SHAMAN,		// 57
	POINT_ATTBONUS_TREE,     	// 58

	POINT_RESIST_WARRIOR,		// 59
	POINT_RESIST_ASSASSIN,		// 60
	POINT_RESIST_SURA,			// 61
	POINT_RESIST_SHAMAN,		// 62

	POINT_STEAL_HP,             // 63
	POINT_STEAL_SP,             // 64

	POINT_MANA_BURN_PCT,        // 65
	POINT_DAMAGE_SP_RECOVER,    // 66

	POINT_BLOCK,                // 67
	POINT_DODGE,                // 68

	POINT_RESIST_SWORD,         // 69
	POINT_RESIST_TWOHAND,       // 70
	POINT_RESIST_DAGGER,        // 71
	POINT_RESIST_BELL,          // 72
	POINT_RESIST_FAN,           // 73
	POINT_RESIST_BOW,           // 74
	POINT_RESIST_FIRE,          // 75
	POINT_RESIST_ELEC,          // 76
	POINT_RESIST_MAGIC,         // 77
	POINT_RESIST_WIND,          // 78

	POINT_REFLECT_MELEE,        // 79

	POINT_REFLECT_CURSE,		// 80
	POINT_POISON_REDUCE,		// 81

	POINT_KILL_SP_RECOVER,		// 82
	POINT_EXP_DOUBLE_BONUS,		// 83
	POINT_GOLD_DOUBLE_BONUS,	// 84
	POINT_ITEM_DROP_BONUS,		// 85

	POINT_POTION_BONUS,			// 86
	POINT_KILL_HP_RECOVERY,		// 87

	POINT_IMMUNE_STUN,			// 88
	POINT_IMMUNE_SLOW,			// 89
	POINT_IMMUNE_FALL,			// 90
	//////////////////

	POINT_PARTY_ATTACKER_BONUS,		// 91
	POINT_PARTY_TANKER_BONUS,		// 92

	POINT_ATT_BONUS,			// 93
	POINT_DEF_BONUS,			// 94

	POINT_ATT_GRADE_BONUS,		// 95
	POINT_DEF_GRADE_BONUS,		// 96
	POINT_MAGIC_ATT_GRADE_BONUS,	// 97
	POINT_MAGIC_DEF_GRADE_BONUS,	// 98

	POINT_RESIST_NORMAL_DAMAGE,		// 99

	POINT_HIT_HP_RECOVERY,		// 100
	POINT_HIT_SP_RECOVERY, 		// 101
	POINT_MANASHIELD,			// 102

	POINT_PARTY_BUFFER_BONUS,		// 103
	POINT_PARTY_SKILL_MASTER_BONUS,	// 104

	POINT_HP_RECOVER_CONTINUE,		// 105
	POINT_SP_RECOVER_CONTINUE,		// 106

	POINT_STEAL_GOLD,			// 107
	POINT_POLYMORPH,			// 108
	POINT_MOUNT,			// 109

	POINT_PARTY_HASTE_BONUS,		// 110
	POINT_PARTY_DEFENDER_BONUS,		// 111
	POINT_STAT_RESET_COUNT,		// 112

	POINT_HORSE_SKILL,			// 113

	POINT_MALL_ATTBONUS,		// 114
	POINT_MALL_DEFBONUS,		// 115
	POINT_MALL_EXPBONUS,		// 116
	POINT_MALL_ITEMBONUS,		// 117
	POINT_MALL_GOLDBONUS,		// 118

	POINT_MAX_HP_PCT,			// 119
	POINT_MAX_SP_PCT,			// 120

	POINT_SKILL_DAMAGE_BONUS,		// 121
	POINT_NORMAL_HIT_DAMAGE_BONUS,	// 122

	// DEFEND_BONUS_ATTRIBUTES
	POINT_SKILL_DEFEND_BONUS,		// 123
	POINT_NORMAL_HIT_DEFEND_BONUS,	// 124
	// END_OF_DEFEND_BONUS_ATTRIBUTES

	POINT_PC_BANG_EXP_BONUS,		// 125
	POINT_PC_BANG_DROP_BONUS,		// 126

	POINT_RAMADAN_CANDY_BONUS_EXP,		// 127

	POINT_ENERGY = 128,

	POINT_ENERGY_END_TIME = 129,

	POINT_COSTUME_ATTR_BONUS = 130,
	POINT_MAGIC_ATT_BONUS_PER = 131,
	POINT_MELEE_MAGIC_ATT_BONUS_PER = 132,

	POINT_RESIST_ICE = 133,
	POINT_RESIST_EARTH = 134,
	POINT_RESIST_DARK = 135,

	POINT_RESIST_CRITICAL = 136,
	POINT_RESIST_PENETRATE = 137,

#ifdef ENABLE_WOLFMAN_CHARACTER
	POINT_BLEEDING_REDUCE = 138,
	POINT_BLEEDING_PCT = 139,

	POINT_ATTBONUS_WOLFMAN = 140,
	POINT_RESIST_WOLFMAN = 141,
	POINT_RESIST_CLAW = 142,
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	POINT_ACCEDRAIN_RATE = 143,
#endif
#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
	POINT_RESIST_MAGIC_REDUCTION = 144,
#endif

	POINT_ENCHANT_ELECT = 145,
	POINT_ENCHANT_FIRE = 146,
	POINT_ENCHANT_ICE = 147,
	POINT_ENCHANT_WIND = 148,
	POINT_ENCHANT_EARTH = 149,
	POINT_ENCHANT_DARK = 150,
	POINT_RESIST_HUMAN,
	POINT_ATTBONUS_BOSS,
	POINT_ATTBONUS_WLADCA,
	POINT_ATTBONUS_STONE,
	POINT_RESIST_BOSS,
	POINT_RESIST_WLADCA,
	POINT_RESIST_MONSTER,
	POINT_STAT_BONUS,
	POINT_ATTBONUS_DUNGEON,
	POINT_ATTBONUS_LEGENDA,

	POINT_ATTBONUS_KLASY,
	POINT_RESIST_KLASY,
	POINT_DMG_BONUS, // 163
	POINT_FINAL_DMG_BONUS, // 164

#ifdef WEEKLY_RANK_BYLUZER
	POINT_WEEKLY1 = 170,
	POINT_WEEKLY2 = 171,
	POINT_WEEKLY3 = 172,
	POINT_WEEKLY4 = 173,
	POINT_WEEKLY5 = 174,
	POINT_WEEKLY6 = 175,
	POINT_WEEKLY7 = 176,
	POINT_WEEKLY_SEASON = 177,
#endif

	POINT_MINING_SUCCESS_CHANCE, //246
	POINT_MINING_MORE_ORES_CHANCE, //247
	POINT_MINING_LESS_MINE_TIME, //248
	POINT_MINING_MORE_POINTS, //249

	POINT_FISHING_INSTANT_CATCH_CHANCE, //250
	POINT_FISHING_BIGGER_FISH_CHANCE,	//251
	POINT_FISHING_NEXT_SECTION_CHANCE,	//252
	POINT_FISHING_MORE_YANG_MULTIPLIER, //253
	POINT_FISHING_MORE_POINTS_CHANCE,	//254
	POINT_FISHING_SUCCESS_CHANCE,		//255
	POINT_ATTBONUS_PVM_SKILL,		//188
	POINT_ATTBONUS_ELEMENT,		//189
	POINT_DEFBONUS_ELEMENT,		//190
	POINT_DOUBLE_LOOT_SUMMER,		//190
	POINT_DOUBLE_LOOT_DROP,		//190
	POINT_ATTBONUS_PVM_STR,
	POINT_MOUNT_MOVEMENT_SPEED, // 194

#ifdef ENABLE_CHEQUE_SYSTEM
	POINT_CHEQUE = 241,
#endif
#ifdef ENABLE_GLOVE_SYSTEM
	POINT_RANDOM = 242,
#endif
#ifdef ENABLE_PUNKTY_OSIAGNIEC
	POINT_PKT_OSIAG = 243,
#endif
#ifdef ENABLE_SECONDARY_LEVEL
	POINT_SECONDARY_LEVEL = 244,
#endif
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
	POINT_ENLIGHT_LEVEL = 245,
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
	POINT_PRIVATE_SHOP_UNLOCKED_SLOT,
#endif

	POINT_MAX,
};

enum EPKModes
{
	PK_MODE_PEACE,
	PK_MODE_REVENGE,
	PK_MODE_FREE,
	PK_MODE_PROTECT,
	PK_MODE_GUILD,
#ifdef ENABLE_PVP_RANKING
	PK_MODE_BATTLE,
#endif
	PK_MODE_MAX_NUM
};

enum EPositions
{
	POS_DEAD,
	POS_SLEEPING,
	POS_RESTING,
	POS_SITTING,
	POS_FISHING,
	POS_FIGHTING,
	POS_MOUNTING,
	POS_STANDING
};

enum EBlockAction
{
	BLOCK_EXCHANGE		= (1 << 0),
	BLOCK_PARTY_INVITE		= (1 << 1),
	BLOCK_GUILD_INVITE		= (1 << 2),
	BLOCK_WHISPER		= (1 << 3),
	BLOCK_MESSENGER_INVITE	= (1 << 4),
	BLOCK_PARTY_REQUEST		= (1 << 5),
};

// <Factor> Dynamically evaluated CHARACTER* equivalent.
// Referring to SCharDeadEventInfo.
struct DynamicCharacterPtr {
	DynamicCharacterPtr() : is_pc(false), id(0) {}
	DynamicCharacterPtr(const DynamicCharacterPtr& o)
		: is_pc(o.is_pc), id(o.id) {}

	// Returns the LPCHARACTER found in CHARACTER_MANAGER.
	LPCHARACTER Get() const;
	// Clears the current settings.
	void Reset() {
		is_pc = false;
		id = 0;
	}

	// Basic assignment operator.
	DynamicCharacterPtr& operator=(const DynamicCharacterPtr& rhs) {
		is_pc = rhs.is_pc;
		id = rhs.id;
		return *this;
	}
	// Supports assignment with LPCHARACTER type.
	DynamicCharacterPtr& operator=(LPCHARACTER character);
	// Supports type casting to LPCHARACTER.
	operator LPCHARACTER() const {
		return Get();
	}

	bool is_pc;
	uint32_t id;
};

typedef struct character_point
{
#ifdef ENABLE_LONG_LONG
	int64_t			points[POINT_MAX_NUM];
#else
	int			points[POINT_MAX_NUM];
#endif

	BYTE			job;
	BYTE			voice;

	BYTE			level;
#ifdef ENABLE_SECONDARY_LEVEL
	BYTE			secondary_level;
#endif
	DWORD			exp;
#ifdef ENABLE_LONG_LONG
	int64_t		gold;
#else
	int32_t			gold;
#endif
#ifdef ENABLE_CHEQUE_SYSTEM
	int64_t		cheque;
#endif
#ifdef WEEKLY_RANK_BYLUZER
	int			 	w_monsters;
	int				w_bosses;
	int 			w_metins;
	int 			w_dungs;
	int 			w_ds;
	int 			w_refine;
	int 			w_sm;
	int				weekly_season;
#endif
#ifdef ENABLE_PUNKTY_OSIAGNIEC
	int				pkt_osiag;
#endif
	int64_t		hp;
	int64_t		sp;

	int64_t		iRandomHP;
	int64_t		iRandomSP;

	int				stamina;

	BYTE			skill_group;
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
	uint8_t			enlightLevel;
#endif
#ifdef ENABLE_MOUNT_SYSTEM
	int iMountLevel;
	int iMountExp;
#endif
#ifdef ENABLE_PVP_RANKING
	int rank_Kill;
	int rank_Dead;
#endif
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
	BYTE	bTitleAchievement;
	BYTE	bTitleAchievementPremium;
#endif
} CHARACTER_POINT;

typedef struct character_point_instant
{
#ifdef ENABLE_LONG_LONG
	int64_t			points[POINT_MAX_NUM];
#else
	int			points[POINT_MAX_NUM];
#endif

	float			fRot;

	int64_t		iMaxHP;
	int64_t		iMaxSP;

	int32_t			position;

	int32_t			instant_flag;
	DWORD			dwAIFlag;
	DWORD			dwImmuneFlag;
	DWORD			dwLastShoutPulse;

	DWORD			parts[PART_MAX_NUM]; // @fixme502

	LPCHARACTER		pCubeNpc;
	LPCHARACTER		battle_victim;
#ifdef WEEKLY_RANK_BYLUZER
	int				weekly_season;
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	LPITEM 			pBuffEquipmentItem[BUFF_WINDOW_SLOT_MAX_NUM];
#endif
	BYTE			gm_level;

#ifdef ENABLE_AURA_SYSTEM
	LPITEM			pAuraMaterials[AURA_WINDOW_MAX_MATERIALS];
#endif

	BYTE			bBasePart;

	int				iMaxStamina;

	BYTE			bBlockMode;

	int				iDragonSoulActiveDeck;
	LPENTITY		m_pDragonSoulRefineWindowOpener;
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	DWORD			real_owner;
	bool			isOfflineShop;
	int                     leftTime;
	BYTE			bSaveTime;
	BYTE			bChannel;
	DWORD			dwVID;	
#endif
} CHARACTER_POINT_INSTANT;

// @fixme199 BEGIN
struct PlayerSlotT {
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	UINT			bItemGrid[INVENTORY_AND_EQUIP_SLOT_MAX];
	LPITEM			pItems[INVENTORY_AND_EQUIP_SLOT_MAX];
#else
	BYTE			bItemGrid[INVENTORY_AND_EQUIP_SLOT_MAX];
	LPITEM			pItems[INVENTORY_AND_EQUIP_SLOT_MAX];
#endif
	LPITEM			pDSItems[DRAGON_SOUL_INVENTORY_MAX_NUM];
	WORD			wDSItemGrid[DRAGON_SOUL_INVENTORY_MAX_NUM];
#ifdef ENABLE_SWITCHBOT
	LPITEM			pSwitchbotItems[SWITCHBOT_SLOT_COUNT];
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	TItemPosEx		pAcceMaterials[ACCE_WINDOW_MAX_MATERIALS];
#endif
	TQuickslot		pQuickslot[QUICKSLOT_MAX_NUM];
};
// @fixme199 END

#define TRIGGERPARAM		LPCHARACTER ch, LPCHARACTER causer

typedef struct trigger
{
	BYTE	type;
	int		(*func) (TRIGGERPARAM);
	int32_t	value;
} TRIGGER;

class CTrigger
{
	public:
		CTrigger() : bType(0), pFunc(NULL)
		{
		}

		BYTE	bType;
		int	(*pFunc) (TRIGGERPARAM);
};

EVENTINFO(char_event_info)
{
	DynamicCharacterPtr ch;
};

typedef std::map<DWORD, size_t> target_map;
struct TSkillUseInfo
{
	int	    iHitCount;
	int	    iMaxHitCount;
	int	    iSplashCount;
	DWORD   dwNextSkillUsableTime;
	int	    iRange;
	bool    bUsed;
	DWORD   dwVID;
	bool    isGrandMaster;
	#ifdef ENABLE_SKILL_COOLDOWN_CHECK
	bool	cooldown{false};
	int		skillCount{0};
	#endif

	target_map TargetVIDMap;

	TSkillUseInfo()
		: iHitCount(0), iMaxHitCount(0), iSplashCount(0), dwNextSkillUsableTime(0), iRange(0), bUsed(false),
		dwVID(0), isGrandMaster(false)
   	{}

	bool    HitOnce(DWORD dwVnum = 0);

	bool    UseSkill(bool isGrandMaster, DWORD vid, DWORD dwCooltime, int splashcount = 1, int hitcount = -1, int range = -1);
	DWORD   GetMainTargetVID() const	{ return dwVID; }
	void    SetMainTargetVID(DWORD vid) { dwVID=vid; }
	void    ResetHitCount() { if (iSplashCount) { iHitCount = iMaxHitCount; iSplashCount--; } }
	#ifdef ENABLE_SKILL_COOLDOWN_CHECK
	bool	IsOnCooldown(int vnum);
	#endif
};
enum ActivateTimeCheckList
{
	REFINE_CHECK_TIME,
	EXCHANGE_CHECK_TIME,
	SAFEBOX_CHECK_TIME,
	MYSHOP_CHECK_TIME,
	OPEN_CHEST_CHECK_TIME,
#ifdef ENABLE_TELEPORT_TO_A_FRIEND
	FRIEND_TELEPORT_CHECK_TIME,
#endif
#ifdef ENABLE_BUFFI_SYSTEM
	BUFFI_SUMMON_TIME,
#endif
	MAX_ACTIVATE_CHECK
};

typedef struct packet_party_update TPacketGCPartyUpdate;
class CExchange;
class CSkillProto;
class CParty;
class CDungeon;
class CWarMap;
class CAffect;
class CGuild;
class CSafebox;
class CArena;

class CShop;
typedef class CShop * LPSHOP;
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
class COfflineShop;
typedef class COfflineShop * LPOFFLINESHOP;
#endif

class CMob;
class CMobInstance;
typedef struct SMobSkillInfo TMobSkillInfo;

//SKILL_POWER_BY_LEVEL
extern int GetSkillPowerByLevelFromType(int job, int skillgroup, int skilllevel);
//END_SKILL_POWER_BY_LEVEL

namespace marriage
{
	class WeddingMap;
}
enum e_overtime
{
	OT_NONE,
	OT_3HOUR,
	OT_5HOUR,
};

#if defined(__ENABLE_LOTTERY__)
typedef struct SLotteryCharacter {
	WORD	MonsterVnum;
	BYTE	LotteryIndex;
	int		LotteryCooltime;
	bool	LotteryStarted;

	SLotteryCharacter() :
		MonsterVnum(0),
		LotteryIndex(LOTTERY_MAX_ITEM),
		LotteryCooltime(0),
		LotteryStarted(false)
	{}

	void Clear() {
		MonsterVnum		= 0;
		LotteryIndex	= LOTTERY_MAX_ITEM;
		LotteryStarted	= false;
	}
} TLotteryCharacter;
#endif

#define NEW_ICEDAMAGE_SYSTEM
class CHARACTER : public CEntity, public CFSM, public CHorseRider
{
	public:
	    using OnClickSignals =
	        boost::signals2::signal<void(CHARACTER&, CHARACTER&)>;

	protected:
		//////////////////////////////////////////////////////////////////////////////////
		virtual void	EncodeInsertPacket(LPENTITY entity);
		virtual void	EncodeRemovePacket(LPENTITY entity);
		//////////////////////////////////////////////////////////////////////////////////

	public:
		LPCHARACTER			FindCharacterInView(const char * name, bool bFindPCOnly);
		void				UpdatePacket();

		//////////////////////////////////////////////////////////////////////////////////
	protected:
		CStateTemplate<CHARACTER>	m_stateMove;
		CStateTemplate<CHARACTER>	m_stateBattle;
		CStateTemplate<CHARACTER>	m_stateIdle;

	public:
		virtual void		StateMove();
		virtual void		StateBattle();
		virtual void		StateIdle();
		virtual void		StateFlag();
		virtual void		StateFlagBase();
		void				StateHorse();

	protected:
		// STATE_IDLE_REFACTORING
		void				__StateIdle_Monster();
		void				__StateIdle_Stone();
		void				__StateIdle_NPC();
		// END_OF_STATE_IDLE_REFACTORING

	public:
		DWORD GetAIFlag() const	{ return m_pointsInstant.dwAIFlag; }

		void				SetAggressive();
		bool				IsAggressive() const;

		void				SetCoward();
		bool				IsCoward() const;
		void				CowardEscape();

		void				SetNoAttackShinsu();
		bool				IsNoAttackShinsu() const;

		void				SetNoAttackChunjo();
		bool				IsNoAttackChunjo() const;

		void				SetNoAttackJinno();
		bool				IsNoAttackJinno() const;

		void				SetAttackMob();
		bool				IsAttackMob() const;

		virtual void			BeginStateEmpty();
		virtual void			EndStateEmpty() {}

		void				RestartAtSamePos();

	protected:
		DWORD				m_dwStateDuration;
		//////////////////////////////////////////////////////////////////////////////////

	public:
		CHARACTER();
		virtual ~CHARACTER();

		void			Create(const char * c_pszName, DWORD vid, bool isPC);
		void			Destroy();

		void			Disconnect(const char * c_pszReason);

	protected:
		void			Initialize();

		//////////////////////////////////////////////////////////////////////////////////
		// Basic Points
	public:
		DWORD			GetPlayerID() const	{ return m_dwPlayerID; }

		void			SetPlayerProto(const TPlayerTable * table);
		void			CreatePlayerProto(TPlayerTable & tab);

		void			SetProto(const CMob * c_pkMob);
		DWORD			GetRaceNum() const; // @fixme501
#ifdef __PET_SYSTEM__
		bool			SummonedPet();
#endif	
		void			Save();		// DelayedSave
		void			SaveReal();
		void			FlushDelayedSaveItem();

		const char *	GetName() const;
		const VID &		GetVID() const		{ return m_vid;		}

		void			SetName(const std::string& name) { m_stName = name; }

		void			SetRace(BYTE race);
#ifdef RENEWAL_DEAD_PACKET
		DWORD			CalculateDeadTime(BYTE type);
#endif
		bool			ChangeSex();

		DWORD			GetAID() const;
		int				GetChangeEmpireCount() const;
		void			SetChangeEmpireCount();
		int				ChangeEmpire(BYTE empire);

		bool CanDoAction();

		BYTE			GetJob() const;
		BYTE			GetCharType() const;

		bool			IsPC() const		{ return GetDesc() ? true : false; }
		bool			IsNPC()	const		{ return m_bCharType != CHAR_TYPE_PC; }
		bool			IsMonster()	const	{ return m_bCharType == CHAR_TYPE_MONSTER; }
		bool			IsStone() const		{ return m_bCharType == CHAR_TYPE_STONE; }
		bool			IsDoor() const		{ return m_bCharType == CHAR_TYPE_DOOR; }
		bool			IsBuilding() const	{ return m_bCharType == CHAR_TYPE_BUILDING;  }
		bool			IsWarp() const		{ return m_bCharType == CHAR_TYPE_WARP; }
		bool			IsGoto() const		{ return m_bCharType == CHAR_TYPE_GOTO; }
		bool			IsPvM() const { return (m_bCharType == CHAR_TYPE_MONSTER) || (m_bCharType == CHAR_TYPE_STONE) || GetMobRank() >= MOB_RANK_BOSS; }
		
		bool			IsMount() const 	{ return ((GetRaceNum() >= 2999 && GetRaceNum() <= 3089) || (GetRaceNum() >= 20101 && GetRaceNum() <= 20109) || (GetRaceNum() >= 20900 && GetRaceNum() <= 20999) || (GetRaceNum() == 20204)); }
//		bool			IsPet() const		{ return m_bCharType == CHAR_TYPE_PET; }
		bool			IsBoss() const		{ return m_bCharType == CHAR_TYPE_MONSTER && GetMobRank() == MOB_RANK_BOSS; }
		bool			IsWladca() const		{ return m_bCharType == CHAR_TYPE_MONSTER && GetMobRank() == MOB_RANK_BOSS; }
		bool			IsElitka() const		{ return m_bCharType == CHAR_TYPE_MONSTER && GetMobRank() == MOB_RANK_ELITKA; }
		bool			IsLegenda() const		{ return m_bCharType == CHAR_TYPE_MONSTER && GetMobRank() == MOB_RANK_LEGENDA; }
		bool			IsGroupDungeonChest() const { return (GetRaceNum() == 16157 || GetRaceNum() == 16179); }
		bool			IsSummerMob() const { return (GetRaceNum() >= 200305 && GetRaceNum() <= 200317); }
		// bool			IsHorse() const		{ return m_bCharType == CHAR_TYPE_HORSE; }
		
		DWORD			GetLastShoutPulse() const	{ return m_pointsInstant.dwLastShoutPulse; }
		void			SetLastShoutPulse(DWORD pulse) { m_pointsInstant.dwLastShoutPulse = pulse; }
		int				GetLevel() const		{ return m_points.level;	}
		void			SetLevel(BYTE level);
		
#ifdef ENABLE_SECONDARY_LEVEL
		BYTE			GetSecondaryLevel() const { return m_points.secondary_level; }
		void			SetSecondaryLevel(BYTE level);
#endif

		BYTE			GetGMLevel() const;
		BOOL 			IsGM() const;
		void			SetGMLevel();

		DWORD			GetExp() const		{ return m_points.exp;	}
		void			SetExp(DWORD exp)	{ m_points.exp = exp;	}
		DWORD			GetNextExp() const;
		LPCHARACTER		DistributeExp();
		void			DistributeHP(LPCHARACTER pkKiller);
		void			DistributeSP(LPCHARACTER pkKiller, int iMethod=0);

		bool IsInSafezone() const;

		void			ConvertTime(DWORD sec_new);
#ifdef ENABLE_SKILE_PASYWNE
		void			LoadPasywki();
		void			LoadPasywkiBonus();
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		void			SendLogs();
		DWORD			m_dwLastLogTime;
		void			SetLastLogsTime(DWORD time) { m_dwLastLogTime = time; }	
		DWORD			GetLastLogsTime() const  { return m_dwLastLogTime;  }
#endif

#ifdef ENABLE_KILL_EVENT_FIX
		LPCHARACTER		GetMostAttacked();
#endif

		void			SetPosition(int pos);
		bool			IsPosition(int pos) const	{ return m_pointsInstant.position == pos ? true : false; }
		int				GetPosition() const		{ return m_pointsInstant.position; }

		void			SetPart(BYTE bPartPos, DWORD wVal); // @fixme502
		DWORD			GetPart(BYTE bPartPos) const; // @fixme502
		DWORD			GetOriginalPart(BYTE bPartPos) const; // @fixme502

		void			SetHP(int64_t hp)		{ m_points.hp = hp; }
		int64_t		GetHP() const		{ return m_points.hp; }

		void			SetSP(int64_t sp)		{ m_points.sp = sp; }
		int64_t		GetSP() const		{ return m_points.sp; }
		
#ifdef ENABLE_PVP_RANKING
		int	GetRankKill()	{ return m_points.rank_Kill; }
		int	GetRankDead()	{ return m_points.rank_Dead; }
		void	SetRankKill(int count)	{ m_points.rank_Kill = count; }
		void	SetRankDead(int count)	{ m_points.rank_Dead = count; }
#endif

		void			SetStamina(int stamina)	{ m_points.stamina = GetMaxStamina(); }
		int				GetStamina() const		{ return m_points.stamina; }

		void			SetMaxHP(int64_t iVal)	{ m_pointsInstant.iMaxHP = iVal; }
		int64_t		GetMaxHP() const	{ return m_pointsInstant.iMaxHP; }

		void			SetMaxSP(int64_t iVal)	{ m_pointsInstant.iMaxSP = iVal; }
		int64_t		GetMaxSP() const	{ return m_pointsInstant.iMaxSP; }

		void			SetMaxStamina(int iVal)	{ m_pointsInstant.iMaxStamina = iVal; }
		int				GetMaxStamina() const	{ return m_pointsInstant.iMaxStamina; }

		void			SetRandomHP(int64_t v)	{ m_points.iRandomHP = v; }
		void			SetRandomSP(int64_t v)	{ m_points.iRandomSP = v; }

		int64_t		GetRandomHP() const	{ return m_points.iRandomHP; }
		int64_t		GetRandomSP() const	{ return m_points.iRandomSP; }

		int				GetHPPct() const;

		void			SetRealPoint(BYTE idx, int val);
		int				GetRealPoint(BYTE idx) const;

		void			SetPoint(BYTE idx, int64_t val);
#ifdef ENABLE_LONG_LONG
		int64_t		GetPoint(BYTE idx) const;
#else
		int				GetPoint(BYTE idx) const;
#endif
		int		GetLimitPoint(BYTE idx) const;
		int64_t		GetPolymorphPoint(BYTE idx) const;

		const TMobTable &	GetMobTable() const;
		BYTE				GetMobRank() const;
		BYTE				GetMobBattleType() const;
		BYTE				GetMobSize() const;
		DWORD				GetMobDamageMin() const;
		DWORD				GetMobDamageMax() const;
		WORD				GetMobAttackRange() const;
		DWORD				GetMobDropItemVnum() const;
		float				GetMobDamageMultiply() const;

		// NEWAI
		bool			IsBerserker() const;
		bool			IsBerserk() const;
		void			SetBerserk(bool mode);

		bool			IsStoneSkinner() const;

		bool			IsGodSpeeder() const;
		bool			IsGodSpeed() const;
		void			SetGodSpeed(bool mode);

		bool			IsDeathBlower() const;
		bool			IsDeathBlow() const;

		bool			IsReviver() const;
		bool			HasReviverInParty() const;
		bool			IsRevive() const;
		void			SetRevive(bool mode);
		// NEWAI END

		bool			IsRaceFlag(DWORD dwBit) const;
		bool			IsSummonMonster() const;
		DWORD			GetSummonVnum() const;

		DWORD			GetPolymorphItemVnum() const;
		DWORD			GetMonsterDrainSPPoint() const;

		void			MainCharacterPacket();
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
		void			ComputeEnlightBonuses();
#endif

		void			ComputePoints();
		void			ComputeBattlePoints();
#ifdef ENABLE_LONG_LONG
		void			PointChange(BYTE type, int64_t amount, bool bAmount = false, bool bBroadcast = false);
#else
		void			PointChange(BYTE type, int amount, bool bAmount = false, bool bBroadcast = false);
#endif
		void			PointsPacket();
		void			ApplyPoint(BYTE bApplyType, int iVal);
		void			CheckMaximumPoints();

#ifdef ENABLE_NEW_PET_SYSTEM
		void			SendPetLevelUpEffect(int vid, int type, int value, int amount);
#endif

		bool			Show(int32_t lMapIndex, int32_t x, int32_t y, int32_t z = INT_MAX, bool bShowSpawnMotion = false);

		void			Sitdown(int is_ground);
		void			Standup();

		void			SetRotation(float fRot);
		void			SetRotationToXY(int32_t x, int32_t y);
		float			GetRotation() const	{ return m_pointsInstant.fRot; }

		void			MotionPacketEncode(BYTE motion, LPCHARACTER victim, struct packet_motion * packet);
		void			Motion(BYTE motion, LPCHARACTER victim = NULL);

		void			ChatPacket(BYTE type, const char *format, ...);
		void			MonsterChat(BYTE bMonsterChatType);
		void			SendGreetMessage();

		void			ResetPoint(int iLv);

		void			SetBlockMode(BYTE bFlag);
		void			SetBlockModeForce(BYTE bFlag);
		bool			IsBlockMode(BYTE bFlag) const	{ return (m_pointsInstant.bBlockMode & bFlag)?true:false; }

		bool			IsPolymorphed() const		{ return m_dwPolymorphRace>0; }
		bool			IsPolyMaintainStat() const	{ return m_bPolyMaintainStat; }
		void			SetPolymorph(DWORD dwRaceNum, bool bMaintainStat = false);
		DWORD			GetPolymorphVnum() const	{ return m_dwPolymorphRace; }
		int				GetPolymorphPower() const;

		// FISING
		void			fishing();
		void			fishing_take();
		// END_OF_FISHING

		// MINING
		void			mining(LPCHARACTER chLoad);
		void			mining_cancel();
		void			mining_take();
		// END_OF_MINING

		void			ResetPlayTime(DWORD dwTimeRemain = 0);

		void			CreateFly(BYTE bType, LPCHARACTER pkVictim);

		void			ResetChatCounter();
		BYTE			IncreaseChatCounter();
		BYTE			GetChatCounter() const;
		void			ResetMountCounter();
		BYTE			IncreaseMountCounter();
		BYTE			GetMountCounter() const;

	protected:
		DWORD			m_dwPolymorphRace;
		bool			m_bPolyMaintainStat;
		DWORD			m_dwLoginPlayTime;
		DWORD			m_dwPlayerID;
		VID				m_vid;
		std::string		m_stName;
		BYTE			m_bCharType;

		CHARACTER_POINT		m_points;
		CHARACTER_POINT_INSTANT	m_pointsInstant;
		std::unique_ptr<PlayerSlotT> m_PlayerSlots; // @fixme199

		int				m_iMoveCount;
		DWORD			m_dwPlayStartTime;
		BYTE			m_bAddChrState;
		bool			m_bSkipSave;
		std::string		m_stMobile;
		char			m_szMobileAuth[5];
		BYTE			m_bChatCounter;
		BYTE			m_bMountCounter;
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		int 			m_offline_vip;
		int 			m_offline_norm;
#endif

		// End of Basic Points

		//////////////////////////////////////////////////////////////////////////////////
		// Move & Synchronize Positions
		//////////////////////////////////////////////////////////////////////////////////
	public:
		bool			IsStateMove() const			{ return IsState((CState&)m_stateMove); }
		bool			IsStateIdle() const			{ return IsState((CState&)m_stateIdle); }
		bool			IsWalking() const			{ return m_bNowWalking || GetStamina()<=0; }
		bool			IsStateBattle() const			{ return IsState((CState&)m_stateBattle); }
		void			SetWalking(bool bWalkFlag)	{ m_bWalking=bWalkFlag; }
		void			SetNowWalking(bool bWalkFlag);
		void			ResetWalking()			{ SetNowWalking(m_bWalking); }

		bool			Goto(int32_t x, int32_t y);
		void			Stop();

		bool			CanMove() const;

		void			SyncPacket();
		bool			Sync(int32_t x, int32_t y);
		bool			Move(int32_t x, int32_t y);
		void			OnMove(bool bIsAttack = false);
		DWORD			GetMotionMode() const;
		float			GetMoveMotionSpeed() const;
		float			GetMoveSpeed() const;
		void			CalculateMoveDuration();
		void			SendMovePacket(BYTE bFunc, BYTE bArg, DWORD x, DWORD y, DWORD dwDuration, DWORD dwTime=0, int iRot=-1);
		DWORD			GetCurrentMoveDuration() const	{ return m_dwMoveDuration; }
		DWORD			GetWalkStartTime() const	{ return m_dwWalkStartTime; }
		DWORD			GetLastMoveTime() const		{ return m_dwLastMoveTime; }
		DWORD			GetLastAttackTime() const	{ return m_dwLastAttackTime; }
		void			MarkPVPHitReceived(uint32_t time) { m_LastPVPHitReceivedTime = time; }
		uint32_t		GetLastPVPHitReceivedTime() const	{ return m_LastPVPHitReceivedTime; }
		void			MarkPVPHitDealt(uint32_t time) { m_LastPVPHitDealtTime = time; }
		uint32_t		GetLastPVPHitDealtTime() const	{ return m_LastPVPHitDealtTime; }

		void			SetLastAttacked(DWORD time);

		bool			SetSyncOwner(LPCHARACTER ch, bool bRemoveFromList = true);
		bool			IsSyncOwner(LPCHARACTER ch) const;

#if defined(__BL_MOVE_CHANNEL__)
		void			MoveChannel(const TRespondMoveChannel* p);
		bool			WarpSet(int32_t x, int32_t y, int32_t lRealMapIndex = 0, int32_t lCustomAddr = 0, WORD wCustomPort = 0);
#else
		bool			WarpSet(int32_t x, int32_t y, int32_t lRealMapIndex = 0);
#endif
		void			SetWarpLocation(int32_t lMapIndex, int32_t x, int32_t y);
		void			WarpEnd();
		const PIXEL_POSITION & GetWarpPosition() const { return m_posWarp; }
		bool			WarpToPID(DWORD dwPID);

		void			SaveExitLocation();
		void			ExitToSavedLocation();

		void			StartStaminaConsume();
		void			StopStaminaConsume();
		bool			IsStaminaConsume() const;
		bool			IsStaminaHalfConsume() const;

		void			ResetStopTime();
		DWORD			GetStopTime() const;

	protected:
		void			ClearSync();

		DWORD			m_fSyncTime;
		LPCHARACTER		m_pkChrSyncOwner;
		CHARACTER_LIST	m_kLst_pkChrSyncOwned;

		PIXEL_POSITION	m_posDest;
		PIXEL_POSITION	m_posStart;
		PIXEL_POSITION	m_posWarp;
		int32_t			m_lWarpMapIndex;

		PIXEL_POSITION	m_posExit;
		int32_t			m_lExitMapIndex;

		DWORD			m_dwMoveStartTime;
		DWORD			m_dwMoveDuration;

		DWORD			m_dwLastMoveTime;
		DWORD			m_dwLastAttackTime;
		uint32_t		m_LastPVPHitReceivedTime;
		uint32_t		m_LastPVPHitDealtTime;
		DWORD			m_dwWalkStartTime;
		DWORD			m_dwStopTime;

		bool			m_bWalking;
		bool			m_bNowWalking;
		bool			m_bStaminaConsume;
		// End

	public:
		void			SyncQuickslot(BYTE bType, BYTE bOldPos, BYTE bNewPos);
		bool			GetQuickslot(BYTE pos, TQuickslot ** ppSlot);
		bool			SetQuickslot(BYTE pos, TQuickslot & rSlot);
		bool			DelQuickslot(BYTE pos);
		bool			SwapQuickslot(BYTE a, BYTE b);
		void			ChainQuickslotItem(LPITEM pItem, BYTE bType, BYTE bOldPos);

		////////////////////////////////////////////////////////////////////////////////////////
		// Affect
	public:
		void			StartAffectEvent();
		void			ClearAffect(bool bSave = false
#ifdef ENABLE_NO_CLEAR_BUFF_WHEN_MONSTER_KILL
			, bool letBuffs = false
#endif
		);
		void			ComputeAffect(CAffect * pkAff, bool bAdd);
		bool			AddAffect(DWORD dwType, BYTE bApplyOn, int32_t lApplyValue, DWORD dwFlag, int32_t lDuration, int32_t lSPCost, bool bOverride, bool IsCube = false);
		void			RefreshAffect();
		bool			RemoveAffect(DWORD dwType);
		bool			IsAffectFlag(DWORD dwAff) const;

		bool			UpdateAffect();	// called from EVENT
		int				ProcessAffect();

		void			LoadAffect(DWORD dwCount, TPacketAffectElement * pElements);
		void			SaveAffect();

		bool			IsLoadedAffect() const	{ return m_bIsLoadedAffect; }

		bool			IsGoodAffect(BYTE bAffectType) const;

		void			RemoveGoodAffect();
		void			RemoveBadAffect();

		CAffect *		FindAffect(DWORD dwType, BYTE bApply=APPLY_NONE) const;
#ifdef ENABLE_SKILL_AFFECT_REMOVE
		CAffect *		FindAffectByFlag(DWORD dwFlag) const;
#endif
		const std::list<CAffect *> & GetAffectContainer() const	{ return m_list_pkAffect; }
		bool			RemoveAffect(CAffect * pkAff);

	protected:
		bool			m_bIsLoadedAffect;
		TAffectFlag		m_afAffectFlag;
		std::list<CAffect *>	m_list_pkAffect;

	public:
		// PARTY_JOIN_BUG_FIX
		void			SetParty(LPPARTY pkParty);
		LPPARTY			GetParty() const	{ return m_pkParty; }

		bool			RequestToParty(LPCHARACTER leader);
		void			DenyToParty(LPCHARACTER member);
		void			AcceptToParty(LPCHARACTER member);

		void			PartyInvite(LPCHARACTER pchInvitee);

		void			PartyInviteAccept(LPCHARACTER pchInvitee);

		void			PartyInviteDeny(DWORD dwPID);

		bool			BuildUpdatePartyPacket(TPacketGCPartyUpdate & out);
		int				GetLeadershipSkillLevel() const;

		bool			CanSummon(int iLeaderShip);

		void			SetPartyRequestEvent(LPEVENT pkEvent) { m_pkPartyRequestEvent = pkEvent; }

	protected:

		void			PartyJoin(LPCHARACTER pkLeader);

		enum PartyJoinErrCode {
			PERR_NONE		= 0,
			PERR_SERVER,
			PERR_DUNGEON,
			PERR_OBSERVER,
			PERR_LVBOUNDARY,
			PERR_LOWLEVEL,
			PERR_HILEVEL,
			PERR_ALREADYJOIN,
			PERR_PARTYISFULL,
			PERR_SEPARATOR,			///< Error type separator.
			PERR_DIFFEMPIRE,
			PERR_MAX
		};

		static PartyJoinErrCode	IsPartyJoinableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest);

		static PartyJoinErrCode	IsPartyJoinableMutableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest);

		LPPARTY			m_pkParty;
		DWORD			m_dwLastDeadTime;
		LPEVENT			m_pkPartyRequestEvent;

		typedef std::map< DWORD, LPEVENT >	EventMap;
		EventMap		m_PartyInviteEventMap;

		// END_OF_PARTY_JOIN_BUG_FIX

		////////////////////////////////////////////////////////////////////////////////////////
		// Dungeon
	public:
		void			SetDungeon(LPDUNGEON pkDungeon);
		LPDUNGEON		GetDungeon() const	{ return m_pkDungeon; }
		LPDUNGEON		GetDungeonForce() const;
	protected:
		LPDUNGEON	m_pkDungeon;
		int			m_iEventAttr;

		////////////////////////////////////////////////////////////////////////////////////////
		// Guild
	public:
		void			SetGuild(CGuild * pGuild);
		CGuild*			GetGuild() const	{ return m_pGuild; }

		void			SetWarMap(CWarMap* pWarMap);
		CWarMap*		GetWarMap() const	{ return m_pWarMap; }

	protected:
		CGuild *		m_pGuild;
		DWORD			m_dwUnderGuildWarInfoMessageTime;
		CWarMap *		m_pWarMap;

		////////////////////////////////////////////////////////////////////////////////////////
		// Item related
	public:
		bool			CanHandleItem(bool bSkipRefineCheck = false, bool bSkipObserver = false);

		bool			IsItemLoaded() const	{ return m_bItemLoaded; }
		void			SetItemLoaded()	{ m_bItemLoaded = true; }

		void			ClearItem();
#ifdef ENABLE_SORT_INVENTORY_ITEMS
		void			SortSpecialInventoryItems(BYTE type);
#endif
#ifdef ENABLE_HIGHLIGHT_NEW_ITEM
		void			SetItem(TItemPos Cell, LPITEM item, bool bWereMine = false);
#else
		void			SetItem(TItemPos Cell, LPITEM item);
#endif
		LPITEM			GetItem(TItemPos Cell) const;
		LPITEM			GetInventoryItem(WORD wCell) const;
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	private:
		bool			m_wpadanie_val;
	public:
		bool			IsWpadanie() { return m_wpadanie_val; }
		void			SetToggleWpadanie(bool status) { m_wpadanie_val = status; }

		LPITEM			GetSkillBookInventoryItem(WORD wCell) const;
		LPITEM			GetUpgradeItemsInventoryItem(WORD wCell) const;
		LPITEM			GetStoneInventoryItem(WORD wCell) const;
		LPITEM			GetBoxInventoryItem(WORD wCell) const;
		LPITEM			GetEfsunInventoryItem(WORD wCell) const;
		LPITEM			GetCicekInventoryItem(WORD wCell) const;
#endif
		bool			IsEmptyItemGrid(TItemPos Cell, BYTE size, int iExceptionCell = -1) const;

		void			SetWear(BYTE bCell, LPITEM item);
		LPITEM			GetWear(BYTE bCell) const;

		// MYSHOP_PRICE_LIST
		void			UseSilkBotary(void);

		void			UseSilkBotaryReal(const TPacketMyshopPricelistHeader* p);
		// END_OF_MYSHOP_PRICE_LIST

		bool			UseItemEx(LPITEM item, TItemPos DestCell);
		bool			UseItem(TItemPos Cell, TItemPos DestCell = NPOS);

		// ADD_REFINE_BUILDING
		bool			IsRefineThroughGuild() const;
		CGuild *		GetRefineGuild() const;
		int				ComputeRefineFee(int iCost, int iMultiply = 5) const;
		void			PayRefineFee(int iTotalMoney);
		void			SetRefineNPC(LPCHARACTER ch);
		// END_OF_ADD_REFINE_BUILDING

		bool			RefineItem(LPITEM pkItem, LPITEM pkTarget);

		// bool			DropItem(TItemPos Cell,  BYTE bCount=0);
#ifdef __EXTENDED_ITEM_COUNT__
		bool DropItem(TItemPos Cell, uint16_t bCount = 0);
#else
		bool DropItem(TItemPos Cell, BYTE bCount = 0);
#endif
		bool			GiveRecallItem(LPITEM item);
		void			ProcessRecallItem(LPITEM item);

		//	void			PotionPacket(int iPotionType);
		void			EffectPacket(int enumEffectType);
		void SpecificEffectPacket(const std::string& sFileName);

		// ADD_MONSTER_REFINE
		bool			DoRefine(LPITEM item, bool bMoneyOnly = false);
		bool			CanRefineItemByLevel(LPITEM item, int refineType);
		// END_OF_ADD_MONSTER_REFINE

		bool			DoRefineWithScroll(LPITEM item);
		bool			RefineInformation(BYTE bCell, BYTE bType, int iAdditionalCell = -1);

		void			SetRefineMode(int iAdditionalCell = -1);
		void			ClearRefineMode();

		bool			GiveItem(LPCHARACTER victim, TItemPos Cell);
		bool			CanReceiveItem(LPCHARACTER from, LPITEM item) const;
		void			ReceiveItem(LPCHARACTER from, LPITEM item);

        bool            GiveItemFromSpecialItemGroup(DWORD dwGroupNum, std::vector <DWORD> &dwItemVnums,
                            std::vector <DWORD> &dwItemCounts, std::vector <LPITEM> &item_gets, int &count, bool sendMessage = false);


#ifdef __EXTENDED_ITEM_COUNT__
		bool			MoveItem(TItemPos pos, TItemPos change_pos, uint16_t num);
#else
		bool			MoveItem(TItemPos pos, TItemPos change_pos, BYTE num);
#endif
		bool			PickupItem(DWORD vid, bool isPremiumPickUp = false);
		bool			EquipItem(LPITEM item, int iCandidateCell = -1);
		bool			UnequipItem(LPITEM item);
		bool			FastStack(TItemPos cell);

		bool			CanEquipNow(const LPITEM item, const TItemPos& srcCell = NPOS, const TItemPos& destCell = NPOS);

		bool			CanUnequipNow(const LPITEM item, const TItemPos& srcCell = NPOS, const TItemPos& destCell = NPOS);

		bool			SwapItem(BYTE bCell, BYTE bDestCell);
#ifdef __EXTENDED_ITEM_COUNT__
		LPITEM			AutoGiveItem(DWORD dwItemVnum, uint16_t bCount = 1, int iRarePct = -1, bool bMsg = true, bool bDrop = true);
		void			AutoGiveItem(LPITEM item, bool longOwnerShip = false);
#else
		LPITEM			AutoGiveItem(DWORD dwItemVnum, BYTE bCount = 1, int iRarePct = -1, bool bMsg = true);
		void			AutoGiveItem(LPITEM item, bool longOwnerShip = false);
#endif

#ifdef ENABLE_SYSTEM_RUNE
		void			CheckRuneExtraBonus(LPITEM item, BYTE type);
#endif
#ifdef ENABLE_EXTRABONUS_SYSTEM
#ifdef ENABLE_GLOVE_SYSTEM
		void			CheckGloveExtraBonus(LPITEM item, BYTE type);
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
		void			CheckPetExtraBonus(LPITEM, BYTE type);
#endif
#endif

		int				GetEmptyInventory(BYTE size) const;
		int				GetEmptyDragonSoulInventory(BYTE size) const;

        void            SetNextTransferInventoryPulse(int pulse) { m_TransferInventoryPulse = pulse; }
        int             GetTransferInventoryPulse() { return m_TransferInventoryPulse; }

		int				GetEmptyInventoryFromIndex(WORD index, BYTE itemSize) const; //SPLIT ITEMS
		int				GetEmptyUpgradeItemsInventoryFromIndex(WORD index, BYTE itemSize) const; //SPLIT ITEMS
		int				GetEmptySkillBookInventoryFromIndex(WORD index, BYTE itemSize) const; //SPLIT ITEMS
		int				GetEmptyStoneInventoryFromIndex(WORD index, BYTE itemSize) const; //SPLIT ITEMS
		int				GetEmptyBoxInventoryFromIndex(WORD index, BYTE itemSize) const; //SPLIT ITEMS
		int				GetEmptyEfsunInventoryFromIndex(WORD index, BYTE itemSize) const; //SPLIT ITEMS
		int				GetEmptyCicekInventoryFromIndex(WORD index, BYTE itemSize) const; //SPLIT ITEMS

		int				GetEmptyDragonSoulInventory(LPITEM pItem) const;
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
		int				GetEmptySkillBookInventory(BYTE size) const;
		int				GetEmptyUpgradeItemsInventory(BYTE size) const;
		int				GetEmptyStoneInventory(BYTE size) const;
		int				GetEmptyBoxInventory(BYTE size) const;
		int				GetEmptyEfsunInventory(BYTE size) const;
		int				GetEmptyCicekInventory(BYTE size) const;
#endif
		void			CopyDragonSoulItemGrid(std::vector<WORD>& vDragonSoulItemGrid) const;

		int				CountEmptyInventory() const;

		int				CountSpecifyItem(DWORD vnum) const;
		void			RemoveSpecifyItem(DWORD vnum, DWORD count = 1);
		LPITEM			FindSpecifyItem(DWORD vnum) const;
		LPITEM			FindItemByID(DWORD id) const;
		bool			IsIllegalMapForMount() const;
		
		void			SendTitleAchievementProgress();

		int				CountSpecifyTypeItem(BYTE type) const;
		void			RemoveSpecifyTypeItem(BYTE type, DWORD count = 1);

		bool			IsEquipUniqueItem(DWORD dwItemVnum) const;

		// CHECK_UNIQUE_GROUP
		bool			IsEquipUniqueGroup(DWORD dwGroupVnum) const;
		// END_OF_CHECK_UNIQUE_GROUP

		void			SendEquipment(LPCHARACTER ch);
		// End of Item
#ifdef ENABLE_OFFLINE_SHOP
	public:
		bool CanTakeInventoryItem(LPITEM item, TItemPos* cell);
		offlineshop::CShop* GetOfflineShop() { return m_pkOfflineShop; }
		void						SetOfflineShop(offlineshop::CShop* pkShop) { m_pkOfflineShop = pkShop; }
	
		offlineshop::CShop* GetOfflineShopGuest() const { return m_pkOfflineShopGuest; }
		void						SetOfflineShopGuest(offlineshop::CShop* pkShop) { m_pkOfflineShopGuest = pkShop; }
	
		offlineshop::CShopSafebox*	GetShopSafebox() { return m_pkShopSafebox; }
		void						SetShopSafebox(offlineshop::CShopSafebox* pk);
	
		int GetOfflineShopUseTime() const { return m_iOfflineShopUseTime; }
		void SetOfflineShopUseTime() { m_iOfflineShopUseTime = thecore_pulse(); }
	
	private:
		offlineshop::CShop* m_pkOfflineShop;
		offlineshop::CShop* m_pkOfflineShopGuest;
		offlineshop::CShopSafebox* m_pkShopSafebox;
		int m_iOfflineShopUseTime = 0;
#endif

	protected:
#ifdef ENABLE_LONG_LONG
		void			SendMyShopPriceListCmd(DWORD dwItemVnum, int64_t dwItemPrice);
#else
		void			SendMyShopPriceListCmd(DWORD dwItemVnum, DWORD dwItemPrice);
#endif

		bool			m_bNoOpenedShop;

		bool			m_bItemLoaded;
		int				m_iRefineAdditionalCell;
		bool			m_bUnderRefine;
		DWORD			m_dwRefineNPCVID;

	public:
		////////////////////////////////////////////////////////////////////////////////////////
		// Money related
#ifdef ENABLE_LONG_LONG
		int64_t				GetGold() const { return m_points.gold; }
		void			SetGold(int64_t gold) { m_points.gold = gold; }
#else
		INT				GetGold() const		{ return m_points.gold;	}
		void			SetGold(INT gold)	{ m_points.gold = gold;	}
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		void			SendGold();
#endif
		bool			DropGold(INT gold);
#ifdef ENABLE_LONG_LONG
		int64_t				GetAllowedGold() const;
		void			GiveGold(int64_t iAmount);	// ��Ƽ�� ������ ��Ƽ �й�, �α� ���� ó��
#else
		INT				GetAllowedGold() const;
		void			GiveGold(INT iAmount);	// ��Ƽ�� ������ ��Ƽ �й�, �α� ���� ó��
#endif
#ifdef ENABLE_CHEQUE_SYSTEM
		int64_t				GetCheque() const		{ return m_points.cheque;	}
		void			SetCheque(int64_t cheque)	{ m_points.cheque = cheque;	}
		bool 			DropCheque(int64_t cheque);
		void			GiveCheque(int64_t iAmount);
#endif
#ifdef ENABLE_PUNKTY_OSIAGNIEC
		INT				GetPktOsiag() const			{ return m_points.pkt_osiag; }
		void			SetPktOsiag(INT pkt_osiag)			{ m_points.pkt_osiag = pkt_osiag; }
		void			GivePktOsiag(INT iAmount);
#endif
		// End of Money
#ifdef WEEKLY_RANK_BYLUZER
		INT				GetWeekly(BYTE id) const;
		void			SetWeekly(BYTE id, int value);
		void			CheckWeekly();
#endif
		////////////////////////////////////////////////////////////////////////////////////////
		// Shop related
	public:
		void			SetShop(LPSHOP pkShop);
		LPSHOP			GetShop() const { return m_pkShop; }
		void			ShopPacket(BYTE bSubHeader);

		void			SetShopOwner(LPCHARACTER ch) { m_pkChrShopOwner = ch; }
		LPCHARACTER		GetShopOwner() const { return m_pkChrShopOwner;}

		// void			OpenMyShop(const char * c_pszSign, TShopItemTable * pTable, BYTE bItemCount);
#ifdef __EXTENDED_ITEM_COUNT__
		void			OpenMyShop(const char* c_pszSign, TShopItemTable* pTable, uint16_t bItemCount);
#else
		void			OpenMyShop(const char* c_pszSign, TShopItemTable* pTable, BYTE bItemCount);
#endif
		LPSHOP			GetMyShop() const { return m_pkMyShop; }
		void			CloseMyShop();

	protected:

		LPSHOP			m_pkShop;
		LPSHOP			m_pkMyShop;
		std::string		m_stShopSign;
		LPCHARACTER		m_pkChrShopOwner;
		// End of shop

#ifdef ENABLE_PROMO_CODE_SYSTEM
	public:
		bool			check_PromoCodeSystem(const char * promo_code);
		bool			restriction_PromoCodeSystem(const char * promo_code);
		
		void			reward_PromoCodeSystem(const char * promo_code);
		void			lock_PromoCodeSystem(const char * promo_code);
		void			reduce_PromoCodeSystem(const char * promo_code, int iCount);
		
		void			give_PromoCodeSystem(const std::string& sType, int iVnum, int iCount);
#endif

		int				m_TransferInventoryPulse;

#ifdef ENABLE_SORT_INVENTORY_ITEMS
		int				m_sortInventoryPulse;
		int				m_sortSpecialInventoryPulse;
#endif
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
		int				m_splitInventoryPulse;
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	public:
		// Give me point
		void			SetOfflineShop(LPOFFLINESHOP pkOfflineShop);
		LPOFFLINESHOP	GetOfflineShop() const { return m_pkOfflineShop; }

		// Return owner
		void			SetOfflineShopOwner(LPCHARACTER ch) { m_pkChrOfflineShopOwner = ch; }
		LPCHARACTER		GetOfflineShopOwner() const { return m_pkChrOfflineShopOwner; }
		// End of return owner
                
		// Give me real owner
		void			SetOfflineShopRealOwner(DWORD dwVID) { m_pointsInstant.real_owner = dwVID; }
		DWORD			GetOfflineShopRealOwner() const { return m_pointsInstant.real_owner; }
		// End of give me real owner

		// Is Offline Shop NPC?
		void			SetOfflineShopNPC(bool flag) { m_pointsInstant.isOfflineShop = flag; }
		bool			IsOfflineShopNPC() const { return m_pointsInstant.isOfflineShop; }
		// End Of Is Offline Shop NPC?

		// Open My Offline Shop
		void			OpenMyOfflineShop(const char * c_pszSign, TShopItemTable * pTable, int bItemCount);

		// Offline Shop Timer
		void			SetOfflineShopTimer(int iTime) { m_pointsInstant.leftTime = iTime; }
		int				GetOfflineShopTimer() { return m_pointsInstant.leftTime; }
		void			SetOfflineShopSaveTime(BYTE bSaveTime) { m_pointsInstant.bSaveTime = bSaveTime; }
		BYTE			GetOfflineShopSaveTime() { return m_pointsInstant.bSaveTime; }
		// End Of Offline Shop Time
                
                // Set Title 
                void                    SetOfflineShopSign(const char * c_pszSign);
                // End Of Set Title
                
                // Destroy Offline Shop
                void                    DestroyOfflineShop();
                // End Of Destroy Offline Shop

		// Offline Shop Save Time
		void			StartOfflineShopUpdateEvent();
		void			StopOfflineShopUpdateEvent();
		// End Of Offline Shop Save Time

		// Configure Offline Shop Channel
		void			SetOfflineShopChannel(BYTE bChannel) { m_pointsInstant.bChannel = bChannel; }
		BYTE			GetOfflineShopChannel() { return m_pointsInstant.bChannel; }
		// End Of Configure Offline Shop Channel

		// Configure Offline Shop NPC VID
		void			SetOfflineShopVID(DWORD dwVID) { m_pointsInstant.dwVID = dwVID; }
		DWORD			GetOfflineShopVID() { return m_pointsInstant.dwVID; }
		// End Of Configure Offline Shop NPC VID

		// DWORD	GetLastTimePanelOpened() { return m_dwLastTimePanelOpen; }
		// void	SetLastTimePanelOpened(DWORD time) { m_dwLastTimePanelOpen = time; }		
		
	protected:
		LPOFFLINESHOP	m_pkOfflineShop;
		LPCHARACTER     m_pkChrOfflineShopOwner;
        std::string     m_stOfflineShopSign;
		// DWORD	m_dwLastTimePanelOpen;
#endif
		////////////////////////////////////////////////////////////////////////////////////////
		// Exchange related
	public:
		bool			ExchangeStart(LPCHARACTER victim);
		void			SetExchange(CExchange * pkExchange);
		CExchange *		GetExchange() const	{ return m_pkExchange;	}

#ifdef WEEKLY_RANK_BYLUZER
		void 			SelectWeeklyRankPage(BYTE page);
#endif
#ifdef TITLE_SYSTEM_BYLUZER
		void 			TitleSendGeneralInfo();
		void			ActiveTitle(BYTE id);
		void			ActiveTitleByClient(BYTE id);
		void			DetachTitle(BYTE id);
#endif
	protected:
		CExchange *		m_pkExchange;
		// End of Exchange

		////////////////////////////////////////////////////////////////////////////////////////
		// Battle
	public:
		struct TBattleInfo
		{
			int64_t iTotalDamage;
			int iAggro;

			TBattleInfo(int64_t iTot, int iAggr)
				: iTotalDamage(iTot), iAggro(iAggr)
				{}
		};
		typedef std::map<VID, TBattleInfo>	TDamageMap;

#ifdef TAKE_LEGEND_DAMAGE_BOARD_SYSTEM
		struct TBattleInfoLegend
		{
			DWORD		dwVID;
			char		szName[CHARACTER_NAME_MAX_LEN + 1];
			int			iLevel;
			int			iRace;
			int			iEmpire;
			int64_t	iTotalDamage;
			int			iAggro;

			TBattleInfoLegend(DWORD dVID, const char* sName, int iLv, int iRc, int iEmp, int64_t iTot, int iAggr)
				: dwVID(dVID), iLevel(iLv), iRace(iRc), iEmpire(iEmp), iTotalDamage(iTot), iAggro(iAggr)
			{
				std::strncpy(szName, sName, CHARACTER_NAME_MAX_LEN);
				szName[CHARACTER_NAME_MAX_LEN] = '\0';
			}
		};
		typedef std::map<std::string, TBattleInfoLegend>	TDamageMapLegend;
#endif

		typedef struct SAttackLog
		{
			DWORD	dwVID;
			DWORD	dwTime;
		} AttackLog;

		bool				Damage(LPCHARACTER pAttacker, int64_t dam, EDamageType type = DAMAGE_TYPE_NORMAL);
		bool				__Profile__Damage(LPCHARACTER pAttacker, int64_t dam, EDamageType type = DAMAGE_TYPE_NORMAL);
		void				DeathPenalty(BYTE bExpLossPercent);
		void				ReviveInvisible(int iDur);

		bool				Attack(LPCHARACTER pkVictim, BYTE bType = 0);
		bool				IsAlive() const		{ return m_pointsInstant.position == POS_DEAD ? false : true; }
		bool				CanFight() const;
#ifdef __ENABLE_STEJNE_DMG__
		bool				IsStejneDMG() const;
#endif

		bool				CanBeginFight() const;
		void				BeginFight(LPCHARACTER pkVictim);

		bool				CounterAttack(LPCHARACTER pkChr);

		bool				IsStun() const;
		void				Stun(bool bJustDie = false);
		bool				IsDead() const;
		void				Dead(LPCHARACTER pkKiller = NULL, bool bImmediateDead=true); // @fixme188 instant death false to true

		void				Reward(bool bItemDrop);
		void				RewardGold(LPCHARACTER pkAttacker);
#ifdef ENABLE_KILL_NOTICE
		void 				CreateKillNotice(const CMob* pkMob, LPCHARACTER pkAttacker);
#endif
		bool				Shoot(BYTE bType);
		void				FlyTarget(DWORD dwTargetVID, int32_t x, int32_t y, BYTE bHeader);

		void				ForgetMyAttacker(
#ifdef __RESET_PLAYER_DAMAGE__
			bool fullClear_ = false
#endif
		);
#ifdef ENABLE_VS_PELERYNKA_VIP
		void				AggregateMonster(BYTE type);
#else
		void				AggregateMonster();
#endif
		void				AttractRanger();
		void				PullMonster();

		int					GetArrowAndBow(LPITEM * ppkBow, LPITEM * ppkArrow, int iArrowCount = 1);
		void				UseArrow(LPITEM pkArrow, DWORD dwArrowCount);

		DWORD				GetLastSuccessShootTime() const;
		void				SetLastSuccessShootTime(DWORD value);

		void				AttackedByPoison(LPCHARACTER pkAttacker);
		void				RemovePoison();
#ifdef ENABLE_WOLFMAN_CHARACTER
		void				AttackedByBleeding(LPCHARACTER pkAttacker);
		void				RemoveBleeding();
#endif
		void				AttackedByFire(LPCHARACTER pkAttacker, int amount, int count);
		void				RemoveFire();

		void				UpdateAlignment(int64_t iAmount);
		int64_t			GetAlignment() const;

		int64_t			GetRealAlignment() const;
		void				ShowAlignment(bool bShow);

		void				SetKillerMode(bool bOn);
		bool				IsKillerMode() const;
		void				UpdateKillerMode();

		BYTE				GetPKMode() const;
		void				SetPKMode(BYTE bPKMode);

		void				UpdateAggrPoint(LPCHARACTER ch, EDamageType type, int64_t dam);

		// HACK

	public:
		void SetComboSequence(BYTE seq);
		BYTE GetComboSequence() const;

		void SetLastComboTime(DWORD time);
		DWORD GetLastComboTime() const;

		int GetValidComboInterval() const;
		void SetValidComboInterval(int interval);

		BYTE GetComboIndex() const;

		void IncreaseComboHackCount(int k = 1);
		void ResetComboHackCount();
		void SkipComboAttackByTime(int interval);
		DWORD GetSkipComboAttackByTime() const;

	protected:
		BYTE m_bComboSequence;
		DWORD m_dwLastComboTime;
		int m_iValidComboInterval;
		BYTE m_bComboIndex;
		int m_iComboHackCount;
		DWORD m_dwSkipComboAttackByTime;

	protected:
		void				UpdateAggrPointEx(LPCHARACTER ch, EDamageType type, int64_t dam, TBattleInfo & info);
		void				ChangeVictimByAggro(int iNewAggro, LPCHARACTER pNewVictim);

		DWORD				m_dwFlyTargetID;
		std::vector<DWORD>	m_vec_dwFlyTargets;
		TDamageMap			m_map_kDamage;
#ifdef TAKE_LEGEND_DAMAGE_BOARD_SYSTEM
		TDamageMapLegend	m_map_kDamageLegend;
#endif
		DWORD				m_dwKillerPID;

		int					m_iAlignment;		// Lawful/Chaotic value -200000 ~ 200000
		int					m_iRealAlignment;
		int					m_iKillerModePulse;
		BYTE				m_bPKMode;

		// Aggro
		DWORD				m_dwLastVictimSetTime;
		int					m_iMaxAggro;
		// End of Battle

		// Stone
	public:
		void				SetStone(LPCHARACTER pkChrStone);
		void				ClearStone();
		void				DetermineDropMetinStone();
		DWORD				GetDropMetinStoneVnum() const { return m_dwDropMetinStone; }
		BYTE				GetDropMetinStonePct() const { return m_bDropMetinStonePct; }

	protected:
		LPCHARACTER			m_pkChrStone;
		CHARACTER_SET		m_set_pkChrSpawnedBy;
		DWORD				m_dwDropMetinStone;
		BYTE				m_bDropMetinStonePct;
		// End of Stone

	public:
		enum
		{
			SKILL_UP_BY_POINT,
			SKILL_UP_BY_BOOK,
			SKILL_UP_BY_TRAIN,

			// ADD_GRANDMASTER_SKILL
			SKILL_UP_BY_QUEST,
			// END_OF_ADD_GRANDMASTER_SKILL
		};

		void				SkillLevelPacket();
		void				SkillLevelUp(DWORD dwVnum, BYTE bMethod = SKILL_UP_BY_POINT);
		bool				SkillLevelDown(DWORD dwVnum);
		// ADD_GRANDMASTER_SKILL
		bool				UseSkill(DWORD dwVnum, LPCHARACTER pkVictim, bool bUseGrandMaster = true);
		void				ResetSkill();
		void				SetSkillLevel(DWORD dwVnum, BYTE bLev);
		int					GetUsedSkillMasterType(DWORD dwVnum);

		bool				IsLearnableSkill(DWORD dwSkillVnum) const;
		// END_OF_ADD_GRANDMASTER_SKILL

		bool				CheckSkillHitCount(const BYTE SkillID, const VID dwTargetVID);
		bool				CanUseSkill(DWORD dwSkillVnum) const;
		bool				IsUsableSkillMotion(DWORD dwMotionIndex) const;
		int					GetSkillLevel(DWORD dwVnum) const;
		int					GetSkillMasterType(DWORD dwVnum) const;
		int					GetSkillPower(DWORD dwVnum, BYTE bLevel = 0) const;

		time_t				GetSkillNextReadTime(DWORD dwVnum) const;
		void				SetSkillNextReadTime(DWORD dwVnum, time_t time);
		void				SkillLearnWaitMoreTimeMessage(DWORD dwVnum);

		void				ComputePassiveSkill(DWORD dwVnum);
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
		int					ComputeSkill(DWORD dwVnum, LPCHARACTER pkVictim, BYTE bSkillLevel = 0, BOOL isBuffNPC = false);
#else
		int					ComputeSkill(DWORD dwVnum, LPCHARACTER pkVictim, BYTE bSkillLevel = 0);
#endif
#ifdef ENABLE_SKILL_FLAG_PARTY
		int					ComputeSkillParty(DWORD dwVnum, LPCHARACTER pkVictim, BYTE bSkillLevel = 0);
#endif
		int					ComputeSkillAtPosition(DWORD dwVnum, const PIXEL_POSITION& posTarget, BYTE bSkillLevel = 0);
		void				ComputeSkillPoints();

		void				SetSkillGroup(BYTE bSkillGroup);
		BYTE				GetSkillGroup() const		{ return m_points.skill_group; }

		int					ComputeCooltime(int time);

		void				GiveRandomSkillBook();

		void				DisableCooltime();
		bool				LearnSkillByBook(DWORD dwSkillVnum, BYTE bProb = 0);
		bool				LearnSkillByNewBook(DWORD dwSkillVnum, BYTE bProb = 0);
		
		bool				LearnGrandMasterSkill(DWORD dwSkillVnum);

		#ifdef ENABLE_SKILL_COOLDOWN_CHECK
		bool				SkillIsOnCooldown(int vnum) { return m_SkillUseInfo[vnum].IsOnCooldown(vnum); }
		#endif

		DWORD				GetLastSkillTime()	{ return m_dwLastSkillTime; }
	private:
		bool				m_bDisableCooltime;
		DWORD				m_dwLastSkillTime;
		// End of Skill

		// MOB_SKILL
	public:
		bool				HasMobSkill() const;
		size_t				CountMobSkill() const;
		const TMobSkillInfo * GetMobSkill(unsigned int idx) const;
		bool				CanUseMobSkill(unsigned int idx) const;
		bool				UseMobSkill(unsigned int idx);
		void				ResetMobSkillCooltime();
	protected:
		DWORD				m_adwMobSkillCooltime[MOB_SKILL_MAX_NUM];
		// END_OF_MOB_SKILL

		// for SKILL_MUYEONG
	public:
		void				StartMuyeongEvent();
		void				StopMuyeongEvent();

	private:
		LPEVENT				m_pkMuyeongEvent;

		// for SKILL_CHAIN lighting
	public:
		int					GetChainLightningIndex() const { return m_iChainLightingIndex; }
		void				IncChainLightningIndex() { ++m_iChainLightingIndex; }
		void				AddChainLightningExcept(LPCHARACTER ch) { m_setExceptChainLighting.emplace(ch); }
		void				ResetChainLightningIndex() { m_iChainLightingIndex = 0; m_setExceptChainLighting.clear(); }
		int					GetChainLightningMaxCount() const;
		const CHARACTER_SET& GetChainLightingExcept() const { return m_setExceptChainLighting; }

	private:
		int					m_iChainLightingIndex;
		CHARACTER_SET m_setExceptChainLighting;

		// for SKILL_EUNHYUNG
	public:
		void				SetAffectedEunhyung();
		void				ClearAffectedEunhyung() { m_dwAffectedEunhyungLevel = 0; }
		bool				GetAffectedEunhyung() const { return m_dwAffectedEunhyungLevel; }

	private:
		DWORD				m_dwAffectedEunhyungLevel;

		// Skill levels

	protected:
		TPlayerSkill*					m_pSkillLevels;
		std::unordered_map<BYTE, int>		m_SkillDamageBonus;
		std::map<int, TSkillUseInfo>	m_SkillUseInfo;

#ifdef TITLE_SYSTEM_BYLUZER
	protected:
		TPlayerTitle*					m_pTitle;
	public:
		bool			IsTitleActive(int id);
#endif

		////////////////////////////////////////////////////////////////////////////////////////
		// AI related
	public:
		void			AssignTriggers(const TMobTable * table);
		LPCHARACTER		GetVictim() const;
		void			SetVictim(LPCHARACTER pkVictim);
		LPCHARACTER		GetNearestVictim(LPCHARACTER pkChr);
		LPCHARACTER		GetProtege() const;

		bool			Follow(LPCHARACTER pkChr, float fMinimumDistance = 150.0f);
		bool			Return();
		bool			IsGuardNPC() const;
		bool			IsChangeAttackPosition(LPCHARACTER target) const;
		void			ResetChangeAttackPositionTime() { m_dwLastChangeAttackPositionTime = get_dword_time() - AI_CHANGE_ATTACK_POISITION_TIME_NEAR;}
		void			SetChangeAttackPositionTime() { m_dwLastChangeAttackPositionTime = get_dword_time();}

		bool			OnIdle();

		void			OnAttack(LPCHARACTER pkChrAttacker);
		void			OnClick(LPCHARACTER pkChrCauser);
		bool IsSpecialNonPvpMob() const;

		VID				m_kVIDVictim;

	protected:
		DWORD			m_dwLastChangeAttackPositionTime;
		CTrigger		m_triggerOnClick;
		// End of AI

		////////////////////////////////////////////////////////////////////////////////////////
		// Target
	protected:
		LPCHARACTER				m_pkChrTarget;
		CHARACTER_SET	m_set_pkChrTargetedBy;

	public:
		void				SetTarget(LPCHARACTER pkChrTarget);
		void				BroadcastTargetPacket();
		void				ClearTarget();
		void				CheckTarget();
		LPCHARACTER			GetTarget() const { return m_pkChrTarget; }

		////////////////////////////////////////////////////////////////////////////////////////
		// Safebox
	public:
		int					GetSafeboxSize() const;
		void				QuerySafeboxSize();
		void				SetSafeboxSize(int size);

		CSafebox *			GetSafebox() const;
		void				LoadSafebox(int iSize, DWORD dwGold, int iItemCount, TPlayerItem * pItems);
		void				ChangeSafeboxSize(BYTE bSize);
		void				CloseSafebox();

		void				ReqSafeboxLoad(const char* pszPassword);

		void				CancelSafeboxLoad( void ) { m_bOpeningSafebox = false; }

		void				SetMallLoadTime(int t) { m_iMallLoadTime = t; }
		int					GetMallLoadTime() const { return m_iMallLoadTime; }

		CSafebox *			GetMall() const;
		void				LoadMall(int iItemCount, TPlayerItem * pItems);
		void				CloseMall();

		void				SetSafeboxOpenPosition();
		float				GetDistanceFromSafeboxOpen() const;

	protected:
		CSafebox *			m_pkSafebox;
		int					m_iSafeboxSize;
		int					m_iSafeboxLoadTime;
		bool				m_bOpeningSafebox;

		DWORD				dwLastSuccessShootTime;

		CSafebox *			m_pkMall;
		int					m_iMallLoadTime;
#ifdef WEEKLY_RANK_BYLUZER
		int					m_CooldownWeekly;
		BYTE				m_WeeklyLoadCount;
#endif
		PIXEL_POSITION		m_posSafeboxOpen;

#ifdef TITLE_SYSTEM_BYLUZER
		int					m_CooldownTitle;
#endif

		////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////////////
		// Mounting
	public:
		void				MountVnum(DWORD vnum);
		DWORD				GetMountVnum() const { return m_dwMountVnum; }
		DWORD				GetLastMountTime() const { return m_dwMountTime; }

		bool				CanUseHorseSkill();

		// Horse
		virtual	void		SetHorseLevel(int iLevel);

		virtual	bool		StartRiding();
		virtual	bool		StopRiding();

		virtual	DWORD		GetMyHorseVnum() const;

		virtual	void		HorseDie();
		virtual bool		ReviveHorse();

		virtual void		SendHorseInfo();
		virtual	void		ClearHorseInfo();

		void				HorseSummon(bool bSummon, bool bFromFar = false, DWORD dwVnum = 0, const char* name = 0);

		LPCHARACTER			GetHorse() const			{ return m_chHorse; }
		LPCHARACTER			GetRider() const; // rider on horse
		void				SetRider(LPCHARACTER ch);

		bool				IsRiding() const;

#ifdef ENABLE_MOUNT_SYSTEM
		short GetMountLevel() const { return m_points.iMountLevel; }
		int GetMountExp() const { return m_points.iMountExp; }
		int GetMountNextExp() const { return mount_exp_table[GetMountLevel()]; }
		void SetMountLevel(short sLevel) { m_points.iMountLevel = sLevel; }
		void UpdateMountExp(int iAmount) { SetMountExp(GetMountExp() + iAmount); }
		void SetMountExp(int iAmount);
		void SendMountInfoPacket();
#endif

#ifdef __PET_SYSTEM__
	public:
		CPetSystem*			GetPetSystem()				{ return m_petSystem; }

	protected:
		CPetSystem*			m_petSystem;

	public:
#endif

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	public:
		CMountSystem*		GetMountSystem() { return m_mountSystem; }
		
		// DWORD				MountGetNextExp() const;
		// DWORD				MountGetNextExpByLevel(int level) const;
		// void				SendMountLevelUpEffect(int vid, int type, int value, int amount);

		void 				MountSummon(LPITEM mountItem);
		void 				MountUnsummon(LPITEM mountItem);
		void 				MountUnmount(LPITEM mountItem);
		// void 				MountSetExp(int count);
		// int					GetMountLevel();
		void 				CheckMount();
		bool 				IsRidingMount();
	protected:
		CMountSystem*		m_mountSystem;
#endif

	protected:
		LPCHARACTER			m_chHorse;
		LPCHARACTER			m_chRider;
		
#ifdef ENABLE_MOUNT_SYSTEM
		DWORD m_dwLastMountUpdate;
#endif

		DWORD				m_dwMountVnum;
		DWORD				m_dwMountTime;
		BYTE				m_bSendHorseLevel;
		BYTE				m_bSendHorseHealthGrade;
		BYTE				m_bSendHorseStaminaGrade;

		////////////////////////////////////////////////////////////////////////////////////////
		// Detailed Log
	public:
		void				DetailLog() { m_bDetailLog = !m_bDetailLog; }
		void				ToggleMonsterLog();
		void				MonsterLog(const char* format, ...);
	private:
		bool				m_bDetailLog;
		bool				m_bMonsterLog;

		////////////////////////////////////////////////////////////////////////////////////////
		// Empire

	public:
		void 				SetEmpire(BYTE bEmpire);
		BYTE				GetEmpire() const { return m_bEmpire; }

	protected:
		BYTE				m_bEmpire;

		////////////////////////////////////////////////////////////////////////////////////////
		// Regen
	public:
		void				SetRegen(LPREGEN pkRegen);

	protected:
		PIXEL_POSITION			m_posRegen;
		float				m_fRegenAngle;
		LPREGEN				m_pkRegen;
		size_t				regen_id_; // to help dungeon regen identification
		// End of Regen

		////////////////////////////////////////////////////////////////////////////////////////
		// Resists & Proofs
	public:
		bool				CannotMoveByAffect() const;
		bool				IsImmune(DWORD dwImmuneFlag);
		void				SetImmuneFlag(DWORD dw) { m_pointsInstant.dwImmuneFlag = dw; }

	protected:
		void				ApplyMobAttribute(const TMobTable* table);
		// End of Resists & Proofs

		////////////////////////////////////////////////////////////////////////////////////////
		// QUEST

	public:
		void				SetQuestNPCID(DWORD vid);
		DWORD				GetQuestNPCID() const { return m_dwQuestNPCVID; }
		LPCHARACTER			GetQuestNPC() const;

		void				SetQuestItemPtr(LPITEM item);
		void				ClearQuestItemPtr();
		LPITEM				GetQuestItemPtr() const;

#ifdef ENABLE_QUEST_DND_EVENT
		void				SetQuestDNDItemPtr(LPITEM item);
		void				ClearQuestDNDItemPtr();
		LPITEM				GetQuestDNDItemPtr() const;
#endif

		void				SetQuestBy(DWORD dwQuestVnum)	{ m_dwQuestByVnum = dwQuestVnum; }
		DWORD				GetQuestBy() const			{ return m_dwQuestByVnum; }

		int					GetQuestFlag(const std::string& flag) const;
		void				SetQuestFlag(const std::string& flag, int value);

		void				ConfirmWithMsg(const char* szMsg, int iTimeout, DWORD dwRequestPID);

	private:
		DWORD				m_dwQuestNPCVID;
		DWORD				m_dwQuestByVnum;
		DWORD				m_dwQuestItemVID{}; // @fixme304 (LPITEM -> DWORD)
#ifdef ENABLE_QUEST_DND_EVENT
		DWORD				m_dwQuestDNDItemVID{}; // @fixme304 (LPITEM -> DWORD)
#endif

		// Events
	public:
		bool				StartStateMachine(int iPulse = 1);
		void				StopStateMachine();
		void				UpdateStateMachine(DWORD dwPulse);
		void				SetNextStatePulse(int iPulseNext);

		void				UpdateCharacter(DWORD dwPulse);

	protected:
		DWORD				m_dwNextStatePulse;

		// Marriage
	public:
		LPCHARACTER			GetMarryPartner() const;
		void				SetMarryPartner(LPCHARACTER ch);
		int					GetMarriageBonus(DWORD dwItemVnum, bool bSum = true);

		void				SetWeddingMap(marriage::WeddingMap* pMap);
		marriage::WeddingMap* GetWeddingMap() const { return m_pWeddingMap; }

	private:
		marriage::WeddingMap* m_pWeddingMap;
		LPCHARACTER			m_pkChrMarried;

		// Warp Character
	public:
		void				StartWarpNPCEvent();

	public:
		void				StartSaveEvent();
		void				StartRecoveryEvent();
		void				StartCheckSpeedHackEvent();
		void				StartDestroyWhenIdleEvent();
		LPEVENT				m_pkDeadEvent;
		LPEVENT				m_pkStunEvent;
		LPEVENT				m_pkSaveEvent;
		LPEVENT				m_pkRecoveryEvent;
		LPEVENT				m_pkTimedEvent;
		LPEVENT				m_pkFishingEvent;
		LPEVENT				m_pkAffectEvent;
		LPEVENT				m_pkPoisonEvent;
#ifdef ENABLE_WOLFMAN_CHARACTER
		LPEVENT				m_pkBleedingEvent;
#endif
		LPEVENT				m_pkFireEvent;
		LPEVENT				m_pkWarpNPCEvent;
		//DELAYED_WARP
		//END_DELAYED_WARP

		// MINING
		LPEVENT				m_pkMiningEvent;
		// END_OF_MINING
		LPEVENT				m_pkWarpEvent;
		LPEVENT				m_pkCheckSpeedHackEvent;
		LPEVENT				m_pkDestroyWhenIdleEvent;
		LPEVENT				m_pkPetSystemUpdateEvent;
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		LPEVENT				m_pkOfflineShopUpdateEvent;
#endif
#ifdef ENABLE_PVP_RANKING
		LPEVENT				m_pkPvPRankingEvent;
#endif
		bool IsWarping() const { return m_pkWarpEvent ? true : false; }

		bool				m_bHasPoisoned;
#ifdef ENABLE_WOLFMAN_CHARACTER
		bool				m_bHasBled;
#endif

		const CMob *		m_pkMobData;
		CMobInstance *		m_pkMobInst;

		std::map<int, LPEVENT> m_mapMobSkillEvent;

		friend struct FuncSplashDamage;
		friend struct FuncSplashAffect;
		friend class CFuncShoot;

	public:
		int				GetPremiumRemainSeconds(BYTE bType) const;

	private:
		int				m_aiPremiumTimes[PREMIUM_MAX_NUM];

		// CHANGE_ITEM_ATTRIBUTES
		static const char		msc_szLastChangeItemAttrFlag[];
		// END_OF_CHANGE_ITEM_ATTRIBUTES

		// NEW_HAIR_STYLE_ADD
	public :
		bool ItemProcess_Hair(LPITEM item, int iDestCell);
		// END_NEW_HAIR_STYLE_ADD

	public :
		void ClearSkill();
		void ClearSubSkill();

		// RESET_ONE_SKILL
		bool ResetOneSkill(DWORD dwVnum);
		// END_RESET_ONE_SKILL

	private :
		void SendDamagePacket(LPCHARACTER pAttacker, int64_t damage, BYTE DamageFlag);

	// ARENA
	private :
		CArena *m_pArena;
		bool m_ArenaObserver;
		int m_nPotionLimit;

	public :
		void 	SetArena(CArena* pArena) { m_pArena = pArena; }
		void	SetArenaObserverMode(bool flag) { m_ArenaObserver = flag; }

		CArena* GetArena() const { return m_pArena; }
		bool	GetArenaObserverMode() const { return m_ArenaObserver; }

		void	SetPotionLimit(int count) { m_nPotionLimit = count; }
		int		GetPotionLimit() const { return m_nPotionLimit; }
	// END_ARENA

		//PREVENT_TRADE_WINDOW
	public:
		bool	IsOpenSafebox() const { return m_isOpenSafebox ? true : false; }
		void 	SetOpenSafebox(bool b) { m_isOpenSafebox = b; }

		int		GetSafeboxLoadTime() const { return m_iSafeboxLoadTime; }
		void	SetSafeboxLoadTime() { m_iSafeboxLoadTime = thecore_pulse(); }
		//END_PREVENT_TRADE_WINDOW
#ifdef WEEKLY_RANK_BYLUZER
		int		GetCooldownWeekly() const { return m_CooldownWeekly; }
		void	SetCooldownWeekly() { m_CooldownWeekly = thecore_pulse(); }
		int		GetWeeklyLoadCount() const { return m_WeeklyLoadCount; }
		void	SetWeeklyLoadCount(BYTE ilosc) { m_WeeklyLoadCount = ilosc; }
#endif
#ifdef TITLE_SYSTEM_BYLUZER
		int		GetCooldownTitle() const {return m_CooldownTitle;}
		void	SetCooldownTitle() {m_CooldownTitle = thecore_pulse();}
#endif

	private:
		bool	m_isOpenSafebox;

	public:
		int		GetSkillPowerByLevel(int level, bool bMob = false) const;

		//PREVENT_REFINE_HACK
		int		GetRefineTime() const { return m_iRefineTime; }
		void	SetRefineTime() { m_iRefineTime = thecore_pulse(); }
		int		m_iRefineTime;
		//END_PREVENT_REFINE_HACK

#ifdef ENABLE_COOLDOWN_ARMOR
		//PREVENT_ARMOR_WALLHACK
		int		GetArmorUseTime() const { return m_iArmorUseTime; }
		void	SetArmorUseTime() { m_iArmorUseTime = thecore_pulse(); }
		int		m_iArmorUseTime;
		//END_PREVENT_ARMOR_WALLHACK
#endif

		//RESTRICT_USE_SEED_OR_MOONBOTTLE
		int 	GetUseSeedOrMoonBottleTime() const { return m_iSeedTime; }
		void  	SetUseSeedOrMoonBottleTime() { m_iSeedTime = thecore_pulse(); }
		int 	m_iSeedTime;
		//END_RESTRICT_USE_SEED_OR_MOONBOTTLE

		//PREVENT_PORTAL_AFTER_EXCHANGE
		int		GetExchangeTime() const { return m_iExchangeTime; }
		void	SetExchangeTime() { m_iExchangeTime = thecore_pulse(); }
		int		m_iExchangeTime;
		//END_PREVENT_PORTAL_AFTER_EXCHANGE

		int 	m_iMyShopTime;
		int		GetMyShopTime() const	{ return m_iMyShopTime; }
		void	SetMyShopTime() { m_iMyShopTime = thecore_pulse(); }

		bool	IsHack(bool bSendMsg = true, bool bCheckShopOwner = true, int limittime = g_nPortalLimitTime);

		// MONARCH
		BOOL	IsMonarch() const;
		// END_MONARCH
		void Say(const std::string & s);

		enum MONARCH_COOLTIME
		{
			MC_HEAL = 10,
			MC_WARP	= 60,
			MC_TRANSFER = 60,
			MC_TAX = (60 * 60 * 24 * 7),
			MC_SUMMON = (60 * 60),
		};

		enum MONARCH_INDEX
		{
			MI_HEAL = 0,
			MI_WARP,
			MI_TRANSFER,
			MI_TAX,
			MI_SUMMON,
			MI_MAX
		};

		DWORD m_dwMonarchCooltime[MI_MAX]; //todo move inside PlayerSlotT
		DWORD m_dwMonarchCooltimelimit[MI_MAX]; //todo move inside PlayerSlotT

		void  InitMC();
		DWORD GetMC(enum MONARCH_INDEX e) const;
		void SetMC(enum MONARCH_INDEX e);
		bool IsMCOK(enum MONARCH_INDEX e) const;
		DWORD GetMCL(enum MONARCH_INDEX e) const;
		DWORD GetMCLTime(enum MONARCH_INDEX e) const;

	public:
		bool ItemProcess_Polymorph(LPITEM item);

#ifdef ENABLE_COOLDOWN_AUTOPOTION
	public:
		AutoPotionCooldown = get_global_time();
#endif

	public:
		bool IsSiegeNPC() const;

	private:

		e_overtime m_eOverTime;

	public:
		bool IsOverTime(e_overtime e) const { return (e == m_eOverTime); }
		void SetOverTime(e_overtime e) { m_eOverTime = e; }

	private:
		int		m_deposit_pulse;

	public:
		void	UpdateDepositPulse();
		bool	CanDeposit() const;

	private:
		void	__OpenPrivateShop();

	public:
		struct AttackedLog
		{
			DWORD 	dwPID;
			DWORD	dwAttackedTime;

			AttackedLog() : dwPID(0), dwAttackedTime(0)
			{
			}
		};

		AttackLog	m_kAttackLog;
		AttackedLog m_AttackedLog;
		int			m_speed_hack_count;

	private :
		std::string m_strNewName;

	public :
		const std::string GetNewName() const { return this->m_strNewName; }
		void SetNewName(const std::string name) { this->m_strNewName = name; }

	public :
		void GoHome();

	private :
		std::set<DWORD>	m_known_guild;

	public :
		void SendGuildName(CGuild* pGuild);
		void SendGuildName(DWORD dwGuildID);

	private :
		DWORD m_dwLogOffInterval;

	public :
		DWORD GetLogOffInterval() const { return m_dwLogOffInterval; }


	public:
		bool UnEquipSpecialRideUniqueItem ();

		bool CanWarp () const;
	#ifdef WEEKLY_RANK_BYLUZER
		bool InDungeon() const;
	#endif
		bool CanProcessPolymorph() const;

	public:
		void AutoRecoveryItemProcess (const EAffectTypes);

	public:
		void BuffOnAttr_AddBuffsFromItem(LPITEM pItem);
		void BuffOnAttr_RemoveBuffsFromItem(LPITEM pItem);

	private:
		void BuffOnAttr_ValueChange(BYTE bType, BYTE bOldValue, BYTE bNewValue);
		void BuffOnAttr_ClearAll();

		typedef std::map <BYTE, CBuffOnAttributes*> TMapBuffOnAttrs;
		TMapBuffOnAttrs m_map_buff_on_attrs;

	public:
		void SetArmada() { cannot_dead = true; }
		void ResetArmada() { cannot_dead = false; }
	private:
		bool cannot_dead;
#ifdef __RENEWAL_MOUNT__
	private:
		bool m_bIsHorse;
	public:
		void SetHorse() { m_bIsHorse = true; }
		bool IsHorse() { return m_bIsHorse; }
#endif
#ifdef __PET_SYSTEM__
	private:
		bool m_bIsPet;
	public:
		void SetPet() { m_bIsPet = true; }
		bool IsPet() { return m_bIsPet; }
#endif
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	private:
		bool m_bIsMount;
	public:
		void SetMount() { m_bIsMount = true; }
		bool IsMount() const { return m_bIsMount; }
#endif
#ifdef ENABLE_ALIGN_RENEWAL
	public:
		void	OnAlignUpdate(int64_t lOldAlignment);
#endif

#ifdef NEW_ICEDAMAGE_SYSTEM
	private:
		DWORD m_dwNDRFlag;
		std::set<DWORD> m_setNDAFlag;
	public:
		const DWORD GetNoDamageRaceFlag();
		void SetNoDamageRaceFlag(DWORD dwRaceFlag);
		void UnsetNoDamageRaceFlag(DWORD dwRaceFlag);
		void ResetNoDamageRaceFlag();
		const std::set<DWORD> & GetNoDamageAffectFlag();
		void SetNoDamageAffectFlag(DWORD dwAffectFlag);
		void UnsetNoDamageAffectFlag(DWORD dwAffectFlag);
		void ResetNoDamageAffectFlag();
#endif

	private:
		float m_fAttMul;
		float m_fDamMul;
	public:
		float GetAttMul() { return this->m_fAttMul; }
		void SetAttMul(float newAttMul) {this->m_fAttMul = newAttMul; }
		float GetDamMul() { return this->m_fDamMul; }
		void SetDamMul(float newDamMul) {this->m_fDamMul = newDamMul; }

	private:
		bool IsValidItemPosition(TItemPos Pos) const;

	public:

		void	DragonSoul_Initialize();

		bool	DragonSoul_IsQualified() const;
		void	DragonSoul_GiveQualification();

		int		DragonSoul_GetActiveDeck() const;
		bool	DragonSoul_IsDeckActivated() const;
		bool	DragonSoul_ActivateDeck(int deck_idx);

		void	DragonSoul_DeactivateAll();

		void	DragonSoul_CleanUp();

	public:
		bool		DragonSoul_RefineWindow_Open(LPENTITY pEntity);
#if defined(__DS_CHANGE_ATTR__)
		bool DragonSoul_RefineWindow_ChangeAttr_Open(LPENTITY pEntity);
#endif
		bool		DragonSoul_RefineWindow_Close();
		LPENTITY	DragonSoul_RefineWindow_GetOpener() { return  m_pointsInstant.m_pDragonSoulRefineWindowOpener; }
		bool		DragonSoul_RefineWindow_CanRefine();
		
#ifdef ENABLE_VOTE4BUFF
	public:
		void Vote4Buff_Initialize();
		void Vote4Buff_CheckHWID();
		void Vote4Buff_SendClientInfo(bool withBonusInfo = false);
		bool Vote4Buff_IsVoted();
		void Vote4Buff_Vote(int32_t time_left, bool is_main_voter);
		void Vote4Buff_SelectBonus(int index);
		void Vote4Buff_Reset();
		std::string Vote4Buff_GetApiURL();
		std::string Vote4Buff_GetURL();
#endif
		
#if defined(BL_OFFLINE_MESSAGE)
	protected:
		DWORD				dwLastOfflinePMTime;
	public:
		DWORD				GetLastOfflinePMTime() const { return dwLastOfflinePMTime; }
		void				SetLastOfflinePMTime() { dwLastOfflinePMTime = get_dword_time(); }
		void				SendOfflineMessage(const char* To, const char* Message);
		void				ReadOfflineMessages();
#endif

	private:
		unsigned int itemAward_vnum;
		char		 itemAward_cmd[20];
	public:
		unsigned int GetItemAward_vnum() { return itemAward_vnum; }
		char*		 GetItemAward_cmd() { return itemAward_cmd;	  }
		void		 SetItemAward_vnum(unsigned int vnum) { itemAward_vnum = vnum; }
		void		 SetItemAward_cmd(char* cmd) { strcpy(itemAward_cmd,cmd); }

	private:
		timeval		m_tvLastSyncTime;
		int			m_iSyncHackCount;
#ifdef ENABLE_SWITCHBOT
		DWORD		use_item_anti_flood_count_;
		int			use_item_anti_flood_pulse_;
#endif
	public:
		void			SetLastSyncTime(const timeval &tv) { memcpy(&m_tvLastSyncTime, &tv, sizeof(timeval)); }
		const timeval&	GetLastSyncTime() { return m_tvLastSyncTime; }
		void			SetSyncHackCount(int iCount) { m_iSyncHackCount = iCount;}
		int				GetSyncHackCount() { return m_iSyncHackCount; }
#ifdef ENABLE_COOLDOWN_AUTOPOTION
		time_t			AutoPotionCooldown;
#endif

#if defined(__ENABLE_LOTTERY__)
	private:
		TLotteryCharacter	m_lotteryCharacter;

	public:
		void	SetLotteryMobVnum(WORD wMobVnum);
		void	SetLotteryIndex(BYTE byIndex);
		void	SetLotteryState(bool isStart);

		WORD	GetLotteryMobVnum() const;
		BYTE	GetLotteryIndex() const;
		bool	IsLotteryStarted() const;

		void	ClearLotteryInfo();

		void	SetLotteryCoolTime();
		int		GetLotteryCoolTime() const;
#endif

#ifdef ENABLE_RESP_SYSTEM
	public:
		PIXEL_POSITION GetRegenPos() { return m_posRegen; }
		bool	CanActRespManager() { return g_setRespAllowedMap.find(GetMapIndex()) != g_setRespAllowedMap.end(); }
#endif
	// @fixme188 BEGIN
	public:
		void ResetRewardInfo() {
			m_map_kDamage.clear();
			if (!IsPC())
				SetExp(0);
		}
		void DeadNoReward() {
			if (!IsDead())
				ResetRewardInfo();
			Dead();
		}
	// @fixme188 END

#ifdef ENABLE_AURA_SYSTEM
	protected:
		bool	m_bAuraRefine, m_bAuraAbsorption;
	
	public:
		bool	isAuraOpened(bool bCombination) { return bCombination ? m_bAuraRefine : m_bAuraAbsorption; }
		bool	isAuraOpened(bool bCombination) const { return bCombination ? m_bAuraRefine : m_bAuraAbsorption; }
		void	OpenAura(bool bCombination);
		void	CloseAura();
		void	ClearAuraMaterials();
		bool	CleanAuraAttr(LPITEM pkItem, LPITEM pkTarget);
		LPITEM* GetAuraMaterials() { return m_pointsInstant.pAuraMaterials; }
		bool	AuraIsSameGrade(int32_t lGrade);
		DWORD	GetAuraCombinePrice(int32_t lGrade);
		void	GetAuraCombineResult(DWORD& dwItemVnum, DWORD& dwMinAbs, DWORD& dwMaxAbs);
		BYTE	CheckAuraEmptyMaterialSlot();
		void	AddAuraMaterial(TItemPos tPos, BYTE bPos);
		void	RemoveAuraMaterial(BYTE bPos);
		BYTE	CanRefineAuraMaterials();
		void	RefineAuraMaterials();
#endif

#ifdef ENABLE_HIDE_COSTUME_SYSTEM
	public:
		void			SetBodyCostumeHidden(bool hidden);
		bool			IsBodyCostumeHidden() const { return m_bHideBodyCostume; };

		void			SetHairCostumeHidden(bool hidden);
		bool			IsHairCostumeHidden() const { return m_bHideHairCostume; };

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		void			SetAcceCostumeHidden(bool hidden);
		bool			IsAcceCostumeHidden() const { return m_bHideAcceCostume; };
#endif

#ifdef ENABLE_STOLE_COSTUME
		void			SetStoleCostumeHidden(bool hidden);
		bool			IsStoleCostumeHidden() const { return m_bHideStoleCostume; };
#endif

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
		void			SetWeaponCostumeHidden(bool hidden);
		bool			IsWeaponCostumeHidden() const { return m_bHideWeaponCostume; };
#endif

#ifdef ENABLE_AURA_SYSTEM
		void			SetAuraCostumeHidden(bool hidden);
		bool			IsAuraCostumeHidden() const { return m_bHideAuraCostume; };
#endif

	private:
		bool			m_bHideBodyCostume;
		bool			m_bHideHairCostume;
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		bool			m_bHideAcceCostume;
#endif

#ifdef ENABLE_STOLE_COSTUME
		bool			m_bHideStoleCostume;
#endif

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
		bool			m_bHideWeaponCostume;
#endif

#ifdef ENABLE_AURA_SYSTEM
		bool			m_bHideAuraCostume;
#endif
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	protected:
		bool	m_bAcceCombination, m_bAcceAbsorption;

	public:
		bool	IsAcceOpened(bool bCombination) {return bCombination ? m_bAcceCombination : m_bAcceAbsorption;}
		bool	IsAcceOpened() const { return (m_bAcceCombination || m_bAcceAbsorption); }
		void	OpenAcce(bool bCombination);
		void	CloseAcce();
		void	ClearAcceMaterials();
		bool	CleanAcceAttr(LPITEM pkItem, LPITEM pkTarget);
		std::vector<LPITEM>	GetAcceMaterials();
		const TItemPosEx*	GetAcceMaterialsInfo();
		void	SetAcceMaterial(int pos, LPITEM ptr);
		bool	AcceIsSameGrade(int32_t lGrade);

		int64_t	GetAcceCombinePrice(int32_t lGrade);
		int64_t	GetAcceCombineCheque(int32_t lGrade);

		BYTE	GetAcceCombineChance(int32_t lGrade);

		void	SetAcceCombineMaterials(int32_t lGrade);
		struct AcceCombineMaterials
		{
			int	vnum[ACCE_MATERIALS_COUNT];
			DWORD count[ACCE_MATERIALS_COUNT];
		};

		void	GetAcceCombineResult(DWORD & dwItemVnum, DWORD & dwMinAbs, DWORD & dwMaxAbs);
		BYTE	CheckEmptyMaterialSlot();
		void	AddAcceMaterial(TItemPos tPos, BYTE bPos);
		void	RemoveAcceMaterial(BYTE bPos);
		BYTE	CanRefineAcceMaterials();
		void	RefineAcceMaterials();
	protected:
		AcceCombineMaterials taccematerials;
#endif
//CHARACTER ACTIVATE CHECK BEGIN
public:
	void			WindowCloseAll();
	bool			WindowOpenCheck() const;
	bool			ActivateCheck(bool notquest = false, bool dead = false) const;
	bool			OfflineShopActivateCheck() const;
	bool			IsOpenedRefine() const { return m_bUnderRefine; }
	int				GetActivateTime(BYTE type) const { return m_ActivateTime[type]; }
	bool			IsActivateTime(BYTE type,BYTE second) const;
	void			SetActivateTime(BYTE type) { m_ActivateTime[type] = thecore_pulse();}

	bool			IsOpenOfflineShop() const;
	bool			IsOfflineShopActivateTime(BYTE second) const;
private:
	int m_ActivateTime[MAX_ACTIVATE_CHECK];
//CHARACTER ACTIVATE CHECK END
#ifdef ENABLE_EXTENDED_BLEND
	public: 
		int GetAffectType(LPITEM item);
#endif
#ifdef ENABLE_BOT_CONTROL
public:
	void SetBotControl(bool state) { m_isBotControl = state; }
	bool IsBotControl() { return m_isBotControl; }
private:
	bool m_isBotControl;
#endif
public:
	CFishWiki*	GetFishWiki();
private:
	CFishWiki* m_fishWikiSystem;
	
public:
	bool		IsGuildDungeonHistoryLoaded() { return isGuildDungeonHistoryLoaded; }
	void		SetIsGuildDungeonHistoryLoaded(bool val) { isGuildDungeonHistoryLoaded = val; }
	bool		IsGuildGuardItemsLoaded() { return isGuildGuardItemsLoaded; }
	void		SetIsGuildGuardItemsLoaded(bool val) { isGuildGuardItemsLoaded = val; }
	void		SetNextGuildLogsRefreshTime(uint32_t nextTime) { nextCanRefreshGuildLogsTime = nextTime; }
	uint32_t	GetNextGuildLogsRefreshTime() { return nextCanRefreshGuildLogsTime; }
protected:
	bool		isGuildDungeonHistoryLoaded;
	bool		isGuildGuardItemsLoaded;
	uint32_t	nextCanRefreshGuildLogsTime;
	
public:
	bool IsFishing() { return m_pkMiningEvent != NULL; }
	bool IsMining() { return m_pkFishingEvent != NULL; }
public:
	int GetNextCanMountTime() { return m_iNextCanMountTime; }
	void SetNextCanMountTime(int time) { m_iNextCanMountTime = time; }
private:
	int m_iNextCanMountTime;

#ifdef ENABLE_MOUNT_SYSTEM
protected:
	DWORD m_dwLastMountUseTime;
#endif

#ifdef ENABLE_REAL_TIME_REGEN
public:
	void		SetRealTimeRegenNum(uint32_t num)	{ m_wRealTimeRegenNum = num; };
	uint32_t		GetRealTimeRegenNum() 			{ return m_wRealTimeRegenNum; };
	
	LPREGEN		GetRegen();
	
private:
	uint32_t		m_wRealTimeRegenNum;
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
public:
	void SendPetInfo(bool summon = false);
	void PetSkillUp(int skill_id, bool set_from_quest = false);
	void PetSetExp(int count);
	void SummonPet();
	void AddPetAffect();
	void UpdatePet();
	bool PetName(std::string name);
	void PetSetExpPercent(int count);
	int PetGetLevel();
#endif

#ifdef ENABLE_CHEQUE_SYSTEM
	private:
		DWORD m_dwLastChequeDropTime;
#endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	protected:
		CBuffNPCActor*	m_buffNPCSystem;
		
	public:
		CBuffNPCActor*	GetBuffNPCSystem() { return m_buffNPCSystem; }

		void			LoadBuffNPC();

		void			SetBuffNPC(bool isActive)	{ m_bIsBuffNPC = isActive;}
		bool			IsBuffNPC() const { return m_bIsBuffNPC; }

		void			SendBuffNPCUseSkillPacket(int skill_vnum, int skill_level);
		LPITEM			GetBuffWear(UINT bCell) const;
		LPITEM			GetBuffEquipmentItem(WORD wCell) const;
		bool			IsBuffEquipUniqueItem(DWORD dwItemVnum) const;
		bool 			CheckBuffEquipmentPositionAvailable(int iWearCell);
		bool 			EquipBuffItem(BYTE cell, LPITEM item);
		bool 			UnequipBuffItem(BYTE cell, LPITEM item);
		bool 			IsBuffEquipEmpty();

		void			SetBuffWearWeapon(DWORD vnum) { m_dwBuffWeapon = vnum; }
		int				GetBuffWearWeapon() { return m_dwBuffWeapon; }
		void			SetBuffWearHead(DWORD value0) { m_dwBuffHead = value0; }
		int				GetBuffWearHead() { return m_dwBuffHead; }
		void			SetBuffWearArmor(DWORD vnum) { m_dwBuffArmor = vnum; }
		int				GetBuffWearArmor() { return m_dwBuffArmor; }

		void			SetBuffNPCInt(int value) { m_dwBuffNPCInt = value; }
		DWORD		GetBuffNPCInt() { return m_dwBuffNPCInt; }

		void			SetBuffHealValue(int value) { m_buffSkillRecoverHPValue = value; }
		DWORD		GetBuffHealValue() { return m_buffSkillRecoverHPValue; }
		
		void			SetBuffSealVID(int vid) { m_sealVID = vid; }
		int				GetBuffSealVID() { return m_sealVID; }
		
		LPEVENT	m_pkBuffNPCSystemUpdateEvent;
		LPEVENT	m_pkBuffNPCSystemExpireEvent;

	private:
		bool		m_bIsBuffNPC;
		DWORD	m_dwBuffNPCVid;
		DWORD	m_dwBuffNPCInt;
		int			m_sealVID;
		int			m_buffSkillRecoverHPValue;
		
		DWORD	m_dwBuffWeapon;
		DWORD	m_dwBuffHead;
		DWORD	m_dwBuffArmor;
#endif
#if defined(__DUNGEON_INFO_SYSTEM__)
	public:
		void SetDungeonInfoOpen(bool bOpen = true) { m_bDungeonInfoOpen = bOpen; };
		bool IsDungeonInfoOpen() { return m_bDungeonInfoOpen; }

		void StartDungeonInfoReloadEvent();
		void StopDungeonInfoReloadEvent();

		bool UpdateDungeonRanking(const std::string c_strQuestName);

		void SetLastDamage(int iLastDamage) { m_iLastDamage = iLastDamage; }
		int GetLastDamage() { return m_iLastDamage; }

	protected:
		bool m_bDungeonInfoOpen;
		LPEVENT m_pkDungeonInfoReloadEvent;
		int m_iLastDamage;
#endif
#ifdef ENABLE_CHEQUE_SYSTEM
	public:
		void		WonExchange(BYTE bOption, WORD wValue);
#endif

#ifdef ENABLE_MINIMAP_DUNGEONINFO
	public:
		void SendCachedMiniMapDungeonInfo();
		void SendMiniMapDungeonInfo(BYTE subheader);
#endif
#ifdef ENABLE_SYSTEMY_KARTY_OKEY
	public:
		struct S_CARD
		{
			DWORD	type;
			DWORD	value;
		};

		struct CARDS_INFO
		{
			S_CARD cards_in_hand[MAX_CARDS_IN_HAND];
			S_CARD cards_in_field[MAX_CARDS_IN_FIELD];
			DWORD	cards_left;
			DWORD	field_points;
			DWORD	points;
		};
		
		void			Cards_open(DWORD safemode);
		void			Cards_clean_list();
		DWORD			GetEmptySpaceInHand();
		void			Cards_pullout();
		void			RandomizeCards();
		bool			CardWasRandomized(DWORD type, DWORD value);
		void			SendUpdatedInformations();
		void			SendReward();
		void			CardsDestroy(DWORD reject_index);
		void			CardsAccept(DWORD accept_index);
		void			CardsRestore(DWORD restore_index);
		DWORD			GetEmptySpaceInField();
		DWORD			GetAllCardsCount();
		bool			TypesAreSame();
		bool			ValuesAreSame();
		bool			CardsMatch();
		DWORD			GetLowestCard();
		bool			CheckReward();
		void			CheckCards();
		void			RestoreField();
		void			ResetField();
		void			CardsEnd();
		void			GetGlobalRank(char * buffer, size_t buflen);
		void			GetRundRank(char * buffer, size_t buflen);
	protected:
		CARDS_INFO	character_cards;
		S_CARD	randomized_cards[24];
#endif
#ifdef ENABLE_COLLECT_WINDOW
	public:
		void			SendCollectPacket(const BYTE windowType, const DWORD time, const DWORD count, const DWORD itemVnum, const DWORD countTotal, const BYTE chance, const DWORD bRenderTargetID, const DWORD bQuestIndex, const DWORD reqLevel);
#endif
	protected:
		bool			m_block_exp;
		DWORD			m_dwLastAntyExpTime;
	public:
		bool			IsAntyExp() { return m_block_exp; }
		void			SetToggleAntyExp(bool exp) { m_block_exp = exp; }
		void			CheckAntyExp();
	private:
		int		m_iEquipItemsTime;
		int		m_iEquipItemsCount;
		int		m_iEquipItemsLastTime;
	public:
		int		GetEquipItemsTime() const { return m_iEquipItemsTime; }
		void	SetEquipItemsTime() { m_iEquipItemsTime = thecore_pulse(); }
		int		GetEquipItemsCount() const { return m_iEquipItemsCount; }
	private:
		int		m_iPlayerDamageTime;
	public:
		int		GetPlayerDamageTime() const { return m_iPlayerDamageTime; }
		void	SetPlayerDamageTime() { m_iPlayerDamageTime = thecore_pulse(); }
#ifdef ENABLE_HWID_SYSTEM
	public:
		bool		IsHwidBanned() const;
		std::string GetHwid(BYTE bComponent);
		std::string GetCompositeHWID();
#endif

#ifdef TAKE_LEGEND_DAMAGE_BOARD_SYSTEM
	public:
		int				GetLegendLastDamageInfo() const { return m_LegendLastDamageInfo; }
		void			SetLegendLastDamageInfo() { m_LegendLastDamageInfo = thecore_pulse(); }
	protected:
		int				m_LegendLastDamageInfo;
#endif // TAKE_LEGEND_DAMAGE_BOARD_SYSTEM


#ifdef ENABLE_ANTI_PACKET_FLOOD
public:
	int analyze_protect; 
	int analyze_protect_count;
#endif

#ifdef __RESET_PLAYER_DAMAGE__
	public:
		void ErasePlayerDamage(const CHARACTER* player) {
			if (!player)
				return;

			auto fIt = m_map_kDamage.find(player->GetVID());
			if (fIt != m_map_kDamage.end()) {
				m_map_kDamage.erase(fIt);
			}
		};
#endif
#ifdef __RENEWAL_MOUNT__
		bool			MountBlockMap();
#endif
public:
	void SetSkipUpdatePacket(bool skipUpdatePacket) {
		skipUpdatePacket_ = skipUpdatePacket;
	}
	bool IsSkipUpdatePacket() const { return skipUpdatePacket_; }

protected:
	bool skipUpdatePacket_;

#ifdef __PREMIUM_PRIVATE_SHOP__
	private:
		LPPRIVATE_SHOP		m_pPrivateShop;
		LPPRIVATE_SHOP		m_pMyPrivateShop;
		DWORD				m_dwPrivateShopOwner;
		TPrivateShop		m_privateShopTable;
		bool				m_bIsEditingPrivateShop;
		BYTE				m_bShopSearchMode;

		time_t				m_tLastPrivateShopModify;
		time_t				m_tLastPrivateShopWithdraw;
		time_t				m_tLastPrivateShopClose;
		time_t				m_tLastPrivateShopBuy;
		time_t				m_tLastPrivateShopSearch;
		time_t				m_tLastPrivateShopStateChange;
		time_t				m_tLastPrivateShopBuild;

		std::vector<TPlayerPrivateShopItem>		m_vec_privateShopItem;
		std::vector<TPrivateShopSale>			m_vec_privateShopSale;
		DWORD				m_dwCurrentPrivateShopTime;

	public:
		bool				BuildPrivateShop(const char* c_szTitle, DWORD dwPolyVnum, BYTE bTitleType, BYTE bPageCount, WORD wItemCount, TPrivateShopItem* pShopItemTable);
		void				ClosePrivateShop();

		void				SetViewingPrivateShop(LPPRIVATE_SHOP pShop) { m_pPrivateShop = pShop; }
		LPPRIVATE_SHOP		GetViewingPrivateShop() const { return m_pPrivateShop; }

		void				SetMyPrivateShop(LPPRIVATE_SHOP pShop) { m_pMyPrivateShop = pShop; }
		LPPRIVATE_SHOP		GetMyPrivateShop() const { return m_pMyPrivateShop; }

		void				SetPrivateShopOwner(DWORD dwPID) { m_dwPrivateShopOwner = dwPID; }
		DWORD				GetPrivateShopOwner() { return m_dwPrivateShopOwner; }

		void				SetPrivateShopTable(const TPrivateShop& rPrivateShopTable);
		TPrivateShop*		GetPrivateShopTable() { return &m_privateShopTable; }
		bool				IsPrivateShopOwner() { return m_privateShopTable.dwOwner != 0; }
		bool				CanModifyPrivateShop() { return m_privateShopTable.bState == STATE_MODIFY; }

		void				SetEditingPrivateShop(bool bEditingPrivateShop) { m_bIsEditingPrivateShop = bEditingPrivateShop; }
		bool				IsEditingPrivateShop() const { return m_bIsEditingPrivateShop; }
		void				OpenPrivateShopPanel();
		void				ClosePrivateShopPanel(bool bSendClient = false);

		void				OpenShopSearch(BYTE bMode);
		void				CloseShopSearch();
		bool				IsShopSearch() const { return m_bShopSearchMode != MODE_NONE; }
		BYTE				GetShopSearchMode() { return m_bShopSearchMode; }

		int64_t						GetPrivateShopTotalGold();
		DWORD							GetPrivateShopTotalCheque();

		void							SetPrivateShopItem(const TPlayerPrivateShopItem& c_rPrivateShopItem);
		bool							RemovePrivateShopItem(WORD wPos);
		const TPlayerPrivateShopItem*	GetPrivateShopItem(WORD wPos);
		WORD							GetPrivateShopItemCount() { return m_vec_privateShopItem.size(); }

		void							SetPrivateShopSale(const TPrivateShopSale& c_rPrivateShopSale);
		WORD							GetPrivateShopSaleCount() { return m_vec_privateShopSale.size(); }

		void							ChangePrivateShopItemPrice(WORD wPos, int64_t llGold, DWORD dwCheque);
		void							ChangePrivateShopItemPos(WORD wPos, WORD wChangePos);
		void							ChangePrivateShopTitle(const char* c_szTitle);
		void							SaleUpdate(const TPrivateShopSale* c_pShopSale);
		void							ItemExpireUpdate(WORD wPos);
		void							SetPrivateShopState(BYTE bState, bool bIsMainPlayerPrivateShop);
		void							WithdrawPrivateShop(int64_t llGold, DWORD dwCheque);
		void							WarpToPrivateShop(int32_t lAddr, WORD wPort);

		bool							SetPremiumPrivateShopBonus(time_t tDuration, bool bIsAccumulable = false);

		int								GetLastPrivateShopModifyTime() const { return m_tLastPrivateShopModify; }
		void							SetLastPrivateShopModifyTime() { m_tLastPrivateShopModify = thecore_pulse(); }

		int								GetLastPrivateShopWithdrawTime() const { return m_tLastPrivateShopWithdraw; }
		void							SetLastPrivateShopWithdrawTime() { m_tLastPrivateShopWithdraw = thecore_pulse(); }

		int								GetLastPrivateShopCloseTime() const { return m_tLastPrivateShopClose; }
		void							SetLastPrivateShopCloseTime() { m_tLastPrivateShopClose = thecore_pulse(); }

		int								GetLastPrivateShopBuildTime() const { return m_tLastPrivateShopBuild; }
		void							SetLastPrivateShopBuildTime() { m_tLastPrivateShopBuild = thecore_pulse(); }

		int								GetLastPrivateShopBuyTime() const { return m_tLastPrivateShopBuy; }
		void							SetLastPrivateShopBuyTime() { m_tLastPrivateShopBuy = thecore_pulse(); }

		int								GetLastPrivateShopSearchTime() const { return m_tLastPrivateShopSearch; }
		void							SetLastPrivateShopSearchTime() { m_tLastPrivateShopSearch = thecore_pulse(); }

		int								GetLastPrivateShopStateChangeTime() const { return m_tLastPrivateShopStateChange; }
		void							SetLastPrivateShopStateChangeTime() { m_tLastPrivateShopStateChange = thecore_pulse(); }
#endif

public:
		int					GetMoveWaitTime() const { return m_iMoveWaitTime; }
		void				SetMoveWaitTime() { m_iMoveWaitTime = thecore_pulse(); }
protected:
	int					m_iMoveWaitTime;

	protected:
	    cube::CubePtr cube_;

	public:
	    void SetCube(cube::CubePtr cube) { cube_ = std::move(cube); }
	    cube::Cube* GetCube() const { return cube_.get(); }

	    void ResetCube() { cube_.reset(); }
	    void CloseCube() { ResetCube(); }

	    bool IsCubeOpen() const { return cube_.get() != nullptr; }
	    bool CanDoCube() const;

	public:
	    void Sort(EWindows window);
	    void SortStack(EWindows window);

	public:
	    inline Inventory& GetInventory() { return inventory_; }

	protected:
	    Inventory inventory_;

	protected:
	    OnClickSignals onClickSignals_;

	public:
	    boost::signals2::connection AddOnClickEvent(
	        const OnClickSignals::slot_type& f);
			
#ifdef _ENABLE_BATTLEPASS_
public:
	void LoadBP(const TPlayerBattlePass* db);
	void HandleBPTrigger(uint8_t trigger, uint32_t subject, uint32_t amount);
	void RollNewMissions(uint8_t track);
	void RestoreEventProgress();
	void SendBPState(uint8_t sub = 0);
	void UpgradeToPremium();
	void ForceComplete(uint16_t defId);
	void ResetDailyWeekly();

	void SetBPOpen(bool v) { m_bpOpen = v; }
	bool IsBPOpen()  const { return m_bpOpen; }

	const auto& GetBPGroups() const { return m_bpGroups; }

private:
	std::array<std::vector<BPSlot>, 3> m_bpGroups; // [0]=daily [1]=weekly [2]=event
	bool m_bpOpen = false;
	void AdvanceSlot(BPSlot& slot, const MissionDef& def, uint32_t amount);
#endif

#ifdef __ENABLE_POLYMORPH_SYSTEM__
public:
	uint8_t GetPolySkin() const { return m_bPolySkin; }
	void SetPolySkin(uint8_t bSkin) { m_bPolySkin = bSkin; }

	bool IsPolyShopping() const { return m_IsPolyShopping; }
	void SetPolyShopping(bool bStatus) { m_IsPolyShopping = bStatus; }

	bool CanPolyShop() const;

private:
	uint8_t m_bPolySkin = 0;
	bool m_IsPolyShopping = false;
#endif
#ifdef __INVENTORY_BUFFERING__
public:
	void SetInventoryBuffer(bool bMode) { bInvBuff = bMode; }
	bool IsInvBuffOn() { return bInvBuff; }
	void AddItemToInvBuff(LPITEM item) { us_buffered_items.insert(item); }
	void RemoveItemFromInvBuff(LPITEM item) { us_buffered_items.erase(item); }
	void SendBufferedInventoryPacket();
	void QuickOpenStack(LPITEM item);

private:
	std::unordered_set<LPITEM> us_buffered_items;
	bool bInvBuff;
	bool m_bQuickOpenInProgress;
#endif
#ifdef ENABLE_SAVE_LOCATION_SYSTEM
public:
	void	AddSaveLocation(BYTE bPos, const char* c_szName);
	void	DeleteSaveLocation(BYTE bPos);
	void	WarpToSaveLocation(BYTE bPos);
	void	RenameSaveLocation(BYTE bPos, const char* c_szName);
	void	UpdateSaveLocation();

protected:
	int		m_iLastSaveLocationWarpTime;
#endif

#ifdef __ENABLE_ITEM_TOGGLE__
	public:
		item::ItemToggle* FindToggleItem(bool active, uint8_t subtype, int32_t group = -1, CItem* except = nullptr);
		void ProcessAutoRecoveryItem(item::ItemToggle* item);

		void PostEntergameToggleItemCheck();
#endif
#ifdef RANKING_SYSTEM
public:
	void		SetActualServerRankLoadingType(uint8_t actualType) { m_iMyActualServerRankingLoadType = actualType; }
	uint8_t		GetActualServerRankLoadingType() { return m_iMyActualServerRankingLoadType; }
	void		SetNextServerRankRefreshTime(uint8_t rankType, uint32_t nextTime) { nextCanRefreshServerRankTime[rankType] = nextTime; }
	uint32_t	GetNextServerRankRefreshTime(uint8_t rankType) { return nextCanRefreshServerRankTime[rankType]; }

private:
	uint32_t	nextCanRefreshServerRankTime[SERVER_RANK_TYPE_MAX_NUM];
	uint8_t		m_iMyActualServerRankingLoadType;

public:
	void		SetActualRankLoadingType(uint8_t actualType) { m_iMyActualRankingLoadType = actualType; }
	uint8_t		GetActualRankLoadingType() { return m_iMyActualRankingLoadType; }
	void		SetNextRankRefreshTime(uint8_t rankType, uint32_t nextTime) { nextCanRefreshRankTime[rankType] = nextTime; }
	uint32_t	GetNextRankRefreshTime(uint8_t rankType) { return nextCanRefreshRankTime[rankType]; }
	
	void		SetRankDataLoaded(bool val) { isRankDataLoaded = val; }
	bool		IsRankDataLoaded() { return isRankDataLoaded; }

	void		DeleteAllRankingData();

private:
	bool		isRankDataLoaded;

	uint32_t	nextCanRefreshRankTime[RANK_TYPE_MAX_NUM];
	uint8_t		m_iMyActualRankingLoadType;
#endif
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
public:
	void SetEnlightLevel(uint8_t level) { m_points.enlightLevel = level; }
	uint8_t GetEnlightLevel() { return m_points.enlightLevel; }
#endif

#ifdef __ENABLE_MINING_WITH_SPACE__
public:
	void MiningNeareastOre();
#endif

#ifdef DUNGEON_CARDS
public:
	void OpenCards(LPCHARACTER ch);
#endif

private:
	std::map<std::string, int>  m_protection_Time;

#ifdef ENABLE_NOTIFICATION_SYSTEM
public:
	void SendNotification(BYTE byCategory, DWORD dwValue, DWORD dwAdditionalValue = 0);
#endif

#ifdef __VIP_SYSTEM__
public:
	bool 	IsVIP() const;
	bool	ItemProcessVIP(LPITEM item);
	bool 	RemoveVIP();
#endif

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
public:
	const std::string&	GetLanguage() const		{ return m_strLanguage; }
	void				SetLanguage(const std::string& lang) { m_strLanguage = lang; }
private:
	std::string			m_strLanguage;
#endif

public:
	void        SetProtectTime(const std::string& flagname, int value);
	int         GetProtectTime(const std::string& flagname) const;
	public:
		bool m_bEntityViewNotSendable;
		bool IsEntityViewSendable() { return !m_bEntityViewNotSendable; }
		void SetEntityViewSendable(bool state) { m_bEntityViewNotSendable = !state; }

public:
    cheat::Controller* GetCheatController() const {
        return cheatController_.get();
    }

protected:
    std::unique_ptr<cheat::Controller> cheatController_;
	//Abuse
private:
	spAbuseController	m_abuse;

public:
	spAbuseController	GetAbuseController() const { return m_abuse; }

public:
	bool AreAllSkillsMaxed() const;
#ifdef ENABLE_ANTY_CHEAT
private:
	uint32_t funcComboLimiter = 0;
	bool toDisconnectBfuncCombo = false;

	int32_t lastAttackPosX = 0;
	int32_t lastAttackPosY = 0;
	std::unordered_map<DWORD, DWORD> simpleAttackRateCounterMap;
public:
	void SetFuncComboLimterTime(int32_t timed) { funcComboLimiter = timed; }
	int GetFuncComboLimiterTime() const { return funcComboLimiter; }

	void SetLastAttackPosition(int32_t x, int32_t y) { lastAttackPosX= x; lastAttackPosY = y; }
#endif

#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
public:
	BYTE	GetTitleAchievement() const { return m_points.bTitleAchievement; }
	void	SetTitleAchievement(const BYTE val) { m_points.bTitleAchievement = val; UpdatePacket(); }
	
	BYTE	GetTitleAchievementPremium() const { return m_points.bTitleAchievementPremium; }
	void	SetTitleAchievementPremium(const BYTE val) { m_points.bTitleAchievementPremium = val; UpdatePacket(); }
#endif

#ifdef __SKILL_COSTUME__
public:
	void 			UseSkillCostumeItem(LPITEM item, bool bEquip);
	void 			SetSkillMotion(BYTE * bSkillMotion);
	BYTE* 			GetSkillMotion() { return m_bSkillMotion; }
protected:
	BYTE			m_bSkillMotion[ESkillMotionLength::MAX_SKILL_COUNT + ESkillMotionLength::MAX_BUFF_COUNT]; 		
#endif

#ifdef ENABLE_MESSENGER_BLOCK
private:
	int m_iBlockAntiFloodPulse;
	DWORD m_dwBlockAntiFloodCount;
public:
	int GetBlockAntiFloodPulse() const { return m_iBlockAntiFloodPulse; }
	DWORD GetBlockAntiFloodCount() const { return m_dwBlockAntiFloodCount; }
	DWORD IncreaseBlockAntiFloodCount() { return ++m_dwBlockAntiFloodCount; }
	void SetBlockAntiFloodPulse(int dwPulse) { m_iBlockAntiFloodPulse = dwPulse; }
	void SetBlockAntiFloodCount(DWORD dwCount) { m_dwBlockAntiFloodCount = dwCount; }
#endif
//#ifdef ENABLE_ITEMSHOP
private:
	bool m_bAccountMoneyLoaded;

	long long m_lldAccountCoins;
#ifdef USE_ITEMSHOP_RENEWED
	long long m_lldAccountJCoins;
#endif

public:
#ifdef USE_ITEMSHOP_RENEWED
	void GetAccountMoney(long long& lldCoins, long long& lldJCoins, bool bReloadForced = false);
	void SetAccountMoney(long long lldCoins, long long lldJCoins, bool bPlus);
#else
	void GetAccountMoney(long long& lldCoins, bool bReloadForced = false);
	void SetAccountMoney(long long lldCoins, bool bPlus);
#endif
	void RefreshAccountMoney();
//#endif
#ifdef __DUNGEON_INFO__
public:
	void SendDungeonCooldown(DWORD bossIdx);
	int GetQuestFlagSpecial(const char* szQuestFlag, ...);
#endif

public:
	template<typename... Args>
	void SendFormattedEventToClient(const std::string& eventName, Args&&... args);

	template<typename... Args>
	void SendEventToClient(const std::string& eventName, Args&&... args);

#ifdef ENABLE_EVENT_MANAGER
public:
	bool ActivatePersonalEvent(WORD eventID);
	bool IsPersonalEventActivated(WORD eventID) const;
	bool HasActivatedEventIndex(BYTE eventIndex) const;
	void SetActivatedEvents(const std::vector<std::pair<WORD, int>>& events);
	void SendActivatedEventsToClient() const;
	void LoadActivatedEvents();
protected:
	std::map<WORD, int> m_activatedEvents; // eventID -> UNIX activation timestamp
#endif


#ifdef __AUTO_SKILL_READER__
public:
	void			GetAutoSkill(BYTE skillIdx, bool status, bool isFromEvent = false);
	bool			ReadSkill(BYTE skillIdx);
	bool			SkillToBook(BYTE skillIdx, DWORD& itemIdx, DWORD& socket0, DWORD& exorcismIdx, DWORD& concentratedIdx);
	void			SetAutoSkillEvent(LPEVENT _event) { m_pkAutoSkill = _event; }
	BYTE			GetSelectedSkillIndex() { return m_bSelectedSkillIdx; }
	BYTE			GetSkillGradeByLevel(int iSkillLevel);
protected:
	enum EAutoSkillRead
	{
		// ! Don't touch !
		SKILL_GRADE_MASTER = 0,
		SKILL_GRADE_GRAND_MASTER = 1,
		SKILL_GRADE_PERFECT_MASTER = 2,
		SKILL_GRADE_L_MASTER = 3,
		// ! Don't touch !

		USE_YMIR_50300_SKILLBOOK = 0,// if the 0 system will search the manuel books example: 50401
		
		USE_COOLTIME_ON_BOOKS = 1,
		USE_COOLTIME_ON_SOULSTONE = 1,

		USE_DECREASE_ALIGNMENT_SOULSTONE = 1,
		USE_ZEN_BEAN_SOULSTONE = 1,

	};
	bool			m_bAutoSkillStatus;
	BYTE			m_bSelectedSkillIdx;
	LPEVENT			m_pkAutoSkill;
#endif

#ifdef ENABLE_MULTI_FARM_BLOCK
public:
	bool				GetRewardStatus() { return m_bmultiFarmStatus; }
	void				SetRewardStatus(bool bValue) { m_bmultiFarmStatus = bValue; }
	
protected:
	bool				m_bmultiFarmStatus;
#endif
};

template<typename... Args>
void CHARACTER::SendFormattedEventToClient(const std::string& eventName, Args&&... args) {
	SendEventToClient(eventName, std::forward<Args>(args)...);
}

template<typename... Args>
void CHARACTER::SendEventToClient(const std::string& eventName, Args&&... args) {
    std::ostringstream oss;
    oss << "event " << eventName;
    
    ((oss << " " << std::forward<Args>(args)), ...);
    
    std::string commandStr = oss.str();
    ChatPacket(CHAT_TYPE_COMMAND, "%s", commandStr.c_str());
}

ESex GET_SEX(LPCHARACTER ch);

#if defined(ENABLE_COLLECT_WINDOW) && defined(ENABLE_NOTIFICATION_SYSTEM)
void StartCollectNotifyTimer(LPCHARACTER ch, DWORD dwRemainSec, DWORD dwItemVnum);
void StopCollectNotifyTimer(LPCHARACTER ch, bool bNotify = false);
#endif

#endif
//martysama0134's ceqyqttoaf71vasf9t71218
