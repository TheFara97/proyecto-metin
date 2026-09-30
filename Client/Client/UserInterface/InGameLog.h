#pragma once
#ifdef ENABLE_IN_GAME_LOG_SYSTEM
#include "packet.h"
class InGameLog : public singleton<InGameLog>
{
public:
	InGameLog();
	virtual ~InGameLog();
	void Destroy();
	void SendPacket(BYTE subHeader, DWORD logID = 0);
	void ChangePage(BYTE page);

	bool OfflineShopSoldOpen();
	void OfflineShopSoldLogAddData (const TOfflineShopSoldLog& sold);
	inline void OfflineShopSoldReserve(DWORD logcount) { m_vecOfflineShopSoldItem.reserve(logcount); }
	inline void OfflineShopSoldLoad() { m_isOfflineShopSoldLoad = true; }

	inline int SoldItemCount() { return m_vecOfflineShopSoldItem.size(); }
	inline DWORD SoldItemGetVnum(int idx) { return m_vecOfflineShopSoldItem[idx].item.dwVnum; }
	inline MAX_COUNT SoldItemGetCount(int idx) { return m_vecOfflineShopSoldItem[idx].item.dwCount; }
	inline unsigned long long SoldItemGetPrice(int idx) { return m_vecOfflineShopSoldItem[idx].sellPrice; }
	inline char* SoldItemGetBuyerName(int idx) { return m_vecOfflineShopSoldItem[idx].buyerName; }
	inline BYTE SoldItemGetAttrType(int idx,BYTE attridx) { return m_vecOfflineShopSoldItem[idx].item.aAttr[attridx].bType; }
	inline short SoldItemGetAttrValue(int idx, BYTE attridx) { return m_vecOfflineShopSoldItem[idx].item.aAttr[attridx].sValue; }
	inline long SoldItemGetSocket(int idx, BYTE socketidx) { return m_vecOfflineShopSoldItem[idx].item.alSockets[socketidx]; }
	inline char* SoldItemGetDate(int idx) { return m_vecOfflineShopSoldItem[idx].soldDate; }
	inline bool IsLoadedSoldItem() { return m_isOfflineShopSoldLoad; }

	/**********************Trade Log ***********************/
	inline void TradeLogLoad() { m_isTradeLogLoad = true; }
	void TradeLogAddData(const TTradeLogRequestInfo& sold);
	inline void TradeLogInfoReserve(DWORD logcount) { m_vecTradeLogInfo.reserve(logcount); }
	bool TradeLogOpen();
	inline bool IsLoadedTradeLog() { return m_isTradeLogLoad; }

	inline int TradeLogInfoCount() { return m_vecTradeLogInfo.size(); }
	inline DWORD GetTradeLogId(int idx) { return m_vecTradeLogInfo[idx].logID; }
	inline char* GetTradeDate(int idx) { return m_vecTradeLogInfo[idx].tradingDate; }
	inline char* GetTradeVictimName(int idx) { return m_vecTradeLogInfo[idx].victimName; }

	/**********************Trade Log Details***********************/
	typedef std::vector<TTradeLogRequestItem> VEC_TRADE_DETAILS_ITEM;
	typedef struct STradeDetailsInfo
	{
		TTradeLogDetailsRequest info;
		VEC_TRADE_DETAILS_ITEM item;
	}TTradeDetailsInfo;
	typedef std::map<DWORD, TTradeDetailsInfo> MAP_TRADE_DETAILS;

	void SendTradeDetails(DWORD logID);
	void TradeDetailsLogCreate(const TTradeLogDetailsRequest info);
	void TradeDetailsPutItem(const TTradeLogRequestItem& item, DWORD logID);
	inline void TradeDetailsUpdate(DWORD logID);

	char* GetTradeDetailsVictimName(DWORD logID);
	void GetTradeDetailsGold(uint64_t& owner, uint64_t& victim, DWORD logID);

	BYTE GetTradeDetailsItemCount(DWORD logID);
	BYTE TradeDetailsGetPos(DWORD logID, BYTE idx);
	bool TradeDetailsIsOwner(DWORD logID, BYTE idx);
	BYTE TradeDetailsGetAttrTpye(DWORD logID, BYTE idx, BYTE attridx);
	short TradeDetailsGetAttrValue(DWORD logID, BYTE idx, BYTE attridx);
	long TradeDetailsGetSocket(DWORD logID, BYTE idx, BYTE socketidx);
	DWORD TradeDetailsGetVnum(DWORD logID, BYTE idx);
	DWORD TradeDetailsGetCount(DWORD logID, BYTE idx);
private:
	std::vector<TOfflineShopSoldLog> m_vecOfflineShopSoldItem;
	bool m_isOfflineShopSoldLoad;

	std::vector<TTradeLogRequestInfo> m_vecTradeLogInfo;
	bool m_isTradeLogLoad;

	MAP_TRADE_DETAILS m_mapTradeDetails; //first logid
};
#endif
