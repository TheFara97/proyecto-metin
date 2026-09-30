#include "stdafx.h"
#include "constants.h"
#include "config.h"
#include "utils.h"
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
#include "locale_item_manager.h"
#endif
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "packet.h"
#include "protocol.h"
#include "mob_manager.h"
#include "shop_manager.h"
#include "sectree_manager.h"
#include "skill.h"
#include "questmanager.h"
#include "p2p.h"
#include "guild.h"
#include "guild_manager.h"
#include "start_position.h"
#include "party.h"
#include "refine.h"
#include "banword.h"
#include "priv_manager.h"
#include "db.h"
#include "building.h"
#include "login_sim.h"
#include "wedding.h"
#include "login_data.h"
#include "unique_item.h"
#ifdef ENABLE_EXTENDED_BATTLE_PASS
#include "battlepass_manager.h"
#endif

#ifdef ENABLE_OFFLINE_SHOP
#include "new_offlineshop.h"
#include "new_offlineshop_manager.h"
#endif

#ifdef ENABLE_IN_GAME_LOG_SYSTEM
#include "InGameLogManager.h"
#endif
#include "monarch.h"
#include "affect.h"
#include "castle.h"
#include "motion.h"

#include "log.h"

#include "horsename_manager.h"
#include "gm.h"
#include "panama.h"
#include "map_location.h"
#include "DragonSoul.h"

#ifdef ENABLE_REAL_TIME_REGEN
	#include "RealTimeRegen.hpp"
#endif
#include "shutdown_manager.h"
#include "../../common/CommonDefines.h"

#define MAPNAME_DEFAULT	"none"

#ifdef __PREMIUM_PRIVATE_SHOP__
#include "private_shop_manager.h"
#include "private_shop.h"
#include "private_shop_util.h"
#endif
#ifdef __ENABLE_POLYMORPH_SYSTEM__
	#include "PolySystem.h"
#endif
#ifdef __DUNGEON_INFO_ENABLE__
	#include "DungeonInfoManager.hpp"
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	#include "offlineshop_config.h"
	#include "offlineshop_manager.h"
extern bool g_bEnableRespawnOfflineShop;
#endif
#ifdef RANKING_SYSTEM
#include "server_ranking_manager.h"
#include "ranking_manager.h"
#endif
#include "fish_wiki.h"
#include "MonsterSpawner.hpp"

bool GetServerLocation(TAccountTable & rTab, BYTE bEmpire)
{
	bool bFound = false;

	for (int i = 0; i < PLAYER_PER_ACCOUNT; ++i)
	{
		if (0 == rTab.players[i].dwID)
			continue;

		bFound = true;
		int32_t lIndex = 0;

		if (!CMapLocation::instance().Get(rTab.players[i].x,
					rTab.players[i].y,
					lIndex,
					rTab.players[i].lAddr,
					rTab.players[i].wPort))
		{
			sys_err("location error name %s mapindex %d %d x %d empire %d",
					rTab.players[i].szName, lIndex, rTab.players[i].x, rTab.players[i].y, rTab.bEmpire);

			rTab.players[i].x = EMPIRE_START_X(rTab.bEmpire);
			rTab.players[i].y = EMPIRE_START_Y(rTab.bEmpire);

			lIndex = 0;

			if (!CMapLocation::instance().Get(rTab.players[i].x, rTab.players[i].y, lIndex, rTab.players[i].lAddr, rTab.players[i].wPort))
			{
				sys_err("cannot find server for mapindex %d %d x %d (name %s)",
						lIndex,
						rTab.players[i].x,
						rTab.players[i].y,
						rTab.players[i].szName);
#ifdef ENABLE_NEWSTUFF
				if (!g_stProxyIP.empty())
					rTab.players[i].lAddr=inet_addr(g_stProxyIP.c_str());
#endif
				continue;
			}
		}
#ifdef ENABLE_NEWSTUFF
		if (!g_stProxyIP.empty())
			rTab.players[i].lAddr=inet_addr(g_stProxyIP.c_str());
#endif
		struct in_addr in;
		in.s_addr = rTab.players[i].lAddr;
		sys_log(0, "success to %s:%d", inet_ntoa(in), rTab.players[i].wPort);
	}

	return bFound;
}

extern std::map<DWORD, CLoginSim *> g_sim;
extern std::map<DWORD, CLoginSim *> g_simByPID;

void CInputDB::LoginSuccess(DWORD dwHandle, const char *data)
{
	sys_log(0, "LoginSuccess");

	TAccountTable * pTab = (TAccountTable *) data;

	itertype(g_sim) it = g_sim.find(pTab->id);
	if (g_sim.end() != it)
	{
		sys_log(0, "CInputDB::LoginSuccess - already exist sim [%s]", pTab->login);
		it->second->SendLoad();
		return;
	}

	LPDESC d = DESC_MANAGER::instance().FindByHandle(dwHandle);

	if (!d)
	{
		sys_log(0, "CInputDB::LoginSuccess - cannot find handle [%s]", pTab->login);

		TLogoutPacket pack;

		strlcpy(pack.login, pTab->login, sizeof(pack.login));
		db_clientdesc->DBPacket(HEADER_GD_LOGOUT, dwHandle, &pack, sizeof(pack));
		return;
	}

	if (strcmp(pTab->status, "OK"))
	{
		sys_log(0, "CInputDB::LoginSuccess - status[%s] is not OK [%s]", pTab->status, pTab->login);

		TLogoutPacket pack;

		strlcpy(pack.login, pTab->login, sizeof(pack.login));
		db_clientdesc->DBPacket(HEADER_GD_LOGOUT, dwHandle, &pack, sizeof(pack));

		LoginFailure(d, pTab->status);
		return;
	}

	for (int i = 0; i != PLAYER_PER_ACCOUNT; ++i)
	{
		TSimplePlayer& player = pTab->players[i];
		sys_log(0, "\tplayer(%s).job(%d)", player.szName, player.byJob);
	}

	bool bFound = GetServerLocation(*pTab, pTab->bEmpire);

	d->BindAccountTable(pTab);

	if (!bFound)
	{
		TPacketGCEmpire pe;
		pe.bHeader = HEADER_GC_EMPIRE;
		pe.bEmpire = number(1, 3);
		d->Packet(&pe, sizeof(pe));
	}
	else
	{
		TPacketGCEmpire pe;
		pe.bHeader = HEADER_GC_EMPIRE;
		pe.bEmpire = d->GetEmpire();
		d->Packet(&pe, sizeof(pe));
	}

	d->SetPhase(PHASE_SELECT);
	d->SendLoginSuccessPacket();

	// __SHUTDOWN::Shutdown Register
	CShutdownManager::Instance().AddDesc(d);

	sys_log(0, "InputDB::login_success: %s", pTab->login);
}

void CInputDB::PlayerCreateFailure(LPDESC d, BYTE bType)
{
	if (!d)
		return;

	TPacketGCCreateFailure pack;

	pack.header	= HEADER_GC_CHARACTER_CREATE_FAILURE;
	pack.bType	= bType;

	d->Packet(&pack, sizeof(pack));
}

void CInputDB::PlayerCreateSuccess(LPDESC d, const char * data)
{
	if (!d)
		return;

	TPacketDGCreateSuccess * pPacketDB = (TPacketDGCreateSuccess *) data;

	if (pPacketDB->bAccountCharacterIndex >= PLAYER_PER_ACCOUNT)
	{
		d->Packet(encode_byte(HEADER_GC_CHARACTER_CREATE_FAILURE), 1);
		return;
	}

	int32_t lIndex = 0;

	if (!CMapLocation::instance().Get(pPacketDB->player.x,
				pPacketDB->player.y,
				lIndex,
				pPacketDB->player.lAddr,
				pPacketDB->player.wPort))
	{
		sys_err("InputDB::PlayerCreateSuccess: cannot find server for mapindex %d %d x %d (name %s)",
				lIndex,
				pPacketDB->player.x,
				pPacketDB->player.y,
				pPacketDB->player.szName);
	}

	TAccountTable & r_Tab = d->GetAccountTable();
	r_Tab.players[pPacketDB->bAccountCharacterIndex] = pPacketDB->player;

	TPacketGCPlayerCreateSuccess pack;

	pack.header = HEADER_GC_CHARACTER_CREATE_SUCCESS;
	pack.bAccountCharacterIndex = pPacketDB->bAccountCharacterIndex;
	pack.player = pPacketDB->player;
#ifdef ENABLE_NEWSTUFF
	if (!g_stProxyIP.empty())
		pack.player.lAddr=inet_addr(g_stProxyIP.c_str());
#endif
	d->Packet(&pack, sizeof(TPacketGCPlayerCreateSuccess));

	TPlayerItem t;
	memset(&t, 0, sizeof(t));

	if (china_event_server)
	{
		t.window	= INVENTORY;
		t.count	= 1;
		t.owner	= r_Tab.players[pPacketDB->bAccountCharacterIndex].dwID;

		struct SInitialItem
		{
			DWORD dwVnum;
			BYTE pos;
		};

		const int MAX_INITIAL_ITEM = 9;

		static SInitialItem initialItems[JOB_MAX_NUM][MAX_INITIAL_ITEM] =
		{
			{ {11243,	2}, {12223,	3}, {15103,	4}, {   93,	1}, {16143,	8}, {17103,	9}, { 3083,	0}, {13193,	11}, {14103, 12}, },
			{ {11443,	0}, {12363,	3}, {15103,	4}, { 1053,	2}, { 2083,	1}, {16083,	7}, {17083,	8}, {13193,	9}, {14103,	10}, },
			{ {11643,	0}, {12503,	2}, {15103,	3}, {   93,	1}, {16123,	4}, {17143,	7}, {13193,	8}, {14103,	9}, {    0,	0}, },
			{ {11843,	0}, {12643,	1}, {15103,	2}, { 7083,	3}, { 5053,	4}, {16123,	6}, {17143,	7}, {13193,	8}, {14103,	9}, },
#ifdef ENABLE_WOLFMAN_CHARACTER
			{ {21023,	2}, {12223,	3}, {21513,	4}, { 6023,	1}, {16143,	8}, {17103,	9}, { 0,	0}, {13193,	11}, {14103, 12}, },
#endif
		};

		int job = pPacketDB->player.byJob;
		for (int i=0; i < MAX_INITIAL_ITEM; i++)
		{
			if (initialItems[job][i].dwVnum == 0)
				continue;

			t.id	= ITEM_MANAGER::instance().GetNewID();
			t.pos	= initialItems[job][i].pos;
			t.vnum	= initialItems[job][i].dwVnum;

			db_clientdesc->DBPacketHeader(HEADER_GD_ITEM_SAVE, 0, sizeof(TPlayerItem));
			db_clientdesc->Packet(&t, sizeof(TPlayerItem));
		}
	}

	LogManager::instance().CharLog(pack.player.dwID, 0, 0, 0, "CREATE PLAYER", "", d->GetHostName());
}

void CInputDB::PlayerDeleteSuccess(LPDESC d, const char * data)
{
	if (!d)
		return;

	BYTE account_index;
	account_index = decode_byte(data);
	d->BufferedPacket(encode_byte(HEADER_GC_CHARACTER_DELETE_SUCCESS),	1);
	d->Packet(encode_byte(account_index),			1);

	d->GetAccountTable().players[account_index].dwID = 0;
}

void CInputDB::PlayerDeleteFail(LPDESC d)
{
	if (!d)
		return;

	d->Packet(encode_byte(HEADER_GC_CHARACTER_DELETE_WRONG_SOCIAL_ID),	1);
	//d->Packet(encode_byte(account_index),			1);

	//d->GetAccountTable().players[account_index].dwID = 0;
}

void CInputDB::ChangeName(LPDESC d, const char * data)
{
	if (!d)
		return;

	TPacketDGChangeName * p = (TPacketDGChangeName *) data;

	TAccountTable & r = d->GetAccountTable();

	if (!r.id)
		return;

	for (size_t i = 0; i < PLAYER_PER_ACCOUNT; ++i)
		if (r.players[i].dwID == p->pid)
		{
			strlcpy(r.players[i].szName, p->name, sizeof(r.players[i].szName));
			r.players[i].bChangeName = 0;

			TPacketGCChangeName pgc;

			pgc.header = HEADER_GC_CHANGE_NAME;
			pgc.pid = p->pid;
			strlcpy(pgc.name, p->name, sizeof(pgc.name));

			d->Packet(&pgc, sizeof(TPacketGCChangeName));
			break;
		}
}

#define ENABLE_GOHOME_IF_MAP_NOT_EXIST
void CInputDB::PlayerLoad(LPDESC d, const char * data)
{
	TPlayerTable * pTab = (TPlayerTable *) data;

	if (!d)
		return;
 
	if (!d->IsPhase(PHASE_SELECT))
	{
		sys_err("SECURITY: PlayerLoad called in wrong phase (not PHASE_SELECT)");
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	const TAccountTable & account = d->GetAccountTable();
	
	if (!account.id)
	{
		sys_err("SECURITY: PlayerLoad with invalid account table");
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	bool validCharacter = false;
	for (int i = 0; i < PLAYER_PER_ACCOUNT; ++i)
	{
		if (account.players[i].dwID == pTab->id)
		{
			validCharacter = true;
			break;
		}
	}

	if (!validCharacter)
	{
		sys_err("SECURITY: Attempt to load character not belonging to account. PID: %u, Account: %s", 
				pTab->id, account.login);
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	int32_t lMapIndex = pTab->lMapIndex;
	PIXEL_POSITION pos;

	if (lMapIndex == 0)
	{
		lMapIndex = SECTREE_MANAGER::instance().GetMapIndex(pTab->x, pTab->y);

		if (lMapIndex == 0)
		{
			lMapIndex = EMPIRE_START_MAP(d->GetAccountTable().bEmpire);
			pos.x = EMPIRE_START_X(d->GetAccountTable().bEmpire);
			pos.y = EMPIRE_START_Y(d->GetAccountTable().bEmpire);
		}
		else
		{
			pos.x = pTab->x;
			pos.y = pTab->y;
		}
	}
	pTab->lMapIndex = lMapIndex;

	// ----

	// by rtsummit
	if (!SECTREE_MANAGER::instance().GetValidLocation(pTab->lMapIndex, pTab->x, pTab->y, lMapIndex, pos, d->GetEmpire()))
	{
		sys_err("InputDB::PlayerLoad : cannot find valid location %d x %d (name: %s)", pTab->x, pTab->y, pTab->name);
#ifdef ENABLE_GOHOME_IF_MAP_NOT_EXIST
		lMapIndex = EMPIRE_START_MAP(d->GetAccountTable().bEmpire);
		pos.x = EMPIRE_START_X(d->GetAccountTable().bEmpire);
		pos.y = EMPIRE_START_Y(d->GetAccountTable().bEmpire);
#else
		d->SetPhase(PHASE_CLOSE);
		return;
#endif
	}

	pTab->x = pos.x;
	pTab->y = pos.y;
	pTab->lMapIndex = lMapIndex;

	if (d->GetCharacter() || d->IsPhase(PHASE_GAME))
	{
		LPCHARACTER p = d->GetCharacter();
		sys_err("SECURITY: login state already has main state (character %s %p)", 
				p ? p->GetName() : "NULL", get_pointer(p));
 		d->SetPhase(PHASE_CLOSE);
 		return;
	}

	if (NULL != CHARACTER_MANAGER::Instance().FindPC(pTab->name))
	{
		sys_err("SECURITY: PlayerLoad - character already exists in world: %s", pTab->name);
		d->SetPhase(PHASE_CLOSE);
		return;
	}
	if (NULL != CHARACTER_MANAGER::Instance().FindByPID(pTab->id))
	{
		sys_err("SECURITY: PlayerLoad - character with PID already loaded: %u", pTab->id);
 		d->SetPhase(PHASE_CLOSE);
 		return;
 	}

	LPCHARACTER ch = CHARACTER_MANAGER::instance().CreateCharacter(pTab->name, pTab->id);

	ch->BindDesc(d);
	ch->SetPlayerProto(pTab);
	ch->SetEmpire(d->GetEmpire());

	d->BindCharacter(ch);

	{
		// P2P Login
		TPacketGGLogin p;

		p.bHeader = HEADER_GG_LOGIN;
		strlcpy(p.szName, ch->GetName(), sizeof(p.szName));

		p.dwPID = ch->GetPlayerID();
		p.bEmpire = ch->GetEmpire();
		p.lMapIndex = SECTREE_MANAGER::instance().GetMapIndex(ch->GetX(), ch->GetY());
		p.bChannel = g_bChannel;

		P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGLogin));

		char buf[55];
#ifdef ENABLE_LONG_LONG
	// #ifdef ENABLE_CHEQUE_SYSTEM
		snprintf(buf, sizeof(buf), "%s %lld %lld %d %d %lld",
	// #else
	// snprintf(buf, sizeof(buf), "%s %lld %d %d %lld",
	// #endif
#else
		snprintf(buf, sizeof(buf), "%s %d %d %d %lld",
#endif
	// #ifdef ENABLE_CHEQUE_SYSTEM
			inet_ntoa(ch->GetDesc()->GetAddr().sin_addr), ch->GetGold(), ch->GetCheque(), g_bChannel, ch->GetMapIndex(), ch->GetAlignment());
	// #else
			// inet_ntoa(ch->GetDesc()->GetAddr().sin_addr), ch->GetGold(), g_bChannel, ch->GetMapIndex(), ch->GetAlignment());
	// #endif
		LogManager::instance().CharLog(ch, 0, "LOGIN", buf);
	}

	d->SetPhase(PHASE_LOADING);
	ch->MainCharacterPacket();

	int32_t lPublicMapIndex = lMapIndex >= 10000 ? lMapIndex / 10000 : lMapIndex;

	//Send Supplementary Data Block if new map requires security packages in loading this map
	const TMapRegion * rMapRgn = SECTREE_MANAGER::instance().GetMapRegion(lPublicMapIndex);
	if( rMapRgn )
	{
		DESC_MANAGER::instance().SendClientPackageSDBToLoadMap( d, rMapRgn->strMapName.c_str() );
	}
	//if (!map_allow_find(lMapIndex >= 10000 ? lMapIndex / 10000 : lMapIndex) || !CheckEmpire(ch, lMapIndex))
	if (!map_allow_find(lPublicMapIndex))
	{
		sys_err("InputDB::PlayerLoad : entering %d map is not allowed here (name: %s, empire %u)",
				lMapIndex, pTab->name, d->GetEmpire());

		ch->SetWarpLocation(EMPIRE_START_MAP(d->GetEmpire()),
				EMPIRE_START_X(d->GetEmpire()) / 100,
				EMPIRE_START_Y(d->GetEmpire()) / 100);

		d->SetPhase(PHASE_CLOSE);
		return;
	}

	quest::CQuestManager::instance().BroadcastEventFlagOnLogin(ch);

	for (int i = 0; i < QUICKSLOT_MAX_NUM; ++i)
		ch->SetQuickslot(i, pTab->quickslot[i]);

	ch->PointsPacket();
	ch->SkillLevelPacket();

	sys_log(0, "InputDB: player_load %s %dx%dx%d LEVEL %d MOV_SPEED %d JOB %d ATG %d DFG %d GMLv %d",
			pTab->name,
			ch->GetX(), ch->GetY(), ch->GetZ(),
			ch->GetLevel(),
			ch->GetPoint(POINT_MOV_SPEED),
			ch->GetJob(),
			ch->GetPoint(POINT_ATT_GRADE),
			ch->GetPoint(POINT_DEF_GRADE),
			ch->GetGMLevel());

	ch->QuerySafeboxSize();
}

#ifdef __ENABLE_POLYMORPH_SYSTEM__
const char* LoadPolymorphData(const char* data)
{
	if (decode_2bytes(data) != sizeof(TPolymorphStage))
	{
		sys_err("TBasicItem table size error");
		thecore_shutdown();
		return data;
	}
	data += sizeof(uint16_t);

	uint16_t size = decode_2bytes(data);
	data += sizeof(uint16_t);
	sys_log(0, "InputDB.cpp - Size of a TBasicItem %d", size);


	CPolymorphMgr::instance().LoadPolyStage(reinterpret_cast<TPolymorphStage*>(const_cast<char*>(data)), size);
	data += size * sizeof(TPolymorphStage);

	// Basic Skills
	if (decode_2bytes(data) != sizeof(TPolymorphSkin))
	{
		sys_err("TPolymorphSkin table size error");
		thecore_shutdown();
		return data;
	}
	data += sizeof(uint16_t);

	size = decode_2bytes(data);
	data += sizeof(uint16_t);
	sys_log(0, "InputDB.cpp - Size of a TPolymorphSkin %d", size);


	CPolymorphMgr::instance().LoadPolySkins(reinterpret_cast<TPolymorphSkin*>(const_cast<char*>(data)), size);
	return data + sizeof(TPolymorphSkin) * size;
}
#endif

void CInputDB::Boot(const char* data)
{
	signal_timer_disable();

	DWORD dwPacketSize = decode_4bytes(data);
	data += 4;

	BYTE bVersion = decode_byte(data);
	data += 1;

	sys_log(0, "BOOT: PACKET: %d", dwPacketSize);
	sys_log(0, "BOOT: VERSION: %d", bVersion);
	if (bVersion != 6)
	{
		sys_err("boot version error");
		thecore_shutdown();
	}

	sys_log(0, "sizeof(TMobTable) = %d", sizeof(TMobTable));
	sys_log(0, "sizeof(TItemTable) = %d", sizeof(TItemTable));
	sys_log(0, "sizeof(TShopTable) = %d", sizeof(TShopTable));
	sys_log(0, "sizeof(TSkillTable) = %d", sizeof(TSkillTable));
	sys_log(0, "sizeof(TRefineTable) = %d", sizeof(TRefineTable));
	sys_log(0, "sizeof(TItemAttrTable) = %d", sizeof(TItemAttrTable));
	sys_log(0, "sizeof(TItemRareTable) = %d", sizeof(TItemAttrTable));
#ifdef ENABLE_BELT_ATTRIBUTES
	sys_log(0, "sizeof(TBeltAttrTable) = %d", sizeof(TItemAttrTable));
#endif
#ifdef ENABLE_GLOVE_ATTRIBUTES
	sys_log(0, "sizeof(TGloveAttrTable) = %d", sizeof(TItemAttrTable));
#endif
#ifdef ENABLE_TALISMAN_ATTRIBUTES
	sys_log(0, "sizeof(TTalismanAttrTable) = %d", sizeof(TItemAttrTable));
#endif
#ifdef ENABLE_TALISMAN_ATTRIBUTES_RARE
	sys_log(0, "sizeof(TTalismanAttrRareTable) = %d", sizeof(TItemAttrTable));
#endif
	sys_log(0, "sizeof(TBanwordTable) = %d", sizeof(TBanwordTable));
	sys_log(0, "sizeof(TLand) = %d", sizeof(building::TLand));
	sys_log(0, "sizeof(TObjectProto) = %d", sizeof(building::TObjectProto));
	sys_log(0, "sizeof(TObject) = %d", sizeof(building::TObject));
	//ADMIN_MANAGER
	sys_log(0, "sizeof(TAdminManager) = %d", sizeof (TAdminInfo) );
	//END_ADMIN_MANAGER

	WORD size;

	/*
	 * MOB
	 */

	if (decode_2bytes(data)!=sizeof(TMobTable))
	{
		sys_err("mob table size error");
		thecore_shutdown();
		return;
	}
	data += 2;

	size = decode_2bytes(data);
	data += 2;
	sys_log(0, "BOOT: MOB: %d", size);

	if (size)
	{
		CMobManager::instance().Initialize((TMobTable *) data, size);
		data += size * sizeof(TMobTable);
	}

	/*
	 * ITEM
	 */

	if (decode_2bytes(data) != sizeof(TItemTable))
	{
		sys_err("item table size error");
		thecore_shutdown();
		return;
	}
	data += 2;

	size = decode_2bytes(data);
	data += 2;
	sys_log(0, "BOOT: ITEM: %d", size);

	if (size)
	{
		ITEM_MANAGER::instance().Initialize((TItemTable *) data, size);
		data += size * sizeof(TItemTable);
	}

	/*
	 * SHOP
	 */

	if (decode_2bytes(data) != sizeof(TShopTable))
	{
		sys_err("shop table size error");
		thecore_shutdown();
		return;
	}
	data += 2;

	size = decode_2bytes(data);
	data += 2;
	sys_log(0, "BOOT: SHOP: %d", size);

	if (size)
	{
		if (!CShopManager::instance().Initialize((TShopTable *) data, size))
		{
			sys_err("shop table Initialize error");
			thecore_shutdown();
			return;
		}
		data += size * sizeof(TShopTable);
	}

	/*
	 * SKILL
	 */

	if (decode_2bytes(data) != sizeof(TSkillTable))
	{
		sys_err("skill table size error");
		thecore_shutdown();
		return;
	}
	data += 2;

	size = decode_2bytes(data);
	data += 2;
	sys_log(0, "BOOT: SKILL: %d", size);

	if (size)
	{
		if (!CSkillManager::instance().Initialize((TSkillTable *) data, size))
		{
			sys_err("cannot initialize skill table");
			thecore_shutdown();
			return;
		}

		data += size * sizeof(TSkillTable);
	}
	/*
	 * REFINE RECIPE
	 */
	if (decode_2bytes(data) != sizeof(TRefineTable))
	{
		sys_err("refine table size error");
		thecore_shutdown();
		return;
	}
	data += 2;

	size = decode_2bytes(data);
	data += 2;
	sys_log(0, "BOOT: REFINE: %d", size);

	if (size)
	{
		CRefineManager::instance().Initialize((TRefineTable*) data, size);
		data += size * sizeof(TRefineTable);
	}

	/*
	 * ITEM ATTR
	 */
	if (decode_2bytes(data) != sizeof(TItemAttrTable))
	{
		sys_err("item attr table size error");
		thecore_shutdown();
		return;
	}
	data += 2;

	size = decode_2bytes(data);
	data += 2;
	sys_log(0, "BOOT: ITEM_ATTR: %d", size);

	if (size)
	{
		TItemAttrTable * p = (TItemAttrTable *) data;

		for (int i = 0; i < size; ++i, ++p)
		{
			if (p->dwApplyIndex >= MAX_APPLY_NUM)
				continue;

			g_map_itemAttr[p->dwApplyIndex] = *p;
			sys_log(0, "ITEM_ATTR[%d]: %s %u", p->dwApplyIndex, p->szApply, p->dwProb);
		}
	}

	data += size * sizeof(TItemAttrTable);

	/*
     * ITEM RARE
     */
	if (decode_2bytes(data) != sizeof(TItemAttrTable))
	{
		sys_err("item rare table size error");
		thecore_shutdown();
		return;
	}
	data += 2;

	size = decode_2bytes(data);
	data += 2;
	sys_log(0, "BOOT: ITEM_RARE: %d", size);

	if (size)
	{
		TItemAttrTable * p = (TItemAttrTable *) data;

		for (int i = 0; i < size; ++i, ++p)
		{
			if (p->dwApplyIndex >= MAX_APPLY_NUM)
				continue;

			g_map_itemRare[p->dwApplyIndex] = *p;
			sys_log(0, "ITEM_RARE[%d]: %s %u", p->dwApplyIndex, p->szApply, p->dwProb);
		}
	}

	data += size * sizeof(TItemAttrTable);

#ifdef ENABLE_BELT_ATTRIBUTES
	/*
	 * BELT ATTR
	 */
	if (decode_2bytes(data) != sizeof(TItemAttrTable))
	{
		sys_err("belt attr table size error");
		thecore_shutdown();
		return;
	}
	data += 2;

	size = decode_2bytes(data);
	data += 2;
	sys_log(0, "BOOT: BELT_ATTR: %d", size);

	if (size)
	{
		TItemAttrTable * p = (TItemAttrTable *) data;

		for (int i = 0; i < size; ++i, ++p)
		{
			if (p->dwApplyIndex >= MAX_APPLY_NUM)
				continue;

			g_map_beltAttr[p->dwApplyIndex] = *p;
			sys_log(0, "BELT_ATTR[%d]: %s %u", p->dwApplyIndex, p->szApply, p->dwProb);
		}
	}

	data += size * sizeof(TItemAttrTable);
#endif

#ifdef ENABLE_GLOVE_ATTRIBUTES
	/*
	 * GLOVE ATTR
	 */
	if (decode_2bytes(data) != sizeof(TItemAttrTable))
	{
		sys_err("glove attr table size error");
		thecore_shutdown();
		return;
	}
	data += 2;

	size = decode_2bytes(data);
	data += 2;
	sys_log(0, "BOOT: GLOVE_ATTR: %d", size);

	if (size)
	{
		TItemAttrTable * p = (TItemAttrTable *) data;

		for (int i = 0; i < size; ++i, ++p)
		{
			if (p->dwApplyIndex >= MAX_APPLY_NUM)
				continue;

			g_map_gloveAttr[p->dwApplyIndex] = *p;
			sys_log(0, "GLOVE_ATTR[%d]: %s %u", p->dwApplyIndex, p->szApply, p->dwProb);
		}
	}

	data += size * sizeof(TItemAttrTable);
#endif

#ifdef ENABLE_TALISMAN_ATTRIBUTES
	/*
	 * GLOVE ATTR
	 */
	if (decode_2bytes(data) != sizeof(TItemAttrTable))
	{
		sys_err("talisman attr table size error");
		thecore_shutdown();
		return;
	}
	data += 2;

	size = decode_2bytes(data);
	data += 2;
	sys_log(0, "BOOT: TALISMAN_ATTR: %d", size);

	if (size)
	{
		TItemAttrTable * p = (TItemAttrTable *) data;

		for (int i = 0; i < size; ++i, ++p)
		{
			if (p->dwApplyIndex >= MAX_APPLY_NUM)
				continue;

			g_map_talismanAttr[p->dwApplyIndex] = *p;
			sys_log(0, "TALISMAN_ATTR[%d]: %s %u", p->dwApplyIndex, p->szApply, p->dwProb);
		}
	}

	data += size * sizeof(TItemAttrTable);
#endif

#ifdef ENABLE_TALISMAN_ATTRIBUTES_RARE
	/*
	 * GLOVE ATTR
	 */
	if (decode_2bytes(data) != sizeof(TItemAttrTable))
	{
		sys_err("talisman attr rare table size error");
		thecore_shutdown();
		return;
	}
	data += 2;

	size = decode_2bytes(data);
	data += 2;
	sys_log(0, "BOOT: TALISMAN_ATTR_RARE: %d", size);

	if (size)
	{
		TItemAttrTable * p = (TItemAttrTable *) data;

		for (int i = 0; i < size; ++i, ++p)
		{
			if (p->dwApplyIndex >= MAX_APPLY_NUM)
				continue;

			g_map_talismanAttrRare[p->dwApplyIndex] = *p;
			sys_log(0, "TALISMAN_ATTR_RARE[%d]: %s %u", p->dwApplyIndex, p->szApply, p->dwProb);
		}
	}

	data += size * sizeof(TItemAttrTable);
#endif

	/*
	 * BANWORDS
	 */

	if (decode_2bytes(data) != sizeof(TBanwordTable))
	{
		sys_err("ban word table size error");
		thecore_shutdown();
		return;
	}
	data += 2;

	size = decode_2bytes(data);
	data += 2;

	CBanwordManager::instance().Initialize((TBanwordTable *) data, size);
	data += size * sizeof(TBanwordTable);

	{
		using namespace building;

		/*
		 * LANDS
		 */

		if (decode_2bytes(data) != sizeof(TLand))
		{
			sys_err("land table size error");
			thecore_shutdown();
			return;
		}
		data += 2;

		size = decode_2bytes(data);
		data += 2;

		TLand * kLand = (TLand *) data;
		data += size * sizeof(TLand);

		for (WORD i = 0; i < size; ++i, ++kLand)
			CManager::instance().LoadLand(kLand);

		/*
		 * OBJECT PROTO
		 */

		if (decode_2bytes(data) != sizeof(TObjectProto))
		{
			sys_err("object proto table size error");
			thecore_shutdown();
			return;
		}
		data += 2;

		size = decode_2bytes(data);
		data += 2;

		CManager::instance().LoadObjectProto((TObjectProto *) data, size);
		data += size * sizeof(TObjectProto);

		/*
		 * OBJECT
		 */
		if (decode_2bytes(data) != sizeof(TObject))
		{
			sys_err("object table size error");
			thecore_shutdown();
			return;
		}
		data += 2;

		size = decode_2bytes(data);
		data += 2;

		TObject * kObj = (TObject *) data;
		data += size * sizeof(TObject);

		for (WORD i = 0; i < size; ++i, ++kObj)
			CManager::instance().LoadObject(kObj, true);
	}

#ifdef __PREMIUM_PRIVATE_SHOP__
	if (decode_2bytes(data) != sizeof(TMarketItemPrice))
	{
		sys_err("private shop sale price size error");
		thecore_shutdown();
		return;
	}
	data += 2;

	size = decode_2bytes(data);
	data += 2;
	sys_log(0, "BOOT: PRIVATE SHOP SALE PRICE: %d", size);

	TMarketItemPrice* pMarketItemPrice = (TMarketItemPrice*)data;
	data += size * sizeof(TMarketItemPrice);

	for (WORD i = 0; i < size; ++i, ++pMarketItemPrice)
		CPrivateShopManager::Instance().AddMarketItemPrice(pMarketItemPrice->dwVnum, pMarketItemPrice->TPrice);
#endif

	set_global_time(*(time_t *) data);
	data += sizeof(time_t);

	if (decode_2bytes(data) != sizeof(TItemIDRangeTable) )
	{
		sys_err("ITEM ID RANGE size error");
		thecore_shutdown();
		return;
	}
	data += 2;

	size = decode_2bytes(data);
	data += 2;

	TItemIDRangeTable* range = (TItemIDRangeTable*) data;
	data += size * sizeof(TItemIDRangeTable);

	TItemIDRangeTable* rangespare = (TItemIDRangeTable*) data;
	data += size * sizeof(TItemIDRangeTable);

#ifdef __ENABLE_POLYMORPH_SYSTEM__
	data = LoadPolymorphData(data);
#endif
	//ADMIN_MANAGER

#ifdef __ENABLE_DAMAGE_RESTRICTIONS__
	// Monster Damage Restrictions
	if (decode_2bytes(data) != sizeof(TMonsterRestriction))
	{
		sys_err("TMonsterRestriction table size error");
		thecore_shutdown();
		return;
	}
	data += sizeof(WORD);

	size = decode_2bytes(data);
	data += sizeof(WORD);
	sys_log(0, "BOOT: MONSTER_RESTRICTIONS: %d", size);

	if (size)
	{
		CMobManager::instance().InitializeRestrictions(reinterpret_cast<TMonsterRestriction*>(const_cast<char*>(data)), size);
		data += size * sizeof(TMonsterRestriction);
	}
#endif

	int ChunkSize = decode_2bytes(data );
	data += 2;
	int HostSize = decode_2bytes(data );
	data += 2;
	sys_log(0, "GM Value Count %d %d", HostSize, ChunkSize  );
	for (int n = 0; n < HostSize; ++n )
	{
		gm_new_host_inert(data );
		sys_log(0, "GM HOST : IP[%s] ", data );
		data += ChunkSize;
	}

	data += 2;
	int adminsize = decode_2bytes(data );
	data += 2;

	for (int n = 0; n < adminsize; ++n )
	{
		tAdminInfo& rAdminInfo = *(tAdminInfo*)data;

		gm_new_insert(rAdminInfo );

		data += sizeof(rAdminInfo );
	}

	//END_ADMIN_MANAGER

	//MONARCH
	data += 2;
	data += 2;

	TMonarchInfo& p = *(TMonarchInfo *) data;
	data += sizeof(TMonarchInfo);

	CMonarch::instance().SetMonarchInfo(&p);

	for (int n = 1; n < 4; ++n)
	{
		if (p.name[n] && *p.name[n])
			sys_log(0, "[MONARCH] Empire %d Pid %d Money %d %s", n, p.pid[n], p.money[n], p.name[n]);
	}

	int CandidacySize = decode_2bytes(data);
	data += 2;

	int CandidacyCount = decode_2bytes(data);
	data += 2;

	if (test_server)
		sys_log (0, "[MONARCH] Size %d Count %d", CandidacySize, CandidacyCount);

	data += CandidacySize * CandidacyCount;

	//END_MONARCH

	WORD endCheck=decode_2bytes(data);
	if (endCheck != 0xffff)
	{
		sys_err("boot packet end check error [%x]!=0xffff", endCheck);
		thecore_shutdown();
		return;
	}
	else
		sys_log(0, "boot packet end check ok [%x]==0xffff", endCheck );
	data +=2;

	if (!ITEM_MANAGER::instance().SetMaxItemID(*range))
	{
		sys_err("not enough item id contact your administrator!");
		thecore_shutdown();
		return;
	}

	if (!ITEM_MANAGER::instance().SetMaxSpareItemID(*rangespare))
	{
		sys_err("not enough item id for spare contact your administrator!");
		thecore_shutdown();
		return;
	}

	// LOCALE_SERVICE
	const int FILE_NAME_LEN = 256;
	char szCommonDropItemFileName[FILE_NAME_LEN];
	char szETCDropItemFileName[FILE_NAME_LEN];
	char szMOBDropItemFileName[FILE_NAME_LEN];
	char szDropItemGroupFileName[FILE_NAME_LEN];
	char szSpecialItemGroupFileName[FILE_NAME_LEN];
	char szMapIndexFileName[FILE_NAME_LEN];
	char szItemVnumMaskTableFileName[FILE_NAME_LEN];
	char szDragonSoulTableFileName[FILE_NAME_LEN];
#ifdef ENABLE_DAMAGE_LIMIT_CONFIG
	char szDamageLimitTableFileName[FILE_NAME_LEN];
#endif

	snprintf(szCommonDropItemFileName, sizeof(szCommonDropItemFileName),
			"%s/common_drop_item.txt", LocaleService_GetBasePath().c_str());
	snprintf(szETCDropItemFileName, sizeof(szETCDropItemFileName),
			"%s/etc_drop_item.txt", LocaleService_GetBasePath().c_str());
	snprintf(szMOBDropItemFileName, sizeof(szMOBDropItemFileName),
			"%s/mob_drop_item.txt", LocaleService_GetBasePath().c_str());
	snprintf(szSpecialItemGroupFileName, sizeof(szSpecialItemGroupFileName),
			"%s/special_item_group.txt", LocaleService_GetBasePath().c_str());
	snprintf(szDropItemGroupFileName, sizeof(szDropItemGroupFileName),
			"%s/drop_item_group.txt", LocaleService_GetBasePath().c_str());
	snprintf(szMapIndexFileName, sizeof(szMapIndexFileName),
			"%s/index", LocaleService_GetMapPath().c_str());
	snprintf(szItemVnumMaskTableFileName, sizeof(szItemVnumMaskTableFileName),
			"%s/ori_to_new_table.txt", LocaleService_GetBasePath().c_str());
	snprintf(szDragonSoulTableFileName, sizeof(szDragonSoulTableFileName),
			"%s/dragon_soul_table.txt", LocaleService_GetBasePath().c_str());
#ifdef ENABLE_DAMAGE_LIMIT_CONFIG
	snprintf(szDamageLimitTableFileName, sizeof(szDamageLimitTableFileName),
			"%s/damage_limits.txt", LocaleService_GetBasePath().c_str());
#endif

	sys_log(0, "LoadLocaleFile: CommonDropItem: %s", szCommonDropItemFileName);
	if (!ITEM_MANAGER::instance().ReadCommonDropItemFile(szCommonDropItemFileName))
	{
		sys_err("cannot load CommonDropItem: %s", szCommonDropItemFileName);
		thecore_shutdown();
		return;
	}

	sys_log(0, "LoadLocaleFile: ETCDropItem: %s", szETCDropItemFileName);
	if (!ITEM_MANAGER::instance().ReadEtcDropItemFile(szETCDropItemFileName))
	{
		sys_err("cannot load ETCDropItem: %s", szETCDropItemFileName);
		thecore_shutdown();
		return;
	}

	sys_log(0, "LoadLocaleFile: DropItemGroup: %s", szDropItemGroupFileName);
	if (!ITEM_MANAGER::instance().ReadDropItemGroup(szDropItemGroupFileName))
	{
		sys_err("cannot load DropItemGroup: %s", szDropItemGroupFileName);
		thecore_shutdown();
		return;
	}

	sys_log(0, "LoadLocaleFile: SpecialItemGroup: %s", szSpecialItemGroupFileName);
	if (!ITEM_MANAGER::instance().ReadSpecialDropItemFile(szSpecialItemGroupFileName))
	{
		sys_err("cannot load SpecialItemGroup: %s", szSpecialItemGroupFileName);
		thecore_shutdown();
		return;
	}

	sys_log(0, "LoadLocaleFile: ItemVnumMaskTable : %s", szItemVnumMaskTableFileName);
	if (!ITEM_MANAGER::instance().ReadItemVnumMaskTable(szItemVnumMaskTableFileName))
	{
		sys_log(0, "Could not open MaskItemTable");
	}

	sys_log(0, "LoadLocaleFile: MOBDropItemFile: %s", szMOBDropItemFileName);
	if (!ITEM_MANAGER::instance().ReadMonsterDropItemGroup(szMOBDropItemFileName))
	{
		sys_err("cannot load MOBDropItemFile: %s", szMOBDropItemFileName);
		thecore_shutdown();
		return;
	}

	sys_log(0, "LoadLocaleFile: MapIndex: %s", szMapIndexFileName);
	if (!SECTREE_MANAGER::instance().Build(szMapIndexFileName, LocaleService_GetMapPath().c_str()))
	{
		sys_err("cannot load MapIndex: %s", szMapIndexFileName);
		thecore_shutdown();
		return;
	}

	sys_log(0, "LoadLocaleFile: DragonSoulTable: %s", szDragonSoulTableFileName);
	if (!DSManager::instance().ReadDragonSoulTableFile(szDragonSoulTableFileName))
	{
		sys_err("cannot load DragonSoulTable: %s", szDragonSoulTableFileName);
		//thecore_shutdown();
		//return;
	}

#ifdef ENABLE_DAMAGE_LIMIT_CONFIG
	sys_log(0, "ReadDamageLimits: %s", szDamageLimitTableFileName);
	if (!ReadDamageLimits(szDamageLimitTableFileName))
	{
		sys_err("cannot load ReadDamageLimits: %s", szDamageLimitTableFileName);
		thecore_shutdown();
		return;
	}
#endif

	// END_OF_LOCALE_SERVICE
#ifdef ENABLE_EXTENDED_BATTLE_PASS
	if (!CBattlePassManager::instance().InitializeBattlePass())
		sys_err("Failure to Initialize Extended BattlePass!");
#endif

#if defined(__ITEM_APPLY_RANDOM__)
	char szApplyRandomTableFileName[FILE_NAME_LEN];
	snprintf(szApplyRandomTableFileName, sizeof(szApplyRandomTableFileName),
		"%s/apply_random_table.txt", LocaleService_GetBasePath().c_str());

	sys_log(0, "LoadLocaleFile: ApplyRandomTable: %s", szApplyRandomTableFileName);
	if (!ITEM_MANAGER::instance().ReadApplyRandomTableFile(szApplyRandomTableFileName))
	{
		sys_err("LoadLocaleFile: Cannot load ApplyRandomTable: %s", szApplyRandomTableFileName);
		//thecore_shutdown();
		//return;
	}
#endif

	CGuildManager::instance().InitializeGuardItems();

	building::CManager::instance().FinalizeBoot();
	CMotionManager::instance().Build();

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	CLocaleItemManager::instance().LoadAll((LocaleService_GetBasePath() + "/language").c_str());
#endif

	signal_timer_enable(30);

	if (test_server)
		CMobManager::instance().DumpRegenCount("mob_count");

#ifdef __PREMIUM_PRIVATE_SHOP__
	BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_INIT;
	db_clientdesc->DBPacket(HEADER_GD_PRIVATE_SHOP, 0, &bSubHeader, sizeof(bSubHeader));
#endif

	// castle_boot
	castle_boot();
	
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	if (g_bEnableRespawnOfflineShop)
	{
		MYSQL_ROW row;
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT * FROM %soffline_shop_npc", get_table_postfix()));
            
		if(pMsg->Get()->uiNumRows != 0)
		{
			while (NULL != (row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
			{			
				char szName[CHARACTER_NAME_MAX_LEN + 1];
				char szSign[SHOP_SIGN_MAX_LEN + 1];
				DWORD dwOwnerID = 0;
				int32_t lMapIndex = 0,x = 0, y = 0, z = 0;
				int iTime = 0;
				BYTE bChannel = 0;
				int mNpc = 0;

				str_to_number(dwOwnerID, row[0]);
				snprintf(szSign, sizeof(szSign), "%s", row[1]);					
				snprintf(szName, sizeof(szName), "%s", row[2]);
				str_to_number(iTime, row[3]);
				str_to_number(x, row[4]);
				str_to_number(y, row[5]);
				str_to_number(z, row[6]);
				str_to_number(lMapIndex, row[7]);
				str_to_number(bChannel, row[8]);
				str_to_number(mNpc, row[9]);

				if (g_bChannel == bChannel)
				{
					LPCHARACTER npc = CHARACTER_MANAGER::instance().SpawnMob(mNpc, lMapIndex, x, y, z, false, -1, false, true, dwOwnerID);
					if (npc)
					{
						npc->SetName(szName);
						npc->SetOfflineShopSign(szSign);							
							
						if (iTime != 0)							
						{
							npc->SetOfflineShopTimer(iTime);                        
							npc->StartOfflineShopUpdateEvent();						
						}

						LPOFFLINESHOP pkOfflineShop = COfflineShopManager::instance().CreateOfflineShop(npc, dwOwnerID);
						npc->SetOfflineShop(pkOfflineShop);
						npc->SetOfflineShopChannel(bChannel);

						npc->Show(lMapIndex, x, y, z, true);
						++g_shopCount[lMapIndex];
					}
				}
			}
		}
	}
#endif

#ifdef ENABLE_QUEST_BOOT_EVENT
	quest::CQuestManager::instance().Boot();
#endif
#ifdef ENABLE_REAL_TIME_REGEN
	CRealTimeRegen::Instance().Initialize(); 
#endif
	MonsterSpawner::instance().Initialize();
}

EVENTINFO(quest_login_event_info)
{
	DWORD dwPID;

	quest_login_event_info()
	: dwPID( 0 )
	{
	}
};

EVENTFUNC(quest_login_event)
{
	quest_login_event_info* info = dynamic_cast<quest_login_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "quest_login_event> <Factor> Null pointer" );
		return 0;
	}

	DWORD dwPID = info->dwPID;

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(dwPID);

	if (!ch)
		return 0;

	LPDESC d = ch->GetDesc();

	if (!d)
		return 0;

	if (d->IsPhase(PHASE_HANDSHAKE) ||
		d->IsPhase(PHASE_LOGIN) ||
		d->IsPhase(PHASE_SELECT) ||
		d->IsPhase(PHASE_DEAD) ||
		d->IsPhase(PHASE_LOADING))
	{
		return PASSES_PER_SEC(1);
	}
	else if (d->IsPhase(PHASE_CLOSE))
	{
		return 0;
	}
	else if (d->IsPhase(PHASE_GAME))
	{
		sys_log(0, "QUEST_LOAD: Login pc %d by event", ch->GetPlayerID());
#ifdef __SPIN_WHEEL__
		if (quest::CQuestManager::instance().GetEventFlag("spin_wheel") == 1)
			ch->ChatPacket(CHAT_TYPE_COMMAND, "SetSpinWheel %d 1", ch->GetQuestFlag("spin_wheel.count"));
#endif
		quest::CQuestManager::instance().Login(ch->GetPlayerID());
		return 0;
	}
	else
	{
		sys_err(0, "input_db.cpp:quest_login_event INVALID PHASE pid %d", ch->GetPlayerID());
		return 0;
	}
}

void CInputDB::QuestLoad(LPDESC d, const char * c_pData)
{
	if (NULL == d)
		return;

	LPCHARACTER ch = d->GetCharacter();

	if (NULL == ch)
		return;

	const DWORD dwCount = decode_4bytes(c_pData);

	const TQuestTable* pQuestTable = reinterpret_cast<const TQuestTable*>(c_pData+4);

	if (NULL != pQuestTable)
	{
		if (dwCount != 0)
		{
			if (ch->GetPlayerID() != pQuestTable[0].dwPID)
			{
				sys_err("PID differs %u %u", ch->GetPlayerID(), pQuestTable[0].dwPID);
				return;
			}
		}

		sys_log(0, "QUEST_LOAD: count %d", dwCount);

		quest::PC * pkPC = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());

		if (!pkPC)
		{
			sys_err("null quest::PC with id %u", pQuestTable[0].dwPID);
			return;
		}

		if (pkPC->IsLoaded())
			return;

		for (unsigned int i = 0; i < dwCount; ++i)
		{
			std::string st(pQuestTable[i].szName);

			st += ".";
			st += pQuestTable[i].szState;

			sys_log(0,  "            %s %d", st.c_str(), pQuestTable[i].lValue);
			pkPC->SetFlag(st.c_str(), pQuestTable[i].lValue, false);
		}

		pkPC->SetLoaded();
		pkPC->Build();

		if (ch->GetDesc()->IsPhase(PHASE_GAME))
		{
			sys_log(0, "QUEST_LOAD: Login pc %d", pQuestTable[0].dwPID);
#ifdef __SPIN_WHEEL__
			if (quest::CQuestManager::instance().GetEventFlag("spin_wheel") == 1)
				ch->ChatPacket(CHAT_TYPE_COMMAND, "SetSpinWheel %d 1", ch->GetQuestFlag("spin_wheel.count"));
#endif
			quest::CQuestManager::instance().Login(pQuestTable[0].dwPID);
		}
		else
		{
			quest_login_event_info* info = AllocEventInfo<quest_login_event_info>();
			info->dwPID = ch->GetPlayerID();

			event_create(quest_login_event, info, PASSES_PER_SEC(1));
		}
	}
}

void CInputDB::SafeboxLoad(LPDESC d, const char * c_pData)
{
	if (!d)
		return;

	TSafeboxTable * p = (TSafeboxTable *) c_pData;

	if (d->GetAccountTable().id != p->dwID)
	{
		sys_err("SafeboxLoad: safebox has different id %u != %u", d->GetAccountTable().id, p->dwID);
		return;
	}

	if (!d->GetCharacter())
		return;

	BYTE bSize = 1;

	LPCHARACTER ch = d->GetCharacter();

	//PREVENT_TRADE_WINDOW
	if (ch->GetShopOwner() || ch->GetExchange() || ch->GetMyShop() || ch->IsCubeOpen()
#ifdef __ENABLE_POLYMORPH_SYSTEM__
		|| ch->IsPolyShopping()
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		|| ch->GetOfflineShopOwner()
#endif
	)	
	{
		d->GetCharacter()->ChatPacket(CHAT_TYPE_INFO, "[LS;773]" );
		d->GetCharacter()->CancelSafeboxLoad();
		return;
	}
	//END_PREVENT_TRADE_WINDOW

	// ADD_PREMIUM
	if (d->GetCharacter()->GetPremiumRemainSeconds(PREMIUM_SAFEBOX) > 0 ||
			d->GetCharacter()->IsEquipUniqueGroup(UNIQUE_GROUP_LARGE_SAFEBOX))
		bSize = 3;
	// END_OF_ADD_PREMIUM

	//if (d->GetCharacter()->IsEquipUniqueItem(UNIQUE_ITEM_SAFEBOX_EXPAND))

	//d->GetCharacter()->LoadSafebox(p->bSize * SAFEBOX_PAGE_SIZE, p->dwGold, p->wItemCount, (TPlayerItem *) (c_pData + sizeof(TSafeboxTable)));
	d->GetCharacter()->LoadSafebox(bSize * SAFEBOX_PAGE_SIZE, p->dwGold, p->wItemCount, (TPlayerItem *) (c_pData + sizeof(TSafeboxTable)));
}

void CInputDB::SafeboxChangeSize(LPDESC d, const char * c_pData)
{
	if (!d)
		return;

	BYTE bSize = *(BYTE *) c_pData;

	if (!d->GetCharacter())
		return;

	d->GetCharacter()->ChangeSafeboxSize(bSize);
}

//
//
void CInputDB::SafeboxWrongPassword(LPDESC d)
{
	if (!d)
		return;

	if (!d->GetCharacter())
		return;

	TPacketCGSafeboxWrongPassword p;
	p.bHeader = HEADER_GC_SAFEBOX_WRONG_PASSWORD;
	d->Packet(&p, sizeof(p));

	d->GetCharacter()->CancelSafeboxLoad();
}

void CInputDB::SafeboxChangePasswordAnswer(LPDESC d, const char* c_pData)
{
	if (!d)
		return;

	if (!d->GetCharacter())
		return;

	TSafeboxChangePasswordPacketAnswer* p = (TSafeboxChangePasswordPacketAnswer*) c_pData;
	if (p->flag)
	{
		d->GetCharacter()->ChatPacket(CHAT_TYPE_INFO, "[LS;774]");
	}
	else
	{
		d->GetCharacter()->ChatPacket(CHAT_TYPE_INFO, "[LS;775]");
	}
}

void CInputDB::MallLoad(LPDESC d, const char * c_pData)
{
	if (!d)
		return;

	TSafeboxTable * p = (TSafeboxTable *) c_pData;

	if (d->GetAccountTable().id != p->dwID)
	{
		sys_err("safebox has different id %u != %u", d->GetAccountTable().id, p->dwID);
		return;
	}

	if (!d->GetCharacter())
		return;

	d->GetCharacter()->LoadMall(p->wItemCount, (TPlayerItem *) (c_pData + sizeof(TSafeboxTable)));
}

void CInputDB::LoginAlready(LPDESC d, const char * c_pData)
{
	if (!d)
		return;

	{
		TPacketDGLoginAlready * p = (TPacketDGLoginAlready *) c_pData;

		LPDESC d2 = DESC_MANAGER::instance().FindByLoginName(p->szLogin);

		if (d2)
			d2->DisconnectOfSameLogin();
		else
		{
			TPacketGGDisconnect pgg;

			pgg.bHeader = HEADER_GG_DISCONNECT;
			strlcpy(pgg.szLogin, p->szLogin, sizeof(pgg.szLogin));

			P2P_MANAGER::instance().Send(&pgg, sizeof(TPacketGGDisconnect));
		}
	}
	// END_OF_INTERNATIONAL_VERSION

	LoginFailure(d, "ALREADY");
}

void CInputDB::EmpireSelect(LPDESC d, const char * c_pData)
{
	sys_log(0, "EmpireSelect %p", get_pointer(d));

	if (!d)
		return;

	TAccountTable & rTable = d->GetAccountTable();
	rTable.bEmpire = *(BYTE *) c_pData;

	TPacketGCEmpire pe;
	pe.bHeader = HEADER_GC_EMPIRE;
	pe.bEmpire = rTable.bEmpire;
	d->Packet(&pe, sizeof(pe));

	for (int i = 0; i < PLAYER_PER_ACCOUNT; ++i)
		if (rTable.players[i].dwID)
		{
			rTable.players[i].x = EMPIRE_START_X(rTable.bEmpire);
			rTable.players[i].y = EMPIRE_START_Y(rTable.bEmpire);
		}

	GetServerLocation(d->GetAccountTable(), rTable.bEmpire);

	d->SendLoginSuccessPacket();
}

void CInputDB::MapLocations(const char * c_pData)
{
	BYTE bCount = *(BYTE *) (c_pData++);

	sys_log(0, "InputDB::MapLocations %d", bCount);

	TMapLocation * pLoc = (TMapLocation *) c_pData;

	while (bCount--)
	{
		for (int i = 0; i < MAP_ALLOW_LIMIT; ++i)
		{
			if (0 == pLoc->alMaps[i])
				break;

			CMapLocation::instance().Insert(pLoc->alMaps[i], pLoc->szHost, pLoc->wPort);
		}

		pLoc++;
	}
}

void CInputDB::P2P(const char * c_pData)
{
	extern LPFDWATCH main_fdw;

	TPacketDGP2P * p = (TPacketDGP2P *) c_pData;

	P2P_MANAGER& mgr = P2P_MANAGER::instance();

	if (false == DESC_MANAGER::instance().IsP2PDescExist(p->szHost, p->wPort))
	{
	    LPCLIENT_DESC pkDesc = NULL;
		sys_log(0, "InputDB:P2P %s:%u", p->szHost, p->wPort);
	    pkDesc = DESC_MANAGER::instance().CreateConnectionDesc(main_fdw, p->szHost, p->wPort, PHASE_P2P, false);
		mgr.RegisterConnector(pkDesc);
		pkDesc->SetP2P(p->szHost, p->wPort, p->bChannel);
	}
}

void CInputDB::GuildLoad(const char * c_pData)
{
	CGuildManager::instance().LoadGuild(*(DWORD *) c_pData);
}

void CInputDB::GuildSkillUpdate(const char* c_pData)
{
	TPacketGuildSkillUpdate * p = (TPacketGuildSkillUpdate *) c_pData;

	CGuild * g = CGuildManager::instance().TouchGuild(p->guild_id);

	if (g)
	{
		g->UpdateSkill(p->skill_point, p->skill_levels);
		g->GuildPointChange(POINT_SP, p->amount, p->save?true:false);
	}
}

void CInputDB::GuildWar(const char* c_pData)
{
	TPacketGuildWar * p = (TPacketGuildWar*) c_pData;

	sys_log(0, "InputDB::GuildWar %u %u state %d", p->dwGuildFrom, p->dwGuildTo, p->bWar);

	switch (p->bWar)
	{
		case GUILD_WAR_SEND_DECLARE:
		case GUILD_WAR_RECV_DECLARE:
			CGuildManager::instance().DeclareWar(p->dwGuildFrom, p->dwGuildTo, p->bType);
			break;

		case GUILD_WAR_REFUSE:
			CGuildManager::instance().RefuseWar(p->dwGuildFrom, p->dwGuildTo);
			break;

		case GUILD_WAR_WAIT_START:
			CGuildManager::instance().WaitStartWar(p->dwGuildFrom, p->dwGuildTo);
			break;

		case GUILD_WAR_CANCEL:
			CGuildManager::instance().CancelWar(p->dwGuildFrom, p->dwGuildTo);
			break;

		case GUILD_WAR_ON_WAR:
			CGuildManager::instance().StartWar(p->dwGuildFrom, p->dwGuildTo);
			break;

		case GUILD_WAR_END:
			CGuildManager::instance().EndWar(p->dwGuildFrom, p->dwGuildTo);
			break;

		case GUILD_WAR_OVER:
			CGuildManager::instance().WarOver(p->dwGuildFrom, p->dwGuildTo, p->bType);
			break;

		case GUILD_WAR_RESERVE:
			CGuildManager::instance().ReserveWar(p->dwGuildFrom, p->dwGuildTo, p->bType);
			break;

		default:
			sys_err("Unknown guild war state");
			break;
	}
}

void CInputDB::GuildWarScore(const char* c_pData)
{
	TPacketGuildWarScore* p = (TPacketGuildWarScore*) c_pData;
	CGuild * g = CGuildManager::instance().TouchGuild(p->dwGuildGainPoint);
	g->SetWarScoreAgainstTo(p->dwGuildOpponent, p->lScore);
}

void CInputDB::GuildSkillRecharge()
{
	CGuildManager::instance().SkillRecharge();
}

void CInputDB::GuildUpdateGuardItemData(const char* c_pData)
{
	TPacketGDGuildUpdateGuardItemData* p = (TPacketGDGuildUpdateGuardItemData*)c_pData;
	if (test_server)
		sys_log(0, "GuildUpdateGuardItemData %u %u %u %u", p->dwGuild, p->setID, p->pos, p->vnum);

	if (p->setID != 0 && p->setID != 1)
		return;

	if (p->pos < 0 || p->pos > 8)
		return;

	CGuild* g = CGuildManager::instance().TouchGuild(p->dwGuild);

	if (g)
		g->UpdateGuardItemData(p);
}

void CInputDB::GuildGuardUpdateActiveSetData(const char* c_pData)
{
	TPacketGuildUpdateGuardActiveSetData* p = (TPacketGuildUpdateGuardActiveSetData*)c_pData;
	if (test_server)
		sys_log(0, "GuildGuardUpdateActiveSetData %u %u", p->dwGuild, p->activeSetID);

	CGuild* g = CGuildManager::instance().TouchGuild(p->dwGuild);
	if (g)
		g->GuardGetActiveSetData(p->activeSetID, p->nextCanChangeBonusActiveStatusTime, p->guard_active_expire_time);
}

void CInputDB::GuildExpUpdate(const char* c_pData)
{
	TPacketGuildSkillUpdate * p = (TPacketGuildSkillUpdate *) c_pData;
	if (test_server)
		sys_log(0, "InputDB::GuildExpUpdate  %u", p->guild_id);

	CGuild * g = CGuildManager::instance().TouchGuild(p->guild_id);
	if (g)
		g->GuildPointChange(GUILD_POINT_EXP, p->amount);
}

void CInputDB::GuildAddMember(const char* c_pData)
{
	TPacketDGGuildMember * p = (TPacketDGGuildMember *) c_pData;
	CGuild * g = CGuildManager::instance().TouchGuild(p->dwGuild);

	if (g)
		g->AddMember(p);
}

void CInputDB::GuildRemoveMember(const char* c_pData)
{
	TPacketGuild* p=(TPacketGuild*)c_pData;
	CGuild* g = CGuildManager::instance().TouchGuild(p->dwGuild);

	if (g)
		g->RemoveMember(p->dwInfo);
}

void CInputDB::GuildChangeGrade(const char* c_pData)
{
	TPacketGuild* p=(TPacketGuild*)c_pData;
	CGuild* g = CGuildManager::instance().TouchGuild(p->dwGuild);

	if (g)
		g->P2PChangeGrade((BYTE)p->dwInfo);
}

void CInputDB::GuildChangeMemberData(const char* c_pData)
{
	sys_log(0, "Recv GuildChangeMemberData");
	TPacketGuildChangeMemberData * p = (TPacketGuildChangeMemberData *) c_pData;
	CGuild * g = CGuildManager::instance().TouchGuild(p->guild_id);

	if (g)
		g->ChangeMemberData(p->pid, p->offer, p->level, p->grade);
}

void CInputDB::GuildDisband(const char* c_pData)
{
	TPacketGuild * p = (TPacketGuild*) c_pData;
	CGuildManager::instance().DisbandGuild(p->dwGuild);
}

void CInputDB::GuildLadder(const char* c_pData)
{
	TPacketGuildLadder* p = (TPacketGuildLadder*) c_pData;
	sys_log(0, "Recv GuildLadder %u %d / w %d d %d l %d", p->dwGuild, p->lLadderPoint, p->lWin, p->lDraw, p->lLoss);
	CGuild * g = CGuildManager::instance().TouchGuild(p->dwGuild);

	g->SetLadderPoint(p->lLadderPoint);
	g->SetWarData(p->lWin, p->lDraw, p->lLoss);
}

void CInputDB::GuildRefreshNote(const char* c_pData)
{
	if (test_server)
		sys_log(0, "InputDB::GuildRefreshNote  %u", *(DWORD*)c_pData);
	CGuild* g = CGuildManager::instance().TouchGuild(*(DWORD*)c_pData);

	if (g)
		g->RefreshNote(NULL, true);
}

void CInputDB::GuildNoticeDungeonOpen(const char* c_pData)
{
	TPacketGDGuildNoticeDungeonOpened* p = (TPacketGDGuildNoticeDungeonOpened*)c_pData;
	if (test_server)
		sys_log(0, "InputDB::GuildNoticeDungeonOpen  %u", p->dwGuild);

	CGuild* g = CGuildManager::instance().TouchGuild(p->dwGuild);
	if (g)
		g->NoticeDungeonOpen(p->dungeonIdx, p->dungeonMapIdx);
}

void CInputDB::GuildNoticeDungeonEnd(const char* c_pData)
{
	TPacketGDGuildNoticeDungeonEnd* p = (TPacketGDGuildNoticeDungeonEnd*)c_pData;
	if (test_server)
		sys_log(0, "InputDB::GuildNoticeDungeonEnd  %u", p->dwGuild);

	CGuild* g = CGuildManager::instance().TouchGuild(p->dwGuild);
	if (g)
		g->NoticeDungeonEnd(p);
}

void CInputDB::GuildNoticeTicketsChanged(const char* c_pData)
{
	TPacketGDGuildNoticeTicketsChanged* p = (TPacketGDGuildNoticeTicketsChanged*)c_pData;
	if (test_server)
		sys_log(0, "InputDB::GuildNoticeTicketsChanged  %u", p->dwGuild);

	CGuild* g = CGuildManager::instance().TouchGuild(p->dwGuild);
	if (g)
		g->NoticeTicketsChanged(p->tickets);
}

void CInputDB::ItemLoad(LPDESC d, const char * c_pData)
{
	LPCHARACTER ch;

	if (!d || !(ch = d->GetCharacter()))
		return;

	if (ch->IsItemLoaded())
		return;

	DWORD dwCount = decode_4bytes(c_pData);
	c_pData += sizeof(DWORD);

	sys_log(0, "ITEM_LOAD: COUNT %s %u", ch->GetName(), dwCount);

	std::vector<LPITEM> v;

	TPlayerItem * p = (TPlayerItem *) c_pData;

	for (DWORD i = 0; i < dwCount; ++i, ++p)
	{
		LPITEM item = ITEM_MANAGER::instance().CreateItem(p->vnum, p->count, p->id);

		if (!item)
		{
			sys_err("cannot create item by vnum %u (name %s id %u)", p->vnum, ch->GetName(), p->id);
			continue;
		}

		item->SetSkipSave(true);
		item->SetSockets(p->alSockets);
#if defined(__ITEM_APPLY_RANDOM__)
		item->SetRandomApplies(p->aApplyRandom);
#endif
		item->SetAttributes(p->aAttr);

#ifdef ENABLE_HIGHLIGHT_NEW_ITEM
		item->SetLastOwnerPID(p->owner);
#endif

#ifdef ENABLE_BELT_INVENTORY_EX
		if (p->window == BELT_INVENTORY)
		{
			p->window = INVENTORY;
			p->pos = p->pos + BELT_INVENTORY_SLOT_START;
		}
#endif

		if ((p->window == INVENTORY && ch->GetInventoryItem(p->pos)) ||
				(p->window == EQUIPMENT && ch->GetWear(p->pos)))
		{
			sys_log(0, "ITEM_RESTORE: %s %s", ch->GetName(), item->GetName());
			v.emplace_back(item);
		}
		else
		{
			switch (p->window)
			{
				case INVENTORY:
				case DRAGON_SOUL_INVENTORY:
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
				case BUFF_EQUIPMENT:
#endif
#ifdef ENABLE_SWITCHBOT
				case SWITCHBOT:
#endif
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
				case SKILL_BOOK_INVENTORY:
				case UPGRADE_ITEMS_INVENTORY:
				case STONE_INVENTORY:
				case BOX_INVENTORY:
				case EFSUN_INVENTORY:
				case CICEK_INVENTORY:
#endif
					item->AddToCharacter(ch, TItemPos(p->window, p->pos));
					break;

					case EQUIPMENT:
					if (item->CheckItemUseLevel(ch->GetLevel()) == true )
					{
#ifdef ENABLE_NEW_PET_SYSTEM
						if (item->EquipTo(ch, p->pos, true) == false )
#else
						if (item->EquipTo(ch, p->pos) == false )
#endif
						{
							v.emplace_back(item);
						}
					}
					else
					{
						v.emplace_back(item);
					}
					break;
			}
		}

		if (false == item->OnAfterCreatedItem())
			sys_err("Failed to call ITEM::OnAfterCreatedItem (vnum: %d, id: %d)", item->GetVnum(), item->GetID());

		item->OnLoad();

		item->SetSkipSave(false);
	}

	itertype(v) it = v.begin();

	while (it != v.end())
	{
		LPITEM item = *(it++);

		int pos = ch->GetEmptyInventory(item->GetSize());

		if (pos < 0)
		{
			PIXEL_POSITION coord;
			coord.x = ch->GetX();
			coord.y = ch->GetY();

			item->AddToGround(ch->GetMapIndex(), coord);
			item->SetOwnership(ch, 180);
			item->StartDestroyEvent();
		}
		else
			item->AddToCharacter(ch, TItemPos(INVENTORY, pos));
	}

	ch->CheckMaximumPoints();
	ch->PointsPacket();

	ch->SetItemLoaded();
}

#ifdef ENABLE_EXTENDED_BATTLE_PASS
void CInputDB::ExtBattlePassLoad(LPDESC d, const char* c_pData)
{
	if (!d || !d->GetCharacter())
		return;

	LPCHARACTER ch = d->GetCharacter();
	if (!ch)
		return;

	DWORD dwPID = decode_4bytes(c_pData);
	c_pData += sizeof(DWORD);

	DWORD dwCount = decode_4bytes(c_pData);
	c_pData += sizeof(DWORD);

	if (ch->GetPlayerID() != dwPID)
		return;

	ch->LoadExtBattlePass(dwCount, (TPlayerExtBattlePassMission*)c_pData);
}
#endif

void CInputDB::AffectLoad(LPDESC d, const char * c_pData)
{
	if (!d)
		return;

	if (!d->GetCharacter())
		return;

	LPCHARACTER ch = d->GetCharacter();

	DWORD dwPID = decode_4bytes(c_pData);
	c_pData += sizeof(DWORD);

	DWORD dwCount = decode_4bytes(c_pData);
	c_pData += sizeof(DWORD);

	if (ch->GetPlayerID() != dwPID)
		return;

	ch->LoadAffect(dwCount, (TPacketAffectElement *) c_pData);
}

void CInputDB::PartyCreate(const char* c_pData)
{
	TPacketPartyCreate* p = (TPacketPartyCreate*) c_pData;
	CPartyManager::instance().P2PCreateParty(p->dwLeaderPID);
}

void CInputDB::PartyDelete(const char* c_pData)
{
	TPacketPartyDelete* p = (TPacketPartyDelete*) c_pData;
	CPartyManager::instance().P2PDeleteParty(p->dwLeaderPID);
}

void CInputDB::PartyAdd(const char* c_pData)
{
	TPacketPartyAdd* p = (TPacketPartyAdd*) c_pData;
	CPartyManager::instance().P2PJoinParty(p->dwLeaderPID, p->dwPID, p->bState);
}

void CInputDB::PartyRemove(const char* c_pData)
{
	TPacketPartyRemove* p = (TPacketPartyRemove*) c_pData;
	CPartyManager::instance().P2PQuitParty(p->dwPID);
}

void CInputDB::PartyStateChange(const char* c_pData)
{
	TPacketPartyStateChange * p = (TPacketPartyStateChange *) c_pData;
	LPPARTY pParty = CPartyManager::instance().P2PCreateParty(p->dwLeaderPID);

	if (!pParty)
		return;

	pParty->SetRole(p->dwPID, p->bRole, p->bFlag);
}

void CInputDB::PartySetMemberLevel(const char* c_pData)
{
	TPacketPartySetMemberLevel* p = (TPacketPartySetMemberLevel*) c_pData;
	LPPARTY pParty = CPartyManager::instance().P2PCreateParty(p->dwLeaderPID);

	if (!pParty)
		return;

	pParty->P2PSetMemberLevel(p->dwPID, p->bLevel);
}

void CInputDB::Time(const char * c_pData)
{
	set_global_time(*(time_t *) c_pData);
}

void CInputDB::ReloadProto(const char * c_pData)
{
	WORD wSize;

	/*
	 * Skill
	 */
	wSize = decode_2bytes(c_pData);
	c_pData += sizeof(WORD);
	if (wSize) CSkillManager::instance().Initialize((TSkillTable *) c_pData, wSize);
	c_pData += sizeof(TSkillTable) * wSize;

	/*
	 * Banwords
	 */

	wSize = decode_2bytes(c_pData);
	c_pData += sizeof(WORD);
	CBanwordManager::instance().Initialize((TBanwordTable *) c_pData, wSize);
	c_pData += sizeof(TBanwordTable) * wSize;

	/*
	 * ITEM
	 */
	wSize = decode_2bytes(c_pData);
	c_pData += 2;
	sys_log(0, "RELOAD: ITEM: %d", wSize);

	if (wSize)
	{
		ITEM_MANAGER::instance().Initialize((TItemTable *) c_pData, wSize);
		c_pData += wSize * sizeof(TItemTable);
	}

	/*
	 * MONSTER
	 */
	wSize = decode_2bytes(c_pData);
	c_pData += 2;
	sys_log(0, "RELOAD: MOB: %d", wSize);

	if (wSize)
	{
		CMobManager::instance().Initialize((TMobTable *) c_pData, wSize);
		c_pData += wSize * sizeof(TMobTable);
	}

	/*
	* SHOP
	*/

	wSize = decode_2bytes(c_pData);
	c_pData += sizeof(WORD);
	sys_log(0, "RELOAD: SHOP: %d", wSize);


	if (wSize)
	{
		CShopManager::instance().Initialize((TShopTable *)c_pData, wSize);
		c_pData += wSize * sizeof(TShopTable);
	}

	/*
	* REFINE
	*/
	wSize = decode_2bytes(c_pData);
	c_pData += 2;
	sys_log(0, "RELOAD: REFINE: %d", wSize);

	if (wSize)
	{
		CRefineManager::instance().Initialize((TRefineTable *)c_pData, wSize);
		c_pData += wSize * sizeof(TRefineTable);
	}

	/*
	* ATTR
	*/
	wSize = decode_2bytes(c_pData);
	c_pData += 2;
	sys_log(0, "RELOAD: ItemAtt: %d", wSize);

	if (wSize)
	{
		TItemAttrTable * p = (TItemAttrTable *)c_pData;
		g_map_itemAttr.clear();
		for (int i = 0; i < wSize; ++i, ++p)
		{
			if (p->dwApplyIndex >= MAX_APPLY_NUM)
				continue;

			g_map_itemAttr[p->dwApplyIndex] = *p;
			sys_log(0, "ITEM_ATTR[%d]: %s %u", p->dwApplyIndex, p->szApply, p->dwProb);
		}
		c_pData += wSize*sizeof(TItemAttrTable);
	}

	/*
	* ATTR_RARE
	*/
	wSize = decode_2bytes(c_pData);
	c_pData += 2;
	sys_log(0, "RELOAD: ItemRareAtt: %d", wSize);

	if (wSize)
	{
		TItemAttrTable * p = (TItemAttrTable *)c_pData;
		g_map_itemRare.clear();
		for (int i = 0; i < wSize; ++i, ++p)
		{
			if (p->dwApplyIndex >= MAX_APPLY_NUM)
				continue;

			g_map_itemRare[p->dwApplyIndex] = *p;
			sys_log(0, "ITEM_RARE[%d]: %s %u", p->dwApplyIndex, p->szApply, p->dwProb);
		}
		c_pData += wSize*sizeof(TItemAttrTable);
	}

#ifdef ENABLE_BELT_ATTRIBUTES
	/*
	* BELT_ATTR
	*/
	wSize = decode_2bytes(c_pData);
	c_pData += 2;
	sys_log(0, "RELOAD: BeltAtt: %d", wSize);

	if (wSize)
	{
		TItemAttrTable * p = (TItemAttrTable *)c_pData;
		g_map_beltAttr.clear();
		for (int i = 0; i < wSize; ++i, ++p)
		{
			if (p->dwApplyIndex >= MAX_APPLY_NUM)
				continue;

			g_map_beltAttr[p->dwApplyIndex] = *p;
			sys_log(0, "BELT_ATTR[%d]: %s %u", p->dwApplyIndex, p->szApply, p->dwProb);
		}
		c_pData += wSize*sizeof(TItemAttrTable);
	}
#endif

#ifdef ENABLE_GLOVE_ATTRIBUTES
	/*
	* GLOVE_ATTR
	*/
	wSize = decode_2bytes(c_pData);
	c_pData += 2;
	sys_log(0, "RELOAD: GloveAtt: %d", wSize);

	if (wSize)
	{
		TItemAttrTable * p = (TItemAttrTable *)c_pData;
		g_map_gloveAttr.clear();
		for (int i = 0; i < wSize; ++i, ++p)
		{
			if (p->dwApplyIndex >= MAX_APPLY_NUM)
				continue;

			g_map_gloveAttr[p->dwApplyIndex] = *p;
			sys_log(0, "GLOVE_ATTR[%d]: %s %u", p->dwApplyIndex, p->szApply, p->dwProb);
		}
		c_pData += wSize*sizeof(TItemAttrTable);
	}
#endif

#ifdef ENABLE_TALISMAN_ATTRIBUTES
	/*
	* TALISMAN_ATTR
	*/
	wSize = decode_2bytes(c_pData);
	c_pData += 2;
	sys_log(0, "RELOAD: TalismanAttr: %d", wSize);

	if (wSize)
	{
		TItemAttrTable * p = (TItemAttrTable *)c_pData;
		g_map_talismanAttr.clear();
		for (int i = 0; i < wSize; ++i, ++p)
		{
			if (p->dwApplyIndex >= MAX_APPLY_NUM)
				continue;

			g_map_talismanAttr[p->dwApplyIndex] = *p;
			sys_log(0, "TALISMAN_ATTR[%d]: %s %u", p->dwApplyIndex, p->szApply, p->dwProb);
		}
		c_pData += wSize*sizeof(TItemAttrTable);
	}
#endif

#ifdef ENABLE_TALISMAN_ATTRIBUTES_RARE
	/*
	* TALISMAN_ATTR
	*/
	wSize = decode_2bytes(c_pData);
	c_pData += 2;
	sys_log(0, "RELOAD: TalismanAttrRare: %d", wSize);

	if (wSize)
	{
		TItemAttrTable * p = (TItemAttrTable *)c_pData;
		g_map_talismanAttrRare.clear();
		for (int i = 0; i < wSize; ++i, ++p)
		{
			if (p->dwApplyIndex >= MAX_APPLY_NUM)
				continue;

			g_map_talismanAttrRare[p->dwApplyIndex] = *p;
			sys_log(0, "TALISMAN_ATTR_RARE[%d]: %s %u", p->dwApplyIndex, p->szApply, p->dwProb);
		}
		c_pData += wSize*sizeof(TItemAttrTable);
	}
#endif

#ifdef ENABLE_DAMAGE_LIMIT_CONFIG
	char szDamageLimitTableFileName[256];
	snprintf(szDamageLimitTableFileName, sizeof(szDamageLimitTableFileName),
		"%s/damage_limits.txt", LocaleService_GetBasePath().c_str());

	sys_log(0, "ReadDamageLimits: %s", szDamageLimitTableFileName);
	if (!ReadDamageLimits(szDamageLimitTableFileName))
	{
		sys_err("cannot load ReadDamageLimits: %s", szDamageLimitTableFileName);
	}
#endif

	CMotionManager::instance().Build();

	CHARACTER_MANAGER::instance().for_each_pc(std::mem_fn(&CHARACTER::ComputePoints));
}

void CInputDB::GuildSkillUsableChange(const char* c_pData)
{
	TPacketGuildSkillUsableChange* p = (TPacketGuildSkillUsableChange*) c_pData;

	CGuild* g = CGuildManager::instance().TouchGuild(p->dwGuild);

	g->SkillUsableChange(p->dwSkillVnum, p->bUsable?true:false);
}

void CInputDB::AuthLogin(LPDESC d, const char * c_pData)
{
	if (!d)
		return;

	BYTE bResult = *(BYTE *) c_pData;

	TPacketGCAuthSuccess ptoc;

	ptoc.bHeader = HEADER_GC_AUTH_SUCCESS;

	if (bResult)
	{
		SendPanamaList(d);
		ptoc.dwLoginKey = d->GetLoginKey();

		//Send Client Package CryptKey
		{
			DESC_MANAGER::instance().SendClientPackageCryptKey(d);
			DESC_MANAGER::instance().SendClientPackageSDBToLoadMap(d, MAPNAME_DEFAULT);
		}
	}
	else
	{
		ptoc.dwLoginKey = 0;
	}

	ptoc.bResult = bResult;
	// Validating handshake
	TPacketGGHandshakeValidate pack;
	pack.header = HEADER_GG_HANDSHAKE_VALIDATION;
	strlcpy(pack.sUserIP, d->GetHostName(), sizeof(pack.sUserIP));
	P2P_MANAGER::instance().Send(&pack, sizeof(pack));

	d->Packet(&ptoc, sizeof(TPacketGCAuthSuccess));
	sys_log(0, "AuthLogin result %u key %u", bResult, d->GetLoginKey());
}

void CInputDB::ChangeEmpirePriv(const char* c_pData)
{
	TPacketDGChangeEmpirePriv* p = (TPacketDGChangeEmpirePriv*) c_pData;

	// ADD_EMPIRE_PRIV_TIME
	CPrivManager::instance().GiveEmpirePriv(p->empire, p->type, p->value, p->bLog, p->end_time_sec);
	// END_OF_ADD_EMPIRE_PRIV_TIME
}

void CInputDB::ChangeGuildPriv(const char* c_pData)
{
	TPacketDGChangeGuildPriv* p = (TPacketDGChangeGuildPriv*) c_pData;

	// ADD_GUILD_PRIV_TIME
	CPrivManager::instance().GiveGuildPriv(p->guild_id, p->type, p->value, p->bLog, p->end_time_sec);
	// END_OF_ADD_GUILD_PRIV_TIME
}

void CInputDB::ChangeCharacterPriv(const char* c_pData)
{
	TPacketDGChangeCharacterPriv* p = (TPacketDGChangeCharacterPriv*) c_pData;
	CPrivManager::instance().GiveCharacterPriv(p->pid, p->type, p->value, p->bLog);
}

void CInputDB::MoneyLog(const char* c_pData)
{
	TPacketMoneyLog * p = (TPacketMoneyLog *) c_pData;

	if (p->type == 4) // QUEST_MONEY_LOG_SKIP
		return;

	if (g_bAuthServer ==true )
		return;

	LogManager::instance().MoneyLog(p->type, p->vnum, p->gold);
}

void CInputDB::GuildMoneyChange(const char* c_pData)
{
	TPacketDGGuildMoneyChange* p = (TPacketDGGuildMoneyChange*) c_pData;

	CGuild* g = CGuildManager::instance().TouchGuild(p->dwGuild);
	if (g)
	{
		g->RecvMoneyChange(p->iTotalGold);
	}
}

void CInputDB::GuildWithdrawMoney(const char* c_pData)
{
	TPacketDGGuildMoneyWithdraw* p = (TPacketDGGuildMoneyWithdraw*) c_pData;

	CGuild* g = CGuildManager::instance().TouchGuild(p->dwGuild);
	if (g)
	{
		g->RecvWithdrawMoneyGive(p->iChangeGold);
	}
}

void CInputDB::SetEventFlag(const char* c_pData)
{
	TPacketSetEventFlag* p = (TPacketSetEventFlag*) c_pData;
	quest::CQuestManager::instance().SetEventFlag(p->szFlagName, p->lValue);
}

void CInputDB::CreateObject(const char * c_pData)
{
	using namespace building;
	CManager::instance().LoadObject((TObject *) c_pData);
}

void CInputDB::DeleteObject(const char * c_pData)
{
	using namespace building;
	CManager::instance().DeleteObject(*(DWORD *) c_pData);
}

void CInputDB::UpdateLand(const char * c_pData)
{
	using namespace building;
	CManager::instance().UpdateLand((TLand *) c_pData);
}

////////////////////////////////////////////////////////////////////
// Billing
////////////////////////////////////////////////////////////////////
void CInputDB::BillingRepair(const char * c_pData)
{
	DWORD dwCount = *(DWORD *) c_pData;
	c_pData += sizeof(DWORD);

	TPacketBillingRepair * p = (TPacketBillingRepair *) c_pData;

	for (DWORD i = 0; i < dwCount; ++i, ++p)
	{
		CLoginData * pkLD = M2_NEW CLoginData;

		pkLD->SetKey(p->dwLoginKey);
		pkLD->SetLogin(p->szLogin);
		pkLD->SetIP(p->szHost);

		sys_log(0, "BILLING: REPAIR %s host %s", p->szLogin, p->szHost);
	}
}

void CInputDB::BillingExpire(const char * c_pData)
{
	TPacketBillingExpire * p = (TPacketBillingExpire *) c_pData;

	LPDESC d = DESC_MANAGER::instance().FindByLoginName(p->szLogin);

	if (!d)
		return;

	LPCHARACTER ch = d->GetCharacter();

	if (p->dwRemainSeconds <= 60)
	{
		int i = MAX(5, p->dwRemainSeconds);
		sys_log(0, "BILLING_EXPIRE: %s %u", p->szLogin, p->dwRemainSeconds);
		d->DelayedDisconnect(i);
	}
	else
	{
		if ((p->dwRemainSeconds - d->GetBillingExpireSecond()) > 60)
		{
			d->SetBillingExpireSecond(p->dwRemainSeconds);

			if (ch)
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;776;%d]", (p->dwRemainSeconds / 60));
		}
	}
}

void CInputDB::BillingLogin(const char * c_pData)
{
	if (NULL == c_pData)
		return;

	TPacketBillingLogin * p;

	DWORD dwCount = *(DWORD *) c_pData;
	c_pData += sizeof(DWORD);

	p = (TPacketBillingLogin *) c_pData;

	for (DWORD i = 0; i < dwCount; ++i, ++p)
	{
		DBManager::instance().SetBilling(p->dwLoginKey, p->bLogin);
	}
}

void CInputDB::BillingCheck(const char * c_pData)
{
	DWORD size = *(DWORD *) c_pData;
	c_pData += sizeof(DWORD);

	for (DWORD i = 0; i < size; ++i)
	{
		DWORD dwKey = *(DWORD *) c_pData;
		c_pData += sizeof(DWORD);

		sys_log(0, "BILLING: NOT_LOGIN %u", dwKey);
		DBManager::instance().SetBilling(dwKey, 0, true);
	}
}

void CInputDB::Notice(const char * c_pData)
{
	char szBuf[256+1];
	strlcpy(szBuf, c_pData, sizeof(szBuf));

	sys_log(0, "InputDB:: Notice: %s", szBuf);

	SendNotice(szBuf);
}

void CInputDB::VCard(const char * c_pData)
{
	TPacketGDVCard * p = (TPacketGDVCard *) c_pData;

	sys_log(0, "VCARD: %u %s %s %s %s", p->dwID, p->szSellCharacter, p->szSellAccount, p->szBuyCharacter, p->szBuyAccount);

	auto pmsg(DBManager::instance().DirectQuery("SELECT sell_account, buy_account, time FROM vcard WHERE id=%u", p->dwID));
	if (pmsg->Get()->uiNumRows != 1)
	{
		sys_log(0, "VCARD_FAIL: no data");
		return;
	}

	MYSQL_ROW row = mysql_fetch_row(pmsg->Get()->pSQLResult);

	if (strcmp(row[0], p->szSellAccount))
	{
		sys_log(0, "VCARD_FAIL: sell account differ %s", row[0]);
		return;
	}

	if (!row[1] || *row[1])
	{
		sys_log(0, "VCARD_FAIL: buy account already exist");
		return;
	}

	int time = 0;
	str_to_number(time, row[2]);

	if (!row[2] || time < 0)
	{
		sys_log(0, "VCARD_FAIL: time null");
		return;
	}

	auto pmsg1(DBManager::instance().DirectQuery("UPDATE GameTime SET LimitTime=LimitTime+%d WHERE UserID='%s'", time, p->szBuyAccount));
	if (pmsg1->Get()->uiAffectedRows == 0 || pmsg1->Get()->uiAffectedRows == (uint32_t)-1)
	{
		sys_log(0, "VCARD_FAIL: cannot modify GameTime table");
		return;
	}

	auto pmsg2(DBManager::instance().DirectQuery("UPDATE vcard,GameTime SET sell_pid='%s', buy_pid='%s', buy_account='%s', sell_time=NOW(), new_time=GameTime.LimitTime WHERE vcard.id=%u AND GameTime.UserID='%s'", p->szSellCharacter, p->szBuyCharacter, p->szBuyAccount, p->dwID, p->szBuyAccount));
	if (pmsg2->Get()->uiAffectedRows == 0 || pmsg2->Get()->uiAffectedRows == (uint32_t)-1)
	{
		sys_log(0, "VCARD_FAIL: cannot modify vcard table");
		return;
	}

	sys_log(0, "VCARD_SUCCESS: %s %s", p->szBuyAccount, p->szBuyCharacter);
}

void CInputDB::GuildWarReserveAdd(TGuildWarReserve * p)
{
	CGuildManager::instance().ReserveWarAdd(p);
}

void CInputDB::GuildWarReserveDelete(DWORD dwID)
{
	CGuildManager::instance().ReserveWarDelete(dwID);
}

void CInputDB::GuildWarBet(TPacketGDGuildWarBet * p)
{
	CGuildManager::instance().ReserveWarBet(p);
}

void CInputDB::MarriageAdd(TPacketMarriageAdd * p)
{
	sys_log(0, "MarriageAdd %u %u %u %s %s", p->dwPID1, p->dwPID2, (DWORD)p->tMarryTime, p->szName1, p->szName2);
	marriage::CManager::instance().Add(p->dwPID1, p->dwPID2, p->tMarryTime, p->szName1, p->szName2);
}

void CInputDB::MarriageUpdate(TPacketMarriageUpdate * p)
{
	sys_log(0, "MarriageUpdate %u %u %d %d", p->dwPID1, p->dwPID2, p->iLovePoint, p->byMarried);
	marriage::CManager::instance().Update(p->dwPID1, p->dwPID2, p->iLovePoint, p->byMarried);
}

void CInputDB::MarriageRemove(TPacketMarriageRemove * p)
{
	sys_log(0, "MarriageRemove %u %u", p->dwPID1, p->dwPID2);
	marriage::CManager::instance().Remove(p->dwPID1, p->dwPID2);
}

void CInputDB::WeddingRequest(TPacketWeddingRequest* p)
{
	marriage::WeddingManager::instance().Request(p->dwPID1, p->dwPID2);
}

void CInputDB::WeddingReady(TPacketWeddingReady* p)
{
	sys_log(0, "WeddingReady %u %u %u", p->dwPID1, p->dwPID2, p->dwMapIndex);
	marriage::CManager::instance().WeddingReady(p->dwPID1, p->dwPID2, p->dwMapIndex);
}

void CInputDB::WeddingStart(TPacketWeddingStart* p)
{
	sys_log(0, "WeddingStart %u %u", p->dwPID1, p->dwPID2);
	marriage::CManager::instance().WeddingStart(p->dwPID1, p->dwPID2);
}

void CInputDB::WeddingEnd(TPacketWeddingEnd* p)
{
	sys_log(0, "WeddingEnd %u %u", p->dwPID1, p->dwPID2);
	marriage::CManager::instance().WeddingEnd(p->dwPID1, p->dwPID2);
}

// MYSHOP_PRICE_LIST
void CInputDB::MyshopPricelistRes(LPDESC d, const TPacketMyshopPricelistHeader* p )
{
	LPCHARACTER ch;

	if (!d || !(ch = d->GetCharacter()) )
		return;

	sys_log(0, "RecvMyshopPricelistRes name[%s]", ch->GetName());
	ch->UseSilkBotaryReal(p );
}
// END_OF_MYSHOP_PRICE_LIST

void CInputDB::UserCount(LPDESC d, const TPacketDGUserCount* p)
{
	sys_err("ADMIN WHISPER: USERCOUNT");

	LPCHARACTER ch;

	if (!d || !(ch = d->GetCharacter()))
		return;

	ch->ChatPacket(CHAT_TYPE_PARTY, "PLAYERS: %d", p->user);
}

//RELOAD_ADMIN
void CInputDB::ReloadAdmin(const char * c_pData )
{
	gm_new_clear();
	int ChunkSize = decode_2bytes(c_pData );
	c_pData += 2;
	int HostSize = decode_2bytes(c_pData );
	c_pData += 2;

	for (int n = 0; n < HostSize; ++n )
	{
		gm_new_host_inert(c_pData );
		c_pData += ChunkSize;
	}

	c_pData += 2;
	int size = 	decode_2bytes(c_pData );
	c_pData += 2;

	for (int n = 0; n < size; ++n )
	{
		tAdminInfo& rAdminInfo = *(tAdminInfo*)c_pData;

		gm_new_insert(rAdminInfo );

		c_pData += sizeof (tAdminInfo );

		LPCHARACTER pChar = CHARACTER_MANAGER::instance().FindPC(rAdminInfo.m_szName );
		if (pChar )
		{
			pChar->SetGMLevel();
		}
	}
}
//END_RELOAD_ADMIN

#ifdef ENABLE_OFFLINE_SHOP
template <class T>
const char* Decode(T*& pObj, const char* data)
{
	pObj = (T*)data;
	return data + sizeof(T);
}

void OfflineShopLoadTables(const char* data)
{
	offlineshop::TSubPacketDGLoadTables* pSubPack = nullptr;
	data = Decode(pSubPack, data);
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	for (DWORD i = 0; i < pSubPack->dwShopCount; i++)
	{
		offlineshop::TShopInfo* pShop = nullptr;
#ifdef ENABLE_IRA_REWORK
		offlineshop::TShopPosition* pShopPos = nullptr;
#endif
		offlineshop::TItemInfo* pItem = nullptr;
		data = Decode(pShop, data);
#ifdef ENABLE_IRA_REWORK
		data = Decode(pShopPos, data);
#endif
#ifdef ENABLE_IRA_REWORK
		offlineshop::CShop* pkShop = rManager.PutsNewShop(pShop, pShopPos);
#else
		offlineshop::CShop* pkShop = rManager.PutsNewShop(pShop);
#endif
		for (DWORD j = 0; j < pShop->dwCount; j++)
		{
			data = Decode(pItem, data);
			offlineshop::CShopItem kItem(pItem->dwItemID);
			kItem.SetOwnerID(pItem->dwOwnerID);
			kItem.SetInfo(pItem->item);
			kItem.SetPrice(pItem->price);
			kItem.SetWindow(NEW_OFFSHOP);
			pkShop->AddItem(kItem);
		}
	}
}

void OfflineShopBuyItemPacket(const char* data)
{
	offlineshop::TSubPacketDGBuyItem* subpack;
	data = Decode(subpack, data);
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvShopBuyDBPacket(subpack->dwBuyerID, subpack->dwOwnerID, subpack->dwItemID);
}

void OfflineShopLockedBuyItemPacket(const char* data)
{
	offlineshop::TSubPacketDGLockedBuyItem* subpack;
	data = Decode(subpack, data);
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvShopLockedBuyItemDBPacket(subpack->dwBuyerID, subpack->dwOwnerID, subpack->dwItemID);
}

void OfflineShopEditItemPacket(const char* data)
{
	offlineshop::TSubPacketDGEditItem* subpack;
	data = Decode(subpack, data);
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvShopEditItemDBPacket(subpack->dwOwnerID, subpack->dwItemID, subpack->price);
}

void OfflineShopRemoveItemPacket(const char* data)
{
	offlineshop::TSubPacketDGRemoveItem* subpack;
	data = Decode(subpack, data);
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvShopRemoveItemDBPacket(subpack->dwOwnerID, subpack->dwItemID);
}

void OfflineShopAddItemPacket(const char* data)
{
	offlineshop::TSubPacketDGAddItem* subpack;
	data = Decode(subpack, data);
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvShopAddItemDBPacket(subpack->dwOwnerID, subpack->item);
}

void OfflineShopForceClosePacket(const char* data)
{
	offlineshop::TSubPacketDGShopForceClose* subpack;
	data = Decode(subpack, data);
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvShopForceCloseDBPacket(subpack->dwOwnerID);
}

void OfflineShopShopCreateNewPacket(const char* data)
{
	offlineshop::TSubPacketDGShopCreateNew* subpack;
	data = Decode(subpack, data);
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	std::vector<offlineshop::TItemInfo> vec;
	vec.reserve(subpack->shop.dwCount);
	offlineshop::TItemInfo* pItemInfo = nullptr;
	for (DWORD i = 0; i < subpack->shop.dwCount; i++)
	{
		data = Decode(pItemInfo, data);
		vec.push_back(*pItemInfo);
	}
#ifdef ENABLE_IRA_REWORK
	rManager.RecvShopCreateNewDBPacket(subpack->shop, subpack->pos, vec);
#else
	rManager.RecvShopCreateNewDBPacket(subpack->shop, vec);
#endif
}

#ifdef ENABLE_OFFLINESHOP_EXTEND_TIME
void OfflineShopShopExtendTimePacket(const char* data)
{
	offlineshop::TSubPacketDGShopExtendTime* subpack;
	data = Decode(subpack, data);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvShopExtendTimeDBPacket(subpack->dwOwnerID , subpack->dwTime);
}
#endif

void OfflineShopShopChangeNamePacket(const char* data)
{
	offlineshop::TSubPacketDGShopChangeName* subpack;
	data = Decode(subpack, data);
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvShopChangeNameDBPacket(subpack->dwOwnerID, subpack->szName);
}

void OfflineShopSafeboxAddItemPacket(const char* data)
{
	offlineshop::TSubPacketDGSafeboxAddItem* subpack;
	data = Decode(subpack, data);
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvShopSafeboxAddItemDBPacket(subpack->dwOwnerID, subpack->dwItemID, subpack->item);
}

void OfflineShopSafeboxAddValutesPacket(const char* data)
{
	offlineshop::TSubPacketDGSafeboxAddValutes* subpack;
	data = Decode(subpack, data);
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvShopSafeboxAddValutesDBPacket(subpack->dwOwnerID, subpack->valute);
}

void OfflineShopSafeboxLoad(const char* data)
{
	offlineshop::TSubPacketDGSafeboxLoad* subpack;
	data = Decode(subpack, data);
	std::vector<DWORD> ids;
	std::vector<offlineshop::TItemInfoEx> items;
	ids.reserve(subpack->dwItemCount);
	items.reserve(subpack->dwItemCount);
	DWORD* pdwItemID = nullptr;
	offlineshop::TItemInfoEx* temp;
	for (DWORD i = 0; i < subpack->dwItemCount; i++)
	{
		data = Decode(pdwItemID, data);
		data = Decode(temp, data);
		ids.push_back(*pdwItemID);
		items.push_back(*temp);
	}
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvShopSafeboxLoadDBPacket(subpack->dwOwnerID, subpack->valute, ids, items);
}

void OfflineshopSafeboxExpiredItem(const char* data)
{
	offlineshop::TSubPacketDGSafeboxExpiredItem* subpack;
	data = Decode(subpack, data);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvShopSafeboxExpiredItemDBPacket(subpack->dwOwnerID, subpack->dwItemID);
}

void OfflineshopShopExpired(const char* data)
{
	offlineshop::TSubPacketDGShopExpired* subpack;
	data = Decode(subpack, data);
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvShopExpiredDBPacket(subpack->dwOwnerID);
}

void OfflineshopPacket(const char* data)
{
	TPacketDGNewOfflineShop* pPack = nullptr;
	data = Decode(pPack, data);

	switch (pPack->bSubHeader)
	{
	case offlineshop::SUBHEADER_DG_LOAD_TABLES:			OfflineShopLoadTables(data);				return;
	case offlineshop::SUBHEADER_DG_BUY_ITEM:			OfflineShopBuyItemPacket(data);				return;
	case offlineshop::SUBHEADER_DG_LOCKED_BUY_ITEM:		OfflineShopLockedBuyItemPacket(data);		return;
	case offlineshop::SUBHEADER_DG_EDIT_ITEM:			OfflineShopEditItemPacket(data);			return;
	case offlineshop::SUBHEADER_DG_REMOVE_ITEM:			OfflineShopRemoveItemPacket(data);			return;
	case offlineshop::SUBHEADER_DG_ADD_ITEM:			OfflineShopAddItemPacket(data);				return;
	case offlineshop::SUBHEADER_DG_SHOP_FORCE_CLOSE:	OfflineShopForceClosePacket(data);			return;
	case offlineshop::SUBHEADER_DG_SHOP_CREATE_NEW:		OfflineShopShopCreateNewPacket(data);		return;
	case offlineshop::SUBHEADER_DG_SHOP_CHANGE_NAME:	OfflineShopShopChangeNamePacket(data);		return;
	case offlineshop::SUBHEADER_DG_SHOP_EXPIRED:		OfflineshopShopExpired(data);				return;
	case offlineshop::SUBHEADER_DG_SAFEBOX_ADD_ITEM:	OfflineShopSafeboxAddItemPacket(data);		return;
	case offlineshop::SUBHEADER_DG_SAFEBOX_ADD_VALUTES:	OfflineShopSafeboxAddValutesPacket(data);	return;
	case offlineshop::SUBHEADER_DG_SAFEBOX_LOAD:		OfflineShopSafeboxLoad(data);				return;
	case offlineshop::SUBHEADER_DG_SAFEBOX_EXPIRED_ITEM:	OfflineshopSafeboxExpiredItem(data);	return;
#ifdef ENABLE_OFFLINESHOP_EXTEND_TIME
	case offlineshop::SUBHEADER_DG_SHOP_EXTEND_TIME: OfflineShopShopExtendTimePacket(data);			return;
#endif
	default:
		sys_err("UKNOWN SUB HEADER %d ", pPack->bSubHeader);
		return;
	}
}
#endif
////////////////////////////////////////////////////////////////////
// Analyze
////////////////////////////////////////////////////////////////////
int CInputDB::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	switch (bHeader)
	{
	case HEADER_DG_BOOT:
		Boot(c_pData);
		break;

	case HEADER_DG_LOGIN_SUCCESS:
		LoginSuccess(m_dwHandle, c_pData);
		break;

	case HEADER_DG_LOGIN_NOT_EXIST:
		LoginFailure(DESC_MANAGER::instance().FindByHandle(m_dwHandle), "NOID");
		break;

	case HEADER_DG_LOGIN_WRONG_PASSWD:
		LoginFailure(DESC_MANAGER::instance().FindByHandle(m_dwHandle), "WRONGPWD");
		break;

	case HEADER_DG_LOGIN_ALREADY:
		LoginAlready(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
		break;

	case HEADER_DG_PLAYER_LOAD_SUCCESS:
		PlayerLoad(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
		break;

	case HEADER_DG_PLAYER_CREATE_SUCCESS:
		PlayerCreateSuccess(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
		break;

	case HEADER_DG_PLAYER_CREATE_FAILED:
		PlayerCreateFailure(DESC_MANAGER::instance().FindByHandle(m_dwHandle), 0);
		break;

	case HEADER_DG_PLAYER_CREATE_ALREADY:
		PlayerCreateFailure(DESC_MANAGER::instance().FindByHandle(m_dwHandle), 1);
		break;

	case HEADER_DG_PLAYER_DELETE_SUCCESS:
		PlayerDeleteSuccess(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
		break;

	case HEADER_DG_PLAYER_LOAD_FAILED:
		//sys_log(0, "PLAYER_LOAD_FAILED");
		break;

	case HEADER_DG_PLAYER_DELETE_FAILED:
		//sys_log(0, "PLAYER_DELETE_FAILED");
		PlayerDeleteFail(DESC_MANAGER::instance().FindByHandle(m_dwHandle));
		break;

	case HEADER_DG_ITEM_LOAD:
		ItemLoad(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
		break;

	case HEADER_DG_QUEST_LOAD:
		QuestLoad(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
		break;

	case HEADER_DG_AFFECT_LOAD:
		AffectLoad(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
		break;

	case HEADER_DG_SAFEBOX_LOAD:
		SafeboxLoad(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
		break;

	case HEADER_DG_SAFEBOX_CHANGE_SIZE:
		SafeboxChangeSize(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
		break;
#if defined(__BL_MOVE_CHANNEL__)
	case HEADER_DG_RESPOND_MOVE_CHANNEL:
		MoveChannelRespond(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
		break;
#endif
	case HEADER_DG_SAFEBOX_WRONG_PASSWORD:
		SafeboxWrongPassword(DESC_MANAGER::instance().FindByHandle(m_dwHandle));
		break;

	case HEADER_DG_SAFEBOX_CHANGE_PASSWORD_ANSWER:
		SafeboxChangePasswordAnswer(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
		break;

	case HEADER_DG_MALL_LOAD:
		MallLoad(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
		break;

	case HEADER_DG_EMPIRE_SELECT:
		EmpireSelect(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
		break;

	case HEADER_DG_MAP_LOCATIONS:
		MapLocations(c_pData);
		break;

	case HEADER_DG_P2P:
		P2P(c_pData);
		break;

	case HEADER_DG_GUILD_SKILL_UPDATE:
		GuildSkillUpdate(c_pData);
		break;

	case HEADER_DG_GUILD_LOAD:
		GuildLoad(c_pData);
		break;

	case HEADER_DG_GUILD_SKILL_RECHARGE:
		GuildSkillRecharge();
		break;

	case HEADER_DG_GUILD_EXP_UPDATE:
		GuildExpUpdate(c_pData);
		break;

	case HEADER_DG_GUILD_REFRESH_NOTE:
		GuildRefreshNote(c_pData);
		break;

	case HEADER_DG_GUILD_NOTICE_DUNGEON_OPEN:
		GuildNoticeDungeonOpen(c_pData);
		break;

	case HEADER_DG_GUILD_NOTICE_DUNGEON_END:
		GuildNoticeDungeonEnd(c_pData);
		break;

	case HEADER_DG_GUILD_NOTICE_TICKETS_CHANGED:
		GuildNoticeTicketsChanged(c_pData);
		break;

	case HEADER_DG_GUILD_UPDATE_GUARD_ITEM_DATA:
		GuildUpdateGuardItemData(c_pData);
		break;

	case HEADER_DG_GUILD_UPDATE_GUARD_ACTIVE_SET_DATA:
		GuildGuardUpdateActiveSetData(c_pData);
		break;

	case HEADER_DG_PARTY_CREATE:
		PartyCreate(c_pData);
		break;

	case HEADER_DG_PARTY_DELETE:
		PartyDelete(c_pData);
		break;

	case HEADER_DG_PARTY_ADD:
		PartyAdd(c_pData);
		break;

	case HEADER_DG_PARTY_REMOVE:
		PartyRemove(c_pData);
		break;

	case HEADER_DG_PARTY_STATE_CHANGE:
		PartyStateChange(c_pData);
		break;

	case HEADER_DG_PARTY_SET_MEMBER_LEVEL:
		PartySetMemberLevel(c_pData);
		break;

	case HEADER_DG_TIME:
		Time(c_pData);
		break;

	case HEADER_DG_GUILD_ADD_MEMBER:
		GuildAddMember(c_pData);
		break;

	case HEADER_DG_GUILD_REMOVE_MEMBER:
		GuildRemoveMember(c_pData);
		break;

	case HEADER_DG_GUILD_CHANGE_GRADE:
		GuildChangeGrade(c_pData);
		break;

	case HEADER_DG_GUILD_CHANGE_MEMBER_DATA:
		GuildChangeMemberData(c_pData);
		break;

	case HEADER_DG_GUILD_DISBAND:
		GuildDisband(c_pData);
		break;

	case HEADER_DG_RELOAD_PROTO:
		ReloadProto(c_pData);
		break;

#ifdef __ENABLE_DAMAGE_RESTRICTIONS__
	case HEADER_DG_RELOAD_DAMAGE_RESTRICTIONS:
		ReloadMonsterRestrictions(c_pData);
		break;
#endif

	case HEADER_DG_GUILD_WAR:
		GuildWar(c_pData);
		break;

	case HEADER_DG_GUILD_WAR_SCORE:
		GuildWarScore(c_pData);
		break;

	case HEADER_DG_GUILD_LADDER:
		GuildLadder(c_pData);
		break;

	case HEADER_DG_GUILD_SKILL_USABLE_CHANGE:
		GuildSkillUsableChange(c_pData);
		break;

	case HEADER_DG_CHANGE_NAME:
		ChangeName(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
		break;

	case HEADER_DG_AUTH_LOGIN:
		AuthLogin(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
		break;

	case HEADER_DG_CHANGE_EMPIRE_PRIV:
		ChangeEmpirePriv(c_pData);
		break;

	case HEADER_DG_CHANGE_GUILD_PRIV:
		ChangeGuildPriv(c_pData);
		break;

	case HEADER_DG_CHANGE_CHARACTER_PRIV:
		ChangeCharacterPriv(c_pData);
		break;

	case HEADER_DG_MONEY_LOG:
		MoneyLog(c_pData);
		break;

	case HEADER_DG_GUILD_WITHDRAW_MONEY_GIVE:
		GuildWithdrawMoney(c_pData);
		break;

	case HEADER_DG_GUILD_MONEY_CHANGE:
		GuildMoneyChange(c_pData);
		break;

	case HEADER_DG_SET_EVENT_FLAG:
		SetEventFlag(c_pData);
		break;

	case HEADER_DG_BILLING_REPAIR:
		BillingRepair(c_pData);
		break;

	case HEADER_DG_BILLING_EXPIRE:
		BillingExpire(c_pData);
		break;

	case HEADER_DG_BILLING_LOGIN:
		BillingLogin(c_pData);
		break;

	case HEADER_DG_BILLING_CHECK:
		BillingCheck(c_pData);
		break;

	case HEADER_DG_VCARD:
		VCard(c_pData);
		break;

	case HEADER_DG_CREATE_OBJECT:
		CreateObject(c_pData);
		break;

	case HEADER_DG_DELETE_OBJECT:
		DeleteObject(c_pData);
		break;

	case HEADER_DG_UPDATE_LAND:
		UpdateLand(c_pData);
		break;

	case HEADER_DG_NOTICE:
		Notice(c_pData);
		break;

	case HEADER_DG_GUILD_WAR_RESERVE_ADD:
		GuildWarReserveAdd((TGuildWarReserve *) c_pData);
		break;

	case HEADER_DG_GUILD_WAR_RESERVE_DEL:
		GuildWarReserveDelete(*(DWORD *) c_pData);
		break;

	case HEADER_DG_GUILD_WAR_BET:
		GuildWarBet((TPacketGDGuildWarBet *) c_pData);
		break;

	case HEADER_DG_MARRIAGE_ADD:
		MarriageAdd((TPacketMarriageAdd*) c_pData);
		break;

	case HEADER_DG_MARRIAGE_UPDATE:
		MarriageUpdate((TPacketMarriageUpdate*) c_pData);
		break;

	case HEADER_DG_MARRIAGE_REMOVE:
		MarriageRemove((TPacketMarriageRemove*) c_pData);
		break;

	case HEADER_DG_WEDDING_REQUEST:
		WeddingRequest((TPacketWeddingRequest*) c_pData);
		break;
		
#ifdef ENABLE_ITEMSHOP
	case HEADER_DG_ITEMSHOP:
	{
		ItemShop(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
		break;
	}
#endif

	case HEADER_DG_WEDDING_READY:
		WeddingReady((TPacketWeddingReady*) c_pData);
		break;

	case HEADER_DG_WEDDING_START:
		WeddingStart((TPacketWeddingStart*) c_pData);
		break;

	case HEADER_DG_WEDDING_END:
		WeddingEnd((TPacketWeddingEnd*) c_pData);
		break;

		// MYSHOP_PRICE_LIST
	case HEADER_DG_MYSHOP_PRICELIST_RES:
		MyshopPricelistRes(DESC_MANAGER::instance().FindByHandle(m_dwHandle), (TPacketMyshopPricelistHeader*) c_pData );
		break;
		// END_OF_MYSHOP_PRICE_LIST

	// RELOAD_ADMIN
	case HEADER_DG_RELOAD_ADMIN:
		ReloadAdmin(c_pData );
		break;
	//END_RELOAD_ADMIN

	case HEADER_DG_RET_USER_COUNT:
		UserCount(DESC_MANAGER::instance().FindByHandle(m_dwHandle), (TPacketDGUserCount*)c_pData);
		break;

#ifdef ENABLE_EVENT_MANAGER
	case HEADER_DG_EVENT_MANAGER:
		EventManager(c_pData);
		break;
#endif


	case HEADER_DG_ADD_MONARCH_MONEY:
		AddMonarchMoney(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData );
		break;

	case HEADER_DG_DEC_MONARCH_MONEY:
		DecMonarchMoney(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData );
		break;

	case HEADER_DG_TAKE_MONARCH_MONEY:
		TakeMonarchMoney(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData );
		break;

	case HEADER_DG_CHANGE_MONARCH_LORD_ACK :
		ChangeMonarchLord((TPacketChangeMonarchLordACK*)c_pData);
		break;

	case HEADER_DG_UPDATE_MONARCH_INFO :
		UpdateMonarchInfo((TMonarchInfo*)c_pData);
		break;

	case HEADER_DG_ACK_CHANGE_GUILD_MASTER :
		this->GuildChangeMaster((TPacketChangeGuildMaster*) c_pData);
		break;
	case HEADER_DG_ACK_SPARE_ITEM_ID_RANGE :
		ITEM_MANAGER::instance().SetMaxSpareItemID(*((TItemIDRangeTable*)c_pData) );
		break;

	case HEADER_DG_UPDATE_HORSE_NAME :
	case HEADER_DG_ACK_HORSE_NAME :
		CHorseNameManager::instance().UpdateHorseName(
				((TPacketUpdateHorseName*)c_pData)->dwPlayerID,
				((TPacketUpdateHorseName*)c_pData)->szHorseName);
		break;
#ifdef ENABLE_NEW_PET_SYSTEM
	case HEADER_DG_UPDATE_PET_NAME:
	case HEADER_DG_ACK_PET_NAME:
		CHorseNameManager::instance().UpdatePetName(
				((TPacketUpdateHorseName*)c_pData)->dwPlayerID,
				((TPacketUpdateHorseName*)c_pData)->szHorseName);
		break;
#endif
	case HEADER_DG_NEED_LOGIN_LOG:
		DetailLog( (TPacketNeedLoginLogInfo*) c_pData );
		break;

	case HEADER_DG_ITEMAWARD_INFORMER:
		ItemAwardInformer((TPacketItemAwardInfromer*) c_pData);
		break;
#if defined(BL_OFFLINE_MESSAGE)
	case HEADER_DG_RESPOND_OFFLINE_MESSAGES:
		ReadOfflineMessages(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
		break;
#endif
	case HEADER_DG_RESPOND_CHANNELSTATUS:
		RespondChannelStatus(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
		break;
#ifdef ENABLE_OFFLINE_SHOP
	case HEADER_DG_NEW_OFFLINESHOP:
		OfflineshopPacket(c_pData);
		break;
#endif
#ifdef ENABLE_IN_GAME_LOG_SYSTEM
	case HEADER_DG_IN_GAME_LOG:
		InGameLog::GetManager().RecvInGameLogPacketDG(c_pData);
		break;
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
	case HEADER_DG_PRIVATE_SHOP:
		PrivateShop(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
		break;
#endif

#ifdef RANKING_SYSTEM
	case HEADER_DG_SERVER_RANKING_LOAD:
		ServerRankingLoad(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
		break;
	case HEADER_DG_SERVER_RANKING_LOAD_MY_POS:
		ServerRankingLoadMyPos(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
		break;
	case HEADER_DG_RANKING_LOAD:
		RankingLoad(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
		break;
	case HEADER_DG_RANKING_LOAD_MY_POS:
		RankingLoadMyPos(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
		break;
	case HEADER_DG_RANKING_DATA_STATUS:
		RankingLoadDataStatus(c_pData);
		break;
#endif
	case HEADER_DG_FISH_VNUM_RANKING_LOAD:
		FishVnumRankingLoad(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
		break;


#ifdef ENABLE_EXTENDED_BATTLE_PASS
	case HEADER_DG_EXT_BATTLE_PASS_LOAD:
		ExtBattlePassLoad(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
		break;
#endif
#ifdef __DUNGEON_INFO_ENABLE__
	case HEADER_DG_DUNGEONINFO_RANKING:
		DIHighscoreRecord(c_pData);
		break;
#endif

	default:
		return (-1);
	}

	return 0;
}

bool CInputDB::Process(LPDESC d, const void * orig, int bytes, int & r_iBytesProceed)
{
	const char *	c_pData = (const char *) orig;
	BYTE		bHeader, bLastHeader = 0;
	int			iSize;
	int			iLastPacketLen = 0;

	for (m_iBufferLeft = bytes; m_iBufferLeft > 0;)
	{
		if (m_iBufferLeft < 9)
			return true;

		bHeader		= *((BYTE *) (c_pData));	// 1
		m_dwHandle	= *((DWORD *) (c_pData + 1));	// 4
		iSize		= *((DWORD *) (c_pData + 5));	// 4

		sys_log(1, "DBCLIENT: header %d handle %d size %d bytes %d", bHeader, m_dwHandle, iSize, bytes);

		if (m_iBufferLeft - 9 < iSize)
			return true;

		const char * pRealData = (c_pData + 9);

		if (Analyze(d, bHeader, pRealData) < 0)
		{
			sys_err("in InputDB: UNKNOWN HEADER: %d, LAST HEADER: %d(%d), REMAIN BYTES: %d, DESC: %d",
					bHeader, bLastHeader, iLastPacketLen, m_iBufferLeft, d->GetSocket());

			//printdata((BYTE*) orig, bytes);
			//d->SetPhase(PHASE_CLOSE);
		}

		c_pData		+= 9 + iSize;
		m_iBufferLeft	-= 9 + iSize;
		r_iBytesProceed	+= 9 + iSize;

		iLastPacketLen	= 9 + iSize;
		bLastHeader	= bHeader;
	}

	return true;
}

void CInputDB::AddMonarchMoney(LPDESC d, const char * data )
{
	int Empire = *(int *) data;
	data += sizeof(int);

	int Money = *(int *) data;
	data += sizeof(int);

	CMonarch::instance().AddMoney(Money, Empire);

	DWORD pid = CMonarch::instance().GetMonarchPID(Empire);

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(pid);

}

void CInputDB::DecMonarchMoney(LPDESC d, const char * data)
{
	int Empire = *(int *) data;
	data += sizeof(int);

	int Money = *(int *) data;
	data += sizeof(int);

	CMonarch::instance().DecMoney(Money, Empire);

	DWORD pid = CMonarch::instance().GetMonarchPID(Empire);

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(pid);

	if (ch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;784;%s;%d]", EMPIRE_NAME(Empire), CMonarch::instance().GetMoney(Empire));
	}
}

void CInputDB::TakeMonarchMoney(LPDESC d, const char * data)
{
	int Empire = *(int *) data;
	data += sizeof(int);

	int Money = *(int *) data;
	data += sizeof(int);

	if (!CMonarch::instance().DecMoney(Money, Empire))
	{
		if (!d)
			return;

		if (!d->GetCharacter())
			return;

		LPCHARACTER ch = d->GetCharacter();
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;785]");
	}
}

void CInputDB::ChangeMonarchLord(TPacketChangeMonarchLordACK* info)
{
	char notice[256];
	snprintf(notice, sizeof(notice), "[LS;1015;%s;%s]", EMPIRE_NAME(info->bEmpire), info->szName);
	SendNotice(notice);
}

void CInputDB::UpdateMonarchInfo(TMonarchInfo* info)
{
	CMonarch::instance().SetMonarchInfo(info);
	sys_log(0, "MONARCH INFO UPDATED");
}

void CInputDB::GuildChangeMaster(TPacketChangeGuildMaster* p)
{
	CGuildManager::instance().ChangeMaster(p->dwGuildID);
}

void CInputDB::DetailLog(const TPacketNeedLoginLogInfo* info)
{
	LPCHARACTER pChar = CHARACTER_MANAGER::instance().FindByPID( info->dwPlayerID );

	if (NULL != pChar)
	{
		LogManager::instance().DetailLoginLog(true, pChar);
	}
}

void CInputDB::ItemAwardInformer(TPacketItemAwardInfromer *data)
{
	LPDESC d = DESC_MANAGER::instance().FindByLoginName(data->login);

	if(d == NULL)
		return;
	else
	{
		if (d->GetCharacter())
		{
			LPCHARACTER ch = d->GetCharacter();
			ch->SetItemAward_vnum(data->vnum);
			ch->SetItemAward_cmd(data->command);

			if(d->IsPhase(PHASE_GAME))
			{
				quest::CQuestManager::instance().ItemInformer(ch->GetPlayerID(),ch->GetItemAward_vnum());
			}
		}
	}
}

void CInputDB::RespondChannelStatus(LPDESC desc, const char* pcData)
{
	if (!desc) {
		return;
	}
	const int nSize = decode_4bytes(pcData);
	pcData += sizeof(nSize);

	BYTE bHeader = HEADER_GC_RESPOND_CHANNELSTATUS;
	desc->BufferedPacket(&bHeader, sizeof(BYTE));
	desc->BufferedPacket(&nSize, sizeof(nSize));
	if (0 < nSize) {
		desc->BufferedPacket(pcData, sizeof(TChannelStatus)*nSize);
	}
	BYTE bSuccess = 1;
	desc->Packet(&bSuccess, sizeof(bSuccess));
	desc->SetChannelStatusRequested(false);
}

#if defined(__BL_MOVE_CHANNEL__)
void CInputDB::MoveChannelRespond(LPDESC d, const char* c_pData)
{
	if (d == nullptr)
		return;

	const LPCHARACTER ch = d->GetCharacter();
	if (ch == nullptr)
		return;

	ch->MoveChannel(reinterpret_cast<const TRespondMoveChannel*>(c_pData));
}
#endif

#ifdef ENABLE_EVENT_MANAGER
void CInputDB::EventManager(const char* c_pData)
{
	CHARACTER_MANAGER& chrMngr = CHARACTER_MANAGER::Instance();
	const BYTE subIndex = *(BYTE*)c_pData;
	c_pData += sizeof(BYTE);
	if (subIndex == EVENT_MANAGER_LOAD)
	{
		chrMngr.ClearEventData();
		const BYTE dayCount = *(BYTE*)c_pData;
		c_pData += sizeof(BYTE);

		const bool updateFromGameMaster = *(bool*)c_pData;
		c_pData += sizeof(bool);

		for (DWORD x = 0; x < dayCount; ++x)
		{
			const BYTE dayIndex = *(BYTE*)c_pData;
			c_pData += sizeof(BYTE);

			const BYTE dayEventCount = *(BYTE*)c_pData;
			c_pData += sizeof(BYTE);

			if (dayEventCount > 0)
			{
				std::vector<TEventManagerData> m_vec;
				m_vec.resize(dayEventCount);
				thecore_memcpy(&m_vec[0], c_pData, dayEventCount*sizeof(TEventManagerData));
				c_pData += dayEventCount * sizeof(TEventManagerData);
				chrMngr.SetEventData(dayIndex, m_vec);

			}
		}
		if (updateFromGameMaster)
			chrMngr.UpdateAllPlayerEventData();
	}
	else if (EVENT_MANAGER_EVENT_STATUS == subIndex)
	{
		const WORD& eventID = *(WORD*)c_pData;
		c_pData += sizeof(WORD);
		const bool& eventStatus = *(bool*)c_pData;
		c_pData += sizeof(bool);
		const int& endTime = *(int*)c_pData;
		c_pData += sizeof(int);
		char endTimeText[25];
		strlcpy(endTimeText, c_pData, sizeof(endTimeText));
		c_pData += sizeof(endTimeText);
		chrMngr.SetEventStatus(eventID, eventStatus, endTime, endTimeText);
	}
}
#endif

#if defined(BL_OFFLINE_MESSAGE)
#include "buffer_manager.h"
void CInputDB::ReadOfflineMessages(LPDESC desc, const char* pcData)
{
	if (!desc || !desc->GetCharacter())
		return;

	if (desc->GetCharacter()->IsBlockMode(BLOCK_WHISPER))
		return;
	
	auto p = reinterpret_cast<const TPacketDGReadOfflineMessage*>(pcData);

	TPacketGCWhisper pack;
	int len = MIN(CHAT_MAX_LEN, strlen(p->szMessage) + 1);
	pack.bHeader = HEADER_GC_WHISPER;
	pack.wSize = static_cast<WORD>(sizeof(TPacketGCWhisper) + len);
	pack.bType = WHISPER_TYPE_OFFLINE;
	strlcpy(pack.szNameFrom, p->szFrom, sizeof(pack.szNameFrom));

	TEMP_BUFFER buf;
	buf.write(&pack, sizeof(TPacketGCWhisper));
	buf.write(p->szMessage, len);
	desc->Packet(buf.read_peek(), buf.size());
}
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
void CInputDB::PrivateShop(LPDESC d, const char* c_pData)
{
	const BYTE bSubHeader = *reinterpret_cast<const BYTE*>(c_pData);
	c_pData += sizeof(BYTE);

	switch (bSubHeader)
	{
		case PRIVATE_SHOP_DG_SUBHEADER_CREATE_RESULT:
		{
			TPacketDGPrivateShopCreateResult* p = (TPacketDGPrivateShopCreateResult*)c_pData;

			CPrivateShopManager::Instance().BuildPrivateShopResult(p->privateShopTable.dwOwner, &p->privateShopTable, p->bSuccess);
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_NO_SHOP:
		{
			if (d && d->GetCharacter())
				d->GetCharacter()->ChatPacket(CHAT_TYPE_INFO, "You do not have an open personal shop.");
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_CLOSE_RESULT_BALANCE_AVAILABLE:
		{
			if (d && d->GetCharacter())
				d->GetCharacter()->ChatPacket(CHAT_TYPE_INFO, "You must withdraw your earnings before you can close your personal shop.");
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_DESTROY:
		{
			if (d && d->GetCharacter())
				CPrivateShopManager::Instance().ClosePrivateShop(d->GetCharacter());
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_SPAWN:
		{
			TPrivateShop* pPrivateShopTable = (TPrivateShop*)c_pData;
			c_pData += sizeof(TPrivateShop);

			const WORD wCount = *reinterpret_cast<const WORD*>(c_pData);
			c_pData += sizeof(WORD);

			TPlayerPrivateShopItem* pShopItem = (TPlayerPrivateShopItem*)c_pData;

			std::vector<TPlayerPrivateShopItem> vec_shopItem;
			for (int i = 0; i < wCount; ++i, ++pShopItem)
				vec_shopItem.push_back(*pShopItem);

			CPrivateShopManager::Instance().SpawnPrivateShop(pPrivateShopTable, vec_shopItem);

		}break;

		case PRIVATE_SHOP_DG_SUBHEADER_DESPAWN:
		{
			const DWORD dwPID = *reinterpret_cast<const DWORD*>(c_pData);

			LPPRIVATE_SHOP pPrivateShop = CPrivateShopManager::Instance().GetPrivateShop(dwPID);
			if (!pPrivateShop)
				return;

			LogManager::Instance().CharLog(dwPID, 0, 0, 0, "PRIVATE SHOP DESPAWN", "", "");
			CPrivateShopManager::Instance().DeletePrivateShop(dwPID);
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_LOAD:
		{
			if (!d || !d->GetCharacter())
				return;

			TPrivateShop* pPrivateShopTable = (TPrivateShop*)c_pData;
			d->GetCharacter()->SetPrivateShopTable(*pPrivateShopTable);

		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_ITEM_LOAD:
		{
			if (!d || !d->GetCharacter())
				return;

			const WORD wCount = *reinterpret_cast<const WORD*>(c_pData);
			c_pData += sizeof(WORD);

			TPlayerPrivateShopItem* pShopItem = (TPlayerPrivateShopItem*)c_pData;

			for (int i = 0; i < wCount; ++i, ++pShopItem)
			{
				d->GetCharacter()->SetPrivateShopItem(*pShopItem);
			}
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_SALE_LOAD:
		{
			if (!d || !d->GetCharacter())
				return;

			const WORD wCount = *reinterpret_cast<const WORD*>(c_pData);
			c_pData += sizeof(WORD);

			TPrivateShopSale* pShopSale = (TPrivateShopSale*)c_pData;

			for (int i = 0; i < wCount; ++i, ++pShopSale)
			{
				d->GetCharacter()->SetPrivateShopSale(*pShopSale);
			}
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_BUY_RESULT_FALSE_ITEM:
		case PRIVATE_SHOP_DG_SUBHEADER_ITEM_CHECKIN_FALSE_ITEM:
		{
			if (d && d->GetCharacter())
				d->GetCharacter()->ChatPacket(CHAT_TYPE_INFO, "This item is currently unavailable.");
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_BUY_RESULT_FALSE_PRICE:
		{
			if (d && d->GetCharacter())
				d->GetCharacter()->ChatPacket(CHAT_TYPE_INFO, "Refresh your search result to sync price of the item.");
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_BUY_RESULT_MODIFY_STATE:
		{
			if (d && d->GetCharacter())
				d->GetCharacter()->ChatPacket(CHAT_TYPE_INFO, "You cannot buy an item while a personal shop is in a modifying state.");
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_BUY_RESULT_NO_GOLD:
		{
			if (d && d->GetCharacter())
				d->GetCharacter()->ChatPacket(CHAT_TYPE_INFO, "You don't have enough Yang.");
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_BUY_RESULT_NO_CHEQUE:
		{
			if (d && d->GetCharacter())
				d->GetCharacter()->ChatPacket(CHAT_TYPE_INFO, "You don't have enough Won.");
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_BUY_REQUEST:
		{
			TPacketDGPrivateShopBuyRequest* p = (TPacketDGPrivateShopBuyRequest*)c_pData;

			if (d && d->GetCharacter())
				CPrivateShopManager::Instance().ItemTransaction(d->GetCharacter(), &p->arRequestedItems[0]);
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_REMOVE_ITEM:
		{
			const DWORD dwShopID = *reinterpret_cast<const DWORD*>(c_pData);
			c_pData += sizeof(DWORD);

			const WORD wPos = *reinterpret_cast<const WORD*>(c_pData);

			LPPRIVATE_SHOP pPrivateShop = CPrivateShopManager::Instance().GetPrivateShop(dwShopID);
			if (!pPrivateShop)
				return;

			pPrivateShop->RemoveItem(wPos);
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_ADD_ITEM:
		{
			const DWORD dwShopID = *reinterpret_cast<const DWORD*>(c_pData);
			c_pData += sizeof(DWORD);

			const TPlayerPrivateShopItem TPrivateShopItem = *(TPlayerPrivateShopItem*)c_pData;

			LPPRIVATE_SHOP pPrivateShop = CPrivateShopManager::Instance().GetPrivateShop(dwShopID);
			if (!pPrivateShop)
				return;

			pPrivateShop->ItemCheckin(TPrivateShopItem);
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_SALE_UPDATE:
		{
			const TPrivateShopSale* pPrivateShopSale = (TPrivateShopSale*)c_pData;

			LPCHARACTER pOwner = CHARACTER_MANAGER::Instance().FindByPID(pPrivateShopSale->dwOwner);
			if (pOwner)
				pOwner->SaleUpdate(pPrivateShopSale);
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_STATE_UPDATE:
		{
			const TPacketDGPrivateShopStateUpdate* p = (TPacketDGPrivateShopStateUpdate*)c_pData;

			if (d && d->GetCharacter())
				d->GetCharacter()->SetPrivateShopState(p->bState, true);

			LPPRIVATE_SHOP pPrivateShop = CPrivateShopManager::Instance().GetPrivateShop(p->dwPID);
			if (pPrivateShop)
				pPrivateShop->SetState(p->bState);
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_WITHDRAW_RESULT_NO_BALANCE:
		{
			if (d && d->GetCharacter())
				d->GetCharacter()->ChatPacket(CHAT_TYPE_INFO, "There is no money to collect.");
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_NOT_MODIFY_STATE:
		{
			if (d && d->GetCharacter())
				d->GetCharacter()->ChatPacket(CHAT_TYPE_INFO, "You cannot manage personal shop's content while it is not in a modifying state.");
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_WITHDRAW:
		{
			const TPacketDGPrivateShopWithdraw* p = (TPacketDGPrivateShopWithdraw*)c_pData;

			if (d && d->GetCharacter())
				d->GetCharacter()->WithdrawPrivateShop(p->llGold, p->dwCheque);
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_ITEM_PRICE_CHANGE:
		{
			const TPacketPrivateShopItemPriceChange* p = (TPacketPrivateShopItemPriceChange*)c_pData;

			if (d && d->GetCharacter())
				d->GetCharacter()->ChangePrivateShopItemPrice(p->wPos, p->TPrice.llGold, p->TPrice.dwCheque);

			LPPRIVATE_SHOP pPrivateShop = CPrivateShopManager::Instance().GetPrivateShop(p->dwShopID);
			if (pPrivateShop)
				pPrivateShop->ChangeItemPrice(p->wPos, p->TPrice.llGold, p->TPrice.dwCheque);
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_ITEM_MOVE:
		{
			const TPacketPrivateShopItemMove* p = (TPacketPrivateShopItemMove*)c_pData;

			if (d && d->GetCharacter())
				d->GetCharacter()->ChangePrivateShopItemPos(p->wPos, p->wChangePos);

			LPPRIVATE_SHOP pPrivateShop = CPrivateShopManager::Instance().GetPrivateShop(p->dwShopID);
			if (pPrivateShop)
				pPrivateShop->MoveItem(p->wPos, p->wChangePos);
		}; break;

		case PRIVATE_SHOP_DG_SUBHEADER_CANNOT_MOVE_ITEM:
		{
			if (!d || !d->GetCharacter())
				return;

			d->GetCharacter()->ChatPacket(CHAT_TYPE_INFO, "You cannot move the item to that position.");
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_ITEM_CHECKIN_REQ:
		{
			const TPacketDGPrivateShopItemCheckin* p = (TPacketDGPrivateShopItemCheckin*)c_pData;

			if (d && d->GetCharacter())
			{
				CPrivateShopManager::Instance().ItemCheckin(d->GetCharacter(), &p->TItem);
			}

		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_ITEM_CHECKOUT_REQ:
		{
			const TPacketDGPrivateShopItemCheckout* p = (TPacketDGPrivateShopItemCheckout*)c_pData;

			if (d && d->GetCharacter())
			{
				CPrivateShopManager::Instance().ItemCheckout(d->GetCharacter(), p->wSrcPos, p->TDstPos);
			}
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_ITEM_EXPIRE:
		{
			const WORD wPos = *reinterpret_cast<const WORD*>(c_pData);

			if (d && d->GetCharacter())
			{
				// Notify the player and remove the private shop item
				d->GetCharacter()->ItemExpireUpdate(wPos);
			}
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_SHOP_NOT_AVAILABLE:
		{
			if (!d || !d->GetCharacter())
				return;

			d->GetCharacter()->ChatPacket(CHAT_TYPE_INFO, "Your personal shop is currently unavailable.");
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_TITLE_CHANGE:
		{
			const TPacketPrivateShopTitleChange* p = (TPacketPrivateShopTitleChange*)c_pData;

			if (d && d->GetCharacter())
				d->GetCharacter()->ChangePrivateShopTitle(p->szTitle);

			LPPRIVATE_SHOP pPrivateShop = CPrivateShopManager::Instance().GetPrivateShop(p->dwPID);
			if (pPrivateShop)
				pPrivateShop->ChangeTitle(p->szTitle);

		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_WARP:
		{
			const TPacketDGPrivateShopWarp* p = (TPacketDGPrivateShopWarp*)c_pData;

			if (d && d->GetCharacter())
				d->GetCharacter()->WarpToPrivateShop(p->lAddr, p->wListenPort);
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_UNLOCK_SLOT_RES:
		{
			const TPacketGDPrivateShopSlotUnlockRes* p = (TPacketGDPrivateShopSlotUnlockRes*)c_pData;

			if (d && d->GetCharacter())
			{
				d->GetCharacter()->SetPoint(POINT_PRIVATE_SHOP_UNLOCKED_SLOT, 0);
				d->GetCharacter()->PointChange(POINT_PRIVATE_SHOP_UNLOCKED_SLOT, p->wUnlockedSlots);
				d->GetCharacter()->RemoveSpecifyItem(PRIVATE_SHOP_SLOT_UNLOCK_ITEM, 1);

				d->GetCharacter()->ChatPacket(CHAT_TYPE_INFO, "You have increased available space on your personal shop.");
			}

			LPPRIVATE_SHOP pPrivateShop = CPrivateShopManager::Instance().GetPrivateShop(p->dwShopID);
			if (pPrivateShop)
				pPrivateShop->SetUnlockedSlots(p->wUnlockedSlots);
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_CLOSE:
		{
			if (d && d->GetCharacter())
				d->GetCharacter()->ClosePrivateShop();
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_MARKET_ITEM_PRICE_DATA_UPDATE:
		{
			const WORD wCount = *reinterpret_cast<const WORD*>(c_pData);
			c_pData += sizeof(WORD);

			TMarketItemPrice* pMarketItemPrice = (TMarketItemPrice*)c_pData;

			for (WORD i = 0; i < wCount; ++i, ++pMarketItemPrice)
				CPrivateShopManager::Instance().AddMarketItemPrice(pMarketItemPrice->dwVnum, pMarketItemPrice->TPrice);
		} break;

		case PRIVATE_SHOP_DG_SUBHEADER_NO_AVAILABLE_SPACE:
		{
			if (!d || !d->GetCharacter())
				return;

			d->GetCharacter()->ChatPacket(CHAT_TYPE_INFO, "You cannot place any more items in your personal shop.");
		} break;
	}
}
#endif

#ifdef __DUNGEON_INFO_ENABLE__
void CInputDB::DIHighscoreRecord(const char* c_pData)
{
	CDungeonInfoManager::instance().RecvHighscoreRecord((TPacketGDDIRanking *) c_pData);
}
#endif

#ifdef RANKING_SYSTEM
void CInputDB::ServerRankingLoad(LPDESC d, const char* c_pData)
{
	LPCHARACTER ch;
	if (!d || !(ch = d->GetCharacter()))
		return;

	CServerRankingManager::instance().OnLoadServerRankingFromDB(ch, c_pData);
}

void CInputDB::ServerRankingLoadMyPos(LPDESC d, const char* c_pData)
{
	LPCHARACTER ch;
	if (!d || !(ch = d->GetCharacter()))
		return;

	CServerRankingManager::instance().OnLoadServerRankingMyPosFromDB(ch, c_pData);
}

void CInputDB::RankingLoad(LPDESC d, const char* c_pData)
{
	LPCHARACTER ch;
	if (!d || !(ch = d->GetCharacter()))
		return;

	CRankingManager::instance().OnLoadRankingFromDB(ch, c_pData);
}

void CInputDB::RankingLoadMyPos(LPDESC d, const char* c_pData)
{
	LPCHARACTER ch;
	if (!d || !(ch = d->GetCharacter()))
		return;

	CRankingManager::instance().OnLoadRankingMyPosFromDB(ch, c_pData);
}

struct FRankDeleter
{
	FRankDeleter() {}
	void operator() (LPCHARACTER ch) { ch->DeleteAllRankingData(); }
};

void CInputDB::RankingLoadDataStatus(const char* c_pData)
{
	bool isEnabled = decode_byte(c_pData);
	c_pData += sizeof(bool);
	bool deleteAll = decode_byte(c_pData);
	c_pData += sizeof(bool);

	if (deleteAll)
	{
		CHARACTER_MANAGER::instance().for_each_pc(FRankDeleter());
		CRankingManager::instance().ResetRefreshTimes();
	}

	isRankingSendEnabled = isEnabled;
}
#endif

void CInputDB::FishVnumRankingLoad(LPDESC d, const char* c_pData)
{
	LPCHARACTER ch;
	if (!d || !(ch = d->GetCharacter()))
		return;

	CFishWikiManager::instance().OnLoadFishVnumRankingFromDB(ch, c_pData);
}

#ifdef ENABLE_ITEMSHOP
void CInputDB::ItemShop(LPDESC d, const char* c_pData)
{
	const BYTE subIndex = *(BYTE*)c_pData;
	c_pData += sizeof(BYTE);

	switch (subIndex)
	{
		case ITEMSHOP_LOAD:
		{
			CHARACTER_MANAGER::Instance().LoadItemShopData(c_pData);
			break;
		}
		case ITEMSHOP_LOG:
		{
			if (!d)
			{
				return;
			}

			CHARACTER_MANAGER::Instance().LoadItemShopLogReal(d->GetCharacter(), c_pData);
			break;
		}
		case ITEMSHOP_BUY:
		{
			if (!d)
			{
				return;
			}

			CHARACTER_MANAGER::Instance().LoadItemShopBuyReal(d->GetCharacter(), c_pData);
			break;
		}
		case ITEMSHOP_UPDATE_ITEM:
		{
			CHARACTER_MANAGER::Instance().UpdateItemShopItem(c_pData);
			break;
		}
		default:
		{
			sys_err("unknown subIndex %u", subIndex);
			return;
		}
	}
}
#endif

#ifdef __ENABLE_DAMAGE_RESTRICTIONS__
void CInputDB::ReloadMonsterRestrictions(const char* c_pData)
{
	WORD wSize = decode_2bytes(c_pData);
	c_pData += sizeof(WORD);

	if (wSize == 0)
	{
		sys_err("CInputDB::ReloadMonsterRestrictions -> Failed Packet size is 0...");
		return;
	}

	CMobManager::instance().InitializeRestrictions(reinterpret_cast<TMonsterRestriction*>(const_cast<char*>(c_pData)), wSize);
}
#endif
