#ifndef __INC_CLIENTMANAGER_H__
#define __INC_CLIENTMANAGER_H__

#include <unordered_map>
#include <unordered_set>

#include "../../common/stl.h"
#include "../../common/building.h"
#ifdef ENABLE_OFFLINE_SHOP
#include "OfflineshopCache.h"
#endif
#ifdef ENABLE_IN_GAME_LOG_SYSTEM
#include "InGameLog.h"
#endif

#include "Peer.h"
#include "DBManager.h"
#include "LoginData.h"

#include <chrono>

#define ENABLE_PROTO_FROM_DB

class CPlayerTableCache;
class CItemCache;
class CItemPriceListTableCache;

#ifdef __PREMIUM_PRIVATE_SHOP__
#include "PrivateShop.h"
class CPrivateShop;
class CPrivateShopCache;
class CPrivateShopItemCache;
class CPrivateShopSaleCache;
#endif

class CPacketInfo
{
    public:
	void Add(int header);
	void Reset();

	std::map<int, int> m_map_info;
};

size_t CreatePlayerSaveQuery(char * pszQuery, size_t querySize, TPlayerTable * pkTab);

class CClientManager : public CNetBase, public singleton<CClientManager>
{
    public:
	typedef std::list<CPeer *>			TPeerList;
	typedef std::unordered_map<DWORD, CPlayerTableCache *> TPlayerTableCacheMap;
	typedef std::unordered_map<DWORD, CItemCache *> TItemCacheMap;
	typedef std::unordered_set<CItemCache * > TItemCacheSet;
	typedef std::unordered_map<DWORD, TItemCacheSet *> TItemCacheSetPtrMap;
	typedef std::unordered_map<DWORD, CItemPriceListTableCache*> TItemPriceListCacheMap;
	typedef std::unordered_map<short, BYTE> TChannelStatusMap;

#ifdef __PREMIUM_PRIVATE_SHOP__
	typedef std::unordered_map<DWORD, std::unique_ptr<CPrivateShopCache> > TPrivateShopCacheMap;
	typedef std::unordered_map<DWORD, std::unique_ptr<CPrivateShop> > TPrivateShopMap;

	typedef std::unordered_map<DWORD, std::unique_ptr<CPrivateShopItemCache> > TPrivateShopItemCacheMap;
	typedef std::unordered_set<CPrivateShopItemCache*, std::hash<CPrivateShopItemCache*> > TPrivateShopItemCacheSet;
	typedef std::unordered_map<DWORD, std::unique_ptr<TPrivateShopItemCacheSet> > TPrivateShopItemCacheSetPtrMap;

	typedef std::unordered_map<DWORD, std::unique_ptr<CPrivateShopSaleCache> > TPrivateShopSaleCacheMap;
	typedef std::unordered_set<CPrivateShopSaleCache*, std::hash<CPrivateShopSaleCache*> > TPrivateShopSaleCacheSet;
	typedef std::unordered_map<DWORD, std::unique_ptr<TPrivateShopSaleCacheSet> > TPrivateShopSaleCacheSetPtrMap;

	typedef std::list<TItemPrice>								TMarketItemPriceList;
	typedef std::unordered_map<DWORD, TMarketItemPriceList>	TMarketItemPriceListMap;
	typedef std::unordered_map<DWORD, TItemPrice>				TMarketItemPriceMap;

	typedef std::list<CPrivateShop*>							TPrivateShopPtrList;
#endif
	// MYSHOP_PRICE_LIST

	typedef std::pair< DWORD, DWORD >		TItemPricelistReqInfo;
	// END_OF_MYSHOP_PRICE_LIST

	class ClientHandleInfo
	{
	    public:
		DWORD	dwHandle;
		DWORD	account_id;
		DWORD	player_id;
		BYTE	account_index;
		char	login[LOGIN_MAX_LEN + 1];
		char	safebox_password[SAFEBOX_PASSWORD_MAX_LEN + 1];
		char	ip[MAX_HOST_LENGTH + 1];

		TAccountTable * pAccountTable;
		TSafeboxTable * pSafebox;

		ClientHandleInfo(DWORD argHandle, DWORD dwPID = 0)
		{
		    dwHandle = argHandle;
		    pSafebox = NULL;
		    pAccountTable = NULL;
		    player_id = dwPID;
		};

		ClientHandleInfo(DWORD argHandle, DWORD dwPID, DWORD accountId)
		{
		    dwHandle = argHandle;
		    pSafebox = NULL;
		    pAccountTable = NULL;
		    player_id = dwPID;
			account_id = accountId;
		};

		~ClientHandleInfo()
		{
		    if (pSafebox)
			{
				delete pSafebox;
				pSafebox = NULL;
			}
		}
	};

	#ifdef ENABLE_OFFLINE_SHOP
	public:
		bool InitializeOfflineshopTable();
		bool RecvOfflineShopBuyItemPacket(const char* data);
		bool RecvOfflineShopLockBuyItem(CPeer* peer, const char* data);
		bool RecvOfflineShopCannotBuyLockItem(const char* data);
		bool RecvOfflineShopEditItemPacket(const char* data);
		bool RecvOfflineShopRemoveItemPacket(const char* data);
		bool RecvOfflineShopAddItemPacket(const char* data);
		bool RecvOfflineShopForceClose(const char* data);
		bool RecvOfflineShopCreateNew(const char* data);
		bool RecvOfflineShopChangeName(const char* data);
		bool RecvOfflineShopSafeboxGetItem(const char* data);
		bool RecvOfflineShopSafeboxGetValutes(const char* data);
		bool RecvOfflineShopSafeboxAddItem(const char* data);
	#ifdef ENABLE_OFFLINESHOP_EXTEND_TIME
		bool RecvOfflineShopExtendTime(const char* data);
		bool SendOfflineShopExtendTime(DWORD dwOwnerID, DWORD dwTime);
	#endif
		bool SendOfflineShopBuyItemPacket(DWORD dwOwner, DWORD dwGuest, DWORD dwItem);
		bool SendOfflineShopBuyLockedItemPacket(CPeer* peer, DWORD dwOwner, DWORD dwGuest, DWORD dwItem);
		bool SendOfflineShopEditItemPacket(DWORD dwOwner, DWORD dwItem, const offlineshop::TPriceInfo& price);
		bool SendOfflineShopRemoveItemPacket(DWORD dwOwner, DWORD dwItem);
		bool SendOfflineShopAddItemPacket(DWORD dwOwner, DWORD dwItemID, const offlineshop::TItemInfo& rInfo);
		bool SendOfflineShopForceClose(DWORD dwOwnerID);
	#ifdef ENABLE_IRA_REWORK
		bool SendOfflineShopCreateNew(const offlineshop::TShopInfo& shop, const std::vector<offlineshop::TItemInfo>& vec, offlineshop::TShopPosition& pos);
	#else
		bool SendOfflineShopCreateNew(const offlineshop::TShopInfo& shop, const std::vector<offlineshop::TItemInfo>& vec);
	#endif
		bool SendOfflineShopChangeName(DWORD dwOwnerID, const char* szName);
		bool SendOfflineshopShopExpired(DWORD dwOwnerID);
		void SendOfflineshopTable(CPeer* peer);
		void RecvOfflineShopPacket(CPeer* peer, const char* data);
		void SendOfflineshopSafeboxAddItem(DWORD dwOwnerID, DWORD dwItem, const offlineshop::TItemInfoEx& item);
		void SendOfflineshopSafeboxAddValutes(DWORD dwOwnerID, const uint64_t& valute);
		void SendOfflineshopSafeboxLoad(CPeer* peer, DWORD dwOwnerID, const uint64_t& valute, const std::vector<offlineshop::TItemInfoEx>& items, const std::vector<DWORD>& ids);
		void SendOfflineshopSafeboxExpiredItem(DWORD dwOwnerID, DWORD itemID);
		void OfflineShopResultQuery(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo);
		void OfflineShopResultAddItemQuery(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo);
		void OfflineShopResultCreateShopQuery(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo);
		void OfflineShopResultCreateShopAddItemQuery(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo);
		void OfflineShopResultSafeboxAddItemQuery(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo);
		void OfflineshopDurationProcess();
		void OfflineshopExpiredShop(DWORD dwID);
		void OfflineshopLoadShopSafebox(CPeer* peer, DWORD dwID);
		bool IsUsingOfflineshopSystem(DWORD dwID);
	private:
		offlineshop::CShopCache m_offlineshopShopCache;
		offlineshop::CSafeboxCache m_offlineshopSafeboxCache;
	#endif

	public:
	CClientManager();
	~CClientManager();

	bool	Initialize();
	time_t	GetCurrentTime();

	void	MainLoop();
	void	Quit();

	void	GetPeerP2PHostNames(std::string& peerHostNames);
	void	SetTablePostfix(const char* c_pszTablePostfix);
	void	SetPlayerIDStart(int iIDStart);
	int		GetPlayerIDStart() { return m_iPlayerIDStart; }

	int		GetPlayerDeleteLevelLimit() { return m_iPlayerDeleteLevelLimit; }

	void	SetChinaEventServer(bool flag) { m_bChinaEventServer = flag; }
	bool	IsChinaEventServer() { return m_bChinaEventServer; }

	DWORD	GetUserCount();

	void	SendAllGuildSkillRechargePacket();
	void	SendTime();

	CPlayerTableCache *	GetPlayerCache(DWORD id);
	void			PutPlayerCache(TPlayerTable * pNew);

	void			CreateItemCacheSet(DWORD dwID);
	TItemCacheSet *		GetItemCacheSet(DWORD dwID);
	void			FlushItemCacheSet(DWORD dwID);

	CItemCache *		GetItemCache(DWORD id);
	void			PutItemCache(TPlayerItem * pNew, bool bSkipQuery = false);
	bool			DeleteItemCache(DWORD id);

	void			UpdatePlayerCache();
	void			UpdateItemCache();

	// MYSHOP_PRICE_LIST

	CItemPriceListTableCache*	GetItemPriceListCache(DWORD dwID);

	void			PutItemPriceListCache(const TItemPriceListTable* pItemPriceList);

	void			UpdateItemPriceListCache(void);
	// END_OF_MYSHOP_PRICE_LIST

	void			SendGuildSkillUsable(DWORD guild_id, DWORD dwSkillVnum, bool bUsable);

	void			SetCacheFlushCountLimit(int iLimit);

	template <class Func>
	Func		for_each_peer(Func f);

	CPeer *		GetAnyPeer();

	void			ForwardPacket(BYTE header, const void* data, int size, BYTE bChannel = 0, CPeer * except = NULL);

	void			SendNotice(const char * c_pszFormat, ...);

	// @fixme203 directly GetCommand instead of strcpy
	char*			GetCommand(char* str, char* command);
	void			ItemAward(CPeer * peer, char* login);

    protected:
	void	Destroy();

    private:
	bool		InitializeTables(bool bReload = false);
	bool		InitializeShopTable();
	bool		InitializeMobTable();
	bool		InitializeItemTable();
	bool		InitializeQuestItemTable();
	bool		InitializeSkillTable();
	bool		InitializeRefineTable();
	bool		InitializeBanwordTable();
	bool		InitializeItemAttrTable();
	bool		InitializeItemRareTable();
#ifdef ENABLE_BELT_ATTRIBUTES
	bool		InitializeBeltAttrTable();
#endif
#ifdef ENABLE_GLOVE_ATTRIBUTES
	bool		InitializeGloveAttrTable();
#endif
#ifdef ENABLE_TALISMAN_ATTRIBUTES
	bool		InitializeTalismanAttrTable();
#endif
#ifdef ENABLE_TALISMAN_ATTRIBUTES_RARE
	bool		InitializeTalismanAttrRareTable();
#endif
	bool		InitializeLandTable();
	bool		InitializeObjectProto();
	bool		InitializeObjectTable();

	bool		InitializeMonarch();
#ifdef __ENABLE_DAMAGE_RESTRICTIONS__
	bool		InitializeDamageRestriction();
#endif
#ifdef __ENABLE_POLYMORPH_SYSTEM__
	bool		InitializePolymorphData();
#endif

	bool		MirrorMobTableIntoDB();
	bool		MirrorItemTableIntoDB();

	void		AddPeer(socket_t fd);
	void		RemovePeer(CPeer * pPeer);
	CPeer *		GetPeer(IDENT ident);

	int		AnalyzeQueryResult(SQLMsg * msg);
	int		AnalyzeErrorMsg(CPeer * peer, SQLMsg * msg);

	int		Process();

        void            ProcessPackets(CPeer * peer);

	CLoginData *	GetLoginData(DWORD dwKey);
	CLoginData *	GetLoginDataByLogin(const char * c_pszLogin);
	CLoginData *	GetLoginDataByAID(DWORD dwAID);

	void		InsertLoginData(CLoginData * pkLD);
	void		DeleteLoginData(CLoginData * pkLD);

	bool		InsertLogonAccount(const char * c_pszLogin, DWORD dwHandle, const char * c_pszIP);
	bool		DeleteLogonAccount(const char * c_pszLogin, DWORD dwHandle);
	bool		FindLogonAccount(const char * c_pszLogin);

	void		GuildCreate(CPeer * peer, DWORD dwGuildID);
	void		GuildSkillUpdate(CPeer * peer, TPacketGuildSkillUpdate* p);
	void		GuildExpUpdate(CPeer * peer, TPacketGuildExpUpdate* p);
	void		GuildAddMember(CPeer * peer, TPacketGDGuildAddMember* p);
	void		GuildChangeGrade(CPeer * peer, TPacketGuild* p);
	void		GuildRemoveMember(CPeer * peer, TPacketGuild* p);
	void		GuildChangeMemberData(CPeer * peer, TPacketGuildChangeMemberData* p);
	void		GuildDisband(CPeer * peer, TPacketGuild * p);
	void		GuildWar(CPeer * peer, TPacketGuildWar * p);
	void		GuildWarScore(CPeer * peer, TPacketGuildWarScore * p);
	void		GuildChangeLadderPoint(TPacketGuildLadderPoint* p);
	void		GuildUseSkill(TPacketGuildUseSkill* p);
	void		GuildDepositMoney(TPacketGDGuildMoney* p);
	void		GuildWithdrawMoney(CPeer* peer, TPacketGDGuildMoney* p);
	void		GuildWithdrawMoneyGiveReply(TPacketGDGuildMoneyWithdrawGiveReply* p);
	void		GuildWarBet(TPacketGDGuildWarBet * p);
	void		GuildChangeMaster(TPacketChangeGuildMaster* p);
	void		GuildRefreshNote(CPeer* peer, DWORD dwGuildID);
	void		GuildNoticeDungeonOpen(CPeer* peer, TPacketGDGuildNoticeDungeonOpened* p);
	void		GuildNoticeDungeonEnd(CPeer* peer, TPacketGDGuildNoticeDungeonEnd* p);
	void		GuildNoticeTicketsChanged(CPeer* peer, TPacketGDGuildNoticeTicketsChanged* p);
	void		GuildGuardUpdateItemData(CPeer* peer, TPacketGDGuildUpdateGuardItemData* p);
	void		GuildUpdateGuardActiveSetData(CPeer* peer, TPacketGuildUpdateGuardActiveSetData* p);
	void		GuildChangeMoney(CPeer* peer, TPacketGDGuildMoney* p);

	void		SetGuildWarEndTime(DWORD guild_id1, DWORD guild_id2, time_t tEndTime);

	void		QUERY_BOOT(CPeer * peer, TPacketGDBoot * p);

	void		QUERY_LOGIN(CPeer * peer, DWORD dwHandle, SLoginPacket* data);
	void		QUERY_LOGOUT(CPeer * peer, DWORD dwHandle, const char *);

	void		RESULT_LOGIN(CPeer * peer, SQLMsg *msg);

	void		QUERY_PLAYER_LOAD(CPeer * peer, DWORD dwHandle, TPlayerLoadPacket*);
	void		RESULT_COMPOSITE_PLAYER(CPeer * peer, SQLMsg * pMsg, DWORD dwQID);
	void		RESULT_PLAYER_LOAD(CPeer * peer, MYSQL_RES * pRes, ClientHandleInfo * pkInfo);
	void		RESULT_ITEM_LOAD(CPeer * peer, MYSQL_RES * pRes, DWORD dwHandle, DWORD dwPID);
	void		RESULT_QUEST_LOAD(CPeer * pkPeer, MYSQL_RES * pRes, DWORD dwHandle, DWORD dwPID);
	// @fixme402 (RESULT_AFFECT_LOAD +dwRealPID)
	void		RESULT_AFFECT_LOAD(CPeer * pkPeer, MYSQL_RES * pRes, DWORD dwHandle, DWORD dwRealPID);

	// PLAYER_INDEX_CREATE_BUG_FIX
	void		RESULT_PLAYER_INDEX_CREATE(CPeer *pkPeer, SQLMsg *msg);
	// END_PLAYER_INDEX_CREATE_BUG_FIX

	// MYSHOP_PRICE_LIST

	void		RESULT_PRICELIST_LOAD(CPeer* peer, SQLMsg* pMsg);

	void		RESULT_PRICELIST_LOAD_FOR_UPDATE(SQLMsg* pMsg);
	// END_OF_MYSHOP_PRICE_LIST

	void		QUERY_PLAYER_SAVE(CPeer * peer, DWORD dwHandle, TPlayerTable*);

	void		__QUERY_PLAYER_CREATE(CPeer * peer, DWORD dwHandle, TPlayerCreatePacket *);
	void		__QUERY_PLAYER_DELETE(CPeer * peer, DWORD dwHandle, TPlayerDeletePacket *);
	void		__RESULT_PLAYER_DELETE(CPeer * peer, SQLMsg* msg);

	void		QUERY_PLAYER_COUNT(CPeer * pkPeer, TPlayerCountPacket *);

	void		QUERY_ITEM_SAVE(CPeer * pkPeer, const char * c_pData);
	void		QUERY_ITEM_DESTROY(CPeer * pkPeer, const char * c_pData);
	void		QUERY_ITEM_FLUSH(CPeer * pkPeer, const char * c_pData);

	void		QUERY_QUEST_SAVE(CPeer * pkPeer, TQuestTable *, DWORD dwLen);
	void		QUERY_ADD_AFFECT(CPeer * pkPeer, TPacketGDAddAffect * p);
	void		QUERY_REMOVE_AFFECT(CPeer * pkPeer, TPacketGDRemoveAffect * p);

	void		QUERY_SAFEBOX_LOAD(CPeer * pkPeer, DWORD dwHandle, TSafeboxLoadPacket *, bool bMall);
	void		QUERY_SAFEBOX_SAVE(CPeer * pkPeer, TSafeboxTable * pTable);
	void		QUERY_SAFEBOX_CHANGE_SIZE(CPeer * pkPeer, DWORD dwHandle, TSafeboxChangeSizePacket * p);
	void		QUERY_SAFEBOX_CHANGE_PASSWORD(CPeer * pkPeer, DWORD dwHandle, TSafeboxChangePasswordPacket * p);
#if defined(__BL_MOVE_CHANNEL__)
	void		QUERY_MOVE_CHANNEL(CPeer* pkPeer, DWORD dwHandle, TMoveChannel*);
#endif
	void		RESULT_SAFEBOX_LOAD(CPeer * pkPeer, SQLMsg * msg);
	void		RESULT_SAFEBOX_CHANGE_SIZE(CPeer * pkPeer, SQLMsg * msg);
	void		RESULT_SAFEBOX_CHANGE_PASSWORD(CPeer * pkPeer, SQLMsg * msg);
	void		RESULT_SAFEBOX_CHANGE_PASSWORD_SECOND(CPeer * pkPeer, SQLMsg * msg);

	void		QUERY_EMPIRE_SELECT(CPeer * pkPeer, DWORD dwHandle, TEmpireSelectPacket * p);
	void		QUERY_SETUP(CPeer * pkPeer, DWORD dwHandle, const char * c_pData);

	void		SendPartyOnSetup(CPeer * peer);

	void		QUERY_HIGHSCORE_REGISTER(CPeer * peer, TPacketGDHighscore* data);
	void		RESULT_HIGHSCORE_REGISTER(CPeer * pkPeer, SQLMsg * msg);

	void		QUERY_FLUSH_CACHE(CPeer * pkPeer, const char * c_pData);

	void		QUERY_PARTY_CREATE(CPeer * peer, TPacketPartyCreate* p);
	void		QUERY_PARTY_DELETE(CPeer * peer, TPacketPartyDelete* p);
	void		QUERY_PARTY_ADD(CPeer * peer, TPacketPartyAdd* p);
	void		QUERY_PARTY_REMOVE(CPeer * peer, TPacketPartyRemove* p);
	void		QUERY_PARTY_STATE_CHANGE(CPeer * peer, TPacketPartyStateChange* p);
	void		QUERY_PARTY_SET_MEMBER_LEVEL(CPeer * peer, TPacketPartySetMemberLevel* p);

	void		QUERY_RELOAD_PROTO();
#ifdef __ENABLE_DAMAGE_RESTRICTIONS__
	void		QUERY_RELOAD_DAMAGE_RESTRICTIONS();
#endif

	void		QUERY_CHANGE_NAME(CPeer * peer, DWORD dwHandle, TPacketGDChangeName * p);
	void		GetPlayerFromRes(TPlayerTable * player_table, MYSQL_RES* res);

	void		QUERY_SMS(CPeer * pkPeer, TPacketGDSMS * p);
	void		QUERY_LOGIN_KEY(CPeer * pkPeer, TPacketGDLoginKey * p);

	void		AddGuildPriv(TPacketGiveGuildPriv* p);
	void		AddEmpirePriv(TPacketGiveEmpirePriv* p);
	void		AddCharacterPriv(TPacketGiveCharacterPriv* p);

	void		MoneyLog(TPacketMoneyLog* p);

	void		QUERY_AUTH_LOGIN(CPeer * pkPeer, DWORD dwHandle, TPacketGDAuthLogin * p);

	void		QUERY_LOGIN_BY_KEY(CPeer * pkPeer, DWORD dwHandle, TPacketGDLoginByKey * p);
	void		RESULT_LOGIN_BY_KEY(CPeer * peer, SQLMsg * msg);

	void		ChargeCash(const TRequestChargeCash * p);

	void		LoadEventFlag();
	void		SetEventFlag(TPacketSetEventFlag* p);
	void		SendEventFlagsOnSetup(CPeer* peer);

	void		BillingExpire(TPacketBillingExpire * p);
	void		BillingCheck(const char * data);

	void		SendAllLoginToBilling();
	void		SendLoginToBilling(CLoginData * pkLD, bool bLogin);

	void		MarriageAdd(TPacketMarriageAdd * p);
	void		MarriageUpdate(TPacketMarriageUpdate * p);
	void		MarriageRemove(TPacketMarriageRemove * p);

	void		WeddingRequest(TPacketWeddingRequest * p);
	void		WeddingReady(TPacketWeddingReady * p);
	void		WeddingEnd(TPacketWeddingEnd * p);

#if defined(BL_OFFLINE_MESSAGE)
	void		RequestReadOfflineMessages(CPeer* pkPeer, DWORD dwHandle, TPacketGDReadOfflineMessage* p);
	void		SendOfflineMessage(TPacketGDSendOfflineMessage* p);
	void		OfflineMessageGarbage();
#endif

	// MYSHOP_PRICE_LIST

	void		MyshopPricelistUpdate(const TItemPriceListTable* pPacket); // @fixme403 (TPacketMyshopPricelistHeader to TItemPriceListTable)

	void		MyshopPricelistRequest(CPeer* peer, DWORD dwHandle, DWORD dwPlayerID);
	// END_OF_MYSHOP_PRICE_LIST

	// Building
	void		CreateObject(TPacketGDCreateObject * p);
	void		DeleteObject(DWORD dwID);
	void		UpdateLand(DWORD * pdw);

	// VCard
	void 		VCard(TPacketGDVCard * p);
	void		VCardProcess();

	// BLOCK_CHAT
	void		BlockChat(TPacketBlockChat * p);
	// END_OF_BLOCK_CHAT
#ifdef RANKING_SYSTEM
	void		__QUERY_LOAD_SERVER_RANKING(CPeer* peer, DWORD dwHandle, TLoadServerRanking* data);
	void		__QUERY_LOAD_SERVER_RANKING_MY_POS(CPeer* peer, DWORD dwHandle, TLoadServerRankingMyPos* data);
	TServerRankMyData GetMyServerRankPos(uint32_t pid, uint8_t catIdx);
	
	TRankMyData	GetMyRankPos(uint32_t pid, uint8_t rankType);
	void		__QUERY_LOAD_RANKING(CPeer* peer, DWORD dwHandle, TLoadRanking* data);
	void		__QUERY_LOAD_RANKING_MY_POS(CPeer* peer, DWORD dwHandle, TLoadRanking* data);
#endif
	void		__QUERY_LOAD_FISH_VNUM_RANKING(CPeer* peer, DWORD dwHandle, TLoadFishRanking* data);
	void		__QUERY_LOAD_GUILD_GUARD_RANKING(CPeer* peer, DWORD dwHandle);
    private:
	int					m_looping;
	socket_t				m_fdAccept;
	TPeerList				m_peerList;

	CPeer *					m_pkAuthPeer;

	// LoginKey, LoginData pair
	typedef std::unordered_map<DWORD, CLoginData *> TLoginDataByLoginKey;
	TLoginDataByLoginKey			m_map_pkLoginData;

	// Login LoginData pair
	typedef std::unordered_map<std::string, CLoginData *> TLoginDataByLogin;
	TLoginDataByLogin			m_map_pkLoginDataByLogin;

	// AccountID LoginData pair
	typedef std::unordered_map<DWORD, CLoginData *> TLoginDataByAID;
	TLoginDataByAID				m_map_pkLoginDataByAID;

	typedef std::unordered_map<std::string, CLoginData *> TLogonAccountMap;
	TLogonAccountMap			m_map_kLogonAccount;

	int					m_iPlayerIDStart;
	int					m_iPlayerDeleteLevelLimit;
	int					m_iPlayerDeleteLevelLimitLower;
	bool					m_bChinaEventServer;

	std::vector<TMobTable>			m_vec_mobTable;
	std::vector<TItemTable>			m_vec_itemTable;
	std::map<DWORD, TItemTable *>		m_map_itemTableByVnum;

	int					m_iShopTableSize;
	TShopTable *				m_pShopTable;

	int					m_iRefineTableSize;
	TRefineTable*				m_pRefineTable;

	std::vector<TSkillTable>		m_vec_skillTable;
	std::vector<TBanwordTable>		m_vec_banwordTable;
	std::vector<TItemAttrTable>		m_vec_itemAttrTable;
	std::vector<TItemAttrTable>		m_vec_itemRareTable;
#ifdef ENABLE_BELT_ATTRIBUTES
	std::vector<TItemAttrTable>		m_vec_beltAttrTable;
#endif
#ifdef ENABLE_GLOVE_ATTRIBUTES
	std::vector<TItemAttrTable>		m_vec_gloveAttrTable;
#endif
#ifdef ENABLE_TALISMAN_ATTRIBUTES
	std::vector<TItemAttrTable>		m_vec_talismanAttrTable;
#endif
#ifdef ENABLE_TALISMAN_ATTRIBUTES_RARE
	std::vector<TItemAttrTable>		m_vec_talismanAttrRareTable;
#endif
#ifdef RANKING_SYSTEM
	std::vector<TServerRankTable>		m_vec_serverRankCache[7];
	time_t								nextServerRankRefreshTime[7];
	std::map<std::pair<uint32_t, uint8_t>, TServerRankMyData>	m_serverRankPlayerPoses;
	
	time_t		rankingSundayEndTime;
	time_t		rankingNoMoreRankUpdatesStartTime;
	std::vector<TRankTable>			m_vec_rankCache[7];
	time_t							nextRankRefreshTime[7];
	std::map<std::pair<uint32_t, uint8_t>, TRankMyData>	m_rankPlayerPoses;
#endif

	std::map<uint32_t, std::vector<TFishVnumRankTable>>		m_vec_fishVnumRankCache;
	std::map<uint32_t, time_t>								m_nextFishVnumRankRefreshTime;

	std::vector<building::TLand>		m_vec_kLandTable;
	std::vector<building::TObjectProto>	m_vec_kObjectProto;
	std::map<DWORD, building::TObject *>	m_map_pkObjectTable;

#ifdef __ENABLE_POLYMORPH_SYSTEM__
	std::vector<TPolymorphStage> m_vec_PolyStages;
	std::vector<TPolymorphSkin> m_vec_PolySkins;
#endif
#ifdef __ENABLE_DAMAGE_RESTRICTIONS__
	std::vector<TMonsterRestriction> m_vec_MobDamageRestrictions;
#endif

	std::queue<TPacketGDVCard>		m_queue_vcard;

	bool					m_bShutdowned;

	TPlayerTableCacheMap			m_map_playerCache;

	TItemCacheMap				m_map_itemCache;
	TItemCacheSetPtrMap			m_map_pkItemCacheSetPtr;

	// MYSHOP_PRICE_LIST

	TItemPriceListCacheMap m_mapItemPriceListCache;
	// END_OF_MYSHOP_PRICE_LIST

	TChannelStatusMap m_mChannelStatus;

	struct TPartyInfo
	{
	    BYTE bRole;
	    BYTE bLevel;

		TPartyInfo() :bRole(0), bLevel(0)
		{
		}
	};

	typedef std::map<DWORD, TPartyInfo>	TPartyMember;
	typedef std::map<DWORD, TPartyMember>	TPartyMap;
	typedef std::map<BYTE, TPartyMap>	TPartyChannelMap;
	TPartyChannelMap m_map_pkChannelParty;

	typedef std::map<std::string, int32_t>	TEventFlagMap;
	TEventFlagMap m_map_lEventFlag;

#if defined(BL_OFFLINE_MESSAGE)
	struct SOfflineMessage
	{
		std::string From;
		std::string Message;
		std::chrono::system_clock::time_point t;
		SOfflineMessage(const char* szFrom, const char* szMessage)
			: From(szFrom), Message(szMessage), t(std::chrono::system_clock::now()) {}
	};
	std::unordered_map<std::string, std::vector<std::shared_ptr<SOfflineMessage>>> m_OfflineMessage;
#endif

	BYTE					m_bLastHeader;
	int					m_iCacheFlushCount;
	int					m_iCacheFlushCountLimit;

    private :
	TItemIDRangeTable m_itemRange;

    public :
	bool InitializeNowItemID();
	DWORD GetItemID();
	DWORD GainItemID();
	TItemIDRangeTable GetItemRange() { return m_itemRange; }

	//BOOT_LOCALIZATION
    public:

	bool InitializeLocalization();

#ifdef ENABLE_EVENT_MANAGER
	public:
		bool		InitializeEventManager(bool updateFromGameMaster = false);
		void		RecvEventManagerPacket(const char* data);
		void		UpdateEventManager();
		void		SendEventData(CPeer* pkPeer = NULL, bool updateFromGameMaster = false);
	protected:
		std::map<BYTE, std::vector<TEventManagerData>> m_EventManager;
#endif

#ifdef ENABLE_ITEMSHOP
protected:
	int itemShopUpdateTime;
	std::map<BYTE, std::map<BYTE, std::vector<TIShopData>>> m_IShopManager;
	std::map<DWORD, std::vector<TIShopLogData>> m_IShopLogManager;

public:
	bool InitializeItemShop();
	void SendItemShopData(CPeer* pkPeer = NULL, bool isPacket = false);
	void RecvItemShop(CPeer* pkPeer, DWORD dwHandle, const char* data);

#ifdef USE_ITEMSHOP_RENEWED
	void GetAccountMoney(DWORD id, long long& coins, long long& jcoins);
	void SetAccountMoney(DWORD id, long long coins, long long jcoins);
#else
	long long GetDragonCoin(DWORD id);
	void SetDragonCoin(DWORD id, long long amount);
#endif

	void ItemShopIncreaseSellCount(DWORD itemID, int itemCount);
	void SetEventFlag(const char* flag, int value);
	int GetEventFlag(const char* flag);
#endif

#ifdef RANKING_SYSTEM
	private:
		void	CheckRankingTimes();
		void	SendRankingDataStatusToAll(bool newEnabled, bool deleteAll);
		void	RankingReward();
	public:
		void	InitializeRanking(bool onStartup);
#endif

    private:
	std::vector<tLocale> m_vec_Locale;
	//END_BOOT_LOCALIZATION
	//ADMIN_MANAGER

	bool __GetAdminInfo(const char *szIP, std::vector<tAdminInfo> & rAdminVec);
	bool __GetHostInfo(std::vector<std::string> & rIPVec);
	//END_ADMIN_MANAGER

	//RELOAD_ADMIN
	void ReloadAdmin(CPeer * peer, TPacketReloadAdmin * p);
	//END_RELOAD_ADMIN
	void RequestUserCount(DWORD dwHandle);
	
	void BreakMarriage(CPeer * peer, const char * data);

	struct TLogoutPlayer
	{
	    DWORD	pid;
	    time_t	time;

	    bool operator < (const TLogoutPlayer & r)
	    {
		return (pid < r.pid);
	    }
	};

	typedef std::unordered_map<DWORD, TLogoutPlayer*> TLogoutPlayerMap;
	TLogoutPlayerMap m_map_logout;

	void InsertLogoutPlayer(DWORD pid);
	void DeleteLogoutPlayer(DWORD pid);
	void UpdateLogoutPlayer();
	void UpdateItemCacheSet(DWORD pid);

	void FlushPlayerCacheSet(DWORD pid);

	//MONARCH
	void Election(CPeer * peer, DWORD dwHandle, const char * p);
	void Candidacy(CPeer * peer, DWORD dwHandle, const char * p);
	void AddMonarchMoney(CPeer * peer, DWORD dwHandle, const char * p);
	void TakeMonarchMoney(CPeer * peer, DWORD dwHandle, const char * p);
	void ComeToVote(CPeer * peer, DWORD dwHandle, const char * p);
	void RMCandidacy(CPeer * peer, DWORD dwHandle, const char * p);
	void SetMonarch(CPeer * peer, DWORD dwHandle, const char * p);
	void RMMonarch(CPeer * peer, DWORD dwHandle, const char * p);

	void DecMonarchMoney(CPeer * peer, DWORD dwHandle, const char * p);
	//END_MONARCH

	void ChangeMonarchLord(CPeer* peer, DWORD dwHandle, TPacketChangeMonarchLord* info);

	void SendSpareItemIDRange(CPeer* peer);

	void UpdateHorseName(TPacketUpdateHorseName* data, CPeer* peer);
	void AckHorseName(DWORD dwPID, CPeer* peer);
#ifdef ENABLE_NEW_PET_SYSTEM
	void UpdatePetName(TPacketUpdateHorseName* data, CPeer* peer);
	void AckPetName(DWORD dwPID, CPeer* peer);
#endif
	void DeleteLoginKey(TPacketDC *data);
	void ResetLastPlayerID(const TPacketNeedLoginLogInfo* data);
	//delete gift notify icon
	void DeleteAwardId(TPacketDeleteAwardID* data);
#ifdef WEEKLY_RANK_BYLUZER
	void WeeklyRewards(CPeer * pkPeer, TWeeklyRewardsGD * p);
#endif	
    int  GetEmptyMallPosition(BYTE itemSize, DWORD accountID);
	void GiveItemRewardToPlayer(DWORD accountID, DWORD itemVnum);
	void UpdateChannelStatus(TChannelStatus* pData);
	void RequestChannelStatus(CPeer* peer, DWORD dwHandle);
#ifdef ENABLE_PROTO_FROM_DB
	public:
	bool		InitializeMobTableFromDB();
	bool		InitializeItemTableFromDB();
	protected:
	bool		bIsProtoReadFromDB;
#endif

#ifdef ENABLE_IN_GAME_LOG_SYSTEM
public:
	bool InitializeInGameLogTable();
//QID RESULT
	void ResultOfflineShopLog(CQueryInfo* pQueryInfo);

//GD RECV
	void RecvInGameLogPacketGD(CPeer* peer, const char* data);
	void RecvOfflineShopAddLogGD(const char* data);
	void RecvOfflineShopRequestLogGD(CPeer* peer,const char* data);

	void RecvTradeAddLogGD(const char* data);
	void RecvTradeLogRequestGD(CPeer* peer, const char* data);
	void RecvTradeLogDetailsRequestGD(CPeer* peer, const char* data);

//DG SEND
	void SendOfflineShopRequestLogDG(CPeer* peer, DWORD ownerID);
	void SendTradeLogRequestDG(CPeer* peer, DWORD ownerID);
	void SendTradeLogDetailsRequestDG(CPeer* peer, InGameLog::TTradeDetailsRequestGD data);

private:
	InGameLog::CacheOfflineShopLog m_ingamelogOfflineShop;
	InGameLog::CacheTradeLog m_ingamelogTrade;
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
public:
	void				RESULT_PRIVATE_SHOP_LOAD(CPeer* pPeer, MYSQL_RES* pRes, DWORD dwHandle, DWORD dwPID);
	void				RESULT_PRIVATE_SHOP_ITEM_LOAD(CPeer* pPeer, MYSQL_RES* pRes, DWORD dwHandle, DWORD dwPID);
	void				RESULT_PRIVATE_SHOP_SALE_LOAD(CPeer* pPeer, MYSQL_RES* pRes, DWORD dwHandle, DWORD dwPID);

	CPeer*				GetPrivateShopPeer(BYTE bChannel, WORD wListenPort);
	TItemTable*			GetItemTable(DWORD dwVnum);

	// Private Shop Cache
	CPrivateShopCache*	GetPrivateShopCache(DWORD dwPID);
	void				PutPrivateShopCache(TPrivateShop* pCache);
	bool				DeletePrivateShopCache(DWORD dwPID);
	void				UpdatePrivateShopCache();
	void				FlushPrivateShopCache(DWORD dwPID);

	// Item Cache
	void						CreatePrivateShopItemCacheSet(DWORD dwPID);
	TPrivateShopItemCacheSet*	GetPrivateShopItemCacheSet(DWORD dwPID);
	void						FlushPrivateShopItemCacheSet(DWORD dwPID);
	bool						DeletePrivateShopItemCacheSet(DWORD dwPID);

	CPrivateShopItemCache*		GetPrivateShopItemCache(DWORD dwID);
	void						PutPrivateShopItemCache(TPlayerPrivateShopItem* pNew, bool bSkipQuery = false);
	bool						DeletePrivateShopItemCache(DWORD dwID);

	void						UpdatePrivateShopItemCache();
	void						UpdatePrivateShopItemCacheSet(DWORD dwPID);

	// Sale History Cache
	void						CreatePrivateShopSaleCacheSet(DWORD dwPID);
	TPrivateShopSaleCacheSet*	GetPrivateShopSaleCacheSet(DWORD dwPID);
	void						FlushPrivateShopSaleCacheSet(DWORD dwPID);
	bool						DeletePrivateShopSaleCacheSet(DWORD dwPID);

	CPrivateShopSaleCache*		GetPrivateShopSaleCache(DWORD dwID);
	void						PutPrivateShopSaleCache(TPrivateShopSale* pNew, bool bSkipQuery = false);
	bool						DeletePrivateShopSaleCache(DWORD dwID);

	void						UpdatePrivateShopSaleCache();
	void						UpdatePrivateShopSaleCacheSet(DWORD dwPID);

	// Sales
	void						AddMarketItemPrice(TPrivateShopSale& rSale);
	TItemPrice*					GetMarketItemPrice(DWORD dwVnum);
	void						UpdateMarketItemPrice();


	// Database Entity
	LPPRIVATE_SHOP		CreatePrivateShop(DWORD dwPID);
	bool				DeletePrivateShop(DWORD dwPID);
	LPPRIVATE_SHOP		GetPrivateShop(DWORD dwPID);

	// SQL Data Processing
	bool				InitializePrivateShopMarketItemPrice();

	// Packet Processing
	void				ProcessPrivateShopPacket(CPeer* pPeer, DWORD dwHandle, const char* c_szData);

	LPPRIVATE_SHOP		PrivateShopSpawn(DWORD dwShopID);
	LPPRIVATE_SHOP		PrivateShopCreate(TPrivateShop* pTable, const std::vector<TPlayerPrivateShopItem>& c_vec_shopItem);
	void				PrivateShopBuild(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopClose(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopDelete(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopDespawn(CPeer* pPeer, DWORD dwHandle, const char* c_szData);

	void				PrivateShopWithdrawRequest(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopModifyRequest(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopBuyRequest(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopItemPriceChangeRequest(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopItemMoveRequest(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopItemCheckinRequest(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopItemCheckoutRequest(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopTitleChangeRequest(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopWarpRequest(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopSlotUnlockRequest(CPeer* pPeer, DWORD dwHandle, const char* c_szData);

	void				PrivateShopItemCheckinUpdate(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopItemCheckoutUpdate(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopWithdraw(const char* c_szData);
	void				PrivateShopBuy(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopFailedBuy(const char* c_szData);
	void				PrivateShopItemTransfer(TPlayerItem* pTItem);
	void				PrivateShopItemDelete(const char* c_szData);
	void				PrivateShopItemExpire(const char* c_szData);
	void				PrivateShopPremiumTimeUpdate(const char* c_szData);

	void				PrivateShopStartPremiumEvent(DWORD dwPID);
	void				PrivateShopEndPremiumEvent(DWORD dwPID);
	void				UpdatePrivateShopPremiumEvent();
	bool				IsPrivateShopPremiumEvent(DWORD dwPID);

	void				PrivateShopDestroy(LPPRIVATE_SHOP pPrivateShop);
	void				PrivateShopGameDespawn(LPPRIVATE_SHOP pPrivateShop);
	void				PrivateShopGameSpawn(LPPRIVATE_SHOP pPrivateShop);

	bool				PrivateShopFetchData(DWORD dwShopID, TPrivateShop& rTable, std::vector<TPlayerPrivateShopItem>& c_vec_shopItem);

	void				PrivateShopPeerSpawn(CPeer* pPeer);
private:
	TPrivateShopCacheMap					m_map_privateShopCache;
	TPrivateShopMap							m_map_privateShop;

	TPrivateShopItemCacheMap				m_map_privateShopItemCache;
	TPrivateShopItemCacheSetPtrMap			m_map_pPrivateShopItemCacheSetPtr;

	TPrivateShopSaleCacheMap				m_map_privateShopSaleCache;
	TPrivateShopSaleCacheSetPtrMap			m_map_pPrivateShopSaleCacheSetPtr;

	TMarketItemPriceMap						m_map_marketItemPrice;
	TMarketItemPriceListMap					m_map_marketItemPriceList;

	TPrivateShopPtrList						m_list_privateShopPremium;
	std::vector<TItemTable*>				m_vec_itemVnumRange;
#endif
#ifdef __DUNGEON_INFO_ENABLE__
public:
	void DungeonInfoRankingUpdate(CPeer * peer, DWORD dwHeader, const char * data);
#endif

};

template<class Func>
Func CClientManager::for_each_peer(Func f)
{
    TPeerList::iterator it;
    for (it = m_peerList.begin(); it!=m_peerList.end();++it)
    {
	f(*it);
    }
    return f;
}
#endif
//martysama0134's ceqyqttoaf71vasf9t71218
