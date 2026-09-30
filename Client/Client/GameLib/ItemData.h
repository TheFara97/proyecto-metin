#pragma once

//        Icon, Model (droped on ground), Game Data

#include "../eterLib/GrpSubImage.h"
#include "../eterGrnLib/Thing.h"
#include "GameLibDefines.h"
#ifdef ENABLE_AURA_SYSTEM
#include "../EffectLib/EffectManager.h"
#endif
#include "../UserInterface/Locale_inc.h"

class CItemData
{
	public:
		enum
		{
			ITEM_NAME_MAX_LEN = 30,
			ITEM_LIMIT_MAX_NUM = 2,
			ITEM_VALUES_MAX_NUM = 6,
			ITEM_SMALL_DESCR_MAX_LEN = 256,
			ITEM_APPLY_MAX_NUM = 5,
			ITEM_SOCKET_MAX_NUM = 3,
#ifdef ENABLE_SHINING_SYSTEM
			ITEM_SHINING_MAX_COUNT = 3,
#endif
#ifdef __UNIMPLEMENTED__
			ITEM_PICK_MAX_LEVEL = 9,
			ITEM_ROD_MAX_LEVEL = 20,
#endif
		};

#ifdef ENABLE_SOULBIND_SYSTEM
		enum ESealBind
		{
			SEAL_BIND_FLAG_DROP,
			SEAL_BIND_FLAG_UPGRADE,
			SEAL_BIND_FLAG_SELL,
			SEAL_BIND_FLAG_ENCHANT,
			SEAL_BIND_FLAG_TRADE,
			SEAL_BIND_FLAG_UNSEAL,
			SEAL_BIND_FLAG_MAX,
		};

		enum ESealDate
		{
			SEAL_DATE_DEFAULT_TIMESTAMP = 0, // 2015/11/12 it's -1
			SEAL_DATE_UNLIMITED_TIMESTAMP = -1, // 2015/11/12 it doesn't exist
		};

		enum ESealItem
		{
			SEAL_ITEM_BINDING_VNUM = 50263,
			SEAL_ITEM_UNBINDING_VNUM = 50264,
		};
#endif

#ifdef __UNIMPLEMENTED__
		enum EPetData
		{
			EGG_USE_SUCCESS = 0,
			EGG_USE_FAILED_TIMEOVER = 2,
			EGG_USE_FAILED_BECAUSE_NAME = 1,
			GROWTH_PET_ITEM_USE_COOL_TIME = 1,
			PET_EGG_USE_TRUE = 0,
			PET_EGG_USE_FAILED_BECAUSE_TRADING = 1,
			PET_EGG_USE_FAILED_BECAUSE_SHOP_OPEN = 2,
			PET_EGG_USE_FAILED_BECAUSE_MALL_OPEN = 3,
			PET_EGG_USE_FAILED_BECAUSE_SAFEBOX_OPEN = 4,
			PET_HATCHING_MONEY = 100000,
			PET_NAME_MAX_SIZE = 20,
			PET_NAME_MIN_SIZE = 4,
		};
#endif

		enum EItemType
		{
			ITEM_TYPE_NONE,					//0
			ITEM_TYPE_WEAPON,				//1
			ITEM_TYPE_ARMOR,				//2
			ITEM_TYPE_USE,					//3
			ITEM_TYPE_AUTOUSE,				//4
			ITEM_TYPE_MATERIAL,				//5
			ITEM_TYPE_SPECIAL,				//6
			ITEM_TYPE_TOOL,					//7
			ITEM_TYPE_LOTTERY,				//8
			ITEM_TYPE_ELK,					//9
			ITEM_TYPE_METIN,				//10
			ITEM_TYPE_CONTAINER,			//11
			ITEM_TYPE_FISH,					//12
			ITEM_TYPE_ROD,					//13
			ITEM_TYPE_RESOURCE,				//14
			ITEM_TYPE_CAMPFIRE,				//15
			ITEM_TYPE_UNIQUE,				//16
			ITEM_TYPE_SKILLBOOK,			//17
			ITEM_TYPE_QUEST,				//18
			ITEM_TYPE_POLYMORPH,			//19
			ITEM_TYPE_TREASURE_BOX,			//20
			ITEM_TYPE_TREASURE_KEY,			//21
			ITEM_TYPE_SKILLFORGET,			//22
			ITEM_TYPE_GIFTBOX,				//23
			ITEM_TYPE_PICK,					//24
			ITEM_TYPE_HAIR,					//25
			ITEM_TYPE_TOTEM,				//26
			ITEM_TYPE_BLEND,				//27		
			ITEM_TYPE_COSTUME,				//28
			ITEM_TYPE_DS,					//29
			ITEM_TYPE_SPECIAL_DS,			//30
			ITEM_TYPE_EXTRACT,				//31
			ITEM_TYPE_SECONDARY_COIN,		//32
			ITEM_TYPE_RING,					//33
			ITEM_TYPE_BELT,					//34
			ITEM_TYPE_PET,					//35
#ifdef ENABLE_NEW_PET_SYSTEM
			ITEM_NEW_PET,					//36
			ITEM_NEW_PET_EQ,				//37
#endif
#ifdef TITLE_SYSTEM_BYLUZER
			ITEM_TYPE_TITLE,				//38
#endif
#ifdef ENABLE_CHEQUE_SYSTEM
			ITEM_TYPE_CHEQUE,				//39
#endif
#ifdef ENABLE_SYSTEM_RUNE
			ITEM_TYPE_RUNE,					//40
			ITEM_TYPE_RUNE_RED,				//41
			ITEM_TYPE_RUNE_BLUE,			//42
			ITEM_TYPE_RUNE_GREEN,			//43
			ITEM_TYPE_RUNE_YELLOW,			//44
			ITEM_TYPE_RUNE_BLACK,			//45
#endif
#ifdef ENABLE_ARTEFAKT_SYSTEM
			ITEM_TYPE_ARTEFAKT,				//35 // 46 ?
#endif
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
			ITEM_TYPE_MOUNT_EQUIPMENT,		//47
#endif
			ITEM_BUFF_EQ,					//48
			ITEM_TYPE_GACHA,				//49
#ifdef __ENABLE_TOGGLE_ITEMS__
			ITEM_TOGGLE,					//50
#endif
#ifdef ENABLE_MOUNT_SYSTEM
			ITEM_TYPE_MOUNT,	//51
#endif
#ifdef ENABLE_SKILL_COSTUME_SYSTEM
			ITEM_TYPE_SKILL_COSTUME,	//52
#endif
			ITEM_TYPE_GUILD_GUARD,	//53
#ifdef ENABLE_VIP_SYSTEM
			ITEM_TYPE_VIP,
#endif
			ITEM_TYPE_MAX_NUM,
		};

		enum EPetSubTypes
		{
			PET_EGG,
			PET_UPBRINGING,
			PET_BAG,
			PET_FEEDSTUFF,
			PET_SKILL,
			PET_SKILL_DEL_BOOK,
			PET_NAME_CHANGE,
			PET_EXPFOOD,
			PET_SKILL_ALL_DEL_BOOK,
			PET_EXPFOOD_PER,
			PET_ATTR_DETERMINE,
			PET_ATTR_CHANGE,
			PET_PAY,
			PET_PRIMIUM_FEEDSTUFF,
		};

		enum EWeaponSubTypes
		{
			WEAPON_SWORD, //0
			WEAPON_DAGGER,
			WEAPON_BOW, //2
			WEAPON_TWO_HANDED, //3
			WEAPON_BELL, //4
			WEAPON_FAN, //5
			WEAPON_ARROW, //6
			WEAPON_MOUNT_SPEAR, //7
#ifdef ENABLE_WOLFMAN_CHARACTER
			WEAPON_CLAW = 8, //8
#endif
#ifdef ENABLE_QUIVER_SYSTEM
			WEAPON_QUIVER = 8, //9
#endif
#ifdef __UNIMPLEMENTED__
			WEAPON_BOUQUET = 10, //10
#endif
			WEAPON_NUM_TYPES, //11 2015/11/12

			WEAPON_NONE = WEAPON_NUM_TYPES+1,
		};

		enum EMaterialSubTypes
		{
			MATERIAL_LEATHER,
			MATERIAL_BLOOD,
			MATERIAL_ROOT,
			MATERIAL_NEEDLE,
			MATERIAL_JEWEL,
			MATERIAL_DS_REFINE_NORMAL,
			MATERIAL_DS_REFINE_BLESSED,
			MATERIAL_DS_REFINE_HOLLY,
#if defined(ENABLE_DS_CHANGE_ATTR)
			MATERIAL_DS_CHANGE_ATTR,
#endif
		};

		enum EArmorSubTypes
		{
			ARMOR_BODY,
			ARMOR_HEAD,
			ARMOR_SHIELD,
			ARMOR_WRIST,
			ARMOR_FOOTS,
		    ARMOR_NECK,
			ARMOR_EAR,
#ifdef ENABLE_GLOVE_SYSTEM
			ARMOR_GLOVE,
#endif
#ifdef __PENDANT__
			ARMOR_PENDANT_FIRE,
			ARMOR_PENDANT_ICE,
			ARMOR_PENDANT_THUNDER,
			ARMOR_PENDANT_WIND,
			ARMOR_PENDANT_EARTH,
			ARMOR_PENDANT_DARK,
			ARMOR_PENDANT_SOUL,
#endif
			ARMOR_NUM_TYPES
		};
#ifdef ENABLE_ARTEFAKT_SYSTEM
		enum ERuneSubTypes
		{
			ARTEFAKT_1,
			ARTEFAKT_2,
			ARTEFAKT_3,
			ARTEFAKT_4,
			ARTEFAKT_5,
			ARTEFAKT_6,
			ARTEFAKT_7,
			ARTEFAKT_8,
			ARTEFAKT_9,
			ARTEFAKT_10,
			ARTEFAKT_11,
			ARTEFAKT_12,
			ARTEFAKT_13,
			ARTEFAKT_14,
		};
#endif
		enum EMountEqSubTypes
		{
			MOUNT_SHOE,
			MOUNT_SADDLE,
			MOUNT_ARMOR,
		};
		enum ECostumeSubTypes
		{
			COSTUME_BODY,
			COSTUME_HAIR,
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
			COSTUME_MOUNT	= 2,		//2
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			COSTUME_ACCE	= 3,		//3
#endif
#ifdef ENABLE_STOLE_COSTUME
			COSTUME_STOLE	= 4,			// 4
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
			COSTUME_WEAPON	= 5,		//5
#endif
#ifdef ENABLE_AURA_SYSTEM
			COSTUME_AURA	= 6,
#endif
#ifdef ENABLE_COSTUME_EMBLEMAT
			COSTUME_EMBLEMAT = 7,
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
			BUFF_COSTUME_WEAPON = 8,
			BUFF_COSTUME_BODY = 9,
			BUFF_COSTUME_HAIR = 10,
#endif
			COSTUME_NUM_TYPES,
		};

		enum EUseSubTypes
		{
			USE_POTION,					// 0
			USE_TALISMAN,
			USE_TUNING,
			USE_MOVE,
			USE_TREASURE_BOX,
			USE_MONEYBAG,
			USE_BAIT,
			USE_ABILITY_UP,
			USE_AFFECT,
			USE_CREATE_STONE,
			USE_SPECIAL,				// 10
			USE_POTION_NODELAY,
			USE_CLEAR,
			USE_INVISIBILITY,
			USE_DETACHMENT,
			USE_BUCKET,
			USE_POTION_CONTINUE,
			USE_CLEAN_SOCKET,
			USE_CHANGE_ATTRIBUTE,
			USE_ADD_ATTRIBUTE,
			USE_ADD_ACCESSORY_SOCKET,	// 20
			USE_PUT_INTO_ACCESSORY_SOCKET,
			USE_ADD_ATTRIBUTE2,
			USE_RECIPE,
			USE_CHANGE_ATTRIBUTE2,
			USE_BIND,
			USE_UNBIND,
			USE_TIME_CHARGE_PER,
			USE_TIME_CHARGE_FIX,				// 28
			USE_PUT_INTO_BELT_SOCKET,
			USE_PUT_INTO_RING_SOCKET,
#ifdef ENABLE_USE_COSTUME_ATTR
			USE_CHANGE_COSTUME_ATTR,			// 31
			USE_RESET_COSTUME_ATTR,				// 32
#endif
#ifdef ENABLE_SPECIAL_BONUS
			USE_ADD_ATTRIBUTE_SPECIAL_1,		// 33
#endif
#ifdef ENABLE_COLLECTIONS_SYSTEM
			USE_COLLECTION_SCROLL,
#endif
			USE_ADD_IS_BONUS,
			USE_ADD_NEW_BONUS,
			USE_ADD_EXTRA_BONUS,
#ifdef __ENABLE_POLYMORPH_SYSTEM__
			USE_POLY_SKIN,
#endif
			USE_ALCHEMY_CHANGER,
			USE_SASH_CLEANER,
			USE_FISHING_ROD_ADD_ATTRIBUTE,
#ifdef ENABLE_MOUNT_SYSTEM
			USE_COMPANION_EXP_VOUCHER,
#endif
		};

		enum ENewEquipmentSubTypes
		{
#ifdef ENABLE_GLOVE_SYSTEM
			EQUIPMENT_NEW_GLOVE,
#endif
		};

		enum EDragonSoulSubType
		{
			DS_SLOT1,
			DS_SLOT2,
			DS_SLOT3,
			DS_SLOT4,
			DS_SLOT5,
			DS_SLOT6,
			DS_SLOT_NUM_TYPES = 6,
		};

		enum EMetinSubTypes
		{
			METIN_NORMAL,
			METIN_GOLD,
		};
		
		enum ESkillCostumeSubTypes
		{
			SKILL_COSTUME_1,
			SKILL_COSTUME_2,
			SKILL_COSTUME_3,
			SKILL_COSTUME_4,
			SKILL_COSTUME_5,
			SKILL_COSTUME_6,
		};

		enum ELimitTypes
		{
			LIMIT_NONE,

			LIMIT_LEVEL,
			LIMIT_STR,
			LIMIT_DEX,
			LIMIT_INT,
			LIMIT_CON,
			LIMIT_PCBANG,

			LIMIT_REAL_TIME,

			LIMIT_REAL_TIME_START_FIRST_USE,

			LIMIT_TIMER_BASED_ON_WEAR,
#ifdef ENABLE_SECONDARY_LEVEL
			LIMIT_SECONDARY_LEVEL,
#endif
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
			LIMIT_ENLIGHT_LEVEL,
#endif
#ifdef ENABLE_MOUNT_SYSTEM
			LIMIT_MOUNT_LEVEL = 11,
#endif

			LIMIT_MAX_NUM
		};

		enum EItemAntiFlag
		{
			ITEM_ANTIFLAG_FEMALE        = (1 << 0),
			ITEM_ANTIFLAG_MALE          = (1 << 1),
			ITEM_ANTIFLAG_WARRIOR       = (1 << 2),
			ITEM_ANTIFLAG_ASSASSIN      = (1 << 3),
			ITEM_ANTIFLAG_SURA          = (1 << 4),
			ITEM_ANTIFLAG_SHAMAN        = (1 << 5),
			ITEM_ANTIFLAG_GET           = (1 << 6),
			ITEM_ANTIFLAG_DROP          = (1 << 7),
			ITEM_ANTIFLAG_SELL          = (1 << 8),
			ITEM_ANTIFLAG_EMPIRE_A      = (1 << 9),
			ITEM_ANTIFLAG_EMPIRE_B      = (1 << 10),
			ITEM_ANTIFLAG_EMPIRE_R      = (1 << 11),
			ITEM_ANTIFLAG_SAVE          = (1 << 12),
			ITEM_ANTIFLAG_GIVE          = (1 << 13),
			ITEM_ANTIFLAG_PKDROP        = (1 << 14),
			ITEM_ANTIFLAG_STACK         = (1 << 15),
			ITEM_ANTIFLAG_MYSHOP        = (1 << 16),
			ITEM_ANTIFLAG_SAFEBOX		= (1 << 17),
#ifdef ENABLE_WOLFMAN_CHARACTER
			ITEM_ANTIFLAG_WOLFMAN		= (1 << 18),
#endif
		};

		enum EItemFlag
		{
			ITEM_FLAG_REFINEABLE        = (1 << 0),
			ITEM_FLAG_SAVE              = (1 << 1),
			ITEM_FLAG_STACKABLE         = (1 << 2),
			ITEM_FLAG_COUNT_PER_1GOLD   = (1 << 3),
			ITEM_FLAG_SLOW_QUERY        = (1 << 4),
			ITEM_FLAG_RARE              = (1 << 5),
			ITEM_FLAG_UNIQUE            = (1 << 6),
			ITEM_FLAG_MAKECOUNT			= (1 << 7),
			ITEM_FLAG_IRREMOVABLE		= (1 << 8),
			ITEM_FLAG_CONFIRM_WHEN_USE	= (1 << 9),
			ITEM_FLAG_QUEST_USE         = (1 << 10),
			ITEM_FLAG_QUEST_USE_MULTIPLE= (1 << 11),
			ITEM_FLAG_QUEST_GIVE		= (1 << 12),    // UNUSED03
			ITEM_FLAG_LOG               = (1 << 13),
			ITEM_FLAG_APPLICABLE		= (1 << 14),
		};

		enum EWearPositions
		{
			WEAR_BODY,          // 0
			WEAR_HEAD,          // 1
			WEAR_FOOTS,         // 2
			WEAR_WRIST,         // 3
			WEAR_WEAPON,        // 4
			WEAR_NECK,          // 5
			WEAR_EAR,           // 6
			WEAR_UNIQUE1,       // 7
			WEAR_UNIQUE2,       // 8
			WEAR_ARROW,         // 9
			WEAR_SHIELD,        // 10

			WEAR_ABILITY1,  // 11
			WEAR_ABILITY2,  // 12
			WEAR_ABILITY3,  // 13
			WEAR_ABILITY4,  // 14
			WEAR_ABILITY5,  // 15
			WEAR_ABILITY6,  // 16
			WEAR_ABILITY7,  // 17
			WEAR_ABILITY8,  // 18
			WEAR_COSTUME_BODY,	// 19
			WEAR_COSTUME_HAIR,	// 20
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
			WEAR_COSTUME_MOUNT, // 21
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			WEAR_COSTUME_ACCE, // 22
#endif
#ifdef ENABLE_STOLE_COSTUME
			WEAR_COSTUME_STOLE,// 23
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
			WEAR_COSTUME_WEAPON,// 24
#endif
#ifdef ENABLE_AURA_SYSTEM
			WEAR_COSTUME_AURA,			
#endif
#ifdef ENABLE_COSTUME_EMBLEMAT
			WEAR_COSTUME_EMBLEMAT,	// 28
#endif
			WEAR_RING1,			//25
			WEAR_RING2,
			WEAR_BELT,

#ifdef ENABLE_NEW_PET_SYSTEM
			WEAR_NEW_PET,
			WEAR_NEW_PET_EQ0,
			WEAR_NEW_PET_EQ1,
			WEAR_NEW_PET_EQ2,
#endif
#ifdef ENABLE_GLOVE_SYSTEM
			WEAR_GLOVE,
#endif
#ifdef ENABLE_SYSTEM_RUNE
			WEAR_RUNE,
			WEAR_RUNE_RED,
			WEAR_RUNE_BLUE,
			WEAR_RUNE_GREEN,
			WEAR_RUNE_YELLOW,
			WEAR_RUNE_BLACK,
#endif
#ifdef ENABLE_ARTEFAKT_SYSTEM
			WEAR_ARTEFAKT1,
			WEAR_ARTEFAKT2,
			WEAR_ARTEFAKT3,
			WEAR_ARTEFAKT4,
			WEAR_ARTEFAKT5,
			WEAR_ARTEFAKT6,
			WEAR_ARTEFAKT7,
			WEAR_ARTEFAKT8,
			WEAR_ARTEFAKT9,
			WEAR_ARTEFAKT10,
			WEAR_ARTEFAKT11,
			WEAR_ARTEFAKT12,
#endif
//#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
//			WEAR_MOUNT_EQUIPMENT,
//#endif	
#ifdef ENABLE_MOUNT_SYSTEM
			WEAR_MOUNT_EQUIPMENT_SHOE,
			WEAR_MOUNT_EQUIPMENT_SADDLE,
			WEAR_MOUNT_EQUIPMENT_ARMOR,
#endif
#ifdef __PENDANT__
			WEAR_PENDANT_FIRE,
			WEAR_PENDANT_ICE,
			WEAR_PENDANT_THUNDER,
			WEAR_PENDANT_WIND,
			WEAR_PENDANT_DARK,
			WEAR_PENDANT_EARTH,
			WEAR_PENDANT_SOUL_FIRST,
			WEAR_PENDANT_SOUL_SECOND,
#endif
			WEAR_ARTEFAKT13,
			WEAR_ARTEFAKT14,
			WEAR_NEW_PET_EQ3,
			WEAR_NEW_PET_EQ4,
			WEAR_SKILL_COSTUME_1,
			WEAR_SKILL_COSTUME_2,
			WEAR_SKILL_COSTUME_3,
			WEAR_SKILL_COSTUME_4,
			WEAR_SKILL_COSTUME_5,
			WEAR_SKILL_COSTUME_6,
			
			WEAR_MAX_NUM = 79,
		};


#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
		enum EBuffWindowSlots
		{
			BUFF_WEAR_WEAPON,				// 0
			BUFF_WEAR_HEAD,					// 1
			BUFF_WEAR_BODY,					// 2
			BUFF_WEAR_UNIQUE,				// 3

			BUFF_SLOT_SKILLBOOK,			// 4
			BUFF_SLOT_SKILLBOOK_COMMON_1,	// 5
			BUFF_SLOT_SKILLBOOK_COMMON_2,	// 6

			BUFF_SLOT_CHANGE_NAME,			// 7
			BUFF_SLOT_CHANGE_SEX,			// 8
			BUFF_SLOT_RESET_SKILL,			// 9

			BUFF_WINDOW_SLOT_MAX_NUM,
		};
#endif

		enum EItemWearableFlag
		{
			WEARABLE_BODY       = (1 << 0),
			WEARABLE_HEAD       = (1 << 1),
			WEARABLE_FOOTS      = (1 << 2),
			WEARABLE_WRIST      = (1 << 3),
			WEARABLE_WEAPON     = (1 << 4),
			WEARABLE_NECK       = (1 << 5),
			WEARABLE_EAR        = (1 << 6),
			WEARABLE_UNIQUE     = (1 << 7),
			WEARABLE_SHIELD     = (1 << 8),
			WEARABLE_ARROW      = (1 << 9),
#ifdef ENABLE_GLOVE_SYSTEM
			WEARABLE_GLOVE		= (1 << 10),
#endif
		};

		enum EApplyTypes
		{
			APPLY_NONE,                 // 0
			APPLY_MAX_HP,               // 1
			APPLY_MAX_SP,               // 2
			APPLY_CON,                  // 3
			APPLY_INT,                  // 4
			APPLY_STR,                  // 5
			APPLY_DEX,                  // 6
			APPLY_ATT_SPEED,            // 7
			APPLY_MOV_SPEED,            // 8
			APPLY_CAST_SPEED,           // 9
			APPLY_HP_REGEN,             // 10
			APPLY_SP_REGEN,             // 11
			APPLY_POISON_PCT,           // 12
			APPLY_STUN_PCT,             // 13
			APPLY_SLOW_PCT,             // 14
			APPLY_CRITICAL_PCT,         // 15
			APPLY_PENETRATE_PCT,        // 16
			APPLY_ATTBONUS_HUMAN,       // 17
			APPLY_ATTBONUS_ANIMAL,      // 18
			APPLY_ATTBONUS_ORC,         // 19
			APPLY_ATTBONUS_MILGYO,      // 20
			APPLY_ATTBONUS_UNDEAD,      // 21
			APPLY_ATTBONUS_DEVIL,       // 22
			APPLY_STEAL_HP,             // 23
			APPLY_STEAL_SP,             // 24
			APPLY_MANA_BURN_PCT,        // 25
			APPLY_DAMAGE_SP_RECOVER,    // 26
			APPLY_BLOCK,                // 27
			APPLY_DODGE,                // 28
			APPLY_RESIST_SWORD,         // 29
			APPLY_RESIST_TWOHAND,       // 30
			APPLY_RESIST_DAGGER,        // 31
			APPLY_RESIST_BELL,          // 32
			APPLY_RESIST_FAN,           // 33
			APPLY_RESIST_BOW,           // 34
			APPLY_RESIST_FIRE,          // 35
			APPLY_RESIST_ELEC,          // 36
			APPLY_RESIST_MAGIC,         // 37
			APPLY_RESIST_WIND,          // 38
			APPLY_REFLECT_MELEE,        // 39
			APPLY_REFLECT_CURSE,        // 40
			APPLY_POISON_REDUCE,        // 41
			APPLY_KILL_SP_RECOVER,      // 42
			APPLY_EXP_DOUBLE_BONUS,     // 43
			APPLY_GOLD_DOUBLE_BONUS,    // 44
			APPLY_ITEM_DROP_BONUS,      // 45
			APPLY_POTION_BONUS,         // 46
			APPLY_KILL_HP_RECOVER,      // 47
			APPLY_IMMUNE_STUN,          // 48
			APPLY_IMMUNE_SLOW,          // 49
			APPLY_IMMUNE_FALL,          // 50
			APPLY_SKILL,                // 51
			APPLY_BOW_DISTANCE,         // 52
			APPLY_ATT_GRADE_BONUS,            // 53
			APPLY_DEF_GRADE_BONUS,            // 54
			APPLY_MAGIC_ATT_GRADE,      // 55
			APPLY_MAGIC_DEF_GRADE,      // 56
			APPLY_CURSE_PCT,            // 57
			APPLY_MAX_STAMINA,			// 58
			APPLY_ATT_BONUS_TO_WARRIOR, // 59
			APPLY_ATT_BONUS_TO_ASSASSIN,// 60
			APPLY_ATT_BONUS_TO_SURA,    // 61
			APPLY_ATT_BONUS_TO_SHAMAN,  // 62
			APPLY_ATT_BONUS_TO_MONSTER, // 63
			APPLY_MALL_ATTBONUS,
			APPLY_MALL_DEFBONUS,
			APPLY_MALL_EXPBONUS,
			APPLY_MALL_ITEMBONUS,
			APPLY_MALL_GOLDBONUS,
			APPLY_MAX_HP_PCT,
			APPLY_MAX_SP_PCT,
			APPLY_SKILL_DAMAGE_BONUS,
			APPLY_NORMAL_HIT_DAMAGE_BONUS,
			APPLY_SKILL_DEFEND_BONUS,
			APPLY_NORMAL_HIT_DEFEND_BONUS,
			APPLY_EXTRACT_HP_PCT,		//75
			APPLY_PC_BANG_EXP_BONUS,		//76
			APPLY_PC_BANG_DROP_BONUS,		//77
			APPLY_RESIST_WARRIOR,			//78
			APPLY_RESIST_ASSASSIN,			//79
			APPLY_RESIST_SURA,				//80
			APPLY_RESIST_SHAMAN,			//81
			APPLY_ENERGY,					//82
			APPLY_DEF_GRADE,
			APPLY_COSTUME_ATTR_BONUS,
			APPLY_MAGIC_ATTBONUS_PER,
			APPLY_MELEE_MAGIC_ATTBONUS_PER,

			APPLY_RESIST_ICE,
			APPLY_RESIST_EARTH,
			APPLY_RESIST_DARK,

			APPLY_ANTI_CRITICAL_PCT,
			APPLY_ANTI_PENETRATE_PCT,

#ifdef ENABLE_WOLFMAN_CHARACTER
			APPLY_BLEEDING_REDUCE = 92,	//92
			APPLY_BLEEDING_PCT = 93,	//93
			APPLY_ATT_BONUS_TO_WOLFMAN = 94,	//94
			APPLY_RESIST_WOLFMAN = 95,	//95
			APPLY_RESIST_CLAW = 96,	//96
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			APPLY_ACCEDRAIN_RATE = 97,	//97
#endif

#ifdef ENABLE_GLOVE_SYSTEM
			APPLY_RANDOM,
#endif

#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
			APPLY_RESIST_MAGIC_REDUCTION = 98,	//98
#endif

			APPLY_ENCHANT_ELECT = 99,
			APPLY_ENCHANT_FIRE = 100,
			APPLY_ENCHANT_ICE = 101,
			APPLY_ENCHANT_WIND = 102,
			APPLY_ENCHANT_EARTH = 103,
			APPLY_ENCHANT_DARK = 104,
			APPLY_RESIST_HUMAN,
			APPLY_ATTBONUS_BOSS,
			APPLY_ATTBONUS_WLADCA,
			APPLY_ATTBONUS_STONE,
			APPLY_RESIST_BOSS,
			APPLY_RESIST_WLADCA,
			APPLY_RESIST_MONSTER,
			APPLY_STAT_BONUS,
			APPLY_ATTBONUS_DUNGEON,
			APPLY_ATTBONUS_LEGENDA,
			APPLY_ATTBONUS_KLASY,
			APPLY_RESIST_KLASY,
			APPLY_DMG_BONUS,
			APPLY_FINAL_DMG_BONUS,
			APPLY_MINING_SUCCESS_CHANCE,
			APPLY_MINING_MORE_ORES_CHANCE,
			APPLY_MINING_LESS_MINE_TIME,
			APPLY_MINING_MORE_POINTS,
			APPLY_FISHING_INSTANT_CATCH_CHANCE,
			APPLY_FISHING_BIGGER_FISH_CHANCE,
			APPLY_FISHING_NEXT_SECTION_CHANCE,
			APPLY_FISHING_MORE_YANG_MULTIPLIER,
			APPLY_FISHING_MORE_POINTS_CHANCE,
			APPLY_FISHING_SUCCESS_CHANCE,
			APPLY_ATTBONUS_PVM_SKILL,
			APPLY_ATTBONUS_ELEMENT,
			APPLY_DEFBONUS_ELEMENT,
			APPLY_DOUBLE_LOOT_SUMMER,
			APPLY_DOUBLE_LOOT_DROP,
			APPLY_ATTBONUS_PVM_STR,
			MAX_APPLY_NUM = 140,
		};

		enum EImmuneFlags
		{
			IMMUNE_PARA         = (1 << 0),
			IMMUNE_CURSE        = (1 << 1),
			IMMUNE_STUN         = (1 << 2),
			IMMUNE_SLEEP        = (1 << 3),
			IMMUNE_SLOW         = (1 << 4),
			IMMUNE_POISON       = (1 << 5),
			IMMUNE_TERROR       = (1 << 6),
		};

#ifdef __ENABLE_TOGGLE_ITEMS__
	enum eToggleItemTypes {
		TOGGLE_AUTO_RECOVERY_HP,
		TOGGLE_AUTO_RECOVERY_SP,
		TOGGLE_AFFECT,
#ifdef __ENABLE_AUTO_PICKUP__
		TOGGLE_PICKUP,
#endif
	};
#endif

#pragma pack(push)
#pragma pack(1)
		typedef struct SItemLimit
		{
			BYTE        bType;
			long        lValue;
		} TItemLimit;

		typedef struct SItemApply
		{
			BYTE        bType;
			long        lValue;
		} TItemApply;

		typedef struct SItemTable_r152
		{
			DWORD       dwVnum;
			char        szName[ITEM_NAME_MAX_LEN + 1];
			char        szLocaleName[ITEM_NAME_MAX_LEN + 1];
			BYTE        bType;
			BYTE        bSubType;

			BYTE        bWeight;
			BYTE        bSize;

			DWORD       dwAntiFlags;
			DWORD       dwFlags;
			DWORD       dwWearFlags;
			DWORD       dwImmuneFlag;

			DWORD       dwIBuyItemPrice;
			DWORD		dwISellItemPrice;

			TItemLimit  aLimits[ITEM_LIMIT_MAX_NUM];
			TItemApply  aApplies[ITEM_APPLY_MAX_NUM];
			long        alValues[ITEM_VALUES_MAX_NUM];
			long        alSockets[ITEM_SOCKET_MAX_NUM];
			DWORD       dwRefinedVnum;
			WORD		wRefineSet;
			BYTE        bAlterToMagicItemPct;
			BYTE		bSpecular;
			BYTE        bGainSocketPct;
#ifdef WJ_SPLIT_INVENTORY_SYSTEM
			BYTE		SpecialInvType;
			BYTE		CanUseInSpecialInv;
#endif
		} TItemTable_r152;

		typedef struct SItemTable_r156
		{
			DWORD       dwVnum;
			DWORD       dwVnumRange;
			char        szName[ITEM_NAME_MAX_LEN + 1];
			char        szLocaleName[ITEM_NAME_MAX_LEN + 1];
			BYTE        bType;
			BYTE        bSubType;

			BYTE        bWeight;
			BYTE        bSize;

			DWORD       dwAntiFlags;
			DWORD       dwFlags;
			DWORD       dwWearFlags;
			DWORD       dwImmuneFlag;

			DWORD       dwIBuyItemPrice;
			DWORD		dwISellItemPrice;

			TItemLimit  aLimits[ITEM_LIMIT_MAX_NUM];
			TItemApply  aApplies[ITEM_APPLY_MAX_NUM];
			long        alValues[ITEM_VALUES_MAX_NUM];
			long        alSockets[ITEM_SOCKET_MAX_NUM];
			DWORD       dwRefinedVnum;
			WORD		wRefineSet;
			BYTE        bAlterToMagicItemPct;
			BYTE		bSpecular;
			BYTE        bGainSocketPct;
#ifdef WJ_SPLIT_INVENTORY_SYSTEM
			BYTE		SpecialInvType;
			BYTE		CanUseInSpecialInv;
#endif
		} TItemTable_r156;

		typedef struct SItemTable_r158
		{
			DWORD       dwVnum;
			DWORD       dwVnumRange;
			char        szName[ITEM_NAME_MAX_LEN + 1];
			char        szLocaleName[ITEM_NAME_MAX_LEN + 1];
			BYTE        bType;
			BYTE        bSubType;

			BYTE        bWeight;
			BYTE        bSize;

			DWORD       dwAntiFlags;
			DWORD       dwFlags;
			DWORD       dwWearFlags;
			DWORD       dwImmuneFlag;

			DWORD       dwIBuyItemPrice;
			DWORD		dwISellItemPrice;

			TItemLimit  aLimits[ITEM_LIMIT_MAX_NUM];
			TItemApply  aApplies[ITEM_APPLY_MAX_NUM];
			long        alValues[ITEM_VALUES_MAX_NUM];
			long        alSockets[ITEM_SOCKET_MAX_NUM];
			DWORD       dwRefinedVnum;
			WORD		wRefineSet;
			BYTE        bAlterToMagicItemPct;
			BYTE		bSpecular;
			BYTE        bGainSocketPct;
			WORD		wWearableFlag;
#ifdef WJ_SPLIT_INVENTORY_SYSTEM
			BYTE		SpecialInvType;
			BYTE		CanUseInSpecialInv;
#endif
		} TItemTable_r158;

		typedef SItemTable_r156 SItemTable, TItemTable;

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		struct SScaleInfo
		{
			float	fScaleX, fScaleY, fScaleZ;
			float	fPositionX, fPositionY, fPositionZ;
		};

		typedef struct SScaleTable
		{
			SScaleInfo	tInfo[10];
		} TScaleTable;
#endif

#ifdef ENABLE_PROTOSTRUCT_AUTODETECT
		typedef struct SItemTableAll
		{
			static bool IsValidStruct(DWORD structSize)
			{
				switch (structSize)
				{
					case sizeof(TItemTable_r152):
					case sizeof(TItemTable_r156):
					case sizeof(TItemTable_r158):
						return true;
						break;
				}
				return false;
			}

			static void Process(void* obj, DWORD structSize, DWORD i, CItemData::TItemTable& t)
			{
				#define ITABLE_COPY_STR(x) strncpy_s(t.##x##, sizeof(t.##x##), r.##x##, _TRUNCATE)
				#define ITABLE_COPY_INT(x) t.##x## = r.##x
				#define ITABLE_ZERO_STR(x) memset(t.##x##, 0, sizeof(t.##x##));
				#define ITABLE_ZERO_INT(x) t.##x## = 0
				#define ITABLE_COUNT(x) _countof(t.##x##)
				#define ITABLE_PROCESS(len)\
					CItemData::TItemTable_r##len## & r = *((CItemData::TItemTable_r##len## *) obj + i);\
					ITABLE_COPY_INT(dwVnum);\
					ITABLE_COPY_STR(szName);\
					ITABLE_COPY_STR(szLocaleName);\
					ITABLE_COPY_INT(bType);\
					ITABLE_COPY_INT(bSubType);\
					ITABLE_COPY_INT(bWeight);\
					ITABLE_COPY_INT(bSize);\
					ITABLE_COPY_INT(dwAntiFlags);\
					ITABLE_COPY_INT(dwFlags);\
					ITABLE_COPY_INT(dwWearFlags);\
					ITABLE_COPY_INT(dwImmuneFlag);\
					ITABLE_COPY_INT(dwIBuyItemPrice);\
					ITABLE_COPY_INT(dwISellItemPrice);\
					for (size_t i=0; i<ITABLE_COUNT(aLimits); ++i)\
					{\
						ITABLE_COPY_INT(aLimits[i].bType);\
						ITABLE_COPY_INT(aLimits[i].lValue);\
					}\
					for (size_t i=0; i<CItemData::ITEM_APPLY_MAX_NUM; ++i)\
					{\
						ITABLE_COPY_INT(aApplies[i].bType);\
						ITABLE_COPY_INT(aApplies[i].lValue);\
					}\
					for (size_t i=0; i<CItemData::ITEM_VALUES_MAX_NUM; ++i)\
					{\
						ITABLE_COPY_INT(alValues[i]);\
					}\
					for (size_t i=0; i<CItemData::ITEM_SOCKET_MAX_NUM; ++i)\
					{\
						ITABLE_COPY_INT(alSockets[i]);\
					}\
					ITABLE_COPY_INT(dwRefinedVnum);\
					ITABLE_COPY_INT(wRefineSet);\
					ITABLE_COPY_INT(bAlterToMagicItemPct);\
					ITABLE_COPY_INT(bSpecular);\
					ITABLE_COPY_INT(bGainSocketPct);\
					ITABLE_COPY_INT(SpecialInvType);\
					ITABLE_COPY_INT(CanUseInSpecialInv);

				switch (structSize)
				{
					case sizeof(TItemTable_r152):
						{
							ITABLE_PROCESS(152);
							ITABLE_ZERO_INT(dwVnumRange);
						}
						break;
					case sizeof(TItemTable_r156):
						{
							ITABLE_PROCESS(156);
							ITABLE_COPY_INT(dwVnumRange);
						}
						break;
					case sizeof(TItemTable_r158):
						{
							ITABLE_PROCESS(158);
							ITABLE_COPY_INT(dwVnumRange);
						}
						break;
				}
			}
		} TItemTableAll;
#endif //ENABLE_PROTOSTRUCT_AUTODETECT

#pragma pack(pop)

	public:
		CItemData();
		virtual ~CItemData();

		void Clear();
		void SetSummary(const std::string& c_rstSumm);
		void SetDescription(const std::string& c_rstDesc);

		CGraphicThing * GetModelThing();
		CGraphicThing * GetSubModelThing();
		CGraphicThing * GetDropModelThing();
		CGraphicSubImage * GetIconImage();

		DWORD GetLODModelThingCount();
		BOOL GetLODModelThingPointer(DWORD dwIndex, CGraphicThing ** ppModelThing);

		DWORD GetAttachingDataCount();
		BOOL GetCollisionDataPointer(DWORD dwIndex, const NRaceData::TAttachingData ** c_ppAttachingData);
		BOOL GetAttachingDataPointer(DWORD dwIndex, const NRaceData::TAttachingData ** c_ppAttachingData);

		/////
		const TItemTable*	GetTable() const;
		DWORD GetIndex() const;
		const char * GetName() const;
		const char * GetDescription() const;
		const char * GetSummary() const;
		BYTE GetType() const;
		BYTE GetSubType() const;
		UINT GetRefine() const;
		const char* GetUseTypeString() const;
		DWORD GetWeaponType() const;
		BYTE GetSize() const;
		BOOL IsAntiFlag(DWORD dwFlag) const;
		BOOL IsFlag(DWORD dwFlag) const;
		BOOL IsWearableFlag(DWORD dwFlag) const;
		BOOL HasNextGrade() const;
#if defined(ENABLE_WIKI) || defined(INSIDE_RENDER)
		WORD GetRefinedSet() const;
		WORD GetRefinedVnum() const;
#endif
		DWORD GetWearFlags() const;
		DWORD GetIBuyItemPrice() const;
		DWORD GetISellItemPrice() const;
		BOOL GetLimit(BYTE byIndex, TItemLimit * pItemLimit) const;
		BOOL GetApply(BYTE byIndex, TItemApply * pItemApply) const;
		long GetValue(BYTE byIndex) const;
		long GetSocket(BYTE byIndex) const;
		long SetSocket(BYTE byIndex,DWORD value);
		int GetSocketCount() const;
		DWORD GetIconNumber() const;
#ifdef WJ_SPLIT_INVENTORY_SYSTEM
		BYTE GetSpecialInvType() const;
		BYTE CanUseIn_SpecialInv() const;
#endif

		UINT	GetSpecularPoweru() const;
		float	GetSpecularPowerf() const;

		/////

		BOOL IsEquipment() const;

		void SetName(const std::string& stName)
		{
			m_strName = stName;
		}
		/////

		//BOOL LoadItemData(const char * c_szFileName);
		void SetDefaultItemData(const char * c_szIconFileName, const char * c_szModelFileName  = NULL);
		void SetItemTableData(TItemTable * pItemTable);

#ifdef __EFEKT_SZARFY__
	protected:
		DWORD m_dwAcceEffectID;
	
	public:
		void SetAcceEffectID(const char* szAcceEffectPath);
		DWORD GetAcceEffectID() const { return m_dwAcceEffectID; }
#endif


#ifdef ENABLE_AURA_SYSTEM
	protected:
		DWORD m_dwAuraEffectID;
	public:
		void SetAuraEffectID(const char* szAuraEffectPath);
		DWORD GetAuraEffectID() const { return m_dwAuraEffectID; }

		enum EAuraMisc
		{
			AURA_GRADE_MAX_NUM = 6,
		};
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		void SetItemScale(const std::string strJob, const std::string strSex, const std::string strScaleX, const std::string strScaleY, const std::string strScaleZ, const std::string strPositionX, const std::string strPositionY, const std::string strPositionZ);
		bool GetItemScale(DWORD dwPos, float & fScaleX, float & fScaleY, float & fScaleZ, float & fPositionX, float & fPositionY, float & fPositionZ);
#endif
#ifdef ENABLE_SHINING_SYSTEM
		typedef struct SItemShiningTable {
			char szShinings[ITEM_SHINING_MAX_COUNT][256];
		public:
			//Checking if any shining is set for this item.
			bool Any() const
			{
				for (int i = 0; i < CItemData::ITEM_SHINING_MAX_COUNT; i++)
				{
					if (strcmp(szShinings[i], ""))
					{
						return true;
					}
				}
				return false;
			}
		} TItemShiningTable;
		void SetItemShiningTableData(BYTE bIndex, const char* szEffectname);
		CItemData::TItemShiningTable GetItemShiningTable() { return m_ItemShiningTable; }
#endif
	protected:
		void __LoadFiles();
		void __SetIconImage(const char * c_szFileName);

	protected:
		std::string m_strModelFileName;
		std::string m_strSubModelFileName;
		std::string m_strDropModelFileName;
		std::string m_strIconFileName;
		std::string m_strDescription;
		std::string m_strSummary;
		std::vector<std::string> m_strLODModelFileNameVector;

		CGraphicThing * m_pModelThing;
		CGraphicThing * m_pSubModelThing;
		CGraphicThing * m_pDropModelThing;
		CGraphicSubImage * m_pIconImage;
		std::vector<CGraphicThing *> m_pLODModelThingVector;

		NRaceData::TAttachingDataVector m_AttachingDataVector;
		DWORD		m_dwVnum;
		TItemTable m_ItemTable;
		std::string m_strName;

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		TScaleTable	m_ScaleTable;
#endif
#ifdef ENABLE_SHINING_SYSTEM
		TItemShiningTable m_ItemShiningTable;
#endif
#ifdef ENABLE_COSTUME_EMBLEMAT
	protected:
		DWORD m_dwEmblematEffectID;
	public:
		void SetEmblematEffectID(const char* szEmblematEffectPath);
		DWORD GetEmblematEffectID() const { return m_dwEmblematEffectID; }
#endif
	public:
		static void DestroySystem();

		static CItemData* New();
		static void Delete(CItemData* pkItemData);

		static CDynamicPool<CItemData>		ms_kPool;
};
//martysama0134's ceqyqttoaf71vasf9t71218
