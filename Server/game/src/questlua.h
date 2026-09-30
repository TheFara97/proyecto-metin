#ifndef __HEADER_QUEST_LUA__
#define __HEADER_QUEST_LUA__

#include "quest.h"
#include "buffer_manager.h"

extern int test_server;

namespace quest
{
	extern void RegisterPCFunctionTable();
	extern void RegisterNPCFunctionTable();
	extern void RegisterTargetFunctionTable();
	extern void RegisterAffectFunctionTable();
	extern void RegisterBuildingFunctionTable();
	extern void RegisterMarriageFunctionTable();
	extern void RegisterITEMFunctionTable();
	extern void RegisterDungeonFunctionTable();
	extern void RegisterQuestFunctionTable();
	extern void RegisterPartyFunctionTable();
	extern void RegisterHorseFunctionTable();
	extern void RegisterPetFunctionTable();
	extern void RegisterGuildFunctionTable();
	extern void RegisterGameFunctionTable();
	extern void RegisterArenaFunctionTable();
	extern void RegisterGlobalFunctionTable(lua_State* L);
	extern void RegisterForkedFunctionTable();
	extern void RegisterMonarchFunctionTable();
	extern void RegisterOXEventFunctionTable();
	extern void RegisterMgmtFunctionTable();
	extern void RegisterDanceEventFunctionTable();
	extern void RegisterDragonLairFunctionTable();
	extern void RegisterDragonSoulFunctionTable();
#ifdef ENABLE_QUEST_DND_EVENT
	extern void RegisterDNDFunctionTable();
#endif

	extern void combine_lua_string(lua_State* L, std::ostringstream &s);
	
#if defined(__DUNGEON_INFO_SYSTEM__)
	struct FUpdateDungeonRanking
	{
		std::string m_strQuestName;
		FUpdateDungeonRanking(std::string strQuestName) : m_strQuestName(strQuestName) {}
		void operator () (LPCHARACTER ch);
	};
#endif

	struct FSetWarpLocation
	{
		int32_t map_index;
		int32_t x;
		int32_t y;

		FSetWarpLocation (int32_t _map_index, int32_t _x, int32_t _y) :
			map_index (_map_index), x (_x), y (_y)
		{}
		void operator () (LPCHARACTER ch);
	};

	struct FSetQuestFlag
	{
		std::string flagname;
		int value;

		void operator () (LPCHARACTER ch);
	};

	struct FPartyCheckFlagLt
	{
		std::string flagname;
		int value;

		bool operator () (LPCHARACTER ch);
	};

	struct FPartyChat
	{
		int iChatType;
		const char* str;

		FPartyChat(int ChatType, const char* str);
		void operator() (LPCHARACTER ch);
	};

	struct FPartyClearReady
	{
		void operator() (LPCHARACTER ch);
	};

	struct FSendPacket
	{
		TEMP_BUFFER buf;

		void operator() (LPENTITY ent);
	};

#ifdef ENABLE_NEWSTUFF
	struct FSendChatPacket
	{
		DWORD m_chat_type;
		std::string m_text;

		FSendChatPacket(DWORD chat_type, std::string text) : m_chat_type(chat_type), m_text(text) {}
		void operator() (LPENTITY ent);
	};
#endif

	struct FSendPacketToEmpire
	{
		TEMP_BUFFER buf;
		BYTE bEmpire;

		void operator() (LPENTITY ent);
	};

	struct FWarpEmpire
	{
		BYTE m_bEmpire;
		int32_t m_lMapIndexTo;
		int32_t m_x;
		int32_t m_y;

		void operator() (LPENTITY ent);
	};

	EVENTINFO(warp_all_to_map_my_empire_event_info)
	{
		BYTE 	m_bEmpire;
		int32_t	m_lMapIndexFrom;
		int32_t 	m_lMapIndexTo;
		int32_t 	m_x;
		int32_t	m_y;

		warp_all_to_map_my_empire_event_info()
		: m_bEmpire( 0 )
		, m_lMapIndexFrom( 0 )
		, m_lMapIndexTo( 0 )
		, m_x( 0 )
		, m_y( 0 )
		{
		}
	};

	EVENTFUNC(warp_all_to_map_my_empire_event);

	struct FBuildLuaGuildWarList
	{
		lua_State * L;
		int m_count;

		FBuildLuaGuildWarList(lua_State * L);
		void operator() (DWORD g1, DWORD g2);
	};
}

#ifdef ENABLE_MINIMAP_DUNGEONINFO
	struct FDungeonSendMiniMapDungeonInfo
	{
		BYTE bSubHeader;
		int dValue1;
		int dValue2;
		int dValue3;
		const char* szNotice;

		void operator () (LPCHARACTER ch);
	};
#endif

#endif /*__HEADER_QUEST_LUA__*/
//martysama0134's ceqyqttoaf71vasf9t71218
