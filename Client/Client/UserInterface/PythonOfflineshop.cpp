#include "StdAfx.h"
#ifdef ENABLE_OFFLINE_SHOP
#include <fstream>
//#include <libconfig.h>
#include "../GameLib/ItemManager.h"
#include "Packet.h"
#include "PythonNetworkStream.h"
#include "PythonTextTail.h"
#include "PythonOfflineshop.h"
#include "PythonSystem.h"
#include "InstanceBase.h"
#include "PythonCharacterManager.h"
#define ApplyPyMethod(str, ...)		if(m_poWindow)PyCallClassMemberFunc(m_poWindow, str, __VA_ARGS__); else TraceError("OFFLINESHOP:: CANNOT CALL CLASS MEMBER %s",str);
#ifdef ENABLE_SHOP_SEARCH
#define ApplyPyMethodForSearch(str, ...)		if(m_poWindowForSearch)PyCallClassMemberFunc(m_poWindowForSearch, str, __VA_ARGS__); else TraceError("ShopSearch:: CANNOT CALL CLASS MEMBER %s",str);
#endif
#define GetPyLongLong(tuple,p)		if(!PyTuple_GetLongLong(tuple,		 iArg++,  (p))){	TraceError("%s : cant get %s ",__FUNCTION__, #p);return Py_BadArgument();}
#define GetPyInteger(tuple,p)		if(!PyTuple_GetInteger(tuple,		 iArg++,  (p))){	TraceError("%s : cant get %s ",__FUNCTION__, #p);return Py_BadArgument();}
#define GetPyDWORD(tuple,p)			if(!PyTuple_GetUnsignedLong(tuple,   iArg++,  (p))){	TraceError("%s : cant get %s ",__FUNCTION__, #p);return Py_BadArgument();}
#define GetPyString(tuple,p)		if(!PyTuple_GetString(tuple,		 iArg++,  (p))){	TraceError("%s : cant get %s ",__FUNCTION__, #p);return Py_BadArgument();}
#define GetPyObject(tuple,p)		if(!PyTuple_GetObject(tuple,		 iArg++,  (p))){	TraceError("%s : cant get %s ",__FUNCTION__, #p);return Py_BadArgument();}
#define GetPyBool(tuple,p)			if(!PyTuple_GetBoolean(tuple,		 iArg++,  (p))){	TraceError("%s : cant get %s ",__FUNCTION__, #p);return Py_BadArgument();}
#define NOARGS Py_BuildValue("()")
#define PutsError(fmt, ...) TraceError("In function %s line %d : " ##fmt , __FUNCTION__,__LINE__, __VA_ARGS__)
#define UNUSED_VAR(var) var;

std::wstring Utf8ToWideString(const std::string & str) {
	if (str.empty()) return std::wstring();

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstrTo[0], size_needed);

	wstrTo.resize(size_needed - 1);
	return wstrTo;
}

std::string WideStringToUtf8(const std::wstring & wstr) {
	if (wstr.empty()) return std::string();

	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &strTo[0], size_needed, NULL, NULL);

	strTo.resize(size_needed - 1);
	return strTo;
}
/*template <class T>
bool LookUpInteger(const libconfig::Setting& elm, const char* name, T& res)
{
	long long illTemp = 0;

	if (elm.lookupValue(name, illTemp))
	{
		res = static_cast<T>(illTemp);
		return true;
	}

	return false;
}*/

std::string GetFormatString(const char* fmt, ...)
{
	static char szBuffer[300];
	va_list args;
	va_start(args, fmt);
	vsnprintf(szBuffer, sizeof(szBuffer), fmt, args);
	va_end(args);

	return std::string(szBuffer);
}

CPythonOfflineshop::CPythonOfflineshop()
{
	m_poWindow = nullptr;
#ifdef ENABLE_SHOP_IN_CITIES
	m_bIsShowName = false;
#endif
}

void CPythonOfflineshop::SetWindowObjectPointer(PyObject* poWindow)
{
	m_poWindow = poWindow;
}

PyObject* CPythonOfflineshop::GetOfflineshopBoard()
{
	return m_poWindow;
}

#ifdef ENABLE_SHOP_SEARCH
void CPythonOfflineshop::SetWindowObjectPointerForSearch(PyObject* poWindowForSearch)
{
	m_poWindowForSearch = poWindowForSearch;
}

PyObject* CPythonOfflineshop::GetShopSearchBoard()
{
	return m_poWindowForSearch;
}
#endif

void CPythonOfflineshop::BuyFromSearch(DWORD dwOwnerID, DWORD dwItemID)
{
#ifdef ENABLE_SHOP_SEARCH
	ApplyPyMethodForSearch("SearchFilter_BuyFromSearch", Py_BuildValue("(ii)", dwOwnerID, dwItemID));
#else
	ApplyPyMethod("SearchFilter_BuyFromSearch", Py_BuildValue("(ii)", dwOwnerID, dwItemID));
#endif
}

void CPythonOfflineshop::ShopListAddItem(const offlineshop::TShopInfo& shop)
{
	ApplyPyMethod("ShopListAddItem", Py_BuildValue("(iiis)", shop.dwOwnerID, shop.dwDuration, shop.dwCount, shop.szName));
}

void CPythonOfflineshop::ShopListShow()
{
	ApplyPyMethod("ShopListShow", NOARGS);
}

void CPythonOfflineshop::ShopListClear()
{
	ApplyPyMethod("ShopListClear", NOARGS);
}

void CPythonOfflineshop::OpenShop(const offlineshop::TShopInfo& shop, const std::vector<offlineshop::TItemInfo>& vec)
{
	ApplyPyMethod("OpenShop", Py_BuildValue("(iiis)", shop.dwOwnerID, shop.dwDuration, shop.dwCount, shop.szName));

	for (DWORD i = 0; i < vec.size(); i++)
	{
		const offlineshop::TItemInfo& item = vec[i];
		ApplyPyMethod("OpenShopItem_Alloc", NOARGS);
		{
			ApplyPyMethod("OpenShopItem_SetValue", Py_BuildValue("(sii)", "id", i, item.dwItemID));
			ApplyPyMethod("OpenShopItem_SetValue", Py_BuildValue("(sii)", "vnum", i, item.item.dwVnum));
			ApplyPyMethod("OpenShopItem_SetValue", Py_BuildValue("(sii)", "count", i, item.item.dwCount));
#ifdef ENABLE_CHANGELOOK_SYSTEM
			ApplyPyMethod("OpenShopItem_SetValue", Py_BuildValue("(sii)", "trans", i, item.item.dwTransmutation));
#endif

			for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; j++)
				ApplyPyMethod("OpenShopItem_SetValue", Py_BuildValue("(siiii)", "attr", i, j, item.item.aAttr[j].bType, item.item.aAttr[j].sValue));

			for (int j = 0; j < ITEM_SOCKET_SLOT_MAX_NUM; j++)
				ApplyPyMethod("OpenShopItem_SetValue", Py_BuildValue("(siii)", "socket", i, j, item.item.alSockets[j]));

			ApplyPyMethod("OpenShopItem_SetValue", Py_BuildValue("(siK)", "price", i, item.price.illYang));
#ifdef ENABLE_OFFLINE_SHOP_GRID
			ApplyPyMethod("OpenShopItem_SetValue", Py_BuildValue("(siii)", "display_pos", i, item.item.display_pos.pos, item.item.display_pos.page));
#endif
		}
	}

	ApplyPyMethod("OpenShop_End", NOARGS);
}

void CPythonOfflineshop::OpenShopOwner
(
	const offlineshop::TShopInfo& shop,
	const std::vector<offlineshop::TItemInfo>& vec)
{
	ApplyPyMethod("OpenShopOwner_Start", Py_BuildValue("(iiis)", shop.dwOwnerID, shop.dwDuration, shop.dwCount, shop.szName));
	for (DWORD i = 0; i < vec.size(); i++)
	{
		const offlineshop::TItemInfo& item = vec[i];
		ApplyPyMethod("OpenShopOwnerItem_Alloc", NOARGS);
		{
			ApplyPyMethod("OpenShopOwnerItem_SetValue", Py_BuildValue("(sii)", "id", i, item.dwItemID));
			ApplyPyMethod("OpenShopOwnerItem_SetValue", Py_BuildValue("(sii)", "vnum", i, item.item.dwVnum));
			ApplyPyMethod("OpenShopOwnerItem_SetValue", Py_BuildValue("(sii)", "count", i, item.item.dwCount));
#ifdef ENABLE_CHANGELOOK_SYSTEM
			ApplyPyMethod("OpenShopOwnerItem_SetValue", Py_BuildValue("(sii)", "trans", i, item.item.dwTransmutation));
#endif

			for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; j++)
				ApplyPyMethod("OpenShopOwnerItem_SetValue", Py_BuildValue("(siiii)", "attr", i, j, item.item.aAttr[j].bType, item.item.aAttr[j].sValue));

			for (int j = 0; j < ITEM_SOCKET_SLOT_MAX_NUM; j++)
				ApplyPyMethod("OpenShopOwnerItem_SetValue", Py_BuildValue("(siii)", "socket", i, j, item.item.alSockets[j]));

			ApplyPyMethod("OpenShopOwnerItem_SetValue", Py_BuildValue("(siK)", "price", i, item.price.illYang));
#ifdef ENABLE_OFFLINE_SHOP_GRID
			ApplyPyMethod("OpenShopOwnerItem_SetValue", Py_BuildValue("(siii)", "display_pos", i, item.item.display_pos.pos, item.item.display_pos.page));
#endif
		}

		ApplyPyMethod("OpenShopOwnerItem_Show", NOARGS);
	}

	ApplyPyMethod("OpenShopOwner_End", NOARGS);
}

void CPythonOfflineshop::OpenShopOwnerNoShop()
{
	ApplyPyMethod("OpenShopOwnerNoShop", NOARGS);
}

void CPythonOfflineshop::ShopClose()
{
	ApplyPyMethod("ShopClose", NOARGS);
}
/* if defined shop search ApplyPyMethodForSearch else ApplyPyMethod */
void CPythonOfflineshop::ShopFilterResult(const std::vector<offlineshop::TItemInfo>& vec)
{
	ApplyPyMethodForSearch("ShopFilterResult", Py_BuildValue("(i)", vec.size()));

	for (DWORD i = 0; i < vec.size(); i++)
	{
		const offlineshop::TItemInfo& item = vec[i];
		ApplyPyMethodForSearch("ShopFilterResultItem_Alloc", NOARGS);
		{
			ApplyPyMethodForSearch("ShopFilterResultItem_SetValue", Py_BuildValue("(sii)", "id", i, item.dwItemID));
			ApplyPyMethodForSearch("ShopFilterResultItem_SetValue", Py_BuildValue("(sii)", "vnum", i, item.item.dwVnum));
			ApplyPyMethodForSearch("ShopFilterResultItem_SetValue", Py_BuildValue("(sii)", "count", i, item.item.dwCount));

			ApplyPyMethodForSearch("ShopFilterResultItem_SetValue", Py_BuildValue("(sii)", "owner", i, item.dwOwnerID));
			ApplyPyMethodForSearch("ShopFilterResultItem_SetValue", Py_BuildValue("(sis)", "owner_name", i, item.dwOwnerName));

			for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; j++)
				ApplyPyMethodForSearch("ShopFilterResultItem_SetValue", Py_BuildValue("(siiii)", "attr", i, j, item.item.aAttr[j].bType, item.item.aAttr[j].sValue));

			for (int j = 0; j < ITEM_SOCKET_SLOT_MAX_NUM; j++)
				ApplyPyMethodForSearch("ShopFilterResultItem_SetValue", Py_BuildValue("(siii)", "socket", i, j, item.item.alSockets[j]));

			ApplyPyMethodForSearch("ShopFilterResultItem_SetValue", Py_BuildValue("(siK)", "price", i, item.price.illYang));
		}
	}

	ApplyPyMethodForSearch("ShopFilterResult_Show", NOARGS);
}

void CPythonOfflineshop::SafeboxRefresh(const unsigned long long& valute, const std::vector<DWORD>& ids, const std::vector<offlineshop::TItemInfoEx>& items)
{
	ApplyPyMethod("ShopSafebox_Clear", NOARGS);

	ApplyPyMethod("ShopSafebox_SetValutes", Py_BuildValue("(K)", valute));

	for (DWORD i = 0; i < ids.size(); i++)
	{
		const offlineshop::TItemInfoEx& itemInfo = items[i];
		ApplyPyMethod("ShopSafebox_AllocItem", NOARGS);
		ApplyPyMethod("ShopSafebox_SetValue", Py_BuildValue("(si)", "id", ids[i]));
		ApplyPyMethod("ShopSafebox_SetValue", Py_BuildValue("(si)", "vnum", itemInfo.dwVnum));
		ApplyPyMethod("ShopSafebox_SetValue", Py_BuildValue("(si)", "count", itemInfo.dwCount));
#ifdef ENABLE_CHANGELOOK_SYSTEM
		ApplyPyMethod("ShopSafebox_SetValue", Py_BuildValue("(si)", "count", itemInfo.dwTransmutation));
#endif

		for (int j = 0; j < ITEM_SOCKET_SLOT_MAX_NUM; j++)
			ApplyPyMethod("ShopSafebox_SetValue", Py_BuildValue("(sii)", "socket", j, itemInfo.alSockets[j]));

		for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; j++)
		{
			int iType = itemInfo.aAttr[j].bType;
			int iValue = itemInfo.aAttr[j].sValue;

			ApplyPyMethod("ShopSafebox_SetValue", Py_BuildValue("(sii)", "attr_type", j, iType));
			ApplyPyMethod("ShopSafebox_SetValue", Py_BuildValue("(sii)", "attr_value", j, iValue));

		}
	}

	ApplyPyMethod("ShopSafebox_RefreshEnd", NOARGS);
}

void CPythonOfflineshop::RefreshItemNameMap()
{
	ApplyPyMethod("ClearItemNames", NOARGS);
	std::map<DWORD, std::string> nameMap;
	CItemManager::instance().GetItemsNameMap(nameMap);
	for (auto& it : nameMap)
		ApplyPyMethod("AppendItemName", Py_BuildValue("(is)", it.first, it.second.c_str()));
}

void CPythonOfflineshop::ShopBuilding_AddInventoryItem(int iSlot)
{
	ApplyPyMethod("ShopBuilding_AddInventoryItem", Py_BuildValue("(i)", iSlot));
}

void CPythonOfflineshop::ShopBuilding_AddItem(int iWin, int iSlot)
{
	ApplyPyMethod("ShopBuilding_AddItem", Py_BuildValue("(ii)", iWin, iSlot));
}

#ifdef ENABLE_SHOP_IN_CITIES
void CPythonOfflineshop::InsertEntity(DWORD dwVID, int iType, const char* szName, long x, long y, long z
#ifdef ENABLE_SHOP_DECORATION
, DWORD dwShopDecoration
#endif
)
{
	offlineshop::ShopInstance& shop = *(new offlineshop::ShopInstance());
	shop.SetVID(dwVID);
	shop.SetShopType(iType);
	shop.SetSign(szName);
	CPythonBackground& rkBgMgr = CPythonBackground::Instance();
	rkBgMgr.GlobalPositionToLocalPosition(x, y);
	z = CPythonBackground::Instance().GetHeight(x, y) + 10.0f;
	shop.Show(x, y, z
#ifdef ENABLE_SHOP_DECORATION
	, dwShopDecoration
#endif
	);
	std::string shopname = szName;
	size_t pos = 0;
	if ((pos = shopname.find('@')) != std::string::npos && ++pos != shopname.length())
		shopname = shopname.substr(pos);
	CPythonTextTail::instance().RegisterShopInstanceTextTail(dwVID, shopname.c_str(), shop.GetThingInstancePtr());
	m_vecShopInstance.push_back(&shop);
}

void CPythonOfflineshop::RemoveEntity(DWORD dwVID)
{
	for (auto it = m_vecShopInstance.begin(); it != m_vecShopInstance.end(); it++)
	{
		offlineshop::ShopInstance& shop = *(*it);
		if (shop.GetVID() == dwVID)
		{
			shop.Clear();
			CPythonTextTail::Instance().DeleteShopTextTail(dwVID);
			delete(*it);
			m_vecShopInstance.erase(it);
			return;
		}
	}
}

void CPythonOfflineshop::RenderEntities()
{
#ifdef ENABLE_HIDE_MOB_SYSTEM
	if (!CPythonSystem::Instance().GetHiddenState(3))
	{
#endif
		// fRange == 0.0 means "Hide All"; any positive value means show
		bool bHideAll = (CPythonSystem::Instance().GetOfflineShopRange() <= 0.f);

		for (auto& iter : m_vecShopInstance)
		{
			if (bHideAll)
				continue;
			iter->Render();
			iter->BlendRender();
		}
#ifdef ENABLE_HIDE_MOB_SYSTEM
	}
#endif
}

void CPythonOfflineshop::UpdateEntities()
{
	for (auto& iter : m_vecShopInstance)
	{
		iter->Update();
	}
}

bool CPythonOfflineshop::GetShowNameFlag()
{
	return m_bIsShowName;
}

void CPythonOfflineshop::SetShowNameFlag(bool flag)
{
	m_bIsShowName = flag;
}

void CPythonOfflineshop::DeleteEntities()
{
	for (auto& iter : m_vecShopInstance)
	{
		CPythonTextTail::instance().DeleteShopTextTail(iter->GetVID());
		iter->Clear();
		delete(iter);
	}

	m_vecShopInstance.clear();
}
#endif

#ifdef ENABLE_OFFLINESHOP_NOTIFICATION
void CPythonOfflineshop::SendNotification(DWORD dwItemID, int64_t dwItemPrice,
#ifdef ENABLE_EXTENDED_COUNT_ITEMS
	MAX_COUNT dwItemCount
#else
	uint8_t dwItemCount
#endif
)
{
	ApplyPyMethod("SendNotification", Py_BuildValue("(iLi)", dwItemID, dwItemPrice, dwItemCount));
}
#endif

#ifdef ENABLE_AVERAGE_PRICE
void CPythonOfflineshop::SetAveragePrice(long long price)
{
	ApplyPyMethod("SetAveragePrice", Py_BuildValue("(L)", price));
}
#endif

PyObject* offlineshopSendShopCreate(PyObject* poSelf, PyObject* poArgs)
{
	char* pszName = nullptr;
	DWORD dwDur = 0;
	int iArg = 0;
	PyObject* poTupleItems = nullptr;
	GetPyString(poArgs, &pszName);
	GetPyDWORD(poArgs, &dwDur);
#ifdef ENABLE_SHOP_DECORATION
	DWORD dwShopDecoration = 0;
	GetPyDWORD(poArgs, &dwShopDecoration);
#endif
	GetPyObject(poArgs, &poTupleItems);
	offlineshop::TShopInfo shopInfo;
	shopInfo.dwCount = PyTuple_Size(poTupleItems);
	shopInfo.dwDuration = dwDur;
	shopInfo.dwOwnerID = 0;
	strncpy(shopInfo.szName, pszName, sizeof(shopInfo.szName));
#ifdef ENABLE_SHOP_DECORATION
	shopInfo.dwShopDecoration = dwShopDecoration;
#endif
	std::vector<offlineshop::TShopItemInfo> vec;
	offlineshop::TShopItemInfo temp;
	vec.reserve(shopInfo.dwCount);

	for (int i = 0; i < shopInfo.dwCount; i++)
	{
		iArg = i;
		PyObject* poItemInfo = nullptr;
		GetPyObject(poTupleItems, &poItemInfo);
		int iWindow = 0, iPos = 0;
		long long illYang = 0;
		iArg = 0;
		GetPyInteger(poItemInfo, &iWindow);
		GetPyInteger(poItemInfo, &iPos);
		GetPyLongLong(poItemInfo, &illYang);

#ifdef ENABLE_OFFLINE_SHOP_GRID
		BYTE bPos = 0, bPage = 0;
		offlineshop::TItemDisplayPos display_pos;
		GetPyInteger(poItemInfo, &bPos);
		display_pos.pos = bPos;
		GetPyInteger(poItemInfo, &bPage);
		display_pos.page = bPage;
		offlineshop::CopyObject(temp.display_pos, display_pos);
#endif
		temp.pos = TItemPos((BYTE)iWindow, iPos);
		temp.price.illYang = illYang;
		vec.push_back(temp);
	}

	CPythonNetworkStream::instance().SendOfflineshopShopCreate(shopInfo, vec);
	return Py_BuildNone();
}

PyObject* offlineshopSendChangeName(PyObject* poSelf, PyObject* poArgs)
{
	char* pszName = nullptr;
	int iArg = 0;
	GetPyString(poArgs, &pszName);
	CPythonNetworkStream::instance().SendOfflineshopChangeName(pszName);
	return Py_BuildNone();
}

PyObject* offlineshopSendForceCloseShop(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream::instance().SendOfflineshopForceCloseShop();
	return Py_BuildNone();
}

PyObject* offlineshopSendRequestShopList(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream::instance().SendOfflineshopRequestShopList();
	return Py_BuildNone();
}

PyObject* offlineshopSendOpenShop(PyObject* poSelf, PyObject* poArgs)
{
	int iArg = 0;
	DWORD dwID = 0;
	GetPyDWORD(poArgs, &dwID);
	CPythonNetworkStream::Instance().SendOfflineshopOpenShop(dwID);
	return Py_BuildNone();
}

PyObject* offlineshopSendOpenShopOwner(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream::Instance().SendOfflineshopOpenShopOwner();
	return Py_BuildNone();
}

PyObject* offlineshopSendBuyItem(PyObject* poSelf, PyObject* poArgs)
{
	int iArg = 0;
	DWORD dwOwnerID = 0, dwItemID = 0;
	GetPyDWORD(poArgs, &dwOwnerID);
	GetPyDWORD(poArgs, &dwItemID);
	CPythonNetworkStream::instance().SendOfflineshopBuyItem(dwOwnerID, dwItemID, false);
	return Py_BuildNone();
}

PyObject* offlineshopSendBuyItemFromSearch(PyObject* poSelf, PyObject* poArgs)
{
	int iArg = 0;
	DWORD dwOwnerID = 0, dwItemID = 0;
	GetPyDWORD(poArgs, &dwOwnerID);
	GetPyDWORD(poArgs, &dwItemID);
	CPythonNetworkStream::instance().SendOfflineshopBuyItem(dwOwnerID, dwItemID, true);
	return Py_BuildNone();
}

PyObject* offlineshopSendAddItem(PyObject* poSelf, PyObject* poArgs)
{
	int iArg = 0, iWindow = 0, iSlot = 0;
	long long illYang = 0;
	GetPyInteger(poArgs, &iWindow);
	GetPyInteger(poArgs, &iSlot);
	GetPyLongLong(poArgs, &illYang);
	offlineshop::TShopItemInfo info;

#ifdef ENABLE_OFFLINE_SHOP_GRID
	BYTE bPos = 0, bPage = 0;
	offlineshop::TItemDisplayPos display_pos;
	GetPyInteger(poArgs, &bPos);
	display_pos.pos = bPos;
	GetPyInteger(poArgs, &bPage);
	display_pos.page = bPage;
	offlineshop::CopyObject(info.display_pos, display_pos);
#endif

	TItemPos temp((BYTE)iWindow, iSlot);
	info.price.illYang = illYang;
	offlineshop::CopyObject(info.pos, temp);
	CPythonNetworkStream::instance().SendOfflineshopAddItem(info);
	return Py_BuildNone();
}

PyObject* offlineshopSendRemoveItem(PyObject* poSelf, PyObject* poArgs)
{
	int iArg = 0;
	DWORD dwItemID = 0;
	GetPyDWORD(poArgs, &dwItemID);
	CPythonNetworkStream::instance().SendOfflineshopRemoveItem(dwItemID);
	return Py_BuildNone();
}

PyObject* offlineshopSendEditItem(PyObject* poSelf, PyObject* poArgs)
{
	int iArg = 0;
	long long illYang = 0;
	DWORD dwItemID = 0;
	bool allEdit = false;
	GetPyDWORD(poArgs, &dwItemID);
	GetPyLongLong(poArgs, &illYang);
	GetPyBool(poArgs, &allEdit);
	offlineshop::TPriceInfo price;
	price.illYang = (long long)illYang;
	CPythonNetworkStream::instance().SendOfflineShopEditItem(dwItemID, price, allEdit);
	return Py_BuildNone();
}

PyObject* offlineshopSendFilterRequest(PyObject* poSelf, PyObject* poArgs)
{
	offlineshop::TFilterInfo info;
	char* szName = nullptr;
	PyObject* poPriceTuple = nullptr, * poLevelTuple = nullptr, * poCountTuple = nullptr, * poAbsTuple = nullptr;
	int iArg = 0;

	GetPyInteger(poArgs, &info.bType);
	GetPyInteger(poArgs, &info.bSubType);
	GetPyString(poArgs, &szName);
	GetPyObject(poArgs, &poPriceTuple);
	GetPyObject(poArgs, &poLevelTuple);
	GetPyDWORD(poArgs, &info.dwWearFlag);
	GetPyBool(poArgs, &info.FindCharacter);
	GetPyObject(poArgs, &poCountTuple);
	GetPyObject(poArgs, &poAbsTuple);
	GetPyInteger(poArgs, &info.minAvg);

	GetPyInteger(poArgs, &info.alchemyGrade);
	GetPyInteger(poArgs, &info.alchemyPurity);
	GetPyInteger(poArgs, &info.sashLevel);

	if (info.minAvg < 1)
		info.minAvg = 0;
	else if (info.minAvg > 60)
		info.minAvg = 60;

	std::wstring searchName;
	if (szName != nullptr) 
	{
		std::memset(info.szName, 0, sizeof(info.szName));
		std::strncpy(info.szName, szName, sizeof(info.szName) - 1);

		//searchName = Utf8ToWideString(szName);

		//std::string utf8Name = WideStringToUtf8(searchName);

		//size_t copySize = std::min<size_t>(utf8Name.size(), sizeof(info.szName) - 1);
		//std::memcpy(info.szName, utf8Name.c_str(), copySize);
		//info.szName[copySize] = '\0';
	}
	else
	{
		info.szName[0] = '\0';
	}
	//std::wstring searchName_ = Utf8ToWideString(szName);
	//const auto text_ = WideStringToUtf8(searchName_);

	//strncpy(info.szName, text_.c_str(), text_.size());
	iArg = 0;
	long long priceStart = 0, priceEnd = 0;
	GetPyLongLong(poPriceTuple, &priceStart);
	GetPyLongLong(poPriceTuple, &priceEnd);
	info.priceStart.illYang = priceStart;
	info.priceEnd.illYang = priceEnd;
	
	iArg = 0;
	GetPyInteger(poLevelTuple, &info.iLevelStart);
	GetPyInteger(poLevelTuple, &info.iLevelEnd);
	if (info.iLevelStart < 1)
		info.iLevelStart = 0;
	else if (info.iLevelStart > 120)
		info.iLevelStart = 120;

	if (info.iLevelEnd < 1)
		info.iLevelEnd = 0;
	else if (info.iLevelEnd > 120)
		info.iLevelEnd = 120;

	iArg = 0;
	GetPyInteger(poCountTuple, &info.minCount);
	GetPyInteger(poCountTuple, &info.maxCount);
	if (info.minCount < 1)
		info.minCount = 0;
	else if (info.minCount > 5000)
		info.minCount = 5000;

	if (info.maxCount < 1)
		info.maxCount = 0;
	else if (info.maxCount > 5000)
		info.maxCount = 5000;

	iArg = 0;
	GetPyInteger(poAbsTuple, &info.minAbs);
	GetPyInteger(poAbsTuple, &info.maxAbs);
	if (info.minAbs < 1)
		info.minAbs = 0;
	else if (info.minAbs > 25)
		info.minAbs = 25;

	if (info.maxAbs < 1)
		info.maxAbs = 0;
	else if (info.maxAbs > 25)
		info.maxAbs = 25;

	CPythonNetworkStream::instance().SendOfflineshopFilterRequest(info);
	return Py_BuildNone();
}

PyObject* offlineshopSendSafeboxOpen(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream::instance().SendOfflineshopSafeboxOpen();
	return Py_BuildNone();
}

PyObject* offlineshopSendSafeboxGetItem(PyObject* poSelf, PyObject* poArgs)
{
	int iArg = 0;
	DWORD dwItemID = 0;
	GetPyDWORD(poArgs, &dwItemID);
	CPythonNetworkStream::instance().SendOfflineshopSafeboxGetItem(dwItemID);
	return Py_BuildNone();
}

PyObject* offlineshopSendSafeboxGetValutes(PyObject* poSelf, PyObject* poArgs)
{
	int iArg = 0;
	long long illYang = 0;
	GetPyLongLong(poArgs, &illYang);
	unsigned long long info;
	info = (unsigned long long)illYang;
	CPythonNetworkStream::instance().SendOfflineshopSafeboxGetValutes(info);
	return Py_BuildNone();
}

PyObject* offlineshopSendSafeboxClose(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream::instance().SendOfflineshopSafeboxClose();
	return Py_BuildNone();
}

PyObject* offlineshopSendCloseBoard(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream::instance().SendOfflineshopCloseBoard();
	return Py_BuildNone();
}

PyObject* offlineshopRefreshItemNameMap(PyObject* poSelf, PyObject* poArgs)
{
	CPythonOfflineshop::instance().RefreshItemNameMap();
	return Py_BuildNone();
}

PyObject* offlineshopSetOfflineshopBoard(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* poWin = nullptr;
	int iArg = 0;
	GetPyObject(poArgs, &poWin);
	CPythonOfflineshop::instance().SetWindowObjectPointer(poWin);
	return Py_BuildNone();
}

PyObject* offlineshopGetOfflineshopBoard(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* poInterface = CPythonOfflineshop::instance().GetOfflineshopBoard();

	if (!poInterface)
		return Py_BuildNone();

	Py_IncRef(poInterface);
	return poInterface;
}

PyObject* offlineshopShopBuilding_AddInventoryItem(PyObject* poSelf, PyObject* poArgs)
{
	int iArg = 0, iSlotIndex = 0;
	GetPyInteger(poArgs, &iSlotIndex);
	CPythonOfflineshop::instance().ShopBuilding_AddInventoryItem(iSlotIndex);
	return Py_BuildNone();
}

PyObject* offlineshopShopBuilding_AddItem(PyObject* poSelf, PyObject* poArgs)
{
	int iArg = 0, iSlotIndex = 0, iWin = 0;
	GetPyInteger(poArgs, &iWin);
	GetPyInteger(poArgs, &iSlotIndex);
	CPythonOfflineshop::instance().ShopBuilding_AddItem(iWin, iSlotIndex);
	return Py_BuildNone();
}

#ifdef ENABLE_SHOP_IN_CITIES
PyObject* offlineshopHideShopNames(PyObject* poSelf, PyObject* poArgs)
{
	CPythonOfflineshop::instance().SetShowNameFlag(false);
	return Py_BuildNone();
}

PyObject* offlineshopShowShopNames(PyObject* poSelf, PyObject* poArgs)
{
	CPythonOfflineshop::instance().SetShowNameFlag(true);
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_SHOP_SEARCH
PyObject* offlineshopSetShopSearchBoard(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* poWin = nullptr;
	int iArg = 0;
	GetPyObject(poArgs, &poWin);
	CPythonOfflineshop::instance().SetWindowObjectPointerForSearch(poWin);
	return Py_BuildNone();
}

PyObject* offlineshopGetShopSearchBoard(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* poInterface = CPythonOfflineshop::instance().GetShopSearchBoard();

	if (!poInterface)
		return Py_BuildNone();

	Py_IncRef(poInterface);
	return poInterface;
}
#endif

#ifdef ENABLE_OFFLINESHOP_EXTEND_TIME
PyObject* offlineshopSendExtendTime(PyObject* poSelf, PyObject* poArgs)
{
	//eg		: offlineshop.SendExtendTime(time)
	DWORD dwTime = 0;
	int iArg = 0;
	GetPyDWORD(poArgs, &dwTime);

	CPythonNetworkStream::instance().SendOfflineshopExtendTime(dwTime);
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_AVERAGE_PRICE
PyObject* offlineshopSendAveragePrice(PyObject* poSelf, PyObject* poArgs)
{
	DWORD itemVnum = 0;
	int iArg = 0;
	GetPyDWORD(poArgs, &itemVnum);

	CPythonNetworkStream::instance().SendOfflineShopAveragePrice(itemVnum);
	return Py_BuildNone();
}
#endif

void initofflineshop()
{
	static PyMethodDef s_methods[] =
	{
		{ "SendShopCreate",						offlineshopSendShopCreate,						METH_VARARGS },
		{ "SendChangeName",						offlineshopSendChangeName,						METH_VARARGS },
		{ "SendForceCloseShop",					offlineshopSendForceCloseShop,					METH_VARARGS },
		{ "SendRequestShopList",				offlineshopSendRequestShopList,					METH_VARARGS },
		{ "SendOpenShop",						offlineshopSendOpenShop,						METH_VARARGS },
		{ "SendOpenShopOwner",					offlineshopSendOpenShopOwner,					METH_VARARGS },
		{ "SendBuyItem",						offlineshopSendBuyItem,							METH_VARARGS },
		{ "SendBuyItemFromSearch",				offlineshopSendBuyItemFromSearch,				METH_VARARGS },
		{ "SendAddItem",						offlineshopSendAddItem,							METH_VARARGS },
		{ "SendRemoveItem",						offlineshopSendRemoveItem,						METH_VARARGS },
		{ "SendEditItem",						offlineshopSendEditItem,						METH_VARARGS },
		{ "SendFilterRequest",					offlineshopSendFilterRequest,					METH_VARARGS },
		{ "SendSafeboxOpen",					offlineshopSendSafeboxOpen,						METH_VARARGS },
		{ "SendSafeboxGetItem",					offlineshopSendSafeboxGetItem,					METH_VARARGS },
		{ "SendSafeboxGetValutes",				offlineshopSendSafeboxGetValutes,				METH_VARARGS },
		{ "SendSafeboxClose",					offlineshopSendSafeboxClose,					METH_VARARGS },
		{ "SendCloseBoard",						offlineshopSendCloseBoard,						METH_VARARGS },
		{ "RefreshItemNameMap",					offlineshopRefreshItemNameMap,					METH_VARARGS },
		{ "SetOfflineshopBoard",				offlineshopSetOfflineshopBoard,					METH_VARARGS },
		{ "GetOfflineshopBoard",				offlineshopGetOfflineshopBoard,					METH_VARARGS },
		{ "ShopBuilding_AddInventoryItem",		offlineshopShopBuilding_AddInventoryItem,		METH_VARARGS },
		{ "ShopBuilding_AddItem",				offlineshopShopBuilding_AddItem,				METH_VARARGS },
#ifdef ENABLE_SHOP_IN_CITIES
		{ "HideShopNames",						offlineshopHideShopNames,						METH_VARARGS },
		{ "ShowShopNames",						offlineshopShowShopNames,						METH_VARARGS },
#endif
#ifdef ENABLE_SHOP_SEARCH
		{ "SetShopSearchBoard",				offlineshopSetShopSearchBoard,						METH_VARARGS },
		{ "GetShopSearchBoard",				offlineshopGetShopSearchBoard,						METH_VARARGS },
#endif
#ifdef ENABLE_OFFLINESHOP_EXTEND_TIME
		{ "SendExtendTime",					offlineshopSendExtendTime,							METH_VARARGS },
#endif
#ifdef ENABLE_AVERAGE_PRICE
		{ "SendAveragePrice",				offlineshopSendAveragePrice,						METH_VARARGS},
#endif
		{ NULL,									NULL,									NULL		 },
	};

	PyObject* poModule = Py_InitModule("offlineshop",s_methods);

	PyModule_AddIntConstant(poModule, "FILTER_ATTRIBUTE_NUM",ITEM_ATTRIBUTE_SLOT_NORM_NUM);
	PyModule_AddIntConstant(poModule, "OFFLINESHOP_MAX_DAYS",offlineshop::OFFLINESHOP_DURATION_MAX_DAYS);
	PyModule_AddIntConstant(poModule, "OFFLINESHOP_MAX_HOURS",offlineshop::OFFLINESHOP_DURATION_MAX_HOURS);
	PyModule_AddIntConstant(poModule, "OFFLINESHOP_MAX_MINUTES",offlineshop::OFFLINESHOP_DURATION_MAX_MINUTES);
}

CPythonOfflineshop::~CPythonOfflineshop()
{
	for (auto& iter : m_vecShopInstance)
	{
		delete(iter);
	}

	m_vecShopInstance.clear();
}
#endif