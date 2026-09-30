#include "stdafx.h"
#include "../../common/billing.h"
#include "config.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "p2p.h"
#include "guild.h"
#include "guild_manager.h"
#include "party.h"
#include "messenger_manager.h"
#include "empire_text_convert.h"
#include "unique_item.h"
#include "xmas_event.h"
#include "affect.h"
#include "castle.h"
#include "locale_service.h"
#include "questmanager.h"
#include "skill.h"
#include "threeway_war.h"
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	#include "offlineshop_manager.h"
	#include "sectree_manager.h"
#endif
#include "VersionManager.hpp"

#ifdef __CROSS_CHANNEL_DUNGEON_WARP__
	#include "dungeon.h"
	#include "sectree_manager.h"
#endif
#ifdef __PREMIUM_PRIVATE_SHOP__
#include "private_shop_manager.h"
#include "private_shop.h"
#include "buffer_manager.h"
#endif
#ifdef __DUNGEON_INFO_ENABLE__
#include "DungeonInfoManager.hpp"
#endif

#ifdef __TEAM_MEMBER_STATUS__
#include "TeamList.hpp"
#endif
#include "MonsterSpawner.hpp"
#include "input.h"
#include "cmd.h"
CPacketInfoGG CInputP2P::m_packetInfoGG;

CInputP2P::CInputP2P()
{
	BindPacketInfo(&m_packetInfoGG);
}

void CInputP2P::Login(LPDESC d, const char * c_pData)
{
	P2P_MANAGER::instance().Login(d, (TPacketGGLogin *) c_pData);
}

#ifdef ENABLE_MESSENGER_BLOCK
void CInputP2P::MessengerBlockAdd(const char* c_pData)
{
	TPacketGGMessenger* p = (TPacketGGMessenger*)c_pData;
	MessengerManager::instance().__AddToBlockList(p->szAccount, p->szCompanion);
}

void CInputP2P::MessengerBlockRemove(const char* c_pData)
{
	TPacketGGMessenger* p = (TPacketGGMessenger*)c_pData;
	MessengerManager::instance().__RemoveFromBlockList(p->szAccount, p->szCompanion);
}
#endif

void CInputP2P::Logout(LPDESC d, const char * c_pData)
{
	TPacketGGLogout * p = (TPacketGGLogout *) c_pData;
	P2P_MANAGER::instance().Logout(p->szName);
}

int CInputP2P::Relay(LPDESC d, const char * c_pData, size_t uiBytes)
{
	TPacketGGRelay * p = (TPacketGGRelay *) c_pData;

	if (uiBytes < sizeof(TPacketGGRelay) + p->lSize)
		return -1;

	if (p->lSize < 0)
	{
		sys_err("invalid packet length %d", p->lSize);
		d->SetPhase(PHASE_CLOSE);
		return -1;
	}

	sys_log(0, "InputP2P::Relay : %s size %d", p->szName, p->lSize);

	LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindPC(p->szName);

	const BYTE* c_pbData = (const BYTE *) (c_pData + sizeof(TPacketGGRelay));

	if (!pkChr)
		return p->lSize;

	if (*c_pbData == HEADER_GC_WHISPER)
	{
		if (pkChr->IsBlockMode(BLOCK_WHISPER))
		{
			return p->lSize;
		}

		char buf[1024];
		int iCopySize = MIN(p->lSize, (int)sizeof(buf));
		memcpy(buf, c_pbData, iCopySize);

		TPacketGCWhisper* p2 = (TPacketGCWhisper*) buf;

		if (p2->wSize < sizeof(TPacketGCWhisper))
		{
			sys_err("P2P Whisper: invalid wSize %d", p2->wSize);
			return p->lSize;
		}

		BYTE bToEmpire = (p2->bType >> 4);
		p2->bType = p2->bType & 0x0F;
		if(p2->bType == 0x0F) {
			p2->bType = WHISPER_TYPE_SYSTEM;
		} else {
			if (!pkChr->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE))
				if (bToEmpire >= 1 && bToEmpire <= 3 && pkChr->GetEmpire() != bToEmpire)
				{
					ConvertEmpireText(bToEmpire,
							buf + sizeof(TPacketGCWhisper),
							p2->wSize - sizeof(TPacketGCWhisper),
							10+2*pkChr->GetSkillPower(SKILL_LANGUAGE1 + bToEmpire - 1));
				}
		}

		pkChr->GetDesc()->Packet(buf, iCopySize);
	}
	else
		pkChr->GetDesc()->Packet(c_pbData, p->lSize);

	return (p->lSize);
}

#ifdef ENABLE_FULL_NOTICE
int CInputP2P::Notice(LPDESC d, const char * c_pData, size_t uiBytes, bool bBigFont)
#else
int CInputP2P::Notice(LPDESC d, const char * c_pData, size_t uiBytes)
#endif
{
	TPacketGGNotice * p = (TPacketGGNotice *) c_pData;

	if (uiBytes < sizeof(TPacketGGNotice) + p->lSize)
		return -1;

	if (p->lSize < 0)
	{
		sys_err("invalid packet length %d", p->lSize);
		d->SetPhase(PHASE_CLOSE);
		return -1;
	}

	char szBuf[256+1];
	strlcpy(szBuf, c_pData + sizeof(TPacketGGNotice), MIN(p->lSize + 1, sizeof(szBuf)));
#ifdef ENABLE_FULL_NOTICE
	SendNotice(szBuf, bBigFont);
#else
	SendNotice(szBuf);
#endif
	return (p->lSize);
}

int CInputP2P::MonarchNotice(LPDESC d, const char * c_pData, size_t uiBytes)
{
	TPacketGGMonarchNotice * p = (TPacketGGMonarchNotice *) c_pData;

	if (uiBytes < p->lSize + sizeof(TPacketGGMonarchNotice))
		return -1;

	if (p->lSize < 0)
	{
		sys_err("invalid packet length %d", p->lSize);
		d->SetPhase(PHASE_CLOSE);
		return -1;
	}

	char szBuf[256+1];
	strlcpy(szBuf, c_pData + sizeof(TPacketGGMonarchNotice), MIN(p->lSize + 1, sizeof(szBuf)));
	SendMonarchNotice(p->bEmpire, szBuf);
	return (p->lSize);
}

int CInputP2P::MonarchTransfer(LPDESC d, const char* c_pData)
{
	TPacketMonarchGGTransfer* p = (TPacketMonarchGGTransfer*) c_pData;
	LPCHARACTER pTargetChar = CHARACTER_MANAGER::instance().FindByPID(p->dwTargetPID);

	if (pTargetChar != NULL)
	{
		unsigned int qIndex = quest::CQuestManager::instance().GetQuestIndexByName("monarch_transfer");

		if (qIndex != 0)
		{
			pTargetChar->SetQuestFlag("monarch_transfer.x", p->x);
			pTargetChar->SetQuestFlag("monarch_transfer.y", p->y);
			quest::CQuestManager::instance().Letter(pTargetChar->GetPlayerID(), qIndex, 0);
		}
	}

	return 0;
}

#ifdef __WORLD_BOSS_YUMA__
int CInputP2P::NewNotice(LPDESC d, const char* c_pData, size_t uiBytes)
{
	TPacketGGNewNotice* p = (TPacketGGNewNotice*)c_pData;

	if (uiBytes < sizeof(TPacketGGNewNotice) + p->lSize)
		return -1;

	if (p->lSize < 0)
	{
		sys_err("invalid packet length {}", p->lSize);
		d->SetPhase(PHASE_CLOSE);
		return -1;
	}

	char szBuf[256 + 1];
	strlcpy(szBuf, c_pData + sizeof(TPacketGGNewNotice), MIN(p->lSize + 1, sizeof(szBuf)));
	SendNewNotice(szBuf, p->szName, p->iSecondsToSpawn);

	return (p->lSize);
}
#endif

int CInputP2P::Guild(LPDESC d, const char* c_pData, size_t uiBytes)
{
	TPacketGGGuild * p = (TPacketGGGuild *) c_pData;
	uiBytes -= sizeof(TPacketGGGuild);
	c_pData += sizeof(TPacketGGGuild);

	CGuild * g = CGuildManager::instance().FindGuild(p->dwGuild);

	switch (p->bSubHeader)
	{
		case GUILD_SUBHEADER_GG_CHAT:
			{
				if (uiBytes < sizeof(TPacketGGGuildChat))
					return -1;

				TPacketGGGuildChat * p = (TPacketGGGuildChat *) c_pData;

				if (g)
					g->P2PChat(p->szText);

				return sizeof(TPacketGGGuildChat);
			}

		case GUILD_SUBHEADER_GG_SET_MEMBER_COUNT_BONUS:
			{
				if (uiBytes < sizeof(int))
					return -1;

				int iBonus = *((int *) c_pData);
				CGuild* pGuild = CGuildManager::instance().FindGuild(p->dwGuild);
				if (pGuild)
				{
					pGuild->SetMemberCountBonus(iBonus);
				}
				return sizeof(int);
			}
		case GUILD_SUBHEADER_GG_CHANGE_EXP:
			{
				if (uiBytes < sizeof(TPacketGGGuildChangeExp))
					return -1;

				TPacketGGGuildChangeExp* p2 = (TPacketGGGuildChangeExp*) c_pData;
				if (g)
				{
					g->SetLevel(p2->byLevel);
					g->SetExp(p2->dwExp);

					// Forward GC packet to guild members on this receiving core
					TPacketGCGuild pack;
					pack.header    = HEADER_GC_GUILD;
					pack.size      = sizeof(pack) + 5;
					pack.subheader = GUILD_SUBHEADER_GC_CHANGE_EXP;

					TEMP_BUFFER buf;
					buf.write(&pack,       sizeof(pack));
					buf.write(&p2->byLevel, 1);
					buf.write(&p2->dwExp,   4);
					g->Packet(buf.read_peek(), buf.size());
				}
				return sizeof(TPacketGGGuildChangeExp);
			}

		case GUILD_SUBHEADER_GG_DONATE_MATERIALS:
			{
				if (uiBytes < sizeof(TPacketGGGuildDonateMaterials))
					return -1;

				TPacketGGGuildDonateMaterials* p2 = (TPacketGGGuildDonateMaterials*) c_pData;
				if (g)
				{
					g->SetDonateStone(p2->donate_stone);
					g->SetDonateLog(p2->donate_log);
					g->SetDonatePlywood(p2->donate_plywood);
				}
				return sizeof(TPacketGGGuildDonateMaterials);
			}

		default:
			sys_err ("UNKNOWN GUILD SUB PACKET");
			break;
	}
	return 0;
}

struct FuncShout
{
	const char * m_str;
	BYTE m_bEmpire;

	FuncShout(const char * str, BYTE bEmpire) : m_str(str), m_bEmpire(bEmpire)
	{
	}

	void operator () (LPDESC d)
	{
#ifdef ENABLE_NEWSTUFF
		if (!d->GetCharacter() || (!g_bGlobalShoutEnable && d->GetCharacter()->GetGMLevel() == GM_PLAYER && d->GetEmpire() != m_bEmpire))
			return;
#else
		if (!d->GetCharacter() || (d->GetCharacter()->GetGMLevel() == GM_PLAYER && d->GetEmpire() != m_bEmpire))
			return;
#endif
		d->GetCharacter()->ChatPacket(CHAT_TYPE_SHOUT, "%s", m_str);
	}
};

void SendShout(const char * szText, BYTE bEmpire)
{
	const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();
	std::for_each(c_ref_set.begin(), c_ref_set.end(), FuncShout(szText, bEmpire));
}

void CInputP2P::Shout(const char * c_pData)
{
	TPacketGGShout * p = (TPacketGGShout *) c_pData;
	SendShout(p->szText, p->bEmpire);
}

void CInputP2P::Disconnect(const char * c_pData)
{
	TPacketGGDisconnect * p = (TPacketGGDisconnect *) c_pData;

	LPDESC d = DESC_MANAGER::instance().FindByLoginName(p->szLogin);

	if (!d)
		return;

	if (!d->GetCharacter())
	{
		d->SetPhase(PHASE_CLOSE);
	}
	else
		d->DisconnectOfSameLogin();
}

void CInputP2P::Setup(LPDESC d, const char * c_pData)
{
	TPacketGGSetup * p = (TPacketGGSetup *) c_pData;
	sys_log(0, "P2P: Setup %s:%d", d->GetHostName(), p->wPort);
	d->SetP2P(d->GetHostName(), p->wPort, p->bChannel);
}

void CInputP2P::MessengerAdd(const char * c_pData)
{
	TPacketGGMessenger * p = (TPacketGGMessenger *) c_pData;
	sys_log(0, "P2P: Messenger Add %s %s", p->szAccount, p->szCompanion);
	MessengerManager::instance().__AddToList(p->szAccount, p->szCompanion);
}

void CInputP2P::MessengerRemove(const char * c_pData)
{
	TPacketGGMessenger * p = (TPacketGGMessenger *) c_pData;
	sys_log(0, "P2P: Messenger Remove %s %s", p->szAccount, p->szCompanion);
	MessengerManager::instance().__RemoveFromList(p->szAccount, p->szCompanion);
}

void CInputP2P::FindPosition(LPDESC d, const char* c_pData)
{
	TPacketGGFindPosition* p = (TPacketGGFindPosition*) c_pData;
	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(p->dwTargetPID);

	if (ch)
	{
		TPacketGGWarpCharacter pw;
		pw.header = HEADER_GG_WARP_CHARACTER;
		pw.pid = p->dwFromPID;
		pw.x = ch->GetX();
		pw.y = ch->GetY();
		
        // Supplementary data
        pw.lAddr = inet_addr(g_szPublicIP);
        pw.lMapIndex = ch->GetMapIndex();
        pw.wPort = mother_port;
		
		d->Packet(&pw, sizeof(pw));
	}
}

void CInputP2P::WarpCharacter(const char* c_pData)
{
	TPacketGGWarpCharacter* p = (TPacketGGWarpCharacter*) c_pData;
	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(p->pid);

	if (ch)
	{
		ch->WarpSet(p->x, p->y, p->lMapIndex, p->lAddr, p->wPort);
	}
}

void CInputP2P::GuildWarZoneMapIndex(const char* c_pData)
{
	TPacketGGGuildWarMapIndex * p = (TPacketGGGuildWarMapIndex*) c_pData;
	CGuildManager & gm = CGuildManager::instance();

	sys_log(0, "P2P: GuildWarZoneMapIndex g1(%u) vs g2(%u), mapIndex(%d)", p->dwGuildID1, p->dwGuildID2, p->lMapIndex);

	CGuild * g1 = gm.FindGuild(p->dwGuildID1);
	CGuild * g2 = gm.FindGuild(p->dwGuildID2);

	if (g1 && g2)
	{
		g1->SetGuildWarMapIndex(p->dwGuildID2, p->lMapIndex);
		g2->SetGuildWarMapIndex(p->dwGuildID1, p->lMapIndex);
	}
}

void CInputP2P::Transfer(const char * c_pData)
{
	TPacketGGTransfer * p = (TPacketGGTransfer *) c_pData;

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(p->szName);

	if (ch)
		ch->WarpSet(p->lX, p->lY);
}

void CInputP2P::XmasWarpSanta(const char * c_pData)
{
	TPacketGGXmasWarpSanta * p =(TPacketGGXmasWarpSanta *) c_pData;

	if (p->bChannel == g_bChannel && map_allow_find(p->lMapIndex))
	{
		int	iNextSpawnDelay = 50 * 60;

		xmas::SpawnSanta(p->lMapIndex, iNextSpawnDelay);

		TPacketGGXmasWarpSantaReply pack_reply;
		pack_reply.bHeader = HEADER_GG_XMAS_WARP_SANTA_REPLY;
		pack_reply.bChannel = g_bChannel;
		P2P_MANAGER::instance().Send(&pack_reply, sizeof(pack_reply));
	}
}

void CInputP2P::XmasWarpSantaReply(const char* c_pData)
{
	TPacketGGXmasWarpSantaReply* p = (TPacketGGXmasWarpSantaReply*) c_pData;

	if (p->bChannel == g_bChannel)
	{
		CharacterVectorInteractor i;

		if (CHARACTER_MANAGER::instance().GetCharactersByRaceNum(xmas::MOB_SANTA_VNUM, i))
		{
			CharacterVectorInteractor::iterator it = i.begin();

			while (it != i.end()) {
				M2_DESTROY_CHARACTER(*it++);
			}
		}
	}
}

void CInputP2P::LoginPing(LPDESC d, const char * c_pData)
{
	TPacketGGLoginPing * p = (TPacketGGLoginPing *) c_pData;
	
	if (strlen(p->szLogin) < 3) // P2P Crack spam...
	{
		sys_err("%s:%d p->login().c_str() : '%s' strlen() DONT BROADCAST PACKET", __FILE__, __LINE__, p->szLogin, strlen(p->szLogin));
		return;
	}
	else if (!check_name(p->szLogin)) // P2P Crack spam...
	{
		sys_err("%s:%d check_name(p->login().c_str()) : '%s' failed..  DONT BROADCAST PACKET", __FILE__, __LINE__, p->szLogin, strlen(p->szLogin));
		return;
	}
	
	SendBillingExpire(p->szLogin, BILLING_DAY, 0, NULL);

	if (!g_pkAuthMasterDesc) // If I am master, I have to broadcast
		P2P_MANAGER::instance().Send(p, sizeof(TPacketGGLoginPing), d);
}

void CInputP2P::DisconnectPlayer(const char* c_pData)
{
	TPacketGGDisconnectPlayer* p = (TPacketGGDisconnectPlayer*)c_pData;

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(p->szName);

	if (ch && ch->GetDesc())
	{
		LPDESC d = ch->GetDesc();
		sys_log(0, "DISCONNECT PLAYER success name %s", p->szName);
		DESC_MANAGER::instance().DestroyLoginKey(d);
		d->SetPhase(PHASE_CLOSE);
	}
	else
	{
		sys_log(0, "DISCONNECT PLAYER fail name %s", p->szName);
	}
}

// BLOCK_CHAT
void CInputP2P::BlockChat(const char * c_pData)
{
	TPacketGGBlockChat * p = (TPacketGGBlockChat *) c_pData;

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(p->szName);

	if (ch)
	{
		sys_log(0, "BLOCK CHAT apply name %s dur %d", p->szName, p->lBlockDuration);
		ch->AddAffect(AFFECT_BLOCK_CHAT, POINT_NONE, 0, AFF_NONE, p->lBlockDuration, 0, true);
	}
	else
	{
		sys_log(0, "BLOCK CHAT fail name %s dur %d", p->szName, p->lBlockDuration);
	}
}
// END_OF_BLOCK_CHAT
//

#ifdef ENABLE_MULTI_FARM_BLOCK
void CInputP2P::MultiFarm(const char* c_pData)
{
	TPacketGGMultiFarm* p = (TPacketGGMultiFarm*)c_pData;
	if(p->subHeader == MULTI_FARM_SET)
		CHARACTER_MANAGER::Instance().CheckMultiFarmAccount(p->playerHWID, p->playerID, p->playerName, p->farmStatus, p->affectType, p->affectTime, true);
	else if (p->subHeader == MULTI_FARM_REMOVE)
		CHARACTER_MANAGER::Instance().RemoveMultiFarm(p->playerHWID, p->playerID, true);
}
#endif

void CInputP2P::IamAwake(LPDESC d, const char * c_pData)
{
	std::string hostNames;
	P2P_MANAGER::instance().GetP2PHostNames(hostNames);
	sys_log(0, "P2P Awakeness check from %s. My P2P connection number is %d. and details...\n%s", d->GetHostName(), P2P_MANAGER::instance().GetDescCount(), hostNames.c_str());
}

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
struct FFindOfflineShop
{
	const char * szName;
	
	FFindOfflineShop(const char * c_szName) : szName(c_szName) {};
	
	void operator()(LPENTITY ent)
	{
		if (!ent)
			return;
		
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER ch = (LPCHARACTER)ent;
			if (ch->IsOfflineShopNPC() && !strcmp(szName, ch->GetName()))			
				ch->DestroyOfflineShop();			
		}
	}
};

void CInputP2P::RemoveOfflineShop(LPDESC d, const char * c_pData)
{
	TPacketGGRemoveOfflineShop * p = (TPacketGGRemoveOfflineShop *)c_pData;
	LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(p->lMapIndex);
	
	if (pMap)
	{
		FFindOfflineShop offlineshop(p->szNpcName);
		pMap->for_each(offlineshop);
	}
}

/*void CInputP2P::OfflineShopBuy(LPDESC d, const char * c_pData)
{
	TPacketGGOfflineShopBuy * p = (TPacketGGOfflineShopBuy *)c_pData;
}*/
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
void CInputP2P::PrivateShopItemSearch(const char* c_pData)
{
	TPacketGGPrivateShopItemSearch* p = (TPacketGGPrivateShopItemSearch*)c_pData;

	LPDESC pPeer = P2P_MANAGER::Instance().GetPeer(p->dwCustomerPort);
	if (!pPeer)
		return;

	TEMP_BUFFER buf;
	CPrivateShopManager::Instance().SearchItem(pPeer, p->Filter, p->bUseFilter, p->dwCustomerID);

	if (buf.size())
	{
		TPacketGGPrivateShopItemSearchResult mainPacket{};
		mainPacket.bHeader = HEADER_GG_PRIVATE_SHOP_ITEM_SEARCH_RESULT;
		mainPacket.wSize = buf.size();
		mainPacket.dwCustomerID = p->dwCustomerID;

		pPeer->BufferedPacket(&mainPacket, sizeof(mainPacket));
		pPeer->LargePacket(buf.read_peek(), buf.size());
	}
}

int CInputP2P::PrivateShopItemSearchResult(const char* c_pData, size_t uiBytes)
{
	TPacketGGPrivateShopItemSearchResult* p = (TPacketGGPrivateShopItemSearchResult*)c_pData;

	if (uiBytes < sizeof(TPacketGGPrivateShopItemSearchResult) + p->wSize)
		return -1;

	c_pData += sizeof(TPacketGGPrivateShopItemSearchResult);

	LPCHARACTER pCustomer = CHARACTER_MANAGER::Instance().FindByPID(p->dwCustomerID);
	if (!pCustomer)
		return p->wSize;

	TPacketGCPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_GC_PRIVATE_SHOP;
	mainPacket.wSize = sizeof(TPacketGCPrivateShop) + p->wSize;
	mainPacket.bSubHeader = SUBHEADER_GC_PRIVATE_SHOP_SEARCH_RESULT;

	pCustomer->GetDesc()->BufferedPacket(&mainPacket, sizeof(TPacketGCPrivateShop));
	pCustomer->GetDesc()->LargePacket(c_pData, p->wSize);

	return p->wSize;
}

void CInputP2P::PrivateShopItemSearchUpdate(const char* c_pData)
{
	TPacketGGPrivateShopItemSearchUpdate* p = (TPacketGGPrivateShopItemSearchUpdate*)c_pData;

	TPacketGCPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_GC_PRIVATE_SHOP;
	mainPacket.wSize = sizeof(TPacketGCPrivateShop) + sizeof(TPacketGCPrivateShopSearchUpdate);
	mainPacket.bSubHeader = SUBHEADER_GC_PRIVATE_SHOP_SEARCH_UPDATE;

	TPacketGCPrivateShopSearchUpdate subPacket{};
	subPacket.dwShopID = p->dwShopID;
	subPacket.iSpecificItemPos = p->iSpecificItemPos;
	subPacket.bState = p->bState;

	TEMP_BUFFER buf;
	buf.write(&mainPacket, sizeof(mainPacket));
	buf.write(&subPacket, sizeof(subPacket));

	const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();
	std::for_each(c_ref_set.begin(), c_ref_set.end(),
		[&buf](LPDESC pDesc)
		{
			if (pDesc && pDesc->GetCharacter())
			{
				if (pDesc->GetCharacter()->IsShopSearch())
					pDesc->Packet(buf.read_peek(), buf.size());
			}
		}
	);
}
#endif

int CInputP2P::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	if (test_server)
		sys_log(0, "CInputP2P::Anlayze[Header %d]", bHeader);

	int iExtraLen = 0;

	switch (bHeader)
	{
		case HEADER_GG_SETUP:
			Setup(d, c_pData);
			break;

		case HEADER_GG_LOGIN:
			Login(d, c_pData);
			break;

		case HEADER_GG_LOGOUT:
			Logout(d, c_pData);
			break;

		case HEADER_GG_RELAY:
			if ((iExtraLen = Relay(d, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#ifdef ENABLE_FULL_NOTICE
		case HEADER_GG_BIG_NOTICE:
			if ((iExtraLen = Notice(d, c_pData, m_iBufferLeft, true)) < 0)
				return -1;
			break;
#endif
		case HEADER_GG_NOTICE:
			if ((iExtraLen = Notice(d, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

#ifdef __WORLD_BOSS_YUMA__
		case HEADER_GG_NEW_NOTICE:
			if ((iExtraLen = NewNotice(d, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#endif

		case HEADER_GG_WORLD_BOSS_ALIVE:
			{
				const TPacketGGWorldBossAlive* p = reinterpret_cast<const TPacketGGWorldBossAlive*>(c_pData);
				MonsterSpawner::instance().UpdateBossAliveStatus(p->dwVnum, p->bAlive, p->tNextSpawn);
			}
			break;

		case HEADER_GG_SHUTDOWN:
			sys_err("Accept shutdown p2p command from %s.", d->GetHostName());
			Shutdown(10);
			break;

		case HEADER_GG_GUILD:
			if ((iExtraLen = Guild(d, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_GG_SHOUT:
			Shout(c_pData);
			break;

		case HEADER_GG_DISCONNECT:
			Disconnect(c_pData);
			break;

		case HEADER_GG_MESSENGER_ADD:
			MessengerAdd(c_pData);
			break;
			
#ifdef ENABLE_MESSENGER_BLOCK
		case HEADER_GG_MESSENGER_BLOCK_ADD:
			MessengerBlockAdd(c_pData);
			break;

		case HEADER_GG_MESSENGER_BLOCK_REMOVE:
			MessengerBlockRemove(c_pData);
			break;
#endif

		case HEADER_GG_MESSENGER_REMOVE:
			MessengerRemove(c_pData);
			break;

		case HEADER_GG_FIND_POSITION:
			FindPosition(d, c_pData);
			break;

		case HEADER_GG_WARP_CHARACTER:
			WarpCharacter(c_pData);
			break;

		case HEADER_GG_GUILD_WAR_ZONE_MAP_INDEX:
			GuildWarZoneMapIndex(c_pData);
			break;

		case HEADER_GG_TRANSFER:
			Transfer(c_pData);
			break;

		case HEADER_GG_XMAS_WARP_SANTA:
			XmasWarpSanta(c_pData);
			break;

		case HEADER_GG_XMAS_WARP_SANTA_REPLY:
			XmasWarpSantaReply(c_pData);
			break;

		case HEADER_GG_RELOAD_CRC_LIST:
			LoadValidCRCList();
			break;

		case HEADER_GG_CHECK_CLIENT_VERSION:
			CheckClientVersion();
			break;

		case HEADER_GG_LOGIN_PING:
			LoginPing(d, c_pData);
			break;

		case HEADER_GG_BLOCK_CHAT:
			BlockChat(c_pData);
			break;
	#ifdef __CROSS_CHANNEL_DUNGEON_WARP__
		case HEADER_GG_CREATE_DUNGEON_INSTANCE:
			CreateDungeonInstance(d, c_pData);
			break;
	#endif
		case HEADER_GG_HANDSHAKE_VALIDATION:
			DESC_MANAGER::instance().AddToHandshakeWhiteList((const TPacketGGHandshakeValidate *) c_pData);
			break;

		case HEADER_GG_SIEGE:
			{
				TPacketGGSiege* pSiege = (TPacketGGSiege*)c_pData;
				castle_siege(pSiege->bEmpire, pSiege->bTowerCount);
			}
			break;

		case HEADER_GG_MONARCH_NOTICE:
			if ((iExtraLen = MonarchNotice(d, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_GG_MONARCH_TRANSFER :
			MonarchTransfer(d, c_pData);
			break;

		case HEADER_GG_CHECK_AWAKENESS:
			IamAwake(d, c_pData);
			break;
#ifdef ENABLE_MULTI_FARM_BLOCK
		case HEADER_GG_MULTI_FARM:
			MultiFarm(c_pData);
			break;
#endif
#ifdef ENABLE_SWITCHBOT
		case HEADER_GG_SWITCHBOT:
			Switchbot(d, c_pData);
			break;
#endif

#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
		case HEADER_GG_CMDCHAT:
			if ((iExtraLen = CmdchatToAll(d, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		case HEADER_GG_REMOVE_OFFLINE_SHOP:
			RemoveOfflineShop(d, c_pData);
			break;
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
		case HEADER_GG_PRIVATE_SHOP_ITEM_SEARCH_UPDATE:
			PrivateShopItemSearchUpdate(c_pData);
			break;

		case HEADER_GG_PRIVATE_SHOP_ITEM_SEARCH:
			PrivateShopItemSearch(c_pData);
			break;

		case HEADER_GG_PRIVATE_SHOP_ITEM_SEARCH_RESULT:
			if ((iExtraLen = PrivateShopItemSearchResult(c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#endif
#ifdef __TEAM_MEMBER_STATUS__
		case HEADER_GG_TEAM_MEMBER:
		{
			const TPacketGGTeamList* pPacket = reinterpret_cast<const TPacketGGTeamList*>(c_pData);
			switch (pPacket->bSubHeader)
			{
				case SUBHEADER_GG_TEAM_MEMBER_ADD:
					CTeamListManager::instance().RegisterTeamMember(pPacket);
					break;
				case SUBHEADER_GG_TEAM_MEMBER_REMOVE:
					CTeamListManager::instance().RemoveTeamMember(pPacket);
					break;
				case SUBHEADER_GG_TEAM_MEMBER_RELOAD:
					CTeamListManager::instance().ClearTeamMember();
					break;
			}
		}
#endif
	    case HEADER_GG_RELOAD:
	        Reload(*reinterpret_cast<const TPacketGGReload*>(c_pData));
	        break;
	}

	return (iExtraLen);
}

#ifdef ENABLE_SWITCHBOT
#include "switchbot.h"
void CInputP2P::Switchbot(LPDESC d, const char* c_pData)
{
	const TPacketGGSwitchbot* p = reinterpret_cast<const TPacketGGSwitchbot*>(c_pData);
	if (p->wPort != mother_port)
	{
		return;
	}

	CSwitchbotManager::Instance().P2PReceiveSwitchbot(p->table);
}
#endif

void CInputP2P::Reload(const TPacketGGReload& packet) const {
    switch (packet.type) {
        case RELOAD_VERSION:
			sys_err("Interesting: %i", static_cast<int>(g_bAuthServer));
			if (g_bAuthServer)
			{
				client::g_versionManager->Load();	
			}
			break;
    }
}

#ifdef __CROSS_CHANNEL_DUNGEON_WARP__
void CInputP2P::CreateDungeonInstance(LPDESC d, const char* c_pData)
{
	TPacketGGCreateDungeonInstance* pack = (TPacketGGCreateDungeonInstance*)c_pData;
	// Let's check whether it's input or output packet
	if (pack->bRequest)
	{
		// All right, it's input packet
		// Run over map pools and check if there is a requested index
		// Be sure we lookup on same channel
		if (map_allow_find(pack->lMapIndex) && (g_bChannel == pack->bChannel || pack->bChannel == 99))
		{
			// Rang a bell
			// Send the output packet back
			pack->bRequest = false;
			pack->lAddr = inet_addr(g_szPublicIP);
			pack->wPort = mother_port;

			// Create dungeon instance
			LPDUNGEON pDungeon = CDungeonManager::instance().Create(pack->lMapIndex);
			if (!pDungeon)
			{
				// Oops, something failed!
				sys_err("Could not construct map for index %d", pack->lMapIndex);
				return;
			}

			// Add pids to attender list
			for (const auto& dwPID : pack->aPids)
				pDungeon->AddAttenderByPID(dwPID);

			LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(pDungeon->GetMapIndex());
			pack->lX = pkSectreeMap->m_setting.posSpawn.x;
			pack->lY = pkSectreeMap->m_setting.posSpawn.y;

			// Replace origin map index
			pack->lMapIndex = pDungeon->GetMapIndex();

			// Send it back to source
			d->Packet(pack, sizeof(TPacketGGCreateDungeonInstance));
			sys_log(0, "Request from PID %d was accepted. The private map instance is %d.", pack->aPids[0], pack->lMapIndex);
		}
	}
	else
	{
		// We got an output packet then
		// Let's see if requested PID is still online
		LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(pack->aPids[0]);
		// If not, give up
		if (!ch)
			return;

		sys_log(0, "Receive for PID %d was accepted. The private map instance is %d.", pack->aPids[0], pack->lMapIndex);

		// Otherwise warp everyone
		for (unsigned int i = 0; i < pack->bCount; ++i)
		{
			auto pChar = CHARACTER_MANAGER::instance().FindByPID(pack->aPids[i]);
			if (pChar)
			{
				// Save exit location
				pChar->SaveExitLocation();

				// Warp
				pChar->WarpSet(pack->lX, pack->lY, pack->lMapIndex, pack->lAddr, pack->wPort);
			}
		}
	}
}

#ifdef __DUNGEON_RETURN_ENABLE__
void CInputP2P::RejoinDungeon(LPDESC d, const char* c_pData)
{
	TPacketGGCreateDungeonInstance* pack = (TPacketGGCreateDungeonInstance*)c_pData;
	// Let's check whether it's input or output packet
	if (pack->bRequest)
	{
		// All right, it's input packet
		// Run over map pools and check if there is a requested index
		// Be sure we lookup on same channel (99 is exempted)
		if (map_allow_find(pack->lMapIndex / 10000))
		{
			auto pDungeon = CDungeonManager::instance().FindDungeonByPID(pack->aPids[0], pack->lMapIndex);
			if (!pDungeon)
				return;

			// Rang a bell
			// Send the output packet back
			pack->bRequest = false;
			pack->lAddr = inet_addr(g_szPublicIP);
			pack->wPort = mother_port;

			LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(pDungeon->GetMapIndex());
			// If it's a party dungeon, try to warp player right next to his group members
			if (auto pPartyChs = pDungeon->GetDungeonPlayers(); pPartyChs.size())
			{
				auto pChar = *pPartyChs.begin();
				pack->lX = pChar->GetX();
				pack->lY = pChar->GetY();
			}

			// Replace origin map index
			pack->lMapIndex = pDungeon->GetMapIndex();

			// Send it back to source
			d->Packet(pack, sizeof(TPacketGGCreateDungeonInstance));
			sys_log(0, "Request from PID %d was accepted. The private map instance is %d.", pack->aPids[0], pack->lMapIndex);
		}
	}
	else
	{
		// We got an output packet then
		// Let's see if requested PID is still online
		LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(pack->aPids[0]);
		// If not, give up
		if (!ch)
			return;

		sys_log(0, "Receive for PID %d was accepted. The private map instance is %d.", pack->aPids[0], pack->lMapIndex);

		// Save exit location
		ch->SaveExitLocation();

		// Clear flag
		ch->setDungeonRejoinWaiting(false);

		// Warp
		// If player is on party dungeon and at least one of party member is still there, warp one's next to the player
		if (pack->lX && pack->lY)
			ch->WarpSet(pack->lX, pack->lY, pack->lMapIndex, pack->lAddr, pack->wPort);
		else
			// Otherwise warp to last, cached location
			ch->WarpSet(ch->GetQuestFlag("dungeon_return.x"), ch->GetQuestFlag("dungeon_return.y"), pack->lMapIndex, pack->lAddr, pack->wPort);
	}
}

void CInputP2P::CanRejoinDungeon(LPDESC d, const char* c_pData)
{
	TPacketGGCreateDungeonInstance* pack = (TPacketGGCreateDungeonInstance*)c_pData;
	// Let's check whether it's input or output packet
	if (pack->bRequest)
	{
		// All right, it's input packet
		// Run over map pools and check if there is a requested index
		// Be sure we lookup on same channel (99 is exempted)
		if (map_allow_find(pack->lMapIndex / 10000))
		{
			auto pDungeon = CDungeonManager::instance().FindDungeonByPID(pack->aPids[0], pack->lMapIndex);
			if (!pDungeon)
				return;

			// Rang a bell
			// Send the output packet back
			pack->bRequest = false;

			// Send it back to source
			d->Packet(pack, sizeof(TPacketGGCreateDungeonInstance));
			sys_log(0, "Request from PID %d was accepted. The private map instance is %d.", pack->aPids[0], pack->lMapIndex);
		}
	}
	else
	{
		// We got an output packet then
		// Let's see if requested PID is still online
		LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(pack->aPids[0]);
		// If not, give up
		if (!ch)
			return;

		ch->SetDungeonReturn(true);
		CDungeonInfoManager::instance().SendRejoinStatus(ch, pack->lMapIndex / 10000);
		sys_log(0, "Received ping-back dungeon rejoin packet! PID: %u, Index: %u", pack->aPids[0], pack->lMapIndex);
		// quest::CQuestManager::instance().Letter(ch->GetPlayerID(), quest::CQuestManager::instance().GetQuestIndexByName("dungeon_teleport"), 0);
	}
}
#endif
#endif

#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
int CInputP2P::CmdchatToAll(LPDESC d, const char* c_pData, size_t uiBytes)
{
	TPacketGGCmdchat* p = (TPacketGGCmdchat*)c_pData;
	if (uiBytes < sizeof(TPacketGGCmdchat) + p->lSize)
		return -1;

	if (p->lSize < 0)
	{
		sys_err("invalid packet length %d", p->lSize);
		d->SetPhase(PHASE_CLOSE);
		return -1;
	}

	char szBuf[256 + 1];
	strlcpy(szBuf, c_pData + sizeof(TPacketGGCmdchat), MIN(p->lSize + 1, sizeof(szBuf)));
	SendCmdchatToAll(szBuf);

	return (p->lSize);
}
#endif