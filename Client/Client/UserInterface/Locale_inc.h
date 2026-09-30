#pragma once

#define CHECK_CLIENT_VERSION
//#define ENABLE_PATCHER_LAUNCH_CHECK					// Block direct launch, must use Elvion-Patcher.exe
#ifdef ENABLE_PATCHER_LAUNCH_CHECK
#define PATCHER_EXE_NAME	"Elvion-Patcher.exe"
#define PATCHER_EXE_CRC32	0x7A6CFDE0			
#endif

//hlbot detection
//#define ENABLE_ANTIHL

//#define ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
	//#define ENABLE_PRIVATE_SHOP_PREMIUM_TIME
	//#define ENABLE_PRIVATE_SHOP_BUNDLE_REQ
	#define ENABLE_PRIVATE_SHOP_CHEQUE
	#define ENABLE_PRIVATE_SHOP_LIMITED_DISTANCE_RENDERING
	//#define ENABLE_PRIVATE_SHOP_BUILD_LIMITATIONS
	// #define ENABLE_PRIVATE_SHOP_LOCKED_SLOTS
	// #define ENABLE_PRIVATE_SHOP_PRICE_SORT
	#define ENABLE_PRIVATE_SHOP_TIME_SORT
	//#define ENABLE_PRIVATE_SHOP_DIFFERENT_PRICE_RESTRICTION
	//#define ENABLE_PRIVATE_SHOP_CHANGE_LOOK
	//#define ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
	// #define ENABLE_PRIVATE_SHOP_APPLY_RANDOM
#endif

#define ENABLE_HAIR_SPECULAR
#define ENABLE_AUTO_SHOUT																	// Auto Shout
#define ENABLE_LINK_IN_CHAT																	// Links in Chat
#define ENABLE_CLIP_MASK																	// Clip Mask
#define ENABLE_OFFLINE_SHOP																	// Offlineshop Reworked
#ifdef ENABLE_OFFLINE_SHOP
#define ENABLE_SHOP_IN_CITIES																// Shop Entities in City
#define ENABLE_LARGE_DYNAMIC_PACKET															// Package Limits are increased for offshop
#define ENABLE_SHOP_SEARCH																	// OfflineShop Shop Search
#define ENABLE_OFFLINE_SHOP_GRID															// OfflineShop Grid
//#define ENABLE_OFFLINESHOP_NOTIFICATION														// Offlineshop Notification
#define ENABLE_SHOP_DECORATION																// Offlineshop Decoration
#define ENABLE_OFFLINESHOP_EXTEND_TIME														// Extend shop time
#define ENABLE_AVERAGE_PRICE
#endif
#define MAX_COUNT uint16_t// (uint16_t = 65535)
//////////////////////////////////////////////////////////////////////////
// ### Default Ymir Macros ###
#define LOCALE_SERVICE_EUROPE
//#define _IMPROVED_PACKET_ENCRYPTION_
#define ENABLE_COSTUME_SYSTEM
#define ENABLE_ENERGY_SYSTEM
#define ENABLE_DRAGON_SOUL_SYSTEM
#define ENABLE_NEW_EQUIPMENT_SYSTEM
// ### Default Ymir Macros ###
//////////////////////////////////////////////////////////////////////////


//////////////////TESTYYYYYYYY?????????????????????????????????? wida�?
#define ENABLE_DROP_INFO_PCT						// Procent na drop w wiki dropu
#define ENABLE_BELT_ATTRIBUTES						// Bonowanie pasow w switchbocie
#define ENABLE_GLOVE_ATTRIBUTES						// Bonowanie rekawic w switchbocie
#define ENABLE_TALISMAN_ATTRIBUTES						// Bonowanie rekawic w switchbocie

///NOWE
#define ENABLE_QUICK_PICKUP																	// Pickup all items quickly
#define ENABLE_QUEST_RENEWAL // Quest page renewal
#define ENABLE_DROP_INFO							// Wikipedia Dropu
//#define ENABLE_LRN_LOCATION_SYSTEM					// Zapis Lokalizacji
#define ENABLE_ALIGN_RENEWAL						// System Rang z Bonusami
#define ENABLE_SHINING_SYSTEM						// System Efekt�w
#define ENABLE_EXTENDED_BLEND						// Niesko�czone dopalacze
#define ENABLE_NEW_PET_SYSTEM						// System Pet�w by Zoom
#define __BL_MOVE_CHANNEL__							//
//#define ENABLE_OFFLINE_SHOP_SYSTEM					// Sklepy Offline by Ups..
#define ENABLE_LONG_LONG							// Limit Yang (Long Long)

#define ENABLE_TOMBOLA								// Ko�o losu
#ifdef ENABLE_TOMBOLA
//#define ENABLE_TOMBOLA_PERCENTAGE
#endif

#define ITEM_CHECKINOUT_UPDATE

//#define RANKING_SYSTEM								// New ranking
#define WEEKLY_RANK_BYLUZER							// Tygodniowy Ranking				- 03.03.2023
#define TITLE_SYSTEM_BYLUZER						// System Tytu��w					- 04.03.2023
#define ENABLE_SWITCHBOT							// SwitchBot						- 04.04.2023

#define WJ_SPLIT_INVENTORY_SYSTEM                   // Dodatkowy Ekwipunek				- 13.05.2023
//#define ENABLE_CUBE_RENEWAL_WORLDARD				// Cube System						- 18.05.2023
#define ENABLE_REFINE_RENEWAL						// Auto Refine System				- 18.05.2023
#define ENABLE_FOV_OPTION							// Zmiana FOV						- 19.05.2023
#define ENABLE_CHEQUE_SYSTEM						// System Won�w
//#define ENABLE_DYNAMIC_FONTS						// Dynamiczna zmiana czcionki		- 18.06.2023
#define ENABLE_DOG_MODE								// Dynamiczna zmiana mob�w na psy	- 18.06.2023
#define ENABLE_REMOVE_ITEM							// Kosz na Przedmioty by Filip		- 18.06.2023 
#define ENABLE_STOLE_COSTUME					
#define ENABLE_GLOVE_SYSTEM							// Rekawice							- 18.07.2023
#define ENABLE_SYSTEM_RUNE							// System Run						- 19.07.2023
#define ENABLE_AFFECT_POLYMORPH_REMOVE			
#define ENABLE_SKILL_AFFECT_REMOVE
#define ENABLE_ARTEFAKT_SYSTEM						//artefakty
#define ENABLE_DUNGEON_INFO_SYSTEM					// Dungeon Info						- 29.07.2023
#define __BL_CHEST_DROP_INFO__						//podglad dropu szkat
#define ENABLE_MINIMAP_DUNGEONINFO
#define RENEWAL_RENDERING_RECT_BOX
#define ENABLE_EXTENDED_ITEM_COUNT
#define ENABLE_PUNKTY_OSIAGNIEC						//po
#define ENABLE_SPECIAL_BONUS						//Dodatkowy Bonus w Broni i Zbroi
#define ENABLE_AURA_SYSTEM							//szaty aury
#define ENABLE_NEW_STONE_DETACH						//Okno wyciagania kd
#define ENABLE_HIDE_COSTUME_SYSTEM					//Ukrywanie kostiumow
#define ENABLE_COLLECT_WINDOW						//System kolekcjonera w GUI.
#define ENABLE_COSTUME_EMBLEMAT						//emblematy
#define ENABLE_SKILE_PASYWNE						// Skile pasywne
#define HIDE_EMPIRE_FLAG							//Ukrywanie flag kr�lestwa dla pet�w, sklep�w offline
#define ENABLE_CHATTING_WINDOW_RENEWAL				// Chat system
//#define ENABLE_INSTANT_CHEST_OPEN					// Szybkie otwieranie szkat
#define BL_OFFLINE_MESSAGE							// Wiadomo�ci offline
#define ENABLE_EXTENDED_ITEMNAME_ON_GROUND			// Extended itemname ground
//#define __BL_PARTY_POSITION__						// Party member position on minimap
#define ENABLE_COLLECTIONS_SYSTEM					// Kolekcjoner
#define WJ_MULTI_TEXTLINE							// ? multitextline
#define ENABLE_EVENT_MANAGER						// Okno Eventow
#define __BACK_DUNGEON__							// Powrot na dungi
//#define ENABLE_INSTANT_INGOT_OPEN					// Uzywanie sztabek bez laga
#define USE_FISH_SYSTEM								// Rybki z bonusami
#define ENABLE_ODLAMKI_SYSTEM						// Wymiana kd na odlamki
#define __BL_SMOOTH_SCROLL__						// smooth scroll
#define ENABLE_KEYCHANGE_SYSTEM						// ustawienia klawiszy
#define ENABLE_EXTRABONUS_SYSTEM					// wzmocnienia
#define ENABLE_QUIVER_SYSTEM						// kolczany ninja
#define ENABLE_SKILL_TABLE_RENEW					// Enable synchronization skill_proto -> skilltable
#define __BL_HYPERLINK_ITEM_ICON__
#define __TEAM_MEMBER_STATUS__
#define ENABLE_SAVE_LOCATION_SYSTEM
#define ENABLE_MOUNT_SYSTEM
//#define ENABLE_IN_GAME_LOG_SYSTEM															// Ingame Log System
//#define ENABLE_PVP_RANKING
	#ifdef ENABLE_PVP_RANKING
		#define ENABLE_PVP_RANKING_WITH_EMPIRE
	#endif



//#define ENABLE_VS_SHOP_SEARCH						// Wyszukiwarka sklepy offline
#define ENABLE_VS_COSTUME_BONUSES					// bonusy czasowe w kostiumach

#define WJ_ENABLE_TRADABLE_ICON
#define RENEWAL_DEAD_PACKET
#define ENABLE_VIEW_TARGET_PLAYER_HP
#define ENABLE_VIEW_TARGET_DECIMAL_HP
#define ENABLE_ENTITY_PRELOADING
//#define ENABLE_MESSENGER_TEAM

#define ENABLE_HWID_SYSTEM							// hwid bany
#define TAKE_LEGEND_DAMAGE_BOARD_SYSTEM
#ifdef TAKE_LEGEND_DAMAGE_BOARD_SYSTEM
#	define LEGEND_DAMAGE_BOARD_MAX_ENTRIES	15
#endif

//OPTYMALIZACJE
//#define ENABLE_LOADING_PERFORMANCE			// Client loading performance ( Boost Loading Time )
//#define __LOADING_PERFORMANCE_CHECK__
#define ENABLE_PERFORMANCE_IMPROVEMENTS_NEW
//#define ENABLE_LOADING_PERFORMANCE
//#define IMPROVE_ENTITY_FREEZE
//#define MEMORYLEAK_FIX_OWSAP


#define ENABLE_FIX_MOBS_LAG
#if defined(ENABLE_FIX_MOBS_LAG)
	// -> The define ENABLE_FIX_MOBS_LAG have problems in device reseting.
	// -> With this new define all this problems are fixed.
#define FIX_MOBS_LAG_FIX
#endif

//FIXY
#define ENABLE_AFFECT_FIX
//#define TRANSPARENCY_BUILDING_FIX

// Aslan Buff - START - 15.05.2023
#define ENABLE_EXTENDED_SOCKETS
#define ENABLE_ASLAN_BUFF_NPC_SYSTEM
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	#define ASLAN_BUFF_NPC_USE_SKILL_TECH_LEVEL
#endif
// Aslan Buff - END
// 

//////////////////////////////////////////////////////////////////////////
// ### New From LocaleInc ###
#define ENABLE_PACK_GET_CHECK
//#define ENABLE_CANSEEHIDDENTHING_FOR_GM
#define ENABLE_PROTOSTRUCT_AUTODETECT
#define ENABLE_PLAYER_PER_ACCOUNT5
#define ENABLE_LEVEL_IN_TRADE
#define ENABLE_DICE_SYSTEM
#define ENABLE_EXTEND_INVEN_SYSTEM
#define ENABLE_LVL115_ARMOR_EFFECT
#define ENABLE_SLOT_WINDOW_EX
#define ENABLE_TEXT_LEVEL_REFRESH
#define ENABLE_USE_COSTUME_ATTR
#define ENABLE_DISCORD_RPC
#ifdef ENABLE_DISCORD_RPC
#define WEB_URL		"elvion.cc"
#define BUTTON_NAME		"Play Now!"
#define DISCORD_URL		"https://discord.gg/elvion"
#endif

#define ENABLE_PET_SYSTEM_EX
//#define ENABLE_LOCALE_EX
#define ENABLE_NO_DSS_QUALIFICATION
#define ENABLE_NO_SELL_PRICE_DIVIDED_BY_5
#define ENABLE_RESP_SYSTEM
#define ENABLE_BOT_CONTROL
#define ENABLE_SECONDARY_LEVEL	
//#define ENABLE_ENLIGHTENMENT_SYSTEM	
#define __ENABLE_MINING_WITH_SPACE__
//#define WORLD_BOSS_YUMA
#define __ENABLE_SHAMAN_ATTACK_FIX__
#define __ENABLE_NINJA_ATTACK_FIX__
#define ENABLE_DS_GRADE_MYTH 																// Myth DragonSoul Update
#define ENABLE_DS_CHANGE_ATTR // Dragon Soul Change Attribute
#define ENABLE_SKYBOX_SELECT																// Skybox Select
#define ENABLE_WHEEL_OF_FORTUNE
#define __EFEKT_SZARFY__
#define __BL_KILL_BAR__
#define ENABLE_METIN_QUEUE
#define ENABLE_RENEWAL_AFFECT_SHOWER
#define __PENDANT__
#define FLASH_IN_TASKBAR
#define ENABLE_TITLE_ACHIEVEMENT_SYSTEM
#define ENABLE_SKILL_COSTUME_SYSTEM
#ifdef ENABLE_SKILL_COSTUME_SYSTEM
	#define SKILL_COSTUME_EFFECT_COUNT 8	//count of new skills
#endif
#define ENABLE_SKILL_PREVIEW
#define ENABLE_WIKI
#define __DUNGEON_INFO__
#define ENABLE_VOTE4BUFF
#define ENABLE_BUY_WITH_ITEM
#ifdef ENABLE_BUY_WITH_ITEM
#define MAX_SHOP_PRICES 5
#endif
#define ENABLE_VIP_SYSTEM							// Premium System
#define ENABLE_AUTO_SELECT_SKILL
#define ENABLE_MULTI_FARM_BLOCK

// ### HotReload ###
//////////////////////////////////////////////////////////////////////////
//#define USE_HOT_RELOAD
//#define USE_FILE_SYSTEM
//////////////////////////////////////////////////////////////////////////

// ### New From LocaleInc ###
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ### From GameLib ###
#//define ENABLE_WOLFMAN_CHARACTER
// #define ENABLE_MAGIC_REDUCTION_SYSTEM
#define ENABLE_MOUNT_COSTUME_SYSTEM
#define ENABLE_WEAPON_COSTUME_SYSTEM
// ### From GameLib ###
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ### New System Defines - Extended Version ###

// if is define ENABLE_ACCE_COSTUME_SYSTEM the players can use shoulder sash
// if you want to use object scaling function you must defined ENABLE_OBJ_SCALLING
#define ENABLE_ACCE_COSTUME_SYSTEM
#define ENABLE_OBJ_SCALLING
// #define USE_ACCE_ABSORB_WITH_NO_NEGATIVE_BONUS

// if you want use SetMouseWheelScrollEvent or you want use mouse wheel to move the scrollbar
#define ENABLE_MOUSEWHEEL_EVENT

//if you want to see highlighted a new item when dropped or when exchanged
#define ENABLE_HIGHLIGHT_NEW_ITEM

// it shows emojis in the textlines
#define ENABLE_EMOJI_SYSTEM

// effects while hidden won't show up
//#define __ENABLE_STEALTH_FIX__

// circle dots in minimap instead of squares
#define ENABLE_MINIMAP_WHITEMARK_CIRCLE

// for debug: print received packets
#define ENABLE_PRINT_RECV_PACKET_DEBUG

#define __BL_CLIENT_LOCALE_STRING__
#define ENABLE_GRAPHIC_ON_OFF // System graphic options

// Damage Values
#define ENABLE_EFFECT_TARGET_CRITICAL
#define ENABLE_EFFECT_TARGET_PENETRATE
#define ENABLE_EFFECT_TARGET_CRITICAL_AND_PENETRATE
#define ENABLE_EFFECT_DAMAGE_CRITICAL
#define ENABLE_EFFECT_DAMAGE_PENETRATE
#define ENABLE_EFFECT_DAMAGE_CRITICAL_AND_PENETRATE
#define ENABLE_EFFECT_DAMAGE_POISON
#define ENABLE_EFFECT_DAMAGE_FIRE
#define DOTTED_DMG
//#define __SPIN_WHEEL__
//
#define __ENABLE_POLYMORPH_SYSTEM__
#define __INVENTORY_BUFFERING__

#define __ENABLE_TOGGLE_ITEMS__
#ifdef __ENABLE_TOGGLE_ITEMS__
	#define __ENABLE_AUTO_PICKUP__
#endif
#define ENABLE_CUSTOM_SPECULAR_MAP
#define ENABLE_CHAT_SETTINGS																// New Chat 

#define __FORCE_PICKUP__

//fixes
#define ENABLE_ENITY_CHARACTER_FIX
#define ENABLE_ITEMSHOP
#ifdef ENABLE_ITEMSHOP
#define USE_ITEMSHOP_RENEWED
#endif
#define ENABLE_MESSENGER_BLOCK
#define DISABLE_POST_PROCESSING
#define ANK_SYSTEM_GRAPHICS
#ifdef ANK_SYSTEM_GRAPHICS
	//#define ANKIRA_TRANSPARENCY_BUILDING_FIX
	//#define ANKIRA_IMPROVE_ENTITY_FREEZE
	//#define ANKIRA_BLOOD_EFFECT
	//#define ANKIRA_GPU_LEAF_PLACEMENT
	//#define ANKIRA_GPU_TREE_WIND
	//#define ANKIRA_TREE_DYNAMIC_LIGHT
	#define ANKIRA_DEATH_CAMERA
	//#define ANKIRA_WATER_REFLECTION
	//#define ANKIRA_UNDERWATER_CAMERA
	#define ANKIRA_SHARPNESS
	//#define ANKIRA_DYNAMIC_LIGHT
	//#define ANKIRA_SPECULAR
	//#define ANKIRA_ENVIRONMENT_EFFECT_OPTION
	#define ANKIRA_GRAPHICS_SOUND
	//#define ENABLE_CPP_PSM
#endif
#define ENABLE_FIX_EFFECT_MOVEMENT
#define ENABLE_BOSS_SHOWER_ON_ATLAS															// Boss Shower Atlas
#define ENABLE_MINIMAP_TELEPORT																// Teleport to a Location Via Minimap
#define ENABLE_NOTIFICATION_SYSTEM
#define _ENABLE_BATTLEPASS_
#define STONE_SCALE
#define BOSS_SCALE
//#define __AUTO_SKILL_READER__
#define ENABLE_RENDER_TARGET_EFFECT
#define MT2009_CINEMATIC
#define ENABLE_BADGE_NOTIFICATION_MANAGER	// New notifications for the game.

#define D3DLIGHT8					D3DLIGHT9
#define D3DMATERIAL8				D3DMATERIAL9
#define IDirect3DVertexBuffer8		IDirect3DVertexBuffer9
#define LPDIRECT3DTEXTURE8			LPDIRECT3DTEXTURE9
#define LPDIRECT3DSURFACE8			LPDIRECT3DSURFACE9
#define D3DVIEWPORT8				D3DVIEWPORT9
#define LPDIRECT3DDEVICE8			LPDIRECT3DDEVICE9
#define LPDIRECT3DVERTEXBUFFER8		LPDIRECT3DVERTEXBUFFER9
#define LPDIRECT3DINDEXBUFFER8		LPDIRECT3DINDEXBUFFER9
#define DXLOCKTYPE					(void**)
#define D3DVERTEXELEMENT8			D3DVERTEXELEMENT9
#define LPDIRECT3DBASETEXTURE8		LPDIRECT3DBASETEXTURE9
#define LPDIRECT3DPIXELSHADER8		LPDIRECT3DPIXELSHADER9
#define D3DADAPTER_IDENTIFIER8		D3DADAPTER_IDENTIFIER9
#define IDirect3D8					IDirect3D9
#define IDirect3DDevice8			IDirect3DDevice9
#define D3DCAPS8					D3DCAPS9
#define LPDIRECT3D8					LPDIRECT3D9
#define D3DCAPS2_CANRENDERWINDOWED	DDCAPS2_CANRENDERWINDOWED
#define IDirect3DTexture8			IDirect3DTexture9
#define Direct3DCreate8				Direct3DCreate9
#define IDirect3DSurface8			IDirect3DSurface9