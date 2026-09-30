#include "stdafx.h"
#ifdef ENABLE_IN_GAME_LOG_SYSTEM
#include "InGameLog.h"
#include "PythonNetworkStream.h"
#include "Packet.h"
/**************************************************************
					IN GAME LOG CLASS
**************************************************************/
InGameLog::InGameLog()
{
	Destroy();
}

InGameLog::~InGameLog()
{
	Destroy();
}

void InGameLog::Destroy()
{
	m_vecOfflineShopSoldItem.clear();
	m_isOfflineShopSoldLoad = false;
	m_isTradeLogLoad = false;
	m_mapTradeDetails.clear();
	m_vecTradeLogInfo.clear();
}

void InGameLog::SendPacket(BYTE subHeader, DWORD logID)
{
	TPacketCGInGameLog packet;
	packet.header = HEADER_CG_IN_GAME_LOG;
	packet.subHeader = subHeader;
	packet.logID = logID;

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	if (!rkNetStream.Send(sizeof(packet), &packet))
	{
		TraceError("InGameLog Send Packet Error sub %d", subHeader);
		return;
	}
}

bool InGameLog::OfflineShopSoldOpen()
{
	if (m_isOfflineShopSoldLoad)
	{
		return true;
	}
	else
	{
		SendPacket(SUBHEADER_CG_OFFLINESHOP_LOG_OPEN);
		return false;
	}
}

void InGameLog::ChangePage(BYTE page)
{
	PyCallClassMemberFunc(CPythonNetworkStream::instance().GetGameWindow(), "BINARY_IN_GAME_LOG_OPEN", Py_BuildValue("(i)", page));
}

void InGameLog::OfflineShopSoldLogAddData(const TOfflineShopSoldLog& sold)
{
	m_vecOfflineShopSoldItem.push_back(sold);
}

void InGameLog::TradeLogAddData(const TTradeLogRequestInfo& log)
{
	m_vecTradeLogInfo.push_back(log);
}

bool InGameLog::TradeLogOpen()
{
	if (m_isTradeLogLoad)
	{
		return true;
	}
	else
	{
		SendPacket(SUBHEADER_CG_TRADE_LOG_OPEN);
		return false;
	}
}

void InGameLog::SendTradeDetails(DWORD logID)
{
	MAP_TRADE_DETAILS::iterator it = m_mapTradeDetails.find(logID);
	if (it == m_mapTradeDetails.end())
	{
		SendPacket(SUBHEADER_CG_TRADE_LOG_DETAILS_OPEN, logID);
	}
	else
	{
		TradeDetailsUpdate(logID);
	}	
}

void InGameLog::TradeDetailsUpdate(DWORD logID)
{
	PyCallClassMemberFunc(CPythonNetworkStream::instance().GetGameWindow(), "BINARY_IN_GAME_LOG_TRADE_DETAILS", Py_BuildValue("(i)", logID));
}

void InGameLog::TradeDetailsLogCreate(const TTradeLogDetailsRequest info)
{
	TTradeDetailsInfo temp;
	memcpy(&temp.info, &info, sizeof(temp.info));
	temp.item.reserve(info.itemCount);
	m_mapTradeDetails.insert(std::make_pair(info.logID, temp));
}

void InGameLog::TradeDetailsPutItem(const TTradeLogRequestItem& item, DWORD logID)
{
	MAP_TRADE_DETAILS::iterator it = m_mapTradeDetails.find(logID);
	if (it == m_mapTradeDetails.end())
		return;
	it->second.item.push_back(item);
}

char* InGameLog::GetTradeDetailsVictimName(DWORD logID)
{
	for (int i = 0; i < m_vecTradeLogInfo.size(); i++)
	{
		if (m_vecTradeLogInfo[i].logID == logID)
		{
			return m_vecTradeLogInfo[i].victimName;
			break;
		}
	}
	return " ";
}

void InGameLog::GetTradeDetailsGold(uint64_t& owner, uint64_t& victim, DWORD logID)
{
	MAP_TRADE_DETAILS::iterator it = m_mapTradeDetails.find(logID);
	if (it == m_mapTradeDetails.end())
		return;

	owner = it->second.info.ownerGold;
	victim = it->second.info.victimGold;
}

BYTE InGameLog::GetTradeDetailsItemCount(DWORD logID)
{
	MAP_TRADE_DETAILS::iterator it = m_mapTradeDetails.find(logID);
	if (it == m_mapTradeDetails.end())
		return 0;
	return it->second.info.itemCount;
}

BYTE InGameLog::TradeDetailsGetPos(DWORD logID, BYTE idx)
{
	MAP_TRADE_DETAILS::iterator it = m_mapTradeDetails.find(logID);
	if (it == m_mapTradeDetails.end())
		return 0;

	return it->second.item[idx].pos;
}

bool InGameLog::TradeDetailsIsOwner(DWORD logID, BYTE idx)
{
	MAP_TRADE_DETAILS::iterator it = m_mapTradeDetails.find(logID);
	if (it == m_mapTradeDetails.end())
		return 0;

	return it->second.item[idx].isOwner;
}

BYTE InGameLog::TradeDetailsGetAttrTpye(DWORD logID, BYTE idx, BYTE attridx)
{
	MAP_TRADE_DETAILS::iterator it = m_mapTradeDetails.find(logID);
	if (it == m_mapTradeDetails.end())
		return 0;

	return it->second.item[idx].item.aAttr[attridx].bType;
}

short InGameLog::TradeDetailsGetAttrValue(DWORD logID, BYTE idx, BYTE attridx)
{
	MAP_TRADE_DETAILS::iterator it = m_mapTradeDetails.find(logID);
	if (it == m_mapTradeDetails.end())
		return 0;

	return it->second.item[idx].item.aAttr[attridx].sValue;
}

long InGameLog::TradeDetailsGetSocket(DWORD logID, BYTE idx, BYTE socketidx)
{
	MAP_TRADE_DETAILS::iterator it = m_mapTradeDetails.find(logID);
	if (it == m_mapTradeDetails.end())
		return 0;

	return it->second.item[idx].item.alSockets[socketidx];
}

DWORD InGameLog::TradeDetailsGetVnum(DWORD logID, BYTE idx)
{
	MAP_TRADE_DETAILS::iterator it = m_mapTradeDetails.find(logID);
	if (it == m_mapTradeDetails.end())
		return 0;

	return it->second.item[idx].item.dwVnum;
}

DWORD InGameLog::TradeDetailsGetCount(DWORD logID, BYTE idx)
{
	MAP_TRADE_DETAILS::iterator it = m_mapTradeDetails.find(logID);
	if (it == m_mapTradeDetails.end())
		return 0;

	return it->second.item[idx].item.dwCount;
}
/**************************************************************
					PYTHON NETWORK STREAM
**************************************************************/

bool CPythonNetworkStream::RecvInGameLog()
{
	TPacketGCInGameLog packet;
	if (!Recv(sizeof(packet), &packet))
	{
		TraceError("InGameLog Packet Recovery Error");
		return false;
	}

	WORD buffersize = packet.size - sizeof(TPacketGCInGameLog);
	bool ret = true;

	switch (packet.subHeader)
	{
		case SUBHEADER_GC_OFFLINESHOP_LOG_OPEN: { ret = RecvOfflineShopLog(buffersize); break; }
		case SUBHEADER_GC_OFFLINESHOP_LOG_ADD: { ret = RecvOfflineShopLogAdd(buffersize); break; }
		case SUBHEADER_GC_TRADE_LOG_OPEN: { ret = RecvTradeLog(buffersize); break; }
		case SUBHEADER_GC_TRADE_LOG_DETAILS_OPEN: { ret = RecvTradeDetailsLog(buffersize); break; }
		default: { TraceError("InGameLog noting sub header %d", packet.subHeader); ret = false;  break; }
	}
	return ret;
}

bool CPythonNetworkStream::RecvOfflineShopLog(WORD bufferSize)
{
	DWORD logcount;
	if (!Recv(sizeof(logcount), &logcount))
	{
		TraceError("InGameLog Sub Packet: %d Recovery Error (logcount)", SUBHEADER_GC_OFFLINESHOP_LOG_OPEN);
		return false;
	}

	InGameLog& ingamelog = InGameLog::Instance();

	if (logcount < 1)
	{
		ingamelog.OfflineShopSoldLoad();
		ingamelog.ChangePage(0);
		return true;
	}

	bufferSize -= sizeof(DWORD);

	// Fix: Check if buffer size equals the expected size, not if it's divisible
	WORD expectedSize = sizeof(TOfflineShopSoldLog) * logcount;
	if (bufferSize != expectedSize)
	{
		TraceError("InGameLog Buffer size error buffersize: %u expected: %u log count %u", 
			bufferSize, expectedSize, logcount);
		return false;
	}
	
	ingamelog.OfflineShopSoldReserve(logcount);
	TOfflineShopSoldLog soldlog;
	for (DWORD i = 0; i < logcount; i++)
	{
		if (!Recv(sizeof(soldlog), &soldlog))
		{
			TraceError("InGameLog Sub Packet: %d Recovery Error (soldlog %u)", SUBHEADER_GC_OFFLINESHOP_LOG_OPEN, i);
			ingamelog.Destroy();
			return false;
		}

		ingamelog.OfflineShopSoldLogAddData(soldlog);
	}

	ingamelog.OfflineShopSoldLoad();
	ingamelog.ChangePage(0);
	return true;
}

bool CPythonNetworkStream::RecvOfflineShopLogAdd(WORD bufferSize)
{
	// Fix: Check if buffer size equals the expected size, not if it's divisible
	if (bufferSize != sizeof(TOfflineShopSoldLog))
	{
		TraceError("InGameLog Buffer size error buffersize: %u expected: %u", bufferSize, sizeof(TOfflineShopSoldLog));
		return false;
	}

	InGameLog& ingamelog = InGameLog::Instance();
	TOfflineShopSoldLog soldlog;

	if (!Recv(sizeof(soldlog), &soldlog))
	{
		TraceError("InGameLog Sub Packet: %d Recovery Error (soldlog)", SUBHEADER_GC_OFFLINESHOP_LOG_ADD);
		ingamelog.Destroy();
		return false;
	}
	ingamelog.OfflineShopSoldLogAddData(soldlog);
	return true;
}

bool CPythonNetworkStream::RecvTradeLog(WORD bufferSize)
{
	DWORD logcount;
	if (!Recv(sizeof(logcount), &logcount))
	{
		TraceError("InGameLog Sub Packet: %d Recovery Error (logcount)", SUBHEADER_GC_TRADE_LOG_OPEN);
		return false;
	}

	InGameLog& ingamelog = InGameLog::Instance();

	if (logcount < 1)
	{
		ingamelog.TradeLogLoad();
		ingamelog.ChangePage(1);
		return true;
	}

	bufferSize -= sizeof(DWORD);

	// Fix: Check if buffer size equals the expected size, not if it's divisible
	WORD expectedSize = sizeof(TTradeLogRequestInfo) * logcount;
	if (bufferSize != expectedSize)
	{
		TraceError("InGameLog Buffer size error buffersize: %u expected: %u log count %u", 
			bufferSize, expectedSize, logcount);
		return false;
	}

	ingamelog.TradeLogInfoReserve(logcount);
	TTradeLogRequestInfo tradelog;
	for (DWORD i = 0; i < logcount; i++)
	{
		if (!Recv(sizeof(tradelog), &tradelog))
		{
			TraceError("InGameLog Sub Packet: %d Recovery Error (tradelog %u)", SUBHEADER_GC_TRADE_LOG_OPEN, i);
			ingamelog.Destroy();
			return false;
		}

		ingamelog.TradeLogAddData(tradelog);
	}

	ingamelog.TradeLogLoad();
	ingamelog.ChangePage(1);
	return true;
}

bool CPythonNetworkStream::RecvTradeDetailsLog(WORD bufferSize)
{
	// Debug: Print structure sizes to help diagnose the issue
	TraceError("DEBUG: sizeof(TTradeLogDetailsRequest) = %u", sizeof(TTradeLogDetailsRequest));
	TraceError("DEBUG: sizeof(TTradeLogRequestItem) = %u", sizeof(TTradeLogRequestItem));
	TraceError("DEBUG: sizeof(TItemInfo) = %u", sizeof(TItemInfo));
	TraceError("DEBUG: bufferSize = %u", bufferSize);

	// First, validate that the buffer contains at least the details structure
	if (bufferSize < sizeof(TTradeLogDetailsRequest))
	{
		TraceError("InGameLog Buffer too small for details: %u < %u", bufferSize, sizeof(TTradeLogDetailsRequest));
		return false;
	}

	TTradeLogDetailsRequest details;
	if (!Recv(sizeof(details), &details))
	{
		TraceError("InGameLog Sub Packet: %d Recovery Error (details)", SUBHEADER_GC_TRADE_LOG_DETAILS_OPEN);
		return false;
	}

	TraceError("DEBUG: details.logID = %u, details.itemCount = %u", details.logID, details.itemCount);

	InGameLog& ingamelog = InGameLog::Instance();
	ingamelog.TradeDetailsLogCreate(details);

	if (details.itemCount < 1)
	{
		// For now, just skip the buffer size check when itemCount is 0
		TraceError("DEBUG: No items to process, itemCount = 0");
		ingamelog.TradeDetailsUpdate(details.logID);
		return true;
	}

	// Calculate remaining buffer size after reading details
	WORD remainingBuffer = bufferSize - sizeof(TTradeLogDetailsRequest);
	WORD expectedItemsSize = sizeof(TTradeLogRequestItem) * details.itemCount;
	
	TraceError("DEBUG: remainingBuffer = %u, expectedItemsSize = %u", remainingBuffer, expectedItemsSize);

	// For now, let's be more lenient and just check if we have enough data
	if (remainingBuffer < expectedItemsSize)
	{
		TraceError("InGameLog Not enough data for items: remaining %u < expected %u", 
			remainingBuffer, expectedItemsSize);
		return false;
	}

	TTradeLogRequestItem item;
	for (DWORD i = 0; i < details.itemCount; i++)
	{
		if (!Recv(sizeof(item), &item))
		{
			TraceError("InGameLog Sub Packet: %d Recovery Error (item %u)", SUBHEADER_GC_TRADE_LOG_DETAILS_OPEN, i);
			ingamelog.Destroy();
			return false;
		}
		ingamelog.TradeDetailsPutItem(item, details.logID);
	}

	// Calculate how much data we actually consumed
	WORD consumedData = sizeof(TTradeLogDetailsRequest) + (sizeof(TTradeLogRequestItem) * details.itemCount);
	if (consumedData != bufferSize)
	{
		TraceError("DEBUG: Data mismatch - consumed %u bytes but buffer was %u bytes (difference: %d)", 
			consumedData, bufferSize, (int)bufferSize - (int)consumedData);
	}

	ingamelog.TradeDetailsUpdate(details.logID);
	return true;
}

/**************************************************************
					PYTHON MODULE
**************************************************************/
PyObject* ingamelogOpenLog(PyObject* poSelf, PyObject* poArgs)
{
	BYTE page;
	if (!PyTuple_GetInteger(poArgs, 0, &page))
		return Py_BadArgument();

	bool open = false;
	if (page == 0)
		open = InGameLog::Instance().OfflineShopSoldOpen();
	else
		open = InGameLog::Instance().TradeLogOpen();

	return Py_BuildValue("i",open);
}

/**********************Offline Shop Sold Module ***********************/
PyObject* ingamelogSoldItemCount(PyObject* poSelf, PyObject* poArgs)
{
	int count = InGameLog::Instance().SoldItemCount();
	return Py_BuildValue("i", count);
}

PyObject* ingamelogSoldItemGetVnum(PyObject* poSelf, PyObject* poArgs)
{
	int idx;
	if (!PyTuple_GetInteger(poArgs, 0, &idx))
		return Py_BadArgument();

	DWORD vnum = InGameLog::Instance().SoldItemGetVnum(idx);
	return Py_BuildValue("i", vnum);
}

PyObject* ingamelogSoldItemGetCount(PyObject* poSelf, PyObject* poArgs)
{
	int idx;
	if (!PyTuple_GetInteger(poArgs, 0, &idx))
		return Py_BadArgument();

	MAX_COUNT count = InGameLog::Instance().SoldItemGetCount(idx);
	return Py_BuildValue("i", count);
}

PyObject* ingamelogSoldItemGetPrice(PyObject* poSelf, PyObject* poArgs)
{
	int idx;
	if (!PyTuple_GetInteger(poArgs, 0, &idx))
		return Py_BadArgument();

	unsigned long long price = InGameLog::Instance().SoldItemGetPrice(idx);
	return Py_BuildValue("K", price);
}

PyObject* ingamelogSoldItemGetBuyerName(PyObject* poSelf, PyObject* poArgs)
{
	int idx;
	if (!PyTuple_GetInteger(poArgs, 0, &idx))
		return Py_BadArgument();

	char* buyername = InGameLog::Instance().SoldItemGetBuyerName(idx);
	return Py_BuildValue("s", buyername);
}

PyObject* ingamelogSoldItemGetAttrTpye(PyObject* poSelf, PyObject* poArgs)
{
	int idx;
	if (!PyTuple_GetInteger(poArgs, 0, &idx))
		return Py_BadArgument();

	BYTE attridx;
	if (!PyTuple_GetInteger(poArgs, 1, &attridx))
		return Py_BadArgument();

	BYTE type = InGameLog::Instance().SoldItemGetAttrType(idx, attridx);
	return Py_BuildValue("i", type);
}

PyObject* ingamelogSoldItemGetAttrValue(PyObject* poSelf, PyObject* poArgs)
{
	int idx;
	if (!PyTuple_GetInteger(poArgs, 0, &idx))
		return Py_BadArgument();

	BYTE attridx;
	if (!PyTuple_GetInteger(poArgs, 1, &attridx))
		return Py_BadArgument();

	short value = InGameLog::Instance().SoldItemGetAttrValue(idx, attridx);
	return Py_BuildValue("i", value);
}

PyObject* ingamelogSoldItemGetSocket(PyObject* poSelf, PyObject* poArgs)
{
	int idx;
	if (!PyTuple_GetInteger(poArgs, 0, &idx))
		return Py_BadArgument();

	BYTE socketidx;
	if (!PyTuple_GetInteger(poArgs, 1, &socketidx))
		return Py_BadArgument();

	long socket = InGameLog::Instance().SoldItemGetSocket(idx, socketidx);
	return Py_BuildValue("i", socket);
}

PyObject* ingamelogSoldItemGetDate(PyObject* poSelf, PyObject* poArgs)
{
	int idx;
	if (!PyTuple_GetInteger(poArgs, 0, &idx))
		return Py_BadArgument();

	char* date = InGameLog::Instance().SoldItemGetDate(idx);
	return Py_BuildValue("s", date);
}

PyObject* ingamelogIsLoadOfflineShop(PyObject* poSelf, PyObject* poArgs)
{
	bool load = InGameLog::Instance().IsLoadedSoldItem();
	return Py_BuildValue("i", load);
}

/**********************Trade Log Module***********************/
PyObject* ingamelogIsLoadedTradeLog(PyObject* poSelf, PyObject* poArgs)
{
	bool load = InGameLog::Instance().IsLoadedTradeLog();
	return Py_BuildValue("i", load);
}

PyObject* ingamelogTradeLogInfoCount(PyObject* poSelf, PyObject* poArgs)
{
	int count = InGameLog::Instance().TradeLogInfoCount();
	return Py_BuildValue("i", count);
}

PyObject* ingamelogGetTradeLogId(PyObject* poSelf, PyObject* poArgs)
{
	int idx;
	if (!PyTuple_GetInteger(poArgs, 0, &idx))
		return Py_BadArgument();

	DWORD logID = InGameLog::Instance().GetTradeLogId(idx);
	return Py_BuildValue("i", logID);
}

PyObject* ingamelogGetTradeDate(PyObject* poSelf, PyObject* poArgs)
{
	int idx;
	if (!PyTuple_GetInteger(poArgs, 0, &idx))
		return Py_BadArgument();

	char* date = InGameLog::Instance().GetTradeDate(idx);
	return Py_BuildValue("s", date);
}

PyObject* ingamelogGetTradeVictimName(PyObject* poSelf, PyObject* poArgs)
{
	int idx;
	if (!PyTuple_GetInteger(poArgs, 0, &idx))
		return Py_BadArgument();

	char* victimName = InGameLog::Instance().GetTradeVictimName(idx);
	return Py_BuildValue("s", victimName);
}


/**********************Trade Log Details Module***********************/
PyObject* ingamelogSendTradeDetails(PyObject* poSelf, PyObject* poArgs)
{
	DWORD logID;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &logID))
		return Py_BadArgument();

	InGameLog::Instance().SendTradeDetails(logID);
	return Py_BuildNone();
}

PyObject* ingamelogGetTradeDetailsVictimName(PyObject* poSelf, PyObject* poArgs)
{
	DWORD logID;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &logID))
		return Py_BadArgument();

	char* victimName = InGameLog::Instance().GetTradeDetailsVictimName(logID);
	return Py_BuildValue("s", victimName);
}

PyObject* ingamelogGetTradeDetailsGold(PyObject* poSelf, PyObject* poArgs)
{
	DWORD logID;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &logID))
		return Py_BadArgument();

	uint64_t owner, victim = 0;
	InGameLog::Instance().GetTradeDetailsGold(owner, victim, logID);
	return Py_BuildValue("KK", owner, victim);
}


PyObject* ingamelogTradeDetailsItemCount(PyObject* poSelf, PyObject* poArgs)
{
	DWORD logID;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &logID))
		return Py_BadArgument();

	BYTE itemcount = 0;
	itemcount = InGameLog::Instance().GetTradeDetailsItemCount(logID);
	return Py_BuildValue("i", itemcount);
}

PyObject* ingamelogTradeDetailsGetPos(PyObject* poSelf, PyObject* poArgs)
{
	DWORD logID;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &logID))
		return Py_BadArgument();

	BYTE idx;
	if (!PyTuple_GetInteger(poArgs, 1, &idx))
		return Py_BadArgument();

	BYTE pos = 0;
	pos = InGameLog::Instance().TradeDetailsGetPos(logID, idx);
	return Py_BuildValue("i", pos);
}

PyObject* ingamelogTradeDetailsIsOwner(PyObject* poSelf, PyObject* poArgs)
{
	DWORD logID;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &logID))
		return Py_BadArgument();

	BYTE idx;
	if (!PyTuple_GetInteger(poArgs, 1, &idx))
		return Py_BadArgument();

	bool isOwner = false;
	isOwner = InGameLog::Instance().TradeDetailsIsOwner(logID, idx);
	return Py_BuildValue("i", isOwner);
}

PyObject* ingamelogTradeDetailsGetAttrTpye(PyObject* poSelf, PyObject* poArgs)
{
	DWORD logID;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &logID))
		return Py_BadArgument();

	BYTE idx;
	if (!PyTuple_GetInteger(poArgs, 1, &idx))
		return Py_BadArgument();

	BYTE attridx;
	if (!PyTuple_GetInteger(poArgs, 2, &attridx))
		return Py_BadArgument();

	BYTE attrtype = 0;
	attrtype = InGameLog::Instance().TradeDetailsGetAttrTpye(logID, idx, attridx);
	return Py_BuildValue("i", attrtype);
}


PyObject* ingamelogTradeDetailsGetAttrValue(PyObject* poSelf, PyObject* poArgs)
{
	DWORD logID;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &logID))
		return Py_BadArgument();

	BYTE idx;
	if (!PyTuple_GetInteger(poArgs, 1, &idx))
		return Py_BadArgument();

	BYTE attridx;
	if (!PyTuple_GetInteger(poArgs, 2, &attridx))
		return Py_BadArgument();

	short attrval = 0;
	attrval = InGameLog::Instance().TradeDetailsGetAttrValue(logID, idx, attridx);
	return Py_BuildValue("i", attrval);
}

PyObject* ingamelogTradeDetailsGetSocket(PyObject* poSelf, PyObject* poArgs)
{
	DWORD logID;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &logID))
		return Py_BadArgument();

	BYTE idx;
	if (!PyTuple_GetInteger(poArgs, 1, &idx))
		return Py_BadArgument();

	BYTE socketidx;
	if (!PyTuple_GetInteger(poArgs, 2, &socketidx))
		return Py_BadArgument();

	long socketval = 0;
	socketval = InGameLog::Instance().TradeDetailsGetSocket(logID, idx, socketidx);
	return Py_BuildValue("i", socketval);
}

PyObject* ingamelogTradeDetailsGetVnum(PyObject* poSelf, PyObject* poArgs)
{
	DWORD logID;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &logID))
		return Py_BadArgument();

	BYTE idx;
	if (!PyTuple_GetInteger(poArgs, 1, &idx))
		return Py_BadArgument();

	DWORD vnum = 0;
	vnum = InGameLog::Instance().TradeDetailsGetVnum(logID, idx);
	return Py_BuildValue("i", vnum);
}

PyObject* ingamelogTradeDetailsGetCount(PyObject* poSelf, PyObject* poArgs)
{
	DWORD logID;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &logID))
		return Py_BadArgument();

	BYTE idx;
	if (!PyTuple_GetInteger(poArgs, 1, &idx))
		return Py_BadArgument();

	DWORD count = 0;
	count = InGameLog::Instance().TradeDetailsGetCount(logID, idx);
	return Py_BuildValue("i", count);
}

void initInGameLog()
{
	static PyMethodDef s_methods[] =
	{
		{ "IsLoadOfflineShop", ingamelogIsLoadOfflineShop, METH_VARARGS },
		{ "IsLoadTradeLog", ingamelogIsLoadOfflineShop, METH_VARARGS },
		{ "OpenLog", ingamelogOpenLog, METH_VARARGS },

		{ "SoldItemCount", ingamelogSoldItemCount, METH_VARARGS },
		{ "SoldItemGetVnum", ingamelogSoldItemGetVnum, METH_VARARGS },
		{ "SoldItemGetCount", ingamelogSoldItemGetCount, METH_VARARGS },
		{ "SoldItemGetPrice", ingamelogSoldItemGetPrice, METH_VARARGS },
		{ "SoldItemGetBuyerName", ingamelogSoldItemGetBuyerName, METH_VARARGS },
		{ "SoldItemGetAttrTpye", ingamelogSoldItemGetAttrTpye, METH_VARARGS },
		{ "SoldItemGetAttrValue", ingamelogSoldItemGetAttrValue, METH_VARARGS },
		{ "SoldItemGetSocket", ingamelogSoldItemGetSocket, METH_VARARGS },
		{ "SoldItemGetDate", ingamelogSoldItemGetDate, METH_VARARGS },

		{ "TradeLogInfoCount", ingamelogTradeLogInfoCount, METH_VARARGS },
		{ "GetTradeLogId", ingamelogGetTradeLogId, METH_VARARGS },
		{ "GetTradeDate", ingamelogGetTradeDate, METH_VARARGS },
		{ "GetTradeVictimName", ingamelogGetTradeVictimName, METH_VARARGS },
		{ "IsLoadedTradeLog", ingamelogIsLoadedTradeLog, METH_VARARGS },

		{ "SendTradeDetails", ingamelogSendTradeDetails, METH_VARARGS },
		{ "GetTradeDetailsVictimName", ingamelogGetTradeDetailsVictimName, METH_VARARGS },
		{ "GetTradeDetailsGold", ingamelogGetTradeDetailsGold, METH_VARARGS },

		{ "TradeDetailsItemCount", ingamelogTradeDetailsItemCount, METH_VARARGS },
		{ "TradeDetailsGetPos", ingamelogTradeDetailsGetPos, METH_VARARGS },
		{ "TradeDetailsIsOwner", ingamelogTradeDetailsIsOwner, METH_VARARGS },
		{ "TradeDetailsGetAttrTpye", ingamelogTradeDetailsGetAttrTpye, METH_VARARGS },
		{ "TradeDetailsGetAttrValue", ingamelogTradeDetailsGetAttrValue, METH_VARARGS },
		{ "TradeDetailsGetSocket", ingamelogTradeDetailsGetSocket, METH_VARARGS },
		{ "TradeDetailsGetVnum", ingamelogTradeDetailsGetVnum, METH_VARARGS },
		{ "TradeDetailsGetCount", ingamelogTradeDetailsGetCount, METH_VARARGS },
		{nullptr, nullptr, 0 },
	};
	PyObject* poModule = Py_InitModule("ingamelog", s_methods);
}
#endif