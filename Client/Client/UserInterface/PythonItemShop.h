#pragma once

#include "Packet.h"

enum EEditorFlag
{
	EDITOR_FLAG_ADD,
	EDITOR_FLAG_EDIT,
	EDITOR_FLAG_DELETE
};

enum EItemShopBonusType
{
	BONUS_NONE,
	BONUS_EXP,
	BONUS_PVP,
	BONUS_MAX_NUM,
};

typedef struct STombolaSetting
{
	float fSpinSpeed;
	int iBlinkCount;
	int iMinSpin;
	int iMaxSpin;
	int iSlowMin;
	int iSlowMax;
} TTombolaSetting;

static const TTombolaSetting tombolaSetting = {
	0.058, 6, 1, 3, 4, 10
};

class CItemShop : public CSingleton<CItemShop>
{
public:
	CItemShop();
	virtual						~CItemShop();
	void						SetWnd(PyObject* pyWnd) { m_pyItemShopWnd = pyWnd; }

	void						ClearItems();
	void						SetItemData(TItemShopItem item);
	void						DelItemData(DWORD dwSlotIndex, DWORD dwCategoryIndex);
	void						UpdateItem(TItemShopItem item);
	void						SetBuyStats(TItemShopBuyStats item);
	void						RefreshLimitedCountItem(TItemShopBuyStats item);
	void						AppendCategory(BYTE byIndex, std::string stName);

	void						SetCoins(DWORD coins) { m_dwCoins = coins; }
	DWORD						GetCoins() { return m_dwCoins; }

protected:
	DWORD						m_dwCoins;
	PyObject*					m_pyItemShopWnd;
};
