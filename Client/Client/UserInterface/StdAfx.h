#pragma once

// Patcher launch enforcement: game must be started via Elvion-Patcher.exe
// The patcher must pass this string as a command-line argument to Elvion.exe
//#define NEEDED_COMMAND_ARGUMENT "elvion_auth_launch_2026"

#pragma warning(disable:4702)
#pragma warning(disable:4100)
#pragma warning(disable:4201)
#pragma warning(disable:4511)
#pragma warning(disable:4663)
#pragma warning(disable:4018)
#pragma warning(disable:4245)

#if _MSC_VER >= 1400
//if don't use below, time_t is 64bit
#define _USE_32BIT_TIME_T
#endif
#include <iterator>
#include "../eterLib/StdAfx.h"
#include "../eterPythonLib/StdAfx.h"
#include "../gameLib/StdAfx.h"
#include "../scriptLib/StdAfx.h"
#include "../milesLib/StdAfx.h"
#include "../EffectLib/StdAfx.h"
#include "../PRTerrainLib/StdAfx.h"
#include "../SpeedTreeLib/StdAfx.h"

#ifndef __D3DRM_H__
#define __D3DRM_H__
#endif

#include <dshow.h>
#include <MicrosoftDirectX/qedit.h>

#include "Locale.h"
#define Py_NO_ENABLE_SHARED

#include "GameType.h"
extern DWORD __DEFAULT_CODE_PAGE__;

#define APP_NAME	"Metin 2"

enum
{
	POINT_MAX_NUM = 255,
	CHARACTER_NAME_MAX_LEN = 24,
#if defined(LOCALE_SERVICE_JAPAN)
	PLAYER_NAME_MAX_LEN = 16,
#else
	PLAYER_NAME_MAX_LEN = 12,
#endif
#ifdef ENABLE_OFFLINE_SHOP
	OFFLINE_SHOP_NAME_MAX_LEN = 40 + CHARACTER_NAME_MAX_LEN + 1,
	OFFLINE_SHOP_ITEM_MAX_LEN = 24,
#endif
};
#ifdef ENABLE_LRN_LOCATION_SYSTEM
enum
{
	MAX_LOCATION_POSITION = 25,
	MAX_LOCATION_CHAR_STRING = 2048,
};
#endif
void initudp();
void initapp();
void initime();
void initsystemSetting();
void initemojiSystem();
void initchr();
void initchrmgr();
void initChat();
void initCube();
void initTextTail();
void initime();
void initItem();
void initNonPlayer();
void initnet();
void initPlayer();
void initSectionDisplayer();
void initServerStateChecker();
void initRenderTarget();
void initTrade();
void initMiniMap1();
void initProfiler();
void initEvent();
void initeffect();
void initsnd();
void initeventmgr();
void initBackground();
void initwndMgr();
void initshop();
void initpack();
void initskill();
void initfly();
void initquest();
void initsafebox();
void initguild();
#ifdef ENABLE_WIKI
void initWiki();
#endif
#ifdef _ENABLE_BATTLEPASS_
void initBattlePass();
#endif
void initMessenger();
#ifdef ENABLE_OFFLINE_SHOP
void initofflineshop();
#endif
#if defined(ENABLE_DUNGEON_INFO_SYSTEM)
void initDungeonInfo();
#endif
#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
void intcuberenewal();
#endif
#ifdef ENABLE_SWITCHBOT
void initSwitchbot();
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
void initAcce();
#endif
#ifdef ENABLE_AURA_SYSTEM
void initAura();
#endif

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
void initPrivateShop();
#endif
#ifdef ENABLE_IN_GAME_LOG_SYSTEM
void initInGameLog();
#endif
#ifdef ENABLE_SECONDARY_LEVEL
void initSecondaryLevelManager();
#endif

extern const std::string& ApplicationStringTable_GetString(DWORD dwID);
extern const std::string& ApplicationStringTable_GetString(DWORD dwID, LPCSTR szKey);

extern const char* ApplicationStringTable_GetStringz(DWORD dwID);
extern const char* ApplicationStringTable_GetStringz(DWORD dwID, LPCSTR szKey);

extern void ApplicationSetErrorString(const char* szErrorString);
//martysama0134's ceqyqttoaf71vasf9t71218
