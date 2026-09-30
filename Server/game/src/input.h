#pragma once

#ifndef __INC_METIN_II_GAME_INPUT_PROCESSOR__
#define __INC_METIN_II_GAME_INPUT_PROCESSOR__

#include "packet_info.h"

enum
{
	INPROC_CLOSE,
	INPROC_HANDSHAKE,
	INPROC_LOGIN,
	INPROC_MAIN,
	INPROC_DEAD,
	INPROC_DB,
	INPROC_P2P,
	INPROC_AUTH,
};
#ifdef ENABLE_ANTIHL
const std::vector<DWORD> m_vecCleanIDs{ 0x1337, 0x2137, 0x5894, 0x215874, 0x235698, 0x224853 };
enum eCheatDetections
{
	HLBOT = 0x188222,
	COM2,
	REX,
	MACRO,
	UNKOWN
};
#endif

void LoginFailure(LPDESC d, const char * c_pszStatus);
extern void SendShout(const char * szText, BYTE bEmpire);

class CInputProcessor
{
	public:
		CInputProcessor();
		virtual ~CInputProcessor() {};

		virtual bool Process(LPDESC d, const void * c_pvOrig, int iBytes, int & r_iBytesProceed);
		virtual BYTE GetType() = 0;

		void BindPacketInfo(CPacketInfo * pPacketInfo);
#ifdef ENABLE_ANTIHL
		void Pong(LPDESC d, const char* c_pData);
#else
		void Pong(LPDESC d);
#endif
		void Handshake(LPDESC d, const char * c_pData);
		void Version(LPCHARACTER ch, const char* c_pData);

	protected:
		virtual int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData) = 0;

		CPacketInfo * m_pPacketInfo;
		int	m_iBufferLeft;

		static CPacketInfoCG 	m_packetInfoCG;
};

class CInputClose : public CInputProcessor
{
	public:
		virtual BYTE	GetType() { return INPROC_CLOSE; }

	protected:
		virtual int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData) { return m_iBufferLeft; }
};

class CInputHandshake : public CInputProcessor
{
	public:
		CInputHandshake();
		virtual ~CInputHandshake();

		virtual BYTE	GetType() { return INPROC_HANDSHAKE; }

	protected:
		virtual int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData);

	protected:
		void		GuildMarkLogin(LPDESC d, const char* c_pData);

		CPacketInfo *	m_pMainPacketInfo;
};

class CInputLogin : public CInputProcessor
{
	public:
		virtual BYTE	GetType() { return INPROC_LOGIN; }

	protected:
		virtual int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData);

	protected:
		void		Login(LPDESC d, const char * data);
		void		LoginByKey(LPDESC d, const char * data);

		void		CharacterSelect(LPDESC d, const char * data);
		void		CharacterCreate(LPDESC d, const char * data);
		void		CharacterDelete(LPDESC d, const char * data);
		void		Entergame(LPDESC d, const char * data);
		void		Empire(LPDESC d, const char * c_pData);
		void		GuildMarkCRCList(LPDESC d, const char* c_pData);
		// MARK_BUG_FIX
		void		GuildMarkIDXList(LPDESC d, const char* c_pData);
		// END_OF_MARK_BUG_FIX
		void		GuildMarkUpload(LPDESC d, const char* c_pData);
		int			GuildSymbolUpload(LPDESC d, const char* c_pData, size_t uiBytes);
		void		GuildSymbolCRC(LPDESC d, const char* c_pData);
		void		ChangeName(LPDESC d, const char * data);
};

class CInputMain : public CInputProcessor
{
	public:
		virtual BYTE	GetType() { return INPROC_MAIN; }

	protected:
		virtual int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData);

	protected:
		void		Attack(LPCHARACTER ch, const BYTE header, const char* data);
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		int			OfflineShop(LPCHARACTER ch, const char * data, size_t uiBytes);
		int			MyOfflineShop(LPCHARACTER ch, const char * c_pData, size_t uiBytes);
#endif

#ifdef ENABLE_CHEQUE_SYSTEM
		void		WonExchange(LPCHARACTER ch, const char* pcData);
#endif
		int			Whisper(LPCHARACTER ch, const char * data, size_t uiBytes);
		int			Chat(LPCHARACTER ch, const char * data, size_t uiBytes);
		void		ItemUse(LPCHARACTER ch, const char * data);
		void		ItemDrop(LPCHARACTER ch, const char * data);
		void		ItemDrop2(LPCHARACTER ch, const char * data);

		void		pozdro(LPCHARACTER ch, const char * data);
		void		ItemMove(LPCHARACTER ch, const char * data);
		void		ItemPickup(LPCHARACTER ch, const char * data);
		void		ItemToItem(LPCHARACTER ch, const char * pcData);
		void		QuickslotAdd(LPCHARACTER ch, const char * data);
		void		QuickslotDelete(LPCHARACTER ch, const char * data);
		void		QuickslotSwap(LPCHARACTER ch, const char * data);
		int			Shop(LPCHARACTER ch, const char * data, size_t uiBytes);
		void		OnClick(LPCHARACTER ch, const char * data);
		void		Exchange(LPCHARACTER ch, const char * data);
		void		Position(LPCHARACTER ch, const char * data);
		void		Move(LPCHARACTER ch, const char * data);
		int			SyncPosition(LPCHARACTER ch, const char * data, size_t uiBytes);
		void		FlyTarget(LPCHARACTER ch, const char * pcData, BYTE bHeader);
		void		UseSkill(LPCHARACTER ch, const char * pcData);
		void		FastStack(LPCHARACTER ch, const char* pcData);

		void		ScriptAnswer(LPCHARACTER ch, const void * pvData);
		void		ScriptButton(LPCHARACTER ch, const void * pvData);
		void		ScriptSelectItem(LPCHARACTER ch, const void * pvData);

		void		QuestInputString(LPCHARACTER ch, const void * pvData);
		void		QuestConfirm(LPCHARACTER ch, const void* pvData);
		void		Target(LPCHARACTER ch, const char * pcData);
		void		Warp(LPCHARACTER ch, const char * pcData);
		void		SafeboxCheckin(LPCHARACTER ch, const char * c_pData);
		void		SafeboxCheckout(LPCHARACTER ch, const char * c_pData, bool bMall);
#ifdef WEEKLY_RANK_BYLUZER
		void		SelectWeeklyRank(LPCHARACTER ch, const char* c_pData);
#endif
		void		SafeboxItemMove(LPCHARACTER ch, const char * data);
		int			Messenger(LPCHARACTER ch, const char* c_pData, size_t uiBytes);
#ifdef TITLE_SYSTEM_BYLUZER
		void		ActiveTitleNow(LPCHARACTER ch, const char* c_pData);
#endif
		void 		PartyInvite(LPCHARACTER ch, const char * c_pData);
#if defined(__BL_MOVE_CHANNEL__)
		void		MoveChannel(LPCHARACTER ch, const char* c_pData);
#endif
		void 		PartyInviteAnswer(LPCHARACTER ch, const char * c_pData);
		void		PartyRemove(LPCHARACTER ch, const char * c_pData);
		void		PartySetState(LPCHARACTER ch, const char * c_pData);
		void		PartyUseSkill(LPCHARACTER ch, const char * c_pData);
		void		PartyParameter(LPCHARACTER ch, const char * c_pData);

		void		GivedItems(LPCHARACTER ch, const char * c_pData);

		int			Guild(LPCHARACTER ch, const char * data, size_t uiBytes);
		void		AnswerMakeGuild(LPCHARACTER ch, const char* c_pData);

		void		Fishing(LPCHARACTER ch, const char* c_pData);
		void		ItemGive(LPCHARACTER ch, const char* c_pData);
		void		Hack(LPCHARACTER ch, const char * c_pData);
		int			MyShop(LPCHARACTER ch, const char * c_pData, size_t uiBytes);

#ifdef __ENABLE_MINING_WITH_SPACE__
		void		Mining(LPCHARACTER ch, const char* c_pData);
#endif

		void		Refine(LPCHARACTER ch, const char* c_pData);
#ifdef ENABLE_RESP_SYSTEM
		int			Resp(LPCHARACTER ch, const char* c_pData, size_t uiBytes);
#endif
#ifdef __REMOVE_ITEM__
		int			RemoveItem(LPCHARACTER ch, const char* c_pData, size_t uiBytes);
#endif
#ifdef ENABLE_SAVE_LOCATION_SYSTEM
		void		SaveLocation(LPCHARACTER pkChar, const char* c_pData);
#endif
#ifdef ENABLE_ODLAMKI_SYSTEM
		int			OdlamkiItem(LPCHARACTER ch, const char* c_pData, size_t uiBytes);
#endif
#ifdef ENABLE_VS_SHOP_SEARCH
		int			SearchItemList(LPCHARACTER ch, const char* c_pData, size_t uiBytes);
#endif
#ifdef ENABLE_DROP_INFO
		void		DropInfo(LPCHARACTER ch);
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		void		Acce(LPCHARACTER pkChar, const char* c_pData);
#endif
#ifdef ENABLE_AURA_SYSTEM
		void		Aura(LPCHARACTER pkChar, const char* c_pData);
#endif

#ifdef ENABLE_NEW_PET_SYSTEM
		void		RecivePetAction(LPCHARACTER ch, const char* c_pData);
#endif
#ifdef ENABLE_SWITCHBOT
		int			Switchbot(LPCHARACTER ch, const char* data, size_t uiBytes);
#endif
#ifdef RANKING_SYSTEM
		void		LoadServerRanking(LPCHARACTER ch, const char* data);
		void		LoadRanking(LPCHARACTER ch, const char* data);
#endif
#ifdef ENABLE_SECONDARY_LEVEL
		int			SecondaryLevel(LPCHARACTER ch, const void* c_pData, size_t uiBytes);
#endif
		void		Roulette(LPCHARACTER ch, const char* c_pData);

#if defined(__DUNGEON_INFO_SYSTEM__)
		void 		DungeonInfo(LPCHARACTER ch, const char* c_pData);
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
		int			ReciveBuffNPCCreate(LPCHARACTER ch, const char* c_pData);
		int			ReciveBuffNPCAction(LPCHARACTER ch, const char* c_pData);
#endif
// #ifdef ENABLE_MOUNT_COSTUME_SYSTEM
// 		int			ReciveMountNPCAction(LPCHARACTER ch, const char* c_pData);
// #endif
#ifdef __PREMIUM_PRIVATE_SHOP__
		int			PrivateShopBuild(LPCHARACTER ch, const char* c_pData, size_t uiBytes);
		void		PrivateShopClose(LPCHARACTER ch);
		void		PrivateShopPanelOpen(LPCHARACTER ch, bool bCheck = false);
		void		PrivateShopPanelClose(LPCHARACTER ch);
		int			PrivateShopStart(LPCHARACTER ch, const char* c_pData, size_t uiBytes);
		void		PrivateShopEnd(LPCHARACTER ch);
		int			PrivateShopBuy(LPCHARACTER ch, const char* c_pData, size_t uiBytes);
		void		PrivateShopWithdraw(LPCHARACTER ch);
		void		PrivateShopModify(LPCHARACTER ch);
		int			PrivateShopItemPriceChange(LPCHARACTER ch, const char* c_pData, size_t uiBytes);
		int			PrivateShopItemMove(LPCHARACTER ch, const char* c_pData, size_t uiBytes);
		int			PrivateShopItemCheckin(LPCHARACTER ch, const char* c_pData, size_t uiBytes);
		int			PrivateShopItemCheckout(LPCHARACTER ch, const char* c_pData, size_t uiBytes);
		int			PrivateShopTitleChange(LPCHARACTER ch, const char* c_pData, size_t uiBytes);
		void		PrivateShopWarpRequest(LPCHARACTER ch);
		int			PrivateShopSlotUnlockRequest(LPCHARACTER ch, const char* c_pData, size_t uiBytes);

		void		PrivateShopSearchClose(LPCHARACTER ch);
		int			PrivateShopSearch(LPCHARACTER ch, const char* c_pData, size_t uiBytes);
		int			PrivateShopSearchBuy(LPCHARACTER ch, const char* c_pData, size_t uiBytes);

		void		PrivateShopMarketItemPriceDataRequest(LPCHARACTER ch);
		int			PrivateShopMarketItemPriceRequest(LPCHARACTER ch, const char* c_pData, size_t uiBytes);
#endif
#ifdef ENABLE_IN_GAME_LOG_SYSTEM
		int			InGameLog(LPCHARACTER ch, const char* data, UINT uiBytes);
#endif
#ifdef ENABLE_EVENT_MANAGER
		void		EventActivate(LPCHARACTER ch, const char* c_pData);
#endif
};

class CInputDead : public CInputMain
{
	public:
		virtual BYTE	GetType() { return INPROC_DEAD; }

	protected:
		virtual int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData);
};

class CInputDB : public CInputProcessor
{
public:
	virtual bool Process(LPDESC d, const void * c_pvOrig, int iBytes, int & r_iBytesProceed);
	virtual BYTE GetType() { return INPROC_DB; }

protected:
	virtual int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData);

protected:

	void		MapLocations(const char * c_pData);
	void		LoginSuccess(DWORD dwHandle, const char *data);
	void		PlayerCreateFailure(LPDESC d, BYTE bType);
	void		PlayerDeleteSuccess(LPDESC d, const char * data);
	void		PlayerDeleteFail(LPDESC d);
	void		PlayerLoad(LPDESC d, const char* data);
	void		PlayerCreateSuccess(LPDESC d, const char * data);
	void		Boot(const char* data);
	void		QuestLoad(LPDESC d, const char * c_pData);
	void		SafeboxLoad(LPDESC d, const char * c_pData);
#if defined(__BL_MOVE_CHANNEL__)
	void		MoveChannelRespond(LPDESC d, const char* c_pData);
#endif
	void		SafeboxChangeSize(LPDESC d, const char * c_pData);
	void		SafeboxWrongPassword(LPDESC d);
	void		SafeboxChangePasswordAnswer(LPDESC d, const char* c_pData);
	void		MallLoad(LPDESC d, const char * c_pData);
	void		EmpireSelect(LPDESC d, const char * c_pData);
	void		P2P(const char * c_pData);
	void		ItemLoad(LPDESC d, const char * c_pData);
	void		AffectLoad(LPDESC d, const char * c_pData);
	void		GuildLoad(const char * c_pData);
	void		GuildSkillUpdate(const char* c_pData);
	void		GuildSkillRecharge();
	void		GuildExpUpdate(const char* c_pData);
	void		GuildAddMember(const char* c_pData);
	void		GuildRemoveMember(const char* c_pData);
	void		GuildChangeGrade(const char* c_pData);
	void		GuildChangeMemberData(const char* c_pData);
	void		GuildDisband(const char* c_pData);
	void		GuildLadder(const char* c_pData);
	void		GuildWar(const char* c_pData);
	void		GuildWarScore(const char* c_pData);
	void		GuildSkillUsableChange(const char* c_pData);
	void		GuildMoneyChange(const char* c_pData);
	void		GuildWithdrawMoney(const char* c_pData);
	void		GuildWarReserveAdd(TGuildWarReserve * p);
	void		GuildWarReserveUpdate(TGuildWarReserve * p);
	void		GuildWarReserveDelete(DWORD dwID);
	void		GuildWarBet(TPacketGDGuildWarBet * p);
	void		GuildChangeMaster(TPacketChangeGuildMaster* p);
	void		GuildRefreshNote(const char* c_pData);
	void		GuildNoticeDungeonOpen(const char* c_pData);
	void		GuildNoticeDungeonEnd(const char* c_pData);
	void		GuildNoticeTicketsChanged(const char* c_pData);
	void		GuildUpdateGuardItemData(const char* c_pData);
	void		GuildGuardUpdateActiveSetData(const char* c_pData);

	void		LoginAlready(LPDESC d, const char * c_pData);

	void		PartyCreate(const char* c_pData);
	void		PartyDelete(const char* c_pData);
	void		PartyAdd(const char* c_pData);
	void		PartyRemove(const char* c_pData);
	void		PartyStateChange(const char* c_pData);
	void		PartySetMemberLevel(const char* c_pData);

	void		Time(const char * c_pData);

	void		ReloadProto(const char * c_pData);
	void		ChangeName(LPDESC d, const char * data);
#ifdef __ENABLE_DAMAGE_RESTRICTIONS__
	void		ReloadMonsterRestrictions(const char* c_pData);
#endif

	void		AuthLogin(LPDESC d, const char * c_pData);
	void		ItemAward(const char * c_pData);

	void		ChangeEmpirePriv(const char* c_pData);
	void		ChangeGuildPriv(const char* c_pData);
	void		ChangeCharacterPriv(const char* c_pData);

	void		MoneyLog(const char* c_pData);

	void		SetEventFlag(const char* c_pData);

	void		BillingRepair(const char * c_pData);
	void		BillingExpire(const char * c_pData);
	void		BillingLogin(const char * c_pData);
	void		BillingCheck(const char * c_pData);
	void		VCard(const char * c_pData);

	void		CreateObject(const char * c_pData);
	void		DeleteObject(const char * c_pData);
	void		UpdateLand(const char * c_pData);

	void		Notice(const char * c_pData);

	void		MarriageAdd(TPacketMarriageAdd * p);
	void		MarriageUpdate(TPacketMarriageUpdate * p);
	void		MarriageRemove(TPacketMarriageRemove * p);

	void		WeddingRequest(TPacketWeddingRequest* p);
	void		WeddingReady(TPacketWeddingReady* p);
	void		WeddingStart(TPacketWeddingStart* p);
	void		WeddingEnd(TPacketWeddingEnd* p);
#ifdef ENABLE_ITEMSHOP
	void		ItemShop(LPDESC d, const char* c_pData);
#endif

	void		TakeMonarchMoney(LPDESC d, const char * data );
	void		AddMonarchMoney(LPDESC d, const char * data );
	void		DecMonarchMoney(LPDESC d, const char * data );
	void		SetMonarch( LPDESC d, const char * data );

	void		ChangeMonarchLord(TPacketChangeMonarchLordACK* data);
	void		UpdateMonarchInfo(TMonarchInfo* data);

	// MYSHOP_PRICE_LIST

	void		MyshopPricelistRes( LPDESC d, const TPacketMyshopPricelistHeader* p );
	// END_OF_MYSHOP_PRICE_LIST

	//RELOAD_ADMIN
	void ReloadAdmin( const char * c_pData );
	//END_RELOAD_ADMIN
	void		UserCount(LPDESC d, const TPacketDGUserCount* p);
#ifdef ENABLE_EVENT_MANAGER
	void		EventManager(const char* c_pData);
#endif
	
	void		DetailLog(const TPacketNeedLoginLogInfo* info);

	void		ItemAwardInformer(TPacketItemAwardInfromer* data);
#if defined(BL_OFFLINE_MESSAGE)
	void		ReadOfflineMessages(LPDESC desc, const char* pcData);
#endif
	void		RespondChannelStatus(LPDESC desc, const char* pcData);
#ifdef __DUNGEON_INFO_ENABLE__
	void		DIHighscoreRecord(const char * c_pData);
#endif
#ifdef __PREMIUM_PRIVATE_SHOP__
	void		PrivateShop(LPDESC d, const char* c_pData);
#endif
#ifdef RANKING_SYSTEM
	void ServerRankingLoad(LPDESC d, const char* c_pData);
	void ServerRankingLoadMyPos(LPDESC d, const char* c_pData);

	void RankingLoad(LPDESC d, const char* c_pData);
	void RankingLoadMyPos(LPDESC d, const char* c_pData);
	void RankingLoadDataStatus(const char* c_pData);
#endif
	void FishVnumRankingLoad(LPDESC d, const char* c_pData);

	protected:
		DWORD		m_dwHandle;
};

class CInputP2P : public CInputProcessor
{
	public:
		CInputP2P();
		virtual BYTE	GetType() { return INPROC_P2P; }

	protected:
		virtual int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData);

	public:
		void		Setup(LPDESC d, const char * c_pData);
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		void		RemoveOfflineShop(LPDESC d, const char * c_pData);
		void		OfflineShopBuy(LPDESC d, const char * c_pData);
#endif

		void		Login(LPDESC d, const char * c_pData);
		void		Logout(LPDESC d, const char * c_pData);
		int			Relay(LPDESC d, const char * c_pData, size_t uiBytes);
#ifdef ENABLE_FULL_NOTICE
		int			Notice(LPDESC d, const char * c_pData, size_t uiBytes, bool bBigFont=false);
#else
		int			Notice(LPDESC d, const char * c_pData, size_t uiBytes);
#endif
#ifdef __WORLD_BOSS_YUMA__
		int			NewNotice(LPDESC d, const char* c_pData, size_t uiBytes);
#endif
		int			MonarchNotice(LPDESC d, const char * c_pData, size_t uiBytes);
		int			MonarchTransfer(LPDESC d, const char * c_pData);
		int			Guild(LPDESC d, const char* c_pData, size_t uiBytes);
		void		Shout(const char * c_pData);
		void		Disconnect(const char * c_pData);
		void		MessengerAdd(const char * c_pData);
		void		MessengerRemove(const char * c_pData);
		void		MessengerMobile(const char * c_pData);
		void		FindPosition(LPDESC d, const char* c_pData);
		void		WarpCharacter(const char* c_pData);
		void		GuildWarZoneMapIndex(const char* c_pData);
		void		Transfer(const char * c_pData);
		void		XmasWarpSanta(const char * c_pData);
		void		XmasWarpSantaReply(const char * c_pData);
		void		LoginPing(LPDESC d, const char * c_pData);
		void		DisconnectPlayer(const char* c_pData);
		void		BlockChat(const char * c_pData);
		void		IamAwake(LPDESC d, const char * c_pData);
#ifdef ENABLE_MULTI_FARM_BLOCK
		void		MultiFarm(const char* c_pData);
#endif
#ifdef ENABLE_SWITCHBOT
		void		Switchbot(LPDESC d, const char* c_pData);
#endif
#ifdef __PREMIUM_PRIVATE_SHOP__
		void		PrivateShopItemSearchUpdate(const char* c_pData);
		void		PrivateShopItemSearch(const char* c_pData);
		int			PrivateShopItemSearchResult(const char* c_pData, size_t uiBytes);
#endif
#ifdef __CROSS_CHANNEL_DUNGEON_WARP__
		void		CreateDungeonInstance(LPDESC d, const char * c_pData);
		#ifdef __DUNGEON_RETURN_ENABLE__
		void		RejoinDungeon(LPDESC d, const char * c_pData);
		void		CanRejoinDungeon(LPDESC d, const char * c_pData);
		#endif
#endif
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
		int			CmdchatToAll(LPDESC d, const char* c_pData, size_t uiBytes);
#endif

#ifdef ENABLE_MESSENGER_BLOCK
		void		MessengerBlockAdd(const char* c_pData);
		void		MessengerBlockRemove(const char* c_pData);
#endif

		void Reload(const TPacketGGReload& packet) const;

	protected:
		static CPacketInfoGG 	m_packetInfoGG;
};

class CInputAuth : public CInputProcessor
{
	public:
		CInputAuth();
		virtual BYTE GetType() { return INPROC_AUTH; }

	protected:
		virtual int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData);

	public:
		void		Login(LPDESC d, const char * c_pData);
};

#endif /* __INC_METIN_II_GAME_INPUT_PROCESSOR__ */
//martysama0134's ceqyqttoaf71vasf9t71218
