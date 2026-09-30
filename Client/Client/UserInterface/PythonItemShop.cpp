#include "StdAfx.h"

#ifdef ENABLE_ITEMSHOP
#include "PythonApplication.h"
#include "PythonNetworkStream.h"
#include "PythonItemShop.h"

void ResultItemFromData(TItemShopItem item, PyObject* pItem)
{
	PyDict_SetItemString(pItem, "id", Py_BuildValue("i", item.id));
	PyDict_SetItemString(pItem, "category", Py_BuildValue("i", item.category));
	PyDict_SetItemString(pItem, "price", Py_BuildValue("i", item.price));
	PyDict_SetItemString(pItem, "bonus_type", Py_BuildValue("i", item.bonus_type));
	PyDict_SetItemString(pItem, "vnum", Py_BuildValue("i", item.vnum));
	PyDict_SetItemString(pItem, "count", Py_BuildValue("i", item.count));
	PyDict_SetItemString(pItem, "is_vip", Py_BuildValue("i", item.is_vip));
	PyDict_SetItemString(pItem, "fixed_count", Py_BuildValue("i", item.fixed_count));
	PyDict_SetItemString(pItem, "add_time", Py_BuildValue("i", item.add_time));
	PyDict_SetItemString(pItem, "discount", Py_BuildValue("i", item.discount));
	PyDict_SetItemString(pItem, "discount_time", Py_BuildValue("i", item.discount_time));
	PyDict_SetItemString(pItem, "additional_time", Py_BuildValue("i", item.additional_time));
	PyDict_SetItemString(pItem, "buy", Py_BuildValue("i", 0));
	PyDict_SetItemString(pItem, "limited_time", Py_BuildValue("i", item.limited_time));
	PyDict_SetItemString(pItem, "limited_count", Py_BuildValue("i", item.limited_count));
	PyDict_SetItemString(pItem, "limited_buy_count", Py_BuildValue("i", item.limited_buy_count));

	auto pItemSocket = PyList_New(Py_ssize_t(ITEM_SOCKET_SLOT_MAX_NUM));
	for (int j = 0; j < ITEM_SOCKET_SLOT_MAX_NUM; j++)
		PyList_SetItem(pItemSocket, Py_ssize_t(j), Py_BuildValue("i", item.alSockets[j]));

	PyDict_SetItemString(pItem, "socket", Py_BuildValue("O", pItemSocket));

	auto pItemAttr = PyList_New(Py_ssize_t(ITEM_ATTRIBUTE_SLOT_MAX_NUM));
	for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; j++)
	{
		auto pTempAttr = PyList_New(Py_ssize_t(2));
		PyList_SetItem(pTempAttr, Py_ssize_t(0), Py_BuildValue("i", item.aAttr[j].bType));
		PyList_SetItem(pTempAttr, Py_ssize_t(1), Py_BuildValue("i", item.aAttr[j].sValue));

		PyList_SetItem(pItemAttr, Py_ssize_t(j), Py_BuildValue("O", pTempAttr));
	}

	PyDict_SetItemString(pItem, "attr", Py_BuildValue("O", pItemAttr));
}

void CItemShop::ClearItems()
{
	PyCallClassMemberFunc(m_pyItemShopWnd, "BINARY_ClearItem", Py_BuildValue("()"));
}

void CItemShop::SetItemData(TItemShopItem item)
{
	auto pItem = PyDict_New();
	ResultItemFromData(item, pItem);

	PyCallClassMemberFunc(m_pyItemShopWnd, "BINARY_AddItem", Py_BuildValue("(O)", pItem));

}

void CItemShop::DelItemData(DWORD dwSlotIndex, DWORD dwCategoryIndex)
{
	PyCallClassMemberFunc(m_pyItemShopWnd, "BINARY_RemoveItem", Py_BuildValue("(i)", dwSlotIndex));
}

void CItemShop::UpdateItem(TItemShopItem item)
{
	auto pItem = PyDict_New();
	ResultItemFromData(item, pItem);

	PyCallClassMemberFunc(m_pyItemShopWnd, "BINARY_UpdateItem", Py_BuildValue("(O)", pItem));
}

void CItemShop::SetBuyStats(TItemShopBuyStats item)
{
	PyCallClassMemberFunc(m_pyItemShopWnd, "BINARY_SetBuyStats", Py_BuildValue("(iii)", item.dwID, item.dwCategory, item.dwCount));
}

void CItemShop::RefreshLimitedCountItem(TItemShopBuyStats item)
{
	PyCallClassMemberFunc(m_pyItemShopWnd, "BINARY_RefreshLimitedCountItem", Py_BuildValue("(iii)", item.dwID, item.dwCategory, item.dwCount));
}

void CItemShop::AppendCategory(BYTE byIndex, std::string stName)
{
	PyCallClassMemberFunc(m_pyItemShopWnd, "BINARY_AppendCategory", Py_BuildValue("(is)", byIndex, stName.c_str()));
}

CItemShop::CItemShop() : m_dwCoins(0), m_pyItemShopWnd{ nullptr }
{
	m_dwCoins = 0;
	m_pyItemShopWnd = nullptr;
}

CItemShop::~CItemShop()
{
	m_dwCoins = 0;
	m_pyItemShopWnd = nullptr;
}

PyObject* itemshopGetCoins(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CItemShop::Instance().GetCoins());
}

PyObject* itemshopSendItemShopPacket(PyObject* poSelf, PyObject* poArgs)
{
	int ipos, icategory, ivnum, iprice, icount;
	BYTE bfixedcount, bflag;

	long alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];

	if (!PyTuple_GetInteger(poArgs, 0, &ipos))
		return Py_BadArgument();

	if (!PyTuple_GetInteger(poArgs, 1, &icategory))
		return Py_BadArgument();

	if (!PyTuple_GetInteger(poArgs, 2, &ivnum))
		return Py_BadArgument();

	if (!PyTuple_GetInteger(poArgs, 3, &iprice))
		return Py_BadArgument();

	if (!PyTuple_GetInteger(poArgs, 4, &icount))
		return Py_BadArgument();

	if (!PyTuple_GetByte(poArgs, 5, &bfixedcount))
		return Py_BadArgument();

	PyObject* poSocket = PyTuple_GetItem(poArgs, 6);
	if (!PyList_Check(poSocket))
		return Py_BadArgument();

	for (int i = 0; i < ITEM_SOCKET_SLOT_MAX_NUM; i++)
	{
		alSockets[i] = PyInt_AsLong(PyList_GetItem(poSocket, i));
	}

	PyObject* poAttr = PyTuple_GetItem(poArgs, 7);
	if (!PyList_Check(poAttr))
		return Py_BadArgument();

	for (int i = 0; i < ITEM_ATTRIBUTE_SLOT_MAX_NUM; i++)
	{
		PyObject* poTempAttr = PyList_GetItem(poAttr, i);
		if (!poTempAttr)
		{
			aAttr[i].bType = 0;
			aAttr[i].sValue = 0;

			continue;
		}

		aAttr[i].bType = PyInt_AsLong(PyList_GetItem(poTempAttr, 0));
		aAttr[i].sValue = PyInt_AsLong(PyList_GetItem(poTempAttr, 1));
	}

	if (!PyTuple_GetByte(poArgs, 8, &bflag))
		return Py_BadArgument();

	TItemShopItem aItemShopItem = {};
	aItemShopItem.id = ipos;
	aItemShopItem.category = icategory;
	aItemShopItem.vnum = ivnum;
	aItemShopItem.count = icount;
	aItemShopItem.price = iprice;
	aItemShopItem.fixed_count = bfixedcount;

	memcpy(aItemShopItem.alSockets, alSockets, sizeof(alSockets));
	memcpy(aItemShopItem.aAttr, aAttr, sizeof(aAttr));

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendItemShopAddItemPacket(aItemShopItem, bflag);

	return Py_BuildNone();
}

PyObject* itemshopSetWnd(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* pyWnd;

	if (!PyTuple_GetObject(poArgs, 0, &pyWnd))
		return Py_BadArgument();

	CItemShop::instance().SetWnd(pyWnd);

	return Py_BuildNone();
}


void inititemshop()
{
	static PyMethodDef s_methods[] =
	{
		{ "GetCoins", itemshopGetCoins, METH_VARARGS },
		{ "SetWnd",	itemshopSetWnd,	METH_VARARGS, },

		{ "SendItemShopPacket", itemshopSendItemShopPacket, METH_VARARGS },

		{ NULL, NULL, NULL },
	};

	PyObject* poModule = Py_InitModule("itemshop", s_methods);

	PyModule_AddIntConstant(poModule, "EDITOR_FLAG_ADD", EDITOR_FLAG_ADD);
	PyModule_AddIntConstant(poModule, "EDITOR_FLAG_EDIT", EDITOR_FLAG_EDIT);
	PyModule_AddIntConstant(poModule, "EDITOR_FLAG_DELETE", EDITOR_FLAG_DELETE);
	PyModule_AddIntConstant(poModule, "ENABLE_EXTENDED_COIN", 0);

#ifdef ENABLE_TOMBOLA_PERCENTAGE
	PyModule_AddIntConstant(poModule, "ENABLE_TOMBOLA_PERCENTAGE", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_TOMBOLA_PERCENTAGE", 0);
#endif

	PyModule_AddIntConstant(poModule, "ERROR_NOT_ENOUGH_COINS", ITEMSHOP_ERROR_NOT_ENOUGH_COINS);
	PyModule_AddIntConstant(poModule, "ERROR_INVENTORY_FULL", ITEMSHOP_ERROR_INVENTORY_FULL);
	PyModule_AddIntConstant(poModule, "ERROR_WARP", ITEMSHOP_ERROR_WARP);
	PyModule_AddIntConstant(poModule, "ERROR_NON_EDITOR", ITEMSHOP_ERROR_NON_EDITOR);
	PyModule_AddIntConstant(poModule, "ERROR_COUNT", ITEMSHOP_ERROR_COUNT);
	PyModule_AddIntConstant(poModule, "ERROR_UNKNOWN_ERROR", ITEMSHOP_ERROR_UNKNOWN_ERROR);	
	
	PyModule_AddIntConstant(poModule, "BONUS_NONE", BONUS_NONE);
	PyModule_AddIntConstant(poModule, "BONUS_EXP", BONUS_EXP);
	PyModule_AddIntConstant(poModule, "BONUS_PVP", BONUS_PVP);
	PyModule_AddIntConstant(poModule, "BONUS_MAX_NUM", BONUS_MAX_NUM);	

	PyModule_AddIntConstant(poModule, "ITEMSHOP_POPUP_ITEM_PROMO", ITEMSHOP_POPUP_ITEM_PROMO);
	PyModule_AddIntConstant(poModule, "ITEMSHOP_POPUP_ITEM_LIMITED", ITEMSHOP_POPUP_ITEM_LIMITED);
	PyModule_AddIntConstant(poModule, "ITEMSHOP_POPUP_NEW_ITEM", ITEMSHOP_POPUP_NEW_ITEM);

	PyModule_AddObject(poModule, "TOMBOLA_SPIN_SPEED", Py_BuildValue("f", tombolaSetting.fSpinSpeed));
	PyModule_AddIntConstant(poModule, "TOMBOLA_BLINK_COUNT", tombolaSetting.iBlinkCount);
	PyModule_AddIntConstant(poModule, "TOMBOLA_MIN_SPIN", tombolaSetting.iMinSpin);
	PyModule_AddIntConstant(poModule, "TOMBOLA_MAX_SPIN", tombolaSetting.iMaxSpin);
	PyModule_AddIntConstant(poModule, "TOMBOLA_SLOW_MIN", tombolaSetting.iSlowMin);
	PyModule_AddIntConstant(poModule, "TOMBOLA_SLOW_MAX", tombolaSetting.iSlowMax);
}

#endif
