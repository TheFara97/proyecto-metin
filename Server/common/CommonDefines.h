#pragma once

#define ENABLE_REDUCED_ENTITY_VIEW
#define __TEAM_MEMBER_STATUS__

#define __BL_CLIENT_LOCALE_STRING__

#define CHECK_CLIENT_VERSION					//-> Check client version
#ifdef CHECK_CLIENT_VERSION
#	define CURRENT_VERSION 15
#endif
#define ENABLE_ENITY_CHARACTER_FIX

#define __ENABLE_ITEM_TOGGLE__
#ifdef __ENABLE_ITEM_TOGGLE__
	#define __ENABLE_AUTO_PICKUP__
#endif

/// Version 1.1
#define __CANNOT_DEAL_DAMAGE_TO_PLAYERS_ON_POLYMOPH__
#define __RESET_PLAYER_DAMAGE__
#define __NEW_DROP_SYSTEM__

//#define ENABLE_OFFLINE_SHOP_SYSTEM
//#define ENABLE_VS_SHOP_SEARCH
//#define __PREMIUM_PRIVATE_SHOP__
#ifdef __PREMIUM_PRIVATE_SHOP__
	//#define ENABLE_PRIVATE_SHOP_PREMIUM_TIME
	#define ENABLE_CHEQUE_SYSTEM
	//#define ENABLE_PRIVATE_SHOP_BUNDLE_REQ
	#define ENABLE_PRIVATE_SHOP_BUILD_LIMITATIONS
	//#define ENABLE_PRIVATE_SHOP_SPECIAL_INV
	// #define ENABLE_PRIVATE_SHOP_EXTEND_INV
	// #define ENABLE_PRIVATE_SHOP_EXTEND_SPECIAL_INV
	// #define ENABLE_PRIVATE_SHOP_LOCKED_SLOTS
	// #define ENABLE_PRIVATE_SHOP_CHANGE_LOOK
	// #define ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
	// #define ENABLE_PRIVATE_SHOP_APPLY_RANDOM
	// #define ENABLE_PRIVATE_SHOP_SOCKET5
#endif

#define ENABLE_OFFLINE_SHOP
#ifdef ENABLE_OFFLINE_SHOP
#define ENABLE_SHOPS_IN_CITIES
//#define ENABLE_OFFLINESHOP_NOTIFICATION
#define ENABLE_SHOP_DECORATION
#define ENABLE_IRA_REWORK
#define ENABLE_LARGE_DYNAMIC_PACKET
#define ENABLE_OFFLINE_SHOP_LOG
#define ENABLE_OFFLINE_SHOP_GRID
#define ENABLE_OFFLINESHOP_EXTEND_TIME														// Extend shop time
#define ENABLE_AVERAGE_PRICE
#endif

#define MAX_COUNT uint16_t	// (uint16_t = 65535)

//////////////////////////////////////////////////////////////////////////
// ### Standard Features ###
//#define _IMPROVED_PACKET_ENCRYPTION_
#ifdef _IMPROVED_PACKET_ENCRYPTION_
	#define USE_IMPROVED_PACKET_DECRYPTED_BUFFER
#endif

#define __PET_SYSTEM__
#define __UDP_BLOCK__

#define __QUEST_RENEWAL__
#ifdef __QUEST_RENEWAL__
	#define _QR_MS_ // Marty Sama
#endif						

// ### END Standard Features ###
//////////////////////////////////////////////////////////////////////////

//Dodatki
#define ENABLE_DROP_INFO_PCT
#define ENABLE_BELT_ATTRIBUTES
#define ENABLE_GLOVE_ATTRIBUTES
#define ENABLE_TALISMAN_ATTRIBUTES
#define ENABLE_TALISMAN_ATTRIBUTES_RARE

#define ENABLE_ALIGN_RENEWAL
#define ENABLE_EXTENDED_BLEND
#define __BL_MOVE_CHANNEL__
#define ENABLE_REAL_TIME_REGEN
#define ENABLE_POLY_SHOP
#define __ENABLE_POLYMORPH_SYSTEM__
#define ENABLE_DROP_INFO
#define WEEKLY_RANK_BYLUZER
#define TITLE_SYSTEM_BYLUZER
#define ENABLE_SWITCHBOT
#define ENABLE_RESP_SYSTEM

#define DROP_LEVEL_LIMIT_RANGE
#define ENABLE_SPLIT_INVENTORY_SYSTEM

#define ENABLE_CUBE_ATTR_SOCKET
#define ENABLE_CHEQUE_SYSTEM
#define ENABLE_SHOP_USE_CHEQUE
#define __REMOVE_ITEM__
#define ENABLE_STOLE_COSTUME
#define ENABLE_GLOVE_SYSTEM
#define ENABLE_SYSTEM_RUNE
#define ENABLE_AFFECT_POLYMORPH_REMOVE
#define ENABLE_SKILL_AFFECT_REMOVE
#define ENABLE_SYSTEMY_KARTY_OKEY
#define ENABLE_MINIMAP_DUNGEONINFO					//
//#define __DUNGEON_INFO_ENABLE__
#define ENABLE_SORT_INVENTORY_ITEMS
#define __EXTENDED_ITEM_COUNT__
#define ENABLE_PUNKTY_OSIAGNIEC
#define ENABLE_SPECIAL_BONUS
#define ENABLE_AURA_SYSTEM
#define ENABLE_NEW_STONE_DETACH
#define ENABLE_HIDE_COSTUME_SYSTEM
#define ENABLE_COLLECT_WINDOW
#define ENABLE_COSTUME_EMBLEMAT
#define ENABLE_PROMO_CODE_SYSTEM
#define ENABLE_SKILE_PASYWNE
#define __CHATTING_WINDOW_RENEWAL__
#ifdef ENABLE_PROMO_CODE_SYSTEM
	#define ENABLE_PROMO_CODE_ONE_USE_PER_ACCOUNT
#endif
#define ENABLE_EXTENDED_ITEMNAME_ON_GROUND
//#define __BL_PARTY_POSITION__
#define __DUNGEON_INFO_SYSTEM__
#define __ENABLE_COLLECTIONS_SYSTEM__
#define ENABLE_DUNGEON_RETURN
#define __CROSS_CHANNEL_DUNGEON_WARP__
#define ENABLE_VIP_ITEMS
#define ENABLE_AREZZO_CHEST_DROP
#define __CASKET_PREVIEW_ENABLE__
#define __INVENTORY_BUFFERING__
#define ENABLE_DRAGON_SOUL_TIME_RENEWAL
#define BL_OFFLINE_MESSAGE
#define USE_FISH_SYSTEM
#define ENABLE_ODLAMKI_SYSTEM
#define ENABLE_EXTRABONUS_SYSTEM
//#define ENABLE_SKILL_SELECT_FEATURE
#define ENABLE_GOTO_LAG_FIX
#define WJ_ENABLE_TRADABLE_ICON
#define RENEWAL_DEAD_PACKET
#define __AUTO_QUQUE_ATTACK__
#define __VIEW_TARGET_PLAYER_HP__
#define __VIEW_TARGET_DECIMAL_HP__
#define ENABLE_ENTITY_PRELOADING
#define ENABLE_SAVE_LOCATION_SYSTEM
#define ENABLE_EVENT_MANAGER
#define __ENABLE_CHARACTER_BOOST__
#define ENABLE_NO_CLEAR_BUFF_WHEN_MONSTER_KILL

#define ENABLE_ITEMSHOP_TIMED_BONUSES
#define ENABLE_RENEWAL_DROP

#ifdef ENABLE_RENEWAL_DROP
static const int WORLDBOSS_MAP_INDICES[] = { 36, 35, 37, 106, 29 };
static const int WORLDBOSS_MAP_COUNT = sizeof(WORLDBOSS_MAP_INDICES) / sizeof(WORLDBOSS_MAP_INDICES[0]);
#endif


//#define ENABLE_KILL_NOTICE
#define ENABLE_VS_PELERYNKA_VIP
#define ENABLE_VS_COSTUME_BONUSES


#define ENABLE_HWID_SYSTEM							//-> x
#ifdef ENABLE_HWID_SYSTEM
enum eHWIDSettings {
	CPU_ID_MAX_LEN 									= 128,	//	128
	HDD_MODEL_MAX_LEN 								= 128,	//	128
	MACHINE_GUID_MAX_LEN 							= 128,	//	128
	MAC_ADDR_MAX_LEN 								= 128,	//	128
	HDD_SERIAL_MAX_LEN 								= 128,	//	128
	COMPOSITE_HWID_MAX_LEN							= 644,	//	128*5 + 4 separators
};
#endif
//#define ENABLE_ANTIHL
#define ENABLE_GOHOME_IF_MAP_NOT_ALLOWED
#define __AUTH_QUEUEING__
#define TAKE_LEGEND_DAMAGE_BOARD_SYSTEM
#ifdef TAKE_LEGEND_DAMAGE_BOARD_SYSTEM
#	define LEGEND_DAMAGE_BOARD_MAX_ENTRIES	15
#endif
#define ITEM_CHECKINOUT_UPDATE
#define ENABLE_QUIVER_SYSTEM
#define _ENABLE_BATTLEPASS_
//#define __AUTO_SKILL_READER__


//#define /* @author: Owsap */ __ITEM_APPLY_RANDOM__ // Apply Random Individual Attributes

// Aslan Buff - 15.05.2023 - Start
#define ENABLE_EXTENDED_SOCKETS
#define ENABLE_ASLAN_BUFF_NPC_SYSTEM
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	// You can Deactivate or Activate this
	#define ASLAN_BUFF_NPC_USE_SKILL_17_TO_M
	#define ASLAN_BUFF_NPC_USE_SKILL_TECH_LEVEL
	#define ASLAN_BUFF_NPC_ENABLE_EQ
	// Config values ( !!! no deactivate this defines !!! ) 
	#define ASLAN_BUFF_NPC_START_INT 180
	#define ASLAN_BUFF_NPC_MAX_INT 300
	#define ASLAN_BUFF_NPC_MIN_HP_PERC_USE_SKILL_HEAL 30
	#define ASLAN_BUFF_EXP_RING_ITEM_1 72001
	#define ASLAN_BUFF_EXP_RING_ITEM_2 72002
	#define ASLAN_BUFF_EXP_RING_ITEM_3 72003
#endif
// Aslan Buff - End

#define ENABLE_NEW_PET_SYSTEM // pety
#ifdef ENABLE_NEW_PET_SYSTEM
	#define ENABLE_NEW_PET_SYSTEM_CHANGE_NAME_ITEM_VNUM (DWORD)30000
	#define ENABLE_NEW_PET_SYSTEM_CHANGE_NAME_ITEM_COUNT (WORD)1
	#define ENABLE_NEW_PET_SYSTEM_BOOK
#endif

#define ENABLE_LONG_LONG							// Limit Yang

//////////////////////////////////////////////////////////////////////////
// ### New Features ###
#define ENABLE_D_NJGUILD
#define ENABLE_FULL_NOTICE
#define ENABLE_NEWSTUFF
#define ENABLE_PORT_SECURITY
#define ENABLE_BELT_INVENTORY_EX
#define ENABLE_CMD_WARP_IN_DUNGEON
// #define ENABLE_ITEM_ATTR_COSTUME
// #define ENABLE_SEQUENCE_SYSTEM
#define ENABLE_PLAYER_PER_ACCOUNT5
//#define ENABLE_DICE_SYSTEM
#define ENABLE_EXTEND_INVEN_SYSTEM

//#define ENABLE_MOUNT_COSTUME_SYSTEM
#define ENABLE_NEW_MOUNT_SYSTEM
#define __RENEWAL_MOUNT__

#define ENABLE_WEAPON_COSTUME_SYSTEM
#define ENABLE_QUEST_DIE_EVENT
#define ENABLE_QUEST_BOOT_EVENT
#define ENABLE_QUEST_DND_EVENT
#define ENABLE_SKILL_FLAG_PARTY
#define ENABLE_NO_DSS_QUALIFICATION
#define ENABLE_NO_SELL_PRICE_DIVIDED_BY_5
#define ENABLE_CHECK_SELL_PRICE
#define ENABLE_GET_TOTAL_ONLINE_COUNT												// Shows total online count on all cores
#define ENABLE_BOT_CONTROL
#define __ENABLE_MINING_WITH_SPACE__
//#define __WORLD_BOSS_YUMA__
#define DUNGEON_CARDS
#define __ENABLE_STEJNE_DMG__
#define __DUNGEON_LIMIT_RATES__
//#define __SPIN_WHEEL__
#define ENABLE_MOUNT_SYSTEM
//#define ENABLE_DAMAGE_LIMIT_CONFIG
#define SYSTEM_EFFECT_FISH
#define ENABLE_IGNORE_LOWER_BUFFS
#define ENABLE_DS_GRADE_MYTH														// Myth DS
#define __ENABLE_SKILL_DMG_MULTIPLIER__
#define __DS_CHANGE_ATTR__ // Dragon Soul Change Attribute
#define ENABLE_WHEEL_OF_FORTUNE
#define ENABLE_SUMMER_HWID_LIMIT
#define __BL_KILL_BAR__
#define ENABLE_AFFECT_BUFF_REMOVE													// Remove affect buff
#define __PENDANT__
#define ENABLE_TITLE_ACHIEVEMENT_SYSTEM
#define __SKILL_COSTUME__
#define ENABLE_MESSENGER_BLOCK
#define __DUNGEON_INFO__
#define ENABLE_NOTIFICATION_SYSTEM
#define __ENABLE_DAMAGE_RESTRICTIONS__
#define ENABLE_BUY_WITH_ITEM
#define __VIP_SYSTEM__												// Premium system
#define ENABLE_AUTO_SELECT_SKILL
#define ENABLE_MULTI_LANGUAGE_SYSTEM						// Multi-language item name lookup
#define ENABLE_MULTI_FARM_BLOCK
#define ENABLE_VOTE4BUFF

enum eCommonDefines {
	MAP_ALLOW_LIMIT = 32, // 32 default
};

//#define ENABLE_WOLFMAN_CHARACTER
//#ifdef ENABLE_WOLFMAN_CHARACTER
//#define USE_MOB_BLEEDING_AS_POISON
//#define USE_MOB_CLAW_AS_DAGGER
// #define USE_ITEM_BLEEDING_AS_POISON
// #define USE_ITEM_CLAW_AS_DAGGER
//#define USE_WOLFMAN_STONES
//#define USE_WOLFMAN_BOOKS
//#endif

// #define ENABLE_MAGIC_REDUCTION_SYSTEM
#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
// #define USE_MAGIC_REDUCTION_STONES
#endif

// ### END New Features ###
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ### Ex Features ###
#define DISABLE_STOP_RIDING_WHEN_DIE //	if DISABLE_TOP_RIDING_WHEN_DIE is defined, the player doesn't lose the horse after dying
#define ENABLE_ACCE_COSTUME_SYSTEM //fixed version
// #define USE_ACCE_ABSORB_WITH_NO_NEGATIVE_BONUS //enable only positive bonus in acce absorb
#define ENABLE_HIGHLIGHT_NEW_ITEM //if you want to see highlighted a new item when dropped or when exchanged
#define ENABLE_KILL_EVENT_FIX //if you want to fix the 0 exp problem about the when kill lua event (recommended)
// #define ENABLE_SYSLOG_PACKET_SENT // debug purposes

#define ENABLE_EXTEND_ITEM_AWARD //slight adjustement
#ifdef ENABLE_EXTEND_ITEM_AWARD
	// #define USE_ITEM_AWARD_CHECK_ATTRIBUTES //it prevents bonuses higher than item_attr lvl1-lvl5 min-max range limit
#endif

//#define RANKING_SYSTEM
#define ENABLE_SECONDARY_LEVEL										// Secondary, upgradeable level
//#define ENABLE_ENLIGHTENMENT_SYSTEM
//#define __PICKUPER_HELPER__
#define ENABLE_ITEMSHOP
#ifdef ENABLE_ITEMSHOP
#define USE_ITEMSHOP_RENEWED
#endif

// ### END Ex Features ###
//////////////////////////////////////////////////////////////////////////
