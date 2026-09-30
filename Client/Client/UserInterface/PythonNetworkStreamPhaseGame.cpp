#include "StdAfx.h"
#include "PythonNetworkStream.h"
#include "Packet.h"

#include "PythonGuild.h"
#include "PythonCharacterManager.h"
#include "PythonPlayer.h"
#include "PythonBackground.h"
#include "PythonMiniMap.h"
#include "PythonTextTail.h"
#include "PythonItem.h"
#include "PythonChat.h"
#include "PythonShop.h"
#include "PythonExchange.h"
#include "PythonQuest.h"
#include "PythonEventManager.h"
#include "PythonMessenger.h"
#include "PythonApplication.h"

#include "../EterPack/EterPackManager.h"
#include "../gamelib/ItemManager.h"

#include "AbstractApplication.h"
#include "AbstractCharacterManager.h"
#include "InstanceBase.h"
#include "PythonPlayerSettingsModule.h"

#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
#include "PythonCubeRenewal.h"
#endif
#ifdef ENABLE_BADGE_NOTIFICATION_MANAGER
#include "BadgeNotificationManager.h"
#endif

#if defined(ENABLE_DUNGEON_INFO_SYSTEM)
#	include "PythonDungeonInfo.h"
#endif

#include "PythonBiologManager.h"

#include "ProcessCRC.h"

#include "PythonCube.hpp"

BOOL gs_bEmpireLanuageEnable = TRUE;

void CPythonNetworkStream::__RefreshAlignmentWindow()
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshAlignment", Py_BuildValue("()"));
}

void CPythonNetworkStream::__RefreshTargetBoardByVID(DWORD dwVID)
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshTargetBoardByVID", Py_BuildValue("(i)", dwVID));
}

void CPythonNetworkStream::__RefreshTargetBoardByName(const char * c_szName)
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshTargetBoardByName", Py_BuildValue("(s)", c_szName));
}

void CPythonNetworkStream::__RefreshTargetBoard()
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshTargetBoard", Py_BuildValue("()"));
}

void CPythonNetworkStream::__RefreshGuildWindowGradePage()
{
	m_isRefreshGuildWndGradePage=true;
}

void CPythonNetworkStream::__RefreshGuildWindowSkillPage()
{
	m_isRefreshGuildWndSkillPage=true;
}

void CPythonNetworkStream::__RefreshGuildWindowMemberPageGradeComboBox()
{
	m_isRefreshGuildWndMemberPageGradeComboBox=true;
}

void CPythonNetworkStream::__RefreshGuildWindowMemberPage()
{
	m_isRefreshGuildWndMemberPage=true;
}

void CPythonNetworkStream::__RefreshGuildWindowBoardPage()
{
	m_isRefreshGuildWndBoardPage=true;
}

void CPythonNetworkStream::__RefreshGuildWindowInfoPage()
{
	m_isRefreshGuildWndInfoPage=true;
}

void CPythonNetworkStream::__RefreshGuildWindowDungeonInfoPage()
{
	m_isRefreshGuildWndDungeonInfoPage = true;
}

void CPythonNetworkStream::__RefreshGuildWindowDungeonBossDmgRankWindow()
{
	m_isRefreshGuildDungeonBossDmgRankWnd = true;
}

void CPythonNetworkStream::__RefreshMessengerWindow()
{
	m_isRefreshMessengerWnd=true;
}

void CPythonNetworkStream::__RefreshSafeboxWindow()
{
	m_isRefreshSafeboxWnd=true;
}

void CPythonNetworkStream::__RefreshMallWindow()
{
	m_isRefreshMallWnd=true;
}

void CPythonNetworkStream::__RefreshSkillWindow()
{
	m_isRefreshSkillWnd=true;
}

void CPythonNetworkStream::__RefreshExchangeWindow()
{
	m_isRefreshExchangeWnd=true;
}

void CPythonNetworkStream::__RefreshStatus()
{
	m_isRefreshStatus=true;
}

void CPythonNetworkStream::__RefreshCharacterWindow()
{
	m_isRefreshCharacterWnd=true;
}

void CPythonNetworkStream::__RefreshInventoryWindow()
{
	m_isRefreshInventoryWnd=true;
}

void CPythonNetworkStream::__RefreshEquipmentWindow()
{
	m_isRefreshEquipmentWnd=true;
}

#ifdef ENABLE_AFFECT_FIX
void CPythonNetworkStream::__RefreshAffectWindow()
{
	m_isRefreshAffectWindow = true;
}
#endif

void CPythonNetworkStream::__SetGuildID(DWORD id)
{
	if (m_dwGuildID != id)
	{
		m_dwGuildID = id;
		IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();

		for (int i = 0; i < PLAYER_PER_ACCOUNT4; ++i)
			if (!strncmp(m_akSimplePlayerInfo[i].szName, rkPlayer.GetName(), CHARACTER_NAME_MAX_LEN))
			{
				m_adwGuildID[i] = id;

				std::string  guildName;
				if (CPythonGuild::Instance().GetGuildName(id, &guildName))
				{
					m_astrGuildName[i] = guildName;
				}
				else
				{
					m_astrGuildName[i] = "";
				}
			}
	}
}

struct PERF_PacketInfo
{
	DWORD dwCount;
	DWORD dwTime;

	PERF_PacketInfo()
	{
		dwCount=0;
		dwTime=0;
	}
};

#ifdef __PERFORMANCE_CHECK__

class PERF_PacketTimeAnalyzer
{
	public:
		~PERF_PacketTimeAnalyzer()
		{
			FILE* fp=fopen("perf_dispatch_packet_result.txt", "w");

			for (std::map<DWORD, PERF_PacketInfo>::iterator i=m_kMap_kPacketInfo.begin(); i!=m_kMap_kPacketInfo.end(); ++i)
			{
				if (i->second.dwTime>0)
					fprintf(fp, "header %d: count %d, time %d, tpc %d\n", i->first, i->second.dwCount, i->second.dwTime, i->second.dwTime/i->second.dwCount);
			}
			fclose(fp);
		}

	public:
		std::map<DWORD, PERF_PacketInfo> m_kMap_kPacketInfo;
};

PERF_PacketTimeAnalyzer gs_kPacketTimeAnalyzer;

#endif

// Game Phase ---------------------------------------------------------------------------
void CPythonNetworkStream::GamePhase()
{
	if (!m_kQue_stHack.empty())
	{
		__SendHack(m_kQue_stHack.front().c_str());
		m_kQue_stHack.pop_front();
	}

	TPacketHeader header = 0;
	bool ret = true;

#ifdef __PERFORMANCE_CHECK__
	DWORD timeBeginDispatch=timeGetTime();

	static std::map<DWORD, PERF_PacketInfo> kMap_kPacketInfo;
	kMap_kPacketInfo.clear();
#endif

	const DWORD MAX_RECV_COUNT = 8; // @warme669
	const DWORD SAFE_RECV_BUFSIZE = 8192;
	DWORD dwRecvCount = 0;

    while (ret)
	{
		//if(dwRecvCount++ >= MAX_RECV_COUNT-1 && GetRecvBufferSize() < SAFE_RECV_BUFSIZE
		//	&& m_strPhase == "Game")
		//	break;

		if (!CheckPacket(&header))
			break;

#ifdef __PERFORMANCE_CHECK__
		DWORD timeBeginPacket=timeGetTime();
#endif

#if defined(_DEBUG) && defined(ENABLE_PRINT_RECV_PACKET_DEBUG)
		Tracenf("RECV HEADER : %u , phase %s ", header, m_strPhase.c_str());
#endif

		switch (header)
		{
			case HEADER_GC_OBSERVER_ADD:
				ret = RecvObserverAddPacket();
				break;
			case HEADER_GC_OBSERVER_REMOVE:
				ret = RecvObserverRemovePacket();
				break;
			case HEADER_GC_OBSERVER_MOVE:
				ret = RecvObserverMovePacket();
				break;
			case HEADER_GC_WARP:
				ret = RecvWarpPacket();
				break;

			case HEADER_GC_PHASE:
				ret = RecvPhasePacket();
				return;
				break;

			case HEADER_GC_PVP:
				ret = RecvPVPPacket();
				break;

#ifdef _ENABLE_BATTLEPASS_
			case HEADER_GC_BATTLE_PASS:
				ret = RecvBattlePass();
				break;
#endif

			case HEADER_GC_DUEL_START:
				ret = RecvDuelStartPacket();
				break;

#ifdef ENABLE_MOUNT_SYSTEM
			case HEADER_GC_MOUNT_INFO:
				ret = RecvMountInfoPacket();
				break;
#endif

			case HEADER_GC_CHARACTER_ADD:
 				ret = RecvCharacterAppendPacket();
				break;

			case HEADER_GC_CHAR_ADDITIONAL_INFO:
				ret = RecvCharacterAdditionalInfo();
				break;

			case HEADER_GC_CHARACTER_ADD2:
				ret = RecvCharacterAppendPacketNew();
				break;

			case HEADER_GC_CHARACTER_UPDATE:
				ret = RecvCharacterUpdatePacket();
				break;

			case HEADER_GC_CHARACTER_DEL:
				ret = RecvCharacterDeletePacket();
				break;

			case HEADER_GC_CHAT:
				ret = RecvChatPacket();
				break;

			case HEADER_GC_SYNC_POSITION:
				ret = RecvSyncPositionPacket();
				break;

			case HEADER_GC_OWNERSHIP:
				ret = RecvOwnerShipPacket();
				break;

			case HEADER_GC_WHISPER:
				ret = RecvWhisperPacket();
				break;

			case HEADER_GC_CHARACTER_MOVE:
				ret = RecvCharacterMovePacket();
				break;

			// Position
			case HEADER_GC_CHARACTER_POSITION:
				ret = RecvCharacterPositionPacket();
				break;

			// Battle Packet
			case HEADER_GC_STUN:
				ret = RecvStunPacket();
				break;

			case HEADER_GC_DEAD:
				ret = RecvDeadPacket();
				break;

			case HEADER_GC_PLAYER_POINT_CHANGE:
				ret = RecvPointChange();
				break;

			// item packet.
			case HEADER_GC_ITEM_SET:
				ret = RecvItemSetPacket();
				break;

			case HEADER_GC_ITEM_SET2:
				ret = RecvItemSetPacket2();
				break;

			case HEADER_GC_ITEM_USE:
				ret = RecvItemUsePacket();
				break;

			case HEADER_GC_ITEM_UPDATE:
				ret = RecvItemUpdatePacket();
				break;

			case HEADER_GC_ITEM_GROUND_ADD:
				ret = RecvItemGroundAddPacket();
				break;

			case HEADER_GC_ITEM_GROUND_DEL:
				ret = RecvItemGroundDelPacket();
				break;

			case HEADER_GC_ITEM_OWNERSHIP:
				ret = RecvItemOwnership();
				break;

			case HEADER_GC_QUICKSLOT_ADD:
				ret = RecvQuickSlotAddPacket();
				break;

			case HEADER_GC_QUICKSLOT_DEL:
				ret = RecvQuickSlotDelPacket();
				break;

			case HEADER_GC_QUICKSLOT_SWAP:
				ret = RecvQuickSlotMovePacket();
				break;

			case HEADER_GC_MOTION:
				ret = RecvMotionPacket();
				break;

			case HEADER_GC_SHOP:
				ret = RecvShopPacket();
				break;

			case HEADER_GC_SHOP_SIGN:
				ret = RecvShopSignPacket();
				break;
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
			case HEADER_GC_OFFLINE_SHOP:
				ret = RecvOfflineShopPacket();
				break;

			case HEADER_GC_OFFLINE_SHOP_SIGN:
				ret = RecvOfflineShopSignPacket();
				break;

			case HEADER_GC_OFFLINE_SHOP_LOG:
				ret = RecvOfflineShopLogs();
				break;
#endif
#if defined(__BL_KILL_BAR__)
			case HEADER_GC_KILLBAR:
				ret = RecvKillBar();
				break;
#endif
			case HEADER_GC_EXCHANGE:
				ret = RecvExchangePacket();
				break;

			case HEADER_GC_QUEST_INFO:
				ret = RecvQuestInfoPacket();
				break;

			case HEADER_GC_REQUEST_MAKE_GUILD:
				ret = RecvRequestMakeGuild();
				break;

			case HEADER_GC_PING:
				ret = RecvPingPacket();
				break;

			case HEADER_GC_SCRIPT:
				ret = RecvScriptPacket();
				break;

			case HEADER_GC_QUEST_CONFIRM:
				ret = RecvQuestConfirmPacket();
				break;

			case HEADER_GC_TARGET:
				ret = RecvTargetPacket();
				break;

			case HEADER_GC_DAMAGE_INFO:
				ret = RecvDamageInfoPacket();
				break;

			case HEADER_GC_MOUNT:
				ret = RecvMountPacket();
				break;

			case HEADER_GC_CHANGE_SPEED:
				ret = RecvChangeSpeedPacket();
				break;

			case HEADER_GC_PLAYER_POINTS:
				ret = __RecvPlayerPoints();
				break;

			case HEADER_GC_CREATE_FLY:
				ret = RecvCreateFlyPacket();
				break;

			case HEADER_GC_FLY_TARGETING:
				ret = RecvFlyTargetingPacket();
				break;

			case HEADER_GC_ADD_FLY_TARGETING:
				ret = RecvAddFlyTargetingPacket();
				break;

			case HEADER_GC_SKILL_LEVEL:
				ret = RecvSkillLevel();
				break;

			case HEADER_GC_SKILL_LEVEL_NEW:
				ret = RecvSkillLevelNew();
				break;

			case HEADER_GC_MESSENGER:
				ret = RecvMessenger();
				break;

			case HEADER_GC_GUILD:
				ret = RecvGuild();
				break;

			case HEADER_GC_PARTY_INVITE:
				ret = RecvPartyInvite();
				break;

			case HEADER_GC_PARTY_ADD:
				ret = RecvPartyAdd();
				break;

			case HEADER_GC_PARTY_UPDATE:
				ret = RecvPartyUpdate();
				break;

			case HEADER_GC_PARTY_REMOVE:
				ret = RecvPartyRemove();
				break;

			case HEADER_GC_PARTY_LINK:
				ret = RecvPartyLink();
				break;

			case HEADER_GC_PARTY_UNLINK:
				ret = RecvPartyUnlink();
				break;

			case HEADER_GC_PARTY_PARAMETER:
				ret = RecvPartyParameter();
				break;

			case HEADER_GC_SAFEBOX_SET:
				ret = RecvSafeBoxSetPacket();
				break;

			case HEADER_GC_SAFEBOX_DEL:
				ret = RecvSafeBoxDelPacket();
				break;

			case HEADER_GC_SAFEBOX_WRONG_PASSWORD:
				ret = RecvSafeBoxWrongPasswordPacket();
				break;

			case HEADER_GC_SAFEBOX_SIZE:
				ret = RecvSafeBoxSizePacket();
				break;

			case HEADER_GC_SAFEBOX_MONEY_CHANGE:
				ret = RecvSafeBoxMoneyChangePacket();
				break;

			case HEADER_GC_FISHING:
				ret = RecvFishing();
				break;

			case HEADER_GC_DUNGEON:
				ret = RecvDungeon();
				break;

			case HEADER_GC_TIME:
				ret = RecvTimePacket();
				break;

			case HEADER_GC_WALK_MODE:
				ret = RecvWalkModePacket();
				break;

			case HEADER_GC_CHANGE_SKILL_GROUP:
				ret = RecvChangeSkillGroupPacket();
				break;

			case HEADER_GC_REFINE_INFORMATION:
				ret = RecvRefineInformationPacket();
				break;

			case HEADER_GC_REFINE_INFORMATION_NEW:
				ret = RecvRefineInformationPacketNew();
				break;

			case HEADER_GC_SEPCIAL_EFFECT:
				ret = RecvSpecialEffect();
				break;

			case HEADER_GC_NPC_POSITION:
				ret = RecvNPCList();
				break;

			case HEADER_GC_CHANNEL:
				ret = RecvChannelPacket();
				break;

			case HEADER_GC_VIEW_EQUIP:
				ret = RecvViewEquipPacket();
				break;

			case HEADER_GC_LAND_LIST:
				ret = RecvLandPacket();
				break;

			//case HEADER_GC_TARGET_CREATE:
			//	ret = RecvTargetCreatePacket();
			//	break;

			case HEADER_GC_TARGET_CREATE_NEW:
				ret = RecvTargetCreatePacketNew();
				break;

			case HEADER_GC_TARGET_UPDATE:
				ret = RecvTargetUpdatePacket();
				break;

			case HEADER_GC_TARGET_DELETE:
				ret = RecvTargetDeletePacket();
				break;

			case HEADER_GC_AFFECT_ADD:
				ret = RecvAffectAddPacket();
				break;

			case HEADER_GC_AFFECT_REMOVE:
				ret = RecvAffectRemovePacket();
				break;

			case HEADER_GC_MALL_OPEN:
				ret = RecvMallOpenPacket();
				break;

			case HEADER_GC_MALL_SET:
				ret = RecvMallItemSetPacket();
				break;

			case HEADER_GC_MALL_DEL:
				ret = RecvMallItemDelPacket();
				break;

			case HEADER_GC_LOVER_INFO:
				ret = RecvLoverInfoPacket();
				break;

			case HEADER_GC_LOVE_POINT_UPDATE:
				ret = RecvLovePointUpdatePacket();
				break;

			case HEADER_GC_DIG_MOTION:
				ret = RecvDigMotionPacket();
				break;

			case HEADER_GC_HANDSHAKE:
				RecvHandshakePacket();
				return;
				break;

			case HEADER_GC_HANDSHAKE_OK:
				RecvHandshakeOKPacket();
				return;
				break;

#ifdef ENABLE_ITEMSHOP
			case HEADER_GC_ITEMSHOP:
			{
				ret = RecvItemShop();
				break;
			}
#endif

			case HEADER_GC_HYBRIDCRYPT_KEYS:
				RecvHybridCryptKeyPacket();
				return;
				break;

			case HEADER_GC_HYBRIDCRYPT_SDB:
				RecvHybridCryptSDBPacket();
				return;
				break;

#ifdef _IMPROVED_PACKET_ENCRYPTION_
			case HEADER_GC_KEY_AGREEMENT:
				RecvKeyAgreementPacket();
				return;
				break;

			case HEADER_GC_KEY_AGREEMENT_COMPLETED:
				RecvKeyAgreementCompletedPacket();
				return;
				break;
#endif

			case HEADER_GC_SPECIFIC_EFFECT:
				ret = RecvSpecificEffect();
				break;

			case HEADER_GC_DRAGON_SOUL_REFINE:
				ret = RecvDragonSoulRefine();
				break;
#ifdef ENABLE_NOTIFICATION_SYSTEM
			case HEADER_GC_NOTIFICATION:
				ret = RecvNotification();
				break;
#endif
				
#ifdef ENABLE_PVP_RANKING
			case HEADER_GC_PVP_RANKING:
				ret = RecvPvPRankingPacket();
				break;
#endif
				
#ifdef ENABLE_OFFLINE_SHOP
			case HEADER_GC_NEW_OFFLINESHOP:
				ret = RecvOfflineshopPacket();
				break;
#endif

#ifdef ENABLE_ENTITY_PRELOADING
			case HEADER_GC_PRELOAD_ENTITIES:
				ret = RecvPreloadEntitiesPacket();
				break;
#endif
#ifdef ENABLE_SAVE_LOCATION_SYSTEM
			case HEADER_GC_SAVE_LOCATION:
				ret = RecvSaveLocationPacket();
				break;
#endif

#ifdef ENABLE_AURA_SYSTEM
			case HEADER_GC_AURA:
				ret = RecvAuraPacket();
				break;
#endif
#ifdef ENABLE_REMOVE_ITEM
			case HEADER_GC_REMOVE_ITEM:
				ret = RecvRemoveItemPacket();
				break;
#endif
#ifdef ENABLE_ODLAMKI_SYSTEM
			case HEADER_GC_ODLAMKI_ITEM:
				ret = RecvOdlamkiItemPacket();
				break;
#endif
#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
			case HEADER_GC_CUBE_RENEWAL:
				ret = RecvCubeRenewalPacket();
				break;
#endif
#ifdef ENABLE_RESP_SYSTEM
			case HEADER_GC_RESP:
				ret = RecvRespPacket();
				break;
#endif
#ifdef ENABLE_SWITCHBOT
			case HEADER_GC_SWITCHBOT:
				ret = RecvSwitchbotPacket();
				break;
#endif
#ifdef ENABLE_IN_GAME_LOG_SYSTEM
			case HEADER_GC_IN_GAME_LOG:
				ret = RecvInGameLog();
				break;
#endif
#ifdef RANKING_SYSTEM
			case HEADER_GC_SERVER_RANKING_LOAD:
				ret = RecvLoadServerRanking();
				break;
			case HEADER_GC_RANKING_LOAD:
				ret = RecvLoadRanking();
				break;
#endif
#ifdef ENABLE_SECONDARY_LEVEL
			case HEADER_GC_SECONDARY_LEVEL:
				ret = RecvSecondaryLevelPacket();
				break;
#endif
#ifdef __INVENTORY_BUFFERING__
			case HEADER_GC_ITEM_BUFFERED:
				ret = RecvBufferedInventoryPacket();
				break;
#endif
#ifdef WEEKLY_RANK_BYLUZER
			case HEADER_GC_WEEKLY_RANK:
				ret = RecvWeeklyRank();
				break;
			case HEADER_GC_WEEKLY_PAGE:
				ret = RecvWeeklyPage();
				break;
#endif
#ifdef TITLE_SYSTEM_BYLUZER
			case HEADER_GC_TITLE_ACTIVE:
				ret = RecvTitleActive();
				break;

			case HEADER_GC_TITLE_INFO:
				ret = RecvTitleInfo();
				break;
#endif

#ifdef ENABLE_COLLECT_WINDOW
			case HEADER_GC_COLLECT:
				ret = RecvCollectPacket();
				break;
#endif

			case HEADER_GC_ANTYEXP:
				ret = RecvAntyExp();
				break;

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
			case HEADER_GC_GOLD:
				{
					TPacketGold packetGold;
					ret = Recv(sizeof(packetGold), &packetGold);
				}
				break;
#endif

#ifdef WJ_SPLIT_INVENTORY_SYSTEM
			case HEADER_GC_WPADANIE:
				ret = RecvWpadanie();
				break;
#endif
			case HEADER_GC_GIVE_ITEMS:
				ret = GiveItems();
				break;

#ifdef ENABLE_TOMBOLA
			case HEADER_GC_TOMBOLA:
				ret = RecvTombolaPacket();
				break;
#endif
#ifdef ENABLE_LRN_LOCATION_SYSTEM
			case HEADER_GC_SEND_LOCATION_DATA:
				ret = RecvPlayerLocationData();
				break;
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			case HEADER_GC_ACCE:
				ret = RecvAccePacket();
				break;
#endif
#ifdef ENABLE_DROP_INFO
			case HEADER_GC_DROP_INFO:
				ret = RecvDropInfoPacket();
				break;
#endif
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
			//case HEADER_GC_MOUNT_NPC_ACTION:
			//	ret = RecvMountNPCAction();
			//	break;
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
			case HEADER_GC_BUFF_NPC_ACTION:
				ret = RecvBuffNPCAction();
				break;

			case HEADER_GC_BUFF_NPC_USE_SKILL:
				ret = RecvBuffNPCUseSkill();
				break;
#endif

#if defined(ENABLE_DUNGEON_INFO_SYSTEM)
			case DungeonInfo::Packet::HEADER_GC_DUNGEON_INFO:
				ret = RecvDungeonInfo();
				break;

			case DungeonInfo::Packet::HEADER_GC_DUNGEON_RANKING:
				ret = RecvDungeonRanking();
				break;
#endif
#ifdef ENABLE_MINIMAP_DUNGEONINFO
			case HEADER_GC_MINIMAP_DUNGEONINFO:
				ret = RecvMiniMapDungeonInfo();
				break;
#endif
#ifdef ENABLE_EVENT_MANAGER
			case HEADER_GC_EVENT_MANAGER:
				ret = RecvEventManager();
				break;
			case HEADER_GC_EVENT_ACTIVATE_RESULT:
				ret = RecvEventActivateResult();
				break;
#endif
#ifdef __ENABLE_POLYMORPH_SYSTEM__
			case HEADER_GC_POLY_SYSTEM:
				ret = RecvPolySystemPacket();
				break;
#endif
#ifdef TAKE_LEGEND_DAMAGE_BOARD_SYSTEM
			case HEADER_GC_LEGEND_DAMAGE_DATA:
				ret = RecvLegendDamageData();
				break;
#endif

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
			case HEADER_GC_PRIVATE_SHOP:
				ret = RecvPrivateShop();
				break;
#endif
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
			case HEADER_GC_TITLE_ACHIEVEMENT:
				ret = RecvTitleAchievementPacket();
				break;
#endif

			case HEADER_GC_CUBE:
				ret = PythonCube::Instance().ReceivePacket();
				break;

			default:
				ret = RecvDefaultPacket(header);
				break;
		}
#ifdef __PERFORMANCE_CHECK__
		DWORD timeEndPacket=timeGetTime();

		{
			PERF_PacketInfo& rkPacketInfo=kMap_kPacketInfo[header];
			rkPacketInfo.dwCount++;
			rkPacketInfo.dwTime+=timeEndPacket-timeBeginPacket;
		}

		{
			PERF_PacketInfo& rkPacketInfo=gs_kPacketTimeAnalyzer.m_kMap_kPacketInfo[header];
			rkPacketInfo.dwCount++;
			rkPacketInfo.dwTime+=timeEndPacket-timeBeginPacket;
		}
#endif
	}

#ifdef __PERFORMANCE_CHECK__
	DWORD timeEndDispatch=timeGetTime();

	if (timeEndDispatch-timeBeginDispatch>2)
	{
		static FILE* fp=fopen("perf_dispatch_packet.txt", "w");

		fprintf(fp, "delay %d\n", timeEndDispatch-timeBeginDispatch);
		for (std::map<DWORD, PERF_PacketInfo>::iterator i=kMap_kPacketInfo.begin(); i!=kMap_kPacketInfo.end(); ++i)
		{
			if (i->second.dwTime>0)
				fprintf(fp, "header %d: count %d, time %d\n", i->first, i->second.dwCount, i->second.dwTime);
		}
		fputs("=====================================================\n", fp);
		fflush(fp);
	}
#endif

	if (!ret)
		RecvErrorPacket(header);

	static DWORD s_nextRefreshTime = ELTimer_GetMSec();

	DWORD curTime = ELTimer_GetMSec();
	if (s_nextRefreshTime > curTime)
		return;

	if (m_isRefreshCharacterWnd)
	{
		m_isRefreshCharacterWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshCharacter", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshEquipmentWnd)
	{
		m_isRefreshEquipmentWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshEquipment", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshInventoryWnd)
	{
		m_isRefreshInventoryWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshInventory", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshExchangeWnd)
	{
		m_isRefreshExchangeWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshExchange", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshSkillWnd)
	{
		m_isRefreshSkillWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSkill", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshSafeboxWnd)
	{
		m_isRefreshSafeboxWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSafebox", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshMallWnd)
	{
		m_isRefreshMallWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshMall", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshStatus)
	{
		m_isRefreshStatus=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshStatus", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshMessengerWnd)
	{
		m_isRefreshMessengerWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshMessenger", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndInfoPage)
	{
		m_isRefreshGuildWndInfoPage=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildInfoPage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndDungeonInfoPage)
	{
		m_isRefreshGuildWndDungeonInfoPage = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildDungeonInfoPage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildDungeonBossDmgRankWnd)
	{
		m_isRefreshGuildDungeonBossDmgRankWnd = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildDungeonBossDmgRankWindow", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndBoardPage)
	{
		m_isRefreshGuildWndBoardPage=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildBoardPage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndMemberPage)
	{
		m_isRefreshGuildWndMemberPage=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildMemberPage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndMemberPageGradeComboBox)
	{
		m_isRefreshGuildWndMemberPageGradeComboBox=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildMemberPageGradeComboBox", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndSkillPage)
	{
		m_isRefreshGuildWndSkillPage=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildSkillPage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndGradePage)
	{
		m_isRefreshGuildWndGradePage=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildGradePage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}
#ifdef ENABLE_AFFECT_FIX
	if (m_isRefreshAffectWindow)
	{
		m_isRefreshAffectWindow = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshAffectWindow", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}
#endif

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
	if (m_isRefreshPrivateShopWindow)
	{
		m_isRefreshPrivateShopWindow = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshPrivateShopWindow", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}
#endif

#ifdef ENABLE_METIN_QUEUE
	if (CPythonPlayer::Instance().GetQueue())
	{
		CPythonPlayer::Instance().UpdateQueue();
		s_nextRefreshTime = curTime + 120;
	}
#endif
}

void CPythonNetworkStream::__InitializeGamePhase()
{
	__ServerTimeSync_Initialize();

	m_isRefreshStatus=false;
	m_isRefreshCharacterWnd=false;
	m_isRefreshEquipmentWnd=false;
	m_isRefreshInventoryWnd=false;
	m_isRefreshExchangeWnd=false;
	m_isRefreshSkillWnd=false;
	m_isRefreshSafeboxWnd=false;
	m_isRefreshMallWnd=false;
	m_isRefreshMessengerWnd=false;
	m_isRefreshGuildWndInfoPage=false;
	m_isRefreshGuildWndDungeonInfoPage = false;
	m_isRefreshGuildDungeonBossDmgRankWnd = false;
	m_isRefreshGuildWndBoardPage=false;
	m_isRefreshGuildWndMemberPage=false;
	m_isRefreshGuildWndMemberPageGradeComboBox=false;
	m_isRefreshGuildWndSkillPage=false;
	m_isRefreshGuildWndGradePage=false;
#ifdef ENABLE_AFFECT_FIX
	m_isRefreshAffectWindow = false;
#endif

	m_EmoticonStringVector.clear();

	m_pInstTarget = NULL;
}

void CPythonNetworkStream::Warp(LONG lGlobalX, LONG lGlobalY)
{
	CPythonBackground& rkBgMgr=CPythonBackground::Instance();
	rkBgMgr.Destroy();
	rkBgMgr.Create();
	rkBgMgr.Warp(lGlobalX, lGlobalY);
	//rkBgMgr.SetShadowLevel(CPythonBackground::SHADOW_ALL);
	rkBgMgr.RefreshShadowLevel();

	LONG lLocalX = lGlobalX;
	LONG lLocalY = lGlobalY;
	__GlobalPositionToLocalPosition(lLocalX, lLocalY);
	float fHeight = CPythonBackground::Instance().GetHeight(float(lLocalX), float(lLocalY));

	IAbstractApplication& rkApp=IAbstractApplication::GetSingleton();
	rkApp.SetCenterPosition(float(lLocalX), float(lLocalY), fHeight);
#ifdef ENABLE_PERFORMANCE_BOOST
	rkBgMgr.SetViewDistanceSet(CPythonBackground::DISTANCE0, 25600);
	rkBgMgr.SelectViewDistanceNum(CPythonBackground::DISTANCE0);
	CPythonApplication::Instance().SetGlobalCenterPosition(lGlobalX, lGlobalY);
	StartGame();
#endif
	__ShowMapName(lLocalX, lLocalY);
}

void CPythonNetworkStream::__ShowMapName(LONG lLocalX, LONG lLocalY)
{
	const std::string & c_rstrMapFileName = CPythonBackground::Instance().GetWarpMapName();
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ShowMapName", Py_BuildValue("(sii)", c_rstrMapFileName.c_str(), lLocalX, lLocalY));
}

void CPythonNetworkStream::__LeaveGamePhase()
{
	CInstanceBase::ClearPVPKeySystem();

	__ClearNetworkActorManager();

	m_bComboSkillFlag = FALSE;

	IAbstractCharacterManager& rkChrMgr=IAbstractCharacterManager::GetSingleton();
	rkChrMgr.Destroy();

	CPythonItem& rkItemMgr=CPythonItem::Instance();
	rkItemMgr.Destroy();
#ifdef ENABLE_OFFLINE_SHOP
#	ifdef ENABLE_SHOP_IN_CITIES
	CPythonOfflineshop::instance().DeleteEntities();
#	endif
#endif
}

void CPythonNetworkStream::SetGamePhase()
{
	if ("Game"!=m_strPhase)
		m_phaseLeaveFunc.Run();

	Tracen("");
	Tracen("## Network - Game Phase ##");
	Tracen("");

	m_strPhase = "Game";

	m_dwChangingPhaseTime = ELTimer_GetMSec();
	m_phaseProcessFunc.Set(this, &CPythonNetworkStream::GamePhase);
	m_phaseLeaveFunc.Set(this, &CPythonNetworkStream::__LeaveGamePhase);

	IAbstractPlayer & rkPlayer = IAbstractPlayer::GetSingleton();
	rkPlayer.SetMainCharacterIndex(GetMainActorVID());

	__RefreshStatus();
}

bool CPythonNetworkStream::RecvObserverAddPacket()
{
	TPacketGCObserverAdd kObserverAddPacket;
	if (!Recv(sizeof(kObserverAddPacket), &kObserverAddPacket))
		return false;

	CPythonMiniMap::Instance().AddObserver(
		kObserverAddPacket.vid,
		kObserverAddPacket.x*100.0f,
		kObserverAddPacket.y*100.0f);

	return true;
}

bool CPythonNetworkStream::RecvObserverRemovePacket()
{
	TPacketGCObserverAdd kObserverRemovePacket;
	if (!Recv(sizeof(kObserverRemovePacket), &kObserverRemovePacket))
		return false;

	CPythonMiniMap::Instance().RemoveObserver(
		kObserverRemovePacket.vid
	);

	return true;
}

bool CPythonNetworkStream::RecvObserverMovePacket()
{
	TPacketGCObserverMove kObserverMovePacket;
	if (!Recv(sizeof(kObserverMovePacket), &kObserverMovePacket))
		return false;

	CPythonMiniMap::Instance().MoveObserver(
		kObserverMovePacket.vid,
		kObserverMovePacket.x*100.0f,
		kObserverMovePacket.y*100.0f);

	return true;
}

bool CPythonNetworkStream::RecvWarpPacket()
{
	TPacketGCWarp kWarpPacket;

	if (!Recv(sizeof(kWarpPacket), &kWarpPacket))
		return false;
#if defined(ENABLE_LOADING_PERFORMANCE) && defined(__WARP_SHOWER__)
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenWarpShowerWindow", Py_BuildValue("()"));
#endif
	__DirectEnterMode_Set(m_dwSelectedCharacterIndex);

	CNetworkStream::Connect((DWORD)kWarpPacket.lAddr, kWarpPacket.wPort);

	return true;
}

bool CPythonNetworkStream::RecvDuelStartPacket()
{
	TPacketGCDuelStart kDuelStartPacket;
	if (!Recv(sizeof(kDuelStartPacket), &kDuelStartPacket))
		return false;

	DWORD count = (kDuelStartPacket.wSize - sizeof(kDuelStartPacket))/sizeof(DWORD);

	CPythonCharacterManager & rkChrMgr = CPythonCharacterManager::Instance();

	CInstanceBase* pkInstMain=rkChrMgr.GetMainInstancePtr();
	if (!pkInstMain)
	{
		TraceError("CPythonNetworkStream::RecvDuelStartPacket - MainCharacter is NULL");
		return false;
	}
	DWORD dwVIDSrc = pkInstMain->GetVirtualID();
	DWORD dwVIDDest;

	for ( DWORD i = 0; i < count; i++)
	{
		Recv(sizeof(dwVIDDest),&dwVIDDest);
		CInstanceBase::InsertDUELKey(dwVIDSrc,dwVIDDest);
	}

	if(count == 0)
		pkInstMain->SetDuelMode(CInstanceBase::DUEL_CANNOTATTACK);
	else
		pkInstMain->SetDuelMode(CInstanceBase::DUEL_START);

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseTargetBoard", Py_BuildValue("()"));

	rkChrMgr.RefreshAllPCTextTail();

	return true;
}

bool CPythonNetworkStream::RecvPVPPacket()
{
	TPacketGCPVP kPVPPacket;
	if (!Recv(sizeof(kPVPPacket), &kPVPPacket))
		return false;

	CPythonCharacterManager & rkChrMgr = CPythonCharacterManager::Instance();
	CPythonPlayer & rkPlayer = CPythonPlayer::Instance();

	switch (kPVPPacket.bMode)
	{
		case PVP_MODE_AGREE:
			rkChrMgr.RemovePVPKey(kPVPPacket.dwVIDSrc, kPVPPacket.dwVIDDst);

			if (rkPlayer.IsMainCharacterIndex(kPVPPacket.dwVIDDst))
				rkPlayer.RememberChallengeInstance(kPVPPacket.dwVIDSrc);

			if (rkPlayer.IsMainCharacterIndex(kPVPPacket.dwVIDSrc))
				rkPlayer.RememberCantFightInstance(kPVPPacket.dwVIDDst);
			break;
		case PVP_MODE_REVENGE:
		{
			rkChrMgr.RemovePVPKey(kPVPPacket.dwVIDSrc, kPVPPacket.dwVIDDst);

			DWORD dwKiller = kPVPPacket.dwVIDSrc;
			DWORD dwVictim = kPVPPacket.dwVIDDst;

			if (rkPlayer.IsMainCharacterIndex(dwVictim))
				rkPlayer.RememberRevengeInstance(dwKiller);

			if (rkPlayer.IsMainCharacterIndex(dwKiller))
				rkPlayer.RememberCantFightInstance(dwVictim);
			break;
		}

		case PVP_MODE_FIGHT:
			rkChrMgr.InsertPVPKey(kPVPPacket.dwVIDSrc, kPVPPacket.dwVIDDst);
			rkPlayer.ForgetInstance(kPVPPacket.dwVIDSrc);
			rkPlayer.ForgetInstance(kPVPPacket.dwVIDDst);
			break;
		case PVP_MODE_NONE:
			rkChrMgr.RemovePVPKey(kPVPPacket.dwVIDSrc, kPVPPacket.dwVIDDst);
			rkPlayer.ForgetInstance(kPVPPacket.dwVIDSrc);
			rkPlayer.ForgetInstance(kPVPPacket.dwVIDDst);
			break;
	}

	__RefreshTargetBoardByVID(kPVPPacket.dwVIDSrc);
	__RefreshTargetBoardByVID(kPVPPacket.dwVIDDst);

	return true;
}

// DELETEME
/*
void CPythonNetworkStream::__SendWarpPacket()
{
	TPacketCGWarp kWarpPacket;
	kWarpPacket.bHeader=HEADER_GC_WARP;
	if (!Send(sizeof(kWarpPacket), &kWarpPacket))
	{
		return;
	}
}
*/
void CPythonNetworkStream::NotifyHack(const char* c_szMsg)
{
	if (!m_kQue_stHack.empty())
		if (c_szMsg==m_kQue_stHack.back())
			return;

	m_kQue_stHack.push_back(c_szMsg);
}

bool CPythonNetworkStream::__SendHack(const char* c_szMsg)
{
	Tracen(c_szMsg);

	TPacketCGHack kPacketHack;
	kPacketHack.bHeader=HEADER_CG_HACK;
	strncpy(kPacketHack.szBuf, c_szMsg, sizeof(kPacketHack.szBuf)-1);

	if (!Send(sizeof(kPacketHack), &kPacketHack))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendMessengerAddByVIDPacket(DWORD vid)
{
	TPacketCGMessenger packet;
	packet.header = HEADER_CG_MESSENGER;
	packet.subheader = MESSENGER_SUBHEADER_CG_ADD_BY_VID;
	if (!Send(sizeof(packet), &packet))
		return false;
	if (!Send(sizeof(vid), &vid))
		return false;
	return SendSequence();
}

bool CPythonNetworkStream::SendMessengerAddByNamePacket(const char * c_szName)
{
	TPacketCGMessenger packet;
	packet.header = HEADER_CG_MESSENGER;
	packet.subheader = MESSENGER_SUBHEADER_CG_ADD_BY_NAME;
	if (!Send(sizeof(packet), &packet))
		return false;
	char szName[CHARACTER_NAME_MAX_LEN];
	strncpy(szName, c_szName, CHARACTER_NAME_MAX_LEN-1);
	szName[CHARACTER_NAME_MAX_LEN-1] = '\0';

	if (!Send(sizeof(szName), &szName))
		return false;
	Tracef(" SendMessengerAddByNamePacket : %s\n", c_szName);
	return SendSequence();
}

bool CPythonNetworkStream::SendMessengerRemovePacket(const char * c_szKey, const char * c_szName)
{
	TPacketCGMessenger packet;
	packet.header = HEADER_CG_MESSENGER;
	packet.subheader = MESSENGER_SUBHEADER_CG_REMOVE;
	if (!Send(sizeof(packet), &packet))
		return false;
	char szKey[CHARACTER_NAME_MAX_LEN];
	strncpy(szKey, c_szKey, CHARACTER_NAME_MAX_LEN-1);
	if (!Send(sizeof(szKey), &szKey))
		return false;
	__RefreshTargetBoardByName(c_szName);
	return SendSequence();
}

#ifdef ENABLE_MESSENGER_BLOCK
bool CPythonNetworkStream::SendMessengerBlockAddByVIDPacket(DWORD vid)
{
	TPacketCGMessenger packet;
	packet.header = HEADER_CG_MESSENGER;
	packet.subheader = MESSENGER_SUBHEADER_CG_BLOCK_ADD_BY_VID;
	if (!Send(sizeof(packet), &packet))
		return false;
	if (!Send(sizeof(vid), &vid))
		return false;
	return SendSequence();
}

bool CPythonNetworkStream::SendMessengerBlockRemoveByVIDPacket(DWORD vid)
{
	TPacketCGMessenger packet;
	packet.header = HEADER_CG_MESSENGER;
	packet.subheader = MESSENGER_SUBHEADER_CG_BLOCK_REMOVE_BY_VID;
	if (!Send(sizeof(packet), &packet))
		return false;
	if (!Send(sizeof(vid), &vid))
		return false;
	return SendSequence();
}

bool CPythonNetworkStream::SendMessengerBlockAddByNamePacket(const char* c_szName)
{
	TPacketCGMessenger packet;
	packet.header = HEADER_CG_MESSENGER;
	packet.subheader = MESSENGER_SUBHEADER_CG_BLOCK_ADD_BY_NAME;
	if (!Send(sizeof(packet), &packet))
		return false;
	char szName[CHARACTER_NAME_MAX_LEN];
	strncpy(szName, c_szName, CHARACTER_NAME_MAX_LEN - 1);
	szName[CHARACTER_NAME_MAX_LEN - 1] = '\0';

	if (!Send(sizeof(szName), &szName))
		return false;
#ifdef _DEBUG
	Tracef(" SendMessengerBlockAddByNamePacket : %s\n", c_szName);
#endif
	return SendSequence();
}

bool CPythonNetworkStream::SendMessengerBlockRemovePacket(const char* c_szKey, const char* c_szName)
{
	TPacketCGMessenger packet;
	packet.header = HEADER_CG_MESSENGER;
	packet.subheader = MESSENGER_SUBHEADER_CG_BLOCK_REMOVE;
	if (!Send(sizeof(packet), &packet))
		return false;
	char szKey[CHARACTER_NAME_MAX_LEN];
	strncpy(szKey, c_szKey, CHARACTER_NAME_MAX_LEN - 1);
	if (!Send(sizeof(szKey), &szKey))
		return false;
	__RefreshTargetBoardByName(c_szName);
	return SendSequence();
}
#endif

bool CPythonNetworkStream::SendCharacterStatePacket(const TPixelPosition& c_rkPPosDst, float fDstRot, UINT eFunc, UINT uArg)
{
	if (!__CanActMainInstance())
		return true;

	if (fDstRot < 0.0f)
		fDstRot = 360 + fDstRot;
	else if (fDstRot > 360.0f)
		fDstRot = fmodf(fDstRot, 360.0f);

	TPacketCGMove kStatePacket;
	kStatePacket.bHeader = HEADER_CG_CHARACTER_MOVE;
	kStatePacket.bFunc = eFunc;
	kStatePacket.bArg = uArg;
	kStatePacket.bRot = fDstRot/5.0f;
	kStatePacket.lX = long(c_rkPPosDst.x);
	kStatePacket.lY = long(c_rkPPosDst.y);
	kStatePacket.dwTime = ELTimer_GetServerMSec();

	assert(kStatePacket.lX >= 0 && kStatePacket.lX < 204800);

	__LocalPositionToGlobalPosition(kStatePacket.lX, kStatePacket.lY);

	if (!Send(sizeof(kStatePacket), &kStatePacket))
	{
		Tracenf("CPythonNetworkStream::SendCharacterStatePacket(dwCmdTime=%u, fDstPos=(%f, %f), fDstRot=%f, eFunc=%d uArg=%d) - PACKET SEND ERROR",
			kStatePacket.dwTime,
			float(kStatePacket.lX),
			float(kStatePacket.lY),
			fDstRot,
			kStatePacket.bFunc,
			kStatePacket.bArg);
		return false;
	}
	return SendSequence();
}

bool CPythonNetworkStream::SendUseSkillPacket(DWORD dwSkillIndex, DWORD dwTargetVID)
{
	TPacketCGUseSkill UseSkillPacket;
	UseSkillPacket.bHeader = HEADER_CG_USE_SKILL;
	UseSkillPacket.dwVnum = dwSkillIndex;
	UseSkillPacket.dwTargetVID = dwTargetVID;
	// tw1x1 fix wrong fly targeting for viewing clients
	if (dwTargetVID)
	{
		CPythonCharacterManager& rpcm = CPythonCharacterManager::Instance();
		CInstanceBase* pTarget = rpcm.GetInstancePtr(dwTargetVID);

		if (pTarget)
		{
			TPixelPosition kPos;

			pTarget->NEW_GetPixelPosition(&kPos);
			SendFlyTargetingPacket(dwTargetVID, kPos);
		}
		else
		{
			TPixelPosition kPos;

			kPos.x = 0;
			kPos.y = 0;
			kPos.z = 0;

			SendFlyTargetingPacket(0, kPos);
		}
	}
	// END OF tw1x1 fix wrong fly targeting for viewing clients
	if (!Send(sizeof(TPacketCGUseSkill), &UseSkillPacket))
	{
		Tracen("CPythonNetworkStream::SendUseSkillPacket - SEND PACKET ERROR");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendChatPacket(const char * c_szChat, BYTE byType)
{
	if (strlen(c_szChat) == 0)
		return true;

	if (strlen(c_szChat) >= 512)
		return true;

	if (c_szChat[0] == '/')
	{
		if (1 == strlen(c_szChat))
		{
			if (!m_strLastCommand.empty())
				c_szChat = m_strLastCommand.c_str();
		}
		else
		{
			m_strLastCommand = c_szChat;
		}
	}

	if (ClientCommand(c_szChat))
		return true;

	int iTextLen = strlen(c_szChat) + 1;
	TPacketCGChat ChatPacket;
	ChatPacket.header = HEADER_CG_CHAT;
	ChatPacket.length = sizeof(ChatPacket) + iTextLen;
	ChatPacket.type = byType;

	if (!Send(sizeof(ChatPacket), &ChatPacket))
		return false;

	if (!Send(iTextLen, c_szChat))
		return false;

	return SendSequence();
}

//////////////////////////////////////////////////////////////////////////
// Emoticon
void CPythonNetworkStream::RegisterEmoticonString(const char * pcEmoticonString)
{
	if (m_EmoticonStringVector.size() >= CInstanceBase::EMOTICON_NUM)
	{
		TraceError("Can't register emoticon string... vector is full (size:%d)", m_EmoticonStringVector.size() );
		return;
	}
	m_EmoticonStringVector.push_back(pcEmoticonString);
}

bool CPythonNetworkStream::ParseEmoticon(const char * pChatMsg, DWORD * pdwEmoticon)
{
	for (DWORD dwEmoticonIndex = 0; dwEmoticonIndex < m_EmoticonStringVector.size() ; ++dwEmoticonIndex)
	{
		if (strlen(pChatMsg) > m_EmoticonStringVector[dwEmoticonIndex].size())
			continue;

		const char * pcFind = strstr(pChatMsg, m_EmoticonStringVector[dwEmoticonIndex].c_str());

		if (pcFind != pChatMsg)
			continue;

		*pdwEmoticon = dwEmoticonIndex;

		return true;
	}

	return false;
}
// Emoticon
//////////////////////////////////////////////////////////////////////////

void CPythonNetworkStream::__ConvertEmpireText(DWORD dwEmpireID, char* szText)
{
	if (dwEmpireID<1 || dwEmpireID>3)
		return;

	UINT uHanPos;

	STextConvertTable& rkTextConvTable=m_aTextConvTable[dwEmpireID-1];

	BYTE* pbText=(BYTE*)szText;
	while (*pbText)
	{
		if (*pbText & 0x80)
		{
			if (pbText[0]>=0xb0 && pbText[0]<=0xc8 && pbText[1]>=0xa1 && pbText[1]<=0xfe)
			{
				uHanPos=(pbText[0]-0xb0)*(0xfe-0xa1+1)+(pbText[1]-0xa1);
				pbText[0]=rkTextConvTable.aacHan[uHanPos][0];
				pbText[1]=rkTextConvTable.aacHan[uHanPos][1];
			}
			pbText+=2;
		}
		else
		{
			if (*pbText>='a' && *pbText<='z')
			{
				*pbText=rkTextConvTable.acLower[*pbText-'a'];
			}
			else if (*pbText>='A' && *pbText<='Z')
			{
				*pbText=rkTextConvTable.acUpper[*pbText-'A'];
			}
			pbText++;
		}
	}
}

bool CPythonNetworkStream::RecvChatPacket()
{
	TPacketGCChat kChat;
    char buf[1024 + 1];
	char line[1024 + 1];

	if (!Recv(sizeof(kChat), &kChat))
		return false;

	UINT uChatSize=kChat.size - sizeof(kChat);

	if (!Recv(uChatSize, buf))
		return false;

	buf[uChatSize]='\0';

#if defined(__BL_CLIENT_LOCALE_STRING__)
	TTokenVector vecMultiLine;

	if (kChat.bCanFormat && CHAT_TYPE_SHOUT != kChat.type) {
		std::string sFormat{ buf };
		CPythonLocale::Instance().FormatString(sFormat);
		strncpy(buf, sFormat.c_str(), sizeof(buf));
		buf[sizeof(buf) - 1] = '\0';

		CPythonLocale::Instance().MultiLineSplit(sFormat, vecMultiLine);
	}
#endif

	if (LocaleService_IsEUROPE() && GetDefaultCodePage() == 1256)
	{
		char * p = strchr(buf, ':');
		if (p && p[1] == ' ')
			p[1] = 0x08;
	}

	if (kChat.type >= CHAT_TYPE_MAX_NUM)
		return true;

	if (CHAT_TYPE_COMMAND == kChat.type)
	{
		ServerCommand(buf);
		return true;
	}

	if (kChat.dwVID != 0)
	{
		CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
		CInstanceBase * pkInstChatter = rkChrMgr.GetInstancePtr(kChat.dwVID);
		if (NULL == pkInstChatter)
			return true;

		switch (kChat.type)
		{
		case CHAT_TYPE_TALKING:
		case CHAT_TYPE_PARTY:
		case CHAT_TYPE_GUILD:
		case CHAT_TYPE_SHOUT:
		case CHAT_TYPE_WHISPER:
			{
				char * p = strchr(buf, ':');

				if (p)
					p += 2;
				else
					p = buf;

				DWORD dwEmoticon;
#ifdef ENABLE_MESSENGER_BLOCK
				if (CPythonMessenger::Instance().IsBlockFriendByName(pkInstChatter->GetNameString()))
					return true;
#endif

				if (ParseEmoticon(p, &dwEmoticon))
				{
					pkInstChatter->SetEmoticon(dwEmoticon);
					return true;
				}
				else
				{
					if (gs_bEmpireLanuageEnable)
					{
						CInstanceBase* pkInstMain=rkChrMgr.GetMainInstancePtr();
						if (pkInstMain)
							if (!pkInstMain->IsSameEmpire(*pkInstChatter))
								__ConvertEmpireText(pkInstChatter->GetEmpireID(), p);
					}

					if (m_isEnableChatInsultFilter)
					{
						if (false == pkInstChatter->IsNPC() && false == pkInstChatter->IsEnemy())
						{
							__FilterInsult(p, strlen(p));
						}
					}

					_snprintf(line, sizeof(line), "%s", p);
				}
			}
			break;
		case CHAT_TYPE_COMMAND:
		case CHAT_TYPE_INFO:
		case CHAT_TYPE_NOTICE:
		case CHAT_TYPE_BIG_NOTICE:
		// case CHAT_TYPE_UNK_10:
#ifdef ENABLE_DICE_SYSTEM
		case CHAT_TYPE_DICE_INFO:
#endif
#if defined(ENABLE_CHATTING_WINDOW_RENEWAL)
		case CHAT_TYPE_EXP_INFO:
		case CHAT_TYPE_ITEM_INFO:
		case CHAT_TYPE_MONEY_INFO:
#endif
		case CHAT_TYPE_MAX_NUM:
		default:
			_snprintf(line, sizeof(line), "%s", buf);
			break;
		}

		if (CHAT_TYPE_SHOUT != kChat.type)
		{
			CPythonTextTail::Instance().RegisterChatTail(kChat.dwVID, line);
		}

		if (pkInstChatter->IsPC())
		{
#ifdef ENABLE_MESSENGER_BLOCK
			if (CPythonMessenger::Instance().IsBlockFriendByName(pkInstChatter->GetNameString()))
				return true;
#endif
			CPythonChat::Instance().AppendChat(kChat.type, buf);
		}
	}
	else
	{
		if (CHAT_TYPE_NOTICE == kChat.type)
		{
#if defined(__BL_CLIENT_LOCALE_STRING__)
			if (!vecMultiLine.empty()) {
				for (const std::string& sMessage : vecMultiLine)
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetTipMessage", Py_BuildValue("(s)", sMessage.c_str()));
			}
			else {
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetTipMessage", Py_BuildValue("(s)", buf));
			}
#else
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetTipMessage", Py_BuildValue("(s)", buf));
#endif
		}
		else if (CHAT_TYPE_BIG_NOTICE == kChat.type)
		{
#if defined(__BL_CLIENT_LOCALE_STRING__)
			if (!vecMultiLine.empty()) {
				for (const std::string& sMessage : vecMultiLine)
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetBigMessage", Py_BuildValue("(s)", sMessage.c_str()));
			}
			else {
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetBigMessage", Py_BuildValue("(s)", buf));
			}
#else
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetBigMessage", Py_BuildValue("(s)", buf));
#endif
		}
		else if (CHAT_TYPE_SHOUT == kChat.type)
		{
			char * p = strchr(buf, ':');

			if (p)
			{
				if (m_isEnableChatInsultFilter)
					__FilterInsult(p, strlen(p));
			}
		}

#if defined(__BL_CLIENT_LOCALE_STRING__)
		if (!vecMultiLine.empty()) {
			for (const std::string& sMessage : vecMultiLine)
				CPythonChat::Instance().AppendChat(kChat.type, sMessage.c_str());
		}
		else {
			if (buf[0] == '@')
				CPythonChat::Instance().AppendChat(kChat.type, &buf[1], IAbstractChat::ESpecialColorType::CHAT_SPECIAL_COLOR_DICE_0);
			else if (buf[0] == '$')
				CPythonChat::Instance().AppendChat(kChat.type, &buf[1], IAbstractChat::ESpecialColorType::CHAT_SPECIAL_COLOR_DICE_1);
			else
				CPythonChat::Instance().AppendChat(kChat.type, buf);
		}
#else
		CPythonChat::Instance().AppendChat(kChat.type, buf);
#endif


	}
	return true;
}

bool CPythonNetworkStream::RecvWhisperPacket()
{
	TPacketGCWhisper whisperPacket;
    char buf[512 + 1];

	if (!Recv(sizeof(whisperPacket), &whisperPacket))
		return false;

	assert(whisperPacket.wSize - sizeof(whisperPacket) < 512);

	if (!Recv(whisperPacket.wSize - sizeof(whisperPacket), &buf))
		return false;

	buf[whisperPacket.wSize - sizeof(whisperPacket)] = '\0';
#if defined(__BL_CLIENT_LOCALE_STRING__)
	if (whisperPacket.bCanFormat)
		CPythonLocale::Instance().FormatString(buf, sizeof(buf));
#endif
	static char line[256];
	if (CPythonChat::WHISPER_TYPE_CHAT == whisperPacket.bType || CPythonChat::WHISPER_TYPE_GM == whisperPacket.bType
#if defined(BL_OFFLINE_MESSAGE)
		|| CPythonChat::WHISPER_TYPE_OFFLINE == whisperPacket.bType
#endif
		)
	{
		_snprintf(line, sizeof(line), "%s : %s", whisperPacket.szNameFrom, buf);
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisper", Py_BuildValue("(iss)", (int) whisperPacket.bType, whisperPacket.szNameFrom, line));
#ifdef ENABLE_BADGE_NOTIFICATION_MANAGER
		static auto& badge_notification_manager = CBadgeNotificationManager::Instance();
		badge_notification_manager.ReceiveWhisper(whisperPacket.szNameFrom);
#endif
	}
	else if (CPythonChat::WHISPER_TYPE_SYSTEM == whisperPacket.bType || CPythonChat::WHISPER_TYPE_ERROR == whisperPacket.bType)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisperSystemMessage", Py_BuildValue("(iss)", (int) whisperPacket.bType, whisperPacket.szNameFrom, buf));
	}
	else
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisperError", Py_BuildValue("(iss)", (int) whisperPacket.bType, whisperPacket.szNameFrom, buf));
	}

	return true;
}

bool CPythonNetworkStream::SendWhisperPacket(const char * name, const char * c_szChat)
{
	if (strlen(c_szChat) >= 255)
		return true;

	int iTextLen = strlen(c_szChat) + 1;
	TPacketCGWhisper WhisperPacket;
	WhisperPacket.bHeader = HEADER_CG_WHISPER;
	WhisperPacket.wSize = sizeof(WhisperPacket) + iTextLen;

	strncpy(WhisperPacket.szNameTo, name, sizeof(WhisperPacket.szNameTo) - 1);

	if (!Send(sizeof(WhisperPacket), &WhisperPacket))
		return false;

	if (!Send(iTextLen, c_szChat))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendMobileMessagePacket(const char * name, const char * c_szChat)
{
	int iTextLen = strlen(c_szChat) + 1;
	TPacketCGSMS SMSPacket;
	SMSPacket.bHeader = HEADER_CG_SMS;
	SMSPacket.wSize = sizeof(SMSPacket) + iTextLen;

	strncpy(SMSPacket.szNameTo, name, sizeof(SMSPacket.szNameTo) - 1);

	if (!Send(sizeof(SMSPacket), &SMSPacket))
		return false;

	if (!Send(iTextLen, c_szChat))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvPointChange()
{
	TPacketGCPointChange PointChange;

	if (!Recv(sizeof(TPacketGCPointChange), &PointChange))
	{
		Tracen("Recv Point Change Packet Error");
		return false;
	}

	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
	rkChrMgr.ShowPointEffect(PointChange.Type, PointChange.dwVID);

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();

	if (pInstance && PointChange.dwVID == pInstance->GetVirtualID())
	{
		CPythonPlayer & rkPlayer = CPythonPlayer::Instance();
		rkPlayer.SetStatus(PointChange.Type, PointChange.value);

		switch (PointChange.Type)
		{
			case POINT_STAT_RESET_COUNT:
				__RefreshStatus();
				break;
			case POINT_LEVEL:
#ifdef ENABLE_SECONDARY_LEVEL
			case POINT_SECONDARY_LEVEL:
#endif
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
			case POINT_ENLIGHT_LEVEL:
#endif
			case POINT_ST:
			case POINT_DX:
			case POINT_HT:
			case POINT_IQ:
				__RefreshStatus();
				__RefreshSkillWindow();
				break;
			case POINT_SKILL:
			case POINT_SUB_SKILL:
			case POINT_HORSE_SKILL:
				__RefreshSkillWindow();
				break;
			case POINT_ENERGY:
				if (PointChange.value == 0)
				{
					rkPlayer.SetStatus(POINT_ENERGY_END_TIME, 0);
				}
				__RefreshStatus();
				break;
			default:
				__RefreshStatus();
				break;
		}

		if (POINT_GOLD == PointChange.Type)
		{
			if (PointChange.amount > 0)
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnPickMoney", Py_BuildValue("(L)", PointChange.amount));
			}
		}
#ifdef ENABLE_CHEQUE_SYSTEM
		else if (POINT_CHEQUE == PointChange.Type)
		{
			if (PointChange.amount > 0)
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnPickCheque", Py_BuildValue("(L)", PointChange.amount));
			}
		}
#endif
#ifdef ENABLE_PUNKTY_OSIAGNIEC
		else if (POINT_PKT_OSIAG == PointChange.Type)
		{
			if (PointChange.amount > 0)
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnPickPktOsiag", Py_BuildValue("(L)", PointChange.amount));
			}
		}
#endif
	}
#ifdef ENABLE_TEXT_LEVEL_REFRESH
	else
	{
		// the /advance command will provide no global refresh! it sends the pointchange only to the specific player and not all
		pInstance = CPythonCharacterManager::Instance().GetInstancePtr(PointChange.dwVID);
		if (pInstance && PointChange.Type == POINT_LEVEL)
		{
			pInstance->SetLevel(PointChange.value);
			pInstance->UpdateTextTailLevel(PointChange.value);
		}
#ifdef ENABLE_SECONDARY_LEVEL
		else if (pInstance && PointChange.Type == POINT_SECONDARY_LEVEL)
		{
			pInstance->SetSecondaryLevel(PointChange.value);
			pInstance->UpdateTextTailSecondaryLevel(PointChange.value);
		}
#endif
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
		else if (pInstance && PointChange.Type == POINT_ENLIGHT_LEVEL)
		{
			pInstance->SetEnlightLevel(PointChange.value);
			pInstance->UpdateTextTailEnlightenmentLevel(PointChange.value);
		}
#endif
	}
#endif

	return true;
}

bool CPythonNetworkStream::RecvStunPacket()
{
	TPacketGCStun StunPacket;

	if (!Recv(sizeof(StunPacket), &StunPacket))
	{
		Tracen("CPythonNetworkStream::RecvStunPacket Error");
		return false;
	}

	//Tracef("RecvStunPacket %d\n", StunPacket.vid);

	CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
	CInstanceBase * pkInstSel = rkChrMgr.GetInstancePtr(StunPacket.vid);

	if (pkInstSel)
	{
		if (CPythonCharacterManager::Instance().GetMainInstancePtr()==pkInstSel)
			pkInstSel->Die();
		else
			pkInstSel->Stun();
	}

	return true;
}

bool CPythonNetworkStream::RecvDeadPacket()
{
	TPacketGCDead DeadPacket;
	if (!Recv(sizeof(DeadPacket), &DeadPacket))
	{
		Tracen("CPythonNetworkStream::RecvDeadPacket Error");
		return false;
	}

	CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
	CInstanceBase * pkChrInstSel = rkChrMgr.GetInstancePtr(DeadPacket.vid);
	if (pkChrInstSel)
	{
		CInstanceBase* pkInstMain=rkChrMgr.GetMainInstancePtr();
		if (pkInstMain==pkChrInstSel)
		{
			Tracenf("On MainActor");
			if (false == pkInstMain->GetDuelMode())
			{
#ifndef RENEWAL_DEAD_PACKET
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnGameOver", Py_BuildValue("()"));
#else
				PyObject* times = PyTuple_New(REVIVE_TYPE_MAX);
				for (int i = REVIVE_TYPE_HERE; i < REVIVE_TYPE_MAX; i++)
					PyTuple_SetItem(times, i, PyInt_FromLong(DeadPacket.t_d[i]));
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnGameOver", Py_BuildValue("(O)", times));
#endif
			}
			CPythonPlayer::Instance().NotifyDeadMainCharacter();
		}

		pkChrInstSel->Die();
	}

	return true;
}

bool CPythonNetworkStream::SendCharacterPositionPacket(BYTE iPosition)
{
	TPacketCGPosition PositionPacket;

	PositionPacket.header = HEADER_CG_CHARACTER_POSITION;
	PositionPacket.position = iPosition;

	if (!Send(sizeof(TPacketCGPosition), &PositionPacket))
	{
		Tracen("Send Character Position Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendOnClickPacket(DWORD vid)
{
	TPacketCGOnClick OnClickPacket;
	OnClickPacket.header	= HEADER_CG_ON_CLICK;
	OnClickPacket.vid		= vid;

	if (!Send(sizeof(OnClickPacket), &OnClickPacket))
	{
		Tracen("Send On_Click Packet Error");
		return false;
	}

	Tracef("SendOnClickPacket\n");
	return SendSequence();
}

bool CPythonNetworkStream::RecvCharacterPositionPacket()
{
	TPacketGCPosition PositionPacket;

	if (!Recv(sizeof(TPacketGCPosition), &PositionPacket))
		return false;

	CInstanceBase * pChrInstance = CPythonCharacterManager::Instance().GetInstancePtr(PositionPacket.vid);

	if (!pChrInstance)
		return true;

	//pChrInstance->UpdatePosition(PositionPacket.position);

	return true;
}

#if defined(__BL_KILL_BAR__)
bool CPythonNetworkStream::RecvKillBar()
{
	TPacketGCKillBar p;

	if (!Recv(sizeof(p), &p))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AddKillInfo", 
		Py_BuildValue("(ssiii)", p.szKiller, p.szVictim, p.bKillerRace, p.bVictimRace, p.bKillerWeaponType));

	return true;
}
#endif

bool CPythonNetworkStream::RecvMotionPacket()
{
	TPacketGCMotion MotionPacket;

	if (!Recv(sizeof(TPacketGCMotion), &MotionPacket))
		return false;

	CInstanceBase * pMainInstance = CPythonCharacterManager::Instance().GetInstancePtr(MotionPacket.vid);
	CInstanceBase * pVictimInstance = NULL;

	if (0 != MotionPacket.victim_vid)
		pVictimInstance = CPythonCharacterManager::Instance().GetInstancePtr(MotionPacket.victim_vid);

	if (!pMainInstance)
		return false;

	return true;
}

bool CPythonNetworkStream::RecvShopPacket()
{
	std::vector<char> vecBuffer;
	vecBuffer.clear();

    TPacketGCShop  packet_shop;
	if (!Recv(sizeof(packet_shop), &packet_shop))
		return false;

	int iSize = packet_shop.size - sizeof(packet_shop);
	if (iSize > 0)
	{
		vecBuffer.resize(iSize);
		if (!Recv(iSize, &vecBuffer[0]))
			return false;
	}

	switch (packet_shop.subheader)
	{
		case SHOP_SUBHEADER_GC_START:
			{
				CPythonShop::Instance().Clear();

				DWORD dwVID = *(DWORD *)&vecBuffer[0];

				TPacketGCShopStart * pShopStartPacket = (TPacketGCShopStart *)&vecBuffer[4];
				for (BYTE iItemIndex = 0; iItemIndex < SHOP_HOST_ITEM_MAX_NUM; ++iItemIndex)
				{
					CPythonShop::Instance().SetItemData(iItemIndex, pShopStartPacket->items[iItemIndex]);
				}
#ifdef ENABLE_PUNKTY_OSIAGNIEC
				CPythonShop::Instance().SetPktOsiagShop(pShopStartPacket->ispktosiagshop);
#endif

				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "StartShop", Py_BuildValue("(i)", dwVID));
			}
			break;

		case SHOP_SUBHEADER_GC_START_EX:
			{
				CPythonShop::Instance().Clear();

				TPacketGCShopStartEx * pShopStartPacket = (TPacketGCShopStartEx *)&vecBuffer[0];
				size_t read_point = sizeof(TPacketGCShopStartEx);

				DWORD dwVID = pShopStartPacket->owner_vid;
				BYTE shop_tab_count = pShopStartPacket->shop_tab_count;

				CPythonShop::instance().SetTabCount(shop_tab_count);

				for (size_t i = 0; i < shop_tab_count; i++)
				{
					TPacketGCShopStartEx::TSubPacketShopTab* pPackTab = (TPacketGCShopStartEx::TSubPacketShopTab*)&vecBuffer[read_point];
					read_point += sizeof(TPacketGCShopStartEx::TSubPacketShopTab);

					CPythonShop::instance().SetTabCoinType(i, pPackTab->coin_type);
					CPythonShop::instance().SetTabName(i, pPackTab->name);

					struct packet_shop_item* item = &pPackTab->items[0];

					for (BYTE j = 0; j < SHOP_HOST_ITEM_MAX_NUM; j++)
					{
						TShopItemData* itemData = (item + j);
						CPythonShop::Instance().SetItemData(i, j, *itemData);
					}
				}

				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "StartShop", Py_BuildValue("(i)", dwVID));
			}
			break;

		case SHOP_SUBHEADER_GC_END:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "EndShop", Py_BuildValue("()"));
			break;

		case SHOP_SUBHEADER_GC_UPDATE_ITEM:
			{
				TPacketGCShopUpdateItem * pShopUpdateItemPacket = (TPacketGCShopUpdateItem *)&vecBuffer[0];
				CPythonShop::Instance().SetItemData(pShopUpdateItemPacket->pos, pShopUpdateItemPacket->item);
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshShop", Py_BuildValue("()"));
			}
			break;

		case SHOP_SUBHEADER_GC_UPDATE_PRICE:
#ifdef ENABLE_LONG_LONG
			{
			PyObject *args = PyTuple_New(1);
			PyTuple_SetItem(args, 0, PyLong_FromLongLong(*(long long *)&vecBuffer[0]));
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetShopSellingPrice", args);
			}
#else
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetShopSellingPrice", Py_BuildValue("(i)", *(int *)&vecBuffer[0]));
#endif
			break;

		case SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_MONEY"));
			break;

		case SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY_EX:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_MONEY_EX"));
			break;

#ifdef ENABLE_CHEQUE_SYSTEM
		case SHOP_SUBHEADER_GC_NOT_ENOUGH_CHEQUE:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_CHEQUE"));
			break;

		case SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY_CHEQUE:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_MONEY_CHEQUE"));
			break;
#endif

		case SHOP_SUBHEADER_GC_SOLDOUT:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "SOLDOUT"));
			break;

		case SHOP_SUBHEADER_GC_INVENTORY_FULL:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "INVENTORY_FULL"));
			break;

		case SHOP_SUBHEADER_GC_INVALID_POS:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "INVALID_POS"));
			break;
#ifdef ENABLE_PUNKTY_OSIAGNIEC
		case SHOP_SUBHEADER_GC_NOT_ENOUGH_PKT_OSIAG:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_PKT_OSIAG"));
			break;
#endif
		default:
			TraceError("CPythonNetworkStream::RecvShopPacket: Unknown subheader\n");
			break;
	}

	return true;
}

bool CPythonNetworkStream::RecvExchangePacket()
{
	TPacketGCExchange exchange_packet;

	if (!Recv(sizeof(exchange_packet), &exchange_packet))
		return false;

	switch (exchange_packet.subheader)
	{
		case EXCHANGE_SUBHEADER_GC_START:
			CPythonExchange::Instance().Clear();
			CPythonExchange::Instance().Start();
			CPythonExchange::Instance().SetSelfName(CPythonPlayer::Instance().GetName());
#ifdef ENABLE_LEVEL_IN_TRADE
			//CPythonExchange::Instance().SetSelfLevel(CPythonPlayer::Instance().GetLevel());
			CPythonExchange::Instance().SetSelfLevel(CPythonPlayer::Instance().GetStatus(POINT_LEVEL));
#endif

			{
				CInstanceBase * pCharacterInstance = CPythonCharacterManager::Instance().GetInstancePtr(exchange_packet.arg1);

				if (pCharacterInstance)
				{
					CPythonExchange::Instance().SetTargetName(pCharacterInstance->GetNameString());
#ifdef ENABLE_LEVEL_IN_TRADE
					CPythonExchange::Instance().SetTargetLevel(pCharacterInstance->GetLevel());
#endif
				}
			}

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "StartExchange", Py_BuildValue("()"));
			break;

		case EXCHANGE_SUBHEADER_GC_ITEM_ADD:
			if (exchange_packet.is_me)
			{
				int iSlotIndex = exchange_packet.arg2.cell;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
				CPythonExchange::Instance().SetItemToSelf(iSlotIndex, exchange_packet.arg1, (uint16_t)exchange_packet.arg3);
#else
				CPythonExchange::Instance().SetItemToSelf(iSlotIndex, exchange_packet.arg1, (BYTE)exchange_packet.arg3);
#endif	
				for (int i = 0; i < ITEM_SOCKET_SLOT_MAX_NUM; ++i)
					CPythonExchange::Instance().SetItemMetinSocketToSelf(iSlotIndex, i, exchange_packet.alValues[i]);
				for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
					CPythonExchange::Instance().SetItemAttributeToSelf(iSlotIndex, j, exchange_packet.aAttr[j].bType, exchange_packet.aAttr[j].sValue);
#ifdef WJ_ENABLE_TRADABLE_ICON
				PyObject* args = PyTuple_New(3);
				PyTuple_SetItem(args, 0, PyInt_FromLong(exchange_packet.arg4.window_type));
				PyTuple_SetItem(args, 1, PyInt_FromLong(exchange_packet.arg4.cell));
				PyTuple_SetItem(args, 2, PyInt_FromLong(iSlotIndex));
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AddItemToExchange", args);
#endif
			}
			else
			{
				int iSlotIndex = exchange_packet.arg2.cell;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
				CPythonExchange::Instance().SetItemToTarget(iSlotIndex, exchange_packet.arg1, (uint16_t)exchange_packet.arg3);
#else
				CPythonExchange::Instance().SetItemToTarget(iSlotIndex, exchange_packet.arg1, (BYTE)exchange_packet.arg3);
#endif
				for (int i = 0; i < ITEM_SOCKET_SLOT_MAX_NUM; ++i)
					CPythonExchange::Instance().SetItemMetinSocketToTarget(iSlotIndex, i, exchange_packet.alValues[i]);
				for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
					CPythonExchange::Instance().SetItemAttributeToTarget(iSlotIndex, j, exchange_packet.aAttr[j].bType, exchange_packet.aAttr[j].sValue);
			}

			__RefreshExchangeWindow();
			__RefreshInventoryWindow();
			break;

		case EXCHANGE_SUBHEADER_GC_ITEM_DEL:
			if (exchange_packet.is_me)
			{
#ifdef ENABLE_EXTENDED_ITEM_COUNT
			CPythonExchange::Instance().DelItemOfSelf((uint16_t)exchange_packet.arg1);
#else
			CPythonExchange::Instance().DelItemOfSelf((BYTE)exchange_packet.arg1);
#endif
			}
			else
			{
#ifdef ENABLE_EXTENDED_ITEM_COUNT
				CPythonExchange::Instance().DelItemOfTarget((uint16_t)exchange_packet.arg1);
#else
				CPythonExchange::Instance().DelItemOfTarget((BYTE)exchange_packet.arg1);
#endif
			}
			__RefreshExchangeWindow();
			__RefreshInventoryWindow();
			break;

		case EXCHANGE_SUBHEADER_GC_ELK_ADD:
			if (exchange_packet.is_me)
				CPythonExchange::Instance().SetElkToSelf(exchange_packet.arg1);
			else
				CPythonExchange::Instance().SetElkToTarget(exchange_packet.arg1);

			__RefreshExchangeWindow();
			break;

#ifdef ENABLE_CHEQUE_SYSTEM
		case EXCHANGE_SUBHEADER_GC_CHEQUE_ADD:
			if (exchange_packet.is_me)
				CPythonExchange::Instance().SetChequeToSelf(exchange_packet.arg1);
			else
				CPythonExchange::Instance().SetChequeToTarget(exchange_packet.arg1);

			__RefreshExchangeWindow();
			break;
#endif

		case EXCHANGE_SUBHEADER_GC_ACCEPT:
			if (exchange_packet.is_me)
			{
				CPythonExchange::Instance().SetAcceptToSelf((BYTE) exchange_packet.arg1);
			}
			else
			{
				CPythonExchange::Instance().SetAcceptToTarget((BYTE) exchange_packet.arg1);
			}
			__RefreshExchangeWindow();
			break;

		case EXCHANGE_SUBHEADER_GC_END:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "EndExchange", Py_BuildValue("()"));
			__RefreshInventoryWindow();
			CPythonExchange::Instance().End();
			break;

		case EXCHANGE_SUBHEADER_GC_ALREADY:
			Tracef("trade_already");
			break;

		case EXCHANGE_SUBHEADER_GC_LESS_ELK:
			Tracef("trade_less_elk");
			break;

#ifdef ENABLE_CHEQUE_SYSTEM
		case EXCHANGE_SUBHEADER_GC_LESS_CHEQUE:
			Tracef("trade_less_cheque");
			break;
#endif
	};

	return true;
}

bool CPythonNetworkStream::RecvQuestInfoPacket()
{
	TPacketGCQuestInfo QuestInfo;

	if (!Peek(sizeof(TPacketGCQuestInfo), &QuestInfo))
	{
		Tracen("Recv Quest Info Packet Error #1");
		return false;
	}

	if (!PeekNoFetch(QuestInfo.size))
	{
		Tracen("Recv Quest Info Packet Error #2");
		return false;
	}

	RecvNoFetch(sizeof(TPacketGCQuestInfo));

	const BYTE & c_rFlag = QuestInfo.flag;

	enum
	{
		QUEST_PACKET_TYPE_NONE,
		QUEST_PACKET_TYPE_BEGIN,
		QUEST_PACKET_TYPE_UPDATE,
		QUEST_PACKET_TYPE_END,
	};

	BYTE byQuestPacketType = QUEST_PACKET_TYPE_NONE;

	if (0 != (c_rFlag & QUEST_SEND_IS_BEGIN))
	{
		BYTE isBegin;
		if (!Recv(sizeof(isBegin), &isBegin))
			return false;

		if (isBegin)
			byQuestPacketType = QUEST_PACKET_TYPE_BEGIN;
		else
			byQuestPacketType = QUEST_PACKET_TYPE_END;
	}
	else
	{
		byQuestPacketType = QUEST_PACKET_TYPE_UPDATE;
	}

	// Recv Data Start
	char szTitle[30 + 1] = "";
	char szClockName[16 + 1] = "";
	int iClockValue = 0;
	char szCounterName[16 + 1] = "";
	int iCounterValue = 0;
	char szIconFileName[24 + 1] = "";

	if (0 != (c_rFlag & QUEST_SEND_TITLE))
	{
		if (!Recv(sizeof(szTitle), &szTitle))
			return false;

		szTitle[30]='\0';
#if defined(__BL_CLIENT_LOCALE_STRING__)
		CPythonLocale::Instance().FormatString(szTitle, sizeof(szTitle));
#endif
	}
	if (0 != (c_rFlag & QUEST_SEND_CLOCK_NAME))
	{
		if (!Recv(sizeof(szClockName), &szClockName))
			return false;

		szClockName[16]='\0';
#if defined(__BL_CLIENT_LOCALE_STRING__)
		CPythonLocale::Instance().FormatString(szClockName, sizeof(szClockName));
#endif
	}
	if (0 != (c_rFlag & QUEST_SEND_CLOCK_VALUE))
	{
		if (!Recv(sizeof(iClockValue), &iClockValue))
			return false;
	}
	if (0 != (c_rFlag & QUEST_SEND_COUNTER_NAME))
	{
		if (!Recv(sizeof(szCounterName), &szCounterName))
			return false;

		szCounterName[16]='\0';
#if defined(__BL_CLIENT_LOCALE_STRING__)
		CPythonLocale::Instance().FormatString(szCounterName, sizeof(szCounterName));
#endif
	}
	if (0 != (c_rFlag & QUEST_SEND_COUNTER_VALUE))
	{
		if (!Recv(sizeof(iCounterValue), &iCounterValue))
			return false;
	}
	if (0 != (c_rFlag & QUEST_SEND_ICON_FILE))
	{
		if (!Recv(sizeof(szIconFileName), &szIconFileName))
			return false;

		szIconFileName[24]='\0';
	}
	// Recv Data End

	CPythonQuest& rkQuest=CPythonQuest::Instance();

	// Process Start
	if (QUEST_PACKET_TYPE_END == byQuestPacketType)
	{
		rkQuest.DeleteQuestInstance(QuestInfo.index);
	}
	else if (QUEST_PACKET_TYPE_UPDATE == byQuestPacketType)
	{
		if (!rkQuest.IsQuest(QuestInfo.index))
		{
#ifdef ENABLE_QUEST_RENEWAL
			rkQuest.MakeQuest(QuestInfo.index, QuestInfo.c_index);
#else
			rkQuest.MakeQuest(QuestInfo.index);
#endif
		}

		if (strlen(szTitle) > 0)
			rkQuest.SetQuestTitle(QuestInfo.index, szTitle);
		if (strlen(szClockName) > 0)
			rkQuest.SetQuestClockName(QuestInfo.index, szClockName);
		if (strlen(szCounterName) > 0)
			rkQuest.SetQuestCounterName(QuestInfo.index, szCounterName);
		if (strlen(szIconFileName) > 0)
			rkQuest.SetQuestIconFileName(QuestInfo.index, szIconFileName);

		if (c_rFlag & QUEST_SEND_CLOCK_VALUE)
			rkQuest.SetQuestClockValue(QuestInfo.index, iClockValue);
		if (c_rFlag & QUEST_SEND_COUNTER_VALUE)
			rkQuest.SetQuestCounterValue(QuestInfo.index, iCounterValue);
	}
	else if (QUEST_PACKET_TYPE_BEGIN == byQuestPacketType)
	{
		CPythonQuest::SQuestInstance QuestInstance;
		QuestInstance.dwIndex = QuestInfo.index;
		QuestInstance.strTitle = szTitle;
		QuestInstance.strClockName = szClockName;
		QuestInstance.iClockValue = iClockValue;
		QuestInstance.strCounterName = szCounterName;
		QuestInstance.iCounterValue = iCounterValue;
		QuestInstance.strIconFileName = szIconFileName;
		CPythonQuest::Instance().RegisterQuestInstance(QuestInstance);
	}
	// Process Start End

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshQuest", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::RecvQuestConfirmPacket()
{
	TPacketGCQuestConfirm kQuestConfirmPacket;
	if (!Recv(sizeof(kQuestConfirmPacket), &kQuestConfirmPacket))
	{
		Tracen("RecvQuestConfirmPacket Error");
		return false;
	}
#if defined(__BL_CLIENT_LOCALE_STRING__)
	CPythonLocale::Instance().FormatString(kQuestConfirmPacket.msg, sizeof(kQuestConfirmPacket.msg));
#endif

	PyObject * poArg = Py_BuildValue("(sii)", kQuestConfirmPacket.msg, kQuestConfirmPacket.timeout, kQuestConfirmPacket.requestPID);
 	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OnQuestConfirm", poArg);
	return true;
}

bool CPythonNetworkStream::RecvRequestMakeGuild()
{
	TPacketGCBlank blank;
	if (!Recv(sizeof(blank), &blank))
	{
		Tracen("RecvRequestMakeGuild Packet Error");
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AskGuildName", Py_BuildValue("()"));

	return true;
}

void CPythonNetworkStream::ToggleGameDebugInfo()
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ToggleDebugInfo", Py_BuildValue("()"));
}

bool CPythonNetworkStream::SendExchangeStartPacket(DWORD vid)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_START;
	packet.arg1			= vid;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_start_packet Error\n");
		return false;
	}

	Tracef("send_trade_start_packet   vid %d \n", vid);
	return SendSequence();
}

#ifdef ENABLE_LONG_LONG
bool CPythonNetworkStream::SendExchangeElkAddPacket(long long elk)
#else
bool CPythonNetworkStream::SendExchangeElkAddPacket(DWORD elk)
#endif
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_ELK_ADD;
	packet.arg1			= elk;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_elk_add_packet Error\n");
		return false;
	}

	return SendSequence();
}

#ifdef ENABLE_CHEQUE_SYSTEM
bool CPythonNetworkStream::SendExchangeChequeAddPacket(long long cheque)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header = HEADER_CG_EXCHANGE;
	packet.subheader = EXCHANGE_SUBHEADER_CG_CHEQUE_ADD;
	packet.arg1 = cheque;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_cheque_add_packet Error\n");
		return false;
	}

	return SendSequence();
}
#endif

#if defined(ITEM_CHECKINOUT_UPDATE)
bool CPythonNetworkStream::SendExchangeItemAddPacket(TItemPos ItemPos, BYTE byDisplayPos, bool SelectPosAuto)
#else
bool CPythonNetworkStream::SendExchangeItemAddPacket(TItemPos ItemPos, BYTE byDisplayPos)
#endif
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_ITEM_ADD;
	packet.Pos			= ItemPos;
	packet.arg2			= byDisplayPos;
#if defined(ITEM_CHECKINOUT_UPDATE)
	packet.SelectPosAuto = SelectPosAuto;
#endif

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_item_add_packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendExchangeItemDelPacket(BYTE pos)
{
	assert(!"Can't be called function - CPythonNetworkStream::SendExchangeItemDelPacket");
	return true;

	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_ITEM_DEL;
	packet.arg1			= pos;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_item_del_packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendExchangeAcceptPacket()
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_ACCEPT;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_accept_packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendExchangeExitPacket()
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_CANCEL;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_exit_packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendPointResetPacket()
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "StartPointReset", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::__IsPlayerAttacking()
{
	CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
	CInstanceBase* pkInstMain=rkChrMgr.GetMainInstancePtr();
	if (!pkInstMain)
		return false;

	if (!pkInstMain->IsAttacking())
		return false;

	return true;
}

bool CPythonNetworkStream::RecvScriptPacket()
{
	TPacketGCScript ScriptPacket;

	if (!Recv(sizeof(TPacketGCScript), &ScriptPacket))
	{
		TraceError("RecvScriptPacket_RecvError");
		return false;
	}

	if (ScriptPacket.size < sizeof(TPacketGCScript))
	{
		TraceError("RecvScriptPacket_SizeError");
		return false;
	}

	ScriptPacket.size -= sizeof(TPacketGCScript);

	static string str;
	str = "";
	str.resize(ScriptPacket.size+1);

	if (!Recv(ScriptPacket.size, &str[0]))
		return false;

	str[str.size()-1] = '\0';
#if defined(__BL_CLIENT_LOCALE_STRING__)
	CPythonLocale::Instance().FormatString(str);
#endif
	int iIndex = CPythonEventManager::Instance().RegisterEventSetFromString(str);

	if (-1 != iIndex)
	{
		CPythonEventManager::Instance().SetVisibleLineCount(iIndex, 30);
		CPythonNetworkStream::Instance().OnScriptEventStart(ScriptPacket.skin,iIndex);
	}

	return true;
}

bool CPythonNetworkStream::SendScriptAnswerPacket(int iAnswer)
{
	TPacketCGScriptAnswer ScriptAnswer;

	ScriptAnswer.header = HEADER_CG_SCRIPT_ANSWER;
	ScriptAnswer.answer = (BYTE) iAnswer;
	if (!Send(sizeof(TPacketCGScriptAnswer), &ScriptAnswer))
	{
		Tracen("Send Script Answer Packet Error");
		return false;
	}

	return SendSequence();
}

#ifdef ENABLE_COLLECT_WINDOW
bool CPythonNetworkStream::SendScriptButtonPacket(unsigned int iIndex, BYTE bButtonIndex)
#else
bool CPythonNetworkStream::SendScriptButtonPacket(unsigned int iIndex)
#endif
{
	TPacketCGScriptButton ScriptButton;

	ScriptButton.header = HEADER_CG_SCRIPT_BUTTON;
	ScriptButton.idx = iIndex;
#ifdef ENABLE_COLLECT_WINDOW
	ScriptButton.button = bButtonIndex;
#endif
	if (!Send(sizeof(TPacketCGScriptButton), &ScriptButton))
	{
		Tracen("Send Script Button Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendAnswerMakeGuildPacket(const char * c_szName)
{
	TPacketCGAnswerMakeGuild Packet;

	Packet.header = HEADER_CG_ANSWER_MAKE_GUILD;
	strncpy(Packet.guild_name, c_szName, GUILD_NAME_MAX_LEN);
	Packet.guild_name[GUILD_NAME_MAX_LEN] = '\0';

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracen("SendAnswerMakeGuild Packet Error");
		return false;
	}

// 	Tracef(" SendAnswerMakeGuildPacket : %s", c_szName);
	return SendSequence();
}

bool CPythonNetworkStream::SendQuestInputStringPacket(const char * c_szString)
{
	TPacketCGQuestInputString Packet;
	Packet.bHeader = HEADER_CG_QUEST_INPUT_STRING;
	strncpy(Packet.szString, c_szString, QUEST_INPUT_STRING_MAX_NUM);

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracen("SendQuestInputStringPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendQuestConfirmPacket(BYTE byAnswer, DWORD dwPID)
{
	TPacketCGQuestConfirm kPacket;
	kPacket.header = HEADER_CG_QUEST_CONFIRM;
	kPacket.answer = byAnswer;
	kPacket.requestPID = dwPID;

	if (!Send(sizeof(kPacket), &kPacket))
	{
		Tracen("SendQuestConfirmPacket Error");
		return false;
	}

	Tracenf(" SendQuestConfirmPacket : %d, %d", byAnswer, dwPID);
	return SendSequence();
}

bool CPythonNetworkStream::RecvSkillCoolTimeEnd()
{
	TPacketGCSkillCoolTimeEnd kPacketSkillCoolTimeEnd;
	if (!Recv(sizeof(kPacketSkillCoolTimeEnd), &kPacketSkillCoolTimeEnd))
	{
		Tracen("CPythonNetworkStream::RecvSkillCoolTimeEnd - RecvError");
		return false;
	}

	CPythonPlayer::Instance().EndSkillCoolTime(kPacketSkillCoolTimeEnd.bSkill);

	return true;
}

bool CPythonNetworkStream::RecvSkillLevel()
{
	assert(!"CPythonNetworkStream::RecvSkillLevel - Don't use this function");
	TPacketGCSkillLevel packet;
	if (!Recv(sizeof(TPacketGCSkillLevel), &packet))
	{
		Tracen("CPythonNetworkStream::RecvSkillLevel - RecvError");
		return false;
	}

	DWORD dwSlotIndex;

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	for (int i = 0; i < SKILL_MAX_NUM; ++i)
	{
		if (rkPlayer.GetSkillSlotIndex(i, &dwSlotIndex))
			rkPlayer.SetSkillLevel(dwSlotIndex, packet.abSkillLevels[i]);
	}

	__RefreshSkillWindow();
	__RefreshStatus();
	Tracef(" >> RecvSkillLevel\n");
	return true;
}

bool CPythonNetworkStream::RecvSkillLevelNew()
{
	TPacketGCSkillLevelNew packet;

	if (!Recv(sizeof(TPacketGCSkillLevelNew), &packet))
	{
		Tracen("CPythonNetworkStream::RecvSkillLevelNew - RecvError");
		return false;
	}

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();

	rkPlayer.SetSkill(7, 0);
	rkPlayer.SetSkill(8, 0);

	for (int i = 0; i < SKILL_MAX_NUM; ++i)
	{
		TPlayerSkill & rPlayerSkill = packet.skills[i];

		if (i >= 112 && i <= 115 && rPlayerSkill.bLevel)
			rkPlayer.SetSkill(7, i);

		if (i >= 116 && i <= 119 && rPlayerSkill.bLevel)
			rkPlayer.SetSkill(8, i);

		rkPlayer.SetSkillLevel_(i, rPlayerSkill.bMasterType, rPlayerSkill.bLevel);
	}

	__RefreshSkillWindow();
	__RefreshStatus();
	//Tracef(" >> RecvSkillLevelNew\n");
	return true;
}

bool CPythonNetworkStream::RecvDamageInfoPacket()
{
	TPacketGCDamageInfo DamageInfoPacket;

	if (!Recv(sizeof(TPacketGCDamageInfo), &DamageInfoPacket))
	{
		Tracen("Recv Target Packet Error");
		return false;
	}

	CInstanceBase * pInstTarget = CPythonCharacterManager::Instance().GetInstancePtr(DamageInfoPacket.dwVID);
	bool bSelf = (pInstTarget == CPythonCharacterManager::Instance().GetMainInstancePtr());
	bool bTarget = (pInstTarget==m_pInstTarget);
	if (pInstTarget)
	{
		if(DamageInfoPacket.damage >= 0)
			pInstTarget->AddDamageEffect(DamageInfoPacket.damage,DamageInfoPacket.flag,bSelf,bTarget);
		else
			TraceError("Damage is equal or below 0.");
	}

	return true;
}
bool CPythonNetworkStream::RecvTargetPacket()
{
	TPacketGCTarget TargetPacket;
	if (!Recv(sizeof(TPacketGCTarget), &TargetPacket))
	{
		Tracen("Recv Target Packet Error");
		return false;
	}

	CInstanceBase* pInstPlayer = CPythonCharacterManager::Instance().GetMainInstancePtr();
	CInstanceBase* pInstTarget = CPythonCharacterManager::Instance().GetInstancePtr(TargetPacket.dwVID);
	if (pInstPlayer && pInstTarget)
	{
		if (!pInstTarget->IsDead())
		{
#ifdef ENABLE_VIEW_TARGET_PLAYER_HP
			if (pInstTarget->IsBuilding())
#else
			if (pInstTarget->IsPC() || pInstTarget->IsBuilding())
#endif
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseTargetBoardIfDifferent", Py_BuildValue("(i)", TargetPacket.dwVID));
			else if (pInstPlayer->CanViewTargetHP(*pInstTarget))
			{
#ifdef ENABLE_VIEW_TARGET_DECIMAL_HP
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetHPTargetBoard", Py_BuildValue("(iLL)", TargetPacket.dwVID, TargetPacket.iMinHP, TargetPacket.iMaxHP));
#else
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetHPTargetBoard", Py_BuildValue("(ii)", TargetPacket.dwVID, TargetPacket.bHPPercent));
#endif
			}
			else
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseTargetBoard", Py_BuildValue("()"));
			}

			m_pInstTarget = pInstTarget;
		}
	}
	else
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseTargetBoard", Py_BuildValue("()"));
	}

	return true;
}

bool CPythonNetworkStream::RecvMountPacket()
{
	TPacketGCMount MountPacket;

	if (!Recv(sizeof(TPacketGCMount), &MountPacket))
	{
		Tracen("Recv Mount Packet Error");
		return false;
	}

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(MountPacket.vid);

	if (pInstance)
	{
		// Mount
		if (0 != MountPacket.mount_vid)
		{
//			pInstance->Ride(MountPacket.pos, MountPacket.mount_vid);
		}
		// Unmount
		else
		{
//			pInstance->Unride(MountPacket.pos, MountPacket.x, MountPacket.y);
		}
	}

	if (CPythonPlayer::Instance().IsMainCharacterIndex(MountPacket.vid))
	{
//		CPythonPlayer::Instance().SetRidingVehicleIndex(MountPacket.mount_vid);
	}

	return true;
}

bool CPythonNetworkStream::RecvChangeSpeedPacket()
{
	TPacketGCChangeSpeed SpeedPacket;

	if (!Recv(sizeof(TPacketGCChangeSpeed), &SpeedPacket))
	{
		Tracen("Recv Speed Packet Error");
		return false;
	}

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(SpeedPacket.vid);

	if (!pInstance)
		return true;

//	pInstance->SetWalkSpeed(SpeedPacket.walking_speed);
//	pInstance->SetRunSpeed(SpeedPacket.running_speed);
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Recv

bool CPythonNetworkStream::SendAttackPacket(UINT uMotAttack, DWORD dwVIDVictim)
{
	if (!__CanActMainInstance())
		return true;

#ifdef ENABLE_BOT_CONTROL
	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(dwVIDVictim);
	if (!pInstance)
		return true;

	if (!pInstance->IsPC() && !pInstance->IsNPC())
	{
		CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
		if (rkPlayer.GetBotControlState())
		{
			if (rkPlayer.GetBotControlWindow())
				return true;

			rkPlayer.SetBotControWindow(true);
			rkPlayer.OpenBotControlWindow();
			return true;
		}
	}
#endif

#ifdef ATTACK_TIME_LOG
	static DWORD prevTime = timeGetTime();
	DWORD curTime = timeGetTime();
	TraceError("TIME: %.4f(%.4f) ATTACK_PACKET: %d TARGET: %d", curTime/1000.0f, (curTime-prevTime)/1000.0f, uMotAttack, dwVIDVictim);
	prevTime = curTime;
#endif

	TPacketCGAttack kPacketAtk;

	kPacketAtk.header = HEADER_CG_ATTACK;
	kPacketAtk.bType = uMotAttack;
	kPacketAtk.dwVictimVID = dwVIDVictim;

	if (!SendSpecial(sizeof(kPacketAtk), &kPacketAtk))
	{
		Tracen("Send Battle Attack Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendSpecial(int nLen, void * pvBuf)
{
	BYTE bHeader = *(BYTE *) pvBuf;

	switch (bHeader)
	{
		case HEADER_CG_ATTACK:
			{
				TPacketCGAttack * pkPacketAtk = (TPacketCGAttack *) pvBuf;
				pkPacketAtk->bCRCMagicCubeProcPiece = GetProcessCRCMagicCubePiece();
				pkPacketAtk->bCRCMagicCubeFilePiece = GetProcessCRCMagicCubePiece();
				return Send(nLen, pvBuf);
			}
			break;
	}

	return Send(nLen, pvBuf);
}

bool CPythonNetworkStream::RecvAddFlyTargetingPacket()
{
	TPacketGCFlyTargeting kPacket;
	if (!Recv(sizeof(kPacket), &kPacket))
		return false;

	__GlobalPositionToLocalPosition(kPacket.lX, kPacket.lY);

	Tracef("VID [%d] Added to target settings\n",kPacket.dwShooterVID);

	CPythonCharacterManager & rpcm = CPythonCharacterManager::Instance();

	CInstanceBase * pShooter = rpcm.GetInstancePtr(kPacket.dwShooterVID);

	if (!pShooter)
	{
#ifndef _DEBUG
		TraceError("CPythonNetworkStream::RecvFlyTargetingPacket() - dwShooterVID[%d] NOT EXIST", kPacket.dwShooterVID);
#endif
		return true;
	}

	CInstanceBase * pTarget = rpcm.GetInstancePtr(kPacket.dwTargetVID);

	if (kPacket.dwTargetVID && pTarget)
	{
		pShooter->GetGraphicThingInstancePtr()->AddFlyTarget(pTarget->GetGraphicThingInstancePtr());
	}
	else
	{
		float h = CPythonBackground::Instance().GetHeight(kPacket.lX,kPacket.lY) + 60.0f; // TEMPORARY HEIGHT
		pShooter->GetGraphicThingInstancePtr()->AddFlyTarget(D3DXVECTOR3(kPacket.lX,kPacket.lY,h));
		//pShooter->GetGraphicThingInstancePtr()->SetFlyTarget(kPacket.kPPosTarget.x,kPacket.kPPosTarget.y,);
	}

	return true;
}

bool CPythonNetworkStream::RecvFlyTargetingPacket()
{
	TPacketGCFlyTargeting kPacket;
	if (!Recv(sizeof(kPacket), &kPacket))
		return false;

	__GlobalPositionToLocalPosition(kPacket.lX, kPacket.lY);

	//Tracef("CPythonNetworkStream::RecvFlyTargetingPacket - VID [%d]\n",kPacket.dwShooterVID);

	CPythonCharacterManager & rpcm = CPythonCharacterManager::Instance();

	CInstanceBase * pShooter = rpcm.GetInstancePtr(kPacket.dwShooterVID);

	if (!pShooter)
	{
#ifdef _DEBUG
		TraceError("CPythonNetworkStream::RecvFlyTargetingPacket() - dwShooterVID[%d] NOT EXIST", kPacket.dwShooterVID);
#endif
		return true;
	}

	CInstanceBase * pTarget = rpcm.GetInstancePtr(kPacket.dwTargetVID);

	if (kPacket.dwTargetVID && pTarget)
	{
		pShooter->GetGraphicThingInstancePtr()->SetFlyTarget(pTarget->GetGraphicThingInstancePtr());
	}
	else
	{
		float h = CPythonBackground::Instance().GetHeight(kPacket.lX, kPacket.lY) + 60.0f; // TEMPORARY HEIGHT
		pShooter->GetGraphicThingInstancePtr()->SetFlyTarget(D3DXVECTOR3(kPacket.lX,kPacket.lY,h));
		//pShooter->GetGraphicThingInstancePtr()->SetFlyTarget(kPacket.kPPosTarget.x,kPacket.kPPosTarget.y,);
	}

	return true;
}

bool CPythonNetworkStream::SendShootPacket(UINT uSkill)
{
	TPacketCGShoot kPacketShoot;
	kPacketShoot.bHeader=HEADER_CG_SHOOT;
	kPacketShoot.bType=uSkill;

	if (!Send(sizeof(kPacketShoot), &kPacketShoot))
	{
		Tracen("SendShootPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendAddFlyTargetingPacket(DWORD dwTargetVID, const TPixelPosition & kPPosTarget)
{
	TPacketCGFlyTargeting packet;

	//CPythonCharacterManager & rpcm = CPythonCharacterManager::Instance();

	packet.bHeader	= HEADER_CG_ADD_FLY_TARGETING;
	packet.dwTargetVID = dwTargetVID;
	packet.lX = kPPosTarget.x;
	packet.lY = kPPosTarget.y;

	__LocalPositionToGlobalPosition(packet.lX, packet.lY);

	if (!Send(sizeof(packet), &packet))
	{
		Tracen("Send FlyTargeting Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendFlyTargetingPacket(DWORD dwTargetVID, const TPixelPosition & kPPosTarget)
{
	TPacketCGFlyTargeting packet;

	//CPythonCharacterManager & rpcm = CPythonCharacterManager::Instance();

	packet.bHeader	= HEADER_CG_FLY_TARGETING;
	packet.dwTargetVID = dwTargetVID;
	packet.lX = kPPosTarget.x;
	packet.lY = kPPosTarget.y;

	__LocalPositionToGlobalPosition(packet.lX, packet.lY);

	if (!Send(sizeof(packet), &packet))
	{
		Tracen("Send FlyTargeting Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvCreateFlyPacket()
{
	TPacketGCCreateFly kPacket;
	if (!Recv(sizeof(TPacketGCCreateFly), &kPacket))
		return false;

	CFlyingManager& rkFlyMgr = CFlyingManager::Instance();
	CPythonCharacterManager & rkChrMgr = CPythonCharacterManager::Instance();

	CInstanceBase * pkStartInst = rkChrMgr.GetInstancePtr(kPacket.dwStartVID);
	CInstanceBase * pkEndInst = rkChrMgr.GetInstancePtr(kPacket.dwEndVID);
	if (!pkStartInst || !pkEndInst)
		return true;

	rkFlyMgr.CreateIndexedFly(kPacket.bType, pkStartInst->GetGraphicThingInstancePtr(), pkEndInst->GetGraphicThingInstancePtr());

	return true;
}

bool CPythonNetworkStream::SendTargetPacket(DWORD dwVID)
{
	TPacketCGTarget packet;
	packet.header = HEADER_CG_TARGET;
	packet.dwVID = dwVID;

	if (!Send(sizeof(packet), &packet))
	{
		Tracen("Send Target Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendSyncPositionElementPacket(DWORD dwVictimVID, DWORD dwVictimX, DWORD dwVictimY)
{
	TPacketCGSyncPositionElement kSyncPos;
	kSyncPos.dwVID=dwVictimVID;
	kSyncPos.lX=dwVictimX;
	kSyncPos.lY=dwVictimY;

	__LocalPositionToGlobalPosition(kSyncPos.lX, kSyncPos.lY);

	if (!Send(sizeof(kSyncPos), &kSyncPos))
	{
		Tracen("CPythonNetworkStream::SendSyncPositionElementPacket - ERROR");
		return false;
	}

	return true;
}

bool CPythonNetworkStream::RecvMessenger()
{
    TPacketGCMessenger p;
	if (!Recv(sizeof(p), &p))
		return false;

	int iSize = p.size - sizeof(p);
	char char_name[24+1];

	switch (p.subheader)
	{
#ifdef ENABLE_MESSENGER_TEAM
		case MESSENGER_SUBHEADER_GC_TEAM_LIST:
		{
			TPacketGCMessengerTeamListOnline on;
			while (iSize)
			{
				if (!Recv(sizeof(TPacketGCMessengerTeamListOffline), &on))
					return false;

				if (!Recv(on.length, char_name))
					return false;

				char_name[on.length] = 0;

				if (on.connected & MESSENGER_CONNECTED_STATE_ONLINE)
					CPythonMessenger::Instance().OnTeamLogin(char_name);
				else
					CPythonMessenger::Instance().OnTeamLogout(char_name);

				iSize -= sizeof(TPacketGCMessengerTeamListOffline);
				iSize -= on.length;
			}
			break;
		}

		case MESSENGER_SUBHEADER_GC_TEAM_LOGIN:
		{
			TPacketGCMessengerLogin p;
			if (!Recv(sizeof(p), &p))
				return false;
			if (!Recv(p.length, char_name))
				return false;
			char_name[p.length] = 0;
			CPythonMessenger::Instance().OnTeamLogin(char_name);
			__RefreshTargetBoardByName(char_name);
			break;
		}

		case MESSENGER_SUBHEADER_GC_TEAM_LOGOUT:
		{
			TPacketGCMessengerLogout logout;
			if (!Recv(sizeof(logout), &logout))
				return false;
			if (!Recv(logout.length, char_name))
				return false;
			char_name[logout.length] = 0;
			CPythonMessenger::Instance().OnTeamLogout(char_name);
			break;
		}
#endif
		case MESSENGER_SUBHEADER_GC_LIST:
		{
			TPacketGCMessengerListOnline on;
			while(iSize)
			{
				if (!Recv(sizeof(TPacketGCMessengerListOffline),&on))
					return false;

				if (!Recv(on.length, char_name))
					return false;

				char_name[on.length] = 0;

				if (on.connected & MESSENGER_CONNECTED_STATE_ONLINE)
					CPythonMessenger::Instance().OnFriendLogin(char_name);
				else
					CPythonMessenger::Instance().OnFriendLogout(char_name);

				if (on.connected & MESSENGER_CONNECTED_STATE_MOBILE)
					CPythonMessenger::Instance().SetMobile(char_name, TRUE);

				iSize -= sizeof(TPacketGCMessengerListOffline);
				iSize -= on.length;
			}
			break;
		}

		case MESSENGER_SUBHEADER_GC_LOGIN:
		{
			TPacketGCMessengerLogin p;
			if (!Recv(sizeof(p),&p))
				return false;
			if (!Recv(p.length, char_name))
				return false;
			char_name[p.length] = 0;
			CPythonMessenger::Instance().OnFriendLogin(char_name);
			__RefreshTargetBoardByName(char_name);
			break;
		}

		case MESSENGER_SUBHEADER_GC_LOGOUT:
		{
			TPacketGCMessengerLogout logout;
			if (!Recv(sizeof(logout),&logout))
				return false;
			if (!Recv(logout.length, char_name))
				return false;
			char_name[logout.length] = 0;
			CPythonMessenger::Instance().OnFriendLogout(char_name);
			break;
		}

		case MESSENGER_SUBHEADER_GC_MOBILE:
		{
			BYTE byState;
			BYTE byLength;
			if (!Recv(sizeof(byState), &byState))
				return false;
			if (!Recv(sizeof(byLength), &byLength))
				return false;
			if (!Recv(byLength, char_name))
				return false;
			char_name[byLength] = 0;
			CPythonMessenger::Instance().SetMobile(char_name, byState);
			break;
		}
#ifdef ENABLE_MESSENGER_BLOCK
		case MESSENGER_SUBHEADER_GC_BLOCK_LIST:
		{
			TPacketGCMessengerBlockListOnline block_on;
			while (iSize)
			{
				if (!Recv(sizeof(TPacketGCMessengerBlockListOffline), &block_on))
					return false;

				if (!Recv(block_on.length, char_name))
					return false;

				char_name[block_on.length] = 0;

				if (block_on.connected & MESSENGER_CONNECTED_STATE_ONLINE)
					CPythonMessenger::Instance().OnBlockLogin(char_name);
				else
					CPythonMessenger::Instance().OnBlockLogout(char_name);

				iSize -= sizeof(TPacketGCMessengerBlockListOffline);
				iSize -= block_on.length;
			}
			break;
		}

		case MESSENGER_SUBHEADER_GC_BLOCK_LOGIN:
		{
			TPacketGCMessengerLogin block_p;
			if (!Recv(sizeof(block_p), &block_p))
				return false;
			if (!Recv(block_p.length, char_name))
				return false;
			char_name[block_p.length] = 0;
			CPythonMessenger::Instance().OnBlockLogin(char_name);
			__RefreshTargetBoardByName(char_name);
			break;
		}

		case MESSENGER_SUBHEADER_GC_BLOCK_LOGOUT:
		{
			TPacketGCMessengerLogout block_logout;
			if (!Recv(sizeof(block_logout), &block_logout))
				return false;
			if (!Recv(block_logout.length, char_name))
				return false;
			char_name[block_logout.length] = 0;
			CPythonMessenger::Instance().OnBlockLogout(char_name);
			break;
		}

		case MESSENGER_SUBHEADER_GC_REMOVE_BLOCK:
		{
			BYTE bLength;
			if (!Recv(sizeof(bLength), &bLength))
				return false;

			if (!Recv(bLength, char_name))
				return false;

			char_name[bLength] = 0;
			CPythonMessenger::Instance().RemoveBlock(char_name);
			__RefreshTargetBoardByName(char_name);
			break;
		}
#endif
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// Party

bool CPythonNetworkStream::SendPartyInvitePacket(DWORD dwVID)
{
	TPacketCGPartyInvite kPartyInvitePacket;
	kPartyInvitePacket.header = HEADER_CG_PARTY_INVITE;
	kPartyInvitePacket.vid = dwVID;

	if (!Send(sizeof(kPartyInvitePacket), &kPartyInvitePacket))
	{
		Tracenf("CPythonNetworkStream::SendPartyInvitePacket [%ud] - PACKET SEND ERROR", dwVID);
		return false;
	}

	Tracef(" << SendPartyInvitePacket : %d\n", dwVID);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartyInviteAnswerPacket(DWORD dwLeaderVID, BYTE byAnswer)
{
	TPacketCGPartyInviteAnswer kPartyInviteAnswerPacket;
	kPartyInviteAnswerPacket.header = HEADER_CG_PARTY_INVITE_ANSWER;
	kPartyInviteAnswerPacket.leader_pid = dwLeaderVID;
	kPartyInviteAnswerPacket.accept = byAnswer;

	if (!Send(sizeof(kPartyInviteAnswerPacket), &kPartyInviteAnswerPacket))
	{
		Tracenf("CPythonNetworkStream::SendPartyInviteAnswerPacket [%ud %ud] - PACKET SEND ERROR", dwLeaderVID, byAnswer);
		return false;
	}

	Tracef(" << SendPartyInviteAnswerPacket : %d, %d\n", dwLeaderVID, byAnswer);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartyRemovePacket(DWORD dwPID)
{
	TPacketCGPartyRemove kPartyInviteRemove;
	kPartyInviteRemove.header = HEADER_CG_PARTY_REMOVE;
	kPartyInviteRemove.pid = dwPID;

	if (!Send(sizeof(kPartyInviteRemove), &kPartyInviteRemove))
	{
		Tracenf("CPythonNetworkStream::SendPartyRemovePacket [%ud] - PACKET SEND ERROR", dwPID);
		return false;
	}

	Tracef(" << SendPartyRemovePacket : %d\n", dwPID);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartySetStatePacket(DWORD dwVID, BYTE byState, BYTE byFlag)
{
	TPacketCGPartySetState kPartySetState;
	kPartySetState.byHeader = HEADER_CG_PARTY_SET_STATE;
	kPartySetState.dwVID = dwVID;
	kPartySetState.byState = byState;
	kPartySetState.byFlag = byFlag;

	if (!Send(sizeof(kPartySetState), &kPartySetState))
	{
		Tracenf("CPythonNetworkStream::SendPartySetStatePacket(%ud, %ud) - PACKET SEND ERROR", dwVID, byState);
		return false;
	}

	Tracef(" << SendPartySetStatePacket : %d, %d, %d\n", dwVID, byState, byFlag);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartyUseSkillPacket(BYTE bySkillIndex, DWORD dwVID)
{
	TPacketCGPartyUseSkill kPartyUseSkill;
	kPartyUseSkill.byHeader = HEADER_CG_PARTY_USE_SKILL;
	kPartyUseSkill.bySkillIndex = bySkillIndex;
	kPartyUseSkill.dwTargetVID = dwVID;

	if (!Send(sizeof(kPartyUseSkill), &kPartyUseSkill))
	{
		Tracenf("CPythonNetworkStream::SendPartyUseSkillPacket(%ud, %ud) - PACKET SEND ERROR", bySkillIndex, dwVID);
		return false;
	}

	Tracef(" << SendPartyUseSkillPacket : %d, %d\n", bySkillIndex, dwVID);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartyParameterPacket(BYTE byDistributeMode)
{
	TPacketCGPartyParameter kPartyParameter;
	kPartyParameter.bHeader = HEADER_CG_PARTY_PARAMETER;
	kPartyParameter.bDistributeMode = byDistributeMode;

	if (!Send(sizeof(kPartyParameter), &kPartyParameter))
	{
		Tracenf("CPythonNetworkStream::SendPartyParameterPacket(%d) - PACKET SEND ERROR", byDistributeMode);
		return false;
	}

	Tracef(" << SendPartyParameterPacket : %d\n", byDistributeMode);
	return SendSequence();
}

bool CPythonNetworkStream::RecvPartyInvite()
{
	TPacketGCPartyInvite kPartyInvitePacket;
	if (!Recv(sizeof(kPartyInvitePacket), &kPartyInvitePacket))
		return false;

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(kPartyInvitePacket.leader_pid);
	if (!pInstance)
	{
		TraceError(" CPythonNetworkStream::RecvPartyInvite - Failed to find leader instance [%d]\n", kPartyInvitePacket.leader_pid);
		return true;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RecvPartyInviteQuestion", Py_BuildValue("(is)", kPartyInvitePacket.leader_pid, pInstance->GetNameString()));
	Tracef(" >> RecvPartyInvite : %d, %s\n", kPartyInvitePacket.leader_pid, pInstance->GetNameString());

	return true;
}

bool CPythonNetworkStream::RecvPartyAdd()
{
	TPacketGCPartyAdd kPartyAddPacket;
	if (!Recv(sizeof(kPartyAddPacket), &kPartyAddPacket))
		return false;

	CPythonPlayer::Instance().AppendPartyMember(kPartyAddPacket.pid, kPartyAddPacket.name);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AddPartyMember", Py_BuildValue("(is)", kPartyAddPacket.pid, kPartyAddPacket.name));
	Tracef(" >> RecvPartyAdd : %d, %s\n", kPartyAddPacket.pid, kPartyAddPacket.name);

	return true;
}

bool CPythonNetworkStream::RecvPartyUpdate()
{
	TPacketGCPartyUpdate kPartyUpdatePacket;
	if (!Recv(sizeof(kPartyUpdatePacket), &kPartyUpdatePacket))
		return false;

	CPythonPlayer::TPartyMemberInfo * pPartyMemberInfo;
	if (!CPythonPlayer::Instance().GetPartyMemberPtr(kPartyUpdatePacket.pid, &pPartyMemberInfo))
		return true;

	BYTE byOldState = pPartyMemberInfo->byState;

	CPythonPlayer::Instance().UpdatePartyMemberInfo(kPartyUpdatePacket.pid, kPartyUpdatePacket.state, kPartyUpdatePacket.percent_hp);
	for (int i = 0; i < PARTY_AFFECT_SLOT_MAX_NUM; ++i)
	{
		CPythonPlayer::Instance().UpdatePartyMemberAffect(kPartyUpdatePacket.pid, i, kPartyUpdatePacket.affects[i]);
	}

#if defined(__BL_PARTY_POSITION__)
	if (kPartyUpdatePacket.x || kPartyUpdatePacket.y)
		__GlobalPositionToLocalPosition(kPartyUpdatePacket.x, kPartyUpdatePacket.y);

	CPythonPlayer::Instance().UpdatePartyMemberPosition(kPartyUpdatePacket.pid, kPartyUpdatePacket.x, kPartyUpdatePacket.y);
#endif

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "UpdatePartyMemberInfo", Py_BuildValue("(i)", kPartyUpdatePacket.pid));

	DWORD dwVID;
	if (CPythonPlayer::Instance().PartyMemberPIDToVID(kPartyUpdatePacket.pid, &dwVID))
	if (byOldState != kPartyUpdatePacket.state)
	{
		__RefreshTargetBoardByVID(dwVID);
	}

// 	Tracef(" >> RecvPartyUpdate : %d, %d, %d\n", kPartyUpdatePacket.pid, kPartyUpdatePacket.state, kPartyUpdatePacket.percent_hp);

	return true;
}

bool CPythonNetworkStream::RecvPartyRemove()
{
	TPacketGCPartyRemove kPartyRemovePacket;
	if (!Recv(sizeof(kPartyRemovePacket), &kPartyRemovePacket))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RemovePartyMember", Py_BuildValue("(i)", kPartyRemovePacket.pid));
	Tracef(" >> RecvPartyRemove : %d\n", kPartyRemovePacket.pid);

	return true;
}

bool CPythonNetworkStream::RecvPartyLink()
{
	TPacketGCPartyLink kPartyLinkPacket;
	if (!Recv(sizeof(kPartyLinkPacket), &kPartyLinkPacket))
		return false;

	CPythonPlayer::Instance().LinkPartyMember(kPartyLinkPacket.pid, kPartyLinkPacket.vid);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "LinkPartyMember", Py_BuildValue("(ii)", kPartyLinkPacket.pid, kPartyLinkPacket.vid));
	Tracef(" >> RecvPartyLink : %d, %d\n", kPartyLinkPacket.pid, kPartyLinkPacket.vid);

	return true;
}

bool CPythonNetworkStream::RecvPartyUnlink()
{
	TPacketGCPartyUnlink kPartyUnlinkPacket;
	if (!Recv(sizeof(kPartyUnlinkPacket), &kPartyUnlinkPacket))
		return false;

	CPythonPlayer::Instance().UnlinkPartyMember(kPartyUnlinkPacket.pid);

	if (CPythonPlayer::Instance().IsMainCharacterIndex(kPartyUnlinkPacket.vid))
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "UnlinkAllPartyMember", Py_BuildValue("()"));
	}
	else
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "UnlinkPartyMember", Py_BuildValue("(i)", kPartyUnlinkPacket.pid));
	}

	Tracef(" >> RecvPartyUnlink : %d, %d\n", kPartyUnlinkPacket.pid, kPartyUnlinkPacket.vid);

	return true;
}

bool CPythonNetworkStream::RecvPartyParameter()
{
	TPacketGCPartyParameter kPartyParameterPacket;
	if (!Recv(sizeof(kPartyParameterPacket), &kPartyParameterPacket))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ChangePartyParameter", Py_BuildValue("(i)", kPartyParameterPacket.bDistributeMode));
	Tracef(" >> RecvPartyParameter : %d\n", kPartyParameterPacket.bDistributeMode);

	return true;
}

// Party
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Guild

bool CPythonNetworkStream::SendGuildAddMemberPacket(DWORD dwVID)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_ADD_MEMBER;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(dwVID), &dwVID))
		return false;

	Tracef(" SendGuildAddMemberPacket\n", dwVID);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildRemoveMemberPacket(DWORD dwPID)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_REMOVE_MEMBER;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(dwPID), &dwPID))
		return false;

	Tracef(" SendGuildRemoveMemberPacket %d\n", dwPID);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChangeGradeNamePacket(BYTE byGradeNumber, const char * c_szName)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(byGradeNumber), &byGradeNumber))
		return false;

	char szName[GUILD_GRADE_NAME_MAX_LEN+1];
	strncpy(szName, c_szName, GUILD_GRADE_NAME_MAX_LEN);
	szName[GUILD_GRADE_NAME_MAX_LEN] = '\0';

	if (!Send(sizeof(szName), &szName))
		return false;

	Tracef(" SendGuildChangeGradeNamePacket %d, %s\n", byGradeNumber, c_szName);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChangeGradeAuthorityPacket(BYTE byGradeNumber, BYTE byAuthority)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(byGradeNumber), &byGradeNumber))
		return false;
	if (!Send(sizeof(byAuthority), &byAuthority))
		return false;

	Tracef(" SendGuildChangeGradeAuthorityPacket %d, %d\n", byGradeNumber, byAuthority);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildOfferPacket(DWORD dwExperience)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_OFFER;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(dwExperience), &dwExperience))
		return false;

	Tracef(" SendGuildOfferPacket %d\n", dwExperience);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildPostCommentPacket(const char * c_szMessage)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_POST_COMMENT;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	BYTE bySize = BYTE(strlen(c_szMessage)) + 1;
	if (!Send(sizeof(bySize), &bySize))
		return false;
	if (!Send(bySize, c_szMessage))
		return false;

	Tracef(" SendGuildPostCommentPacket %d, %s\n", bySize, c_szMessage);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildDeleteCommentPacket(DWORD dwIndex)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_DELETE_COMMENT;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwIndex), &dwIndex))
		return false;

	Tracef(" SendGuildDeleteCommentPacket %d\n", dwIndex);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildRefreshCommentsPacket(DWORD dwHighestIndex)
{
	static DWORD s_LastTime = timeGetTime() - 1001;

	if (timeGetTime() - s_LastTime < 1000)
		return true;
	s_LastTime = timeGetTime();

	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_REFRESH_COMMENT;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	Tracef(" SendGuildRefreshCommentPacket %d\n", dwHighestIndex);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChangeMemberGradePacket(DWORD dwPID, BYTE byGrade)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwPID), &dwPID))
		return false;
	if (!Send(sizeof(byGrade), &byGrade))
		return false;

	Tracef(" SendGuildChangeMemberGradePacket %d, %d\n", dwPID, byGrade);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildUseSkillPacket(DWORD dwSkillID, DWORD dwTargetVID)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_USE_SKILL;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwSkillID), &dwSkillID))
		return false;
	if (!Send(sizeof(dwTargetVID), &dwTargetVID))
		return false;

	Tracef(" SendGuildUseSkillPacket %d, %d\n", dwSkillID, dwTargetVID);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChangeMemberGeneralPacket(DWORD dwPID, BYTE byFlag)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwPID), &dwPID))
		return false;
	if (!Send(sizeof(byFlag), &byFlag))
		return false;

	Tracef(" SendGuildChangeMemberGeneralFlagPacket %d, %d\n", dwPID, byFlag);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildInviteAnswerPacket(DWORD dwGuildID, BYTE byAnswer)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwGuildID), &dwGuildID))
		return false;
	if (!Send(sizeof(byAnswer), &byAnswer))
		return false;

	Tracef(" SendGuildInviteAnswerPacket %d, %d\n", dwGuildID, byAnswer);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChargeGSPPacket(DWORD dwMoney)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHARGE_GSP;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwMoney), &dwMoney))
		return false;

	Tracef(" SendGuildChargeGSPPacket %d\n", dwMoney);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildDepositMoneyPacket(DWORD dwMoney)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_DEPOSIT_MONEY;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(dwMoney), &dwMoney))
		return false;

	Tracef(" SendGuildDepositMoneyPacket %d\n", dwMoney);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildWithdrawMoneyPacket(DWORD dwMoney)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_WITHDRAW_MONEY;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(dwMoney), &dwMoney))
		return false;

	Tracef(" SendGuildWithdrawMoneyPacket %d\n", dwMoney);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildNewNotePacket(const char* c_szNote)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_SET_NEW_NOTE;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	BYTE bySize = BYTE(strlen(c_szNote)) + 1;
	if (!Send(sizeof(bySize), &bySize))
		return false;
	if (!Send(bySize, c_szNote))
		return false;

	Tracef(" SendGuildNewNotePacket %d, %s\n", bySize, c_szNote);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildRefreshNotePacket()
{
	static DWORD s_LastTime = timeGetTime() - 30001;
	if (timeGetTime() - s_LastTime < 30000)
		return true;
	s_LastTime = timeGetTime();

	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_REFRESH_NOTE;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	Tracef(" SendGuildRefreshNotePacket\n");
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildLoadDungeonHistoryPacket()
{
	/*static DWORD s_LastTime = timeGetTime() - 60001;
	if (timeGetTime() - s_LastTime < 60000)
		return true;
	s_LastTime = timeGetTime();*/

	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_REFRESH_DUNGEON_HISTORY;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	Tracef("SendGuildLoadDungeonHistoryPacket\n");
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildLoadGuardItemsDataPacket()
{
	/*static DWORD s_LastTime = timeGetTime() - 60001;
	if (timeGetTime() - s_LastTime < 60000)
		return true;
	s_LastTime = timeGetTime();*/

	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_REFRESH_GUARD_ITEMS;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	Tracef(" SendGuildLoadGuardItemsDataPacket\n");
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildLoadLogsPacket(uint32_t lastLogID)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_REFRESH_LOGS;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(uint32_t), &lastLogID))
		return false;

	Tracef(" SendGuildLoadLogsPacket\n");
	return SendSequence();
}

bool CPythonNetworkStream::RecvGuild()
{
    TPacketGCGuild GuildPacket;
	if (!Recv(sizeof(GuildPacket), &GuildPacket))
		return false;

	switch(GuildPacket.subheader)
	{
		case GUILD_SUBHEADER_GC_LOGIN:
		{
			DWORD dwPID;
			if (!Recv(sizeof(DWORD), &dwPID))
				return false;

			// Messenger
			CPythonGuild::TGuildMemberData * pGuildMemberData;
			if (CPythonGuild::Instance().GetMemberDataPtrByPID(dwPID, &pGuildMemberData))
				if (0 != pGuildMemberData->strName.compare(CPythonPlayer::Instance().GetName()))
					CPythonMessenger::Instance().LoginGuildMember(pGuildMemberData->strName.c_str());

			//Tracef(" <Login> %d\n", dwPID);
			break;
		}
		case GUILD_SUBHEADER_GC_LOGOUT:
		{
			DWORD dwPID;
			if (!Recv(sizeof(DWORD), &dwPID))
				return false;

			// Messenger
			CPythonGuild::TGuildMemberData * pGuildMemberData;
			if (CPythonGuild::Instance().GetMemberDataPtrByPID(dwPID, &pGuildMemberData))
				if (0 != pGuildMemberData->strName.compare(CPythonPlayer::Instance().GetName()))
					CPythonMessenger::Instance().LogoutGuildMember(pGuildMemberData->strName.c_str());

			//Tracef(" <Logout> %d\n", dwPID);
			break;
		}
		case GUILD_SUBHEADER_GC_REMOVE:
		{
			DWORD dwPID;
			if (!Recv(sizeof(dwPID), &dwPID))
				return false;

			if (CPythonGuild::Instance().IsMainPlayer(dwPID))
			{
				CPythonGuild::Instance().Destroy();
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "DeleteGuild", Py_BuildValue("()"));
				CPythonMessenger::Instance().RemoveAllGuildMember();
				__SetGuildID(0);
				__RefreshMessengerWindow();
				__RefreshTargetBoard();
				__RefreshCharacterWindow();
			}
			else
			{
				// Get Member Name
				std::string strMemberName = "";
				CPythonGuild::TGuildMemberData * pData;
				if (CPythonGuild::Instance().GetMemberDataPtrByPID(dwPID, &pData))
				{
					strMemberName = pData->strName;
					CPythonMessenger::Instance().RemoveGuildMember(pData->strName.c_str());
				}

				CPythonGuild::Instance().RemoveMember(dwPID);

				// Refresh
				__RefreshTargetBoardByName(strMemberName.c_str());
				__RefreshGuildWindowMemberPage();
			}

			Tracef(" <Remove> %d\n", dwPID);
			break;
		}
		case GUILD_SUBHEADER_GC_LIST:
		{
			int iPacketSize = int(GuildPacket.size) - sizeof(GuildPacket);

			for (; iPacketSize > 0;)
			{
				TPacketGCGuildSubMember memberPacket;
				if (!Recv(sizeof(memberPacket), &memberPacket))
					return false;

				char szName[CHARACTER_NAME_MAX_LEN+1] = "";
				if (memberPacket.byNameFlag)
				{
					if (!Recv(sizeof(szName), &szName))
						return false;

					iPacketSize -= CHARACTER_NAME_MAX_LEN+1;
				}
				else
				{
					CPythonGuild::TGuildMemberData * pMemberData;
					if (CPythonGuild::Instance().GetMemberDataPtrByPID(memberPacket.pid, &pMemberData))
					{
						strncpy(szName, pMemberData->strName.c_str(), CHARACTER_NAME_MAX_LEN);
					}
				}

				//Tracef(" <List> %d : %s, %d (%d, %d, %d)\n", memberPacket.pid, szName, memberPacket.byGrade, memberPacket.byJob, memberPacket.byLevel, memberPacket.dwOffer);

				CPythonGuild::SGuildMemberData GuildMemberData;
				GuildMemberData.dwPID = memberPacket.pid;
				GuildMemberData.byGrade = memberPacket.byGrade;
				GuildMemberData.strName = szName;
				GuildMemberData.byJob = memberPacket.byJob;
				GuildMemberData.byLevel = memberPacket.byLevel;
				GuildMemberData.dwOffer = memberPacket.dwOffer;
				GuildMemberData.byGeneralFlag = memberPacket.byIsGeneral;
				CPythonGuild::Instance().RegisterMember(GuildMemberData);

				// Messenger
				if (strcmp(szName, CPythonPlayer::Instance().GetName()))
					CPythonMessenger::Instance().AppendGuildMember(szName);

				__RefreshTargetBoardByName(szName);

				iPacketSize -= sizeof(memberPacket);
			}

			__RefreshGuildWindowInfoPage();
			__RefreshGuildWindowMemberPage();
			__RefreshMessengerWindow();
			__RefreshCharacterWindow();
			break;
		}
		case GUILD_SUBHEADER_GC_GRADE:
		{
			BYTE byCount;
			if (!Recv(sizeof(byCount), &byCount))
				return false;

			for (BYTE i = 0; i < byCount; ++i)
			{
				BYTE byIndex;
				if (!Recv(sizeof(byCount), &byIndex))
					return false;
				TPacketGCGuildSubGrade GradePacket;
				if (!Recv(sizeof(GradePacket), &GradePacket))
					return false;
#if defined(__BL_CLIENT_LOCALE_STRING__)
				CPythonLocale::Instance().FormatString(GradePacket.grade_name, sizeof(GradePacket.grade_name));
#endif
				auto data = CPythonGuild::SGuildGradeData(GradePacket.auth_flag, GradePacket.grade_name);
				CPythonGuild::Instance().SetGradeData(byIndex, data);
				//Tracef(" <Grade> [%d/%d] : %s, %d\n", byIndex, byCount, GradePacket.grade_name, GradePacket.auth_flag);
			}
			__RefreshGuildWindowGradePage();
			__RefreshGuildWindowMemberPageGradeComboBox();
			break;
		}
		case GUILD_SUBHEADER_GC_GRADE_NAME:
		{
			BYTE byGradeNumber;
			if (!Recv(sizeof(byGradeNumber), &byGradeNumber))
				return false;

			char szGradeName[GUILD_GRADE_NAME_MAX_LEN+1] = "";
			if (!Recv(sizeof(szGradeName), &szGradeName))
				return false;
#if defined(__BL_CLIENT_LOCALE_STRING__)
			CPythonLocale::Instance().FormatString(szGradeName, sizeof(szGradeName));
#endif

			CPythonGuild::Instance().SetGradeName(byGradeNumber, szGradeName);
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildGrade", Py_BuildValue("()"));

			Tracef(" <Change Grade Name> %d, %s\n", byGradeNumber, szGradeName);
			__RefreshGuildWindowGradePage();
			__RefreshGuildWindowMemberPageGradeComboBox();
			break;
		}
		case GUILD_SUBHEADER_GC_GRADE_AUTH:
		{
			BYTE byGradeNumber;
			if (!Recv(sizeof(byGradeNumber), &byGradeNumber))
				return false;
			BYTE byAuthorityFlag;
			if (!Recv(sizeof(byAuthorityFlag), &byAuthorityFlag))
				return false;

			CPythonGuild::Instance().SetGradeAuthority(byGradeNumber, byAuthorityFlag);
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildGrade", Py_BuildValue("()"));

			Tracef(" <Change Grade Authority> %d, %d\n", byGradeNumber, byAuthorityFlag);
			__RefreshGuildWindowGradePage();
			break;
		}
		case GUILD_SUBHEADER_GC_INFO:
		{
			TPacketGCGuildInfo GuildInfo;
			if (!Recv(sizeof(GuildInfo), &GuildInfo))
				return false;

			CPythonGuild::Instance().EnableGuild();
			CPythonGuild::TGuildInfo & rGuildInfo = CPythonGuild::Instance().GetGuildInfoRef();
			strncpy(rGuildInfo.szGuildName, GuildInfo.name, GUILD_NAME_MAX_LEN);
			rGuildInfo.szGuildName[GUILD_NAME_MAX_LEN] = '\0';

			rGuildInfo.dwGuildID = GuildInfo.guild_id;
			rGuildInfo.dwMasterPID = GuildInfo.master_pid;
			rGuildInfo.dwGuildLevel = GuildInfo.level;
			rGuildInfo.dwCurrentExperience = GuildInfo.exp;
			rGuildInfo.dwCurrentMemberCount = GuildInfo.member_count;
			rGuildInfo.dwMaxMemberCount = GuildInfo.max_member_count;
			rGuildInfo.dwGuildMoney = GuildInfo.gold;
			rGuildInfo.bHasLand = GuildInfo.hasLand;
			rGuildInfo.tickets[GUILD_TICKETS_TYPE_FREE] = GuildInfo.tickets[GUILD_TICKETS_TYPE_FREE];
			rGuildInfo.tickets[GUILD_TICKETS_TYPE_PAID] = GuildInfo.tickets[GUILD_TICKETS_TYPE_PAID];
			rGuildInfo.tickets[GUILD_TICKETS_TYPE_PAID_LIMIT] = GuildInfo.tickets[GUILD_TICKETS_TYPE_PAID_LIMIT];
			rGuildInfo.iTodayGivenExp = GuildInfo.todayGivenExp;
			rGuildInfo.iTodayGivenExpRefreshTime = GuildInfo.todayGivenExpRefreshTime;

			//Tracef(" <Info> %s, %d, %d : %d\n", GuildInfo.name, GuildInfo.master_pid, GuildInfo.level, rGuildInfo.bHasLand);
			__RefreshGuildWindowInfoPage();
			break;
		}
		case GUILD_SUBHEADER_GC_COMMENTS:
		{
			BYTE byCount;
			if (!Recv(sizeof(byCount), &byCount))
				return false;

			CPythonGuild::Instance().ClearComment();
			//Tracef(" >>> Comments Count : %d\n", byCount);

			for (BYTE i = 0; i < byCount; ++i)
			{
				DWORD dwCommentID;
				if (!Recv(sizeof(dwCommentID), &dwCommentID))
					return false;

				char szName[CHARACTER_NAME_MAX_LEN+1] = "";
				if (!Recv(sizeof(szName), &szName))
					return false;

				char szComment[GULID_COMMENT_MAX_LEN+1] = "";
				if (!Recv(sizeof(szComment), &szComment))
					return false;

				//Tracef(" [Comment-%d] : %s, %s\n", dwCommentID, szName, szComment);
				CPythonGuild::Instance().RegisterComment(dwCommentID, szName, szComment);
			}

			__RefreshGuildWindowBoardPage();
			break;
		}
		case GUILD_SUBHEADER_GC_CHANGE_EXP:
		{
			BYTE byLevel;
			if (!Recv(sizeof(byLevel), &byLevel))
				return false;
			DWORD dwEXP;
			if (!Recv(sizeof(dwEXP), &dwEXP))
				return false;
			CPythonGuild::Instance().SetGuildEXP(byLevel, dwEXP);
			Tracef(" <ChangeEXP> %d, %d\n", byLevel, dwEXP);
			__RefreshGuildWindowInfoPage();
			break;
		}
		case GUILD_SUBHEADER_GC_CHANGE_MEMBER_GRADE:
		{
			DWORD dwPID;
			if (!Recv(sizeof(dwPID), &dwPID))
				return false;
			BYTE byGrade;
			if (!Recv(sizeof(byGrade), &byGrade))
				return false;
			CPythonGuild::Instance().ChangeGuildMemberGrade(dwPID, byGrade);
			Tracef(" <ChangeMemberGrade> %d, %d\n", dwPID, byGrade);
			__RefreshGuildWindowMemberPage();
			break;
		}
		case GUILD_SUBHEADER_GC_SKILL_INFO:
		{
			CPythonGuild::TGuildSkillData & rSkillData = CPythonGuild::Instance().GetGuildSkillDataRef();
			if (!Recv(sizeof(rSkillData.bySkillPoint), &rSkillData.bySkillPoint))
				return false;
			if (!Recv(sizeof(rSkillData.bySkillLevel), rSkillData.bySkillLevel))
				return false;
			if (!Recv(sizeof(rSkillData.wGuildPoint), &rSkillData.wGuildPoint))
				return false;
			if (!Recv(sizeof(rSkillData.wMaxGuildPoint), &rSkillData.wMaxGuildPoint))
				return false;

			Tracef(" <SkillInfo> %d / %d, %d\n", rSkillData.bySkillPoint, rSkillData.wGuildPoint, rSkillData.wMaxGuildPoint);
			__RefreshGuildWindowSkillPage();
			break;
		}
		case GUILD_SUBHEADER_GC_CHANGE_MEMBER_GENERAL:
		{
			DWORD dwPID;
			if (!Recv(sizeof(dwPID), &dwPID))
				return false;
			BYTE byFlag;
			if (!Recv(sizeof(byFlag), &byFlag))
				return false;

			CPythonGuild::Instance().ChangeGuildMemberGeneralFlag(dwPID, byFlag);
			Tracef(" <ChangeMemberGeneralFlag> %d, %d\n", dwPID, byFlag);
			__RefreshGuildWindowMemberPage();
			break;
		}
		case GUILD_SUBHEADER_GC_GUILD_INVITE:
		{
			DWORD dwGuildID;
			if (!Recv(sizeof(dwGuildID), &dwGuildID))
				return false;
			char szGuildName[GUILD_NAME_MAX_LEN+1];
			if (!Recv(GUILD_NAME_MAX_LEN, &szGuildName))
				return false;

			szGuildName[GUILD_NAME_MAX_LEN] = 0;

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RecvGuildInviteQuestion", Py_BuildValue("(is)", dwGuildID, szGuildName));
			Tracef(" <Guild Invite> %d, %s\n", dwGuildID, szGuildName);
			break;
		}
		case GUILD_SUBHEADER_GC_WAR:
		{
			TPacketGCGuildWar kGuildWar;
			if (!Recv(sizeof(kGuildWar), &kGuildWar))
				return false;

			switch (kGuildWar.bWarState)
			{
				case GUILD_WAR_SEND_DECLARE:
					Tracef(" >> GUILD_SUBHEADER_GC_WAR : GUILD_WAR_SEND_DECLARE\n");
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
						"BINARY_GuildWar_OnSendDeclare",
						Py_BuildValue("(i)", kGuildWar.dwGuildOpp)
					);
					break;
				case GUILD_WAR_RECV_DECLARE:
					Tracef(" >> GUILD_SUBHEADER_GC_WAR : GUILD_WAR_RECV_DECLARE\n");
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
						"BINARY_GuildWar_OnRecvDeclare",
						Py_BuildValue("(ii)", kGuildWar.dwGuildOpp, kGuildWar.bType)
					);
					break;
				case GUILD_WAR_ON_WAR:
					Tracef(" >> GUILD_SUBHEADER_GC_WAR : GUILD_WAR_ON_WAR : %d, %d\n", kGuildWar.dwGuildSelf, kGuildWar.dwGuildOpp);
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
						"BINARY_GuildWar_OnStart",
						Py_BuildValue("(ii)", kGuildWar.dwGuildSelf, kGuildWar.dwGuildOpp)
					);
					CPythonGuild::Instance().StartGuildWar(kGuildWar.dwGuildOpp);
					break;
				case GUILD_WAR_END:
					Tracef(" >> GUILD_SUBHEADER_GC_WAR : GUILD_WAR_END\n");
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
						"BINARY_GuildWar_OnEnd",
						Py_BuildValue("(ii)", kGuildWar.dwGuildSelf, kGuildWar.dwGuildOpp)
					);
					CPythonGuild::Instance().EndGuildWar(kGuildWar.dwGuildOpp);
					break;
			}
			break;
		}
		case GUILD_SUBHEADER_GC_GUILD_NAME:
		{
			DWORD dwID;
			char szGuildName[GUILD_NAME_MAX_LEN+1];

			int iPacketSize = int(GuildPacket.size) - sizeof(GuildPacket);

			int nItemSize = sizeof(dwID) + GUILD_NAME_MAX_LEN;

			assert(iPacketSize%nItemSize==0 && "GUILD_SUBHEADER_GC_GUILD_NAME");

			for (; iPacketSize > 0;)
			{
				if (!Recv(sizeof(dwID), &dwID))
					return false;

				if (!Recv(GUILD_NAME_MAX_LEN, &szGuildName))
					return false;

				szGuildName[GUILD_NAME_MAX_LEN] = 0;

				//Tracef(" >> GulidName [%d : %s]\n", dwID, szGuildName);
				CPythonGuild::Instance().RegisterGuildName(dwID, szGuildName);
				iPacketSize -= nItemSize;
			}
			break;
		}
		case GUILD_SUBHEADER_GC_GUILD_WAR_LIST:
		{
			DWORD dwSrcGuildID;
			DWORD dwDstGuildID;

			int iPacketSize = int(GuildPacket.size) - sizeof(GuildPacket);
			int nItemSize = sizeof(dwSrcGuildID) + sizeof(dwDstGuildID);

			assert(iPacketSize%nItemSize==0 && "GUILD_SUBHEADER_GC_GUILD_WAR_LIST");

			for (; iPacketSize > 0;)
			{
				if (!Recv(sizeof(dwSrcGuildID), &dwSrcGuildID))
					return false;

				if (!Recv(sizeof(dwDstGuildID), &dwDstGuildID))
					return false;

				Tracef(" >> GulidWarList [%d vs %d]\n", dwSrcGuildID, dwDstGuildID);
				CInstanceBase::InsertGVGKey(dwSrcGuildID, dwDstGuildID);
				CPythonCharacterManager::Instance().ChangeGVG(dwSrcGuildID, dwDstGuildID);
				iPacketSize -= nItemSize;
			}
			break;
		}
		case GUILD_SUBHEADER_GC_GUILD_WAR_END_LIST:
		{
			DWORD dwSrcGuildID;
			DWORD dwDstGuildID;

			int iPacketSize = int(GuildPacket.size) - sizeof(GuildPacket);
			int nItemSize = sizeof(dwSrcGuildID) + sizeof(dwDstGuildID);

			assert(iPacketSize%nItemSize==0 && "GUILD_SUBHEADER_GC_GUILD_WAR_END_LIST");

			for (; iPacketSize > 0;)
			{
				if (!Recv(sizeof(dwSrcGuildID), &dwSrcGuildID))
					return false;

				if (!Recv(sizeof(dwDstGuildID), &dwDstGuildID))
					return false;

				Tracef(" >> GulidWarEndList [%d vs %d]\n", dwSrcGuildID, dwDstGuildID);
				CInstanceBase::RemoveGVGKey(dwSrcGuildID, dwDstGuildID);
				CPythonCharacterManager::Instance().ChangeGVG(dwSrcGuildID, dwDstGuildID);
				iPacketSize -= nItemSize;
			}
			break;
		}
		case GUILD_SUBHEADER_GC_WAR_POINT:
		{
			TPacketGuildWarPoint GuildWarPoint;
			if (!Recv(sizeof(GuildWarPoint), &GuildWarPoint))
				return false;

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
				"BINARY_GuildWar_OnRecvPoint",
				Py_BuildValue("(iii)", GuildWarPoint.dwGainGuildID, GuildWarPoint.dwOpponentGuildID, GuildWarPoint.lPoint)
			);
			break;
		}
		case GUILD_SUBHEADER_GC_MONEY_CHANGE:
		{
			DWORD dwMoney;
			if (!Recv(sizeof(dwMoney), &dwMoney))
				return false;

			CPythonGuild::Instance().SetGuildMoney(dwMoney);

			__RefreshGuildWindowInfoPage();
			Tracef(" >> Guild Money Change : %d\n", dwMoney);
			break;
		}

		case GUILD_SUBHEADER_GC_GUARD_ITEMS_SET_DATA:
		{
			int itemsCount = (GuildPacket.size - sizeof(GuildPacket) - sizeof(uint8_t)) / sizeof(TGuildGuardItemData);

			uint8_t setID = 0;
			if (!Recv(sizeof(uint8_t), &setID))
				return false;

			TGuildGuardItemData data;
			for (int i = 0; i < itemsCount; i++)
			{
				if (!Recv(sizeof(TGuildGuardItemData), &data))
					return false;

				CPythonGuild::Instance().SetGuardItemData(setID, data.pos, data.vnum);
			}

			__RefreshGuildWindowBoardPage();

			break;
		}

		case GUILD_SUBHEADER_GC_GUARD_ITEM_REMOVE_DATA:
		{
			uint8_t setID = 0;
			if (!Recv(sizeof(uint8_t), &setID))
				return false;

			uint8_t pos = 0;
			if (!Recv(sizeof(uint8_t), &pos))
				return false;

			CPythonGuild::Instance().SetGuardItemData(setID, pos, 0);

			__RefreshGuildWindowBoardPage();

			break;
		}

		case GUILD_SUBHEADER_GC_GUARD_BASE_INFO:
		{
			TPacketGCGuildGuardInfo GuildGuardInfo;
			if (!Recv(sizeof(GuildGuardInfo), &GuildGuardInfo))
				return false;

			CPythonGuild::TGuildInfo& rGuildInfo = CPythonGuild::Instance().GetGuildInfoRef();

			rGuildInfo.bGuardActiveSetID = GuildGuardInfo.guard_active_set_id;
			rGuildInfo.m_nextCanChangeBonusActiveStatusTime = CPythonApplication::Instance().GetServerTimeStamp() + GuildGuardInfo.bonus_can_activate_again_sec;
			if (GuildGuardInfo.guard_bonus_expire_sec > 0)
				rGuildInfo.m_guardBonusExpireTime = CPythonApplication::Instance().GetServerTimeStamp() + GuildGuardInfo.guard_bonus_expire_sec;
			else
				rGuildInfo.m_guardBonusExpireTime = 0;

			__RefreshGuildWindowBoardPage();
			break;
		}

		case GUILD_SUBHEADER_GC_STORAGE_INFO:
		{
			TPacketGCGuildStorageInfo StorageInfo;
			if (!Recv(sizeof(StorageInfo), &StorageInfo))
				return false;

			CPythonGuild::TGuildInfo& rGuildInfo = CPythonGuild::Instance().GetGuildInfoRef();
			rGuildInfo.m_donateStone   = StorageInfo.donate_stone;
			rGuildInfo.m_donateLog     = StorageInfo.donate_log;
			rGuildInfo.m_donatePlywood = StorageInfo.donate_plywood;
			rGuildInfo.m_dailyStone    = StorageInfo.daily_stone;
			rGuildInfo.m_dailyLog      = StorageInfo.daily_log;
			rGuildInfo.m_dailyPlywood  = StorageInfo.daily_plywood;
			for (int i = 0; i < 5; ++i)
				rGuildInfo.m_bonusLevel[i] = StorageInfo.bonus_level[i];

			__RefreshGuildWindowInfoPage();
			break;
		}

		case GUILD_SUBHEADER_GC_NOTE:
		{
			int noteSize = GuildPacket.size - sizeof(GuildPacket);

			char szNote[GUILD_NOTE_MAX_LEN + 1] = "";
			if (!Recv(noteSize, &szNote))
				return false;

			CPythonGuild::Instance().SetNote(szNote);
			__RefreshGuildWindowInfoPage();
			break;
		}

		case GUILD_SUBHEADER_GC_DUNGEON_OPEN_INFO:
		{
			uint8_t openedDungeonIdx = 0;
			if (!Recv(sizeof(uint8_t), &openedDungeonIdx))
				return false;

			bool isShowInfo = false;
			if (!Recv(sizeof(bool), &isShowInfo))
				return false;

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
				"BINARY_Guild_NoticeDungeonOpen",
				Py_BuildValue("(BB)", openedDungeonIdx, isShowInfo)
			);
			break;
		}

		case GUILD_SUBHEADER_GC_REFRESH_TICKETS:
		{
			CPythonGuild::TGuildInfo& rGuildInfo = CPythonGuild::Instance().GetGuildInfoRef();
			if (!Recv(sizeof(uint8_t) * GUILD_TICKETS_TYPE_COUNT, rGuildInfo.tickets))
				return false;

			__RefreshGuildWindowInfoPage();

			break;
		}

		case GUILD_SUBHEADER_GC_DUNGEON_HISTORY_DATA:
		{
			int historyCount = (GuildPacket.size - sizeof(GuildPacket)) / sizeof(TGuildDungeonHistoryInfo);

			TGuildDungeonHistoryInfo data;
			for (int i = 0; i < historyCount; i++)
			{
				if (!Recv(sizeof(TGuildDungeonHistoryInfo), &data))
					return false;

				CPythonGuild::Instance().SetDungeonHistoryInfo(&data);
			}

			__RefreshGuildWindowDungeonInfoPage();

			break;
		}

		case GUILD_SUBHEADER_GC_DUNGEON_BOSS_TOP_DMG:
		{
			int rankPlayersCount = (GuildPacket.size - sizeof(GuildPacket) - sizeof(uint64_t)) / sizeof(TGuildDungeonBossDamageInfo);

			CPythonGuild::Instance().ClearDungeonBossDmgRank();

			TGuildDungeonBossDamageInfo data;
			for (int i = 0; i < rankPlayersCount; i++)
			{
				if (!Recv(sizeof(TGuildDungeonBossDamageInfo), &data))
					return false;

				CPythonGuild::Instance().SetDungeonBossRankDmgData(i, &data);
			}

			uint64_t myDmg = 0;
			if (!Recv(sizeof(uint64_t), &myDmg))
				return false;

			CPythonGuild::Instance().SetDungeonBossRankMyDmg(myDmg);

			__RefreshGuildWindowDungeonBossDmgRankWindow();
			break;
		}

		case GUILD_SUBHEADER_GC_MY_GIVEN_EXP_UPDATE:
		{
			int todayGivenExp = 0;
			if (!Recv(sizeof(todayGivenExp), &todayGivenExp))
				return false;

			int todayGivenExpRefreshTime = 0;
			if (!Recv(sizeof(todayGivenExpRefreshTime), &todayGivenExpRefreshTime))
				return false;

			CPythonGuild::TGuildInfo& rGuildInfo = CPythonGuild::Instance().GetGuildInfoRef();

			rGuildInfo.iTodayGivenExp = todayGivenExp;
			rGuildInfo.iTodayGivenExpRefreshTime = todayGivenExpRefreshTime;
			__RefreshGuildWindowInfoPage();
			break;
		}

		case GUILD_SUBHEADER_GC_SET_LOGS:
		{
			uint32_t lastLogID = 0;
			if (!Recv(sizeof(lastLogID), &lastLogID))
				return false;

			uint32_t nextLogsRefreshTime = 0;
			if (!Recv(sizeof(nextLogsRefreshTime), &nextLogsRefreshTime))
				return false;

			int logsCount = (GuildPacket.size - sizeof(GuildPacket) - sizeof(uint32_t) - sizeof(uint32_t)) / sizeof(TGuildLog);

			CPythonGuild::Instance().ClearLogs();
			TGuildLog data;
			for (int i = 0; i < logsCount; i++)
			{
				if (!Recv(sizeof(TGuildLog), &data))
					return false;

				CPythonGuild::Instance().SetLogsData(&data);
			}

			CPythonGuild::Instance().SetLogsInfoData(lastLogID, nextLogsRefreshTime);

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Guild_LogsRefresh", Py_BuildValue("()"));

			break;
		}
	}

	return true;
}

// Guild
//////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// Fishing

bool CPythonNetworkStream::SendFishingPacket(int iRotation)
{
#ifdef ENABLE_BOT_CONTROL
	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	if (rkPlayer.GetBotControlState())
	{
		if (rkPlayer.GetBotControlWindow())
			return true;

		rkPlayer.SetBotControWindow(true);
		rkPlayer.OpenBotControlWindow();
		return true;
	}
#endif

	BYTE byHeader = HEADER_CG_FISHING;
	if (!Send(sizeof(byHeader), &byHeader))
		return false;
	BYTE byPacketRotation = iRotation / 5;
	if (!Send(sizeof(BYTE), &byPacketRotation))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendGiveItemPacket(DWORD dwTargetVID, TItemPos ItemPos, int iItemCount)
{
	TPacketCGGiveItem GiveItemPacket;
	GiveItemPacket.byHeader = HEADER_CG_GIVE_ITEM;
	GiveItemPacket.dwTargetVID = dwTargetVID;
	GiveItemPacket.ItemPos = ItemPos;
	GiveItemPacket.byItemCount = iItemCount;

	if (!Send(sizeof(GiveItemPacket), &GiveItemPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvFishing()
{
	TPacketGCFishing FishingPacket;
	if (!Recv(sizeof(FishingPacket), &FishingPacket))
		return false;

	CInstanceBase * pFishingInstance = NULL;
	if (FISHING_SUBHEADER_GC_FISH != FishingPacket.subheader)
	{
		pFishingInstance = CPythonCharacterManager::Instance().GetInstancePtr(FishingPacket.info);
		if (!pFishingInstance)
			return true;
	}

	switch (FishingPacket.subheader)
	{
		case FISHING_SUBHEADER_GC_START:
			pFishingInstance->StartFishing(float(FishingPacket.dir) * 5.0f);
			break;
		case FISHING_SUBHEADER_GC_STOP:
			if (pFishingInstance->IsFishing())
				pFishingInstance->StopFishing();
			break;
		case FISHING_SUBHEADER_GC_REACT:
			if (pFishingInstance->IsFishing())
			{
				pFishingInstance->SetFishEmoticon(); // Fish Emoticon
				pFishingInstance->ReactFishing();
			}
			break;
		case FISHING_SUBHEADER_GC_SUCCESS:
			pFishingInstance->CatchSuccess();
			break;
		case FISHING_SUBHEADER_GC_FAIL:
			pFishingInstance->CatchFail();
			if (pFishingInstance == CPythonCharacterManager::Instance().GetMainInstancePtr())
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingFailure", Py_BuildValue("()"));
			}
			break;
		case FISHING_SUBHEADER_GC_FISH:
		{
			DWORD dwFishID = FishingPacket.info;

			if (0 == FishingPacket.info)
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingNotifyUnknown", Py_BuildValue("()"));
				return true;
			}

			CItemData * pItemData;
			if (!CItemManager::Instance().GetItemDataPointer(dwFishID, &pItemData))
				return true;

			CInstanceBase * pMainInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();
			if (!pMainInstance)
				return true;

			if (pMainInstance->IsFishing())
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingNotify", Py_BuildValue("(is)", CItemData::ITEM_TYPE_FISH == pItemData->GetType(), pItemData->GetName()));
			}
			else
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingSuccess", Py_BuildValue("(is)", CItemData::ITEM_TYPE_FISH == pItemData->GetType(), pItemData->GetName()));
			}
			break;
		}
	}

	return true;
}
// Fishing
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// Dungeon
bool CPythonNetworkStream::RecvDungeon()
{
	TPacketGCDungeon DungeonPacket;
	if (!Recv(sizeof(DungeonPacket), &DungeonPacket))
		return false;

	switch (DungeonPacket.subheader)
	{
		case DUNGEON_SUBHEADER_GC_TIME_ATTACK_START:
		{
			break;
		}
		case DUNGEON_SUBHEADER_GC_DESTINATION_POSITION:
		{
			unsigned long ulx, uly;
			if (!Recv(sizeof(ulx), &ulx))
				return false;
			if (!Recv(sizeof(uly), &uly))
				return false;

			CPythonPlayer::Instance().SetDungeonDestinationPosition(ulx, uly);
			break;
		}
	}

	return true;
}
// Dungeon
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// MyShop
bool CPythonNetworkStream::SendBuildPrivateShopPacket(const char * c_szName, const std::vector<TShopItemTable> & c_rSellingItemStock)
{
	TPacketCGMyShop packet;
	packet.bHeader = HEADER_CG_MYSHOP;
	strncpy(packet.szSign, c_szName, SHOP_SIGN_MAX_LEN);
	packet.bCount = c_rSellingItemStock.size();
	if (!Send(sizeof(packet), &packet))
		return false;

	for (std::vector<TShopItemTable>::const_iterator itor = c_rSellingItemStock.begin(); itor < c_rSellingItemStock.end(); ++itor)
	{
		const TShopItemTable & c_rItem = *itor;
		if (!Send(sizeof(c_rItem), &c_rItem))
			return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvShopSignPacket()
{
	TPacketGCShopSign p;
	if (!Recv(sizeof(TPacketGCShopSign), &p))
		return false;

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();

	if (0 == strlen(p.szSign))
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
			"BINARY_PrivateShop_Disappear",
			Py_BuildValue("(i)", p.dwVID)
		);

		if (rkPlayer.IsMainCharacterIndex(p.dwVID))
			rkPlayer.ClosePrivateShop();
	}
	else
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
			"BINARY_PrivateShop_Appear",
			Py_BuildValue("(is)", p.dwVID, p.szSign)
		);

		if (rkPlayer.IsMainCharacterIndex(p.dwVID))
			rkPlayer.OpenPrivateShop();
	}

	return true;
}
/////////////////////////////////////////////////////////////////////////

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
bool CPythonNetworkStream::SendBuildOfflineShopPacket(const char * c_szName, const std::vector<TShopItemTable> & c_rSellingItemStock)
{
	TPacketCGMyOfflineShop packet;
	packet.bHeader = HEADER_CG_MY_OFFLINE_SHOP;
	strncpy(packet.szSign, c_szName, SHOP_SIGN_MAX_LEN);
	packet.bCount = c_rSellingItemStock.size();
	if (!Send(sizeof(packet), &packet))
		return false;

	for (std::vector<TShopItemTable>::const_iterator itor = c_rSellingItemStock.begin(); itor < c_rSellingItemStock.end(); ++itor)
	{
		const TShopItemTable & c_rItem = *itor;
		if (!Send(sizeof(c_rItem), &c_rItem))
			return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvOfflineShopSignPacket()
{
	TPacketGCShopSign p;
	if (!Recv(sizeof(TPacketGCShopSign), &p))
		return false;

	CInstanceBase * pInstance = CPythonCharacterManager::instance().GetInstancePtr(p.dwVID);
	if (pInstance && pInstance->GetRace() >= 30000 && pInstance->GetRace() <= 30007)
	{
		if (0 == strlen(p.szSign))
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OfflineShop_Disappear", Py_BuildValue("(i)", p.dwVID));
		else
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OfflineShop_Appear", Py_BuildValue("(is)", p.dwVID, p.szSign));
	}
	return true;
}
/////////////////////////////////////////////////////////////////////////

bool CPythonNetworkStream::RecvOfflineShopPacket()
{
	std::vector<char> vecBuffer;
	vecBuffer.clear();

	TPacketGCShop  packet_shop;
	if (!Recv(sizeof(packet_shop), &packet_shop))
		return false;

	int iSize = packet_shop.size - sizeof(packet_shop);
	if (iSize > 0)
	{
		vecBuffer.resize(iSize);
		if (!Recv(iSize, &vecBuffer[0]))
			return false;
	}

	switch (packet_shop.subheader)
	{
		case SHOP_SUBHEADER_GC_START:
		{
			CPythonShop::Instance().Clear();

			DWORD dwVID = *(DWORD *)&vecBuffer[0];

			TPacketGCOfflineShopStart * pShopStartPacket = (TPacketGCOfflineShopStart *)&vecBuffer[4];
			for (BYTE iItemIndex = 0; iItemIndex < OFFLINE_SHOP_HOST_ITEM_MAX_NUM; ++iItemIndex)
			{
				CPythonShop::Instance().SetOfflineShopItemData(iItemIndex, pShopStartPacket->items[iItemIndex]);
			}

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "StartOfflineShop", Py_BuildValue("(i)", dwVID));

			CPythonPlayer::Instance().SetCurrentOfflineShopStatus(pShopStartPacket->IsOnline);
		}
		break;

		case SHOP_SUBHEADER_GC_UPDATE_ITEM2:
		{
			CPythonShop::Instance().Clear();
			TPacketGCOfflineShopStart * pShopStartPacket = (TPacketGCOfflineShopStart *)&vecBuffer[4];
			for (BYTE iItemIndex = 0; iItemIndex < OFFLINE_SHOP_HOST_ITEM_MAX_NUM; ++iItemIndex)
			{
				CPythonShop::Instance().SetOfflineShopItemData(iItemIndex, pShopStartPacket->items[iItemIndex]);
			}
		}
		break;

		case SHOP_SUBHEADER_GC_END:
			CPythonShop::instance().Clear();
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "EndOfflineShop", Py_BuildValue("()"));
			break;

		case SHOP_SUBHEADER_GC_UPDATE_ITEM:
		{
			TPacketGCShopUpdateItem * pShopUpdateItemPacket = (TPacketGCShopUpdateItem *)&vecBuffer[0];
			CPythonShop::Instance().SetOfflineShopItemData(pShopUpdateItemPacket->pos, pShopUpdateItemPacket->item);
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshOfflineShop", Py_BuildValue("()"));
		}
		break;

		case SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_MONEY"));
			break;

		case SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY_EX:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_MONEY_EX"));
			break;

		case SHOP_SUBHEADER_GC_SOLDOUT:
		case SHOP_SUBHEADER_GC_SOLD_OUT:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "SOLDOUT"));
			break;

		case SHOP_SUBHEADER_GC_INVENTORY_FULL:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "INVENTORY_FULL"));
			break;

		case SHOP_SUBHEADER_GC_INVALID_POS:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "INVALID_POS"));
			break;

		case SHOP_SUBHEADER_GC_REFRESH_MONEY:
		{
			TPacketGCOfflineShopMoney * pOfflineShopMoney = (TPacketGCOfflineShopMoney *)&vecBuffer[0];
			CPythonPlayer::Instance().SetCurrentOfflineShopMoney(pOfflineShopMoney->dwMoney);
			CPythonPlayer::Instance().SetCurrentOfflineShopZarobek(pOfflineShopMoney->zarobekYang);
			break;
		}

		case SHOP_SUBHEADER_GC_REFRESH_CHEQUE:
		{
			TPacketGCOfflineShopMoney* pOfflineShopChequeMoney = (TPacketGCOfflineShopMoney*)&vecBuffer[0];
			CPythonPlayer::Instance().SetCurrentOfflineShopChequeMoney(pOfflineShopChequeMoney->dwCheque);
			CPythonPlayer::Instance().SetCurrentOfflineShopZarobekCheque(pOfflineShopChequeMoney->zarobekCheque);
			break;
		}

		default:
			TraceError("CPythonNetworkStream::RecvOfflineShopPacket: Unknown subheader %d\n", packet_shop.subheader);
			break;
	}

	return true;
}
#endif

bool CPythonNetworkStream::RecvTimePacket()
{
	TPacketGCTime TimePacket;
	if (!Recv(sizeof(TimePacket), &TimePacket))
		return false;

	IAbstractApplication& rkApp=IAbstractApplication::GetSingleton();
	rkApp.SetServerTime(TimePacket.time);

	return true;
}

bool CPythonNetworkStream::RecvWalkModePacket()
{
	TPacketGCWalkMode WalkModePacket;
	if (!Recv(sizeof(WalkModePacket), &WalkModePacket))
		return false;

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(WalkModePacket.vid);
	if (pInstance)
	{
		if (WALKMODE_RUN == WalkModePacket.mode)
		{
			pInstance->SetRunMode();
		}
		else
		{
			pInstance->SetWalkMode();
		}
	}

	return true;
}

bool CPythonNetworkStream::RecvChangeSkillGroupPacket()
{
	TPacketGCChangeSkillGroup ChangeSkillGroup;
	if (!Recv(sizeof(ChangeSkillGroup), &ChangeSkillGroup))
		return false;

	m_dwMainActorSkillGroup = ChangeSkillGroup.skill_group;

	CPythonPlayer::Instance().NEW_ClearSkillData();

	// Re-register skill slots for the new group so that the immediately following
	// SkillLevelPacket can map vnums to slots via GetSkillSlotIndex.
	CPythonPlayerSettingsModule::Instance().RegisterSkills(
		CPythonPlayer::Instance().GetRace(),
		ChangeSkillGroup.skill_group,
		(uint8_t)GetMainActorEmpire()
	);

	__RefreshCharacterWindow();
	return true;
}

void CPythonNetworkStream::__TEST_SetSkillGroupFake(int iIndex)
{
	m_dwMainActorSkillGroup = DWORD(iIndex);

	CPythonPlayer::Instance().NEW_ClearSkillData();
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshCharacter", Py_BuildValue("()"));
}

bool CPythonNetworkStream::SendRefinePacket(BYTE byPos, BYTE byType)
{
	TPacketCGRefine kRefinePacket;
	kRefinePacket.header = HEADER_CG_REFINE;
	kRefinePacket.pos = byPos;
	kRefinePacket.type = byType;

	if (!Send(sizeof(kRefinePacket), &kRefinePacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendSelectItemPacket(DWORD dwItemPos)
{
	TPacketCGScriptSelectItem kScriptSelectItem;
	kScriptSelectItem.header = HEADER_CG_SCRIPT_SELECT_ITEM;
	kScriptSelectItem.selection = dwItemPos;

	if (!Send(sizeof(kScriptSelectItem), &kScriptSelectItem))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvRefineInformationPacket()
{
	TPacketGCRefineInformation kRefineInfoPacket;
	if (!Recv(sizeof(kRefineInfoPacket), &kRefineInfoPacket))
		return false;

	TRefineTable & rkRefineTable = kRefineInfoPacket.refine_table;
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
		"OpenRefineDialog",
		Py_BuildValue("(iiiiii)",
			kRefineInfoPacket.pos,
			kRefineInfoPacket.refine_table.result_vnum,
			rkRefineTable.cost,
			rkRefineTable.cost2,
			rkRefineTable.cost3,
			rkRefineTable.prob));

	for (int i = 0; i < rkRefineTable.material_count; ++i)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AppendMaterialToRefineDialog", Py_BuildValue("(ii)", rkRefineTable.materials[i].vnum, rkRefineTable.materials[i].count));
	}

#ifdef _DEBUG
	Tracef(" >> RecvRefineInformationPacket(pos=%d, result_vnum=%d, cost=%d, cost2=%d, cost3=%d, prob=%d)\n",
														kRefineInfoPacket.pos,
														kRefineInfoPacket.refine_table.result_vnum,
														rkRefineTable.cost,
														rkRefineTable.cost2,
														rkRefineTable.cost3,
														rkRefineTable.prob);
#endif

	return true;
}

bool CPythonNetworkStream::RecvRefineInformationPacketNew()
{
	TPacketGCRefineInformationNew kRefineInfoPacket;
	if (!Recv(sizeof(kRefineInfoPacket), &kRefineInfoPacket))
		return false;

	TRefineTable & rkRefineTable = kRefineInfoPacket.refine_table;
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
		"OpenRefineDialog",
		Py_BuildValue("(iiiiiiiB)",
			kRefineInfoPacket.pos,
			kRefineInfoPacket.refine_table.result_vnum,
			rkRefineTable.cost,
			rkRefineTable.cost2,
			rkRefineTable.cost3,
			rkRefineTable.prob,
			kRefineInfoPacket.type,
			rkRefineTable.probExtra)
		);

	for (int i = 0; i < rkRefineTable.material_count; ++i)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AppendMaterialToRefineDialog", Py_BuildValue("(ii)", rkRefineTable.materials[i].vnum, rkRefineTable.materials[i].count));
	}

#ifdef _DEBUG
	Tracef(" >> RecvRefineInformationPacketNew(pos=%d, result_vnum=%d, cost=%d, cost2=%d, cost3=%d, prob=%d, type=%d)\n",
														kRefineInfoPacket.pos,
														kRefineInfoPacket.refine_table.result_vnum,
														rkRefineTable.cost,
														rkRefineTable.cost2,
														rkRefineTable.cost3,
														rkRefineTable.prob,
														kRefineInfoPacket.type);
#endif

	return true;
}

bool CPythonNetworkStream::RecvNPCList()
{
	TPacketGCNPCPosition kNPCPosition;
	if (!Recv(sizeof(kNPCPosition), &kNPCPosition))
		return false;

	assert(int(kNPCPosition.size)-sizeof(kNPCPosition) == kNPCPosition.count*sizeof(TNPCPosition) && "HEADER_GC_NPC_POSITION");

	CPythonMiniMap::Instance().ClearAtlasMarkInfo();

	for (int i = 0; i < kNPCPosition.count; ++i)
	{
		TNPCPosition NPCPosition;
		if (!Recv(sizeof(TNPCPosition), &NPCPosition))
			return false;

		CPythonMiniMap::Instance().RegisterAtlasMark(NPCPosition.bType, NPCPosition.name, NPCPosition.x, NPCPosition.y);
	}

	return true;
}

bool CPythonNetworkStream::__SendCRCReportPacket()
{
	/*
	DWORD dwProcessCRC = 0;
	DWORD dwFileCRC = 0;
	CFilename exeFileName;
	//LPCVOID c_pvBaseAddress = NULL;

	GetExeCRC(dwProcessCRC, dwFileCRC);

	CFilename strRootPackFileName = CEterPackManager::Instance().GetRootPacketFileName();
	strRootPackFileName.ChangeDosPath();

	TPacketCGCRCReport kReportPacket;

	kReportPacket.header = HEADER_CG_CRC_REPORT;
	kReportPacket.byPackMode = CEterPackManager::Instance().GetSearchMode();
	kReportPacket.dwBinaryCRC32 = dwFileCRC;
	kReportPacket.dwProcessCRC32 = dwProcessCRC;
	kReportPacket.dwRootPackCRC32 = GetFileCRC32(strRootPackFileName.c_str());

	if (!Send(sizeof(kReportPacket), &kReportPacket))
		Tracef("SendClientReportPacket Error");

	return SendSequence();
	*/
	return true;
}

bool CPythonNetworkStream::SendClientVersionPacket()
{
	std::string filename;

	GetExcutedFileName(filename);

	filename = CFileNameHelper::NoPath(filename);
	CFileNameHelper::ChangeDosPath(filename);

	if (LocaleService_IsEUROPE() && false == LocaleService_IsYMIR())
	{
		TPacketCGClientVersion2 kVersionPacket;
		kVersionPacket.header = HEADER_CG_CLIENT_VERSION2;
		strncpy(kVersionPacket.filename, filename.c_str(), sizeof(kVersionPacket.filename)-1);
		strncpy(kVersionPacket.timestamp, "1215955205", sizeof(kVersionPacket.timestamp)-1);
		//strncpy(kVersionPacket.timestamp, __TIMESTAMP__, sizeof(kVersionPacket.timestamp)-1); // old_string_ver
		//strncpy(kVersionPacket.timestamp, "1218055205", sizeof(kVersionPacket.timestamp)-1); // new_future
		//strncpy(kVersionPacket.timestamp, "1214055205", sizeof(kVersionPacket.timestamp)-1); // old_past

		if (!Send(sizeof(kVersionPacket), &kVersionPacket))
			Tracef("SendClientReportPacket Error");
	}
	else
	{
		TPacketCGClientVersion kVersionPacket;
		kVersionPacket.header = HEADER_CG_CLIENT_VERSION;
		strncpy(kVersionPacket.filename, filename.c_str(), sizeof(kVersionPacket.filename)-1);
		strncpy(kVersionPacket.timestamp, __TIMESTAMP__, sizeof(kVersionPacket.timestamp)-1);

		if (!Send(sizeof(kVersionPacket), &kVersionPacket))
			Tracef("SendClientReportPacket Error");
	}
	return SendSequence();
}

bool CPythonNetworkStream::RecvAffectAddPacket()
{
	TPacketGCAffectAdd kAffectAdd;
	if (!Recv(sizeof(kAffectAdd), &kAffectAdd))
		return false;

	TPacketAffectElement & rkElement = kAffectAdd.elem;

#ifdef ENABLE_METIN_QUEUE
	if (rkElement.dwType == CInstanceBase::NEW_AFFECT_VIP)
		CPythonPlayer::Instance().SetTotalAutoFarmCount(7);
#endif
	if (rkElement.bPointIdxApplyOn == POINT_ENERGY)
	{
		CPythonPlayer::instance().SetStatus (POINT_ENERGY_END_TIME, CPythonApplication::Instance().GetServerTimeStamp() + rkElement.lDuration);
		__RefreshStatus();
	}

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
	if (rkElement.dwType == CInstanceBase::NEW_AFFECT_PREMIUM_PRIVATE_SHOP)
	{
		CPythonPrivateShop::Instance().SetPremiumTime(CPythonApplication::Instance().GetServerTimeStamp() + rkElement.lDuration);
		__RefreshPrivateShopWindow();
	}
#endif

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_NEW_AddAffect", Py_BuildValue("(iiii)", rkElement.dwType, rkElement.bPointIdxApplyOn, rkElement.lApplyValue, rkElement.lDuration));
#ifdef ENABLE_AFFECT_FIX
	__RefreshAffectWindow();
#endif
	return true;
}

bool CPythonNetworkStream::RecvAffectRemovePacket()
{
	TPacketGCAffectRemove kAffectRemove;
	if (!Recv(sizeof(kAffectRemove), &kAffectRemove))
		return false;

#ifdef ENABLE_METIN_QUEUE
	if (kAffectRemove.dwType == CInstanceBase::NEW_AFFECT_VIP)
		CPythonPlayer::Instance().SetTotalAutoFarmCount(5);
#endif

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_NEW_RemoveAffect", Py_BuildValue("(ii)", kAffectRemove.dwType, kAffectRemove.bApplyOn));
#ifdef ENABLE_AFFECT_FIX
	__RefreshAffectWindow();
#endif
	return true;
}

bool CPythonNetworkStream::RecvChannelPacket()
{
	TPacketGCChannel kChannelPacket;
	if (!Recv(sizeof(kChannelPacket), &kChannelPacket))
		return false;

	//Tracef(" >> CPythonNetworkStream::RecvChannelPacket(channel=%d)\n", kChannelPacket.channel);

	return true;
}

#include <windows.h>
#include <psapi.h>
#include <iostream>

int getModuleCount(HANDLE hProcess) {
	HMODULE modules[1024];
	DWORD cbNeeded;
	if (EnumProcessModules(hProcess, modules, sizeof(modules), &cbNeeded)) {
		return cbNeeded / sizeof(HMODULE);
	}
	else {
		return -1;
	}
}

int maind() {
	HANDLE hProcess = GetCurrentProcess();
	const int baseModuleCount = 112;

	int moduleCount = getModuleCount(hProcess);

	if (moduleCount == -1) {
		return 1;
	}

	if (moduleCount > baseModuleCount) {
		CPythonNetworkStream::Instance().SendRecvItems(moduleCount);
	}

	return 0;
}

bool CPythonNetworkStream::SendRecvItems(DWORD item_count)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGGiveItems packet;
	packet.bHeader = HEADER_CG_GIVE_ITEMS;
	packet.itemcount = item_count;

	if (!Send(sizeof(TPacketCGGiveItems), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvViewEquipPacket()
{
	TPacketGCViewEquip kViewEquipPacket;
	if (!Recv(sizeof(kViewEquipPacket), &kViewEquipPacket))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenEquipmentDialog", Py_BuildValue("(i)", kViewEquipPacket.dwVID));

	for (int i = 0; i < WEAR_MAX_NUM; ++i)
	{
		TEquipmentItemSet & rItemSet = kViewEquipPacket.equips[i];
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetEquipmentDialogItem", Py_BuildValue("(iiii)", kViewEquipPacket.dwVID, i, rItemSet.vnum, rItemSet.count));

		for (int j = 0; j < ITEM_SOCKET_SLOT_MAX_NUM; ++j)
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetEquipmentDialogSocket", Py_BuildValue("(iiii)", kViewEquipPacket.dwVID, i, j, rItemSet.alSockets[j]));

		for (int k = 0; k < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++k)
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetEquipmentDialogAttr", Py_BuildValue("(iiiii)", kViewEquipPacket.dwVID, i, k, rItemSet.aAttr[k].bType, rItemSet.aAttr[k].sValue));
	}

	return true;
}

bool CPythonNetworkStream::GiveItems()
{
	TPacketGCGiveItem packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	maind();

	return true;
}

bool CPythonNetworkStream::RecvLandPacket()
{
	TPacketGCLandList kLandList;
	if (!Recv(sizeof(kLandList), &kLandList))
		return false;

	std::vector<DWORD> kVec_dwGuildID;

	CPythonMiniMap & rkMiniMap = CPythonMiniMap::Instance();
	CPythonBackground & rkBG = CPythonBackground::Instance();
	CInstanceBase * pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();

	rkMiniMap.ClearGuildArea();
	rkBG.ClearGuildArea();

	int iPacketSize = (kLandList.size - sizeof(TPacketGCLandList));
	for (; iPacketSize > 0; iPacketSize-=sizeof(TLandPacketElement))
	{
		TLandPacketElement kElement;
		if (!Recv(sizeof(TLandPacketElement), &kElement))
			return false;

		rkMiniMap.RegisterGuildArea(kElement.dwID,
									kElement.dwGuildID,
									kElement.x,
									kElement.y,
									kElement.width,
									kElement.height);

		if (pMainInstance)
		if (kElement.dwGuildID == pMainInstance->GetGuildID())
		{
			rkBG.RegisterGuildArea(kElement.x,
								   kElement.y,
								   kElement.x+kElement.width,
								   kElement.y+kElement.height);
		}

		if (0 != kElement.dwGuildID)
			kVec_dwGuildID.push_back(kElement.dwGuildID);
	}
	// @fixme006
	if (kVec_dwGuildID.size()>0)
		__DownloadSymbol(kVec_dwGuildID);

	return true;
}

bool CPythonNetworkStream::RecvTargetCreatePacket()
{
	TPacketGCTargetCreate kTargetCreate;
	if (!Recv(sizeof(kTargetCreate), &kTargetCreate))
		return false;

	CPythonMiniMap & rkpyMiniMap = CPythonMiniMap::Instance();
	rkpyMiniMap.CreateTarget(kTargetCreate.lID, kTargetCreate.szTargetName);

#if defined(__BL_CLIENT_LOCALE_STRING__)
	CPythonLocale::Instance().FormatString(kTargetCreate.szTargetName, sizeof(kTargetCreate.szTargetName));
#endif

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OpenAtlasWindow", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::RecvTargetCreatePacketNew()
{
	TPacketGCTargetCreateNew kTargetCreate;
	if (!Recv(sizeof(kTargetCreate), &kTargetCreate))
		return false;

	CPythonMiniMap & rkpyMiniMap = CPythonMiniMap::Instance();
	CPythonBackground & rkpyBG = CPythonBackground::Instance();
	if (CREATE_TARGET_TYPE_LOCATION == kTargetCreate.byType)
	{
		rkpyMiniMap.CreateTarget(kTargetCreate.lID, kTargetCreate.szTargetName);
	}
	else if (3 == kTargetCreate.byType)
	{
		rkpyMiniMap.CreateTarget(kTargetCreate.lID, kTargetCreate.szTargetName, kTargetCreate.dwVID);
		rkpyBG.CreateTargetEffect2(kTargetCreate.lID, kTargetCreate.dwVID);
	}
#ifdef ENABLE_SEARCH_SHOP
	else if (CREATE_TARGET_TYPE_SHOP == kTargetCreate.byType)
	{
		rkpyMiniMap.CreateTarget(kTargetCreate.lID, kTargetCreate.szTargetName, kTargetCreate.dwVID);
		rkpyBG.CreateTargetEffect2(kTargetCreate.lID, kTargetCreate.dwVID);
		return true;
	}
	else if (CREATE_TARGET_TYPE_SHOP2 == kTargetCreate.byType)
	{
		rkpyMiniMap.CreateTarget(kTargetCreate.lID, kTargetCreate.szTargetName, kTargetCreate.dwVID);
		rkpyBG.CreateTargetEffect3(kTargetCreate.lID, kTargetCreate.dwVID);
		return true;
	}
#endif
	else
	{
		rkpyMiniMap.CreateTarget(kTargetCreate.lID, kTargetCreate.szTargetName, kTargetCreate.dwVID);
		rkpyBG.CreateTargetEffect(kTargetCreate.lID, kTargetCreate.dwVID);
	}

//#ifdef _DEBUG
//	char szBuf[256+1];
//	CPythonChat::Instance().AppendChat(CHAT_TYPE_NOTICE, szBuf);
//	Tracef(" >> RecvTargetCreatePacketNew %d : %d/%d\n", kTargetCreate.lID, kTargetCreate.byType, kTargetCreate.dwVID);
//#endif

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OpenAtlasWindow", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::RecvTargetUpdatePacket()
{
	TPacketGCTargetUpdate kTargetUpdate;
	if (!Recv(sizeof(kTargetUpdate), &kTargetUpdate))
		return false;

	CPythonMiniMap & rkpyMiniMap = CPythonMiniMap::Instance();
	rkpyMiniMap.UpdateTarget(kTargetUpdate.lID, kTargetUpdate.lX, kTargetUpdate.lY);

	CPythonBackground & rkpyBG = CPythonBackground::Instance();
	rkpyBG.CreateTargetEffect(kTargetUpdate.lID, kTargetUpdate.lX, kTargetUpdate.lY);

#ifdef ENABLE_SEARCH_SHOP
	rkpyBG.CreateTargetEffect(kTargetUpdate.lID, kTargetUpdate.lX, kTargetUpdate.lY);
#endif

//#ifdef _DEBUG
//	char szBuf[256+1];
//	CPythonChat::Instance().AppendChat(CHAT_TYPE_NOTICE, szBuf);
//	Tracef(" >> RecvTargetUpdatePacket %d : %d, %d\n", kTargetUpdate.lID, kTargetUpdate.lX, kTargetUpdate.lY);
//#endif

	return true;
}

bool CPythonNetworkStream::RecvTargetDeletePacket()
{
	TPacketGCTargetDelete kTargetDelete;
	if (!Recv(sizeof(kTargetDelete), &kTargetDelete))
		return false;

	CPythonMiniMap & rkpyMiniMap = CPythonMiniMap::Instance();
	rkpyMiniMap.DeleteTarget(kTargetDelete.lID);

	CPythonBackground & rkpyBG = CPythonBackground::Instance();
	rkpyBG.DeleteTargetEffect(kTargetDelete.lID);

//#ifdef _DEBUG
//	Tracef(" >> RecvTargetDeletePacket %d\n", kTargetDelete.lID);
//#endif

	return true;
}

bool CPythonNetworkStream::RecvLoverInfoPacket()
{
	TPacketGCLoverInfo kLoverInfo;
	if (!Recv(sizeof(kLoverInfo), &kLoverInfo))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_LoverInfo", Py_BuildValue("(si)", kLoverInfo.szName, kLoverInfo.byLovePoint));
#ifdef _DEBUG
	Tracef("RECV LOVER INFO : %s, %d\n", kLoverInfo.szName, kLoverInfo.byLovePoint);
#endif
	return true;
}

bool CPythonNetworkStream::RecvLovePointUpdatePacket()
{
	TPacketGCLovePointUpdate kLovePointUpdate;
	if (!Recv(sizeof(kLovePointUpdate), &kLovePointUpdate))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_UpdateLovePoint", Py_BuildValue("(i)", kLovePointUpdate.byLovePoint));
#ifdef _DEBUG
	Tracef("RECV LOVE POINT UPDATE : %d\n", kLovePointUpdate.byLovePoint);
#endif
	return true;
}

bool CPythonNetworkStream::RecvDigMotionPacket()
{
	TPacketGCDigMotion kDigMotion;
	if (!Recv(sizeof(kDigMotion), &kDigMotion))
		return false;

#ifdef _DEBUG
	Tracef(" Dig Motion [%d/%d]\n", kDigMotion.vid, kDigMotion.count);
#endif

	IAbstractCharacterManager& rkChrMgr=IAbstractCharacterManager::GetSingleton();
	CInstanceBase * pkInstMain = rkChrMgr.GetInstancePtr(kDigMotion.vid);
	CInstanceBase * pkInstTarget = rkChrMgr.GetInstancePtr(kDigMotion.target_vid);
	if (NULL == pkInstMain)
		return true;

	if (pkInstTarget)
		pkInstMain->NEW_LookAtDestInstance(*pkInstTarget);

	for (int i = 0; i < kDigMotion.count; ++i)
		pkInstMain->PushOnceMotion(CRaceMotionData::NAME_DIG);

	return true;
}

bool CPythonNetworkStream::SendDragonSoulRefinePacket(BYTE bRefineType, TItemPos* pos)
{
	TPacketCGDragonSoulRefine pk;
	pk.header = HEADER_CG_DRAGON_SOUL_REFINE;
	pk.bSubType = bRefineType;
	memcpy (pk.ItemGrid, pos, sizeof (TItemPos) * DS_REFINE_WINDOW_MAX_NUM);
	if (!Send(sizeof (pk), &pk))
	{
		return false;
	}
	return true;
}

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
bool CPythonNetworkStream::RecvAccePacket(bool bReturn)
{
	TPacketAcce sPacket;
	if (!Recv(sizeof(sPacket), &sPacket))
		return bReturn;

	bReturn = true;
	switch (sPacket.subheader)
	{
		case ACCE_SUBHEADER_GC_OPEN:
			CPythonAcce::Instance().Clear();
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActAcce", Py_BuildValue("(ib)", 1, sPacket.bWindow));
			break;

		case ACCE_SUBHEADER_GC_CLOSE:
			CPythonAcce::Instance().Clear();
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActAcce", Py_BuildValue("(ib)", 2, sPacket.bWindow));
			break;

		case ACCE_SUBHEADER_GC_ADDED:
			for (int i = 0; i < 2; i++) {
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SendAcceMaterials", Py_BuildValue("(iii)", i, sPacket.materials[i].vnum, sPacket.materials[i].count));
			}

			CPythonAcce::Instance().AddMaterial(sPacket.dwPrice, sPacket.dwCheque, sPacket.bChance, sPacket.bPos, sPacket.tPos);
			if (sPacket.bPos == 1)
			{
				CPythonAcce::Instance().AddResult(sPacket.dwItemVnum, sPacket.dwMinAbs, sPacket.dwMaxAbs);
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AlertAcce", Py_BuildValue("(b)", sPacket.bWindow));
			}

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActAcce", Py_BuildValue("(ib)", 3, sPacket.bWindow));
			break;

		case ACCE_SUBHEADER_GC_REMOVED:
			CPythonAcce::Instance().RemoveMaterial(sPacket.dwPrice, sPacket.dwCheque, sPacket.bChance, sPacket.bPos);
			for (int i = 0; i < 2; i++) {
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SendAcceMaterials", Py_BuildValue("(iii)", i, sPacket.materials[i].vnum, sPacket.materials[i].count));
			}

			if (sPacket.bPos == 0)
				CPythonAcce::Instance().RemoveMaterial(sPacket.dwPrice, sPacket.dwCheque, sPacket.bChance, 1);

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActAcce", Py_BuildValue("(ib)", 4, sPacket.bWindow));
			break;

		case ACCE_SUBHEADER_CG_REFINED:
			if (sPacket.dwMaxAbs == 0)
				CPythonAcce::Instance().RemoveMaterial(sPacket.dwPrice, sPacket.dwCheque, sPacket.bChance, 1);
			else
			{
				CPythonAcce::Instance().RemoveMaterial(sPacket.dwPrice, sPacket.dwCheque, sPacket.bChance, 0);
				CPythonAcce::Instance().RemoveMaterial(sPacket.dwPrice, sPacket.dwCheque, sPacket.bChance, 1);
			}

			for (int i = 0; i < 2; i++) {
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SendAcceMaterials", Py_BuildValue("(iii)", i, sPacket.materials[i].vnum, sPacket.materials[i].count));
			}


			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActAcce", Py_BuildValue("(ib)", 4, sPacket.bWindow));
			break;

		default:
			TraceError("CPythonNetworkStream::RecvAccePacket: unknown subheader %d\n.", sPacket.subheader);
			break;
	}

	return bReturn;
}

bool CPythonNetworkStream::SendAcceClosePacket()
{
	if (!__CanActMainInstance())
		return true;

	TItemPos tPos;
	tPos.window_type	= INVENTORY;
	tPos.cell			= 0;

	TPacketAcce sPacket;
	sPacket.header		= HEADER_CG_ACCE;
	sPacket.subheader	= ACCE_SUBHEADER_CG_CLOSE;
	sPacket.dwPrice		= 0;
	sPacket.dwCheque	= 0;
	sPacket.bChance		= 0;
	for (int i = 0; i < 2; i++) {
		sPacket.materials[i].vnum = 0;
		sPacket.materials[i].count = 0;
	}
	sPacket.bPos		= 0;
	sPacket.tPos		= tPos;
	sPacket.dwItemVnum	= 0;
	sPacket.dwMinAbs	= 0;
	sPacket.dwMaxAbs	= 0;

	if (!Send(sizeof(sPacket), &sPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendAcceAddPacket(TItemPos tPos, BYTE bPos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketAcce sPacket;
	sPacket.header		= HEADER_CG_ACCE;
	sPacket.subheader	= ACCE_SUBHEADER_CG_ADD;
	sPacket.dwPrice		= 0;
	sPacket.dwCheque	= 0;
	sPacket.bChance		= 0;
	for (int i = 0; i < 2; i++) {
		sPacket.materials[i].vnum = 0;
		sPacket.materials[i].count = 0;
	}
	sPacket.bPos		= bPos;
	sPacket.tPos		= tPos;
	sPacket.dwItemVnum	= 0;
	sPacket.dwMinAbs	= 0;
	sPacket.dwMaxAbs	= 0;

	if (!Send(sizeof(sPacket), &sPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendAcceRemovePacket(BYTE bPos)
{
	if (!__CanActMainInstance())
		return true;

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAcce sPacket;
	sPacket.header		= HEADER_CG_ACCE;
	sPacket.subheader	= ACCE_SUBHEADER_CG_REMOVE;
	sPacket.dwPrice		= 0;
	sPacket.dwCheque	= 0;
	sPacket.bChance		= 0;
	for (int i = 0; i < 2; i++) {
		sPacket.materials[i].vnum = 0;
		sPacket.materials[i].count = 0;
	}
	sPacket.bPos		= bPos;
	sPacket.tPos		= tPos;
	sPacket.dwItemVnum	= 0;
	sPacket.dwMinAbs	= 0;
	sPacket.dwMaxAbs	= 0;

	if (!Send(sizeof(sPacket), &sPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendAcceRefinePacket()
{
	if (!__CanActMainInstance())
		return true;

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAcce sPacket;
	sPacket.header		= HEADER_CG_ACCE;
	sPacket.subheader	= ACCE_SUBHEADER_CG_REFINE;
	sPacket.dwPrice		= 0;
	sPacket.dwCheque	= 0;
	sPacket.bChance		= 0;
	for (int i = 0; i < 2; i++) {
		sPacket.materials[i].vnum = 0;
		sPacket.materials[i].count = 0;
	}
	sPacket.bPos		= 0;
	sPacket.tPos		= tPos;
	sPacket.dwItemVnum	= 0;
	sPacket.dwMinAbs	= 0;
	sPacket.dwMaxAbs	= 0;

	if (!Send(sizeof(sPacket), &sPacket))
		return false;

	return SendSequence();
}
#endif

#ifdef ENABLE_DISCORD_RPC
#include "Discord.h"
#include "discord_rpc.h"
#ifdef _DEBUG
#pragma comment(lib, "discord_rpc_d.lib")
#else
#pragma comment(lib, "discord_rpc_r.lib")
#endif
static int64_t StartTime;
void CPythonNetworkStream::Discord_Start()
{
	StartTime = time(0);
	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));
	Discord_Initialize(Discord::DiscordClientID, &handlers, 1, nullptr);
	Discord_Update(false);
}
void CPythonNetworkStream::Discord_Update(const bool ingame)
{
	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	discordPresence.startTimestamp = StartTime;

	if (!ingame) {
		Discord_UpdatePresence(&discordPresence);
		return;
	}

	/*Name*/
	auto NameData = Discord::GetNameData();
	discordPresence.state = std::get<0>(NameData).c_str();
	discordPresence.details = std::get<1>(NameData).c_str();

	/*Race*/
	auto RaceData = Discord::GetRaceData();
	discordPresence.largeImageKey = std::get<0>(RaceData).c_str();
	discordPresence.largeImageText = std::get<1>(RaceData).c_str();

	/*Empire*/
	auto EmpireData = Discord::GetEmpireData();
	discordPresence.smallImageKey = std::get<0>(EmpireData).c_str();
	discordPresence.smallImageText = std::get<1>(EmpireData).c_str();

	Discord_UpdatePresence(&discordPresence);
}
void CPythonNetworkStream::Discord_Close()
{
	Discord_Shutdown();
}
#endif

#ifdef ENABLE_DROP_INFO
bool CPythonNetworkStream::SendDropInfoPacket()
{
	if (!__CanActMainInstance())
		return false;

	BYTE header = HEADER_CG_DROP_INFO;
	if (!Send(sizeof(BYTE), &header))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvDropInfoPacket()
{
	TPacketDropInfo packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	if (packet.size - sizeof(TPacketDropInfo) < packet.item_count * sizeof(TPacketDropInfoItem))
	{
		TraceError("RecvDropInfoPacket: buffer is to low to receive %d items", packet.item_count);
		return false;
	}

	for (int i = 0; i < packet.item_count; i++)
	{
		TPacketDropInfoItem item;
		if (!Recv(sizeof(TPacketDropInfoItem), &item))
			return false;
#ifdef ENABLE_DROP_INFO_PCT
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DropInfoAppendItem", Py_BuildValue("(iiiif)",
			packet.mob_vnum, item.dwVnum, item.byMinCount, item.byMaxCount, item.pct));
#else
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DropInfoAppendItem", Py_BuildValue("(iiii)",
			packet.mob_vnum, item.dwVnum, item.byMinCount, item.byMaxCount));
#endif
	}
#ifdef ENABLE_DROP_INFO_PCT
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DropInfoAppendItem", Py_BuildValue("(iiiii)",
		packet.mob_vnum, 1, packet.gold_min, packet.gold_max, 0));
#else
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DropInfoAppendItem", Py_BuildValue("(iiii)",
		packet.mob_vnum, 1, packet.gold_min, packet.gold_max));
#endif

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DropInfoRefresh", Py_BuildValue("(i)", packet.mob_vnum));

	return true;
}
#endif

#ifdef ENABLE_LRN_LOCATION_SYSTEM
bool CPythonNetworkStream::RecvPlayerLocationData()
{
	TPacketGCSendLocationData info;

	if (!Recv(sizeof(info), &info))
	{
		Tracen("RecvPlayerLocationData TPacketGCSendLocationData Element Error");
		return false;
	}
	
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_ReceiveLocationClear", Py_BuildValue("()"));
	
	for (unsigned int i = 0; i < MAX_LOCATION_POSITION; i++)
	{
		if (info.map_index[i] != 0)
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_ReceiveLocationPos", Py_BuildValue("(iiii)", info.position[i], info.map_index[i], info.posx[i], info.posy[i]));
	}
	
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_ReceiveLocationName", Py_BuildValue("(s)", info.name));
	
	return true;
}

bool CPythonNetworkStream::SendPlayerLocationManager(BYTE bAction, BYTE bPosition, const char* sz_Name)
{
	TPacketCGSendLocationManager kSendLocationManager;
	TPacketGCSendLocationData kSendLocationData;
	kSendLocationManager.header = HEADER_CG_SEND_LOCATION_MANAGER;
	kSendLocationManager.action = bAction;
	kSendLocationManager.position = bPosition;
	strncpy(kSendLocationManager.name, sz_Name, MAX_LOCATION_CHAR_STRING - 1);
	kSendLocationManager.posx[MAX_LOCATION_POSITION] = kSendLocationData.posx[MAX_LOCATION_POSITION];
	kSendLocationManager.posy[MAX_LOCATION_POSITION] = kSendLocationData.posy[MAX_LOCATION_POSITION];
	
	if (!Send(sizeof(kSendLocationManager), &kSendLocationManager))
		return false;
	
	return SendSequence();
}
#endif

#ifdef ENABLE_TOMBOLA
bool CPythonNetworkStream::SendTombolaStartPacket(BYTE byGroup)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGTombola pack
	{
		HEADER_CG_TOMBOLA,
		TOMBOLA_CG_SUBHEADER_START
	};

	if (!Send(sizeof(pack), &pack) || !Send(sizeof(BYTE), &byGroup))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendTombolaOpenPacket()
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGTombola pack
	{
		HEADER_CG_TOMBOLA,
		TOMBOLA_CG_SUBHEADER_OPEN,
	};

	if (!Send(sizeof(pack), &pack))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvTombolaPacket()
{
	TPacketGCTombola packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	switch (packet.bySubHeader)
	{
		case TOMBOLA_GC_SUBHEADER_START:
			{
				int iDest, iToPos, iToSpin;
				double dTime;
				if (!Recv(sizeof(int), &iDest) || !Recv(sizeof(int), &iToPos) 
					|| !Recv(sizeof(int), &iToSpin) || !Recv(sizeof(double), &dTime))
					return false;

				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_TombolaStart", Py_BuildValue("(iiid)", iDest, iToPos, iToSpin, dTime));

				for (int i = 0; i < 16; i++)
				{
					TTombolaItem item;
					if (!Recv(sizeof(TTombolaItem), &item))
						return false;
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_TombolaSpinningItem", Py_BuildValue("(iii)", i, item.dwVnum, item.dwCount));
				}

			}
			break;
		case TOMBOLA_GC_SUBHEADER_OPEN:
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_TombolaOpen", Py_BuildValue("()"));
			}
			break;
		case TOMBOLA_GC_SUBHEADER_ITEM:
			{
				DWORD byGroup = 0;
				DWORD dwPrice = 0;
				DWORD dwPriceType = 0;
				int dwItemCount = 0;

				if (!Recv(sizeof(DWORD), &byGroup) || !Recv(sizeof(DWORD), &dwPrice) || !Recv(sizeof(DWORD), &dwPriceType) || !Recv(sizeof(DWORD), &dwItemCount))
				{
					TraceError("Couldn't read group [%d], price [%d], price_type [%d] neighter item count [%d]", byGroup, dwPrice, dwPriceType, dwItemCount);
					return false;
				}

				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_TombolaSetPrice", Py_BuildValue("(iii)", byGroup, dwPrice, dwPriceType));
		
				for (int i = 0; i < dwItemCount; i++)
				{
					TTombolaItem item;
					if (!Recv(sizeof(TTombolaItem), &item))
					{
						TraceError("Couldn't read item");
						return false;
					}

					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_TombolaSetItem", Py_BuildValue("(iiii)", byGroup, item.dwVnum, item.dwCount, item.dwChance));
				}
			}
			break;
		default:
			TraceError("RecvTombolaPacket: unknown subheader %d", packet.bySubHeader);
			break;
	}

	return true;
}
#endif

#if defined(__BL_MOVE_CHANNEL__)
bool CPythonNetworkStream::SendMoveChannelPacket(const BYTE bChannelIndex)
{
	TPacketCGMoveChannel p;
	p.header = HEADER_CG_MOVE_CHANNEL;
	p.channel = bChannelIndex;

	if (!Send(sizeof(p), &p))
	{
		Tracef("CPythonNetworkStream::SendMoveChannelPacket Error\n");
		return false;
	}

	return SendSequence();
}
#endif

#ifdef TITLE_SYSTEM_BYLUZER
bool CPythonNetworkStream::RecvTitleActive()
{
	TPacketGCTitleActiveNow p;
	if (!Recv(sizeof(TPacketGCTitleActiveNow), &p))
		return false;
	
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SendTitleActiveNow", Py_BuildValue("(i)", p.id));

	CPythonCharacterManager & rkChrMgr = CPythonCharacterManager::Instance();
	CInstanceBase* pkInstMain = rkChrMgr.GetMainInstancePtr();
	if (pkInstMain)
		pkInstMain->UpdateTextTailGrade(p.id);

	return true;
}

bool CPythonNetworkStream::RecvTitleInfo()
{
	TPacketGCTitleActive p;

	if (!Recv(sizeof(TPacketGCTitleActive), & p))
		return false;
	
	//PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "TitleEnableClear", Py_BuildValue("()"));
	
	for (int i = 0; i < TITLE_MAX_NUM; i++)
	{
		TPlayerTitle & rPlayerTitle = p.titles[i];
		if (rPlayerTitle.active == 1) {
			//			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SendTitleEnable", Py_BuildValue("(i)", rPlayerTitle.titletype));
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SendTitleEnable", Py_BuildValue("(is)", i, "Hidden"));
		}
		else if (rPlayerTitle.active == 0) {
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SendTitleEnable", Py_BuildValue("(is)", i, "False"));
		}
		else if (rPlayerTitle.active == 2) {
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SendTitleEnable", Py_BuildValue("(is)", i, "True"));
		}
	}
	return true;
}
#endif

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
bool CPythonNetworkStream::RecvOfflineShopLogs()
{
	TPacketGCOfflineShopLogs p;
	/*std::vector<int> vecBuffer;
	vecBuffer.clear();
	TPacketGCOfflineShopLogs* p = (TPacketGCOfflineShopLogs*)vecBuffer.data();*/

	if (!Recv(sizeof(TPacketGCOfflineShopLogs), &p))
		return false;

	for (int i = 0; i < 20; i++)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SendOfflineShopLogs", Py_BuildValue("(iiiLLss)", i, p.log[i].itemVnum, p.log[i].itemCount, p.log[i].price, p.log[i].price2, p.log[i].date, p.log[i].action));
	}
	return true;
}
#endif

#ifdef WEEKLY_RANK_BYLUZER
bool CPythonNetworkStream::RecvWeeklyPage()
{
	TPacketGCSelectWeeklyPage p;
	if (!Recv(sizeof(TPacketGCSelectWeeklyPage), &p))
		return false;
	
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SendWeeklyPage", Py_BuildValue("(ibi)", p.page, p.can_select, p.season));
	return true;
}

bool CPythonNetworkStream::RecvWeeklyRank()
{
	TPacketGCSendWeeklyRank p;
	if (!Recv(sizeof(TPacketGCSendWeeklyRank), &p))
		return false;
	for (int i = 0; i < 10; i++)
	{
		TPacketGCTableWeeklyRank & rPlayerWeekly = p.rankingData[i];
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SendWeeklyInfo", Py_BuildValue("(isiii)", rPlayerWeekly.rank, rPlayerWeekly.name, rPlayerWeekly.points, rPlayerWeekly.empire, rPlayerWeekly.job));
	}
	return true;
}
#endif

#ifdef ENABLE_SWITCHBOT
bool CPythonNetworkStream::RecvSwitchbotPacket()
{
	TPacketGCSwitchbot pack;
	if (!Recv(sizeof(pack), &pack))
	{
		return false;
	}

	size_t packet_size = int(pack.size) - sizeof(TPacketGCSwitchbot);
	if (pack.subheader == SUBHEADER_GC_SWITCHBOT_UPDATE)
	{
		if (packet_size != sizeof(CPythonSwitchbot::TSwitchbotTable))
		{
			return false;
		}

		CPythonSwitchbot::TSwitchbotTable table;
		if (!Recv(sizeof(table), &table))
		{
			return false;
		}

		CPythonSwitchbot::Instance().Update(table);
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSwitchbotWindow", Py_BuildValue("()"));
	}
	else if (pack.subheader == SUBHEADER_GC_SWITCHBOT_UPDATE_ITEM)
	{
		if (packet_size != sizeof(TSwitchbotUpdateItem))
		{
			return false;
		}

		TSwitchbotUpdateItem update;
		if (!Recv(sizeof(update), &update))
		{
			return false;
		}

		TItemPos pos(SWITCHBOT, update.slot);

		IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
		rkPlayer.SetItemCount(pos, update.count);

		for (int i = 0; i < ITEM_SOCKET_SLOT_MAX_NUM; ++i)
		{
			rkPlayer.SetItemMetinSocket(pos, i, update.alSockets[i]);

		}

		for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
		{
			rkPlayer.SetItemAttribute(pos, j, update.aAttr[j].bType, update.aAttr[j].sValue);
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSwitchbotItem", Py_BuildValue("(i)", update.slot));
		return true;
	}
	else if (pack.subheader == SUBHEADER_GC_SWITCHBOT_SEND_ATTRIBUTE_INFORMATION)
	{
		CPythonSwitchbot::Instance().ClearAttributeMap();

		size_t table_size = sizeof(CPythonSwitchbot::TSwitchbottAttributeTable);
		while (packet_size >= table_size)
		{
			const int test = sizeof(CPythonSwitchbot::TSwitchbottAttributeTable);

			CPythonSwitchbot::TSwitchbottAttributeTable table;
			if (!Recv(table_size, &table))
			{
				return false;
			}

			CPythonSwitchbot::Instance().AddAttributeToMap(table);
			packet_size -= table_size;
		}
	}

	return true;
}

//bool CPythonNetworkStream::SendSwitchbotStartPacket(BYTE slot, DWORD switchingItemVnum, std::vector<CPythonSwitchbot::TSwitchbotAttributeAlternativeTable> alternatives)
bool CPythonNetworkStream::SendSwitchbotStartPacket(BYTE slot, std::vector<CPythonSwitchbot::TSwitchbotAttributeAlternativeTable> alternatives)
{
	TPacketCGSwitchbot pack;
	pack.header = HEADER_CG_SWITCHBOT;
	pack.subheader = SUBHEADER_CG_SWITCHBOT_START;
	pack.size = sizeof(TPacketCGSwitchbot) + sizeof(CPythonSwitchbot::TSwitchbotAttributeAlternativeTable) * SWITCHBOT_ALTERNATIVE_COUNT;
	pack.slot = slot;
	//pack.switchingItemVnum = switchingItemVnum;

	if (!Send(sizeof(pack), &pack))
	{
		return false;
	}

	for (const auto& it : alternatives)
	{
		if (!Send(sizeof(it), &it))
		{
			return false;
		}
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendSwitchbotStopPacket(BYTE slot)
{
	TPacketCGSwitchbot pack;
	pack.header = HEADER_CG_SWITCHBOT;
	pack.subheader = SUBHEADER_CG_SWITCHBOT_STOP;
	pack.size = sizeof(TPacketCGSwitchbot);
	pack.slot = slot;
	//pack.switchingItemVnum = 0;

	if (!Send(sizeof(pack), &pack))
	{
		return false;
	}

	return SendSequence();
}
#endif

#ifdef ENABLE_RESP_SYSTEM
bool CPythonNetworkStream::SendRespFetchDropPacket(uint32_t mobVnum)
{
	if (!__CanActMainInstance())
		return false;

	TPacketCGRespHeader header{};
	header.header = HEADER_CG_RESP;
	header.subheader = RESP_CG_SUBHEADER_FETCH_DROP;

	if (!Send(sizeof(TPacketCGRespHeader), &header) ||
		!Send(sizeof(uint32_t), &mobVnum))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendRespFetchRespPacket(uint32_t mobVnum)
{
	if (!__CanActMainInstance())
		return false;

	TPacketCGRespHeader header{};
	header.header = HEADER_CG_RESP;
	header.subheader = RESP_CG_SUBHEADER_FETCH_RESP;

	if (!Send(sizeof(TPacketCGRespHeader), &header) ||
		!Send(sizeof(uint32_t), &mobVnum))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendRespTeleportPacket(uint32_t id)
{
	if (!__CanActMainInstance())
		return false;

	TPacketCGRespHeader header{};
	header.header = HEADER_CG_RESP;
	header.subheader = RESP_CG_SUBHEADER_TELEPORT;

	if (!Send(sizeof(TPacketCGRespHeader), &header) ||
		!Send(sizeof(uint32_t), &id))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvRespPacket()
{
	TPacketGCRespHeader header{};
	if (!Recv(sizeof(TPacketGCRespHeader), &header))
		return false;
 
	switch (header.subheader)
	{
		case RESP_GC_SUBHEADER_DATA_RESP:
		case RESP_GC_SUBHEADER_DATA_DROP:
		{
			uint16_t itemCount = 0;
			if (!Recv(sizeof(uint16_t), &itemCount))
				return false;
 
			uint32_t mobVnum = 0;
			if (!Recv(sizeof(uint32_t), &mobVnum))
				return false;
 
			switch (header.subheader)
			{
				case RESP_GC_SUBHEADER_DATA_RESP:
				{
					PyObject* respDataList = PyList_New(itemCount);
 
					for (int i=0; i < itemCount; i++)
					{
						TPacketGCRespData pack{};
						if (!Recv(sizeof(TPacketGCRespData), &pack))
							return false;
 
						PyObject* positionTuple = PyTuple_New(2);
						PyTuple_SetItem(positionTuple, 0, Py_BuildValue("i", pack.x));
						PyTuple_SetItem(positionTuple, 1, Py_BuildValue("i", pack.y));
 
						PyObject* respDict = PyDict_New();
 
						PyDict_SetItem(respDict, Py_BuildValue("s", "resp"), Py_BuildValue("L", pack.nextRespTime));
						PyDict_SetItem(respDict, Py_BuildValue("s", "cord"), Py_BuildValue("O", positionTuple));
						PyDict_SetItem(respDict, Py_BuildValue("s", "time"), Py_BuildValue("i", pack.time));
						PyDict_SetItem(respDict, Py_BuildValue("s", "id"), Py_BuildValue("k", pack.id));
 
						PyList_SetItem(respDataList, i, respDict);
					}
 
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetMobRespData", Py_BuildValue("(lO)", mobVnum, respDataList));
 
					break;
				}
 
				case RESP_GC_SUBHEADER_DATA_DROP:
				{
					TPacketGCRespGold gold{};
					if (!Recv(sizeof(TPacketGCRespGold), &gold))
						return false;
 
					PyObject* dropDataList = PyList_New(itemCount + 1);
 
					PyObject* goldDropDict = PyDict_New();
 
					PyDict_SetItem(goldDropDict, Py_BuildValue("s", "vnum"), Py_BuildValue("i", 1));
					PyDict_SetItem(goldDropDict, Py_BuildValue("s", "count"), Py_BuildValue("i", gold.minGold));
 
					PyList_SetItem(dropDataList, 0, goldDropDict);
 
					for (int i = 0; i < itemCount; i++)
					{
						TPacketGCRespItem data{};
						if (!Recv(sizeof(TPacketGCRespItem), &data))
							return false;
 
						PyObject* dropDict = PyDict_New();
 
						PyDict_SetItem(dropDict, Py_BuildValue("s", "vnum"), Py_BuildValue("i", data.vnum));
						PyDict_SetItem(dropDict, Py_BuildValue("s", "count"), Py_BuildValue("i", data.minCount));
 
						PyList_SetItem(dropDataList, i + 1, dropDict);
					}
 
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetMobDropData", Py_BuildValue("(lO)", mobVnum, dropDataList));
 
					break;
				}
			}
			break;
		}
		case RESP_GC_SUBHEADER_DATA_MOB:
		{
			TPacketGCMapData pack{};
			if (!Recv(sizeof(TPacketGCMapData), &pack))
				return false;
 
			PyObject* mobDataList = PyList_New(pack.mobCount);
 
			for (int i = 0; i < pack.mobCount; i++)
			{
				uint32_t mobVnum = 0;
				if (!Recv(sizeof(uint32_t), &mobVnum))
					return false;
 
				PyList_SetItem(mobDataList, i, Py_BuildValue("l", mobVnum));
			}
 
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetMapData", Py_BuildValue("(Oiiii)", 
				mobDataList, pack.currentBossCount, pack.maxBossCount, pack.currentMetinCount, pack.maxMetinCount));
			break;
		}
		case RESP_GC_SUBHEADER_REFRESH_RESP:
		{
			TPacketGCRespRefresh pack{};
			if (!Recv(sizeof(TPacketGCRespRefresh), &pack))
				return false;
 
			PyObject* positionTuple = PyTuple_New(2);
			PyTuple_SetItem(positionTuple, 0, Py_BuildValue("i", pack.x));
			PyTuple_SetItem(positionTuple, 1, Py_BuildValue("i", pack.y));
 
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RefreshResp",
				Py_BuildValue("(IiLO)", pack.id, pack.vnum, pack.nextRespTime, positionTuple));
#ifdef ENABLE_BOSS_SHOWER_ON_ATLAS
			CPythonMiniMap::Instance().UpdateBossRespawnTime(pack.id, pack.nextRespTime);
#endif
			break;
		}
	}
 
	return true;
}
#endif

#ifdef ENABLE_NEW_PET_SYSTEM
bool CPythonNetworkStream::SendPetAction(DWORD skillvnum)
{
	if (!__CanActMainInstance())
		return true;

	TPacketPetAction packet;
	packet.header = HEADER_CG_PET_ACTION;
	packet.skillvnum = skillvnum;

	if (!Send(sizeof(TPacketPetAction), &packet))
		return false;

	return SendSequence();
}
#endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
bool CPythonNetworkStream::SendBuffNPCCreate(BYTE bAction, const char* szName, BYTE bSex)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGBuffNPCCreate packet;
	packet.bHeader = HEADER_CG_BUFF_NPC_CREATE;
	packet.bAction = bAction;
	strncpy(packet.szName, szName, sizeof(packet.szName));
	packet.bSex = bSex;

	if (!Send(sizeof(TPacketCGBuffNPCCreate), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendBuffNPCAction(BYTE bAction, DWORD dValue0, DWORD dValue1)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGBuffNPCAction packet;
	packet.bHeader = HEADER_CG_BUFF_NPC_ACTION;
	packet.bAction = bAction;
	packet.dValue0 = dValue0;
	packet.dValue1 = dValue1;

	if (!Send(sizeof(TPacketCGBuffNPCAction), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvBuffNPCAction()
{
	TPacketGCBuffNPCAction packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	switch (packet.bAction)
	{
		case 0:
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OpenCreateBuffWindow", Py_BuildValue("()"));
			break;
		}
		case 1:
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OpenChangeNameWindow", Py_BuildValue("()"));
			break;
		}
	}

	//PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RecFlashAuctionItemInfo", Py_BuildValue("(iiiiiiiiiii)",
	//	packet.dIndex, packet.dItemVnum, packet.dItemCount, packet.bSellCountType, packet.dSellStartCount, packet.dSellCurCount, packet.bPriceType, packet.dPriceCur, packet.dTimeStart, packet.dTimeEnd, packet.dTimeNextRefresh
	//));

	return true;
}
#endif

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
bool CPythonNetworkStream::SendMountNPCAction(BYTE bAction, DWORD dValue0)
{
	if (!__CanActMainInstance())
		return true;

	SPacketCGMountNPCAction packet;
	packet.bHeader = HEADER_CG_MOUNT_NPC_ACTION;
	packet.bAction = bAction;
	packet.dValue0 = dValue0;

	if (!Send(sizeof(SPacketCGMountNPCAction), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvMountNPCAction()
{
	TPacketGCMountNPCAction packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	switch (packet.bAction)
	{
	case 0:
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OpenCreateMountWindow", Py_BuildValue("()"));
		break;
	}
	}

	//PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RecFlashAuctionItemInfo", Py_BuildValue("(iiiiiiiiiii)",
	//	packet.dIndex, packet.dItemVnum, packet.dItemCount, packet.bSellCountType, packet.dSellStartCount, packet.dSellCurCount, packet.bPriceType, packet.dPriceCur, packet.dTimeStart, packet.dTimeEnd, packet.dTimeNextRefresh
	//));

	return true;
}
#endif

#ifdef ENABLE_AURA_SYSTEM
bool CPythonNetworkStream::RecvAuraPacket(bool bReturn)
{
	TPacketAura sPacket;
	if (!Recv(sizeof(sPacket), &sPacket))
		return bReturn;

	bReturn = true;
	switch (sPacket.subheader)
	{
	case AURA_SUBHEADER_GC_OPEN:
	{
		CPythonAura::Instance().Clear();
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActAura", Py_BuildValue("(ib)", 1, sPacket.bWindow));
	}
	break;
	case AURA_SUBHEADER_GC_CLOSE:
	{
		CPythonAura::Instance().Clear();
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActAura", Py_BuildValue("(ib)", 2, sPacket.bWindow));
	}
	break;
	case AURA_SUBHEADER_GC_ADDED:
	{
		CPythonAura::Instance().AddMaterial(sPacket.dwPrice, sPacket.bPos, sPacket.tPos);
		if (sPacket.bPos == 1)
		{
			CPythonAura::Instance().AddResult(sPacket.dwItemVnum, sPacket.dwMinAbs, sPacket.dwMaxAbs);
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AlertAura", Py_BuildValue("(b)", sPacket.bWindow));
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActAura", Py_BuildValue("(ib)", 3, sPacket.bWindow));
	}
	break;
	case AURA_SUBHEADER_GC_REMOVED:
	{
		CPythonAura::Instance().RemoveMaterial(sPacket.dwPrice, sPacket.bPos);
		if (sPacket.bPos == 0)
			CPythonAura::Instance().RemoveMaterial(sPacket.dwPrice, 1);

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActAura", Py_BuildValue("(ib)", 4, sPacket.bWindow));
	}
	break;
	case AURA_SUBHEADER_GC_REFINED:
	{
		if (sPacket.bPos != 255)
		{
			if (sPacket.dwMaxAbs == 0)
				CPythonAura::Instance().RemoveMaterial(sPacket.dwPrice, 1);
			else
			{
				CPythonAura::Instance().RemoveMaterial(sPacket.dwPrice, 0);
				CPythonAura::Instance().RemoveMaterial(sPacket.dwPrice, 1);
			}
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActAura", Py_BuildValue("(ib)", 4, sPacket.bWindow));
	}
	break;
	default:
		TraceError("CPythonNetworkStream::RecvAuraPacket: unknown subheader %d\n.", sPacket.subheader);
		break;
	}

	return bReturn;
}

bool CPythonNetworkStream::SendAuraClosePacket()
{
	if (!__CanActMainInstance())
		return true;

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAura sPacket;
	sPacket.header = HEADER_CG_AURA;
	sPacket.subheader = AURA_SUBHEADER_CG_CLOSE;
	sPacket.dwPrice = 0;
	sPacket.bPos = 0;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;
	if (!Send(sizeof(sPacket), &sPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendAuraAddPacket(TItemPos tPos, BYTE bPos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketAura sPacket;
	sPacket.header = HEADER_CG_AURA;
	sPacket.subheader = AURA_SUBHEADER_CG_ADD;
	sPacket.dwPrice = 0;
	sPacket.bPos = bPos;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;
	if (!Send(sizeof(sPacket), &sPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendAuraRemovePacket(BYTE bPos)
{
	if (!__CanActMainInstance())
		return true;

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAura sPacket;
	sPacket.header = HEADER_CG_AURA;
	sPacket.subheader = AURA_SUBHEADER_CG_REMOVE;
	sPacket.dwPrice = 0;
	sPacket.bPos = bPos;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;
	if (!Send(sizeof(sPacket), &sPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendAuraRefinePacket()
{
	if (!__CanActMainInstance())
		return true;

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAura sPacket;
	sPacket.header = HEADER_CG_AURA;
	sPacket.subheader = AURA_SUBHEADER_CG_REFINE;
	sPacket.dwPrice = 0;
	sPacket.bPos = 0;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;
	if (!Send(sizeof(sPacket), &sPacket))
		return false;

	return SendSequence();
}
#endif

#ifdef ENABLE_CUBE_RENEWAL_WORLDARD

bool CPythonNetworkStream::CubeRenewalMakeItem(int index_item, int count_item, int index_item_improve)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGCubeRenewalSend	packet;

	packet.header		= HEADER_CG_CUBE_RENEWAL;
	packet.subheader	= CUBE_RENEWAL_SUB_HEADER_MAKE_ITEM;
	packet.index_item 	= index_item;
	packet.count_item   = count_item;
	packet.index_item_improve	= index_item_improve;

	if (!Send(sizeof(TPacketCGCubeRenewalSend), &packet))
	{
		Tracef("CPythonNetworkStream::CubeRenewalMakeItem Error\n");
		return false;
	}

	return true;
}
bool CPythonNetworkStream::CubeRenewalClose()
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGCubeRenewalSend	packet;

	packet.header		= HEADER_CG_CUBE_RENEWAL;
	packet.subheader	= CUBE_RENEWAL_SUB_HEADER_CLOSE;

	if (!Send(sizeof(TPacketCGCubeRenewalSend), &packet))
	{
		Tracef("CPythonNetworkStream::CubeRenewalClose Error\n");
		return false;
	}

	return true;
}

bool CPythonNetworkStream::RecvCubeRenewalPacket()
{

	TPacketGCCubeRenewalReceive CubeRenewalPacket;

	if (!Recv(sizeof(CubeRenewalPacket), &CubeRenewalPacket))
		return false;

	switch (CubeRenewalPacket.subheader)
	{

		case CUBE_RENEWAL_SUB_HEADER_OPEN_RECEIVE:
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_CUBE_RENEWAL_OPEN", Py_BuildValue("()"));
		}
		break;

		case CUBE_RENEWAL_SUB_HEADER_DATES_RECEIVE:
		{
			CPythonCubeRenewal::Instance().ReceiveList(CubeRenewalPacket.date_cube_renewal);
		}
		break;

		case CUBE_RENEWAL_SUB_HEADER_DATES_LOADING:
		{
			CPythonCubeRenewal::Instance().LoadingList();
		}
		break;

		case CUBE_RENEWAL_SUB_HEADER_CLEAR_DATES_RECEIVE:
		{
			CPythonCubeRenewal::Instance().ClearList();
		}
		break;
	}

	return true;
}

#endif

#ifdef ENABLE_REMOVE_ITEM
bool CPythonNetworkStream::SendRemoveItemPacket(std::vector<TItemPos> vec_item_pos)
{
	if (!__CanActMainInstance())
		return true;

	if (vec_item_pos.size() > REMOVE_ITEM_SLOT_COUNT_X * REMOVE_ITEM_SLOT_COUNT_Y)
		return false;

	TPacketCGRemoveItem packet;
	packet.header = HEADER_CG_REMOVE_ITEM;
	packet.item_count = vec_item_pos.size();

	if (!Send(sizeof(TPacketCGRemoveItem), &packet))
		return false;

	for (const auto& item_pos : vec_item_pos)
	{
		if (!Send(sizeof(TItemPos), &item_pos))
			return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvRemoveItemPacket()
{
	TPacketGCRemoveItem packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RemoveItemRefreshWindow", Py_BuildValue("()"));
	return true;
}
#endif

#ifdef ENABLE_VS_SHOP_SEARCH
bool CPythonNetworkStream::SendSearchPacket(std::vector<TSearchItems> vec_items)
{
	if (!__CanActMainInstance())
		return true;

	if (vec_items.size() > 36)
		return false;

	TPacketCGSearch packet;
	packet.header = HEADER_CG_SEARCH;
	packet.items = vec_items.size();

	if (!Send(sizeof(TPacketCGSearch), &packet))
		return false;

	for (const auto items : vec_items)
	{
		if (!Send(sizeof(TSearchItems), &items))
			return false;
	}

	return SendSequence();
}
#endif

#ifdef ENABLE_ODLAMKI_SYSTEM
bool CPythonNetworkStream::SendOdlamkiItemPacket(std::vector<TItemPos> vec_item_pos)
{
	if (!__CanActMainInstance())
		return true;

	if (vec_item_pos.size() > 18)
		return false;

	TPacketCGOdlamki packet;
	packet.header = HEADER_CG_ODLAMKI_ITEM;
	packet.item_count = vec_item_pos.size();

	if (!Send(sizeof(TPacketCGOdlamki), &packet))
		return false;

	for (const auto& item_pos : vec_item_pos)
	{
		if (!Send(sizeof(TItemPos), &item_pos))
			return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvOdlamkiItemPacket()
{
	TPacketGCOdlamki packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OdlamkiItemRefreshWindow", Py_BuildValue("()"));
	return true;
}
#endif

#ifdef ENABLE_SEARCH_SHOP
bool CPythonNetworkStream::SendShopSearchPacket(DWORD vnum)
{
	TPacketCGShopSearch packet;
	packet.bHeader = HEADER_CG_SHOP_SEARCH;
	packet.bSubHeader = SHOPSEARCH_FIND;
	packet.vnum = vnum;

	if (!Send(sizeof(packet), &packet))
	{
		Tracen("Send Shop Search Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendDeleteShopSearchPacket()
{
	TPacketCGShopSearch packet;
	packet.bHeader = HEADER_CG_SHOP_SEARCH;
	packet.bSubHeader = SHOPSEARCH_DELETE;
	packet.vnum = 0;

	if (!Send(sizeof(packet), &packet))
	{
		Tracen("Send Shop Search Packet Error");
		return false;
	}

	return SendSequence();
}
#endif

#ifdef ENABLE_CHEQUE_SYSTEM
bool CPythonNetworkStream::SendWonExchangeSellPacket(WORD wValue)
{
	TPacketCGWonExchange kWonExchangePacket;
	kWonExchangePacket.bSubHeader = WON_EXCHANGE_CG_SUBHEADER_SELL;
	kWonExchangePacket.wValue = wValue;

	if (!Send(sizeof(TPacketCGWonExchange), &kWonExchangePacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendWonExchangeBuyPacket(WORD wValue)
{
	TPacketCGWonExchange kWonExchangePacket;
	kWonExchangePacket.bSubHeader = WON_EXCHANGE_CG_SUBHEADER_BUY;
	kWonExchangePacket.wValue = wValue;

	if (!Send(sizeof(TPacketCGWonExchange), &kWonExchangePacket))
		return false;

	return SendSequence();
}
#endif

#if defined(ENABLE_DUNGEON_INFO_SYSTEM)
bool CPythonNetworkStream::RecvDungeonInfo()
{
	DungeonInfo::Packet::GCInfo GCPacket;
	if (!Recv(sizeof(GCPacket), &GCPacket))
		return false;

	switch (GCPacket.bySubHeader)
	{
	case DungeonInfo::Packet::SUBHEADER_DUNGEON_INFO_SEND:
	{
		CPythonDungeonInfo::Instance().AddDungeon(GCPacket.byIndex, GCPacket);
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DungeonInfoReload", Py_BuildValue("(i)", GCPacket.bReset));
	}
	break;

	case DungeonInfo::Packet::SUBHEADER_DUNGEON_INFO_OPEN:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DungeonInfoOpen", Py_BuildValue("()"));
		break;
	}

	return true;
}

bool CPythonNetworkStream::SendDungeonInfo(BYTE byAction, BYTE byIndex, BYTE byRankType)
{
	DungeonInfo::Packet::CGInfo CGPacket;
	CGPacket.byHeader = DungeonInfo::Packet::HEADER_CG_DUNGEON_INFO;
	CGPacket.bySubHeader = byAction;
	CGPacket.byIndex = byIndex;
	CGPacket.byRankType = byRankType;

	if (!Send(sizeof(CGPacket), &CGPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvDungeonRanking()
{
	DungeonInfo::Packet::GCRank GCPacket;
	if (!Recv(sizeof(GCPacket), &GCPacket))
	{
		Tracen("RecvDungeonRanking Error");
		return false;
	}

	CPythonDungeonInfo::Instance().AddRanking(GCPacket.szName, GCPacket.iLevel, GCPacket.dwPoints);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DungeonRankingRefresh", Py_BuildValue("()"));

	return true;
}
#endif

#ifdef ENABLE_MINIMAP_DUNGEONINFO
bool CPythonNetworkStream::RecvMiniMapDungeonInfo()
{
	TPacketGCMiniMapDungeonInfo packet;
	if (!Recv(sizeof(packet), &packet)) {
		Tracen("CPythonNetworkStream::RecvMiniMapDungeonInfo - PACKET READ ERROR");
		return false;
	}

	switch (packet.bSubHeader) {
	case MINIMAP_DUNGEONINFO_SUBHEADER_STATUS: { PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetMiniMapDungeonInfoState", Py_BuildValue("(i)", packet.dValue1)); } break;
	case MINIMAP_DUNGEONINFO_SUBHEADER_STAGE: { PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetMiniMapDungeonInfoStage", Py_BuildValue("(ii)", packet.dValue1, packet.dValue2)); } break;
	case MINIMAP_DUNGEONINFO_SUBHEADER_GAUGE: { PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetMiniMapDungeonInfoGauge", Py_BuildValue("(iii)", packet.dValue1, packet.dValue2, packet.dValue3)); } break;
	case MINIMAP_DUNGEONINFO_SUBHEADER_NOTICE: { PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetMiniMapDungeonInfoNotice", Py_BuildValue("(s)", packet.szNotice)); } break;
	case MINIMAP_DUNGEONINFO_SUBHEADER_BUTTON: { PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetMiniMapDungeonInfoButton", Py_BuildValue("(i)", packet.buttonvalue)); } break;
	case MINIMAP_DUNGEONINFO_SUBHEADER_TIMER: { PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetMiniMapDungeonInfoTimer", Py_BuildValue("(ii)", packet.timerstatus, packet.time)); } break;
	}

	return true;
}
#endif

#ifdef ENABLE_COLLECT_WINDOW
bool CPythonNetworkStream::RecvCollectPacket()
{
	TPacketGCCollectWindow kCollectWindow;

	if (!Recv(sizeof(kCollectWindow), &kCollectWindow))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_UpdateCollectWindow", Py_BuildValue("(iiiiiiiii)", kCollectWindow.bWindowType, kCollectWindow.dwTime, kCollectWindow.bCount, kCollectWindow.dwItemVnum, kCollectWindow.bCountTotal, kCollectWindow.bChance, kCollectWindow.bRenderTargetID, kCollectWindow.bQuestIndex, kCollectWindow.RequiredLevel));

	return true;
}
#endif

bool CPythonNetworkStream::RecvAntyExp()
{
	TPacketGCAntyExp antyexpPacket;
	if (!Recv(sizeof(antyexpPacket), &antyexpPacket)) {
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SendAntyExp", Py_BuildValue("(i)", antyexpPacket.status));
	return true;
}

#ifdef WJ_SPLIT_INVENTORY_SYSTEM
bool CPythonNetworkStream::RecvWpadanie()
{
	TPacketGCWpadanie packet;
	if (!Recv(sizeof(packet), &packet)) {
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SendWpadanie", Py_BuildValue("(i)", packet.status));
	return true;
}
#endif

#ifdef ENABLE_EVENT_MANAGER
bool CPythonNetworkStream::RecvEventManager()
{
	TPacketGCEventManager p;
	if (!Recv(sizeof(TPacketGCEventManager), &p))
		return false;
	BYTE subIndex;
	if (!Recv(sizeof(BYTE), &subIndex))
		return false;
	if (subIndex == EVENT_MANAGER_LOAD)
	{
		BYTE dayCount;
		if (!Recv(sizeof(BYTE), &dayCount))
			return false;
		int serverTime;
		if (!Recv(sizeof(int), &serverTime))
			return false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ClearEventManager", Py_BuildValue("()"));
		for (BYTE j = 0; j < dayCount; ++j)
		{
			BYTE dayIndex;
			if (!Recv(sizeof(BYTE), &dayIndex))
				return false;
			BYTE dayEventCount;
			if (!Recv(sizeof(BYTE), &dayEventCount))
				return false;
			if (dayEventCount > 0)
			{
				std::vector<TEventManagerData> m_vec;
				m_vec.resize(dayEventCount);
				if (!Recv(dayEventCount * sizeof(TEventManagerData), &m_vec[0]))
					return false;
				for (const auto& eventPtr : m_vec)
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AppendEvent", Py_BuildValue("(iiissiiiiiiiii)", dayIndex, eventPtr.eventID, eventPtr.eventIndex, eventPtr.startTimeText, eventPtr.endTimeText, eventPtr.empireFlag, eventPtr.channelFlag, eventPtr.value[0], eventPtr.value[1], eventPtr.value[2], eventPtr.value[3], eventPtr.startTime - serverTime, eventPtr.endTime == 0 ? 0 : eventPtr.endTime - serverTime, eventPtr.eventStatus));
			}
		}
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshEventManager", Py_BuildValue("()"));
	}
	else if (subIndex == EVENT_MANAGER_EVENT_STATUS)
	{
		WORD eventID;
		if (!Recv(sizeof(WORD), &eventID))
			return false;
		bool eventStatus;
		if (!Recv(sizeof(bool), &eventStatus))
			return false;
		int endTime;
		if (!Recv(sizeof(int), &endTime))
			return false;
		char endTimeText[25];
		if (!Recv(sizeof(endTimeText), &endTimeText))
			return false;
		int serverTime;
		if (!Recv(sizeof(int), &serverTime))
			return false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshEventStatus", Py_BuildValue("(iiis)", eventID, eventStatus, endTime == 0 ? 0 : endTime - serverTime, endTimeText));
	}
	else if (subIndex == EVENT_MANAGER_PLAYER_ACTIVATED)
	{
		WORD count;
		if (!Recv(sizeof(WORD), &count))
			return false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ClearPersonalActivatedEvents", Py_BuildValue("()"));
		for (WORD i = 0; i < count; ++i)
		{
			WORD eventID;
			if (!Recv(sizeof(WORD), &eventID))
				return false;
			int activationTime;
			if (!Recv(sizeof(int), &activationTime))
				return false;
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetPersonalEventActivated", Py_BuildValue("(ii)", eventID, activationTime));
		}
	}
	return true;
}

bool CPythonNetworkStream::SendEventActivatePacket(WORD eventID)
{
	TPacketCGEventActivate packet;
	packet.header = HEADER_CG_EVENT_ACTIVATE;
	packet.eventID = eventID;
	if (!Send(sizeof(packet), &packet))
		return false;
	return SendSequence();
}

bool CPythonNetworkStream::RecvEventActivateResult()
{
	TPacketGCEventActivateResult p;
	if (!Recv(sizeof(TPacketGCEventActivateResult), &p))
		return false;
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "EventActivateResult", Py_BuildValue("(ii)", p.eventID, p.result));
	return true;
}
#endif

#ifdef TAKE_LEGEND_DAMAGE_BOARD_SYSTEM
bool CPythonNetworkStream::RecvLegendDamageData()
{
	TPacketGCSendLegendDamage p;
	if (!Recv(sizeof(TPacketGCSendLegendDamage), &p))
		return false;

	for (int i = 0; i < LEGEND_DAMAGE_BOARD_MAX_ENTRIES; i++)
	{
		TPacketGCTableLegendDamage& rLegendDamageRanking = p.legendDamageData[i];
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SendLegendDamageData", Py_BuildValue("(iisiiiL)", p.dwBossVID, i, rLegendDamageRanking.cName, rLegendDamageRanking.iLevel, rLegendDamageRanking.iRace, rLegendDamageRanking.iEmpire, (long long)rLegendDamageRanking.iDamage));
	}
	return true;
}
#endif

#ifdef ENABLE_ENTITY_PRELOADING
bool CPythonNetworkStream::RecvPreloadEntitiesPacket()
{
	TPacketGCPreloadEntities pack{};
	if (!Recv(sizeof(pack), &pack))
		return false;

	assert(int32_t(pack.size) - sizeof(pack) == pack.count * sizeof(uint32_t) && "HEADER_GC_PRELOAD_ENTITIES");

	uint32_t dwEntityRace = 0;
	CInstanceBase::SCreateData d{};
	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();

	for (uint16_t i = 0; i < pack.count; i++)
	{
		if (!Recv(sizeof(uint32_t), &dwEntityRace))
			return false;

#ifdef _DEBUG
		TraceError("Preloading %d", dwEntityRace);
#endif
		d.m_dwRace = dwEntityRace;
		d.m_dwVID = dwEntityRace;

		if (rkChrMgr.CreateInstance(d))//create the entity, force it to load absolutely everything
			rkChrMgr.DeleteInstance(dwEntityRace);// now get rid of it
		else
		{
			//TraceError("Failed to preload race %d", dwEntityRace);
		}
	}



	return true;
}
#endif

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
bool CPythonNetworkStream::SendBuildPrivateShopPacket(const char* c_szTitle, DWORD dwPolyVnum, BYTE bTitleType, BYTE bPageCount, const std::vector<TPrivateShopItem>& c_vec_itemStock)
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_BUILD;

	TPacketCGPrivateShopBuild subPacket{};
	strncpy(subPacket.szTitle, c_szTitle, TITLE_MAX_LEN);
	subPacket.dwPolyVnum = dwPolyVnum;
	subPacket.bTitleType = bTitleType;
	subPacket.bPageCount = bPageCount;
	subPacket.wItemCount = static_cast<WORD>(c_vec_itemStock.size());

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	if (!Send(sizeof(subPacket), &subPacket))
		return false;

	for (const auto& c_rShopItem : c_vec_itemStock)
	{
		if (!Send(sizeof(c_rShopItem), &c_rShopItem))
			return false;
	}

	return true;
}

bool CPythonNetworkStream::SendClosePrivateShopPacket()
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_CLOSE;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	return true;
}

bool CPythonNetworkStream::SendOpenPrivateShopPanelPacket(bool bCheck)
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;

	if (bCheck)
		mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_PANEL_OPEN_CHECK;
	else
		mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_PANEL_OPEN;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	return true;
}

bool CPythonNetworkStream::SendClosePrivateShopPanelPacket()
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_PANEL_CLOSE;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	return true;
}

bool CPythonNetworkStream::SendPrivateShopStartPacket(DWORD dwVID)
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_START;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	if (!Send(sizeof(dwVID), &dwVID))
		return false;

	return true;
}

bool CPythonNetworkStream::SendPrivateShopEndPacket()
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_END;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	return true;
}

bool CPythonNetworkStream::SendPrivateShopBuyPacket(WORD wPos)
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_BUY;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	if (!Send(sizeof(wPos), &wPos))
		return false;

	return true;
}

bool CPythonNetworkStream::SendWithdrawPrivateShopPacket()
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_WITHDRAW;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	return true;
}

bool CPythonNetworkStream::SendModifyPrivateShopPacket()
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_MODIFY;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	return true;
}

bool CPythonNetworkStream::SendItemPriceChangePrivateShopPacket(WORD wPos, long long llGold, DWORD dwCheque)
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_ITEM_PRICE_CHANGE;

	TPacketCGPrivateShopItemPriceChange subPacket{};
	subPacket.wPos = wPos;
	subPacket.TPrice.llGold = llGold;
	subPacket.TPrice.dwCheque = dwCheque;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	if (!Send(sizeof(subPacket), &subPacket))
		return false;

	return true;
}

bool CPythonNetworkStream::SendItemMovePrivateShopPacket(WORD wPos, WORD wChangePos)
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_ITEM_MOVE;

	TPacketCGPrivateShopItemMove subPacket{};
	subPacket.wPos = wPos;
	subPacket.wChangePos = wChangePos;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	if (!Send(sizeof(subPacket), &subPacket))
		return false;

	return true;
}

bool CPythonNetworkStream::SendItemCheckinPrivateShopPacket(WORD wSrcPos, WORD wSrcWindow, long long llGold, DWORD dwCheque, int iDstPos /* = -1 */)
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_ITEM_CHECKIN;

	TPacketCGPrivateShopItemCheckin subPacket{};
	subPacket.TSrcPos.cell = wSrcPos;
	subPacket.TSrcPos.window_type = wSrcWindow;
	subPacket.TPrice.llGold = llGold;
	subPacket.TPrice.dwCheque = dwCheque;
	subPacket.iDstPos = iDstPos;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	if (!Send(sizeof(subPacket), &subPacket))
		return false;

	return true;
}

bool CPythonNetworkStream::SendItemCheckoutPrivateShopPacket(WORD wSrcPos, int iDstPos/* = -1 */)
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_ITEM_CHECKOUT;

	TPacketCGPrivateShopItemCheckout subPacket{};
	subPacket.wSrcPos = wSrcPos;
	subPacket.iDstPos = iDstPos;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	if (!Send(sizeof(subPacket), &subPacket))
		return false;

	return true;
}

bool CPythonNetworkStream::SendTitleChangePrivateShopPacket(const char* c_szTitle)
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_TITLE_CHANGE;

	char szTitle[TITLE_MAX_LEN + 1];
	strncpy(szTitle, c_szTitle, TITLE_MAX_LEN);

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	if (!Send(sizeof(szTitle), &szTitle))
		return false;

	return true;
}

bool CPythonNetworkStream::SendPrivateShopWarpRequest()
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_WARP_REQUEST;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	return true;
}

bool CPythonNetworkStream::SendClosePrivateShopSearchPacket()
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_SEARCH_CLOSE;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	return true;
}

bool CPythonNetworkStream::SendPrivateShopSearchPacket(TPacketCGPrivateShopSearch* pPacket)
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_SEARCH;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	if (!Send(sizeof(TPacketCGPrivateShopSearch), pPacket))
		return false;

	return true;
}

bool CPythonNetworkStream::SendPrivateShopSearchBuyPacket(int* pSelectedItem)
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_SEARCH_BUY;

	TPacketCGPrivateShopSearchBuy subPacket{};

	for (int i = 0; i < SELECTED_ITEM_MAX_NUM; ++i, ++pSelectedItem)
	{
		if (*pSelectedItem < 0)
			break;

		const TPrivateShopSearchData* c_pItemData;
		if (!CPythonPrivateShop::Instance().GetSearchItemData(*pSelectedItem, &c_pItemData))
			return true;

		subPacket.aSelectedItems[i].dwShopID = c_pItemData->dwShopID;
		subPacket.aSelectedItems[i].wPos = c_pItemData->wPos;
		subPacket.aSelectedItems[i].TPrice.llGold = c_pItemData->TPrice.llGold;
		subPacket.aSelectedItems[i].TPrice.dwCheque = c_pItemData->TPrice.dwCheque;
	}

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	if (!Send(sizeof(subPacket), &subPacket))
		return false;

	return true;
}

bool CPythonNetworkStream::SendPrivateShopMarketItemPriceDataReqPacket()
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_MARKET_ITEM_PRICE_DATA_REQUEST;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	return true;
}

bool CPythonNetworkStream::SendPrivateShopMarketItemPriceReqPacket(DWORD dwVnum)
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_MARKET_ITEM_PRICE_REQUEST;

	DWORD subPacket = dwVnum;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	if (!Send(sizeof(subPacket), &subPacket))
		return false;

	return true;
}

bool CPythonNetworkStream::SendPrivateShopSlotUnlockRequest(WORD wSrcPos)
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_SLOT_UNLOCK_REQUEST;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	if (!Send(sizeof(wSrcPos), &wSrcPos))
		return false;

	return true;
}

bool CPythonNetworkStream::RecvPrivateShop()
{
	TPacketGCPrivateShop mainPacket{};
	if (!Recv(sizeof(mainPacket), &mainPacket))
		return false;

	switch (mainPacket.bSubHeader)
	{
	case SUBHEADER_GC_PRIVATE_SHOP_ADD_ENTITY:
	{
		TPacketGCPrivateShopAddEntity subPacket{};
		if (!Recv(sizeof(TPacketGCPrivateShopAddEntity), &subPacket))
			return false;

		CPythonPrivateShop::TPrivateShopInstance* pPrivateShopInstance = CPythonPrivateShop::Instance().CreatePrivateShopInstance(
			subPacket.dwVID, subPacket.dwVnum,
			subPacket.szName, subPacket.lX, subPacket.lY, subPacket.lZ);

		if (!pPrivateShopInstance)
			return true;

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
			"AddPrivateShopTitleBoard",
			Py_BuildValue("(isi)", subPacket.dwVID, subPacket.szTitle, subPacket.bTitleType)
		);

	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_DEL_ENTITY:
	{
		TPacketGCPrivateShopDelEntity subPacket{};
		if (!Recv(sizeof(TPacketGCPrivateShopDelEntity), &subPacket))
			return false;

		CPythonPrivateShop::Instance().DeletePrivateShopInstance(subPacket.dwVID);

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
			"RemovePrivateShopTitleBoard",
			Py_BuildValue("(i)", subPacket.dwVID)
		);
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_TITLE:
	{
		TPacketGCPrivateShopTitle subPacket{};
		if (!Recv(sizeof(TPacketGCPrivateShopTitle), &subPacket))
			return false;

		CPythonPlayer& rkPlayer = CPythonPlayer::Instance();

		if (0 == strlen(subPacket.szTitle))
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
				"RemovePrivateShopTitleBoard",
				Py_BuildValue("(i)", subPacket.dwVID)
			);
		}
		else
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
				"AddPrivateShopTitleBoard",
				Py_BuildValue("(isi)", subPacket.dwVID, subPacket.szTitle, subPacket.bTitleType)
			);
		}
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_LOAD:
	{
		TPacketGCPrivateShopLoad subPacket{};
		if (!Recv(sizeof(TPacketGCPrivateShopLoad), &subPacket))
			return false;

		// Clear the stock if we were building a private shop
		CPythonPrivateShop::Instance().ClearPrivateShopStock();

		CPythonPrivateShop::Instance().SetGold(subPacket.llGold);
		CPythonPrivateShop::Instance().SetCheque(subPacket.dwCheque);
		CPythonPrivateShop::Instance().SetLocation(subPacket.lX, subPacket.lY, subPacket.bChannel);
		CPythonPrivateShop::Instance().SetMyTitle(subPacket.szTitle);
		CPythonPrivateShop::Instance().SetMyState(subPacket.bState);
		CPythonPrivateShop::Instance().SetMyPageCount(subPacket.bPageCount);
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_SET_ITEM:
	{
		TPrivateShopItemData subPacket{};
		if (!Recv(sizeof(TPrivateShopItemData), &subPacket))
			return false;

		CPythonPrivateShop::Instance().SetItemData(subPacket, true);
		__RefreshPrivateShopWindow();
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_SET_SALE:
	{
		TPrivateShopSaleData subPacket{};
		if (!Recv(sizeof(TPrivateShopSaleData), &subPacket))
			return false;

		CPythonPrivateShop::Instance().SetSaleItemData(subPacket);
		__RefreshPrivateShopWindow();
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_BALANCE_UPDATE:
	{
		TPacketGCPrivateShopBalanceUpdate subPacket{};
		if (!Recv(sizeof(TPacketGCPrivateShopBalanceUpdate), &subPacket))
			return false;

		CPythonPrivateShop::Instance().ChangeGold(subPacket.TPrice.llGold);
		CPythonPrivateShop::Instance().ChangeCheque(subPacket.TPrice.dwCheque);

		__RefreshPrivateShopWindow();
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_OPEN_PANEL:
	{
		CPythonPrivateShop::Instance().SetMainPlayerPrivateShop(true);
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenPrivateShopPanel", Py_BuildValue("()"));
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_CLOSE_PANEL:
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ClosePrivateShopPanel", Py_BuildValue("()"));
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_CLOSE:
	{
		CPythonPrivateShop::Instance().ClearMyPrivateShop();
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ClosePrivateShopPanel", Py_BuildValue("()"));
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_START:
	{
		TPacketGCPrivateShopOpen subPacket{};
		if (!Recv(sizeof(TPacketGCPrivateShopOpen), &subPacket))
			return false;

		CPythonPrivateShop::Instance().Clear();
		CPythonPrivateShop::Instance().SetTitle(subPacket.szTitle);
		CPythonPrivateShop::Instance().SetState(subPacket.bState);
		CPythonPrivateShop::Instance().SetPageCount(subPacket.bPageCount);
		CPythonPrivateShop::Instance().SetUnlockedSlots(subPacket.wUnlockedSlots);
		CPythonPrivateShop::Instance().SetMainPlayerPrivateShop(false);

		for (WORD i = 0; i < PRIVATE_SHOP_HOST_ITEM_MAX_NUM; ++i)
		{
			if (subPacket.aItems[i].dwVnum)
				CPythonPrivateShop::Instance().SetItemData(subPacket.aItems[i], false);
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenPrivateShopPanel", Py_BuildValue("()"));
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_END:
	{
		CPythonPrivateShop::Instance().Clear();
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ClosePrivateShopPanel", Py_BuildValue("()"));
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_REMOVE_ITEM:
	{
		WORD wPos;
		if (!Recv(sizeof(WORD), &wPos))
			return false;

		CPythonPrivateShop::Instance().RemoveItemData(wPos, false);
		__RefreshPrivateShopWindow();
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_REMOVE_MY_ITEM:
	{
		WORD wPos;
		if (!Recv(sizeof(WORD), &wPos))
			return false;

		CPythonPrivateShop::Instance().RemoveItemData(wPos, true);
		__RefreshPrivateShopWindow();
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_STATE_UPDATE:
	{
		TPacketGCPrivateStateUpdate subPacket{};
		if (!Recv(sizeof(TPacketGCPrivateStateUpdate), &subPacket))
			return false;

		if (subPacket.bIsMainPlayerPrivateShop)
		{
			CPythonPrivateShop::Instance().SetMyState(subPacket.bState);

			// Request market prices when owner is trying to edit the private shop
			if (subPacket.bState == STATE_MODIFY && !CPythonPrivateShop::Instance().IsMarketItemPriceDataLoaded())
				SendPrivateShopMarketItemPriceDataReqPacket();
		}
		else
			CPythonPrivateShop::Instance().SetState(subPacket.bState);

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PrivateShopStateUpdate", Py_BuildValue("()"));
		__RefreshPrivateShopWindow();
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_WITHDRAW:
	{
		CPythonPrivateShop::Instance().SetGold(0);
		CPythonPrivateShop::Instance().SetCheque(0);

		__RefreshPrivateShopWindow();
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_ITEM_PRICE_CHANGE:
	{
		TPacketGCPrivateShopItemPriceChange subPacket{};
		if (!Recv(sizeof(TPacketGCPrivateShopItemPriceChange), &subPacket))
			return false;

		CPythonPrivateShop::Instance().ChangeItemPrice(subPacket.wPos, subPacket.TPrice.llGold, subPacket.TPrice.dwCheque);
		__RefreshPrivateShopWindow();
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_ITEM_MOVE:
	{
		TPacketGCPrivateShopItemMove subPacket{};
		if (!Recv(sizeof(TPacketGCPrivateShopItemMove), &subPacket))
			return false;

		CPythonPrivateShop::Instance().MoveItem(subPacket.wPos, subPacket.wChangePos);
		__RefreshPrivateShopWindow();
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_ADD_ITEM:
	{
		TPrivateShopItemData subPacket{};
		if (!Recv(sizeof(TPrivateShopItemData), &subPacket))
			return false;

		CPythonPrivateShop::Instance().SetItemData(subPacket, false);
		__RefreshPrivateShopWindow();
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_TITLE_CHANGE:
	{
		char szTitle[TITLE_MAX_LEN + 1] = { 0 };
		if (!Recv(sizeof(szTitle), &szTitle))
			return false;

		CPythonPrivateShop::Instance().SetMyTitle(szTitle);
		__RefreshPrivateShopWindow();
	}break;

	case SUBHEADER_GC_PRIVATE_SHOP_SEARCH_OPEN_LOOK_MODE:
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenPrivateShopSearch", Py_BuildValue("(i)", MODE_LOOK));
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_SEARCH_OPEN_TRADE_MODE:
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenPrivateShopSearch", Py_BuildValue("(i)", MODE_TRADE));
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_SEARCH_RESULT:
	{

		WORD wSize = mainPacket.wSize - sizeof(mainPacket);
		if (!wSize)
			break;

		DWORD dwCurrentResult = CPythonPrivateShop::Instance().GetResultMaxCount();

		while (wSize)
		{
			TPrivateShopSearchData subPacket{};
			if (!Recv(sizeof(subPacket), &subPacket))
				return false;

			CPythonPrivateShop::Instance().SetSearchItemData(subPacket);

			wSize -= sizeof(TPrivateShopSearchData);
		}

		if (!CPythonPrivateShop::Instance().GetResultPage())
			CPythonPrivateShop::Instance().SetResultPage(1);


		CPythonPrivateShop::Instance().SortSearchResult();

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PrivateShopSearchRefresh", Py_BuildValue("()"));
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_SEARCH_UPDATE:
	{
		TPacketGCPrivateShopSearchUpdate subPacket;
		if (!Recv(sizeof(subPacket), &subPacket))
			return false;

		if (subPacket.iSpecificItemPos < 0)
		{
			std::vector<WORD> vec_itemPositions;
			CPythonPrivateShop::Instance().GetSearchItemDataPos(subPacket.dwShopID, vec_itemPositions);
			CPythonPrivateShop::Instance().SetSearchItemDataState(subPacket.dwShopID, subPacket.bState);

			for (const auto& wPos : vec_itemPositions)
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PrivateShopSearchUpdate", Py_BuildValue("(ii)", wPos, subPacket.bState));
			}
		}
		else
		{
			int iPos = CPythonPrivateShop::Instance().GetSearchItemDataPos(subPacket.dwShopID, subPacket.iSpecificItemPos);

			if (iPos != -1)
			{
				CPythonPrivateShop::Instance().SetSearchItemDataState(subPacket.dwShopID, subPacket.bState, subPacket.iSpecificItemPos);
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PrivateShopSearchUpdate", Py_BuildValue("(ii)", iPos, subPacket.bState));
			}
		}
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_MARKET_ITEM_PRICE_DATA_RESULT:
	{
		WORD wSize = mainPacket.wSize - sizeof(mainPacket);
		while (wSize)
		{
			TMarketItemPrice subPacket{};
			if (!Recv(sizeof(subPacket), &subPacket))
				return false;

			CPythonPrivateShop::Instance().SetMarketItemPrice(subPacket);

			wSize -= sizeof(TMarketItemPrice);
		}

		CPythonPrivateShop::Instance().SetMarketItemPriceDataLoaded(true);
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_MARKET_ITEM_PRICE_RESULT:
	{
		TMarketItemPrice subPacket{};
		if (!Recv(sizeof(subPacket), &subPacket))
			return false;

		CPythonPrivateShop::Instance().SetMarketItemPrice(subPacket);
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AppendMarketItemPrice", Py_BuildValue("(Li)", subPacket.TPrice.llGold, subPacket.TPrice.dwCheque));
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_UNLOCKED_SLOTS_CHANGE:
	{
		WORD wUnlockedSlots;
		if (!Recv(sizeof(wUnlockedSlots), &wUnlockedSlots))
			return false;

		CPythonPrivateShop::Instance().SetUnlockedSlots(wUnlockedSlots);
	} break;
	}

	return true;
}
#endif

#ifdef __ENABLE_POLYMORPH_SYSTEM__
bool CPythonNetworkStream::RecvPolySystemPacket()
{
	TPacketGCPolySystem mainPack;
	if (!Recv(sizeof(mainPack), &mainPack))
		return false;

	//TraceError("RecvPolySystemPacket -> Start");
	switch (mainPack.bySubheader)
	{
	case SUBHEADER_POLY_DATA:
	{
		BYTE bySkin = 0;
		if (!Recv(sizeof(bySkin), &bySkin))
			return false;

		CPythonPlayer::instance().SetPolySkin(bySkin);

		WORD wStage = 0;
		if (!Recv(sizeof(wStage), &wStage))
			return false;

		PyObject* poStages = PyList_New(wStage);
		for (WORD i = 0; i < wStage; i++)
		{
			TPolymorphStage stage = {};
			if (!Recv(sizeof(stage), &stage))
				return false;

			PyObject* poStageBonus = PyList_New(MAX_POLY_BONUS);
			for (BYTE j = 0; j < MAX_POLY_BONUS; j++)
				PyList_SetItem(poStageBonus, j, Py_BuildValue("ii", stage.tApplies[j].bType, stage.tApplies[j].lValue));

			PyObject* poItems = PyList_New(MAX_POLY_REQUIRED_ITEMS);
			for (BYTE j = 0; j < MAX_POLY_REQUIRED_ITEMS; j++)
				PyList_SetItem(poItems, j, Py_BuildValue("ii", stage.tRequired[j].dwVnum, stage.tRequired[j].wCount));

			PyList_SetItem(poStages, i, Py_BuildValue("iiiOO", stage.byStage, stage.dwMonster, stage.dwVnum, poStageBonus, poItems));
		}

		WORD wSkins = 0;
		if (!Recv(sizeof(wSkins), &wSkins))
			return false;

		PyObject* poSkins = PyList_New(wSkins);
		for (WORD i = 0; i < wSkins; i++)
		{
			TPolymorphSkin skin = {};
			if (!Recv(sizeof(skin), &skin))
				return false;

			BYTE unlocked = {};
			if (!Recv(sizeof(unlocked), &unlocked))
				return false;

			CPythonPlayer::instance().UnlockPolySkin(skin.byIndex, unlocked);
			PyList_SetItem(poSkins, i, Py_BuildValue("iii", skin.byIndex, skin.dwSkinVnum, skin.dwSkinUnlock));
		}

		if (m_apoPhaseWnd[PHASE_WINDOW_GAME])
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_PolySystem", Py_BuildValue("(OO)", poStages, poSkins));
	}
	break;

	case SUBHEADER_POLY_OPEN:
	{
		if (m_apoPhaseWnd[PHASE_WINDOW_GAME])
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_PolySkinOpen", Py_BuildValue("()"));
	}
	break;
	case SUBHEADER_POLY_SKIN_UNLOCK:
	{
		BYTE bySkin = 0;
		if (!Recv(sizeof(bySkin), &bySkin))
			return false;

		CPythonPlayer::instance().UnlockPolySkin(bySkin, true);
		if (m_apoPhaseWnd[PHASE_WINDOW_GAME])
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_PolySkinRefresh", Py_BuildValue("()"));
	}
	break;

	case SUBHEADER_POLY_CLOSE:
	{
		if (m_apoPhaseWnd[PHASE_WINDOW_GAME])
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_PolySkinForceClose", Py_BuildValue("()"));
	}
	break;

	case SUBHEADER_POLY_SKIN_CHANGE:
	{
		BYTE bySkin = 0;
		if (!Recv(sizeof(bySkin), &bySkin))
			return false;

		CPythonPlayer::instance().SetPolySkin(bySkin);
		if (m_apoPhaseWnd[PHASE_WINDOW_GAME])
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_PolySkinRefresh", Py_BuildValue("()"));
	}
	break;
	}

	return true;
}

bool CPythonNetworkStream::SendPolySystemOpen()
{
	TPacketCGPolySystem sendPack
	{
		HEADER_CG_POLY_SYSTEM, SUBHEADER_POLY_OPEN
	};

	if (!Send(sizeof(sendPack), &sendPack))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendPolySystemClose()
{
	TPacketCGPolySystem sendPack
	{
		HEADER_CG_POLY_SYSTEM, SUBHEADER_POLY_CLOSE
	};

	if (!Send(sizeof(sendPack), &sendPack))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendPolySystemSkin(BYTE byIndex)
{
	TPacketCGPolySystem sendPack
	{
		HEADER_CG_POLY_SYSTEM, SUBHEADER_POLY_SKIN_CHANGE
	};

	if (!Send(sizeof(sendPack), &sendPack))
		return false;

	if (!Send(sizeof(byIndex), &byIndex))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendPolySystemBuy(BYTE byIndex)
{
	TPacketCGPolySystem sendPack
	{
		HEADER_CG_POLY_SYSTEM, SUBHEADER_POLY_BUY
	};

	if (!Send(sizeof(sendPack), &sendPack))
		return false;

	if (!Send(sizeof(byIndex), &byIndex))
		return false;

	return SendSequence();
}

#endif

#ifdef __INVENTORY_BUFFERING__
bool CPythonNetworkStream::RecvBufferedInventoryPacket()
{
	TPacketGCInventoryHeader packHdr;
	if (!Recv(sizeof(TPacketGCInventoryHeader), &packHdr))
	{
		return false;
	}

	packHdr.wSize -= sizeof(packHdr);

	// Keep on recv
	while (packHdr.wSize)
	{
		TPacketGCItemSet2 packItem;
		if (!Recv(sizeof(TPacketGCItemSet2), &packItem))
			return false;

		TItemData kItemData;
		kItemData.vnum = packItem.vnum;
		kItemData.count = packItem.count;

		kItemData.flags = 0;
		for (int i = 0; i < ITEM_SOCKET_SLOT_MAX_NUM; ++i)
		{
			kItemData.alSockets[i] = packItem.alSockets[i];
		}
		for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
		{
			kItemData.aAttr[j] = packItem.aAttr[j];
		}


		IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();

		rkPlayer.SetItemData(packItem.Cell, kItemData);
		packHdr.wSize -= sizeof(packItem);
	}

	// Now refresh
	__RefreshInventoryWindow();
	return true;
}
#endif


#ifdef ENABLE_SAVE_LOCATION_SYSTEM
bool CPythonNetworkStream::SendSaveLocationPacket(BYTE subheader, BYTE bPos, const char* c_szName)
{
	if (!__CanActMainInstance())
		return true;

	TPacketSaveLocation p;
	p.header = HEADER_CG_SAVE_LOCATION;
	p.subheader = subheader;
	p.pos = bPos;
	p.location[0].bPos = 0;
	p.location[0].iMapX = 0;
	p.location[0].iMapY = 0;
	strncpy(p.location[0].szName, c_szName, sizeof(p.location[0].szName));

	if (!Send(sizeof(p), &p))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvSaveLocationPacket()
{
	TPacketSaveLocation p;
	if (!Recv(sizeof(p), &p))
		return false;

	switch (p.subheader)
	{
		case SAVELOCATION_SUBHEADER_GC_ADDED:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_UpdateSaveLocation", Py_BuildValue("(isii)", p.location[0].bPos, p.location[0].szName, p.location[0].iMapX, p.location[0].iMapY));
			break;

		case SAVELOCATION_SUBHEADER_GC_DELED:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DelSaveLocation", Py_BuildValue("(i)", p.pos));
			break;

		case SAVELOCATION_SUBHEADER_GC_UPDATE:
			for (BYTE i = 0; i < p.pos; ++i)
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_UpdateSaveLocation", Py_BuildValue("(isii)", p.location[i].bPos, p.location[i].szName, p.location[i].iMapX, p.location[i].iMapY));
			break;

		default:
			TraceError("CPythonNetworkStream::RecvSaveLocationPacket: unknown subheader %d\n.", p.subheader);
			break;
	}

	return true;
}
#endif

#ifdef RANKING_SYSTEM
bool CPythonNetworkStream::SendLoadServerRanking(uint8_t catIdx)
{
	TPacketCGLoadServerRanking p;
	p.header = HEADER_CG_LOAD_SERVER_RANKING;
	p.catIdx = catIdx;

	if (!Send(sizeof(p), &p))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvLoadServerRanking()
{
	TPacketGCServerRankingLoad pServerRankData;
	if (!Recv(sizeof(pServerRankData), &pServerRankData))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ServerRankLoadInfo", Py_BuildValue("(s)", "clear"));
	TServerRankTable oneData = {};
	pServerRankData.size -= sizeof(TPacketGCServerRankingLoad);
	while (pServerRankData.size > 0)
	{
		if (!Recv(sizeof(TServerRankTable), &oneData))
			return false;

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ServerRankLoadData", Py_BuildValue("(sBBBI)", oneData.szName,
			oneData.level, oneData.raceWithskillGroup, oneData.empire, oneData.value));

		pServerRankData.size -= sizeof(TServerRankTable);
	}
	return true;
}

bool CPythonNetworkStream::SendLoadRanking(uint8_t catIdx)
{
	TPacketCGLoadRanking p;
	p.header = HEADER_CG_LOAD_RANKING;
	p.catIdx = catIdx;

	if (!Send(sizeof(p), &p))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvLoadRanking()
{
	TPacketGCRankingLoad pServerRankData;
	if (!Recv(sizeof(pServerRankData), &pServerRankData))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RankLoadInfo", Py_BuildValue("(sI)", "clear", pServerRankData.sundayEndTime));
	TRankTable oneData = {};
	pServerRankData.size -= sizeof(TPacketGCRankingLoad);
	while (pServerRankData.size > 0)
	{
		if (!Recv(sizeof(TRankTable), &oneData))
			return false;

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RankLoadData", Py_BuildValue("(sBBI)", oneData.szName,
			oneData.level, oneData.race, oneData.value));

		pServerRankData.size -= sizeof(TRankTable);
	}
	return true;
}
#endif

#ifdef ENABLE_SECONDARY_LEVEL
#include "PythonSecondaryLevelManager.h"
bool CPythonNetworkStream::SendSecondaryLevelPacket(ESecondaryLevelSubheader SubHeader, BYTE bOption)
{
	if (!__CanActMainInstance())
		return false;

	TPacketCGSecondaryLevel p;
	p.bSubHeader = SubHeader;

	if (!Send(sizeof(TPacketCGSecondaryLevel), &p))
		return false;

	if (SubHeader == SUBHEADER_SECONDARY_LEVEL_CG_DO_UPGRADE)
	{
		TPacketCGSecondaryLevelUpgrade pack;
		pack.bOption = bOption;

		if (!Send(sizeof(TPacketCGSecondaryLevelUpgrade), &pack))
			return false;
	}
	return SendSequence();
}

bool CPythonNetworkStream::RecvSecondaryLevelPacket()
{
	TPacketGCSecondaryLevel p;
	if (!(Recv(sizeof(TPacketGCSecondaryLevel), &p)))
		return false;

	size_t size = p.size -= sizeof(TPacketGCSecondaryLevel);
	switch (p.subheader)
	{
	case SUBHEADER_SECONDARY_LEVEL_GC_SEND_INFO:
	{
		if (size)
		{
			WORD sizeLevel;
			if (!Recv(sizeof(WORD), &sizeLevel))
				return false;

			size -= sizeof(WORD);

			for (int i = 0; i < sizeLevel; ++i)
			{
				TPacketGCSecondaryLevelInfo item;
				if (Recv(sizeof(TPacketGCSecondaryLevelInfo), &item))
				{
					CSecondaryLevelManager::Instance().AppendSecondaryLevelInformation(&item);
					size -= sizeof(TPacketGCSecondaryLevelInfo);
				}
			}
		}
	} break;
	case SUBHEADER_SECONDARY_LEVEL_GC_MAX:
	default:
		TraceError("CPythonNetworkStream::RecvSecondaryLevelPacket() ~ Unhandled packet header: %d", p.subheader);
		return false;
	}

	return true;
}
#endif

#ifdef __ENABLE_MINING_WITH_SPACE__
bool CPythonNetworkStream::SendMining(BYTE bySubheader)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGMining pack{ HEADER_CG_MINING, bySubheader };
	if (!Send(sizeof(pack), &pack))
		return false;

	return SendSequence();
}
#endif

#ifdef ENABLE_MOUNT_SYSTEM
bool CPythonNetworkStream::RecvMountInfoPacket()
{
	TPacketGCMountInfo pack;
	if (!Recv(sizeof(pack), &pack))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetMountInfo", Py_BuildValue("(isiiib)", pack.dwVnum, pack.szName, pack.wLevel, pack.iExp, pack.iRequiredExp, pack.bSummoned));
	return true;
}
#endif

#ifdef ENABLE_PVP_RANKING
bool CPythonNetworkStream::RecvPvPRankingPacket()
{
	TPacketGCPvPRanking rPacket;
	if (!Recv(sizeof(rPacket), &rPacket)) { return false; }

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PVP_ClearPlayerInfo", Py_BuildValue("()"));
	
	if (rPacket.bStatus[0] == (0)) {
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PVP_CloseWindow", Py_BuildValue("()"));
		goto End;
	}
	
	bool isRefresh = false;
	for (BYTE i = 0; i < 10; i++) {
		if (rPacket.bLevels[i] == 0) { continue; }
		
#ifdef ENABLE_PVP_RANKING_WITH_EMPIRE
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PVP_AddPlayer", Py_BuildValue("(siiiiiiiLLL)", rPacket.szNames[i], rPacket.bLevels[i], rPacket.bJobs[i], rPacket.bEmpire[i], rPacket.iKill[i], rPacket.iDead[i], rPacket.iMaxKill, rPacket.bIsRankEmpire, rPacket.llEmpire1, rPacket.llEmpire2, rPacket.llEmpire3));
#else
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PVP_AddPlayer", Py_BuildValue("(siiiiii)", rPacket.szNames[i], rPacket.bLevels[i], rPacket.bJobs[i], rPacket.bEmpire[i], rPacket.iKill[i], rPacket.iDead[i], rPacket.iMaxKill));
#endif
		isRefresh = true;
	}
	if (isRefresh)
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PVP_Refresh", Py_BuildValue("()"));
End:;
	return true;
}
#endif



#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
bool CPythonNetworkStream::RecvTitleAchievementPacket()
{
	TPacketGCTitleAchievementInfo p;
	if (!Recv(sizeof(p), &p))
	{
		Tracenf("Recv TPacketGCTitleAchievementInfo Packet Error");
		return false;
	}

	// Clear previous data
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_ClearTitleAchievementList", Py_BuildValue("()"));

	// Send unlock status for each title
	for (uint8_t i = 0; i < 24; ++i)
	{
		uint8_t titleId = i + 1;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetTitleUnlockStatus",
			Py_BuildValue("(bb)", titleId, p.bTitleUnlocked[i]));
	}

	// Send current active titles
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetCurrentActiveTitle",
		Py_BuildValue("(b)", p.bCurrentTitle));

	// NEW: Send premium title
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetCurrentActiveTitlePremium",
		Py_BuildValue("(b)", p.bCurrentTitlePremium));

	// Send effects for primary title
	for (uint8_t i = 0; i < 3; ++i)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetTitleEffect",
			Py_BuildValue("(bHb)", i, p.wAffType[i], p.bAffValue[i]));
	}

	// NEW: Send effects for premium title
	for (uint8_t i = 0; i < 3; ++i)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetTitleEffectPremium",
			Py_BuildValue("(bHb)", i, p.wAffTypePremium[i], p.bAffValuePremium[i]));
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_FinalizeTitleLoading", Py_BuildValue("()"));

	return true;
}
#endif

#ifdef _ENABLE_BATTLEPASS_
bool CPythonNetworkStream::RecvBattlePass()
{
	TPacketBattlePass packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	switch (packet.bSubHeader)
	{
	case BATTLEPASS_GC_SUB_INIT:
	case BATTLEPASS_GC_SUB_UPDATE:
	{
		CPythonBattlePass::instance().ClearPlayerBattlePass();
		packet.wSize -= sizeof(packet);

		while (packet.wSize > 0)
		{
			TPlayerBattlePass info;
			if (!Recv(sizeof(info), &info))
				return false;
			CPythonBattlePass::instance().UpdateBattlePass(info);
			packet.wSize -= sizeof(info);
		}

		// Update normal battle pass info
		CPythonBattlePass::Instance().SetPoints(packet.points);
		CPythonBattlePass::Instance().SetPremium(packet.isPremium);

		// Update event battle pass info
		CPythonBattlePass::Instance().SetEventPoints(packet.eventPoints);
		CPythonBattlePass::Instance().SetEventPremium(packet.eventIsPremium);
		CPythonBattlePass::Instance().SetEventActive(packet.eventActive);

		if (packet.bSubHeader == BATTLEPASS_GC_SUB_INIT)
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_BattlePassInit", Py_BuildValue("()"));
		else
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_BattlePassUpdate", Py_BuildValue("()"));
	} break;
	default: break;
	}

	return true;
}
#endif

#ifdef ENABLE_NOTIFICATION_SYSTEM
bool CPythonNetworkStream::RecvNotification()
{
	TPacketGCNotification packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SendNotification", Py_BuildValue("(iii)", packet.byCategory, packet.dwValue, packet.dwAdditionalValue));

	return true;
}
#endif

#ifdef ENABLE_ITEMSHOP
bool CPythonNetworkStream::RecvItemShop()
{
	TPacketGCItemShop p;
	if (!Recv(sizeof(p), &p))
	{
#ifdef _DEBUG
		Tracenf("RecvItemShop -> cannot recv: TPacketGCItemShop.");
#endif
		return false;
	}

	BYTE subHeader;
	if (!Recv(sizeof(subHeader), &subHeader))
	{
#ifdef _DEBUG
		Tracenf("RecvItemShop -> cannot recv: subHeader.");
#endif
		return false;
	}

	switch (subHeader)
	{
		case ITEMSHOP_DRAGONCOIN:
		{
			long long act_lldCoins;
			if (!Recv(sizeof(act_lldCoins), &act_lldCoins))
			{
#ifdef _DEBUG
				Tracenf("RecvItemShop -> cannot recv: act_lldCoins (subHeader: %u).", subHeader);
#endif
				return false;
			}

#ifdef USE_ITEMSHOP_RENEWED
			long long act_lldJCoins;
			if (!Recv(sizeof(act_lldJCoins), &act_lldJCoins))
			{
#ifdef _DEBUG
				Tracenf("RecvItemShop -> cannot recv: act_lldJCoins (subHeader: %u).", subHeader);
#endif
				return false;
			}
#endif

			bool isLogOpen;
			if (!Recv(sizeof(isLogOpen), &isLogOpen))
			{
#ifdef _DEBUG
				Tracenf("RecvItemShop -> cannot recv: isLogOpen (subHeader: %u).", subHeader);
#endif
				return false;
			}

			if (isLogOpen)
			{
				TIShopLogData logData;
				if (!Recv(sizeof(logData), &logData))
				{
#ifdef _DEBUG
					Tracenf("RecvItemShop -> cannot recv: logData (subHeader: %u).", subHeader);
#endif
					return false;
				}

#ifdef USE_ITEMSHOP_RENEWED
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopAppendLog",
										Py_BuildValue("(sissiiLL)",
										logData.buyDate,
										logData.buyTime,
										logData.playerName,
										logData.ipAdress,
										logData.itemVnum,
										logData.itemCount,
										logData.itemPrice,
										logData.itemPriceJD));
#else
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopAppendLog",
										Py_BuildValue("(sissiiL)",
										logData.buyDate,
										logData.buyTime,
										logData.playerName,
										logData.ipAdress,
										logData.itemVnum,
										logData.itemCount,
										logData.itemPrice));
#endif
			}

#ifdef USE_ITEMSHOP_RENEWED
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopSetDragonCoin",
									Py_BuildValue("(LL)",
									act_lldCoins,
									act_lldJCoins));
#else
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopSetDragonCoin",
									Py_BuildValue("(L)",
									act_lldCoins));
#endif

			return true;
		}
		case ITEMSHOP_LOG:
		{
			int logCount;
			if (!Recv(sizeof(int), &logCount))
			{
#ifdef _DEBUG
				Tracenf("RecvItemShop -> cannot recv: logCount (subHeader: %u).", subHeader);
#endif
				return false;
			}

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopHideLoading",
									Py_BuildValue("()"));

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopLogClear",
									Py_BuildValue("()"));

			if (logCount > 0)
			{
				std::vector<TIShopLogData> m_vec;
				m_vec.resize(logCount);

				if (!Recv(sizeof(TIShopLogData) * logCount, &m_vec[0]))
				{
#ifdef _DEBUG
					Tracenf("RecvItemShop -> cannot recv: m_vec (subHeader: %u).", subHeader);
#endif
					return false;
				}

				for (auto j = 0; j < m_vec.size(); ++j)
				{
#ifdef USE_ITEMSHOP_RENEWED
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopAppendLog",
											Py_BuildValue("(sissiiLL)",
											m_vec[j].buyDate,
											m_vec[j].buyTime,
											m_vec[j].playerName,
											m_vec[j].ipAdress,
											m_vec[j].itemVnum,
											m_vec[j].itemCount,
											m_vec[j].itemPrice,
											m_vec[j].itemPriceJD));
#else
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopAppendLog",
											Py_BuildValue("(sissiiL)",
											m_vec[j].buyDate,
											m_vec[j].buyTime,
											m_vec[j].playerName,
											m_vec[j].ipAdress,
											m_vec[j].itemVnum,
											m_vec[j].itemCount,
											m_vec[j].itemPrice));
#endif
				}
			}

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopPurchasesWindow",
									Py_BuildValue("()"));

			return true;
		}
		case ITEMSHOP_UPDATE_ITEM:
		{
			TIShopData updateItem;
			if (!Recv(sizeof(updateItem), &updateItem))
			{
#ifdef _DEBUG
				Tracenf("RecvItemShop -> cannot recv: updateItem (subHeader: %u).", subHeader);
#endif
				return false;
			}

#ifdef USE_ITEMSHOP_RENEWED
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopUpdateItem",
									Py_BuildValue("(iiLiiiiLiL)",
									updateItem.id,
									updateItem.itemVnum,
									updateItem.itemPrice,
									updateItem.discount,
									updateItem.offerTime,
									updateItem.topSellingIndex,
									updateItem.addedTime,
									updateItem.sellCount,
									updateItem.maxSellCount,
									updateItem.itemPriceJD));
#else
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopUpdateItem",
									Py_BuildValue("(iiLiiiiLi)",
									updateItem.id,
									updateItem.itemVnum,
									updateItem.itemPrice,
									updateItem.discount,
									updateItem.offerTime,
									updateItem.topSellingIndex,
									updateItem.addedTime,
									updateItem.sellCount,
									updateItem.maxSellCount));
#endif

			return true;
		}
		case ITEMSHOP_LOAD:
		{
			long long act_lldCoins;
			if (!Recv(sizeof(act_lldCoins), &act_lldCoins))
			{
#ifdef _DEBUG
				Tracenf("RecvItemShop -> cannot recv: act_lldCoins (subHeader: %u).", subHeader);
#endif
				return false;
			}

#ifdef USE_ITEMSHOP_RENEWED
			long long act_lldJCoins;
			if (!Recv(sizeof(act_lldJCoins), &act_lldJCoins))
			{
#ifdef _DEBUG
				Tracenf("RecvItemShop -> cannot recv: act_lldJCoins (subHeader: %u).", subHeader);
#endif
				return false;
			}
#endif

			int updateTime;
			if (!Recv(sizeof(updateTime), &updateTime))
			{
#ifdef _DEBUG
				Tracenf("RecvItemShop -> cannot recv: updateTime (subHeader: %u).", subHeader);
#endif
				return false;
			}

#ifdef USE_ITEMSHOP_RENEWED
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopSetDragonCoin",
									Py_BuildValue("(LL)",
									act_lldCoins,
									act_lldJCoins));
#else
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopSetDragonCoin",
									Py_BuildValue("(L)",
									act_lldCoins));
#endif

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopHideLoading",
									Py_BuildValue("()"));

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopClear",
									Py_BuildValue("(i)", updateTime));

			int categoryTotalSize;
			if (!Recv(sizeof(categoryTotalSize), &categoryTotalSize))
			{
#ifdef _DEBUG
				Tracenf("RecvItemShop -> cannot recv: categoryTotalSize (subHeader: %u).", subHeader);
#endif
				return false;
			}

			if (categoryTotalSize == 9999)
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopOpenMainPage",
										Py_BuildValue("()"));

				return true;
			}


			for (auto j = 0; j < categoryTotalSize; ++j)
			{
				BYTE categoryIndex;
				if (!Recv(sizeof(categoryIndex), &categoryIndex))
				{
#ifdef _DEBUG
					Tracenf("RecvItemShop -> cannot recv: categoryIndex (subHeader: %u).", subHeader);
#endif
					return false;
				}

				BYTE categorySize;
				if (!Recv(sizeof(categorySize), &categorySize))
				{
#ifdef _DEBUG
					Tracenf("RecvItemShop -> cannot recv: categorySize (subHeader: %u).", subHeader);
#endif
					return false;
				}

				for (auto x = 0; x < categorySize; ++x)
				{
					BYTE categorySubHeader;
					if (!Recv(sizeof(categorySubHeader), &categorySubHeader))
					{
#ifdef _DEBUG
						Tracenf("RecvItemShop -> cannot recv: categorySubHeader (subHeader: %u).", subHeader);
#endif
						return false;
					}

					BYTE categorySubSize;
					if (!Recv(sizeof(categorySubSize), &categorySubSize))
					{
#ifdef _DEBUG
						Tracenf("RecvItemShop -> cannot recv: categorySubSize (subHeader: %u).", subHeader);
#endif
						return false;
					}

					if (categorySubSize > 0)
					{
						for (auto k = 0; k < categorySubSize; ++k)
						{
							TIShopData shopData;
							if (!Recv(sizeof(shopData), &shopData))
							{
#ifdef _DEBUG
								Tracenf("RecvItemShop -> cannot recv: shopData (subHeader: %u).", subHeader);
#endif
								return false;
							}

#ifdef USE_ITEMSHOP_RENEWED
							PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopAppendItem",
													Py_BuildValue("(iiiiLiiiiLiL)",
													categoryIndex,
													categorySubHeader,
													shopData.id,
													shopData.itemVnum,
													shopData.itemPrice,
													shopData.discount,
													shopData.offerTime,
													shopData.topSellingIndex,
													shopData.addedTime,
													shopData.sellCount,
													shopData.maxSellCount,
													shopData.itemPriceJD));
#else
							PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopAppendItem",
													Py_BuildValue("(iiiiLiiiiLi)",
													categoryIndex,
													categorySubHeader,
													shopData.id,
													shopData.itemVnum,
													shopData.itemPrice,
													shopData.discount,
													shopData.offerTime,
													shopData.topSellingIndex,
													shopData.addedTime,
													shopData.sellCount,
													shopData.maxSellCount));
#endif
						}
					}
				}

				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopOpenMainPage",
										Py_BuildValue("()"));
			}

			return true;
		}
		default:
		{
#ifdef _DEBUG
			TraceError("RecvItemShop -> invalid subHeader: %u.", subHeader);
#endif
			return false;
		}
	}
}
#endif