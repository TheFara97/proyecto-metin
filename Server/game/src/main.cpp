#include "stdafx.h"
#include "constants.h"
#include "config.h"
#include "event.h"
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
#include "locale_item_manager.h"
#endif
#include <lzo/lzo1x.h>
#include "packet.h"
#include "desc_manager.h"
#include "item_manager.h"
#include "char.h"
#include "char_manager.h"
#include "mob_manager.h"
#include "motion.h"
#include "sectree_manager.h"
#include "shop_manager.h"
#include "regen.h"
#include "text_file_loader.h"
#include "skill.h"
#include "pvp.h"
#include "party.h"
#include "questmanager.h"
#include "profiler.h"
#include "lzo_manager.h"
#include "messenger_manager.h"
#include "db.h"
#include "log.h"
#include "p2p.h"
#include "guild_manager.h"
#include "dungeon.h"
#include "cmd.h"
#include "refine.h"
#include "banword.h"
#include "priv_manager.h"
#include "war_map.h"
#include "building.h"
#include "login_sim.h"
#include "target.h"
#include "marriage.h"
#include "wedding.h"
#include "fishing.h"
#include "item_addon.h"
#include "locale_service.h"
#include "arena.h"
#include "OXEvent.h"
#include "monarch.h"
#include "polymorph.h"
#include "blend_item.h"
#include "castle.h"
#include "ani.h"
#include "over9refine.h"
#include "horsename_manager.h"
#include "MarkManager.h"
#include "spam.h"
#include "panama.h"
#include "threeway_war.h"
#include "DragonLair.h"
#include "skill_power.h"
#include "DragonSoul.h"
#ifdef ENABLE_SWITCHBOT
#include "switchbot.h"
#endif

#include <boost/bind.hpp>
#ifndef _WIN32
	#include "limit_time.h"
#endif

#ifdef ENABLE_RESP_SYSTEM
#include "resp_manager.h"
#endif
#ifdef __ENABLE_COLLECTIONS_SYSTEM__
	#include "CollectionsSystem.hpp"
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	#include "offlineshop_config.h"
	#include "offlineshop_manager.h"
#endif
#if defined(__DUNGEON_INFO_SYSTEM__)
#	include "dungeon_info.h"
#endif

#ifdef ENABLE_REAL_TIME_REGEN
	#include "RealTimeRegen.hpp"
#endif
#ifdef __TEAM_MEMBER_STATUS__
#include "TeamList.hpp"
#endif
#if defined (__FreeBSD__) && defined(__FILEMONITOR__)
	#include "FileMonitor_FreeBSD.h"
#endif
#ifdef ENABLE_GOOGLE_TEST
#ifndef _WIN32
#include <gtest/gtest.h>
#endif
#endif

#ifdef ENABLE_PVP_RANKING
#include "pvp_ranking.h"
#endif

#ifdef USE_STACKTRACE
#include <execinfo.h>
#endif
#ifdef __AUTH_QUEUEING__
#	include "AuthQueue.hpp"
#endif

#ifdef __WORLD_BOSS_YUMA__
#include "worldboss.h"
#endif
#ifdef ENABLE_IN_GAME_LOG_SYSTEM
#include "InGameLogManager.h"
#endif


#ifdef __PREMIUM_PRIVATE_SHOP__
#include "private_shop_manager.h"
#include "private_shop.h"
#endif
#ifdef __ENABLE_POLYMORPH_SYSTEM__
	#include "PolySystem.h"
#endif
#ifdef __DUNGEON_INFO_ENABLE__
#include "DungeonInfoManager.hpp"
#endif
#ifdef ENABLE_OFFLINE_SHOP
#include "new_offlineshop.h"
#include "new_offlineshop_manager.h"
#endif
#ifdef RANKING_SYSTEM
#include "server_ranking_manager.h"
#include "ranking_manager.h"
#endif
#include "fish_wiki.h"
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
#include "AchievementTitle.h"
#endif
#ifdef _ENABLE_BATTLEPASS_
#include "BattlePassManager.h"
#endif
#include "MonsterSpawner.hpp"

#include "VersionManager.hpp"

#include "EventFunctionHandler.hpp"
//extern const char * _malloc_options;
#if defined(__FreeBSD__) && defined(DEBUG_ALLOC)
extern void (*_malloc_message)(const char* p1, const char* p2, const char* p3, const char* p4);
// FreeBSD _malloc_message replacement
void WriteMallocMessage(const char* p1, const char* p2, const char* p3, const char* p4) {
	FILE* fp = ::fopen(DBGALLOC_LOG_FILENAME, "a");
	if (fp == NULL) {
		return;
	}
	::fprintf(fp, "%s %s %s %s\n", p1, p2, p3, p4);
	::fclose(fp);
}
#endif

int             num_events_called = 0;
int             max_bytes_written = 0;
int             current_bytes_written = 0;
int             total_bytes_written = 0;
BYTE		g_bLogLevel = 0;

socket_t	tcp_socket = 0;
socket_t	p2p_socket = 0;

LPFDWATCH	main_fdw = NULL;

int		io_loop(LPFDWATCH fdw);

int		start(int argc, char **argv);
int		idle();
void	destroy();

void 	test();

enum EProfile
{
	PROF_EVENT,
	PROF_CHR_UPDATE,
	PROF_IO,
	PROF_HEARTBEAT,
	PROF_MAX_NUM
};

static DWORD s_dwProfiler[PROF_MAX_NUM];

int g_shutdown_disconnect_pulse;
int g_shutdown_disconnect_force_pulse;
int g_shutdown_core_pulse;
bool g_bShutdown=false;

extern void CancelReloadSpamEvent();

void ContinueOnFatalError()
{
#ifdef USE_STACKTRACE
	void* array[200];
	std::size_t size;
	char** symbols;

	size = backtrace(array, 200);
	symbols = backtrace_symbols(array, size);

	std::ostringstream oss;
	oss << std::endl;
	for (std::size_t i = 0; i < size; ++i) {
		oss << "  Stack> " << symbols[i] << std::endl;
	}

	free(symbols);

	sys_err("FatalError on %s", oss.str().c_str());
#else
	sys_err("FatalError");
#endif
}

void ShutdownOnFatalError()
{
	if (!g_bShutdown)
	{
		sys_err("ShutdownOnFatalError!!!!!!!!!!");
		{
			char buf[256];

			strlcpy(buf, "[LS;738]", sizeof(buf));
			SendNotice(buf);
			strlcpy(buf, "[LS;739]", sizeof(buf));
			SendNotice(buf);
			strlcpy(buf, "[LS;740]", sizeof(buf));
			SendNotice(buf);
		}

		g_bShutdown = true;
		g_bNoMoreClient = true;

		g_shutdown_disconnect_pulse = thecore_pulse() + PASSES_PER_SEC(10);
		g_shutdown_disconnect_force_pulse = thecore_pulse() + PASSES_PER_SEC(20);
		g_shutdown_core_pulse = thecore_pulse() + PASSES_PER_SEC(30);
	}
}

namespace
{
	struct SendDisconnectFunc
	{
		void operator () (LPDESC d)
		{
			if (d->GetCharacter())
			{
				if (d->GetCharacter()->GetGMLevel() == GM_PLAYER)
					d->GetCharacter()->ChatPacket(CHAT_TYPE_COMMAND, "quit Shutdown(SendDisconnectFunc)");
			}
		}
	};

	struct DisconnectFunc
	{
		void operator () (LPDESC d)
		{
			if (d->GetType() == DESC_TYPE_CONNECTOR)
				return;

			if (d->IsPhase(PHASE_P2P))
				return;

			d->SetPhase(PHASE_CLOSE);
		}
	};
}
#ifdef WEEKLY_RANK_BYLUZER
bool GetIsWeekend()
{
	time_t ct = get_global_time();
	struct tm tm = *localtime(&ct);
	int dwArray[4] = {0, 23, 59, 59};//wday 0-niedziela,1-poniedzialek itd
	return (tm.tm_wday == dwArray[0] && tm.tm_hour == dwArray[1] && tm.tm_min == dwArray[2]);
//	return (tm.tm_wday == dwArray[0] && tm.tm_hour == dwArray[1] && tm.tm_min == dwArray[2] && (tm.tm_sec == dwArray[3] || tm.tm_sec == 59));
}
bool rozdane = false;
#endif

#ifdef __DUNGEON_INFO__
static bool IsMidnight()
{
	time_t ct = get_global_time();
	struct tm tm = *localtime(&ct);
	return (tm.tm_hour == 0 && tm.tm_min == 0);
}
static bool s_bFastestResetDone = false;
#endif

extern std::map<DWORD, CLoginSim *> g_sim; // first: AID
extern std::map<DWORD, CLoginSim *> g_simByPID;
extern std::vector<TPlayerTable> g_vec_save;
unsigned int save_idx = 0;

void heartbeat(LPHEART ht, int pulse)
{
	DWORD t;

	t = get_dword_time();
	num_events_called += event_process(pulse);
	s_dwProfiler[PROF_EVENT] += (get_dword_time() - t);

	t = get_dword_time();

	if (!(pulse % ht->passes_per_sec))
	{
#ifdef ENABLE_LIMIT_TIME
		if ((unsigned)get_global_time() >= GLOBAL_LIMIT_TIME)
		{
			sys_err("Server life time expired.");
			g_bShutdown = true;
		}
#endif

		if (!g_bAuthServer)
		{
			TPlayerCountPacket pack;
			pack.dwCount = DESC_MANAGER::instance().GetLocalUserCount();
			db_clientdesc->DBPacket(HEADER_GD_PLAYER_COUNT, 0, &pack, sizeof(TPlayerCountPacket));
		}
		else
		{
			DESC_MANAGER::instance().ProcessExpiredLoginKey();
			DBManager::instance().FlushBilling();
			/*
			   if (!(pulse % (ht->passes_per_sec * 600)))
			   DBManager::instance().CheckBilling();
			 */
		}

		{
			int count = 0;
#ifndef __AUTH_QUEUEING__
			itertype(g_sim) it = g_sim.begin();

			while (it != g_sim.end())
			{
				if (!it->second->IsCheck())
				{
					it->second->SendLogin();

					if (++count > 50)
					{
						sys_log(0, "FLUSH_SENT");
						break;
					}
				}

				it++;
			}
#else
			count = CLoginQueue::instance().PassQueue();
			CLoginQueue::instance().CleanUp();
#endif

			if (save_idx < g_vec_save.size())
			{
				count = MIN(100, g_vec_save.size() - save_idx);

				for (int i = 0; i < count; ++i, ++save_idx)
					db_clientdesc->DBPacket(HEADER_GD_PLAYER_SAVE, 0, &g_vec_save[save_idx], sizeof(TPlayerTable));

				sys_log(0, "SAVE_FLUSH %d", count);
			}
		}
	}

	if (!(pulse % (passes_per_sec + 4)))
		CHARACTER_MANAGER::instance().ProcessDelayedSave();

#if defined (__FreeBSD__) && defined(__FILEMONITOR__)
	if (!(pulse % (passes_per_sec * 5)))
	{
		FileMonitorFreeBSD::Instance().Update(pulse);
	}
#endif

	if (!(pulse % (passes_per_sec * 5 + 2)))
	{
		ITEM_MANAGER::instance().Update();
		DESC_MANAGER::instance().UpdateLocalUserCount();
	}

#ifdef ENABLE_AVERAGE_PRICE
	if (!(pulse % (passes_per_sec * g_AveragePriceUpdateTime)))
	{
		offlineshop::CShopManager::Instance().AveragePriceUpdate();
	}
#endif
#ifdef WEEKLY_RANK_BYLUZER
	if (!(pulse % (passes_per_sec * 60)))//co minutke
	{
		if (GetIsWeekend() && mother_port == 31003)//port ch1 tutaj musi byc
		{
			int weekly_season = 0;
			std::unique_ptr<SQLMsg> pkMsg(DBManager::instance().DirectQuery("SELECT season from weekly_season where weekly=1"));
			auto uiNumRows = pkMsg->Get()->uiNumRows;
			if (uiNumRows == 0)
				return;
			MYSQL_ROW row = mysql_fetch_row(pkMsg->Get()->pSQLResult);
			str_to_number(weekly_season, row[0]);
			
			const char * tables [] = {"w_monsters", "w_bosses", "w_metins", "w_dungs", "w_ds", "w_refine", "w_sm"};
			const int items [] = {92056,92055,92054,92059,92058,92057,92062,92061,92060,92065,92064,92063,92074,92073,92072,92077,92076,92075,92080,92079,92078};
			DWORD item = 0;
			// DWORD record_num = 0;
			for (int i = 0; i < 7; i++)
			{
				std::unique_ptr<SQLMsg> pkMsg2(DBManager::instance().DirectQuery("SELECT account_id FROM srv1_player.player WHERE weekly_season=%d ORDER BY %s DESC LIMIT 3", weekly_season, tables[i]));//dopisac where == season==game.get_event_Flag
				auto uiNumRows = pkMsg2->Get()->uiNumRows;
				if (uiNumRows == 0)
					return;
				MYSQL_ROW row2;
				while ((row2 = mysql_fetch_row(pkMsg2->Get()->pSQLResult)))
				{
					TWeeklyRewardsGD p;
					str_to_number(p.account_id, row2[0]);
					p.item_vnum = items[item]; 
					db_clientdesc->DBPacket(HEADER_GD_WEEKLY_REWARDS, 0, &p, sizeof(TWeeklyRewardsGD));
					item++;
				}
				if (item < 3*(i+1))
					item = 3*(i+1);
			}
			DBManager::instance().DirectQuery("UPDATE weekly_season SET season=%d where weekly=1", weekly_season+1);
			SendNotice("Odmeny za tydenni zebricek byly rozdany. Zacina dalsi sezona!");
		}
	}
#endif
#ifdef __DUNGEON_INFO__
	if (!(pulse % (passes_per_sec * 60))) // every minute
	{
		if (IsMidnight())
		{
			if (!s_bFastestResetDone)
			{
				CHARACTER_MANAGER::Instance().ResetFastestRankings();
				s_bFastestResetDone = true;
				sys_log(0, "DungeonFastestRanking: daily midnight reset done.");
			}
		}
		else
		{
			s_bFastestResetDone = false;
		}
	}
#endif
	s_dwProfiler[PROF_HEARTBEAT] += (get_dword_time() - t);

	DBManager::instance().Process();
	AccountDB::instance().Process();
	CPVPManager::instance().Process();

	if (g_bShutdown)
	{
		if (thecore_pulse() > g_shutdown_disconnect_pulse)
		{
			const DESC_MANAGER::DESC_SET & c_set_desc = DESC_MANAGER::instance().GetClientSet();
			std::for_each(c_set_desc.begin(), c_set_desc.end(), ::SendDisconnectFunc());
			g_shutdown_disconnect_pulse = INT_MAX;
		}
		else if (thecore_pulse() > g_shutdown_disconnect_force_pulse)
		{
			const DESC_MANAGER::DESC_SET & c_set_desc = DESC_MANAGER::instance().GetClientSet();
			std::for_each(c_set_desc.begin(), c_set_desc.end(), ::DisconnectFunc());
		}
		else if (thecore_pulse() > g_shutdown_disconnect_force_pulse + PASSES_PER_SEC(5))
		{
			thecore_shutdown();
		}
	}
}

static void CleanUpForEarlyExit() {
	CancelReloadSpamEvent();
}

int main(int argc, char **argv)
{
#ifdef DEBUG_ALLOC
	DebugAllocator::StaticSetUp();
#endif

#ifdef ENABLE_GOOGLE_TEST
#ifndef _WIN32
	// <Factor> start unit tests if option is set
	if ( argc > 1 )
	{
		if ( strcmp( argv[1], "unittest" ) == 0 )
		{
			::testing::InitGoogleTest(&argc, argv);
			return RUN_ALL_TESTS();
		}
	}
#endif
#endif

	ilInit(); // DevIL Initialize


	SECTREE_MANAGER	sectree_manager;
	CHARACTER_MANAGER	char_manager;
	ITEM_MANAGER	item_manager;
	CShopManager	shop_manager;
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	CLocaleItemManager	locale_item_manager;
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	COfflineShopManager offlineshop_manager;
#endif

	CMobManager		mob_manager;
	CMotionManager	motion_manager;
	CPartyManager	party_manager;
	CSkillManager	skill_manager;
	CPVPManager		pvp_manager;
	LZOManager		lzo_manager;
	DBManager		db_manager;
	AccountDB 		account_db;

	LogManager		log_manager;
	MessengerManager	messenger_manager;
	P2P_MANAGER		p2p_manager;
	CGuildManager	guild_manager;
	CGuildMarkManager mark_manager;
	CDungeonManager	dungeon_manager;
	CRefineManager	refine_manager;
	CBanwordManager	banword_manager;
	CPrivManager	priv_manager;
	CWarMapManager	war_map_manager;
	building::CManager	building_manager;
	CTargetManager	target_manager;
	marriage::CManager	marriage_manager;
	marriage::WeddingManager wedding_manager;
	CItemAddonManager	item_addon_manager;
	CArenaManager arena_manager;
	COXEventManager OXEvent_manager;
	CMonarch		Monarch;
	CHorseNameManager horsename_manager;
	CEventFunctionHandler EventFunctionHandler;
#ifdef ENABLE_IN_GAME_LOG_SYSTEM
	InGameLog::InGameLogManager ingamelogManager;
#endif

	DESC_MANAGER	desc_manager;

	CTableBySkill SkillPowerByLevel;
	CPolymorphUtils polymorph_utils;
	CProfiler		profiler;
	COver9RefineManager	o9r;
	SpamManager		spam_mgr;
	CThreeWayWar	threeway_war;
	CDragonLairManager	dl_manager;
	DSManager dsManager;
#ifdef _ENABLE_BATTLEPASS_
	BattlePassManager battlepass;
#endif
#ifdef __PREMIUM_PRIVATE_SHOP__
	CPrivateShopManager private_shop_manager;
#endif
#ifdef ENABLE_PVP_RANKING
	CPvPRanking	pvpRanking;
#endif
#ifdef __DUNGEON_INFO_ENABLE__
	CDungeonInfoManager dungeonInfoManager; 
#endif
#ifdef __ENABLE_POLYMORPH_SYSTEM__
	CPolymorphMgr polymorphMgr;
#endif

#ifdef ENABLE_RESP_SYSTEM
	CRespManager resp_manager;
#endif
#if defined(__DUNGEON_INFO_SYSTEM__)
	CDungeonInfoManager DungeonInfoManager;
#endif

#ifdef ENABLE_SWITCHBOT
	CSwitchbotManager switchbot;
#endif

#ifdef __WORLD_BOSS_YUMA__
	CWorldBossManager WorldBoss_manager;
#endif
#ifdef RANKING_SYSTEM
	CServerRankingManager	server_ranking_manager;
	CRankingManager			ranking_manager;
#endif

	CFishWikiManager fish_wiki_manager;

	cube::CubeManager cubeManager;
	MonsterSpawner g_monster_spawner;

	if (!start(argc, argv)) {
		CleanUpForEarlyExit();
		return 0;
	}

	quest::CQuestManager quest_manager;
#ifdef __TEAM_MEMBER_STATUS__
	CTeamListManager TeamListManager;
#endif
#ifdef ENABLE_OFFLINE_SHOP
	offlineshop::CShopManager offshopManager;
#endif
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
	CTitleAchievementTitle	TitleMng;
#endif
	if (!quest_manager.Initialize()) {
		CleanUpForEarlyExit();
		return 0;
	}

	MessengerManager::instance().Initialize();
	CGuildManager::instance().Initialize();
	fishing::Initialize();
	OXEvent_manager.Initialize();

#ifdef __WORLD_BOSS_YUMA__
	WorldBoss_manager.Initialize();
#endif
#if defined(__DUNGEON_INFO_SYSTEM__)
	if (!g_bAuthServer)
		DungeonInfoManager.Initialize();
#endif

	if (g_bAuthServer)
	{
		client::VersionManager::Create();
	}

    if (!g_bAuthServer) {
        cube::CubeManager::Instance().Initialize(cube::CubeManager::GetCubeFilename());
	}

	Blend_Item_init();
	ani_init();
	PanamaLoad();
#ifdef __ENABLE_COLLECTIONS_SYSTEM__
	CSystemCollections collectonsSystem;
#endif
#ifdef _ENABLE_BATTLEPASS_
	BattlePassManager::Instance().Boot();
#endif

#ifdef __AUTH_QUEUEING__
	if (g_bAuthServer)
		CLoginQueue LoginQueue;
#endif

	// Client PackageCrypt
	const std::string strPackageCryptInfoDir = "package/";
	if( !desc_manager.LoadClientPackageCryptInfo( strPackageCryptInfoDir.c_str() ) )
	{
		sys_err("Failed to Load ClientPackageCryptInfo File(%s)", strPackageCryptInfoDir.c_str());
	}

#if defined (__FreeBSD__) && defined(__FILEMONITOR__)
	PFN_FileChangeListener pPackageNotifyFunc =  &(DESC_MANAGER::NotifyClientPackageFileChanged);
	//FileMonitorFreeBSD::Instance().AddWatch( strPackageCryptInfoName, pPackageNotifyFunc );
#endif
#ifdef ENABLE_REAL_TIME_REGEN
	CRealTimeRegen RealTimeRegen;
#endif
	while (idle());

	sys_log(0, "<shutdown> Starting...");
	g_bShutdown = true;
	g_bNoMoreClient = true;

	if (g_bAuthServer)
	{
		DBManager::instance().FlushBilling(true);

		int iLimit = DBManager::instance().CountQuery() / 50;
		int i = 0;

		do
		{
			DWORD dwCount = DBManager::instance().CountQuery();
			sys_log(0, "Queries %u", dwCount);

			if (dwCount == 0)
				break;

			usleep(500000);

			if (++i >= iLimit)
				if (dwCount == DBManager::instance().CountQuery())
					break;
		} while (1);

#ifdef __AUTH_QUEUEING__
		sys_log(0, "<Shutdown> Destroying LoginQueue...");
		CLoginQueue::instance().CleanUp();
#endif
	}

	sys_log(0, "<shutdown> Destroying CArenaManager...");
	arena_manager.Destroy();
	sys_log(0, "<shutdown> Destroying COXEventManager...");
	OXEvent_manager.Destroy();

#ifdef __WORLD_BOSS_YUMA__
	sys_log(0, "<shutdown> Destroying CWorldBossManager...");
	WorldBoss_manager.Destroy();
#endif

	sys_log(0, "<shutdown> Disabling signal timer...");
	signal_timer_disable();

#ifdef __PREMIUM_PRIVATE_SHOP__
	sys_log(0, "<shutdown> Destroying CPrivateShopManager...");
	private_shop_manager.Destroy();
#endif

	sys_log(0, "<shutdown> Shutting down CHARACTER_MANAGER...");
	char_manager.GracefulShutdown();
	sys_log(0, "<shutdown> Shutting down ITEM_MANAGER...");
	item_manager.GracefulShutdown();

	sys_log(0, "<shutdown> Flushing db_clientdesc...");
	db_clientdesc->FlushOutput();
	sys_log(0, "<shutdown> Flushing p2p_manager...");
	p2p_manager.FlushOutput();

	sys_log(0, "<shutdown> Destroying CShopManager...");
	shop_manager.Destroy();
	sys_log(0, "<shutdown> Destroying CHARACTER_MANAGER...");
	char_manager.Destroy();
	sys_log(0, "<shutdown> Destroying ITEM_MANAGER...");
	item_manager.Destroy();
	sys_log(0, "<shutdown> Destroying DESC_MANAGER...");
	desc_manager.Destroy();
	sys_log(0, "<shutdown> Destroying quest::CQuestManager...");
	quest_manager.Destroy();
	sys_log(0, "<shutdown> Destroying building::CManager...");
#if defined(__DUNGEON_INFO_SYSTEM__)
	if (!g_bAuthServer)
	{
		sys_log(0, "<shutdown> Destroying CDungeonInfoManager...");
		DungeonInfoManager.Destroy();
	}
#endif

#ifdef ENABLE_REAL_TIME_REGEN
	sys_log(0, "<shutdown> Destroying CRealTimeRegen...");
	CRealTimeRegen::Instance().Destroy(); 
#endif
	building_manager.Destroy();
	sys_log(0, "<shutdown> Destroying CEventFunctionHandler...");
	EventFunctionHandler.Destroy();
	destroy();

#ifdef DEBUG_ALLOC
	DebugAllocator::StaticTearDown();
#endif

	return 1;
}

void usage()
{
	printf("Option list\n"
			"-p <port>    : bind port number (port must be over 1024)\n"
			"-l <level>   : sets log level\n"
			"-n <locale>  : sets locale name\n"
#ifdef ENABLE_NEWSTUFF
			"-C <on-off>  : checkpointing check on/off\n"
#endif
			"-v           : log to stdout\n"
			"-r           : do not load regen tables\n"
	);
}

int start(int argc, char **argv)
{
	std::string st_localeServiceName;

	bool bVerbose = false;
	char ch;

	//_malloc_options = "A";
#if defined(__FreeBSD__) && defined(DEBUG_ALLOC)
	_malloc_message = WriteMallocMessage;
#endif
#ifdef ENABLE_LIMIT_TIME
	if ((unsigned)get_global_time() >= GLOBAL_LIMIT_TIME)
	{
		sys_err("Server life time expired.");
		return 0;
	}
#endif

	// LOCALE_SERVICE
	config_init(st_localeServiceName);
	// END_OF_LOCALE_SERVICE

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	offlineshop_config_init();
#endif	
#ifdef _WIN32
	// In Windows dev mode, "verbose" option is [on] by default.
	bVerbose = true;
#endif
	if (!bVerbose)
		freopen("stdout", "a", stdout);

	bool is_thecore_initialized = thecore_init(25, heartbeat);

	if (!is_thecore_initialized)
	{
		fprintf(stderr, "Could not initialize thecore, check owner of pid, syslog\n");
		exit(0);
	}

	if (false == CThreeWayWar::instance().LoadSetting("forkedmapindex.txt"))
	{
		if (false == g_bAuthServer)
		{
			fprintf(stderr, "Could not Load ThreeWayWar Setting file");
			exit(0);
		}
	}

	signal_timer_disable();

	main_fdw = fdwatch_new(4096);

	if ((tcp_socket = socket_tcp_bind(g_szPublicIP, mother_port)) == INVALID_SOCKET)
	{
		perror("socket_tcp_bind: tcp_socket");
		return 0;
	}

	// if internal ip exists, p2p socket uses internal ip, if not use public ip
	//if ((p2p_socket = socket_tcp_bind(*g_szInternalIP ? g_szInternalIP : g_szPublicIP, p2p_port)) == INVALID_SOCKET)
	if ((p2p_socket = socket_tcp_bind(g_szPublicIP, p2p_port)) == INVALID_SOCKET)
	{
		perror("socket_tcp_bind: p2p_socket");
		return 0;
	}

	fdwatch_add_fd(main_fdw, tcp_socket, NULL, FDW_READ, false);
	fdwatch_add_fd(main_fdw, p2p_socket, NULL, FDW_READ, false);

	db_clientdesc = DESC_MANAGER::instance().CreateConnectionDesc(main_fdw, db_addr, db_port, PHASE_DBCLIENT, true);
	if (!g_bAuthServer) {
		db_clientdesc->UpdateChannelStatus(0, true);
	}

	if (g_bAuthServer)
	{
		if (g_stAuthMasterIP.length() != 0)
		{
			fprintf(stderr, "SlaveAuth");
			g_pkAuthMasterDesc = DESC_MANAGER::instance().CreateConnectionDesc(main_fdw, g_stAuthMasterIP.c_str(), g_wAuthMasterPort, PHASE_P2P, true);
			P2P_MANAGER::instance().RegisterConnector(g_pkAuthMasterDesc);
			g_pkAuthMasterDesc->SetP2P(g_stAuthMasterIP.c_str(), g_wAuthMasterPort, g_bChannel);

		}
		else
		{
			fprintf(stderr, "MasterAuth %d\n", LC_GetLocalType());
		}
	}
	/* game server */
	else
	{
		sys_log(0, "SPAM_CONFIG: duration %u score %u reload cycle %u\n",
				g_uiSpamBlockDuration, g_uiSpamBlockScore, g_uiSpamReloadCycle);

		extern void LoadSpamDB();
		LoadSpamDB();
	}

	signal_timer_enable(30);
	return 1;
}

void destroy()
{
	sys_log(0, "<shutdown> Canceling ReloadSpamEvent...");
	CancelReloadSpamEvent();

	sys_log(0, "<shutdown> regen_free()...");
	regen_free();

	sys_log(0, "<shutdown> Closing sockets...");
	socket_close(tcp_socket);

	socket_close(p2p_socket);

	sys_log(0, "<shutdown> fdwatch_delete()...");
	fdwatch_delete(main_fdw);

	sys_log(0, "<shutdown> event_destroy()...");
	event_destroy();

	sys_log(0, "<shutdown> CTextFileLoader::DestroySystem()...");
	CTextFileLoader::DestroySystem();

	sys_log(0, "<shutdown> thecore_destroy()...");
	thecore_destroy();
}

int idle()
{
	static struct timeval	pta = { 0, 0 };
	static int			process_time_count = 0;
	struct timeval		now;

	if (pta.tv_sec == 0)
		gettimeofday(&pta, (struct timezone *) 0);

	int passed_pulses;

	if (!(passed_pulses = thecore_idle()))
		return 0;

	assert(passed_pulses > 0);

	DWORD t;

	while (passed_pulses--) {
		heartbeat(thecore_heart, ++thecore_heart->pulse);

		// To reduce the possibility of abort() in checkpointing
		thecore_tick();
	}

	t = get_dword_time();
	CHARACTER_MANAGER::instance().Update(thecore_heart->pulse);
	db_clientdesc->Update(t);
	s_dwProfiler[PROF_CHR_UPDATE] += (get_dword_time() - t);

#ifdef __WORLD_BOSS_YUMA__
	CWorldBossManager::instance().Idle(thecore_heart->pulse);
#endif
	MonsterSpawner::instance().Process();

	t = get_dword_time();
	if (!io_loop(main_fdw)) return 0;
	s_dwProfiler[PROF_IO] += (get_dword_time() - t);

	log_rotate();

	gettimeofday(&now, (struct timezone *) 0);
	++process_time_count;

	if (now.tv_sec - pta.tv_sec > 0)
	{
		pt_log("[%3d] event %5d/%-5d idle %-4ld event %-4ld heartbeat %-4ld I/O %-4ld chrUpate %-4ld | WRITE: %-7d | PULSE: %d",
				process_time_count,
				num_events_called,
				event_count(),
				thecore_profiler[PF_IDLE],
				s_dwProfiler[PROF_EVENT],
				s_dwProfiler[PROF_HEARTBEAT],
				s_dwProfiler[PROF_IO],
				s_dwProfiler[PROF_CHR_UPDATE],
				current_bytes_written,
				thecore_pulse());

		num_events_called = 0;
		current_bytes_written = 0;

		process_time_count = 0;
		gettimeofday(&pta, (struct timezone *) 0);

		memset(&thecore_profiler[0], 0, sizeof(thecore_profiler));
		memset(&s_dwProfiler[0], 0, sizeof(s_dwProfiler));
	}

#ifdef _WIN32
	if (_kbhit()) {
		int c = _getch();
		switch (c) {
			case 0x1b: // Esc
				return 0; // shutdown
				break;
			default:
				break;
		}
	}
#endif
#ifdef ENABLE_REAL_TIME_REGEN
	CRealTimeRegen::Instance().Process(); 
#endif
	CEventFunctionHandler::instance().Process();

	return 1;
}

int io_loop(LPFDWATCH fdw)
{
	LPDESC	d;
	int		num_events, event_idx;

	DESC_MANAGER::instance().DestroyClosed();
	DESC_MANAGER::instance().TryConnect();

	if ((num_events = fdwatch(fdw, 0)) < 0)
		return 0;

	for (event_idx = 0; event_idx < num_events; ++event_idx)
	{
		d = (LPDESC) fdwatch_get_client_data(fdw, event_idx);

		if (!d)
		{
			if (FDW_READ == fdwatch_check_event(fdw, tcp_socket, event_idx))
			{
				DESC_MANAGER::instance().AcceptDesc(fdw, tcp_socket);
				fdwatch_clear_event(fdw, tcp_socket, event_idx);
			}
			else if (FDW_READ == fdwatch_check_event(fdw, p2p_socket, event_idx))
			{
				DESC_MANAGER::instance().AcceptP2PDesc(fdw, p2p_socket);
				fdwatch_clear_event(fdw, p2p_socket, event_idx);
			}
			continue;
		}

		int iRet = fdwatch_check_event(fdw, d->GetSocket(), event_idx);

		switch (iRet)
		{
			case FDW_READ:
				if (db_clientdesc == d)
				{
					int size = d->ProcessInput();

					if (size)
						sys_log(1, "DB_BYTES_READ: %d", size);

					if (size < 0)
					{
						d->SetPhase(PHASE_CLOSE);
					}
				}
				else if (d->ProcessInput() < 0)
				{
					d->SetPhase(PHASE_CLOSE);
				}
				break;

			case FDW_WRITE:
				if (db_clientdesc == d)
				{
					int buf_size = d->GetOutputBufferSize();
					int sock_buf_size = fdwatch_get_buffer_size(fdw, d->GetSocket());

					int ret = d->ProcessOutput();

					if (ret < 0)
					{
						d->SetPhase(PHASE_CLOSE);
					}

					if (buf_size)
						sys_log(1, "DB_BYTES_WRITE: size %d sock_buf %d ret %d", buf_size, sock_buf_size, ret);
				}
				else if (d->ProcessOutput() < 0)
				{
					d->SetPhase(PHASE_CLOSE);
				}
				break;

			case FDW_EOF:
				{
					d->SetPhase(PHASE_CLOSE);
				}
				break;

			default:
				sys_err("fdwatch_check_event returned unknown %d", iRet);
				d->SetPhase(PHASE_CLOSE);
				break;
		}
	}

	return 1;
}
//martysama0134's ceqyqttoaf71vasf9t71218
