#pragma once

#include "Locale.h"
#include "../gamelib/RaceData.h"
#include "../gamelib/ItemData.h"

typedef BYTE TPacketHeader;

enum
{
	/////////////////////////////////////////////////
	// To Server
	// HEADER_BLANK is the not use(for future use)
	HEADER_CG_LOGIN								= 1,
	HEADER_CG_ATTACK							= 2,
	HEADER_CG_CHAT								= 3,
	HEADER_CG_PLAYER_CREATE						= 4,
	HEADER_CG_PLAYER_DESTROY					= 5,
	HEADER_CG_PLAYER_SELECT						= 6,
	HEADER_CG_CHARACTER_MOVE					= 7,
	HEADER_CG_SYNC_POSITION  					= 8,
	HEADER_CG_DIRECT_ENTER						= 9,
	HEADER_CG_ENTERGAME							= 10,
	HEADER_CG_ITEM_USE							= 11,
	HEADER_CG_ITEM_DROP							= 12,
	HEADER_CG_ITEM_MOVE							= 13,
	HEADER_CG_ITEM_PICKUP						= 15,
	HEADER_CG_QUICKSLOT_ADD                     = 16,
	HEADER_CG_QUICKSLOT_DEL                     = 17,
	HEADER_CG_QUICKSLOT_SWAP                    = 18,
	HEADER_CG_WHISPER							= 19,
	HEADER_CG_ITEM_DROP2                        = 20,
#ifdef ENABLE_INSTANT_CHEST_OPEN
	HEADER_CG_CHEST_OPEN						= 22,
#endif
	//HEADER_BLANK21								= 21,
	//HEADER_BLANK22								= 22,
	//HEADER_BLANK22								= 23,
	//HEADER_BLANK24								= 24,
	//HEADER_BLANK25								= 25,
	HEADER_CG_ON_CLICK							= 26,
	HEADER_CG_EXCHANGE							= 27,
    HEADER_CG_CHARACTER_POSITION                = 28,
    HEADER_CG_SCRIPT_ANSWER						= 29,
	HEADER_CG_QUEST_INPUT_STRING				= 30,
    HEADER_CG_QUEST_CONFIRM                     = 31,
	//HEADER_BLANK32								= 32,
	//HEADER_BLANK33								= 33,
	//HEADER_BLANK34								= 34,
	//HEADER_BLANK35								= 35,
	//HEADER_BLANK36								= 36,
	//HEADER_BLANK37								= 37,
	//HEADER_BLANK38								= 38,
	//HEADER_BLANK39								= 39,
	//HEADER_BLANK40								= 40,
	HEADER_CG_PVP								= 41,
	//HEADER_BLANK42								= 42,
	//HEADER_BLANK43								= 43,
	//HEADER_BLANK44								= 44,
	//HEADER_BLANK45								= 45,
	//HEADER_BLANK46								= 46,
	//HEADER_BLANK47								= 47,
	//HEADER_BLANK48								= 48,
	//HEADER_BLANK49								= 49,
    HEADER_CG_SHOP								= 50,
	HEADER_CG_FLY_TARGETING						= 51,
	HEADER_CG_USE_SKILL							= 52,
    HEADER_CG_ADD_FLY_TARGETING                 = 53,
	HEADER_CG_SHOOT								= 54,
	HEADER_CG_MYSHOP                            = 55,
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	HEADER_CG_OFFLINE_SHOP						= 56,
	HEADER_CG_MY_OFFLINE_SHOP					= 57,
#endif
	//HEADER_BLANK56								= 56,
	//HEADER_BLANK57								= 57,
	//HEADER_BLANK58								= 58,
	//HEADER_BLANK59								= 59,
	HEADER_CG_ITEM_USE_TO_ITEM					= 60,
    HEADER_CG_TARGET                            = 61,
	//HEADER_BLANK62								= 62,
	//HEADER_BLANK63								= 63,
	//HEADER_BLANK64								= 64,
	HEADER_CG_WARP								= 65,
    HEADER_CG_SCRIPT_BUTTON						= 66,
    HEADER_CG_MESSENGER                         = 67,
	//HEADER_BLANK68								= 68,
    HEADER_CG_MALL_CHECKOUT                     = 69,
    HEADER_CG_SAFEBOX_CHECKIN                   = 70,
    HEADER_CG_SAFEBOX_CHECKOUT                  = 71,
    HEADER_CG_PARTY_INVITE                      = 72,
    HEADER_CG_PARTY_INVITE_ANSWER               = 73,
    HEADER_CG_PARTY_REMOVE                      = 74,
    HEADER_CG_PARTY_SET_STATE                   = 75,
    HEADER_CG_PARTY_USE_SKILL                   = 76,
    HEADER_CG_SAFEBOX_ITEM_MOVE                 = 77,
	HEADER_CG_PARTY_PARAMETER                   = 78,
	//HEADER_BLANK68								= 79,
	HEADER_CG_GUILD								= 80,
	HEADER_CG_ANSWER_MAKE_GUILD					= 81,
    HEADER_CG_FISHING                           = 82,
    HEADER_CG_GIVE_ITEM                         = 83,
	//HEADER_BLANK84								= 84,
	//HEADER_BLANK85								= 85,
	//HEADER_BLANK86								= 86,
	//HEADER_BLANK87								= 87,
	//HEADER_BLANK88								= 88,
	//HEADER_BLANK89								= 89,
    HEADER_CG_EMPIRE                            = 90,
	//HEADER_BLANK91								= 91,
	//HEADER_BLANK92								= 92,
	//HEADER_BLANK93								= 93,
	//HEADER_BLANK94								= 94,
	//HEADER_BLANK95								= 95,
    HEADER_CG_REFINE                            = 96,
	//HEADER_BLANK97								= 97,
	//HEADER_BLANK98								= 98,
	//HEADER_BLANK99								= 99,

	HEADER_CG_MARK_LOGIN						= 100,
	HEADER_CG_MARK_CRCLIST						= 101,
	HEADER_CG_MARK_UPLOAD						= 102,
	HEADER_CG_MARK_IDXLIST						= 104,

	HEADER_CG_CRC_REPORT						= 103,

	HEADER_CG_HACK								= 105,
    HEADER_CG_CHANGE_NAME                       = 106,
    HEADER_CG_SMS                               = 107,
    HEADER_CG_LOGIN2                            = 109,
	HEADER_CG_DUNGEON							= 110,
	HEADER_CG_LOGIN3							= 111,
	HEADER_CG_GUILD_SYMBOL_UPLOAD				= 112,
	HEADER_CG_GUILD_SYMBOL_CRC					= 113,
	HEADER_CG_SCRIPT_SELECT_ITEM				= 114,
	HEADER_CG_LOGIN4							= 115,
#ifdef TITLE_SYSTEM_BYLUZER
	HEADER_CG_TITLE_ACTIVE				= 129,
#endif
#ifdef WEEKLY_RANK_BYLUZER
	HEADER_CG_WEEKLY_RANK				= 130,
#endif
#ifdef __ENABLE_MINING_WITH_SPACE__
	HEADER_CG_MINING = 131,
#endif
#ifdef __ENABLE_POLYMORPH_SYSTEM__
	HEADER_CG_POLY_SYSTEM = 138,
#endif
#ifdef ENABLE_SWITCHBOT
	HEADER_CG_SWITCHBOT							= 171,
#endif
#ifdef ENABLE_SECONDARY_LEVEL
	HEADER_CG_SECONDARY_LEVEL = 181,
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	HEADER_CG_BUFF_NPC_CREATE = 186,
	HEADER_CG_BUFF_NPC_ACTION = 187,
#endif

	HEADER_CG_DRAGON_SOUL_REFINE				= 205,
	HEADER_CG_STATE_CHECKER						= 206,
#ifdef ENABLE_SAVE_LOCATION_SYSTEM
	HEADER_CG_SAVE_LOCATION = 212,
#endif
#ifdef ENABLE_LRN_LOCATION_SYSTEM
	HEADER_CG_SEND_LOCATION_MANAGER				= 215,
#endif
#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
	HEADER_CG_CUBE_RENEWAL					= 220,
#endif
#ifdef ENABLE_CHEQUE_SYSTEM
	HEADER_CG_WON_EXCHANGE						= 222,
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
	HEADER_CG_PET_ACTION					= 223,
#endif
#ifdef ENABLE_OFFLINE_SHOP
	HEADER_CG_NEW_OFFLINESHOP 		 = 225,
#endif
#ifdef RANKING_SYSTEM
	HEADER_CG_LOAD_SERVER_RANKING = 226,
	HEADER_CG_LOAD_RANKING = 227,
#endif
#ifdef ENABLE_AURA_SYSTEM
	HEADER_CG_AURA								= 232,
#endif
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	HEADER_CG_MOUNT_NPC_ACTION					= 233,
#endif
	HEADER_CG_GIVE_ITEMS						= 234,
#if defined(__BL_MOVE_CHANNEL__)
	HEADER_CG_MOVE_CHANNEL						= 240,
#endif
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
	HEADER_CG_PRIVATE_SHOP						= 241,
#endif
	HEADER_CG_CUBE								= 242,
#ifdef ENABLE_INSTANT_INGOT_OPEN
	HEADER_CG_INGOT_OPEN = 243,
#endif
	HEADER_CG_KEY_AGREEMENT						= 0xfb, // _IMPROVED_PACKET_ENCRYPTION_
	HEADER_CG_TIME_SYNC							= 0xfc,
	HEADER_CG_CLIENT_VERSION					= 0xfd,
	HEADER_CG_CLIENT_VERSION2					= 0xf1,
	HEADER_CG_PONG								= 0xfe,
    HEADER_CG_HANDSHAKE                         = 0xff,

	/////////////////////////////////////////////////
	// From Server
	HEADER_GC_CHARACTER_ADD						= 1,
	HEADER_GC_CHARACTER_DEL						= 2,
	HEADER_GC_CHARACTER_MOVE					= 3,
	HEADER_GC_CHAT								= 4,
	HEADER_GC_SYNC_POSITION 					= 5,
	HEADER_GC_LOGIN_SUCCESS3					= 6,
	HEADER_GC_LOGIN_FAILURE						= 7,
	HEADER_GC_PLAYER_CREATE_SUCCESS				= 8,
	HEADER_GC_PLAYER_CREATE_FAILURE				= 9,
	HEADER_GC_PLAYER_DELETE_SUCCESS				= 10,
	HEADER_GC_PLAYER_DELETE_WRONG_SOCIAL_ID		= 11,
	// 12
	HEADER_GC_STUN								= 13,
	HEADER_GC_DEAD								= 14,

	HEADER_GC_MAIN_CHARACTER					= 15,
	HEADER_GC_PLAYER_POINTS						= 16,
	HEADER_GC_PLAYER_POINT_CHANGE				= 17,
	HEADER_GC_CHANGE_SPEED						= 18,
	HEADER_GC_CHARACTER_UPDATE                  = 19,
	HEADER_GC_ITEM_SET							= 20,
	HEADER_GC_ITEM_SET2							= 21,
	HEADER_GC_ITEM_USE							= 22,
	HEADER_GC_ITEM_DROP							= 23,
	HEADER_GC_ITEM_UPDATE						= 25,
	HEADER_GC_ITEM_GROUND_ADD					= 26,
	HEADER_GC_ITEM_GROUND_DEL					= 27,
    HEADER_GC_QUICKSLOT_ADD                     = 28,
    HEADER_GC_QUICKSLOT_DEL                     = 29,
    HEADER_GC_QUICKSLOT_SWAP                    = 30,
	HEADER_GC_ITEM_OWNERSHIP					= 31,
	HEADER_GC_LOGIN_SUCCESS4					= 32,
	HEADER_GC_ITEM_UNBIND_TIME					= 33,
	HEADER_GC_WHISPER							= 34,
	HEADER_GC_ALERT								= 35,

	HEADER_GC_MOTION							= 36,

	HEADER_GC_SHOP							    = 38,
	HEADER_GC_SHOP_SIGN							= 39,

	HEADER_GC_DUEL_START						= 40,
	HEADER_GC_PVP								= 41,
	HEADER_GC_EXCHANGE							= 42,
    HEADER_GC_CHARACTER_POSITION                = 43,

	HEADER_GC_PING								= 44,

	HEADER_GC_SCRIPT							= 45,
    HEADER_GC_QUEST_CONFIRM                     = 46,
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	HEADER_GC_OFFLINE_SHOP						= 47,
	HEADER_GC_OFFLINE_SHOP_SIGN					= 48,
	HEADER_GC_GOLD								= 49,
	HEADER_GC_OFFLINE_SHOP_LOG					= 52,
#endif
	HEADER_GC_ANTYEXP							= 58,
#ifdef WJ_SPLIT_INVENTORY_SYSTEM
	HEADER_GC_WPADANIE							= 59,
#endif

	HEADER_GC_MOUNT								= 61,
	HEADER_GC_OWNERSHIP                         = 62,
    HEADER_GC_TARGET                            = 63,
	HEADER_GC_WARP = 65,
	HEADER_GC_ADD_FLY_TARGETING = 69,

	HEADER_GC_CREATE_FLY = 70,
	HEADER_GC_FLY_TARGETING = 71,
	HEADER_GC_SKILL_LEVEL = 72,
	HEADER_GC_SKILL_COOLTIME_END = 73,
	HEADER_GC_MESSENGER = 74,
	HEADER_GC_GUILD = 75,
	HEADER_GC_SKILL_LEVEL_NEW = 76,

	HEADER_GC_PARTY_INVITE = 77,
	HEADER_GC_PARTY_ADD = 78,
	HEADER_GC_PARTY_UPDATE = 79,
	HEADER_GC_PARTY_REMOVE = 80,

	HEADER_GC_QUEST_INFO = 81,
	HEADER_GC_REQUEST_MAKE_GUILD = 82,
	HEADER_GC_PARTY_PARAMETER = 83,

	HEADER_GC_SAFEBOX_MONEY_CHANGE = 84,
	HEADER_GC_SAFEBOX_SET = 85,
	HEADER_GC_SAFEBOX_DEL = 86,
	HEADER_GC_SAFEBOX_WRONG_PASSWORD = 87,
	HEADER_GC_SAFEBOX_SIZE = 88,

	HEADER_GC_FISHING = 89,

	HEADER_GC_EMPIRE = 90,

	HEADER_GC_PARTY_LINK = 91,
	HEADER_GC_PARTY_UNLINK = 92,

	HEADER_GC_REFINE_INFORMATION = 95,

	HEADER_GC_OBSERVER_ADD = 96,
	HEADER_GC_OBSERVER_REMOVE = 97,
	HEADER_GC_OBSERVER_MOVE = 98,
	HEADER_GC_VIEW_EQUIP = 99,

	HEADER_GC_MARK_BLOCK = 100,
	HEADER_GC_MARK_DIFF_DATA = 101,
	HEADER_GC_MARK_IDXLIST = 102,
#ifdef ENABLE_MINIMAP_DUNGEONINFO
	HEADER_GC_MINIMAP_DUNGEONINFO = 103,
#endif
	HEADER_GC_TIME = 106,
	HEADER_GC_CHANGE_NAME = 107,

	HEADER_GC_DUNGEON = 110,
	HEADER_GC_WALK_MODE = 111,
	HEADER_GC_CHANGE_SKILL_GROUP = 112,

	// SUPPORT_BGM
	HEADER_GC_MAIN_CHARACTER2_EMPIRE = 113,
	// END_OF_SUPPORT_BGM

	HEADER_GC_SEPCIAL_EFFECT = 114,
	HEADER_GC_NPC_POSITION = 115,

	HEADER_GC_LOGIN_KEY = 118,
	HEADER_GC_REFINE_INFORMATION_NEW = 119,
	HEADER_GC_CHARACTER_ADD2 = 120,
	HEADER_GC_CHANNEL = 121,

	HEADER_GC_MALL_OPEN = 122,
	HEADER_GC_TARGET_UPDATE = 123,
	HEADER_GC_TARGET_DELETE = 124,
	HEADER_GC_TARGET_CREATE_NEW = 125,

	HEADER_GC_AFFECT_ADD = 126,
	HEADER_GC_AFFECT_REMOVE = 127,

	HEADER_GC_MALL_SET = 128,
	HEADER_GC_MALL_DEL = 129,
	HEADER_GC_LAND_LIST = 130,
	HEADER_GC_LOVER_INFO = 131,
	HEADER_GC_LOVE_POINT_UPDATE = 132,
	HEADER_GC_GUILD_SYMBOL_DATA = 133,
	HEADER_GC_DIG_MOTION = 134,

	HEADER_GC_DAMAGE_INFO = 135,
	HEADER_GC_CHAR_ADDITIONAL_INFO = 136,

	// SUPPORT_BGM
	HEADER_GC_MAIN_CHARACTER3_BGM = 137,
	HEADER_GC_MAIN_CHARACTER4_BGM_VOL = 138,
	// END_OF_SUPPORT_BGM

	HEADER_GC_AUTH_SUCCESS = 150,
	HEADER_GC_PANAMA_PACK = 151,

	//HYBRID CRYPT
	HEADER_GC_HYBRIDCRYPT_KEYS = 152,
	HEADER_GC_HYBRIDCRYPT_SDB = 153, // SDB means Supplmentary Data Blocks
	//HYBRID CRYPT
#ifdef ENABLE_COLLECT_WINDOW
	HEADER_GC_COLLECT = 155,
#endif
#ifdef ENABLE_AURA_SYSTEM
	HEADER_GC_AURA								= 157,
#endif
#ifdef TITLE_SYSTEM_BYLUZER
	HEADER_GC_TITLE_INFO						= 163,
	HEADER_GC_TITLE_ACTIVE						= 164,
#endif
#ifdef WEEKLY_RANK_BYLUZER
	HEADER_GC_WEEKLY_RANK						= 165,
	HEADER_GC_WEEKLY_PAGE						= 166,
#endif
#ifdef ENABLE_SWITCHBOT
	HEADER_GC_SWITCHBOT							= 171,
#endif
#ifdef ENABLE_EVENT_MANAGER
	HEADER_GC_EVENT_MANAGER = 172,
	HEADER_GC_EVENT_ACTIVATE_RESULT = 240,
	HEADER_CG_EVENT_ACTIVATE = 246,
#endif
#ifdef TAKE_LEGEND_DAMAGE_BOARD_SYSTEM
	HEADER_GC_LEGEND_DAMAGE_DATA = 173,
#endif
#ifdef ENABLE_OFFLINE_SHOP
	HEADER_GC_NEW_OFFLINESHOP = 174,
#endif
#ifdef RANKING_SYSTEM
	HEADER_GC_SERVER_RANKING_LOAD = 175,
	HEADER_GC_RANKING_LOAD = 176,
#endif
#ifdef ENABLE_SECONDARY_LEVEL
	HEADER_GC_SECONDARY_LEVEL = 181,
#endif
#ifdef __ENABLE_POLYMORPH_SYSTEM__
	HEADER_GC_POLY_SYSTEM = 183,
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	HEADER_GC_BUFF_NPC_ACTION 					= 188,
	HEADER_GC_BUFF_NPC_USE_SKILL 				= 189,
#endif
	HEADER_GC_SPECIFIC_EFFECT					= 208,
	HEADER_GC_DRAGON_SOUL_REFINE				= 209,
	HEADER_GC_RESPOND_CHANNELSTATUS				= 210,
#ifdef __INVENTORY_BUFFERING__
	HEADER_GC_ITEM_BUFFERED = 213,
#endif
#ifdef ENABLE_SAVE_LOCATION_SYSTEM
	HEADER_GC_SAVE_LOCATION = 216,
#endif
#ifdef ENABLE_ENTITY_PRELOADING
	HEADER_GC_PRELOAD_ENTITIES					= 217,
#endif
#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
	HEADER_GC_CUBE_RENEWAL						= 221,
#endif
	HEADER_GC_GIVE_ITEMS						= 234,
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	//HEADER_GC_MOUNT_NPC_ACTION					= 235,
#endif
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
	HEADER_GC_PRIVATE_SHOP = 235,
#endif
	HEADER_GC_CUBE = 236,
#ifdef ENABLE_BATTLE_PASS_ROBERT
	HEADER_GC_BATTLE_PASS						= 237,
#endif
#ifdef ENABLE_MOUNT_SYSTEM
	HEADER_GC_MOUNT_INFO						= 238,
#endif

	HEADER_GC_KEY_AGREEMENT_COMPLETED			= 0xfa, // _IMPROVED_PACKET_ENCRYPTION_
	HEADER_GC_KEY_AGREEMENT						= 0xfb, // _IMPROVED_PACKET_ENCRYPTION_
	HEADER_GC_HANDSHAKE_OK						= 0xfc, // 252
	HEADER_GC_PHASE								= 0xfd,	// 253
    HEADER_GC_BINDUDP                           = 0xfe, // 254
    HEADER_GC_HANDSHAKE                         = 0xff, // 255
};

enum
{
	ID_MAX_NUM = 30,
	PASS_MAX_NUM = 16,
	CHAT_MAX_NUM = 128,
	PATH_NODE_MAX_NUM = 64,
	SHOP_SIGN_MAX_LEN = 32,

	PLAYER_PER_ACCOUNT3 = 3,
#ifndef ENABLE_PLAYER_PER_ACCOUNT5
	PLAYER_PER_ACCOUNT4 = 4,
#else
	PLAYER_PER_ACCOUNT4 = 5,
	PLAYER_PER_ACCOUNT5 = 5,
#endif

	PLAYER_ITEM_SLOT_MAX_NUM = 20,

	QUICKSLOT_MAX_LINE = 4,
	QUICKSLOT_MAX_COUNT_PER_LINE = 8,
	QUICKSLOT_MAX_COUNT = QUICKSLOT_MAX_LINE * QUICKSLOT_MAX_COUNT_PER_LINE,

	QUICKSLOT_MAX_NUM = 36,
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	OFFLINE_SHOP_HOST_ITEM_MAX_NUM = 120,
#endif
	SHOP_HOST_ITEM_MAX_NUM = 40,

	METIN_SOCKET_COUNT = 6,

	PARTY_AFFECT_SLOT_MAX_NUM = 7,

	GUILD_GRADE_NAME_MAX_LEN = 8,
	GUILD_NAME_MAX_LEN = 12,
	GUILD_GRADE_COUNT = 15,
	GULID_COMMENT_MAX_LEN = 50,

	MARK_CRC_NUM = 8*8,
	MARK_DATA_SIZE = 16*12,
	SYMBOL_DATA_SIZE = 128*256,
	QUEST_INPUT_STRING_MAX_NUM = 64,

	PRIVATE_CODE_LENGTH = 8,

	REFINE_MATERIAL_MAX_NUM = 10,

	WEAR_MAX_NUM = CItemData::WEAR_MAX_NUM,

	SHOP_TAB_NAME_MAX = 32,
	SHOP_TAB_COUNT_MAX = 3,

#ifdef ENABLE_HWID_SYSTEM
	CPU_ID_MAX_NUM = 128,
	HDD_MODEL_MAX_NUM = 128,
	MACHINE_GUID_MAX_NUM = 128,
	MAC_ADDR_MAX_NUM = 128,
	HDD_SERIAL_MAX_NUM = 128,
#endif
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
	PRIVATE_SHOP_PAGE_MAX_NUM = 2,
	PRIVATE_SHOP_WIDTH = 8,
	PRIVATE_SHOP_HEIGHT = 8,
	PRIVATE_SHOP_PAGE_ITEM_MAX_NUM = PRIVATE_SHOP_WIDTH * PRIVATE_SHOP_HEIGHT,
	PRIVATE_SHOP_HOST_ITEM_MAX_NUM = (PRIVATE_SHOP_WIDTH * PRIVATE_SHOP_HEIGHT) * PRIVATE_SHOP_PAGE_MAX_NUM,
	PRIVATE_SHOP_LOCKED_SLOT_MAX_NUM = PRIVATE_SHOP_HOST_ITEM_MAX_NUM / 2,
	PRIVATE_SHOP_SLOT_UNLOCK_ITEM = 72357,

	TITLE_MAX_LEN = 32,
	TITLE_MIN_LEN = 3,
#endif
};

#pragma pack(push)
#pragma pack(1)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mark
typedef struct command_mark_login
{
    BYTE    header;
    DWORD   handle;
    DWORD   random_key;
} TPacketCGMarkLogin;

typedef struct command_mark_upload
{
    BYTE    header;
    DWORD   gid;
    BYTE    image[16*12*4];
} TPacketCGMarkUpload;

typedef struct command_mark_idxlist
{
    BYTE    header;
} TPacketCGMarkIDXList;

typedef struct command_mark_crclist
{
    BYTE    header;
    BYTE    imgIdx;
    DWORD   crclist[80];
} TPacketCGMarkCRCList;

typedef struct packet_mark_idxlist
{
    BYTE    header;
	DWORD	bufSize;
    WORD    count;
} TPacketGCMarkIDXList;

typedef struct packet_mark_block
{
    BYTE    header;
    DWORD   bufSize;
	BYTE	imgIdx;
    DWORD   count;
} TPacketGCMarkBlock;

typedef struct command_symbol_upload
{
	BYTE	header;
	WORD	size;
	DWORD	handle;
} TPacketCGSymbolUpload;

typedef struct command_symbol_crc
{
	BYTE	header;
	DWORD	dwGuildID;
	DWORD	dwCRC;
	DWORD	dwSize;
} TPacketCGSymbolCRC;

typedef struct packet_symbol_data
{
    BYTE header;
    WORD size;
    DWORD guild_id;
} TPacketGCGuildSymbolData;

//
//
//
typedef struct packet_observer_add
{
	BYTE	header;
	DWORD	vid;
	WORD	x;
	WORD	y;
} TPacketGCObserverAdd;

typedef struct packet_observer_move
{
	BYTE	header;
	DWORD	vid;
	WORD	x;
	WORD	y;
} TPacketGCObserverMove;

typedef struct packet_observer_remove
{
	BYTE	header;
	DWORD	vid;
} TPacketGCObserverRemove;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// To Server

typedef struct command_checkin
{
	BYTE header;
	char name[ID_MAX_NUM+1];
	char pwd[PASS_MAX_NUM+1];
} TPacketCGCheckin;

typedef struct command_login
{
    BYTE header;
    char name[ID_MAX_NUM + 1];
    char pwd[PASS_MAX_NUM + 1];
} TPacketCGLogin;

typedef struct command_login2
{
	BYTE	header;
	char	name[ID_MAX_NUM + 1];
	DWORD	login_key;
    DWORD	adwClientKey[4];
} TPacketCGLogin2;

typedef struct command_login3
{
    BYTE	header;
    char	name[ID_MAX_NUM + 1];
    char	pwd[PASS_MAX_NUM + 1];
	char	pin[PASS_MAX_NUM + 1];
    DWORD	adwClientKey[4];
#ifdef CHECK_CLIENT_VERSION
	DWORD	client_version;
#endif
#ifdef ENABLE_HWID_SYSTEM
	char	cpu_id[CPU_ID_MAX_NUM + 1];
	char	hdd_model[HDD_MODEL_MAX_NUM + 1];
	char	machine_guid[MACHINE_GUID_MAX_NUM + 1];
	char	mac_addr[MAC_ADDR_MAX_NUM + 1];
	char	hdd_serial[HDD_SERIAL_MAX_NUM + 1];
#endif
} TPacketCGLogin3;

typedef struct command_direct_enter
{
    BYTE        bHeader;
    char        login[ID_MAX_NUM + 1];
    char        passwd[PASS_MAX_NUM + 1];
    BYTE        index;
} TPacketCGDirectEnter;

typedef struct command_player_select
{
	BYTE	header;
	BYTE	player_index;
} TPacketCGSelectCharacter;

typedef struct command_attack
{
	BYTE	header;
	BYTE	bType;
	DWORD	dwVictimVID;
	BYTE	bCRCMagicCubeProcPiece;
	BYTE	bCRCMagicCubeFilePiece;
} TPacketCGAttack;

typedef struct command_chat
{
	BYTE	header;
	WORD	length;
	BYTE	type;
} TPacketCGChat;

typedef struct command_whisper
{
    BYTE        bHeader;
    WORD        wSize;
    char        szNameTo[CHARACTER_NAME_MAX_LEN + 1];
} TPacketCGWhisper;

typedef struct command_sms
{
    BYTE        bHeader;
    WORD        wSize;
    char        szNameTo[CHARACTER_NAME_MAX_LEN + 1];
} TPacketCGSMS;

enum EBattleMode
{
	BATTLEMODE_ATTACK = 0,
	BATTLEMODE_DEFENSE = 1,
};

typedef struct command_EnterFrontGame
{
	BYTE header;
} TPacketCGEnterFrontGame;

typedef struct command_item_use
{
	BYTE header;
	TItemPos pos;
} TPacketCGItemUse;

#ifdef ENABLE_INSTANT_CHEST_OPEN
typedef struct OpenChestPacket
{
	BYTE header;
	TItemPos pos;
	WORD wOpenCount;
} TPacketCGOpenChest;
#endif

#ifdef ENABLE_INSTANT_INGOT_OPEN
typedef struct OpenIngotPacket
{
	BYTE header;
	TItemPos pos;
	WORD wOpenCount;
} TPacketCGOpenIngot;
#endif

typedef struct command_item_use_to_item
{
	BYTE header;
	TItemPos source_pos;
	TItemPos target_pos;
} TPacketCGItemUseToItem;

typedef struct command_item_drop
{
	BYTE  header;
	TItemPos pos;
	DWORD elk;
#ifdef ENABLE_CHEQUE_SYSTEM
	DWORD cheque;
#endif
} TPacketCGItemDrop;

typedef struct command_item_drop2
{
    BYTE        header;
    TItemPos	pos;
    DWORD       gold;
#ifdef ENABLE_CHEQUE_SYSTEM
	DWORD		cheque;
#endif
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	uint16_t			count;
#else
	BYTE		count;
#endif
} TPacketCGItemDrop2;

typedef struct command_item_move
{
	BYTE header;
	TItemPos pos;
	TItemPos change_pos;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	uint16_t		num;
#else
	BYTE		num;
#endif
} TPacketCGItemMove;

typedef struct command_item_pickup
{
	BYTE header;
	DWORD vid;
} TPacketCGItemPickUp;

typedef struct command_quickslot_add
{
    BYTE        header;
    BYTE        pos;
	TQuickSlot	slot;
}TPacketCGQuickSlotAdd;

typedef struct command_quickslot_del
{
    BYTE        header;
    BYTE        pos;
}TPacketCGQuickSlotDel;

typedef struct command_quickslot_swap
{
    BYTE        header;
    BYTE        pos;
    BYTE        change_pos;
}TPacketCGQuickSlotSwap;

typedef struct command_on_click
{
	BYTE		header;
	DWORD		vid;
} TPacketCGOnClick;

enum
{
    SHOP_SUBHEADER_CG_END,
	SHOP_SUBHEADER_CG_BUY,
	SHOP_SUBHEADER_CG_SELL,
	SHOP_SUBHEADER_CG_SELL2,
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	SHOP_SUBHEADER_CG_ADD_ITEM,
	SHOP_SUBHEADER_CG_REMOVE_ITEM,
	SHOP_SUBHEADER_CG_DESTROY_OFFLINE_SHOP,
	SHOP_SUBHEADER_CG_REFRESH,
	SHOP_SUBHEADER_CG_REFRESH_MONEY,
	SHOP_SUBHEADER_CG_REFRESH_CHEQUE,
	SHOP_SUBHEADER_CG_WITHDRAW_MONEY,
	SHOP_SUBHEADER_CG_WITHDRAW_CHEQUE,
#endif
};

typedef struct command_shop
{
	BYTE        header;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	uint16_t			subheader;
#else
	BYTE		subheader;
#endif
} TPacketCGShop;

enum
{
	EXCHANGE_SUBHEADER_CG_START,			// arg1 == vid of target character
	EXCHANGE_SUBHEADER_CG_ITEM_ADD,			// arg1 == position of item
	EXCHANGE_SUBHEADER_CG_ITEM_DEL,			// arg1 == position of item
	EXCHANGE_SUBHEADER_CG_ELK_ADD,			// arg1 == amount of elk
#ifdef ENABLE_CHEQUE_SYSTEM
	EXCHANGE_SUBHEADER_CG_CHEQUE_ADD,
#endif
	EXCHANGE_SUBHEADER_CG_ACCEPT,			// arg1 == not used
	EXCHANGE_SUBHEADER_CG_CANCEL,			// arg1 == not used
};

typedef struct command_exchange
{
	BYTE		header;
	BYTE		subheader;
#ifdef ENABLE_LONG_LONG
	long long	arg1;
#else
	DWORD		arg1;
#endif
	BYTE		arg2;
	TItemPos	Pos;
#if defined(ITEM_CHECKINOUT_UPDATE)
	bool		SelectPosAuto;
#endif
} TPacketCGExchange;

typedef struct command_position
{
    BYTE        header;
    BYTE        position;
} TPacketCGPosition;
#if defined(__BL_MOVE_CHANNEL__)
typedef struct command_move_channel
{
	BYTE		header;
	BYTE		channel;
} TPacketCGMoveChannel;
#endif
typedef struct command_script_answer
{
    BYTE        header;
	BYTE		answer;
} TPacketCGScriptAnswer;

typedef struct command_script_button
{
	BYTE        header;
	unsigned int			idx;
#ifdef ENABLE_COLLECT_WINDOW
	BYTE		button;
#endif
} TPacketCGScriptButton;

typedef struct command_target
{
    BYTE        header;
    DWORD       dwVID;
} TPacketCGTarget;

typedef struct command_move
{
	BYTE		bHeader;
	BYTE		bFunc;
	BYTE		bArg;
	BYTE		bRot;
	LONG		lX;
	LONG		lY;
	DWORD		dwTime;
} TPacketCGMove;

typedef struct command_sync_position_element
{
    DWORD       dwVID;
    long        lX;
    long        lY;
} TPacketCGSyncPositionElement;

typedef struct command_sync_position
{
    BYTE        bHeader;
	WORD		wSize;
} TPacketCGSyncPosition;

typedef struct command_fly_targeting
{
	BYTE		bHeader;
	DWORD		dwTargetVID;
	long		lX;
	long		lY;
} TPacketCGFlyTargeting;

typedef struct packet_fly_targeting
{
    BYTE        bHeader;
	DWORD		dwShooterVID;
	DWORD		dwTargetVID;
	long		lX;
	long		lY;
} TPacketGCFlyTargeting;

typedef struct packet_shoot
{
    BYTE		bHeader;
    BYTE		bType;
} TPacketCGShoot;

typedef struct command_warp
{
	BYTE			bHeader;
} TPacketCGWarp;

enum
{
#ifdef ENABLE_MESSENGER_TEAM
	MESSENGER_SUBHEADER_GC_TEAM_LIST,
	MESSENGER_SUBHEADER_GC_TEAM_LOGIN,
	MESSENGER_SUBHEADER_GC_TEAM_LOGOUT,
#endif
	MESSENGER_SUBHEADER_GC_LIST,
	MESSENGER_SUBHEADER_GC_LOGIN,
	MESSENGER_SUBHEADER_GC_LOGOUT,
	MESSENGER_SUBHEADER_GC_INVITE,
	MESSENGER_SUBHEADER_GC_MOBILE,
};
typedef struct packet_messenger
{
    BYTE header;
    WORD size;
    BYTE subheader;
} TPacketGCMessenger;

typedef struct packet_messenger_list_offline
{
    BYTE connected; // always 0
	BYTE length;
} TPacketGCMessengerListOffline;

#ifdef ENABLE_MESSENGER_TEAM
typedef struct packet_messenger_team_list_offline
{
	BYTE connected; // always 0
	BYTE length;
} TPacketGCMessengerTeamListOffline;

typedef struct packet_messenger_team_list_online
{
	BYTE connected; // always 1
	BYTE length;
} TPacketGCMessengerTeamListOnline;
#endif

enum
{
	MESSENGER_CONNECTED_STATE_OFFLINE,
	MESSENGER_CONNECTED_STATE_ONLINE,
	MESSENGER_CONNECTED_STATE_MOBILE,
};

typedef struct packet_messenger_list_online
{
    BYTE connected;
	BYTE length;
	//BYTE length_char_name;
} TPacketGCMessengerListOnline;

typedef struct packet_messenger_login
{
	//BYTE length_login;
	//BYTE length_char_name;
	BYTE length;
} TPacketGCMessengerLogin;

typedef struct packet_messenger_logout
{
	BYTE length;
} TPacketGCMessengerLogout;

enum
{
    MESSENGER_SUBHEADER_CG_ADD_BY_VID,
    MESSENGER_SUBHEADER_CG_ADD_BY_NAME,
    MESSENGER_SUBHEADER_CG_REMOVE,
};

typedef struct command_messenger
{
    BYTE header;
    BYTE subheader;
} TPacketCGMessenger;

typedef struct command_messenger_remove
{
	BYTE length;
} TPacketCGMessengerRemove;

enum
{
	SAFEBOX_MONEY_STATE_SAVE,
	SAFEBOX_MONEY_STATE_WITHDRAW,
};

typedef struct command_safebox_money
{
    BYTE        bHeader;
    BYTE        bState;
    DWORD       dwMoney;
} TPacketCGSafeboxMoney;

typedef struct command_safebox_checkout
{
    BYTE        bHeader;
    BYTE        bSafePos;
    TItemPos	ItemPos;
} TPacketCGSafeboxCheckout;

typedef struct command_safebox_checkin
{
    BYTE        bHeader;
    BYTE        bSafePos;
    TItemPos	ItemPos;
} TPacketCGSafeboxCheckin;

typedef struct command_mall_checkout
{
    BYTE        bHeader;
    BYTE        bMallPos;
    TItemPos	ItemPos;
} TPacketCGMallCheckout;

///////////////////////////////////////////////////////////////////////////////////
// Party

typedef struct command_use_skill
{
    BYTE                bHeader;
    DWORD               dwVnum;
	DWORD				dwTargetVID;
} TPacketCGUseSkill;

typedef struct command_party_invite
{
    BYTE header;
    DWORD vid;
} TPacketCGPartyInvite;

typedef struct command_party_invite_answer
{
    BYTE header;
    DWORD leader_pid;
    BYTE accept;
} TPacketCGPartyInviteAnswer;

typedef struct command_party_remove
{
    BYTE header;
    DWORD pid;
} TPacketCGPartyRemove;

typedef struct command_party_set_state
{
    BYTE byHeader;
    DWORD dwVID;
	BYTE byState;
    BYTE byFlag;
} TPacketCGPartySetState;

typedef struct packet_party_link
{
    BYTE header;
    DWORD pid;
    DWORD vid;
} TPacketGCPartyLink;

typedef struct packet_party_unlink
{
    BYTE header;
    DWORD pid;
	DWORD vid;
} TPacketGCPartyUnlink;

typedef struct command_party_use_skill
{
    BYTE byHeader;
	BYTE bySkillIndex;
    DWORD dwTargetVID;
} TPacketCGPartyUseSkill;

enum
{
	GUILD_SUBHEADER_CG_ADD_MEMBER,
	GUILD_SUBHEADER_CG_REMOVE_MEMBER,
	GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME,
	GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY,
	GUILD_SUBHEADER_CG_OFFER,
	GUILD_SUBHEADER_CG_POST_COMMENT,
	GUILD_SUBHEADER_CG_DELETE_COMMENT,
	GUILD_SUBHEADER_CG_REFRESH_COMMENT,
	GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE,
	GUILD_SUBHEADER_CG_USE_SKILL,
	GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL,
	GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER,
	GUILD_SUBHEADER_CG_CHARGE_GSP,
	GUILD_SUBHEADER_CG_DEPOSIT_MONEY,
	GUILD_SUBHEADER_CG_WITHDRAW_MONEY,
};

typedef struct command_guild
{
    BYTE byHeader;
	BYTE bySubHeader;
} TPacketCGGuild;

typedef struct packet_give_items
{
	BYTE	bHeader;
} TPacketGCGiveItem;

typedef struct packet_give_items1
{
	BYTE	bHeader;
	DWORD	itemcount;
} TPacketCGGiveItems;

typedef struct command_guild_answer_make_guild
{
	BYTE header;
	char guild_name[GUILD_NAME_MAX_LEN+1];
} TPacketCGAnswerMakeGuild;

typedef struct command_give_item
{
	BYTE byHeader;
	DWORD dwTargetVID;
	TItemPos ItemPos;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	uint16_t byItemCount;
#else
	BYTE byItemCount;
#endif
} TPacketCGGiveItem;

typedef struct SPacketCGHack
{
    BYTE        bHeader;
    char        szBuf[255 + 1];
} TPacketCGHack;

typedef struct command_dungeon
{
	BYTE		bHeader;
	WORD		size;
} TPacketCGDungeon;

// Private Shop
typedef struct SShopItemTable
{
    DWORD		vnum;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	uint16_t count;
#else
	BYTE count;
#endif

    TItemPos	pos;
#ifdef ENABLE_LONG_LONG
	long long	price;
#else
	DWORD		price;
#endif
#ifdef ENABLE_CHEQUE_SYSTEM
	long long	cheque;
#endif
    BYTE		display_pos;
} TShopItemTable;

typedef struct SPacketCGMyShop
{
    BYTE        bHeader;
    char        szSign[SHOP_SIGN_MAX_LEN + 1];
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	uint16_t	bCount;
#else
	BYTE	bCount; // count of TShopItemTable, max 39
#endif
} TPacketCGMyShop;

typedef struct SPacketCGRefine
{
	BYTE		header;
	BYTE		pos;
	BYTE		type;
} TPacketCGRefine;

typedef struct SPacketCGChangeName
{
    BYTE header;
    BYTE index;
    char name[CHARACTER_NAME_MAX_LEN+1];
} TPacketCGChangeName;

typedef struct command_client_version
{
	BYTE header;
	char filename[32+1];
	char timestamp[32+1];
} TPacketCGClientVersion;

typedef struct command_client_version2
{
	BYTE header;
	char filename[32+1];
	char timestamp[32+1];
} TPacketCGClientVersion2;

typedef struct command_crc_report
{
	BYTE header;
	BYTE byPackMode;
	DWORD dwBinaryCRC32;
	DWORD dwProcessCRC32;
	DWORD dwRootPackCRC32;
} TPacketCGCRCReport;

enum EPartyExpDistributionType
{
    PARTY_EXP_DISTRIBUTION_NON_PARITY,
    PARTY_EXP_DISTRIBUTION_PARITY,
};

typedef struct command_party_parameter
{
    BYTE        bHeader;
    BYTE        bDistributeMode;
} TPacketCGPartyParameter;

typedef struct command_quest_input_string
{
    BYTE        bHeader;
    char		szString[QUEST_INPUT_STRING_MAX_NUM+1];
} TPacketCGQuestInputString;

typedef struct command_quest_confirm
{
    BYTE header;
    BYTE answer;
    DWORD requestPID;
} TPacketCGQuestConfirm;

typedef struct command_script_select_item
{
    BYTE header;
    DWORD selection;
} TPacketCGScriptSelectItem;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// From Server
enum EPhase
{
    PHASE_CLOSE,
    PHASE_HANDSHAKE,
    PHASE_LOGIN,
    PHASE_SELECT,
    PHASE_LOADING,
    PHASE_GAME,
    PHASE_DEAD,

	PHASE_DBCLIENT_CONNECTING,
    PHASE_DBCLIENT,
    PHASE_P2P,
    PHASE_AUTH,
};

typedef struct packet_phase
{
    BYTE        header;
    BYTE        phase;
} TPacketGCPhase;

typedef struct packet_blank
{
	BYTE		header;
} TPacketGCBlank;

typedef struct packet_blank_dynamic
{
	BYTE		header;
	WORD		size;
} TPacketGCBlankDynamic;

typedef struct packet_header_handshake
{
	BYTE		header;
	DWORD		dwHandshake;
	DWORD		dwTime;
	LONG		lDelta;
} TPacketGCHandshake;

typedef struct packet_header_bindudp
{
	BYTE		header;
	DWORD		addr;
	WORD		port;
} TPacketGCBindUDP;

typedef struct packet_header_dynamic_size
{
	BYTE		header;
	WORD		size;
} TDynamicSizePacketHeader;

#ifdef ENABLE_LARGE_DYNAMIC_PACKET
typedef struct SLargeDynamicSizePacketHeader
{
	BYTE		header;
	int			size;
} TLargeDynamicSizePacketHeader;
#endif

typedef struct SSimplePlayerInformation
{
    DWORD               dwID;
    char                szName[CHARACTER_NAME_MAX_LEN + 1];
    BYTE                byJob;
    BYTE                byLevel;
    DWORD               dwPlayMinutes;
    BYTE                byST, byHT, byDX, byIQ;
    DWORD               wMainPart; // @fixme502
    BYTE                bChangeName;
    DWORD				wHairPart; // @fixme502
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	DWORD				wAccePart;
#endif
#ifdef ENABLE_COSTUME_EMBLEMAT
	DWORD				wEmblematPart;
#endif
    BYTE                bDummy[4];
	long				x, y;
	LONG				lAddr;
	WORD				wPort;
	BYTE				bySkillGroup;
} TSimplePlayerInformation;

typedef struct packet_login_success3
{
	BYTE						header;
	TSimplePlayerInformation	akSimplePlayerInformation[PLAYER_PER_ACCOUNT3];
    DWORD						guild_id[PLAYER_PER_ACCOUNT3];
    char						guild_name[PLAYER_PER_ACCOUNT3][GUILD_NAME_MAX_LEN+1];
	DWORD handle;
	DWORD random_key;
} TPacketGCLoginSuccess3;

typedef struct packet_login_success4
{
	BYTE						header;
	TSimplePlayerInformation	akSimplePlayerInformation[PLAYER_PER_ACCOUNT4];
    DWORD						guild_id[PLAYER_PER_ACCOUNT4];
    char						guild_name[PLAYER_PER_ACCOUNT4][GUILD_NAME_MAX_LEN+1];
	DWORD handle;
	DWORD random_key;
} TPacketGCLoginSuccess4;

#ifdef ENABLE_PLAYER_PER_ACCOUNT5
typedef struct packet_login_success5
{
	BYTE						header;
	TSimplePlayerInformation	akSimplePlayerInformation[PLAYER_PER_ACCOUNT5];
    DWORD						guild_id[PLAYER_PER_ACCOUNT5];
    char						guild_name[PLAYER_PER_ACCOUNT5][GUILD_NAME_MAX_LEN+1];
	DWORD handle;
	DWORD random_key;
} TPacketGCLoginSuccess5;
#endif

enum { LOGIN_STATUS_MAX_LEN = 8 };
typedef struct packet_login_failure
{
	BYTE	header;
	char	szStatus[LOGIN_STATUS_MAX_LEN + 1];
} TPacketGCLoginFailure;

typedef struct command_player_create
{
	BYTE        header;
	BYTE        index;
	char        name[CHARACTER_NAME_MAX_LEN + 1];
	WORD        job;
	BYTE		shape;
	BYTE		CON;
	BYTE		INT;
	BYTE		STR;
	BYTE		DEX;
} TPacketCGCreateCharacter;

typedef struct command_player_create_success
{
    BYTE						header;
    BYTE						bAccountCharacterSlot;
    TSimplePlayerInformation	kSimplePlayerInfomation;
} TPacketGCPlayerCreateSuccess;

typedef struct command_create_failure
{
	BYTE	header;
	BYTE	bType;
} TPacketGCCreateFailure;

typedef struct command_player_delete
{
	BYTE        header;
	BYTE        index;
	char		szPrivateCode[PRIVATE_CODE_LENGTH];
} TPacketCGDestroyCharacter;

typedef struct packet_player_delete_success
{
	BYTE        header;
	BYTE        account_index;
} TPacketGCDestroyCharacterSuccess;

enum
{
	ADD_CHARACTER_STATE_DEAD   = (1 << 0),
	ADD_CHARACTER_STATE_SPAWN  = (1 << 1),
	ADD_CHARACTER_STATE_GUNGON = (1 << 2),
	ADD_CHARACTER_STATE_KILLER = (1 << 3),
	ADD_CHARACTER_STATE_PARTY  = (1 << 4),
};

enum EPKModes
{
	PK_MODE_PEACE,
	PK_MODE_REVENGE,
	PK_MODE_FREE,
	PK_MODE_PROTECT,
	PK_MODE_GUILD,
	PK_MODE_MAX_NUM,
};

enum ECharacterEquipmentPart
{
	CHR_EQUIPPART_ARMOR,
	CHR_EQUIPPART_WEAPON,
	CHR_EQUIPPART_HEAD,
	CHR_EQUIPPART_HAIR,
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	CHR_EQUIPPART_ACCE,
#endif
#ifdef ENABLE_AURA_SYSTEM
	CHR_EQUIPPART_AURA,
#endif
#ifdef ENABLE_COSTUME_EMBLEMAT
	CHR_EQUIPPART_EMBLEMAT,
#endif
	CHR_EQUIPPART_NUM,
};

typedef struct packet_char_additional_info
{
	BYTE    header;
	DWORD   dwVID;
	char    name[CHARACTER_NAME_MAX_LEN + 1];
	DWORD   awPart[CHR_EQUIPPART_NUM]; // @fixme502
	BYTE	bEmpire;
	DWORD   dwGuildID;
	DWORD   dwLevel;
#ifdef ENABLE_SECONDARY_LEVEL
	DWORD	dwSecondaryLevel;
#endif
	long long   sAlignment;
	BYTE    bPKMode;
	DWORD   dwMountVnum;
#ifdef ENABLE_QUIVER_SYSTEM
	DWORD	dwArrow;
#endif
#ifdef TITLE_SYSTEM_BYLUZER
	BYTE		title_active;
#endif
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
	uint8_t	bEnlightLvl;
#endif
} TPacketGCCharacterAdditionalInfo;

typedef struct packet_add_char
{
    BYTE        header;

    DWORD       dwVID;

    float       angle;
    long        x;
    long        y;
    long        z;

	BYTE		bType;
    DWORD       wRaceNum; // @fixme501
    BYTE        bMovingSpeed;
    BYTE        bAttackSpeed;

    BYTE        bStateFlag;
    DWORD       dwAffectFlag[2];
#ifdef TITLE_SYSTEM_BYLUZER
	BYTE		title_active;
#endif
} TPacketGCCharacterAdd;

typedef struct packet_add_char2
{
    BYTE        header;

    DWORD       dwVID;

    char        name[CHARACTER_NAME_MAX_LEN + 1];

    float       angle;
    long        x;
    long        y;
    long        z;

	BYTE		bType;
    DWORD       wRaceNum; // @fixme501
    DWORD       awPart[CHR_EQUIPPART_NUM]; // @fixme502
    BYTE        bMovingSpeed;
    BYTE        bAttackSpeed;

    BYTE        bStateFlag;
    DWORD       dwAffectFlag[2];
    BYTE        bEmpire;

    DWORD       dwGuild;
    long long       sAlignment;
	BYTE		bPKMode;
	DWORD		dwMountVnum;
#ifdef ENABLE_QUIVER_SYSTEM
	DWORD	dwArrow;
#endif
} TPacketGCCharacterAdd2;

typedef struct packet_update_char
{
    BYTE        header;
    DWORD       dwVID;

    DWORD		awPart[CHR_EQUIPPART_NUM]; // @fixme502
    BYTE        bMovingSpeed;
	BYTE		bAttackSpeed;

    BYTE        bStateFlag;
    DWORD       dwAffectFlag[2];

	DWORD		dwGuildID;
    long long       sAlignment;
#ifdef ENABLE_NEW_PET_SYSTEM
	DWORD        dwLevel;
#endif
	BYTE		bPKMode;
	DWORD		dwMountVnum;
#ifdef ENABLE_QUIVER_SYSTEM
	DWORD	dwArrow;
#endif
#ifdef TITLE_SYSTEM_BYLUZER
	BYTE	title_active;
#endif
} TPacketGCCharacterUpdate;

typedef struct packet_del_char
{
	BYTE	header;
    DWORD	dwVID;
#ifdef ENABLE_ENITY_CHARACTER_FIX
	bool fix_enity;
#endif
} TPacketGCCharacterDelete;

typedef struct packet_GlobalTime
{
	BYTE	header;
	float	GlobalTime;
} TPacketGCGlobalTime;

enum EChatType
{
	CHAT_TYPE_TALKING,
	CHAT_TYPE_INFO,
	CHAT_TYPE_NOTICE,
	CHAT_TYPE_PARTY,
	CHAT_TYPE_GUILD,
	CHAT_TYPE_COMMAND,
	CHAT_TYPE_SHOUT,
	CHAT_TYPE_WHISPER,
	CHAT_TYPE_BIG_NOTICE,
	CHAT_TYPE_MONARCH_NOTICE,
#ifdef ENABLE_DICE_SYSTEM
	CHAT_TYPE_DICE_INFO, //11
#endif
#if defined(ENABLE_CHATTING_WINDOW_RENEWAL)
	CHAT_TYPE_EXP_INFO,
	CHAT_TYPE_ITEM_INFO,
	CHAT_TYPE_MONEY_INFO,
#endif
	CHAT_TYPE_MAX_NUM,
};

typedef struct packet_chatting
{
	BYTE	header;
	WORD	size;
	BYTE	type;
	DWORD	dwVID;
	BYTE	bEmpire;
#if defined(__BL_CLIENT_LOCALE_STRING__)
	bool	bCanFormat;
#endif

} TPacketGCChat;

typedef struct packet_whisper
{
    BYTE        bHeader;
    WORD        wSize;
    BYTE        bType;
    char        szNameFrom[CHARACTER_NAME_MAX_LEN + 1];
#if defined(__BL_CLIENT_LOCALE_STRING__)
	bool		bCanFormat;
#endif
} TPacketGCWhisper;

typedef struct packet_stun
{
	BYTE		header;
	DWORD		vid;
} TPacketGCStun;

#ifdef RENEWAL_DEAD_PACKET
enum EReviveTypes
{
	REVIVE_TYPE_HERE,
	REVIVE_TYPE_TOWN,
	REVIVE_TYPE_AUTO_TOWN,
	REVIVE_TYPE_MAX
};
#endif
typedef struct packet_dead
{
#ifdef RENEWAL_DEAD_PACKET
	packet_dead() { memset(&t_d, 0, sizeof(t_d)); }
#endif
	BYTE	header;
	DWORD	vid;
#ifdef RENEWAL_DEAD_PACKET
	BYTE	t_d[REVIVE_TYPE_MAX];
#endif
} TPacketGCDead;

typedef struct packet_main_character
{
    BYTE        header;
    DWORD       dwVID;
	DWORD		wRaceNum; // @fixme501
    char        szName[CHARACTER_NAME_MAX_LEN + 1];
    long        lX, lY, lZ;
	BYTE		bySkillGroup;
} TPacketGCMainCharacter;

// SUPPORT_BGM
typedef struct packet_main_character2_empire
{
    BYTE        header;
    DWORD       dwVID;
	DWORD		wRaceNum; // @fixme501
    char        szName[CHARACTER_NAME_MAX_LEN + 1];
    long        lX, lY, lZ;
	BYTE		byEmpire;
	BYTE		bySkillGroup;
} TPacketGCMainCharacter2_EMPIRE;

typedef struct packet_main_character3_bgm
{
	enum
	{
		MUSIC_NAME_MAX_LEN = 24,
	};
    BYTE        header;
    DWORD       dwVID;
	DWORD		wRaceNum; // @fixme501
    char        szUserName[CHARACTER_NAME_MAX_LEN + 1];
	char        szBGMName[MUSIC_NAME_MAX_LEN + 1];
    long        lX, lY, lZ;
	BYTE		byEmpire;
	BYTE		bySkillGroup;
} TPacketGCMainCharacter3_BGM;

typedef struct packet_main_character4_bgm_vol
{
	enum
	{
		MUSIC_NAME_MAX_LEN = 24,
	};
    BYTE        header;
    DWORD       dwVID;
	DWORD		wRaceNum; // @fixme501
    char        szUserName[CHARACTER_NAME_MAX_LEN + 1];
	char        szBGMName[MUSIC_NAME_MAX_LEN + 1];
	float		fBGMVol;
    long        lX, lY, lZ;
	BYTE		byEmpire;
	BYTE		bySkillGroup;
} TPacketGCMainCharacter4_BGM_VOL;
// END_OF_SUPPORT_BGM

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
	POINT_ATT_POWER,		// 16 ...
	POINT_ATT_SPEED,            // 17
	POINT_EVADE_RATE,		// 18
	POINT_MOV_SPEED,            // 19
	POINT_DEF_GRADE,	// 20
	POINT_CASTING_SPEED,        // 21
	POINT_MAGIC_ATT_GRADE,      // 22
	POINT_MAGIC_DEF_GRADE,      // 23
	POINT_EMPIRE_POINT,         // 24
	POINT_LEVEL_STEP,           // 25
	POINT_STAT,                 // 26
	POINT_SUB_SKILL,		// 27
	POINT_SKILL,		// 28
	POINT_MIN_ATK,		// 29
	POINT_MAX_ATK,		// 30
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
	POINT_KILL_HP_RECOVER,		// 87

	POINT_IMMUNE_STUN,			// 88
	POINT_IMMUNE_SLOW,			// 89
	POINT_IMMUNE_FALL,			// 90
	//////////////////

	POINT_PARTY_ATT_GRADE,			// 91
	POINT_PARTY_DEF_GRADE,			// 92

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

	POINT_DMG_BONUS,
	POINT_FINAL_DMG_BONUS,

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
	POINT_ATTBONUS_PVM_SKILL, //250
	POINT_ATTBONUS_ELEMENT, //250
	POINT_DEFBONUS_ELEMENT, //250
	POINT_DOUBLE_LOOT_SUMMER, //250
	POINT_DOUBLE_LOOT_DROP, //250

	POINT_MIN_WEP = 200,
	POINT_MAX_WEP,
	POINT_MIN_MAGIC_WEP,
	POINT_MAX_MAGIC_WEP,
	POINT_HIT_RATE,
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

//#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
//	POINT_PRIVATE_SHOP_UNLOCKED_SLOT,
//#endif

	POINT_MAX,
};


typedef struct packet_points
{
    BYTE        header;
#ifdef ENABLE_LONG_LONG
	long long   points[POINT_MAX_NUM];
#else
    int        points[POINT_MAX_NUM];
#endif
} TPacketGCPoints;

typedef struct packet_point_change
{
    int         header;

	DWORD		dwVID;
	BYTE		Type;

#ifdef ENABLE_LONG_LONG
	long long	amount;
	long long	value;
#else
	long        amount; // �ٲ� ��
    long        value;  // ���� ��
#endif
} TPacketGCPointChange;

typedef struct packet_motion
{
	BYTE		header;
	DWORD		vid;
	DWORD		victim_vid;
	WORD		motion;
} TPacketGCMotion;

typedef struct packet_set_item
{
	BYTE		header;
	TItemPos	Cell;
	DWORD		vnum;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	uint16_t		count;
#else
	BYTE		count;
#endif
	long		alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
    TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
} TPacketGCItemSet;

typedef struct packet_set_item2
{
	BYTE		header;
	TItemPos	Cell;
	DWORD		vnum;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	uint16_t		count;
#else
	BYTE		count;
#endif
	DWORD		flags;
	DWORD		anti_flags;
	bool		highlight;
	long		alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
    TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
} TPacketGCItemSet2;

typedef struct packet_item_del
{
    BYTE        header;
    BYTE        pos;
} TPacketGCItemDel;

typedef struct packet_use_item
{
	BYTE		header;
	TItemPos	Cell;
	DWORD		ch_vid;
	DWORD		victim_vid;

	DWORD		vnum;
} TPacketGCItemUse;

typedef struct packet_update_item
{
	BYTE		header;
	TItemPos	Cell;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	uint16_t count;
#else
	BYTE count;
#endif
	long		alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
    TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
} TPacketGCItemUpdate;

typedef struct packet_ground_add_item
{
#ifdef ENABLE_EXTENDED_ITEMNAME_ON_GROUND
	packet_ground_add_item()
	{
		memset(&alSockets, 0, sizeof(alSockets));
		memset(&aAttrs, 0, sizeof(aAttrs));
	}
#endif

	BYTE	bHeader;
	long	lX;
	long	lY;
	long	lZ;
	DWORD	dwVID;
	DWORD	dwVnum;
	int		count;

#ifdef ENABLE_EXTENDED_ITEMNAME_ON_GROUND
	long	alSockets[ITEM_SOCKET_SLOT_MAX_NUM];//3 
	TPlayerItemAttribute aAttrs[ITEM_ATTRIBUTE_SLOT_MAX_NUM];//7
#endif
} TPacketGCItemGroundAdd;

typedef struct packet_ground_del_item
{
	BYTE		header;
	DWORD		vid;
} TPacketGCItemGroundDel;

typedef struct packet_item_ownership
{
    BYTE        bHeader;
    DWORD       dwVID;
    char        szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCItemOwnership;

typedef struct packet_quickslot_add
{
    BYTE        header;
    BYTE        pos;
	TQuickSlot	slot;
} TPacketGCQuickSlotAdd;

typedef struct packet_quickslot_del
{
    BYTE        header;
    BYTE        pos;
} TPacketGCQuickSlotDel;

typedef struct packet_quickslot_swap
{
    BYTE        header;
    BYTE        pos;
    BYTE        change_pos;
} TPacketGCQuickSlotSwap;

typedef struct packet_shop_start
{
	//DWORD		IsOnline;
	struct packet_shop_item		items[SHOP_HOST_ITEM_MAX_NUM];

#ifdef ENABLE_PUNKTY_OSIAGNIEC
	bool	ispktosiagshop;
#endif
} TPacketGCShopStart;

typedef struct packet_shop_start_ex
{
	typedef struct sub_packet_shop_tab
	{
		char name[SHOP_TAB_NAME_MAX];
		BYTE coin_type;
		packet_shop_item items[SHOP_HOST_ITEM_MAX_NUM];
	} TSubPacketShopTab;
	DWORD owner_vid;
	BYTE shop_tab_count;
} TPacketGCShopStartEx;

typedef struct packet_shop_update_item
{
	BYTE						pos;
	struct packet_shop_item		item;
} TPacketGCShopUpdateItem;

typedef struct packet_shop_update_price
{
#ifdef ENABLE_LONG_LONG
	long long iElkAmount;
#else
	int iElkAmount;
#endif
} TPacketGCShopUpdatePrice;

enum EPacketShopSubHeaders
{
	SHOP_SUBHEADER_GC_START,
    SHOP_SUBHEADER_GC_END,
	SHOP_SUBHEADER_GC_UPDATE_ITEM,
	SHOP_SUBHEADER_GC_UPDATE_PRICE,
    SHOP_SUBHEADER_GC_OK,
    SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY,
#ifdef ENABLE_CHEQUE_SYSTEM
	SHOP_SUBHEADER_GC_NOT_ENOUGH_CHEQUE,
#endif
    SHOP_SUBHEADER_GC_SOLDOUT,
    SHOP_SUBHEADER_GC_INVENTORY_FULL,
    SHOP_SUBHEADER_GC_INVALID_POS,
	SHOP_SUBHEADER_GC_SOLD_OUT,
	SHOP_SUBHEADER_GC_START_EX,
	SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY_EX,
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	SHOP_SUBHEADER_GC_UPDATE_ITEM2,
	SHOP_SUBHEADER_GC_REFRESH_MONEY,
	SHOP_SUBHEADER_GC_REFRESH_CHEQUE,
#endif
#ifdef ENABLE_CHEQUE_SYSTEM
	SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY_CHEQUE,
#endif
#ifdef ENABLE_PUNKTY_OSIAGNIEC
	SHOP_SUBHEADER_GC_NOT_ENOUGH_PKT_OSIAG
#endif
};

typedef struct packet_shop
{
	BYTE        header;
	WORD		size;
	BYTE        subheader;
} TPacketGCShop;

typedef struct packet_exchange
{
    BYTE        header;
    BYTE        subheader;
    BYTE        is_me;
#ifdef ENABLE_LONG_LONG
	long long	arg1;
#else
	DWORD		arg1;
#endif
    TItemPos       arg2;
    DWORD       arg3;
#ifdef WJ_ENABLE_TRADABLE_ICON
	TItemPos       arg4;
#endif
	long		alValues[ITEM_SOCKET_SLOT_MAX_NUM];
    TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
} TPacketGCExchange;

enum
{
    EXCHANGE_SUBHEADER_GC_START,			// arg1 == vid
    EXCHANGE_SUBHEADER_GC_ITEM_ADD,			// arg1 == vnum  arg2 == pos  arg3 == count
	EXCHANGE_SUBHEADER_GC_ITEM_DEL,			// arg1 == pos
    EXCHANGE_SUBHEADER_GC_ELK_ADD,			// arg1 == elk
#ifdef ENABLE_CHEQUE_SYSTEM
	EXCHANGE_SUBHEADER_GC_CHEQUE_ADD,
#endif
    EXCHANGE_SUBHEADER_GC_ACCEPT,			// arg1 == accept
    EXCHANGE_SUBHEADER_GC_END,				// arg1 == not used
    EXCHANGE_SUBHEADER_GC_ALREADY,			// arg1 == not used
    EXCHANGE_SUBHEADER_GC_LESS_ELK,			// arg1 == not used
#ifdef ENABLE_CHEQUE_SYSTEM
	EXCHANGE_SUBHEADER_GC_LESS_CHEQUE,
#endif
};

typedef struct packet_position
{
    BYTE        header;
	DWORD		vid;
    BYTE        position;
} TPacketGCPosition;

typedef struct packet_ping
{
	BYTE		header;
} TPacketGCPing;

typedef struct packet_pong
{
	BYTE		bHeader;
	DWORD		dwPongValue;
} TPacketCGPong;

typedef struct packet_script
{
    BYTE		header;
    WORD        size;
	BYTE		skin;
    WORD        src_size;
} TPacketGCScript;

typedef struct packet_target
{
	BYTE	header;
	DWORD	dwVID;
	//BYTE	bHPPercent;
#ifdef ENABLE_VIEW_TARGET_DECIMAL_HP
	long long	iMinHP;
	long long	iMaxHP;
#endif
} TPacketGCTarget;

typedef struct packet_damage_info
{
	BYTE header;
	DWORD dwVID;
	BYTE flag;
	int  damage;
} TPacketGCDamageInfo;

typedef struct packet_mount
{
    BYTE        header;
    DWORD       vid;
    DWORD       mount_vid;
    BYTE        pos;
	DWORD		_x, _y;
} TPacketGCMount;

typedef struct packet_change_speed
{
	BYTE		header;
	DWORD		vid;
	WORD		moving_speed;
} TPacketGCChangeSpeed;

typedef struct packet_move
{
	BYTE		bHeader;
	BYTE		bFunc;
	BYTE		bArg;
	BYTE		bRot;
	DWORD		dwVID;
	LONG		lX;
	LONG		lY;
	DWORD		dwTime;
	DWORD		dwDuration;
} TPacketGCMove;

enum
{
	QUEST_SEND_IS_BEGIN         = 1 << 0,
    QUEST_SEND_TITLE            = 1 << 1,
    QUEST_SEND_CLOCK_NAME       = 1 << 2,
    QUEST_SEND_CLOCK_VALUE      = 1 << 3,
    QUEST_SEND_COUNTER_NAME     = 1 << 4,
    QUEST_SEND_COUNTER_VALUE    = 1 << 5,
	QUEST_SEND_ICON_FILE		= 1 << 6,
};

typedef struct packet_quest_info
{
	BYTE header;
	WORD size;
	WORD index;
#ifdef ENABLE_QUEST_RENEWAL
	WORD c_index;
#endif
	BYTE flag;
} TPacketGCQuestInfo;

typedef struct packet_quest_confirm
{
    BYTE header;
    char msg[64+1];
    long timeout;
    DWORD requestPID;
} TPacketGCQuestConfirm;

typedef struct packet_attack
{
    BYTE        header;
    DWORD       dwVID;
    DWORD       dwVictimVID;
    BYTE        bType;
} TPacketGCAttack;

typedef struct packet_c2c
{
	BYTE		header;
	WORD		wSize;
} TPacketGCC2C;

typedef struct packetd_sync_position_element
{
    DWORD       dwVID;
    long        lX;
    long        lY;
} TPacketGCSyncPositionElement;

typedef struct packetd_sync_position
{
    BYTE        bHeader;
    WORD		wSize;
} TPacketGCSyncPosition;

typedef struct packet_ownership
{
    BYTE                bHeader;
    DWORD               dwOwnerVID;
    DWORD               dwVictimVID;
} TPacketGCOwnership;

#define	SKILL_MAX_NUM 255

typedef struct packet_skill_level
{
    BYTE        bHeader;
    BYTE        abSkillLevels[SKILL_MAX_NUM];
} TPacketGCSkillLevel;

typedef struct SPlayerSkill
{
	BYTE bMasterType;
	BYTE bLevel;
	int32_t tNextRead;
} TPlayerSkill;

typedef struct packet_skill_level_new
{
	BYTE bHeader;
	TPlayerSkill skills[SKILL_MAX_NUM];
} TPacketGCSkillLevelNew;

// fly
typedef struct packet_fly
{
    BYTE        bHeader;
    BYTE        bType;
    DWORD       dwStartVID;
    DWORD       dwEndVID;
} TPacketGCCreateFly;

enum EPVPModes
{
	PVP_MODE_NONE,
    PVP_MODE_AGREE,
    PVP_MODE_FIGHT,
    PVP_MODE_REVENGE,
};

typedef struct packet_duel_start
{
    BYTE	header ;
    WORD	wSize ;
} TPacketGCDuelStart ;

typedef struct packet_pvp
{
	BYTE		header;
	DWORD		dwVIDSrc;
	DWORD		dwVIDDst;
	BYTE		bMode;
} TPacketGCPVP;

typedef struct packet_skill_cooltime_end
{
	BYTE		header;
	BYTE		bSkill;
} TPacketGCSkillCoolTimeEnd;

typedef struct packet_warp
{
	BYTE			bHeader;
	LONG			lX;
	LONG			lY;
	LONG			lAddr;
	WORD			wPort;
} TPacketGCWarp;

typedef struct packet_party_invite
{
    BYTE header;
    DWORD leader_pid;
} TPacketGCPartyInvite;

typedef struct packet_party_add
{
    BYTE header;
    DWORD pid;
    char name[CHARACTER_NAME_MAX_LEN+1];
} TPacketGCPartyAdd;

typedef struct packet_party_update
{
    BYTE header;
    DWORD pid;
    BYTE state;
    BYTE percent_hp;
    short affects[PARTY_AFFECT_SLOT_MAX_NUM];
#if defined(__BL_PARTY_POSITION__)
	long x;
	long y;
#endif
} TPacketGCPartyUpdate;

typedef struct packet_party_remove
{
    BYTE header;
    DWORD pid;
} TPacketGCPartyRemove;

typedef TPacketCGSafeboxCheckout TPacketGCSafeboxCheckout;
typedef TPacketCGSafeboxCheckin TPacketGCSafeboxCheckin;

typedef struct packet_safebox_wrong_password
{
    BYTE        bHeader;
} TPacketGCSafeboxWrongPassword;

typedef struct packet_safebox_size
{
	BYTE bHeader;
	BYTE bSize;
} TPacketGCSafeboxSize;

typedef struct packet_safebox_money_change
{
    BYTE bHeader;
    DWORD dwMoney;
} TPacketGCSafeboxMoneyChange;

typedef struct command_empire
{
    BYTE        bHeader;
    BYTE        bEmpire;
} TPacketCGEmpire;

typedef struct packet_empire
{
    BYTE        bHeader;
    BYTE        bEmpire;
} TPacketGCEmpire;

enum
{
	FISHING_SUBHEADER_GC_START,
	FISHING_SUBHEADER_GC_STOP,
	FISHING_SUBHEADER_GC_REACT,
	FISHING_SUBHEADER_GC_SUCCESS,
	FISHING_SUBHEADER_GC_FAIL,
    FISHING_SUBHEADER_GC_FISH,
};

typedef struct packet_fishing
{
    BYTE header;
    BYTE subheader;
    DWORD info;
    BYTE dir;
} TPacketGCFishing;

typedef struct paryt_parameter
{
    BYTE        bHeader;
    BYTE        bDistributeMode;
} TPacketGCPartyParameter;

//////////////////////////////////////////////////////////////////////////
// Guild

enum
{
    GUILD_SUBHEADER_GC_LOGIN,
	GUILD_SUBHEADER_GC_LOGOUT,
	GUILD_SUBHEADER_GC_LIST,
	GUILD_SUBHEADER_GC_GRADE,
	GUILD_SUBHEADER_GC_ADD,
	GUILD_SUBHEADER_GC_REMOVE,
	GUILD_SUBHEADER_GC_GRADE_NAME,
	GUILD_SUBHEADER_GC_GRADE_AUTH,
	GUILD_SUBHEADER_GC_INFO,
	GUILD_SUBHEADER_GC_COMMENTS,
    GUILD_SUBHEADER_GC_CHANGE_EXP,
    GUILD_SUBHEADER_GC_CHANGE_MEMBER_GRADE,
	GUILD_SUBHEADER_GC_SKILL_INFO,
	GUILD_SUBHEADER_GC_CHANGE_MEMBER_GENERAL,
	GUILD_SUBHEADER_GC_GUILD_INVITE,
    GUILD_SUBHEADER_GC_WAR,
    GUILD_SUBHEADER_GC_GUILD_NAME,
    GUILD_SUBHEADER_GC_GUILD_WAR_LIST,
    GUILD_SUBHEADER_GC_GUILD_WAR_END_LIST,
    GUILD_SUBHEADER_GC_WAR_POINT,
	GUILD_SUBHEADER_GC_MONEY_CHANGE,
};

typedef struct packet_guild
{
    BYTE header;
    WORD size;
    BYTE subheader;
} TPacketGCGuild;

// SubHeader - Grade
enum
{
    GUILD_AUTH_ADD_MEMBER       = (1 << 0),
    GUILD_AUTH_REMOVE_MEMBER    = (1 << 1),
    GUILD_AUTH_NOTICE           = (1 << 2),
    GUILD_AUTH_SKILL            = (1 << 3),
};

typedef struct packet_guild_sub_grade
{
	char grade_name[GUILD_GRADE_NAME_MAX_LEN+1];
	BYTE auth_flag;
} TPacketGCGuildSubGrade;

typedef struct packet_guild_sub_member
{
	DWORD pid;
	BYTE byGrade;
	BYTE byIsGeneral;
	BYTE byJob;
	BYTE byLevel;
	DWORD dwOffer;
	BYTE byNameFlag;
// if NameFlag is TRUE, name is sent from server.
//	char szName[CHARACTER_ME_MAX_LEN+1];
} TPacketGCGuildSubMember;

typedef struct packet_guild_sub_info
{
    WORD member_count;
    WORD max_member_count;
	DWORD guild_id;
    DWORD master_pid;
    DWORD exp;
    BYTE level;
    char name[GUILD_NAME_MAX_LEN+1];
	DWORD gold;
	BYTE hasLand;
} TPacketGCGuildInfo;

enum EGuildWarState
{
    GUILD_WAR_NONE,
    GUILD_WAR_SEND_DECLARE,
    GUILD_WAR_REFUSE,
    GUILD_WAR_RECV_DECLARE,
    GUILD_WAR_WAIT_START,
    GUILD_WAR_CANCEL,
    GUILD_WAR_ON_WAR,
    GUILD_WAR_END,

    GUILD_WAR_DURATION = 2*60*60,
};

typedef struct packet_guild_war
{
    DWORD       dwGuildSelf;
    DWORD       dwGuildOpp;
    BYTE        bType;
    BYTE        bWarState;
} TPacketGCGuildWar;

typedef struct SPacketGuildWarPoint
{
    DWORD dwGainGuildID;
    DWORD dwOpponentGuildID;
    long lPoint;
} TPacketGuildWarPoint;

// SubHeader - Dungeon
enum
{
	DUNGEON_SUBHEADER_GC_TIME_ATTACK_START = 0,
	DUNGEON_SUBHEADER_GC_DESTINATION_POSITION = 1,
};

typedef struct packet_dungeon
{
	BYTE		bHeader;
    WORD		size;
    BYTE		subheader;
} TPacketGCDungeon;

#ifdef ENABLE_ODLAMKI_SYSTEM
enum
{
	HEADER_CG_ODLAMKI_ITEM = 224,
	HEADER_GC_ODLAMKI_ITEM = 225,
};

typedef struct SPacketGCOdlamki
{
	BYTE	header;
} TPacketGCOdlamki;

typedef struct SPacketCGOdlamki {
	BYTE	header;
	BYTE	item_count;
} TPacketCGOdlamki;
#endif

#ifdef ENABLE_VS_SHOP_SEARCH
enum
{
	HEADER_CG_SEARCH = 226,
};

typedef struct SPacketCGSearch {
	BYTE	header;
	BYTE	items;
} TPacketCGSearch;
#endif

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
typedef struct ShopLog {
	DWORD	itemVnum;
	int		itemCount;
	long long	price;
	long long	price2;
	char		date[20];
	char		action[5];
}TShopLog;
typedef struct command_offlineshop_logs_packet
{
	BYTE bHeader;
	ShopLog		log[20];
} TPacketGCOfflineShopLogs;
#endif

// Private Shop
typedef struct SPacketGCShopSign
{
    BYTE        bHeader;
    DWORD       dwVID;
    char        szSign[SHOP_SIGN_MAX_LEN + 1];
} TPacketGCShopSign;

typedef struct SPacketGCTime
{
    BYTE        bHeader;
    int64_t      time;
} TPacketGCTime;

enum
{
    WALKMODE_RUN,
    WALKMODE_WALK,
};

typedef struct SPacketGCWalkMode
{
    BYTE        header;
    DWORD       vid;
    BYTE        mode;
} TPacketGCWalkMode;

typedef struct SPacketGCChangeSkillGroup
{
    BYTE        header;
    BYTE        skill_group;
} TPacketGCChangeSkillGroup;

struct TMaterial
{
    DWORD vnum;
    DWORD count;
};

typedef struct SRefineTable
{
    DWORD src_vnum;
    DWORD result_vnum;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	uint16_t material_count;
#else
	BYTE material_count;
#endif
    int cost;
	int cost2;
	int cost3;
    int prob;
	uint8_t probExtra;
    TMaterial materials[REFINE_MATERIAL_MAX_NUM];
} TRefineTable;

typedef struct SPacketGCRefineInformation
{
	BYTE			header;
	BYTE			pos;
	TRefineTable	refine_table;
} TPacketGCRefineInformation;

typedef struct SPacketGCRefineInformationNew
{
	BYTE			header;
	BYTE			type;
	BYTE			pos;
	TRefineTable	refine_table;
} TPacketGCRefineInformationNew;

enum SPECIAL_EFFECT
{
	SE_NONE,
	SE_HPUP_RED,
	SE_SPUP_BLUE,
	SE_SPEEDUP_GREEN,
	SE_DXUP_PURPLE,
	SE_CRITICAL,
	SE_PENETRATE,
	SE_BLOCK,
	SE_DODGE,
	SE_CHINA_FIREWORK,
	SE_SPIN_TOP,
	SE_SUCCESS,
	SE_FAIL,
	SE_FR_SUCCESS,
    SE_LEVELUP_ON_14_FOR_GERMANY,
    SE_LEVELUP_UNDER_15_FOR_GERMANY,
    SE_PERCENT_DAMAGE1,
    SE_PERCENT_DAMAGE2,
    SE_PERCENT_DAMAGE3,
	SE_AUTO_HPUP,
	SE_AUTO_SPUP,
	SE_EQUIP_RAMADAN_RING,
	SE_EQUIP_HALLOWEEN_CANDY,
	SE_EQUIP_HAPPINESS_RING,
	SE_EQUIP_LOVE_PENDANT,
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	SE_EFFECT_ACCE_SUCCEDED,
	SE_EFFECT_ACCE_EQUIP,
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
	SE_PET_LV_UP,
#endif
#ifdef ENABLE_SYSTEM_RUNE
	SE_RUNA_WHITE_EFFECT,
	SE_RUNA_RED_EFFECT,
	SE_RUNA_BLUE_EFFECT,
	SE_RUNA_YELLOW_EFFECT,
	SE_RUNA_BLACK_EFFECT,
	SE_RUNA_GREEN_EFFECT,
#endif
	SE_EFFECT_FISH1,
	SE_EFFECT_FISH2,
	SE_EFFECT_FISH3,
	SE_EFFECT_FISH4,
};

typedef struct SPacketGCSpecialEffect
{
    BYTE header;
    BYTE type;
    DWORD vid;
} TPacketGCSpecialEffect;

typedef struct SPacketGCNPCPosition
{
    BYTE header;
	WORD size;
    WORD count;
} TPacketGCNPCPosition;

struct TNPCPosition
{
    BYTE bType;
    char name[CHARACTER_NAME_MAX_LEN+1];
    long x;
    long y;
};

typedef struct SPacketGCChangeName
{
    BYTE header;
    DWORD pid;
    char name[CHARACTER_NAME_MAX_LEN+1];
} TPacketGCChangeName;

enum EBlockAction
{
    BLOCK_EXCHANGE              = (1 << 0),
    BLOCK_PARTY_INVITE          = (1 << 1),
    BLOCK_GUILD_INVITE          = (1 << 2),
    BLOCK_WHISPER               = (1 << 3),
    BLOCK_MESSENGER_INVITE      = (1 << 4),
    BLOCK_PARTY_REQUEST         = (1 << 5),
};

typedef struct packet_login_key
{
	BYTE	bHeader;
	DWORD	dwLoginKey;
} TPacketGCLoginKey;

typedef struct packet_auth_success
{
    BYTE        bHeader;
    DWORD       dwLoginKey;
    BYTE        bResult;
} TPacketGCAuthSuccess;

typedef struct packet_channel
{
    BYTE header;
    BYTE channel;
} TPacketGCChannel;

typedef struct SEquipmentItemSet
{
	DWORD   vnum;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	int	count;
#else
	BYTE	count;
#endif
	long    alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
} TEquipmentItemSet;

typedef struct pakcet_view_equip
{
    BYTE header;
	DWORD dwVID;
	TEquipmentItemSet equips[WEAR_MAX_NUM];
} TPacketGCViewEquip;

typedef struct SLandPacketElement
{
    DWORD       dwID;
    long        x, y;
    long        width, height;
    DWORD       dwGuildID;
} TLandPacketElement;

typedef struct packet_land_list
{
    BYTE        header;
    WORD        size;
} TPacketGCLandList;

typedef struct SPacketGCTargetCreate
{
    BYTE        bHeader;
    long        lID;
    char        szTargetName[32+1];
} TPacketGCTargetCreate;

enum
{
	CREATE_TARGET_TYPE_NONE,
	CREATE_TARGET_TYPE_LOCATION,
	CREATE_TARGET_TYPE_CHARACTER,
};

typedef struct SPacketGCTargetCreateNew
{
	BYTE		bHeader;
	long		lID;
	char		szTargetName[32+1];
	DWORD		dwVID;
	BYTE		byType;
} TPacketGCTargetCreateNew;

typedef struct SPacketGCTargetUpdate
{
    BYTE        bHeader;
    long        lID;
    long        lX, lY;
} TPacketGCTargetUpdate;

typedef struct SPacketGCTargetDelete
{
    BYTE        bHeader;
    long        lID;
} TPacketGCTargetDelete;

typedef struct SPacketAffectElement
{
    DWORD       dwType;
    BYTE        bPointIdxApplyOn;
    long        lApplyValue;
    DWORD       dwFlag;
    long        lDuration;
    long        lSPCost;
} TPacketAffectElement;

typedef struct SPacketGCAffectAdd
{
    BYTE bHeader;
    TPacketAffectElement elem;
} TPacketGCAffectAdd;

typedef struct SPacketGCAffectRemove
{
    BYTE bHeader;
    DWORD dwType;
    BYTE bApplyOn;
} TPacketGCAffectRemove;

typedef struct packet_mall_open
{
	BYTE bHeader;
	BYTE bSize;
} TPacketGCMallOpen;

typedef struct packet_lover_info
{
	BYTE bHeader;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE byLovePoint;
} TPacketGCLoverInfo;

typedef struct packet_love_point_update
{
	BYTE bHeader;
	BYTE byLovePoint;
} TPacketGCLovePointUpdate;

typedef struct packet_dig_motion
{
    BYTE header;
    DWORD vid;
    DWORD target_vid;
	BYTE count;
} TPacketGCDigMotion;

typedef struct SPacketGCOnTime
{
    BYTE header;
    int ontime;     // sec
} TPacketGCOnTime;

typedef struct SPacketGCResetOnTime
{
    BYTE header;
} TPacketGCResetOnTime;

typedef struct SPacketGCPanamaPack
{
    BYTE    bHeader;
    char    szPackName[256];
    BYTE    abIV[32];
} TPacketGCPanamaPack;

typedef struct SPacketGCHybridCryptKeys
{
private:
	SPacketGCHybridCryptKeys() : m_pStream(NULL) {}

public:
	SPacketGCHybridCryptKeys(int iStreamSize) : iKeyStreamLen(iStreamSize)
	{
		m_pStream = new BYTE[iStreamSize];
	}
	~SPacketGCHybridCryptKeys()
	{
		if( m_pStream )
		{
			delete[] m_pStream;
			m_pStream = NULL;
		}
	}
	static int GetFixedHeaderSize()
	{
		return sizeof(BYTE)+sizeof(WORD)+sizeof(int);
	}

	BYTE	bHeader;
	WORD    wDynamicPacketSize;
	int		iKeyStreamLen;
	BYTE*	m_pStream;
} TPacketGCHybridCryptKeys;

typedef struct SPacketGCHybridSDB
{
private:
	SPacketGCHybridSDB() : m_pStream(NULL) {}

public:
	SPacketGCHybridSDB(int iStreamSize) : iSDBStreamLen(iStreamSize)
	{
		m_pStream = new BYTE[iStreamSize];
	}
	~SPacketGCHybridSDB()
	{
		delete[] m_pStream;
		m_pStream = NULL;
	}
	static int GetFixedHeaderSize()
	{
		return sizeof(BYTE)+sizeof(WORD)+sizeof(int);
	}

	BYTE	bHeader;
	WORD    wDynamicPacketSize;
	int		iSDBStreamLen;
	BYTE*	m_pStream;
} TPacketGCHybridSDB;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Client To Client

typedef struct packet_state
{
	BYTE			bHeader;
	BYTE			bFunc;
	BYTE			bArg;
	BYTE			bRot;
	DWORD			dwVID;
	DWORD			dwTime;
	TPixelPosition	kPPos;
} TPacketCCState;

// AUTOBAN
typedef struct packet_autoban_quiz
{
    BYTE bHeader;
	BYTE bDuration;
    BYTE bCaptcha[64*32];
    char szQuiz[256];
} TPacketGCAutoBanQuiz;
// END_OF_AUTOBAN

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
typedef struct SOfflineShopItemTable
{
	BYTE		bDisplayPos;
	TItemPos	itemPos;
	long long	lPrice;
	long long	lPrice2;
} TOfflineShopItemTable;

typedef struct TPacketCGMyOfflineShop
{
	BYTE	bHeader;
	char	szSign[SHOP_SIGN_MAX_LEN + 1];
	int		bCount;
} TPacketCGMyOfflineShop;

typedef struct packet_offline_shop_start
{
	DWORD	IsOnline;
	struct packet_shop_item		items[OFFLINE_SHOP_HOST_ITEM_MAX_NUM];
} TPacketGCOfflineShopStart;

typedef struct packet_offline_shop_money
{
	long long	dwMoney;
	long long	dwCheque;
	long long	zarobekYang;
	long long	zarobekCheque;
} TPacketGCOfflineShopMoney;
#endif

typedef struct packet_anty_exp_info
{
	BYTE	bHeader;
	DWORD	status;
} TPacketGCAntyExp;

#ifdef WJ_SPLIT_INVENTORY_SYSTEM
typedef struct packet_wpadanie_info
{
	BYTE	bHeader;
	DWORD	status;
} TPacketGCWpadanie;
#endif

#ifdef _IMPROVED_PACKET_ENCRYPTION_
struct TPacketKeyAgreement
{
	static const int MAX_DATA_LEN = 256;
	BYTE bHeader;
	WORD wAgreedLength;
	WORD wDataLength;
	BYTE data[MAX_DATA_LEN];
};

struct TPacketKeyAgreementCompleted
{
	BYTE bHeader;
	BYTE data[3]; // dummy (not used)
};
#endif // _IMPROVED_PACKET_ENCRYPTION_

typedef struct SPacketGCSpecificEffect
{
	BYTE header;
	DWORD vid;
	char effect_file[128];
} TPacketGCSpecificEffect;

enum EDragonSoulRefineWindowRefineType
{
	DragonSoulRefineWindow_UPGRADE,
	DragonSoulRefineWindow_IMPROVEMENT,
	DragonSoulRefineWindow_REFINE,
};

enum EPacketCGDragonSoulSubHeaderType
{
	DS_SUB_HEADER_OPEN,
	DS_SUB_HEADER_CLOSE,
	DS_SUB_HEADER_DO_UPGRADE,
	DS_SUB_HEADER_DO_IMPROVEMENT,
	DS_SUB_HEADER_DO_REFINE,
	DS_SUB_HEADER_REFINE_FAIL,
	DS_SUB_HEADER_REFINE_FAIL_MAX_REFINE,
	DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL,
	DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MONEY,
	DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MATERIAL,
	DS_SUB_HEADER_REFINE_FAIL_TOO_MUCH_MATERIAL,
	DS_SUB_HEADER_REFINE_SUCCEED,
};

typedef struct SPacketCGDragonSoulRefine
{
	SPacketCGDragonSoulRefine() : header (HEADER_CG_DRAGON_SOUL_REFINE)
	{}
	BYTE header;
	BYTE bSubType;
	TItemPos ItemGrid[DS_REFINE_WINDOW_MAX_NUM];
} TPacketCGDragonSoulRefine;

typedef struct SPacketGCDragonSoulRefine
{
	SPacketGCDragonSoulRefine() : header(HEADER_GC_DRAGON_SOUL_REFINE)
	{}
	BYTE header;
	BYTE bSubType;
	TItemPos Pos;
} TPacketGCDragonSoulRefine;

typedef struct SChannelStatus
{
	WORD nPort; // @fixme024 (max port limit 32767 to 65535)
	BYTE bStatus;
} TChannelStatus;

#ifdef ENABLE_REMOVE_ITEM
enum ERemoveItemConfig
{
	HEADER_GC_REMOVE_ITEM = 57,
	HEADER_CG_REMOVE_ITEM = 68,
	REMOVE_ITEM_SLOT_COUNT_X = 8,
	REMOVE_ITEM_SLOT_COUNT_Y = 6,
};

typedef struct SPacketGCRemoveItem
{
	BYTE	header;
} TPacketGCRemoveItem;

typedef struct SPacketCGRemoveItem
{
	BYTE	header;
	BYTE	item_count;
} TPacketCGRemoveItem;
#endif

#ifdef ENABLE_RESP_SYSTEM
enum ERespConfig
{
	HEADER_GC_RESP = 158,
	HEADER_CG_RESP = 87,

	RESP_CG_SUBHEADER_FETCH_RESP = 0,
	RESP_CG_SUBHEADER_FETCH_DROP,
	RESP_CG_SUBHEADER_TELEPORT,

	RESP_GC_SUBHEADER_DATA_RESP = 0,
	RESP_GC_SUBHEADER_DATA_DROP,
	RESP_GC_SUBHEADER_DATA_MOB,
	RESP_GC_SUBHEADER_REFRESH_RESP,
};

typedef struct SPacketGCRespHeader
{
	uint8_t	header;
	uint16_t	size;
	uint8_t	subheader;
} TPacketGCRespHeader;

typedef struct SPacketCGRespHeader
{
	uint8_t	header;
	uint8_t	subheader;
} TPacketCGRespHeader;

typedef struct SPacketGCRespData
{
	size_t id;
	uint32_t x;
	uint32_t y;
	uint32_t time;
	time_t nextRespTime;
} TPacketGCRespData;

typedef struct SPacketGCRespGold
{
	uint32_t	minGold;
	uint32_t	maxGold;
} TPacketGCRespGold;

typedef struct SPacketGCRespItem
{
	uint32_t	vnum;
	uint8_t	minCount;
	uint8_t	maxCount;
} TPacketGCRespItem;

typedef struct SPacketGCMapData
{
	uint8_t	mobCount;
	uint32_t	currentMetinCount;
	uint32_t	maxMetinCount;
	uint32_t	currentBossCount;
	uint32_t	maxBossCount;
} TPacketGCMapData;

typedef struct SPacketGCRespRefresh
{
	size_t	id;
	uint32_t	vnum;
	time_t	nextRespTime;
	uint32_t	x;
	uint32_t	y;
} TPacketGCRespRefresh;
#endif

#ifdef ENABLE_NEW_PET_SYSTEM
typedef struct SPetAction
{
	BYTE	header;
	DWORD	skillvnum;
} TPacketPetAction;
#endif


#ifdef ENABLE_ITEMSHOP
typedef struct SItemShopItem
{
	DWORD	vnum;
	int		count;
	DWORD	price;
	BYTE	bonus_type;
	BYTE	category;
	DWORD	id;
	long	alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute	aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
	BYTE	is_vip;
	BYTE	fixed_count;
	DWORD	additional_time;
	DWORD	add_time;
	BYTE	discount;
	DWORD	discount_time;
	DWORD	limited_time;
	DWORD	limited_count;
	DWORD	limited_buy_count;
	DWORD	limited_buy_time;
} TItemShopItem;

enum
{
	ITEMSHOP_SUBHEADER_GC_ITEM,
	ITEMSHOP_SUBHEADER_GC_COINS,
	ITEMSHOP_SUBHEADER_GC_CLEAR,
	ITEMSHOP_SUBHEADER_GC_OPEN,
	ITEMSHOP_SUBHEADER_GC_EDITOR,
	ITEMSHOP_SUBHEADER_GC_UPDATE,
	ITEMSHOP_SUBHEADER_GC_DELETE,
	ITEMSHOP_SUBHEADER_GC_ERROR,
	ITEMSHOP_SUBHEADER_GC_BUY_STATS,
	ITEMSHOP_SUBHEADER_GC_LIMITED_COUNT,
	ITEMSHOP_SUBHEADER_GC_REWARD,
	ITEMSHOP_SUBHEADER_GC_REWARD_STATUS,
	ITEMSHOP_SUBHEADER_GC_POPUP,
	ITEMSHOP_SUBHEADER_GC_CATEGORY,

	ITEMSHOP_SUBHEADER_CG_OPEN = 0,
	ITEMSHOP_SUBHEADER_CG_BUY,
	ITEMSHOP_SUBHEADER_CG_ADD,

	HEADER_CG_ITEMSHOP = 34,
	HEADER_GC_ITEMSHOP = 139,

	CATEGORY_NAME_MAX_NUM = 32,
};

enum EItemShopError : BYTE
{
	ITEMSHOP_ERROR_NOT_ENOUGH_COINS,
	ITEMSHOP_ERROR_INVENTORY_FULL,
	ITEMSHOP_ERROR_WARP,
	ITEMSHOP_ERROR_NON_EDITOR,
	ITEMSHOP_ERROR_COUNT,
	ITEMSHOP_ERROR_UNKNOWN_ERROR,
};

enum EItemShopPopupType
{
	ITEMSHOP_POPUP_ITEM_PROMO,
	ITEMSHOP_POPUP_ITEM_LIMITED,
	ITEMSHOP_POPUP_NEW_ITEM,
};

typedef struct item_shop_packet
{
	BYTE	header;
	DWORD	size;
	BYTE	subheader;
} TItemShopGCPacket;

typedef struct item_shop_client_packet
{
	BYTE	header;
	BYTE	subheader;
} TItemShopCGPacket;

typedef struct item_shop_delete_item
{
	DWORD	id;
	DWORD	category;
} TPacketGCItemShopDeleteItem;

typedef struct item_shop_buy
{
	DWORD	id;
	DWORD	category;
	int		count;
	BYTE	bonus_type;
} TPacketCGItemShopBuy;

typedef struct item_shop_add
{
	TItemShopItem	aItemShopItem;
	BYTE	bFlag;
} TPacketCGItemShopAddItem;

typedef struct item_shop_buy_stats
{
	DWORD	dwID;
	DWORD	dwCategory;
	int		dwCount;
} TItemShopBuyStats;

typedef struct itemshop_open_packet
{
	time_t	ttDataTime;
} TItemShopOpenPacket;

typedef struct item_shop_popup_packet
{
	BYTE	type;
	DWORD	id;
	DWORD	category;
} TItemShopPopupPacket;

typedef struct item_shop_category_packet
{
	BYTE index;
	char szName[CATEGORY_NAME_MAX_NUM + 1];
} TItemShopCategoryPacket;
#endif

#ifdef ENABLE_TOMBOLA
enum
{
	HEADER_CG_TOMBOLA = 37,
	TOMBOLA_CG_SUBHEADER_START = 0,
	TOMBOLA_CG_SUBHEADER_OPEN = 1,

	HEADER_GC_TOMBOLA = 50,
	TOMBOLA_GC_SUBHEADER_ITEM = 0,
	TOMBOLA_GC_SUBHEADER_START = 1,
	TOMBOLA_GC_SUBHEADER_OPEN = 2,
};

typedef struct tombola_packet
{
	BYTE	byHeader;
	WORD	wSize;
	BYTE	bySubHeader;
} TPacketGCTombola;

typedef struct tombola_client_packet
{
	BYTE	byHeader;
	BYTE	bySubHeader;
} TPacketCGTombola;

typedef struct tombola_item
{
	DWORD	dwVnum;
	DWORD	dwCount;
	DWORD	dwChance;

	tombola_item()
	{
		dwVnum = 0;
		dwCount = 0;
		dwChance = 0;
	}
} TTombolaItem;
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
enum EAcceInfo
{
	ACCE_ABSORPTION_SOCKET = 0,
	ACCE_ABSORBED_SOCKET = 1,
	ACCE_CLEAN_ATTR_VALUE0 = 7,
	ACCE_WINDOW_MAX_MATERIALS = 2,
	ACCE_REVERSAL_VNUM_1 = 39046,
	ACCE_REVERSAL_VNUM_2 = 90000,
};

enum
{
	HEADER_CG_ACCE = 211,
	HEADER_GC_ACCE = 215,
	ACCE_SUBHEADER_GC_OPEN = 0,
	ACCE_SUBHEADER_GC_CLOSE,
	ACCE_SUBHEADER_GC_ADDED,
	ACCE_SUBHEADER_GC_REMOVED, ENABLE_ACCE_COSTUME_SYSTEM
	ACCE_SUBHEADER_CG_REFINED,
	ACCE_SUBHEADER_CG_CLOSE = 0,
	ACCE_SUBHEADER_CG_ADD,
	ACCE_SUBHEADER_CG_REMOVE,
	ACCE_SUBHEADER_CG_REFINE,
};

struct AcceMaterials
{
	DWORD vnum;
	DWORD count;
};

typedef struct SPacketAcce
{
	BYTE	header;
	BYTE	subheader;
	bool	bWindow;
	long long	dwPrice;
	long long	dwCheque;
	BYTE	bChance;
	AcceMaterials	materials[2];
	BYTE	bPos;
	TItemPos	tPos;
	DWORD	dwItemVnum;
	DWORD	dwMinAbs;
	DWORD	dwMaxAbs;
} TPacketAcce;

typedef struct SAcceMaterial
{
	BYTE	bHere;
	WORD	wCell;
} TAcceMaterial;

typedef struct SAcceResult
{
	DWORD	dwItemVnum;
	DWORD	dwMinAbs;
	DWORD	dwMaxAbs;
} TAcceResult;
#endif

#ifdef ENABLE_DROP_INFO
enum EDropInfoConfig
{
	HEADER_GC_DROP_INFO = 93,
	HEADER_CG_DROP_INFO = 99,
};

typedef struct SPacketDropInfoItem
{
	DWORD	dwVnum;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	uint16_t	byMinCount;
	uint16_t	byMaxCount;
#else
	BYTE	byMinCount;
	BYTE	byMaxCount;
#endif
#ifdef ENABLE_DROP_INFO_PCT
	float	pct;
#endif
} TPacketDropInfoItem;

typedef struct SPacketDropInfo
{
	BYTE	header;
	WORD	size;
	int		item_count;
	DWORD	gold_min;
	DWORD	gold_max;
	DWORD	mob_vnum;
} TPacketDropInfo;
#endif
#ifdef ENABLE_LRN_MAINTENANCE_SYSTEM
typedef struct SPacketGCSendMaintenanceData
{
	BYTE	header;
	BYTE	status;
	char	message[MAINTENANCE_MESSAGE_MAX_LEN];
	int		time;
} TPacketGCSendMaintenanceData;
#endif

#ifdef ENABLE_LRN_LOCATION_SYSTEM
typedef struct SPacketGCSendLocationData
{
	BYTE	header;
	BYTE	position[MAX_LOCATION_POSITION];
	int		map_index[MAX_LOCATION_POSITION];
	char	name[MAX_LOCATION_CHAR_STRING];
	int		posx[MAX_LOCATION_POSITION];
	int		posy[MAX_LOCATION_POSITION];
} TPacketGCSendLocationData;

typedef struct SPacketCGSendLocationManager
{
	BYTE	header;
	BYTE	action;	
	BYTE	position;
	char	name[MAX_LOCATION_CHAR_STRING];
	int		posx[MAX_LOCATION_POSITION];
	int		posy[MAX_LOCATION_POSITION];
} TPacketCGSendLocationManager;
#endif

#ifdef WEEKLY_RANK_BYLUZER
typedef struct packet_send_weekly_rank
{
	int		rank;
	char	name[CHARACTER_NAME_MAX_LEN+1];
	int		points;
	int		empire;
	int		job;
} TPacketGCTableWeeklyRank;

typedef struct packet_send_weekly_rank2
{
	BYTE 			bHeader;
	TPacketGCTableWeeklyRank rankingData[10];
} TPacketGCSendWeeklyRank;

typedef struct packet_send_weekly_page
{
	BYTE bHeader;
	BYTE page;
	int season;
	bool can_select;
} TPacketGCSelectWeeklyPage;

typedef struct packet_select_weekly_rank
{
	BYTE			bHeader;
	BYTE			page;
} TPacketCGSelectWeeklyRank;
#endif

#ifdef TITLE_SYSTEM_BYLUZER
#define TITLE_MAX_NUM 21
typedef struct SPlayerTitle
{
	BYTE	titletype;
	BYTE	active;
} TPlayerTitle;
typedef struct packet_title_active
{
	BYTE		bHeader;
	TPlayerTitle	titles[TITLE_MAX_NUM];
	BYTE		actually_active;
} TPacketGCTitleActive;

typedef struct packet_title_active_now
{
	BYTE		bHeader;
	BYTE		id;
} TPacketGCTitleActiveNow;

typedef struct packet_title_active_client
{
	BYTE		bHeader;
	int			id;
} TPacketCGTitleActive;
#endif

#ifdef ENABLE_SWITCHBOT
enum ECGSwitchbotSubheader
{
	SUBHEADER_CG_SWITCHBOT_START,
	SUBHEADER_CG_SWITCHBOT_STOP,
};

struct TPacketCGSwitchbot
{
	BYTE header;
	int size;
	BYTE subheader;
	BYTE slot;
};

enum EGCSwitchbotSubheader
{
	SUBHEADER_GC_SWITCHBOT_UPDATE,
	SUBHEADER_GC_SWITCHBOT_UPDATE_ITEM,
	SUBHEADER_GC_SWITCHBOT_SEND_ATTRIBUTE_INFORMATION,
};

struct TPacketGCSwitchbot
{
	BYTE header;
	int size;
	BYTE subheader;
	BYTE slot;
};

struct TSwitchbotUpdateItem
{
	BYTE	slot;
	BYTE	vnum;
	BYTE	count;
	long	alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
};
#endif

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
typedef struct SPacketCGMountNPCAction
{
	BYTE	bHeader;
	BYTE	bAction;
	DWORD	dValue0;
} TPacketCGMountNPCAction;

typedef struct SPacketGCMountNPCAction
{
	BYTE	bHeader;
	BYTE	bAction;
	DWORD	dValue0;
} TPacketGCMountNPCAction;
#endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
typedef struct SPacketCGBuffNPCCreate
{
	BYTE bHeader;
	BYTE	bAction;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE	bSex;
}TPacketCGBuffNPCCreate;

typedef struct SPacketCGBuffNPCAction
{
	BYTE	bHeader;
	BYTE	bAction;
	DWORD	dValue0;
	DWORD	dValue1;
} TPacketCGBuffNPCAction;

typedef struct SPacketGCBuffNPCAction
{
	BYTE	bHeader;
	BYTE	bAction;
	DWORD	dValue0;
	DWORD	dValue1;
} TPacketGCBuffNPCAction;

typedef struct SPacketGCBuffNPCUseSkill
{
	BYTE	bHeader;
	DWORD	dSkillVnum;
	DWORD	dVid;
	DWORD	dSkillLevel;
}TPacketGCBuffNPCUseSkill;
#endif

#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
enum
{
	CUBE_RENEWAL_SUB_HEADER_OPEN_RECEIVE,
	CUBE_RENEWAL_SUB_HEADER_CLEAR_DATES_RECEIVE,
	CUBE_RENEWAL_SUB_HEADER_DATES_RECEIVE,
	CUBE_RENEWAL_SUB_HEADER_DATES_LOADING,

	CUBE_RENEWAL_SUB_HEADER_MAKE_ITEM,
	CUBE_RENEWAL_SUB_HEADER_CLOSE,
};


typedef struct  packet_send_cube_renewal
{
	BYTE header;
	BYTE subheader;
	DWORD index_item;
	DWORD count_item;
	DWORD index_item_improve;
}TPacketCGCubeRenewalSend;



typedef struct dates_cube_renewal
{
	DWORD npc_vnum;
	DWORD index;

	DWORD	vnum_reward;
	int		count_reward;

	bool 	item_reward_stackable;

	DWORD	vnum_material_1;
	int		count_material_1;

	DWORD	vnum_material_2;
	int		count_material_2;

	DWORD	vnum_material_3;
	int		count_material_3;

	DWORD	vnum_material_4;
	int		count_material_4;

	DWORD	vnum_material_5;
	int		count_material_5;

	DWORD	vnum_material_6;
	int		count_material_6;

	DWORD	vnum_material_7;
	int		count_material_7;

	int 	gold;
	int		punktyos;
	int		cheque;
	int 	percent;
	
	char 	category[100];
}TInfoDateCubeRenewal;

typedef struct packet_receive_cube_renewal
{
	packet_receive_cube_renewal(): header(HEADER_GC_CUBE_RENEWAL)
	{}

	BYTE header;
	BYTE subheader;
	TInfoDateCubeRenewal	date_cube_renewal;
}TPacketGCCubeRenewalReceive;
#endif

#ifdef ENABLE_CHEQUE_SYSTEM
enum EWonExchangeCGSubHeader
{
	WON_EXCHANGE_CG_SUBHEADER_SELL,
	WON_EXCHANGE_CG_SUBHEADER_BUY
};

typedef struct SPacketCGWonExchange
{
	SPacketCGWonExchange() :
		bHeader(HEADER_CG_WON_EXCHANGE)
	{}
	BYTE bHeader;
	BYTE bSubHeader;
	WORD wValue;
} TPacketCGWonExchange;
#endif

#ifdef ENABLE_MINIMAP_DUNGEONINFO
enum
{
	MINIMAP_DUNGEONINFO_SUBHEADER_STATUS,
	MINIMAP_DUNGEONINFO_SUBHEADER_STAGE,
	MINIMAP_DUNGEONINFO_SUBHEADER_GAUGE,
	MINIMAP_DUNGEONINFO_SUBHEADER_NOTICE,
	MINIMAP_DUNGEONINFO_SUBHEADER_BUTTON,
	MINIMAP_DUNGEONINFO_SUBHEADER_TIMER,
};

typedef struct SPacketGCMiniMapDungeonInfo
{
	BYTE	bHeader;
	BYTE	bSubHeader;
	DWORD	dValue1;
	DWORD	dValue2;
	DWORD	dValue3;
	DWORD	buttonvalue;
	DWORD	timerstatus;
	DWORD	time;
	char	szNotice[256];
}TPacketGCMiniMapDungeonInfo;
#endif

#ifdef ENABLE_ENTITY_PRELOADING
/*** HEADER_GC_PRELOAD_ENTITIES ***/
typedef struct packet_preload_entities
{
	uint8_t header;
	uint16_t size;
	uint16_t count;
} TPacketGCPreloadEntities;
#endif

#ifdef ENABLE_AURA_SYSTEM
enum EAuraInfo
{
	AURA_ABSORPTION_SOCKET = 0,
	AURA_ABSORBED_SOCKET = 1,
	AURA_LEVEL_SOCKET = 2,
	AURA_WINDOW_MAX_MATERIALS = 2,
	AURA_ICE_RUNIC = 49990,
};

enum
{
	AURA_SUBHEADER_GC_OPEN = 0,
	AURA_SUBHEADER_GC_CLOSE,
	AURA_SUBHEADER_GC_ADDED,
	AURA_SUBHEADER_GC_REMOVED,
	AURA_SUBHEADER_GC_REFINED,
	AURA_SUBHEADER_CG_CLOSE = 0,
	AURA_SUBHEADER_CG_ADD,
	AURA_SUBHEADER_CG_REMOVE,
	AURA_SUBHEADER_CG_REFINE,
};

typedef struct SPacketAura
{
	BYTE	header;
	BYTE	subheader;
	bool	bWindow;
	DWORD	dwPrice;
	BYTE	bPos;
	TItemPos	tPos;
	DWORD	dwItemVnum;
	DWORD	dwMinAbs;
	DWORD	dwMaxAbs;
} TPacketAura;

typedef struct SAuraMaterial
{
	BYTE	bHere;
	WORD	wCell;
} TAuraMaterial;

typedef struct SAuraResult
{
	DWORD	dwItemVnum;
	DWORD	dwMinAbs;
	DWORD	dwMaxAbs;
} TAuraResult;
#endif

#ifdef ENABLE_COLLECT_WINDOW
typedef struct SPacketGCCollectWindow
{
	BYTE	bHeader;
	BYTE	bWindowType;
	DWORD	dwTime;
	DWORD	bCount;
	DWORD	dwItemVnum;
	DWORD	bCountTotal;
	BYTE	bChance;
	DWORD	bRenderTargetID;
	DWORD	bQuestIndex;
	DWORD	RequiredLevel;
} TPacketGCCollectWindow;
#endif

#ifdef ENABLE_EVENT_MANAGER
typedef struct SPacketGCEventManager
{
	BYTE	header;
	DWORD	size;
} TPacketGCEventManager;
typedef struct event_struct_
{
	WORD	eventID;
	BYTE	eventIndex;
	int		startTime;
	int		endTime;
	BYTE	empireFlag;
	BYTE	channelFlag;
	DWORD	value[4];
	bool	eventStatus;
	bool	eventTypeOnlyStart;
	char	startTimeText[25];
	char	endTimeText[25];
}TEventManagerData;
enum
{
	EVENT_MANAGER_LOAD,
	EVENT_MANAGER_EVENT_STATUS,
	EVENT_MANAGER_REMOVE_EVENT,
	EVENT_MANAGER_UPDATE,
	EVENT_MANAGER_PLAYER_ACTIVATED,

	BONUS_EVENT = 1,
	DOUBLE_BOSS_LOOT_EVENT = 2,
	DOUBLE_METIN_LOOT_EVENT = 3,
	DOUBLE_MISSION_BOOK_EVENT = 4,
	DUNGEON_COOLDOWN_EVENT = 5,
	DUNGEON_CHEST_LOOT_EVENT = 6,
	EMPIRE_WAR_EVENT = 7,
	MOONLIGHT_EVENT = 8,
	TOURNAMENT_EVENT = 9,
	WHELL_OF_FORTUNE_EVENT = 10,
	HALLOWEEN_EVENT = 11,
	NPC_SEARCH_EVENT = 12,
	EXP_EVENT = 13,
	ITEM_DROP_EVENT = 14,
	YANG_DROP_EVENT = 15,
	SWITCH_RARE_EVENT = 16,
	VALENTINE_EVENT = 17,
	SWITCH_AVERAGE_EVENT = 18,
	SWITCH_RUNE_EVENT = 19,
	DOUBLE_ACHIEVEMENT_POINTS = 20,
	MINING_CHEST_EVENT = 21,
	DOUBLE_ORE_EVENT = 22,
	BIGGER_STAR_CHANCE = 23,
};

typedef struct command_event_activate
{
	BYTE	header;
	WORD	eventID;
} TPacketCGEventActivate;

enum EEventActivateResult
{
	EVENT_ACTIVATE_SUCCESS = 0,
	EVENT_ACTIVATE_ALREADY  = 1,
	EVENT_ACTIVATE_FAIL     = 2,
};

typedef struct SPacketGCEventActivateResult
{
	BYTE	header;
	WORD	eventID;
	BYTE	result;
} TPacketGCEventActivateResult;
#endif
#ifdef TAKE_LEGEND_DAMAGE_BOARD_SYSTEM
typedef struct packet_send_legend_damage
{
	char		cName[CHARACTER_NAME_MAX_LEN + 1];
	int			iLevel;
	int			iRace;
	int			iEmpire;
	long long	iDamage;
} TPacketGCTableLegendDamage;

typedef struct packet_send_legend_damage2
{
	BYTE	bHeader;
	DWORD	dwBossVID;
	char	cTarget[CHARACTER_NAME_MAX_LEN + 1];
	TPacketGCTableLegendDamage legendDamageData[LEGEND_DAMAGE_BOARD_MAX_ENTRIES];
} TPacketGCSendLegendDamage;
#endif

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
enum EPrivateShopGCSubheader
{
	SUBHEADER_GC_PRIVATE_SHOP_ADD_ENTITY,
	SUBHEADER_GC_PRIVATE_SHOP_DEL_ENTITY,
	SUBHEADER_GC_PRIVATE_SHOP_TITLE,
	SUBHEADER_GC_PRIVATE_SHOP_LOAD,
	SUBHEADER_GC_PRIVATE_SHOP_SET_ITEM,
	SUBHEADER_GC_PRIVATE_SHOP_SET_SALE,
	SUBHEADER_GC_PRIVATE_SHOP_BALANCE_UPDATE,
	SUBHEADER_GC_PRIVATE_SHOP_OPEN_PANEL,
	SUBHEADER_GC_PRIVATE_SHOP_CLOSE_PANEL,
	SUBHEADER_GC_PRIVATE_SHOP_CLOSE,
	SUBHEADER_GC_PRIVATE_SHOP_START,
	SUBHEADER_GC_PRIVATE_SHOP_END,
	SUBHEADER_GC_PRIVATE_SHOP_REMOVE_ITEM,
	SUBHEADER_GC_PRIVATE_SHOP_REMOVE_MY_ITEM,
	SUBHEADER_GC_PRIVATE_SHOP_ADD_ITEM,
	SUBHEADER_GC_PRIVATE_SHOP_STATE_UPDATE,
	SUBHEADER_GC_PRIVATE_SHOP_WITHDRAW,
	SUBHEADER_GC_PRIVATE_SHOP_ITEM_PRICE_CHANGE,
	SUBHEADER_GC_PRIVATE_SHOP_ITEM_MOVE,
	SUBHEADER_GC_PRIVATE_SHOP_TITLE_CHANGE,
	SUBHEADER_GC_PRIVATE_SHOP_UNLOCKED_SLOTS_CHANGE,

	SUBHEADER_GC_PRIVATE_SHOP_SEARCH_OPEN_LOOK_MODE,
	SUBHEADER_GC_PRIVATE_SHOP_SEARCH_OPEN_TRADE_MODE,
	SUBHEADER_GC_PRIVATE_SHOP_SEARCH_RESULT,
	SUBHEADER_GC_PRIVATE_SHOP_SEARCH_UPDATE,

	SUBHEADER_GC_PRIVATE_SHOP_MARKET_ITEM_PRICE_DATA_RESULT,
	SUBHEADER_GC_PRIVATE_SHOP_MARKET_ITEM_PRICE_RESULT,
};

enum EPrivateShopCGSubheader
{
	SUBHEADER_CG_PRIVATE_SHOP_BUILD,
	SUBHEADER_CG_PRIVATE_SHOP_CLOSE,
	SUBHEADER_CG_PRIVATE_SHOP_PANEL_OPEN,
	SUBHEADER_CG_PRIVATE_SHOP_PANEL_OPEN_CHECK,
	SUBHEADER_CG_PRIVATE_SHOP_PANEL_CLOSE,
	SUBHEADER_CG_PRIVATE_SHOP_START,
	SUBHEADER_CG_PRIVATE_SHOP_END,
	SUBHEADER_CG_PRIVATE_SHOP_BUY,
	SUBHEADER_CG_PRIVATE_SHOP_WITHDRAW,
	SUBHEADER_CG_PRIVATE_SHOP_MODIFY,
	SUBHEADER_CG_PRIVATE_SHOP_STATE_UPDATE,
	SUBHEADER_CG_PRIVATE_SHOP_ITEM_PRICE_CHANGE,
	SUBHEADER_CG_PRIVATE_SHOP_ITEM_MOVE,
	SUBHEADER_CG_PRIVATE_SHOP_ITEM_CHECKIN,
	SUBHEADER_CG_PRIVATE_SHOP_ITEM_CHECKOUT,
	SUBHEADER_CG_PRIVATE_SHOP_TITLE_CHANGE,
	SUBHEADER_CG_PRIVATE_SHOP_WARP_REQUEST,
	SUBHEADER_CG_PRIVATE_SHOP_SLOT_UNLOCK_REQUEST,

	SUBHEADER_CG_PRIVATE_SHOP_SEARCH_CLOSE,
	SUBHEADER_CG_PRIVATE_SHOP_SEARCH,
	SUBHEADER_CG_PRIVATE_SHOP_SEARCH_BUY,

	SUBHEADER_CG_PRIVATE_SHOP_MARKET_ITEM_PRICE_DATA_REQUEST,
	SUBHEADER_CG_PRIVATE_SHOP_MARKET_ITEM_PRICE_REQUEST,
};

typedef struct SSalePrice
{
	long long	llGold;
	DWORD		dwCheque;
} TItemPrice;

typedef struct SSaleItemPrice
{
	DWORD		dwVnum;
	TItemPrice	TPrice;
} TMarketItemPrice;

typedef struct SPrivateShopItem
{
	TItemPos	TPos;
	long long	llGold;
	DWORD		dwCheque;
	WORD		wDisplayPos;
} TPrivateShopItem;

typedef struct SPrivateShopItemData
{
	DWORD					dwVnum;
	TItemPrice				TPrice;
	time_t					tCheckin;
	DWORD					dwCount;
	WORD					wPos;
	long					alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute	aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#ifdef ENABLE_PRIVATE_SHOP_CHANGE_LOOK
	DWORD dwTransmutationVnum;
#endif
#ifdef ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
	DWORD dwRefineElement;
#endif
#ifdef ENABLE_PRIVATE_SHOP_APPLY_RANDOM
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_RANDOM_SLOT_MAX_NUM];
#endif
} TPrivateShopItemData;

typedef struct SPrivateShopSaleData
{
	char					szCustomer[CHARACTER_NAME_MAX_LEN + 1];
	time_t					tTime;
	TPrivateShopItemData	TItem;
} TPrivateShopSaleData;

typedef struct SPrivateShopSearchData
{
	DWORD					dwShopID;
	char					szOwnerName[CHARACTER_NAME_MAX_LEN + 1];
	DWORD					dwVnum;
	TItemPrice				TPrice;
	DWORD					dwCount;
	WORD					wPos;
	long					alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute	aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
	time_t					tCheckin;
	BYTE					bState;
	long					lMapIndex;
#ifdef ENABLE_PRIVATE_SHOP_CHANGE_LOOK
	DWORD dwTransmutationVnum;
#endif
#ifdef ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
	DWORD dwRefineElement;
#endif
#ifdef ENABLE_PRIVATE_SHOP_APPLY_RANDOM
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_RANDOM_SLOT_MAX_NUM];
#endif
} TPrivateShopSearchData;

typedef struct SPacketCGPrivateShop
{
	BYTE	bHeader;
	BYTE	bSubHeader;
} TPacketCGPrivateShop;

typedef struct SPacketGCPrivateShopAddEntity
{
	long		lX;
	long		lY;
	long		lZ;
	DWORD		dwVID;
	DWORD		dwVnum;
	char		szName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE		bTitleType;
	char		szTitle[TITLE_MAX_LEN + 1];
} TPacketGCPrivateShopAddEntity;

typedef struct SPacketGCPrivateShopDelEntity
{
	DWORD		dwVID;
} TPacketGCPrivateShopDelEntity;

typedef struct SPacketGCPrivateShopTitle
{
	DWORD		dwVID;
	char		szTitle[TITLE_MAX_LEN + 1];
	BYTE		bTitleType;
} TPacketGCPrivateShopTitle;

typedef struct SPacketCGPrivateShopBuild
{
	char	szTitle[TITLE_MAX_LEN + 1];
	DWORD	dwPolyVnum;
	BYTE	bTitleType;
	BYTE	bPageCount;
	WORD	wItemCount;
} TPacketCGPrivateShopBuild;

typedef struct SPacketCGPrivateShopItemPriceChange
{
	WORD		wPos;
	TItemPrice	TPrice;
} TPacketCGPrivateShopItemPriceChange;

typedef struct SPacketCGPrivateShopItemMove
{
	WORD		wPos;
	WORD		wChangePos;
} TPacketCGPrivateShopItemMove;

typedef struct SPacketCGPrivateShopItemCheckin
{
	TItemPos	TSrcPos;
	TItemPrice	TPrice;
	int			iDstPos;
} TPacketCGPrivateShopItemCheckin;

typedef struct SPacketCGPrivateShopItemCheckout
{
	WORD		wSrcPos;
	int			iDstPos;
} TPacketCGPrivateShopItemCheckout;

typedef struct SPrivateShopSearchFilter
{
	DWORD	dwVnum;
	char	szOwnerName[CHARACTER_NAME_MAX_LEN + 1];

	int		iItemType;
	int		iItemSubType;

	int		iJob;
	int		iGender;

	WORD	wMinStack;
	WORD	wMaxStack;

	BYTE	bMinRefine;
	BYTE	bMaxRefine;

	DWORD	dwMinLevel;
	DWORD	dwMaxLevel;

	TPlayerItemAttribute	aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
	WORD					wSashAbsorption;
	BYTE					bAlchemyLevel;
	BYTE					bAlchemyClarity;
	int						iItemValue0;
	bool					bSearchMode;
} TPrivateShopSearchFilter;

typedef struct SPacketCGPrivateShopSearch
{
	TPrivateShopSearchFilter	Filter;
	bool						bUseFilter;
} TPacketCGPrivateShopSearch;

typedef struct SPrivateShopSearchSelectedItem
{
	DWORD		dwShopID;
	WORD		wPos;
	TItemPrice	TPrice;
} SPrivateShopSearchSelectedItem;

typedef struct SPacketCGPrivateShopSearchBuy
{
	SPrivateShopSearchSelectedItem aSelectedItems[10]; // SELECTED_ITEM_MAX_NUM;
} TPacketCGPrivateShopSearchBuy;

typedef struct SPacketGCPrivateShop
{
	BYTE	bHeader;
	WORD	wSize;
	BYTE	bSubHeader;
} TPacketGCPrivateShop;

typedef struct SPacketGCPrivateShopLoad
{
	char		szTitle[SHOP_SIGN_MAX_LEN + 1];
	long long	llGold;
	DWORD		dwCheque;
	long		lX;
	long		lY;
	BYTE		bChannel;
	BYTE		bState;
	BYTE		bPageCount;
} TPacketGCPrivateShopLoad;

typedef struct SPacketGCPrivateShopOpen
{
	char					szTitle[SHOP_SIGN_MAX_LEN + 1];
	BYTE					bState;
	BYTE					bPageCount;
	WORD					wUnlockedSlots;
	DWORD					dwVID;
	TPrivateShopItemData	aItems[PRIVATE_SHOP_HOST_ITEM_MAX_NUM];
} TPacketGCPrivateShopOpen;

typedef struct SPacketGCPrivateStateUpdate
{
	BYTE	bState;
	bool	bIsMainPlayerPrivateShop;
} TPacketGCPrivateStateUpdate;

typedef struct SPacketGCPrivateShopBalanceUpdate
{
	TItemPrice	TPrice;
} TPacketGCPrivateShopBalanceUpdate;

typedef struct SPacketGCPrivateShopItemPriceChange
{
	WORD		wPos;
	TItemPrice	TPrice;
} TPacketGCPrivateShopItemPriceChange;

typedef struct SPacketGCPrivateShopItemMove
{
	WORD		wPos;
	WORD		wChangePos;
} TPacketGCPrivateShopItemMove;

typedef struct SPacketGCPrivateShopSearchUpdate
{
	DWORD	dwShopID;
	int		iSpecificItemPos;
	BYTE	bState;
} TPacketGCPrivateShopSearchUpdate;
#endif

#ifdef ENABLE_BATTLE_PASS_ROBERT
enum 
{
	BATTLEPASS_GC_SUB_INIT,
	BATTLEPASS_GC_SUB_UPDATE 
};
typedef struct SPacketBattlePass
{
	BYTE	bHeader;
	WORD	wSize;
	BYTE	bSubHeader;

	bool isPremium;
	int points;
} TPacketBattlePass;

typedef struct SPacketSubBattlePassUpdate
{
	uint8_t type;
	uint8_t missionVnum;

	uint32_t progress;
	uint32_t endTime;
} TPacketSubBattlePassUpdate;
#endif

#ifdef __ENABLE_POLYMORPH_SYSTEM__
enum EPolySystem : BYTE
{
	SUBHEADER_POLY_OPEN = 0,
	SUBHEADER_POLY_CLOSE = 1,
	SUBHEADER_POLY_BUY = 2,
	SUBHEADER_POLY_SKIN_UNLOCK = 3,
	SUBHEADER_POLY_SKIN_CHANGE = 4,
	SUBHEADER_POLY_DATA = 5,

	MAX_POLY_REQUIRED_ITEMS = 7,
	MAX_POLY_BONUS = 2,
};

typedef struct SPacketGCPolySystem
{
	BYTE byHeader;
	WORD wSize;
	WORD bySubheader;
} TPacketGCPolySystem;

typedef struct SPacketCGPolySystem
{
	BYTE byHeader;
	BYTE bySubheader;
} TPacketCGPolySystem;

typedef struct SPolymorphSkin
{
	uint8_t byIndex;
	uint32_t dwSkinVnum;
	uint32_t dwSkinUnlock;
} TPolymorphSkin;

typedef struct SPolymorphRequired
{
	uint32_t dwVnum;
	uint16_t wCount;
} TPolyRequired;

typedef struct SPolymorphStage
{
	uint8_t byStage;
	uint32_t dwVnum;
	uint32_t dwMonster;
	CItemData::TItemApply tApplies[MAX_POLY_BONUS];
	TPolyRequired tRequired[MAX_POLY_REQUIRED_ITEMS];
} TPolymorphStage;

#endif

#ifdef __INVENTORY_BUFFERING__
typedef struct SPacketGCInventoryHeader
{
	BYTE bHeader;
	WORD wSize;
} TPacketGCInventoryHeader;
#endif

#ifdef ENABLE_SAVE_LOCATION_SYSTEM
enum ESaveLocationInfo
{
	SAVE_LOCATION_MAX_NUM = 20,
	SAVE_LOCATION_MAX_NUM_NORMAL = 10,
};

enum
{
	SAVELOCATION_SUBHEADER_CG_ADD = 0,
	SAVELOCATION_SUBHEADER_CG_DEL,
	SAVELOCATION_SUBHEADER_CG_WARP,
	SAVELOCATION_SUBHEADER_CG_RENAME,

	SAVELOCATION_SUBHEADER_GC_ADDED = 0,
	SAVELOCATION_SUBHEADER_GC_DELED,
	SAVELOCATION_SUBHEADER_GC_UPDATE,
};

typedef struct SPacketSaveLocation
{
	BYTE	header;
	BYTE	subheader;
	BYTE	pos;
	TSaveLocation	location[SAVE_LOCATION_MAX_NUM];
} TPacketSaveLocation;
#endif


#ifdef ENABLE_OFFLINE_SHOP
typedef struct SPacketGCNewOfflineshop
{
	BYTE bHeader;
#ifdef ENABLE_LARGE_DYNAMIC_PACKET
	int wSize;
#else
	WORD wSize;
#endif
	BYTE bSubHeader;
} TPacketGCNewOfflineshop;

typedef struct SPacketCGNewOfflineShop
{
	BYTE bHeader;
	WORD wSize;
	BYTE bSubHeader;
} TPacketCGNewOfflineShop;

namespace offlineshop
{
	enum class ExpirationType
	{
		EXPIRE_NONE,
		EXPIRE_REAL_TIME,
		EXPIRE_REAL_TIME_FIRST_USE,
	};

	typedef struct SPriceInfo
	{
		long long illYang;
		SPriceInfo() : illYang(0)
		{}
	} TPriceInfo;

#ifdef ENABLE_OFFLINE_SHOP_GRID
	typedef struct SItemDisplayPos
	{
		BYTE pos;
		BYTE page;
	}TItemDisplayPos;
#endif

	typedef struct SItemInfoEx
	{
		DWORD dwVnum;
		DWORD dwCount;
		long alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
		TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#ifdef ENABLE_CHANGELOOK_SYSTEM
		DWORD dwTransmutation;
#endif
		ExpirationType expiration;
#ifdef ENABLE_OFFLINE_SHOP_GRID
		TItemDisplayPos display_pos;
#endif
	} TItemInfoEx;

	typedef struct SItemInfo
	{
		DWORD dwOwnerID, dwItemID;
		TPriceInfo price;
		TItemInfoEx item;
		char dwOwnerName[CHARACTER_NAME_MAX_LEN + 1];
	} TItemInfo;

	typedef struct SShopInfo
	{
		DWORD dwOwnerID;
		DWORD dwDuration;
		char szName[OFFLINE_SHOP_NAME_MAX_LEN];
#ifdef ENABLE_SHOP_DECORATION
		DWORD	dwShopDecoration;
#endif
		DWORD dwCount;
	} TShopInfo;

	typedef struct SFilterInfo
	{
		BYTE bType;
		BYTE bSubType;
		char szName[OFFLINE_SHOP_ITEM_MAX_LEN];
		TPriceInfo priceStart, priceEnd;
		BYTE iLevelStart, iLevelEnd;
		DWORD dwWearFlag;
		bool FindCharacter;//Finding by character name
		MAX_COUNT minCount, maxCount;
		BYTE minAbs, maxAbs;
		BYTE minAvg;
		BYTE alchemyGrade;
		BYTE alchemyPurity;
		BYTE sashLevel;
	} TFilterInfo;

	typedef struct SShopItemInfo
	{
		TItemPos pos;
		TPriceInfo price;
#ifdef ENABLE_OFFLINE_SHOP_GRID
		TItemDisplayPos display_pos;
#endif
	} TShopItemInfo;

	enum eSubHeaderGC
	{
		SUBHEADER_GC_SHOP_LIST,
		SUBHEADER_GC_SHOP_OPEN,
		SUBHEADER_GC_SHOP_OPEN_OWNER,
		SUBHEADER_GC_SHOP_OPEN_OWNER_NO_SHOP,
		SUBHEADER_GC_SHOP_CLOSE,
		SUBHEADER_GC_SHOP_BUY_ITEM_FROM_SEARCH,
		SUBHEADER_GC_SHOP_FILTER_RESULT,
		SUBHEADER_GC_SHOP_SAFEBOX_REFRESH,
#ifdef ENABLE_SHOP_IN_CITIES
		SUBHEADER_GC_INSERT_SHOP_ENTITY,
		SUBHEADER_GC_REMOVE_SHOP_ENTITY,
#endif
#ifdef ENABLE_OFFLINESHOP_NOTIFICATION
		SUBHEADER_GC_NOTIFICATION,
#endif
#ifdef ENABLE_AVERAGE_PRICE
		SUBHEADER_GC_AVERAGE_PRICE,
#endif
	};

	typedef struct SSubPacketGCShopList
	{
		DWORD dwShopCount;
	} TSubPacketGCShopList;

	typedef struct SSubPacketGCShopOpen
	{
		TShopInfo shop;
	} TSubPacketGCShopOpen;

	typedef struct SSubPacketGCShopOpenOwner
	{
		TShopInfo shop;
	} TSubPacketGCShopOpenOwner;

	typedef struct SSubPacketGCShopFilterResult
	{
		DWORD dwCount;
	} TSubPacketGCShopFilterResult;

	typedef struct SSubPacketGCShopSafeboxRefresh
	{
		unsigned long long valute;
		DWORD dwItemCount;
	} TSubPacketGCShopSafeboxRefresh;

	typedef struct SSubPacketGCShopBuyItemFromSearch
	{
		DWORD dwOwnerID;
		DWORD dwItemID;
	} TSubPacketGCShopBuyItemFromSearch;

#ifdef ENABLE_SHOP_IN_CITIES
	typedef struct SSubPacketGCInsertShopEntity
	{
		DWORD dwVID;
		char szName[OFFLINE_SHOP_NAME_MAX_LEN];
		int iType;
		long x, y, z;
#ifdef ENABLE_SHOP_DECORATION
		DWORD	dwShopDecoration;
#endif
	} TSubPacketGCInsertShopEntity;

	typedef struct SSubPacketGCRemoveShopEntity
	{
		DWORD dwVID;
	} TSubPacketGCRemoveShopEntity;
#endif

#ifdef ENABLE_OFFLINESHOP_EXTEND_TIME
	typedef struct TExtendTime
	{
		DWORD dwTime;
	}TSubPacketCGShopExtendTime;
#endif

#ifdef ENABLE_OFFLINESHOP_NOTIFICATION
	typedef struct LightyNotifications {
		DWORD dwItemID;
		int64_t dwItemPrice; // long long
#ifdef ENABLE_EXTENDED_COUNT_ITEMS
		MAX_COUNT dwItemCount;
#else
		uint8_t dwItemCount;
#endif
	} TSubPacketGCNotification;
#endif


#ifdef ENABLE_AVERAGE_PRICE
	typedef struct SSubPacketAveragePrice
	{
		DWORD vnum;
		long long price;
	}TSubPacketAveragePrice;
#endif

	enum eSubHeaderCG
	{
		SUBHEADER_CG_SHOP_CREATE_NEW,
		SUBHEADER_CG_SHOP_CHANGE_NAME,
		SUBHEADER_CG_SHOP_FORCE_CLOSE,
		SUBHEADER_CG_SHOP_REQUEST_SHOPLIST,
		SUBHEADER_CG_SHOP_OPEN,
		SUBHEADER_CG_SHOP_OPEN_OWNER,
		SUBHEADER_CG_SHOP_BUY_ITEM,
		SUBHEADER_CG_SHOP_ADD_ITEM,
		SUBHEADER_CG_SHOP_REMOVE_ITEM,
		SUBHEADER_CG_SHOP_EDIT_ITEM,
		SUBHEADER_CG_SHOP_FILTER_REQUEST,
		SUBHEADER_CG_SHOP_SAFEBOX_OPEN,
		SUBHEADER_CG_SHOP_SAFEBOX_GET_ITEM,
		SUBHEADER_CG_SHOP_SAFEBOX_GET_VALUTES,
		SUBHEADER_CG_SHOP_SAFEBOX_CLOSE,
		SUBHEADER_CG_CLOSE_BOARD,
#ifdef ENABLE_SHOP_IN_CITIES
		SUBHEADER_CG_CLICK_ENTITY,
#endif
#ifdef ENABLE_OFFLINESHOP_EXTEND_TIME
		SUBHEADER_CG_SHOP_EXTEND_TIME,
#endif
#ifdef ENABLE_AVERAGE_PRICE
		SUBHEADER_CG_AVERAGE_PRICE,
#endif
	};

	typedef struct SSubPacketCGShopCreate
	{
		TShopInfo shop;
	} TSubPacketCGShopCreate;

	typedef struct SSubPacketCGShopChangeName
	{
		char szName[OFFLINE_SHOP_NAME_MAX_LEN];
	} TSubPacketCGShopChangeName;

	typedef struct SSubPacketCGShopOpen
	{
		DWORD dwOwnerID;
	} TSubPacketCGShopOpen;

	typedef struct SSubPacketCGAddItem
	{
		TItemPos pos;
		TPriceInfo price;
#ifdef ENABLE_OFFLINE_SHOP_GRID
		TItemDisplayPos display_pos;
#endif
	} TSubPacketCGAddItem;

	typedef struct SSubPacketCGRemoveItem
	{
		DWORD dwItemID;
	} TSubPacketCGRemoveItem;

	typedef struct SSubPacketCGEditItem
	{
		DWORD dwItemID;
		TPriceInfo price;
		bool allEdit;
	} TSubPacketCGEditItem;

	typedef struct SSubPacketCGFilterRequest
	{
		TFilterInfo filter;
	} TSubPacketCGFilterRequest;

	typedef struct SSubPacketCGShopSafeboxGetItem
	{
		DWORD dwItemID;
	} TSubPacketCGShopSafeboxGetItem;

	typedef struct SSubPacketCGShopSafeboxGetValutes
	{
		unsigned long long gold;
	}TSubPacketCGShopSafeboxGetValutes;

	typedef struct SSubPacketCGShopBuyItem
	{
		DWORD dwOwnerID;
		DWORD dwItemID;
		bool bIsSearch;
	} TSubPacketCGShopBuyItem;


#ifdef ENABLE_SHOP_IN_CITIES
	typedef struct SSubPacketCGShopClickEntity
	{
		DWORD dwShopVID;
	} TSubPacketCGShopClickEntity;
#endif

}
#endif

#ifdef RANKING_SYSTEM
typedef struct SPacketCGLoadServerRanking
{
	uint8_t		header;
	uint8_t		catIdx;
} TPacketCGLoadServerRanking;

typedef struct SPacketGCServerRankingLoad
{
	uint8_t		header;
	uint16_t	size;
} TPacketGCServerRankingLoad;

typedef struct SServerRankTable
{
	char		szName[CHARACTER_NAME_MAX_LEN + 1];
	uint8_t		level;
	uint8_t		raceWithskillGroup;
	uint8_t		empire;
	uint32_t	value;
} TServerRankTable;

typedef struct SPacketCGLoadRanking
{
	uint8_t		header;
	uint8_t		catIdx;
} TPacketCGLoadRanking;

typedef struct SPacketGCRankingLoad
{
	uint8_t		header;
	uint16_t	size;
	uint32_t	nextRefreshTime;
	uint32_t	sundayEndTime;
} TPacketGCRankingLoad;

typedef struct SRankTable
{
	char		szName[CHARACTER_NAME_MAX_LEN + 1];
	uint8_t		level;
	uint8_t		race;
	uint32_t	value;
} TRankTable;
#endif

#ifdef ENABLE_SECONDARY_LEVEL
enum
{
	SL_MAX_BONUSES = 5,
	SL_MAX_ITEMS = 7,
	SL_MAX_UPGRADE_OPTIONS = 2,
};

enum ESecondaryLevelSubheader
{
	SUBHEADER_SECONDARY_LEVEL_GC_SEND_INFO,
	SUBHEADER_SECONDARY_LEVEL_GC_MAX,

	SUBHEADER_SECONDARY_LEVEL_CG_DO_UPGRADE = 0,
	SUBHEADER_SECONDARY_LEVEL_CG_MAX,
};

typedef struct SPacketGCSecondaryLevel
{
	BYTE header;
	int	 size;
	BYTE subheader;

	SPacketGCSecondaryLevel() :
		header(HEADER_GC_SECONDARY_LEVEL),
		subheader(SUBHEADER_SECONDARY_LEVEL_GC_MAX)
	{
		size = sizeof(SPacketGCSecondaryLevel);
	}
} TPacketGCSecondaryLevel;

typedef struct SPacketCGSecondaryLevel
{
	BYTE bHeader;
	BYTE bSubHeader;
	int	 iSize;

	SPacketCGSecondaryLevel() :
		bHeader(HEADER_CG_SECONDARY_LEVEL),
		bSubHeader(SUBHEADER_SECONDARY_LEVEL_CG_MAX)
	{
		iSize = sizeof(SPacketCGSecondaryLevel);
	}
} TPacketCGSecondaryLevel;

typedef struct SPacketGCSecondaryLevelInfo
{
	BYTE	bLevel;
	DWORD	dwApplyType[SL_MAX_BONUSES];
	long	lApplyValue[SL_MAX_BONUSES];
	DWORD	dwRequiredItemVnum[SL_MAX_ITEMS];
	long	lRequiredItemCount[SL_MAX_ITEMS];
	int64_t	llRequiredGold[SL_MAX_UPGRADE_OPTIONS];
	DWORD	dwChance[SL_MAX_UPGRADE_OPTIONS];
} TPacketGCSecondaryLevelInfo;

typedef struct SPacketCGSecondaryLevelUpgrade
{
	BYTE	bOption;
} TPacketCGSecondaryLevelUpgrade;
#endif

#ifdef __ENABLE_MINING_WITH_SPACE__
enum EMiningSubheaders
{
	MINING_SUBHEADER_CANCEL,
	MINING_SUBHEADER_NEAREST,
};

typedef struct SPacketCGMining
{
	BYTE header;
	BYTE subheader;
} TPacketCGMining;
#endif

#ifdef ENABLE_MOUNT_SYSTEM
typedef struct SPacketMountInfo
{
	BYTE	bHeader;
	DWORD	dwVnum;
	char	szName[64];
	WORD	wLevel;
	int		iExp;
	int		iRequiredExp;
	bool	bSummoned;
} TPacketGCMountInfo;
#endif

#pragma pack(pop)
//martysama0134's ceqyqttoaf71vasf9t71218
