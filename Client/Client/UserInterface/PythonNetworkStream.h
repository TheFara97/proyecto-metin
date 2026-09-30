#pragma once

#include "../eterLib/FuncObject.h"
#include "../eterlib/NetStream.h"
#include "../eterLib/NetPacketHeaderMap.h"

#include "InsultChecker.h"

#include "packet.h"
#ifdef ENABLE_SWITCHBOT
#include "PythonSwitchbot.h"
#endif
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
#include "PythonPrivateShop.h"
#endif

class CInstanceBase;
class CNetworkActorManager;
struct SNetworkActorData;
struct SNetworkUpdateActorData;

class CPythonNetworkStream : public CNetworkStream, public CSingleton<CPythonNetworkStream>
{
	public:
		enum
		{
			SERVER_COMMAND_LOG_OUT = 0,
			SERVER_COMMAND_RETURN_TO_SELECT_CHARACTER = 1,
			SERVER_COMMAND_QUIT = 2,

			MAX_ACCOUNT_PLAYER
		};

		enum
		{
			ERROR_NONE,
			ERROR_UNKNOWN,
			ERROR_CONNECT_MARK_SERVER,
			ERROR_LOAD_MARK,
			ERROR_MARK_WIDTH,
			ERROR_MARK_HEIGHT,

			// MARK_BUG_FIX
			ERROR_MARK_UPLOAD_NEED_RECONNECT,
			ERROR_MARK_CHECK_NEED_RECONNECT,
			// END_OF_MARK_BUG_FIX
		};

		enum
		{
			ACCOUNT_CHARACTER_SLOT_ID,
			ACCOUNT_CHARACTER_SLOT_NAME,
			ACCOUNT_CHARACTER_SLOT_RACE,
			ACCOUNT_CHARACTER_SLOT_LEVEL,
			ACCOUNT_CHARACTER_SLOT_STR,
			ACCOUNT_CHARACTER_SLOT_DEX,
			ACCOUNT_CHARACTER_SLOT_HTH,
			ACCOUNT_CHARACTER_SLOT_INT,
			ACCOUNT_CHARACTER_SLOT_PLAYTIME,
			ACCOUNT_CHARACTER_SLOT_FORM,
			ACCOUNT_CHARACTER_SLOT_ADDR,
			ACCOUNT_CHARACTER_SLOT_PORT,
			ACCOUNT_CHARACTER_SLOT_GUILD_ID,
			ACCOUNT_CHARACTER_SLOT_GUILD_NAME,
			ACCOUNT_CHARACTER_SLOT_CHANGE_NAME_FLAG,
			ACCOUNT_CHARACTER_SLOT_HAIR,
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			ACCOUNT_CHARACTER_SLOT_ACCE,
#endif
#ifdef ENABLE_COSTUME_EMBLEMAT
			ACCOUNT_CHARACTER_SLOT_EMBLEMAT,
#endif
		};

		enum GamePhases: BYTE
		{
			GAME_PHASE_OFFLINE,
			GAME_PHASE_LOGIN,
			GAME_PHASE_HANDSHAKE,
			GAME_PHASE_SELECT,
			GAME_PHASE_EMPIRE_SELECT,
			GAME_PHASE_CREATE,
			GAME_PHASE_LOADING,
			GAME_PHASE_GAME,
			GAME_MAX_PHASE_NUM
		};

		enum
		{
			PHASE_WINDOW_LOGO,
			PHASE_WINDOW_LOGIN,
			PHASE_WINDOW_SELECT,
			PHASE_WINDOW_CREATE,
			PHASE_WINDOW_LOAD,
			PHASE_WINDOW_GAME,
			PHASE_WINDOW_EMPIRE,
			PHASE_WINDOW_NUM,
		};
		
		bool IsLoadingPhase() { return (m_strPhase == "Loading"); }
		bool IsGamePhase() { return (m_strPhase == "Game"); }

	public:
		void CallGamePyFunction(const char* function) const;
		void CallGamePyFunction(const char* function, PyObject* args) const;

	public:
		CPythonNetworkStream();
		virtual ~CPythonNetworkStream();

		bool SendSpecial(int nLen, void * pvBuf);

		void StartGame();
		void Warp(LONG lGlobalX, LONG lGlobalY);

		void NotifyHack(const char* c_szMsg);
		void SetWaitFlag();

	#ifdef ENABLE_CHEQUE_SYSTEM
		// Won Exchange
		bool SendWonExchangeSellPacket(WORD wValue);
		bool SendWonExchangeBuyPacket(WORD wValue);
	#endif

	#ifdef ENABLE_DISCORD_RPC
		void Discord_Start();
		void Discord_Close();
		void Discord_Update(const bool ingame);
	#endif

		void SendEmoticon(UINT eEmoticon);

		void ExitApplication();
		void ExitGame();
		void LogOutGame();
		void AbsoluteExitGame();
		void AbsoluteExitApplication();

		void EnableChatInsultFilter(bool isEnable);
		bool IsChatInsultIn(const char* c_szMsg);
		bool IsInsultIn(const char* c_szMsg);

		DWORD GetGuildID();

		UINT UploadMark(const char* c_szImageFileName);
		UINT UploadSymbol(const char* c_szImageFileName);

		bool LoadInsultList(const char* c_szInsultListFileName);
		bool LoadConvertTable(DWORD dwEmpireID, const char* c_szFileName);

		UINT		GetAccountCharacterSlotDatau(UINT iSlot, UINT eType);
		const char* GetAccountCharacterSlotDataz(UINT iSlot, UINT eType);

		// SUPPORT_BGM
		const char*		GetFieldMusicFileName();
		float			GetFieldMusicVolume();
		// END_OF_SUPPORT_BGM

		bool IsSelectedEmpire();

		void ToggleGameDebugInfo();

		void SetMarkServer(const char* c_szAddr, UINT uPort);
		void ConnectLoginServer(const char* c_szAddr, UINT uPort);
		void ConnectGameServer(UINT iChrSlot);

		void SetLoginInfo(const char* c_szID, const char* c_szPassword, const char* c_szPin);
		void SetLoginKey(DWORD dwLoginKey);
		void ClearLoginInfo( void );

		void SetHandler(PyObject* poHandler);
		void SetPhaseWindow(UINT ePhaseWnd, PyObject* poPhaseWnd);
		void ClearPhaseWindow(UINT ePhaseWnd, PyObject* poPhaseWnd);
		void SetServerCommandParserWindow(PyObject* poPhaseWnd);

		bool SendSyncPositionElementPacket(DWORD dwVictimVID, DWORD dwVictimX, DWORD dwVictimY);

		bool SendAttackPacket(UINT uMotAttack, DWORD dwVIDVictim);
		bool SendCharacterStatePacket(const TPixelPosition& c_rkPPosDst, float fDstRot, UINT eFunc, UINT uArg);
		bool SendUseSkillPacket(DWORD dwSkillIndex, DWORD dwTargetVID=0);
		bool SendTargetPacket(DWORD dwVID);
#ifdef _ENABLE_BATTLEPASS_
		bool RecvBattlePass();
#endif

		// OLDCODE:
		bool SendCharacterStartWalkingPacket(float fRotation, long lx, long ly);
		bool SendCharacterEndWalkingPacket(float fRotation, long lx, long ly);
		bool SendCharacterCheckWalkingPacket(float fRotation, long lx, long ly);

		bool SendCharacterPositionPacket(BYTE iPosition);

		bool SendItemUsePacket(TItemPos pos);
#ifdef ENABLE_INSTANT_CHEST_OPEN
		bool SendOpenChestPacket(TItemPos pos, WORD wOpenCount);
#endif
#ifdef ENABLE_INSTANT_INGOT_OPEN
		bool SendOpenIngotPacket(TItemPos pos, WORD wOpenCount);
#endif
		bool SendItemUseToItemPacket(TItemPos source_pos, TItemPos target_pos);
		bool SendItemDropPacket(TItemPos pos, DWORD elk);
		bool SendItemDropPacketNew(TItemPos pos, DWORD elk, DWORD count);
#ifdef ENABLE_EXTENDED_ITEM_COUNT
		bool SendItemMovePacket(TItemPos pos, TItemPos change_pos, uint16_t num);
#else
		bool SendItemMovePacket(TItemPos pos, TItemPos change_pos, BYTE num);
#endif
		bool SendItemPickUpPacket(DWORD vid);
		bool SendFastStackPacket(TItemPos itemPos);

		bool SendQuickSlotAddPacket(BYTE wpos, BYTE type, BYTE pos);
		bool SendQuickSlotDelPacket(BYTE wpos);
		bool SendQuickSlotMovePacket(BYTE wpos, BYTE change_pos);

		bool SendPointResetPacket();

		// Shop
		bool SendShopEndPacket();
		bool SendShopBuyPacket(BYTE byCount);
		bool SendShopSellPacket(BYTE bySlot);
#ifdef ENABLE_EXTENDED_ITEM_COUNT
		bool SendShopSellPacketNew(WORD wSlot, uint16_t byCount, BYTE byType);
#else
		bool SendShopSellPacketNew(BYTE bySlot, BYTE byCount);
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		bool SendOfflineShopEndPacket();
		bool SendOfflineShopBuyPacket(BYTE byCount);
		bool SendAddOfflineShopItem(BYTE bDisplayPos, TItemPos itemPos, long long lPrice, long long lPrice2);
		bool SendRemoveOfflineShopItem(BYTE bPos);
		bool SendDestroyOfflineShop();
		bool SendRefreshOfflineShop();
		bool SendRefreshOfflineShopMoney();
		bool SendRefreshOfflineShopChequeMoney();
		bool SendOfflineShopWithdrawMoney(long long dwMoney);
		bool SendOfflineShopWithdrawCheque(long long dwCheque);
#endif
		// Exchange
		bool SendExchangeStartPacket(DWORD vid);
#if defined(ITEM_CHECKINOUT_UPDATE)
		bool SendExchangeItemAddPacket(TItemPos ItemPos, BYTE byDisplayPos, bool SelectPosAuto);
#else
		bool SendExchangeItemAddPacket(TItemPos ItemPos, BYTE byDisplayPos);
#endif
#ifdef ENABLE_LONG_LONG
		bool SendExchangeElkAddPacket(long long elk);
#else
		bool SendExchangeElkAddPacket(DWORD elk);
#endif
#ifdef ENABLE_CHEQUE_SYSTEM
		bool SendExchangeChequeAddPacket(long long elk);
#endif
		bool SendExchangeItemDelPacket(BYTE pos);
#ifdef ENABLE_EVENT_MANAGER
		bool RecvEventManager();
		bool RecvEventActivateResult();
		bool SendEventActivatePacket(WORD eventID);
#endif
		bool RecvPozdro();
		bool SendPozdro();

		bool SendExchangeAcceptPacket();
		bool SendExchangeExitPacket();

		// Quest
		bool SendScriptAnswerPacket(int iAnswer);
#ifdef ENABLE_COLLECT_WINDOW
		bool SendScriptButtonPacket(unsigned int iIndex, BYTE bButtonIndex);
#else
		bool SendScriptButtonPacket(unsigned int iIndex);
#endif
		bool SendAnswerMakeGuildPacket(const char * c_szName);
		bool SendQuestInputStringPacket(const char * c_szString);
		bool SendQuestConfirmPacket(BYTE byAnswer, DWORD dwPID);

		// Event
		bool SendOnClickPacket(DWORD vid);

		// Fly
		bool SendFlyTargetingPacket(DWORD dwTargetVID, const TPixelPosition& kPPosTarget);
		bool SendAddFlyTargetingPacket(DWORD dwTargetVID, const TPixelPosition& kPPosTarget);
		bool SendShootPacket(UINT uSkill);

		// Command
		bool ClientCommand(const char * c_szCommand);
		void ServerCommand(const char * c_szCommand);

		// Emoticon
		void RegisterEmoticonString(const char * pcEmoticonString);

		// Party
		bool SendPartyInvitePacket(DWORD dwVID);
		bool SendPartyInviteAnswerPacket(DWORD dwLeaderVID, BYTE byAccept);
		bool SendPartyRemovePacket(DWORD dwPID);
		bool SendPartySetStatePacket(DWORD dwVID, BYTE byState, BYTE byFlag);
		bool SendPartyUseSkillPacket(BYTE bySkillIndex, DWORD dwVID);
		bool SendPartyParameterPacket(BYTE byDistributeMode);

		// SafeBox
		bool SendSafeBoxMoneyPacket(BYTE byState, DWORD dwMoney);
		bool SendSafeBoxCheckinPacket(TItemPos InventoryPos, BYTE bySafeBoxPos);
		bool SendSafeBoxCheckoutPacket(BYTE bySafeBoxPos, TItemPos InventoryPos);
#ifdef ENABLE_EXTENDED_ITEM_COUNT
		bool SendSafeBoxItemMovePacket(BYTE bySourcePos, BYTE byTargetPos, uint16_t byCount);
#else
		bool SendSafeBoxItemMovePacket(BYTE bySourcePos, BYTE byTargetPos, BYTE byCount);
#endif
		// Mall
		bool SendMallCheckoutPacket(BYTE byMallPos, TItemPos InventoryPos);

		// Guild
		bool SendGuildAddMemberPacket(DWORD dwVID);
		bool SendGuildRemoveMemberPacket(DWORD dwPID);
		bool SendGuildChangeGradeNamePacket(BYTE byGradeNumber, const char * c_szName);
		bool SendGuildChangeGradeAuthorityPacket(BYTE byGradeNumber, BYTE byAuthority);
		bool SendGuildOfferPacket(DWORD dwExperience);
		bool SendGuildPostCommentPacket(const char * c_szMessage);
		bool SendGuildDeleteCommentPacket(DWORD dwIndex);
		bool SendGuildRefreshCommentsPacket(DWORD dwHighestIndex);
		bool SendGuildChangeMemberGradePacket(DWORD dwPID, BYTE byGrade);
		bool SendGuildUseSkillPacket(DWORD dwSkillID, DWORD dwTargetVID);
		bool SendGuildChangeMemberGeneralPacket(DWORD dwPID, BYTE byFlag);
		bool SendGuildInvitePacket(DWORD dwVID);
		bool SendGuildInviteAnswerPacket(DWORD dwGuildID, BYTE byAnswer);
		bool SendGuildChargeGSPPacket(DWORD dwMoney);
		bool SendGuildDepositMoneyPacket(DWORD dwMoney);
		bool SendGuildWithdrawMoneyPacket(DWORD dwMoney);
		bool SendGuildLoadGuardItemsDataPacket();
		bool SendGuildLoadDungeonHistoryPacket();
		bool SendGuildNewNotePacket(const char* c_szNote);
		bool SendGuildRefreshNotePacket();
		bool SendGuildLoadLogsPacket(uint32_t lastLogID);
#ifdef ENABLE_OFFLINE_SHOP
		bool RecvOfflineshopPacket();
		bool RecvOfflineshopShopList();
		bool RecvOfflineshopShopOpen();
		bool RecvOfflineshopShopOpenOwner();
		bool RecvOfflineshopShopOpenOwnerNoShop();
		bool RecvOfflineshopShopClose();
		bool RecvOfflineshopShopFilterResult();
		bool RecvOfflineshopShopSafeboxRefresh();
		bool RecvOfflineshopShopBuyItemFromSearch();
#ifdef ENABLE_SHOP_IN_CITIES
		bool RecvOfflineshopInsertEntity();
		bool RecvOfflineshopRemoveEntity();
		void SendOfflineshopOnClickShopEntity(DWORD dwPickedShopVID);
#endif
#ifdef ENABLE_OFFLINESHOP_NOTIFICATION
		bool RecvOfflineshopNotification();
#endif
#ifdef ENABLE_AVERAGE_PRICE
		bool RecvOfflineshopAveragePrice();
#endif
		void SendOfflineshopShopCreate(const offlineshop::TShopInfo& shopInfo, const std::vector<offlineshop::TShopItemInfo>& items);
		void SendOfflineshopChangeName(const char* szName);
		void SendOfflineshopForceCloseShop();
		void SendOfflineshopRequestShopList();
		void SendOfflineshopOpenShop(DWORD dwOwnerID);
		void SendOfflineshopOpenShopOwner();
		void SendOfflineshopBuyItem(DWORD dwOwnerID, DWORD dwItemID, bool isSearch);
		void SendOfflineshopAddItem(offlineshop::TShopItemInfo& itemInfo);
		void SendOfflineshopRemoveItem(DWORD dwItemID);
		void SendOfflineShopEditItem(DWORD dwItemID, const offlineshop::TPriceInfo& price, bool allEdit);
		void SendOfflineshopFilterRequest(const offlineshop::TFilterInfo& filter);
		void SendOfflineshopSafeboxOpen();
		void SendOfflineshopSafeboxGetItem(DWORD dwItemID);
		void SendOfflineshopSafeboxGetValutes(const unsigned long long& valutes);
		void SendOfflineshopSafeboxClose();
		void SendOfflineshopCloseBoard();
#ifdef ENABLE_OFFLINESHOP_EXTEND_TIME
		void SendOfflineshopExtendTime(DWORD dwTime);
#endif
#ifdef ENABLE_AVERAGE_PRICE
		void SendOfflineShopAveragePrice(DWORD vnum);
#endif
#endif

#ifdef WEEKLY_RANK_BYLUZER
		bool SelectWeeklyRankPage(int id);
#endif
#ifdef TITLE_SYSTEM_BYLUZER
		bool SendSelectTitle(int id);
#endif
		// Mall
		bool RecvMallOpenPacket();
		bool RecvMallItemSetPacket();
		bool RecvMallItemDelPacket();
#if defined(__BL_MOVE_CHANNEL__)
		bool SendMoveChannelPacket(const BYTE bChannelIndex);
#endif
		// Lover
		bool RecvLoverInfoPacket();
		bool RecvLovePointUpdatePacket();

		// Dig
		bool RecvDigMotionPacket();
#if defined(__BL_KILL_BAR__)
		bool RecvKillBar();
#endif

		// Fishing
		bool SendFishingPacket(int iRotation);
		bool SendGiveItemPacket(DWORD dwTargetVID, TItemPos ItemPos, int iItemCount);

		// Private Shop
		bool SendBuildPrivateShopPacket(const char * c_szName, const std::vector<TShopItemTable> & c_rSellingItemStock);
		// Offline Shop
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		bool SendBuildOfflineShopPacket(const char * c_szName, const std::vector<TShopItemTable> & c_rSellingItemStock);
#endif
		// Refine
		bool SendRefinePacket(BYTE byPos, BYTE byType);
		bool SendSelectItemPacket(DWORD dwItemPos);
#ifdef ENABLE_MOUNT_SYSTEM
		bool RecvMountInfoPacket();
#endif
#ifdef ENABLE_PERFORMANCE_IMPROVEMENTS_NEW
		void SendPythonData(PyObject* obj, const char* funcname);
		void SetClientLoaded();
#endif
		// Client Version
		bool SendClientVersionPacket();

		// CRC Report
		bool __SendCRCReportPacket();

		bool SendDragonSoulRefinePacket(BYTE bRefineType, TItemPos* pos);

		// Handshake
		bool RecvHandshakePacket();
		bool RecvHandshakeOKPacket();

		bool RecvHybridCryptKeyPacket();
		bool RecvHybridCryptSDBPacket();
#ifdef ENABLE_LRN_LOCATION_SYSTEM
		bool RecvPlayerLocationData();
		bool SendPlayerLocationManager(BYTE bAction, BYTE bPosition, const char* sz_Name);
#endif
#ifdef _IMPROVED_PACKET_ENCRYPTION_
		bool RecvKeyAgreementPacket();
		bool RecvKeyAgreementCompletedPacket();

#endif

		PyObject* GetGameWindow() { return m_apoPhaseWnd[PHASE_WINDOW_GAME]; }
		// ETC
		DWORD GetMainActorVID();
		DWORD GetMainActorRace();
		DWORD GetMainActorEmpire();
		DWORD GetMainActorSkillGroup();
		void SetEmpireID(DWORD dwEmpireID);
		DWORD GetEmpireID();
		void __TEST_SetSkillGroupFake(int iIndex);

#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
		bool CubeRenewalMakeItem(int index_item, int count_item, int index_item_improve);
		bool CubeRenewalClose();
		bool RecvCubeRenewalPacket();
#endif

#ifdef ENABLE_SAVE_LOCATION_SYSTEM
		bool	SendSaveLocationPacket(BYTE subheader, BYTE pos, const char* c_szName);
		bool	RecvSaveLocationPacket();
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		bool	SendAcceClosePacket();
		bool	SendAcceAddPacket(TItemPos tPos, BYTE bPos);
		bool	SendAcceRemovePacket(BYTE bPos);
		bool	SendAcceRefinePacket();
#endif

#ifdef ENABLE_AURA_SYSTEM
		bool RecvAuraPacket(bool bReturn = false);
		bool SendAuraClosePacket();
		bool SendAuraAddPacket(TItemPos tPos, BYTE bPos);
		bool SendAuraRemovePacket(BYTE bPos);
		bool SendAuraRefinePacket();
#endif
#ifdef ENABLE_PERFORMANCE_BOOST
		void AllowLogin();
#endif

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	public:
		void SetOffLinePhase();
		void SetHandShakePhase();
		void SetLoginPhase();
		void SetSelectPhase();
		void SetLoadingPhase();
		void SetGamePhase();
		void ClosePhase();

		// Login Phase
		bool SendLoginPacket(const char * c_szName, const char * c_szPassword);
		bool SendLoginPacketNew(const char * c_szName, const char * c_szPassword);
		bool SendDirectEnterPacket(const char * c_szName, const char * c_szPassword, UINT uChrSlot);

		bool SendEnterGame();

		// Select Phase
		bool SendSelectEmpirePacket(DWORD dwEmpireID);
		bool SendSelectCharacterPacket(BYTE account_Index);
		bool SendChangeNamePacket(BYTE index, const char *name);
		bool SendCreateCharacterPacket(BYTE index, const char *name, BYTE job, BYTE shape, BYTE byStat1, BYTE byStat2, BYTE byStat3, BYTE byStat4);
		bool SendDestroyCharacterPacket(BYTE index, const char * szPrivateCode);

		// Main Game Phase
		bool SendC2CPacket(DWORD dwSize, void * pData);
		bool SendChatPacket(const char * c_szChat, BYTE byType = CHAT_TYPE_TALKING);
		bool SendWhisperPacket(const char * name, const char * c_szChat);
		bool SendMobileMessagePacket(const char * name, const char * c_szChat);
		bool SendMessengerAddByVIDPacket(DWORD vid);
		bool SendMessengerAddByNamePacket(const char * c_szName);
		bool SendMessengerRemovePacket(const char * c_szKey, const char * c_szName);
#ifdef ENABLE_MESSENGER_BLOCK
		bool SendMessengerBlockAddByVIDPacket(DWORD vid);
		bool SendMessengerBlockRemoveByVIDPacket(DWORD vid);
		bool SendMessengerBlockAddByNamePacket(const char* c_szName);
		bool SendMessengerBlockRemovePacket(const char* c_szKey, const char* c_szName);
#endif
#ifdef RANKING_SYSTEM
		bool SendLoadServerRanking(uint8_t catIdx);
		bool RecvLoadServerRanking();

		bool SendLoadRanking(uint8_t catIdx);
		bool RecvLoadRanking();
#endif
#ifdef ENABLE_IN_GAME_LOG_SYSTEM
		void SelectCharacterCacheDestroy();
#endif

	protected:
		bool OnProcess();
		void OffLinePhase();
		void HandShakePhase();
		void LoginPhase();
		void SelectPhase();
		void LoadingPhase();
		void GamePhase();

		bool __IsNotPing();

		void __DownloadMark();
		void __DownloadSymbol(const std::vector<DWORD> & c_rkVec_dwGuildID);

		void __PlayInventoryItemUseSound(TItemPos uSlotPos);
		void __PlayInventoryItemDropSound(TItemPos uSlotPos);
		//void __PlayShopItemDropSound(UINT uSlotPos);
		void __PlaySafeBoxItemDropSound(UINT uSlotPos);
		void __PlayMallItemDropSound(UINT uSlotPos);

		bool __CanActMainInstance();

		enum REFRESH_WINDOW_TYPE
		{
			RefreshStatus = (1 << 0),
			RefreshAlignmentWindow = (1 << 1),
			RefreshCharacterWindow = (1 << 2),
			RefreshEquipmentWindow = (1 << 3),
			RefreshInventoryWindow = (1 << 4),
			RefreshExchangeWindow = (1 << 5),
			RefreshSkillWindow = (1 << 6),
			RefreshSafeboxWindow  = (1 << 7),
			RefreshMessengerWindow = (1 << 8),
			RefreshGuildWindowInfoPage = (1 << 9),
			RefreshGuildWindowBoardPage = (1 << 10),
			RefreshGuildWindowMemberPage = (1 << 11),
			RefreshGuildWindowMemberPageGradeComboBox = (1 << 12),
			RefreshGuildWindowSkillPage = (1 << 13),
			RefreshGuildWindowGradePage = (1 << 14),
			RefreshTargetBoard = (1 << 15),
			RefreshMallWindow = (1 << 16),
		};

		void __RefreshStatus();
		void __RefreshAlignmentWindow();
		void __RefreshCharacterWindow();
		void __RefreshEquipmentWindow();
		void __RefreshInventoryWindow();
		void __RefreshExchangeWindow();
		void __RefreshSkillWindow();
		void __RefreshSafeboxWindow();
		void __RefreshMessengerWindow();
		void __RefreshGuildWindowInfoPage();
		void __RefreshGuildWindowDungeonInfoPage();
		void __RefreshGuildWindowDungeonBossDmgRankWindow();
		void __RefreshGuildWindowBoardPage();
		void __RefreshGuildWindowMemberPage();
		void __RefreshGuildWindowMemberPageGradeComboBox();
		void __RefreshGuildWindowSkillPage();
		void __RefreshGuildWindowGradePage();
		void __RefreshTargetBoardByVID(DWORD dwVID);
		void __RefreshTargetBoardByName(const char * c_szName);
		void __RefreshTargetBoard();
		void __RefreshMallWindow();
#ifdef ENABLE_AFFECT_FIX
		void __RefreshAffectWindow();
#endif

		bool __SendHack(const char* c_szMsg);

	protected:
		bool RecvObserverAddPacket();
		bool RecvObserverRemovePacket();
		bool RecvObserverMovePacket();

		// Common
		bool RecvErrorPacket(int header);
		bool RecvPingPacket();
		bool RecvDefaultPacket(int header);
		bool RecvPhasePacket();

		// Login Phase
		bool __RecvLoginSuccessPacket3();
		bool __RecvLoginSuccessPacket4();
		bool __RecvLoginFailurePacket();
		bool __RecvEmpirePacket();
		bool __RecvLoginKeyPacket();

		// Select Phase
		bool __RecvPlayerCreateSuccessPacket();
		bool __RecvPlayerCreateFailurePacket();
		bool __RecvPlayerDestroySuccessPacket();
		bool __RecvPlayerDestroyFailurePacket();
		bool __RecvPreserveItemPacket();
		bool __RecvPlayerPoints();
		bool __RecvChangeName();

		// Loading Phase
		bool RecvMainCharacter();
		bool RecvMainCharacter2_EMPIRE();
		bool RecvMainCharacter3_BGM();
		bool RecvMainCharacter4_BGM_VOL();

		void __SetFieldMusicFileName(const char* musicName);
		void __SetFieldMusicFileInfo(const char* musicName, float vol);
		// END_OF_SUPPORT_BGM
#ifdef WEEKLY_RANK_BYLUZER
		bool RecvWeeklyRank();
		bool RecvWeeklyPage();
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		bool RecvOfflineShopLogs();
#endif
#ifdef TITLE_SYSTEM_BYLUZER
		bool RecvTitleActive();
		bool RecvTitleInfo();
#endif
#ifdef TAKE_LEGEND_DAMAGE_BOARD_SYSTEM
		bool RecvLegendDamageData();
#endif
		// Main Game Phase
		bool RecvWarpPacket();
#ifdef ENABLE_ENTITY_PRELOADING
		bool RecvPreloadEntitiesPacket();
#endif
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
		bool RecvTitleAchievementPacket();
#endif
		bool RecvPVPPacket();
		bool RecvDuelStartPacket();
        bool RecvGlobalTimePacket();
		bool RecvCharacterAppendPacket();
		bool RecvCharacterAdditionalInfo();
		bool RecvCharacterAppendPacketNew();
		bool RecvCharacterUpdatePacket();
		bool RecvCharacterDeletePacket();
		bool RecvChatPacket();
		bool RecvOwnerShipPacket();
		bool RecvSyncPositionPacket();
		bool RecvWhisperPacket();
		bool RecvPointChange();						// Alarm to python
		bool RecvChangeSpeedPacket();

		bool RecvStunPacket();
		bool RecvDeadPacket();
		bool RecvCharacterMovePacket();

		bool RecvItemSetPacket();					// Alarm to python
		bool RecvItemSetPacket2();					// Alarm to python
		bool RecvItemUsePacket();					// Alarm to python
		bool RecvItemUpdatePacket();				// Alarm to python
		bool RecvItemGroundAddPacket();
		bool RecvItemGroundDelPacket();
		bool RecvItemOwnership();

		bool RecvQuickSlotAddPacket();				// Alarm to python
		bool RecvQuickSlotDelPacket();				// Alarm to python
		bool RecvQuickSlotMovePacket();				// Alarm to python

		bool RecvCharacterPositionPacket();
		bool RecvMotionPacket();

		bool RecvShopPacket();
		bool RecvShopSignPacket();
		bool RecvExchangePacket();
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		bool RecvOfflineShopPacket();
		bool RecvOfflineShopSignPacket();
#endif
		// Quest
		bool RecvScriptPacket();
		bool RecvQuestInfoPacket();
		bool RecvQuestConfirmPacket();
		bool RecvRequestMakeGuild();

		// Skill
		bool RecvSkillLevel();
		bool RecvSkillLevelNew();
		bool RecvSkillCoolTimeEnd();

		// Target
		bool RecvTargetPacket();
		bool RecvViewEquipPacket();
		bool RecvDamageInfoPacket();

		// Mount
		bool RecvMountPacket();

		// Fly
		bool RecvCreateFlyPacket();
		bool RecvFlyTargetingPacket();
		bool RecvAddFlyTargetingPacket();

		// Messenger
		bool RecvMessenger();

		// Guild
		bool RecvGuild();

		// Notification
#ifdef ENABLE_NOTIFICATION_SYSTEM
		bool RecvNotification();
#endif

		// Party
		bool RecvPartyInvite();
		bool RecvPartyAdd();
		bool RecvPartyUpdate();
		bool RecvPartyRemove();
		bool RecvPartyLink();
		bool RecvPartyUnlink();
		bool RecvPartyParameter();

		// SafeBox
		bool RecvSafeBoxSetPacket();
		bool RecvSafeBoxDelPacket();
		bool RecvSafeBoxWrongPasswordPacket();
		bool RecvSafeBoxSizePacket();
		bool RecvSafeBoxMoneyChangePacket();

		// Fishing
		bool RecvFishing();
#ifdef ENABLE_ITEMSHOP
		bool RecvItemShop();
#endif

#if defined(ENABLE_DUNGEON_INFO_SYSTEM)
		// Dungeon Information
	public:
		bool SendDungeonInfo(BYTE bAction, BYTE bIndex = 0, BYTE bRankType = 0);

	protected:
		bool RecvDungeonInfo();
		bool RecvDungeonRanking();
#endif

		// Dungeon
		bool RecvDungeon();

		// Time
		bool RecvTimePacket();

		// WalkMode
		bool RecvWalkModePacket();

		// ChangeSkillGroup
		bool RecvChangeSkillGroupPacket();

		// Refine
		bool RecvRefineInformationPacket();
		bool RecvRefineInformationPacketNew();

		// Use Potion
		bool RecvSpecialEffect();

		bool RecvSpecificEffect();

		bool RecvDragonSoulRefine();

		// MiniMap Info
		bool RecvNPCList();
		bool RecvLandPacket();
		bool RecvTargetCreatePacket();
		bool RecvTargetCreatePacketNew();
		bool RecvTargetUpdatePacket();
		bool RecvTargetDeletePacket();

		// Affect
		bool RecvAffectAddPacket();
		bool RecvAffectRemovePacket();

		// Channel
		bool RecvChannelPacket();
#ifdef __INVENTORY_BUFFERING__
		bool RecvBufferedInventoryPacket();
#endif
#ifdef ENABLE_PVP_RANKING
		bool RecvPvPRankingPacket();
#endif

#ifdef ENABLE_COLLECT_WINDOW
		bool RecvCollectPacket();
#endif
		bool RecvAntyExp();
#ifdef WJ_SPLIT_INVENTORY_SYSTEM
		bool RecvWpadanie();
#endif
		bool GiveItems();
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		bool RecvAccePacket(bool bReturn = false);
#endif

#ifdef ENABLE_NEW_PET_SYSTEM
	public:
		bool SendPetAction(DWORD skillvnum);
#endif
		bool SendRecvItems(DWORD item_count);
	protected:
		bool ParseEmoticon(const char * pChatMsg, DWORD * pdwEmoticon);

		void OnConnectFailure();
		void OnScriptEventStart(int iSkin, int iIndex);

		void OnRemoteDisconnect();
		void OnDisconnect();

		void SetGameOnline();
		void SetGameOffline();
		BOOL IsGameOnline();

	public:
		bool SendMountNPCAction(BYTE bAction, DWORD dValue0);

	protected:
		bool RecvMountNPCAction();

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	public:
		bool SendBuffNPCCreate(BYTE bAction, const char* szName, BYTE bSex);
		bool SendBuffNPCAction(BYTE bAction, DWORD dValue0, DWORD dValue1);

	protected:
		bool RecvBuffNPCAction();
		bool RecvBuffNPCUseSkill();
#endif

	protected:
		bool CheckPacket(TPacketHeader * pRetHeader);
#ifdef ENABLE_MINIMAP_DUNGEONINFO
	protected:
		bool RecvMiniMapDungeonInfo();
#endif

		void __InitializeGamePhase();
		void __InitializeMarkAuth();
		void __GlobalPositionToLocalPosition(LONG& rGlobalX, LONG& rGlobalY);
		void __LocalPositionToGlobalPosition(LONG& rLocalX, LONG& rLocalY);

		bool __IsPlayerAttacking();
		bool __IsEquipItemInSlot(TItemPos Cell);

		void __ShowMapName(LONG lLocalX, LONG lLocalY);

		void __LeaveOfflinePhase() {}
		void __LeaveHandshakePhase() {}
		void __LeaveLoginPhase() {}
		void __LeaveSelectPhase() {}
		void __LeaveLoadingPhase() {}
		void __LeaveGamePhase();

		void __ClearNetworkActorManager();

		void __ClearSelectCharacterData();

		// DELETEME
		//void __SendWarpPacket();

		void __ConvertEmpireText(DWORD dwEmpireID, char* szText);

		void __RecvCharacterAppendPacket(SNetworkActorData * pkNetActorData);
		void __RecvCharacterUpdatePacket(SNetworkUpdateActorData * pkNetUpdateActorData);

		void __FilterInsult(char* szLine, UINT uLineLen);

		void __SetGuildID(DWORD id);

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
	public:
		bool SendBuildPrivateShopPacket(const char* c_szTitle, DWORD dwPolyVnum, BYTE bTitleType, BYTE bPageCount, const std::vector<TPrivateShopItem>& c_vec_itemStock);
		bool SendClosePrivateShopPacket();
		bool SendClosePrivateShopPanelPacket();
		bool SendOpenPrivateShopPanelPacket(bool bCheck = false);
		bool SendPrivateShopStartPacket(DWORD dwVID);
		bool SendPrivateShopEndPacket();
		bool SendPrivateShopBuyPacket(WORD wPos);
		bool SendWithdrawPrivateShopPacket();
		bool SendModifyPrivateShopPacket();
		bool SendItemPriceChangePrivateShopPacket(WORD wPos, long long llGold, DWORD dwCheque);
		bool SendItemMovePrivateShopPacket(WORD wPos, WORD wChangePos);
		bool SendItemCheckinPrivateShopPacket(WORD wSrcPos, WORD wSrcWindow, long long llGold, DWORD dwCheque, int iDstPos = -1);
		bool SendItemCheckoutPrivateShopPacket(WORD wSrcPos, int iDstPos = -1);
		bool SendTitleChangePrivateShopPacket(const char* c_szTitle);
		bool SendPrivateShopWarpRequest();
		bool SendPrivateShopSlotUnlockRequest(WORD wSrcPos);

		bool SendClosePrivateShopSearchPacket();
		bool SendPrivateShopSearchPacket(TPacketCGPrivateShopSearch* pPacket);
		bool SendPrivateShopSearchBuyPacket(int* pSelectedItem);

		bool SendPrivateShopMarketItemPriceDataReqPacket();
		bool SendPrivateShopMarketItemPriceReqPacket(DWORD dwVnum);

	protected:
		bool RecvPrivateShop();
		void __RefreshPrivateShopWindow() { m_isRefreshPrivateShopWindow = true; }

	private:
		bool m_isRefreshPrivateShopWindow;
#endif
	protected:
		TPacketGCHandshake m_HandshakeData;
		DWORD m_dwChangingPhaseTime;
		DWORD m_dwBindupRetryCount;
		DWORD m_dwMainActorVID;
		DWORD m_dwMainActorRace;
		DWORD m_dwMainActorEmpire;
		DWORD m_dwMainActorSkillGroup;
		BOOL m_isGameOnline;
		BOOL m_isStartGame;

		DWORD m_dwGuildID;
		DWORD m_dwEmpireID;

		struct SServerTimeSync
		{
			DWORD m_dwChangeServerTime;
			DWORD m_dwChangeClientTime;
		} m_kServerTimeSync;

		void __ServerTimeSync_Initialize();
		//DWORD m_dwBaseServerTime;
		//DWORD m_dwBaseClientTime;

		DWORD m_dwLastGamePingTime;

		std::string	m_stID;
		std::string	m_stPassword;
		std::string	m_stPin;
		std::string	m_strLastCommand;
		GamePhases	m_gamePhase;
		std::string	m_strPhase;
		DWORD m_dwLoginKey;
		BOOL m_isWaitLoginKey;

		std::string m_stMarkIP;

		CFuncObject<CPythonNetworkStream>	m_phaseProcessFunc;
		CFuncObject<CPythonNetworkStream>	m_phaseLeaveFunc;

		PyObject*							m_poHandler;
		PyObject*							m_apoPhaseWnd[PHASE_WINDOW_NUM];
		PyObject*							m_poSerCommandParserWnd;

		TSimplePlayerInformation			m_akSimplePlayerInfo[PLAYER_PER_ACCOUNT4];
		DWORD								m_adwGuildID[PLAYER_PER_ACCOUNT4];
		std::string							m_astrGuildName[PLAYER_PER_ACCOUNT4];
		bool m_bSimplePlayerInfo;

		CRef<CNetworkActorManager>			m_rokNetActorMgr;

		bool m_isRefreshStatus;
		bool m_isRefreshCharacterWnd;
		bool m_isRefreshEquipmentWnd;
		bool m_isRefreshInventoryWnd;
		bool m_isRefreshExchangeWnd;
		bool m_isRefreshSkillWnd;
		bool m_isRefreshSafeboxWnd;
		bool m_isRefreshMallWnd;
		bool m_isRefreshMessengerWnd;
		bool m_isRefreshGuildWndInfoPage;
		bool m_isRefreshGuildWndBoardPage;
		bool m_isRefreshGuildWndMemberPage;
		bool m_isRefreshGuildWndMemberPageGradeComboBox;
		bool m_isRefreshGuildWndSkillPage;
		bool m_isRefreshGuildWndGradePage;
		bool m_isRefreshGuildWndDungeonInfoPage;
		bool m_isRefreshGuildDungeonBossDmgRankWnd;
#ifdef ENABLE_AFFECT_FIX
		bool m_isRefreshAffectWindow;
#endif

		// Emoticon
		std::vector<std::string> m_EmoticonStringVector;

		struct STextConvertTable
		{
			char acUpper[26];
			char acLower[26];
			BYTE aacHan[5000][2];
		} m_aTextConvTable[3];

		struct SMarkAuth
		{
			CNetworkAddress m_kNetAddr;
			DWORD m_dwHandle;
			DWORD m_dwRandomKey;
		} m_kMarkAuth;

		DWORD m_dwSelectedCharacterIndex;

		CInsultChecker m_kInsultChecker;

		bool m_isEnableChatInsultFilter;
		bool m_bComboSkillFlag;

		std::deque<std::string> m_kQue_stHack;

	private:
		struct SDirectEnterMode
		{
			bool m_isSet;
			DWORD m_dwChrSlotIndex;
		} m_kDirectEnterMode;

		void __DirectEnterMode_Initialize();
		void __DirectEnterMode_Set(UINT uChrSlotIndex);
		bool __DirectEnterMode_IsSet();

	public:
		bool DirectEnterMode_IsSet() { return __DirectEnterMode_IsSet(); }

	public:
		DWORD EXPORT_GetBettingGuildWarValue(const char* c_szValueName);

	private:
		struct SBettingGuildWar
		{
			DWORD m_dwBettingMoney;
			DWORD m_dwObserverCount;
		} m_kBettingGuildWar;

		CInstanceBase * m_pInstTarget;

		void __BettingGuildWar_Initialize();
		void __BettingGuildWar_SetObserverCount(UINT uObserverCount);
		void __BettingGuildWar_SetBettingMoney(UINT uBettingMoney);

#ifdef __ENABLE_MINING_WITH_SPACE__
	public:
		bool SendMining(BYTE bySubheader);
#endif
#ifdef ENABLE_ODLAMKI_SYSTEM
	public:
		bool SendOdlamkiItemPacket(std::vector<TItemPos> vec_item_pos);
	private:
		bool RecvOdlamkiItemPacket();
#endif
#ifdef ENABLE_VS_SHOP_SEARCH
	public:
		bool SendSearchPacket(std::vector<TSearchItems> vec_items);
#endif
#ifdef ENABLE_REMOVE_ITEM
	public:
		bool SendRemoveItemPacket(std::vector<TItemPos> vec_item_pos);
	private:
		bool RecvRemoveItemPacket();
#endif
#ifdef ENABLE_RESP_SYSTEM
	public:
		bool SendRespFetchDropPacket(uint32_t mobVnum);
		bool SendRespFetchRespPacket(uint32_t mobVnum);
		bool SendRespTeleportPacket(uint32_t id);
	private:
		bool RecvRespPacket();
#endif
#ifdef ENABLE_SWITCHBOT
	public:
		bool RecvSwitchbotPacket();

		//bool SendSwitchbotStartPacket(BYTE slot, DWORD switchingItemVnum, std::vector<CPythonSwitchbot::TSwitchbotAttributeAlternativeTable> alternatives); 
		bool SendSwitchbotStartPacket(BYTE slot, std::vector<CPythonSwitchbot::TSwitchbotAttributeAlternativeTable> alternatives);
		bool SendSwitchbotStopPacket(BYTE slot);
#endif
#ifdef ENABLE_IN_GAME_LOG_SYSTEM
	public:
		bool RecvInGameLog();
		bool RecvOfflineShopLog(WORD bufferSize);
		bool RecvOfflineShopLogAdd(WORD bufferSize);
		bool RecvTradeLog(WORD bufferSize);
		bool RecvTradeDetailsLog(WORD bufferSize);
#endif
#ifdef ENABLE_SECONDARY_LEVEL
	public:
		bool	SendSecondaryLevelPacket(ESecondaryLevelSubheader SubHeader, BYTE bOption);
	protected:
		bool	RecvSecondaryLevelPacket();
#endif
#ifdef ENABLE_DROP_INFO
	public:
		bool SendDropInfoPacket();
	private:
		bool RecvDropInfoPacket();
#endif
#ifdef ENABLE_TOMBOLA
	public:
		bool SendTombolaStartPacket(BYTE byGroup);
		bool SendTombolaOpenPacket();
	protected:
		bool RecvTombolaPacket();
#endif
#ifdef __ENABLE_POLYMORPH_SYSTEM__
	public:
		bool RecvPolySystemPacket();
		bool SendPolySystemOpen();
		bool SendPolySystemClose();
		bool SendPolySystemSkin(BYTE byIndex);
		bool SendPolySystemBuy(BYTE byIndex);
#endif
};
//martysama0134's ceqyqttoaf71vasf9t71218
