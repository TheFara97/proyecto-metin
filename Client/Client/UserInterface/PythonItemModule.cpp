#include "StdAfx.h"
#include "PythonItem.h"

#include "../gamelib/ItemManager.h"
#include "../gamelib/GameLibDefines.h"
#include "InstanceBase.h"
#include "AbstractApplication.h"

extern int TWOHANDED_WEWAPON_ATT_SPEED_DECREASE_VALUE;

PyObject* itemGetItemsByName(PyObject* poSelf, PyObject* poArgs)
{
	char* szItemName;
	if (!PyTuple_GetString(poArgs, 0, &szItemName))
		return Py_BadArgument();

	std::map<DWORD, CItemData*> m_vec_ItemRange = CItemManager::Instance().GetItemsVector();
	int n = m_vec_ItemRange.size();

	std::string searchedFileName(szItemName);
	std::transform(searchedFileName.begin(), searchedFileName.end(), searchedFileName.begin(),[](unsigned char c) { 
		return std::tolower(c); 
	});

	PyObject* poList = PyList_New(0);

	std::map<DWORD, CItemData*>::iterator i;
	for (i = m_vec_ItemRange.begin(); i != m_vec_ItemRange.end(); ++i)
	{
		CItemData* p = i->second;
		const CItemData::TItemTable* pTable = p->GetTable();
		if (pTable)
		{
			std::string tempFileName(p->GetName());
			std::transform(tempFileName.begin(), tempFileName.end(), tempFileName.begin(), [](unsigned char c) { 
				return std::tolower(c); 
			});

			if (tempFileName.compare(0, searchedFileName.length(), searchedFileName) == 0)
			{
				PyObject* poItemVnum = PyLong_FromUnsignedLong(pTable->dwVnum);
				PyList_Append(poList, poItemVnum);
			}
		}
	}

	return poList;
}

PyObject * itemSetUseSoundFileName(PyObject * poSelf, PyObject * poArgs)
{
	int iUseSound;
	if (!PyTuple_GetInteger(poArgs, 0, &iUseSound))
		return Py_BadArgument();

	char* szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
		return Py_BadArgument();

	CPythonItem& rkItem=CPythonItem::Instance();
	rkItem.SetUseSoundFileName(iUseSound, szFileName);
	return Py_BuildNone();
}

PyObject * itemSetDropSoundFileName(PyObject * poSelf, PyObject * poArgs)
{
	int iDropSound;
	if (!PyTuple_GetInteger(poArgs, 0, &iDropSound))
		return Py_BadArgument();

	char* szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
		return Py_BadArgument();

	CPythonItem& rkItem=CPythonItem::Instance();
	rkItem.SetDropSoundFileName(iDropSound, szFileName);
	return Py_BuildNone();
}

PyObject * itemSelectItem(PyObject * poSelf, PyObject * poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();

	if (!CItemManager::Instance().SelectItemData(iIndex))
	{
#ifndef ENABLE_OFFLINE_SHOP_SYSTEM
		TraceError("Cannot find item by %d", iIndex);
#endif
		CItemManager::Instance().SelectItemData(60001);
	}

	return Py_BuildNone();
}

PyObject * itemGetItemName(PyObject * poSelf, PyObject * poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("s", pItemData->GetName());
}

PyObject* itemGetItemVnum(PyObject* poSelf, PyObject* poArgs)
{
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->GetIndex());
}

PyObject * itemGetItemDescription(PyObject * poSelf, PyObject * poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("s", pItemData->GetDescription());
}

PyObject * itemGetItemSummary(PyObject * poSelf, PyObject * poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("s", pItemData->GetSummary());
}

#ifdef WJ_SPLIT_INVENTORY_SYSTEM
PyObject* itemGetSpecialInvType(PyObject* poSelf, PyObject* poArgs)
{
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->GetSpecialInvType());
}

PyObject* itemCanUseInSpecialInv(PyObject* poSelf, PyObject* poArgs)
{
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->CanUseIn_SpecialInv());
}
#endif

#include "Packet.h"
#include <unordered_map>
PyObject* itemGetApplyPoint(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bVal;
	if (!PyTuple_GetByte(poArgs, 0, &bVal))
		return Py_BuildException();

	static const std::unordered_map<BYTE, BYTE> PointMap
	{
		{ CItemData::APPLY_ATTBONUS_HUMAN, POINT_ATTBONUS_HUMAN },
		{ CItemData::APPLY_ATTBONUS_ORC, POINT_ATTBONUS_ORC },
		{ CItemData::APPLY_ATTBONUS_UNDEAD, POINT_ATTBONUS_UNDEAD },
		{ CItemData::APPLY_ATTBONUS_ANIMAL, POINT_ATTBONUS_ANIMAL },
		{ CItemData::APPLY_ATTBONUS_MILGYO, POINT_ATTBONUS_MILGYO },
		{ CItemData::APPLY_ATTBONUS_DEVIL, POINT_ATTBONUS_DEVIL },
		{ CItemData::APPLY_ATT_GRADE_BONUS, POINT_ATT_GRADE_BONUS },
		{ CItemData::APPLY_DEF_GRADE_BONUS, POINT_DEF_GRADE_BONUS },
		{ CItemData::APPLY_NORMAL_HIT_DAMAGE_BONUS, POINT_NORMAL_HIT_DAMAGE_BONUS },
		{ CItemData::APPLY_SKILL_DAMAGE_BONUS, POINT_SKILL_DAMAGE_BONUS },
		{ CItemData::APPLY_SKILL_DEFEND_BONUS, POINT_SKILL_DEFEND_BONUS },
		{ CItemData::APPLY_MAGIC_ATTBONUS_PER, POINT_MAGIC_ATT_GRADE_BONUS },
		{ CItemData::APPLY_CRITICAL_PCT, POINT_CRITICAL_PCT },
		{ CItemData::APPLY_PENETRATE_PCT, POINT_PENETRATE_PCT },
		{ CItemData::APPLY_RESIST_MAGIC, POINT_RESIST_MAGIC },
		{ CItemData::APPLY_RESIST_ELEC, POINT_RESIST_ELEC },
		{ CItemData::APPLY_RESIST_FIRE, POINT_RESIST_FIRE },
		{ CItemData::APPLY_RESIST_WIND, POINT_RESIST_WIND },
		//----------------FIND START----------------
		{ CItemData::APPLY_ATT_BONUS_TO_MONSTER, POINT_ATTBONUS_MONSTER },
		{ CItemData::APPLY_MELEE_MAGIC_ATTBONUS_PER, POINT_MAGIC_ATT_BONUS_PER },
		{ CItemData::APPLY_ANTI_CRITICAL_PCT, POINT_RESIST_CRITICAL },
		{ CItemData::APPLY_ANTI_PENETRATE_PCT, POINT_RESIST_PENETRATE },
		{ CItemData::APPLY_RESIST_ICE, POINT_RESIST_ICE },
		{ CItemData::APPLY_RESIST_DARK, POINT_RESIST_DARK },
		{ CItemData::APPLY_RESIST_EARTH, POINT_RESIST_EARTH },
		{ CItemData::APPLY_ATT_BONUS_TO_WARRIOR, POINT_ATTBONUS_WARRIOR },
		{ CItemData::APPLY_ATT_BONUS_TO_ASSASSIN, POINT_ATTBONUS_ASSASSIN },
		{ CItemData::APPLY_ATT_BONUS_TO_SURA, POINT_ATTBONUS_SURA },
		{ CItemData::APPLY_ATT_BONUS_TO_SHAMAN, POINT_ATTBONUS_SHAMAN },
		{ CItemData::APPLY_RESIST_WARRIOR, POINT_RESIST_WARRIOR },
		{ CItemData::APPLY_RESIST_ASSASSIN, POINT_RESIST_ASSASSIN },
		{ CItemData::APPLY_RESIST_SURA, POINT_RESIST_SURA },
		{ CItemData::APPLY_RESIST_SHAMAN, POINT_RESIST_SHAMAN },
		//----------------END OF FIND----------------
		{ CItemData::APPLY_RESIST_SWORD, POINT_RESIST_SWORD },
		{ CItemData::APPLY_RESIST_TWOHAND, POINT_RESIST_TWOHAND },
		{ CItemData::APPLY_RESIST_DAGGER, POINT_RESIST_DAGGER },
		{ CItemData::APPLY_RESIST_BELL, POINT_RESIST_BELL },
		{ CItemData::APPLY_RESIST_FAN, POINT_RESIST_FAN },
		{ CItemData::APPLY_RESIST_BOW, POINT_RESIST_BOW },
		{ CItemData::APPLY_STUN_PCT, POINT_STUN_PCT },
		{ CItemData::APPLY_SLOW_PCT, POINT_SLOW_PCT },
		{ CItemData::APPLY_POISON_PCT, POINT_POISON_PCT },
		{ CItemData::APPLY_POISON_REDUCE, POINT_POISON_REDUCE },
		{ CItemData::APPLY_STEAL_HP, POINT_STEAL_HP },
		{ CItemData::APPLY_STEAL_SP, POINT_STEAL_SP },
		{ CItemData::APPLY_HP_REGEN, POINT_HP_REGEN },
		{ CItemData::APPLY_SP_REGEN, POINT_SP_REGEN },
		{ CItemData::APPLY_BLOCK, POINT_BLOCK },
		{ CItemData::APPLY_DODGE, POINT_DODGE },
		{ CItemData::APPLY_REFLECT_MELEE, POINT_REFLECT_MELEE },
		{ CItemData::APPLY_KILL_HP_RECOVER, POINT_KILL_HP_RECOVER },
		{ CItemData::APPLY_KILL_SP_RECOVER, POINT_KILL_SP_RECOVER },
		{ CItemData::APPLY_EXP_DOUBLE_BONUS, POINT_EXP_DOUBLE_BONUS },
		{ CItemData::APPLY_GOLD_DOUBLE_BONUS, POINT_GOLD_DOUBLE_BONUS },
		{ CItemData::APPLY_ITEM_DROP_BONUS, POINT_ITEM_DROP_BONUS },
		{ CItemData::APPLY_MALL_ATTBONUS, POINT_ATT_BONUS },
		{ CItemData::APPLY_MALL_DEFBONUS, POINT_MALL_DEFBONUS },
		{ CItemData::APPLY_MALL_EXPBONUS, POINT_MALL_EXPBONUS },
		{ CItemData::APPLY_MALL_ITEMBONUS, POINT_MALL_ITEMBONUS },
		{ CItemData::APPLY_MALL_GOLDBONUS, POINT_MALL_GOLDBONUS },
		{ CItemData::APPLY_NORMAL_HIT_DEFEND_BONUS, POINT_NORMAL_HIT_DEFEND_BONUS},
		{ CItemData::APPLY_ATTBONUS_WLADCA, POINT_ATTBONUS_WLADCA },
		{ CItemData::APPLY_ATTBONUS_LEGENDA, POINT_ATTBONUS_LEGENDA },
		{ CItemData::APPLY_ATTBONUS_STONE, POINT_ATTBONUS_STONE },
		{ CItemData::APPLY_ATTBONUS_BOSS, POINT_ATTBONUS_BOSS },
		{ CItemData::APPLY_RESIST_MONSTER, POINT_RESIST_MONSTER },
		{ CItemData::APPLY_ENCHANT_ELECT, POINT_ENCHANT_ELECT },
		{ CItemData::APPLY_ENCHANT_EARTH, POINT_ENCHANT_EARTH },
		{ CItemData::APPLY_ENCHANT_FIRE, POINT_ENCHANT_FIRE },
		{ CItemData::APPLY_ENCHANT_DARK, POINT_ENCHANT_DARK },
		{ CItemData::APPLY_ENCHANT_WIND, POINT_ENCHANT_WIND },
		{ CItemData::APPLY_ENCHANT_ICE, POINT_ENCHANT_ICE },
		{ CItemData::APPLY_MINING_SUCCESS_CHANCE, POINT_MINING_SUCCESS_CHANCE },
		{ CItemData::APPLY_MINING_MORE_ORES_CHANCE, POINT_MINING_MORE_ORES_CHANCE },
		{ CItemData::APPLY_MINING_LESS_MINE_TIME, POINT_MINING_LESS_MINE_TIME },
		{ CItemData::APPLY_MINING_MORE_POINTS, POINT_MINING_MORE_POINTS },
		{ CItemData::APPLY_FISHING_INSTANT_CATCH_CHANCE, POINT_FISHING_INSTANT_CATCH_CHANCE },
		{ CItemData::APPLY_FISHING_BIGGER_FISH_CHANCE, POINT_FISHING_BIGGER_FISH_CHANCE },
		{ CItemData::APPLY_FISHING_NEXT_SECTION_CHANCE, POINT_FISHING_NEXT_SECTION_CHANCE },
		{ CItemData::APPLY_FISHING_MORE_YANG_MULTIPLIER, POINT_FISHING_MORE_YANG_MULTIPLIER },
		{ CItemData::APPLY_FISHING_MORE_POINTS_CHANCE, POINT_FISHING_MORE_POINTS_CHANCE },
		{ CItemData::APPLY_FISHING_SUCCESS_CHANCE, POINT_FISHING_SUCCESS_CHANCE },
		{ CItemData::APPLY_ATTBONUS_PVM_SKILL, POINT_ATTBONUS_PVM_SKILL },
		{ CItemData::APPLY_ATTBONUS_ELEMENT, POINT_ATTBONUS_ELEMENT },
		{ CItemData::APPLY_DEFBONUS_ELEMENT, POINT_DEFBONUS_ELEMENT },
		{ CItemData::APPLY_DOUBLE_LOOT_SUMMER, POINT_DOUBLE_LOOT_SUMMER },
		{ CItemData::APPLY_DOUBLE_LOOT_DROP, POINT_DOUBLE_LOOT_DROP },
		{ CItemData::APPLY_ATTBONUS_PVM_STR, POINT_ATTBONUS_PVM_STR },

	};

	BYTE bRet = POINT_MAX_NUM;

	auto it = PointMap.find(bVal);
	if (it != PointMap.end())
		bRet = it->second;

	return Py_BuildValue("i", bRet);
}

PyObject * itemGetIconImage(PyObject * poSelf, PyObject * poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

//	if (CItemData::ITEM_TYPE_SKILLBOOK == pItemData->GetType())
//	{
//		char szItemName[64+1];
//		_snprintf(szItemName, "d:/ymir work/ui/items/etc/book_%02d.sub", );
//		CGraphicImage * pImage = (CGraphicImage *)CResourceManager::Instance().GetResourcePointer(szItemName);
//	}

	return Py_BuildValue("i", pItemData->GetIconImage());
}

PyObject * itemGetIconImageFileName(PyObject * poSelf, PyObject * poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	CGraphicSubImage * pImage = pItemData->GetIconImage();
	if (!pImage)
		return Py_BuildValue("s", "Noname");

	return Py_BuildValue("s", pImage->GetFileName());
}

PyObject * itemGetItemSize(PyObject * poSelf, PyObject * poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("(ii)", 1, pItemData->GetSize());
}

PyObject * itemGetItemType(PyObject * poSelf, PyObject * poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->GetType());
}

PyObject * itemGetItemSubType(PyObject * poSelf, PyObject * poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->GetSubType());
}

#ifdef ENABLE_LONG_LONG
PyObject * itemGetIBuyItemPrice(PyObject * poSelf, PyObject * poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();

	if (!pItemData)
		return Py_BuildException("no selected item data");

	return PyLong_FromLongLong(pItemData->GetIBuyItemPrice());
}

PyObject * itemGetISellItemPrice(PyObject * poSelf, PyObject * poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();

	if (!pItemData)
		return Py_BuildException("no selected item data");

	return PyLong_FromLongLong(pItemData->GetISellItemPrice());
}

#else
PyObject * itemGetIBuyItemPrice(PyObject * poSelf, PyObject * poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();

	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->GetIBuyItemPrice());
}

PyObject * itemGetISellItemPrice(PyObject * poSelf, PyObject * poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();

	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->GetISellItemPrice());
}
#endif

PyObject * itemIsAntiFlag(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BadArgument();

	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->IsAntiFlag(iFlag));
}

PyObject * itemIsFlag(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BadArgument();

	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->IsFlag(iFlag));
}

PyObject * itemIsWearableFlag(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BadArgument();

	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->IsWearableFlag(iFlag));
}

PyObject * itemIs1GoldItem(PyObject * poSelf, PyObject * poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->IsFlag(CItemData::ITEM_FLAG_COUNT_PER_1GOLD));
}

PyObject * itemGetLimit(PyObject * poSelf, PyObject * poArgs)
{
	int iValueIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iValueIndex))
		return Py_BadArgument();

	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Not yet select item data");

	CItemData::TItemLimit ItemLimit;
	if (!pItemData->GetLimit(iValueIndex, &ItemLimit))
		return Py_BuildException();

	return Py_BuildValue("ii", ItemLimit.bType, ItemLimit.lValue);
}

PyObject * itemGetAffect(PyObject * poSelf, PyObject * poArgs)
{
	int iValueIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iValueIndex))
		return Py_BadArgument();

	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Not yet select item data");

	CItemData::TItemApply ItemApply;
	if (!pItemData->GetApply(iValueIndex, &ItemApply))
		return Py_BuildException();

	if ((CItemData::APPLY_ATT_SPEED == ItemApply.bType) && (CItemData::ITEM_TYPE_WEAPON == pItemData->GetType()) && (CItemData::WEAPON_TWO_HANDED == pItemData->GetSubType()))
	{
		ItemApply.lValue -= TWOHANDED_WEWAPON_ATT_SPEED_DECREASE_VALUE;
	}

	return Py_BuildValue("ii", ItemApply.bType, ItemApply.lValue);
}

PyObject * itemGetValue(PyObject * poSelf, PyObject * poArgs)
{
	int iValueIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iValueIndex))
		return Py_BadArgument();

	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Not yet select item data");

	return Py_BuildValue("i", pItemData->GetValue(iValueIndex));
}

PyObject * itemGetSocket(PyObject * poSelf, PyObject * poArgs)
{
	int iValueIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iValueIndex))
		return Py_BadArgument();

	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Not yet select item data");

	return Py_BuildValue("i", pItemData->GetSocket(iValueIndex));
}

PyObject * itemGetIconInstance(PyObject * poSelf, PyObject * poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Not yet select item data");

	CGraphicSubImage * pImage = pItemData->GetIconImage();
	if (!pImage)
		return Py_BuildException("Cannot get icon image by %d", pItemData->GetIndex());

	CGraphicImageInstance * pImageInstance = CGraphicImageInstance::New();
	pImageInstance->SetImagePointer(pImage);

	return Py_BuildValue("i", pImageInstance);
}

PyObject * itemDeleteIconInstance(PyObject * poSelf, PyObject * poArgs)
{
	int iHandle;
	if (!PyTuple_GetInteger(poArgs, 0, &iHandle))
		return Py_BadArgument();

	CGraphicImageInstance::Delete((CGraphicImageInstance *) iHandle);

	return Py_BuildNone();
}

PyObject * itemIsEquipmentVID(PyObject * poSelf, PyObject * poArgs)
{
	int iItemVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemVID))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemVID);
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Not yet select item data");

	return Py_BuildValue("i", pItemData->IsEquipment());
}

PyObject* itemGetUseType(PyObject * poSelf, PyObject * poArgs)
{
	int iItemVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemVID))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemVID);
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	return Py_BuildValue("s", pItemData->GetUseTypeString());
}

PyObject * itemIsRefineScroll(PyObject * poSelf, PyObject * poArgs)
{
	int iItemIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemIndex))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemIndex);
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	if (pItemData->GetType() != CItemData::ITEM_TYPE_USE)
		return Py_BuildValue("i", FALSE);

	switch (pItemData->GetSubType())
	{
		case CItemData::USE_TUNING:
			return Py_BuildValue("i", TRUE);
			break;
	}

	return Py_BuildValue("i", FALSE);
}

PyObject * itemIsDetachScroll(PyObject * poSelf, PyObject * poArgs)
{
	int iItemIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemIndex))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemIndex);
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	int iType = pItemData->GetType();
	int iSubType = pItemData->GetSubType();
	if (iType == CItemData::ITEM_TYPE_USE)
	if (iSubType == CItemData::USE_DETACHMENT)
	{
		return Py_BuildValue("i", TRUE);
	}

	return Py_BuildValue("i", FALSE);
}

PyObject * itemCanAddToQuickSlotItem(PyObject * poSelf, PyObject * poArgs)
{
	int iItemIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemIndex))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemIndex);
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	if (CItemData::ITEM_TYPE_USE == pItemData->GetType() || CItemData::ITEM_TYPE_QUEST == pItemData->GetType()
		|| iItemIndex == 70104 || iItemIndex == 70105 || iItemIndex == 70106
		)
	{
		return Py_BuildValue("i", TRUE);
	}

	return Py_BuildValue("i", FALSE);
}

PyObject * itemIsKey(PyObject * poSelf, PyObject * poArgs)
{
	int iItemIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemIndex))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemIndex);
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	if (CItemData::ITEM_TYPE_TREASURE_KEY == pItemData->GetType())
	{
		return Py_BuildValue("i", TRUE);
	}

	return Py_BuildValue("i", FALSE);
}

PyObject * itemIsMetin(PyObject * poSelf, PyObject * poArgs)
{
	int iItemIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemIndex))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemIndex);
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	if (CItemData::ITEM_TYPE_METIN == pItemData->GetType())
	{
		return Py_BuildValue("i", TRUE);
	}

	return Py_BuildValue("i", FALSE);
}

PyObject * itemRender(PyObject * poSelf, PyObject * poArgs)
{
	CPythonItem::Instance().Render();
	return Py_BuildNone();
}

PyObject * itemUpdate(PyObject * poSelf, PyObject * poArgs)
{
	IAbstractApplication& rkApp=IAbstractApplication::GetSingleton();

	POINT ptMouse;
	rkApp.GetMousePosition(&ptMouse);

	CPythonItem::Instance().Update(ptMouse);
	return Py_BuildNone();
}

PyObject * itemCreateItem(PyObject * poSelf, PyObject * poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BadArgument();
	int iVirtualNumber;
	if (!PyTuple_GetInteger(poArgs, 1, &iVirtualNumber))
		return Py_BadArgument();

	float x;
	if (!PyTuple_GetFloat(poArgs, 2, &x))
		return Py_BadArgument();
	float y;
	if (!PyTuple_GetFloat(poArgs, 3, &y))
		return Py_BadArgument();
	float z;
	if (!PyTuple_GetFloat(poArgs, 4, &z))
		return Py_BadArgument();

	int count;
	if (!PyTuple_GetInteger(poArgs, 5, &count))
		return Py_BadArgument();

	bool bDrop = true;
	PyTuple_GetBoolean(poArgs, 6, &bDrop);

	CPythonItem::Instance().CreateItem(iVirtualID, iVirtualNumber, x, y, z, count, bDrop);

	return Py_BuildNone();
}

PyObject * itemDeleteItem(PyObject * poSelf, PyObject * poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BadArgument();

	CPythonItem::Instance().DeleteItem(iVirtualID);
	return Py_BuildNone();
}

PyObject * itemPick(PyObject * poSelf, PyObject * poArgs)
{
	DWORD dwItemID;
	if (CPythonItem::Instance().GetPickedItemID(&dwItemID))
		return Py_BuildValue("i", dwItemID);
	else
		return Py_BuildValue("i", -1);
}

PyObject* itemLoadItemTable(PyObject* poSelf, PyObject* poArgs)
{
	char * szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BadArgument();

	CItemManager::Instance().LoadItemTable(szFileName);
	return Py_BuildNone();
}

#ifdef ENABLE_ITEMSHOP
PyObject* itemGetWeaponSockets(PyObject* poSelf, PyObject* poArgs)
{
	CItemManager::TItemMap m_ItemMap = CItemManager::Instance().GetItems();
	CItemManager::TItemMap::iterator f = m_ItemMap.begin();
	std::vector<DWORD> m_SocketVec;
	int i = 0;
	while (m_ItemMap.end() != f)
	{
		if (f->second->GetType() == CItemData::ITEM_TYPE_METIN && f->second->GetWearFlags() == CItemData::WEARABLE_WEAPON)
			m_SocketVec.push_back(f->second->GetIndex());
		f++;
	}
	PyObject* tuple = PyTuple_New((Py_ssize_t)m_SocketVec.size());

	for (int i = 0; i < m_SocketVec.size(); i++)
	{
		PyTuple_SetItem(tuple, i, Py_BuildValue("i", m_SocketVec[i]));
	}
	return tuple;
}
PyObject* itemGetBodySockets(PyObject* poSelf, PyObject* poArgs)
{
	CItemManager::TItemMap m_ItemMap = CItemManager::Instance().GetItems();
	CItemManager::TItemMap::iterator f = m_ItemMap.begin();
	std::vector<DWORD> m_SocketVec;
	int i = 0;
	while (m_ItemMap.end() != f)
	{
		if (f->second->GetType() == CItemData::ITEM_TYPE_METIN && f->second->GetWearFlags() == CItemData::WEARABLE_BODY)
			m_SocketVec.push_back(f->second->GetIndex());
		f++;
	}
	PyObject* tuple = PyTuple_New((Py_ssize_t)m_SocketVec.size());

	for (int i = 0; i < m_SocketVec.size(); i++)
	{
		PyTuple_SetItem(tuple, i, Py_BuildValue("i", m_SocketVec[i]));
	}
	return tuple;
}

PyObject* itemGetItemNames(PyObject* poSelf, PyObject* poArgs)
{
	CItemManager::TItemMap m_ItemMap = CItemManager::Instance().GetItems();
	CItemManager::TItemMap::iterator f = m_ItemMap.begin();
	PyObject* dict = PyTuple_New(m_ItemMap.size());
	int i = 0;
	while (m_ItemMap.end() != f)
	{
		PyObject* item = PyDict_New();
		PyDict_SetItemString(item, "vnum", Py_BuildValue("i", f->second->GetIndex()));
		PyDict_SetItemString(item, "name", Py_BuildValue("s", f->second->GetName()));
		PyTuple_SetItem(dict, i++, item);
		f++;
	}
	return dict;
}
#endif

#ifdef ENABLE_SEARCH_SHOP
static inline bool IsDragonSoulCoreVnum(DWORD dwVnum)
{
	return ((dwVnum >= 51511 && dwVnum <= 51638) || (dwVnum >= 50252 && dwVnum <= 50260));
}

PyObject * itemGetNameItem(PyObject * poSelf, PyObject * poArgs)
{
	char* szItemName;
	if (!PyTuple_GetString(poArgs, 0, &szItemName))
		return Py_BadArgument();
	std::map<DWORD, CItemData*> m_vec_ItemRange = CItemManager::Instance().GetItemMap();

	std::string searchedFileName(szItemName);
	PyObject* poList = PyList_New(0);

	std::map<DWORD, CItemData*>::iterator it;
	for (it = m_vec_ItemRange.begin(); it != m_vec_ItemRange.end(); ++it) 
	{
		CItemData * p = it->second;
		const CItemData::TItemTable * pTable = p->GetTable();
		if (pTable) 
		{
			std::string tempFileName(p->GetName());
			bool _IsBreakChecking = false;
			
			DWORD blockVnum[] = {50300, 70005, 72501, 72502,
				72013, 72014, 72015,
				71051, 71052, 71053, 39029, 39028, 76013,
				76014, 72004, 72006, 70043, 71016, 72001,
				72003, 72049, 72050, 39002, 71015
			};
			
			if (IsDragonSoulCoreVnum(pTable->dwVnum))
				continue;
								
			for (DWORD i = 0; i < _countof(blockVnum); i++) {
				if (pTable->dwVnum == blockVnum[i]) 
				{
					_IsBreakChecking = true;
					break;
				}
			}

			if (_IsBreakChecking)
				continue;

			std::transform(tempFileName.begin(), tempFileName.end(), tempFileName.begin(), ::tolower);
			std::transform(searchedFileName.begin(), searchedFileName.end(), searchedFileName.begin(), ::tolower);

			if (tempFileName.compare(0, searchedFileName.length(), searchedFileName) == 0) 
			{
				PyObject * poItemVnum = PyInt_FromLong(pTable->dwVnum);
				PyList_Append(poList, poItemVnum);
			}
		}
	}
	return poList;
}
#endif

#if defined(__BL_CHEST_DROP_INFO__)
#include "../EterBase/grid.h"

PyObject* itemHasDropInfo(PyObject* poSelf, PyObject* poArgs)
{
	int iItemIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemIndex))
		return Py_BadArgument();

	bool bMain;
	if (!PyTuple_GetBoolean(poArgs, 1, &bMain))
		return Py_BadArgument();

	CItemManager::TChestDropItemInfoVec* vDropInfo = nullptr;
	
	if (bMain)
		vDropInfo = CItemManager::Instance().GetItemDropInfoVec(iItemIndex);
	else
		vDropInfo = CItemManager::Instance().GetBaseItemDropInfoVec(iItemIndex);

	return Py_BuildValue("b", (vDropInfo && !vDropInfo->empty()));
}

static int __CreateDropPage(CItemManager::TChestDropItemInfoVec* vDropInfo, PyObject* poList)
{
	int iPageCount(0);
	CGrid grid(7, 6);
	
	if (vDropInfo && !vDropInfo->empty())
	{
		for (CItemManager::TChestDropItemInfoVec::const_iterator it = vDropInfo->begin(); it != vDropInfo->end(); ++it)
		{
			const CItemManager::SDropItemInfo& dwDropInfo = *it;

			CItemData* pItemData;
			if (!CItemManager::Instance().GetItemDataPointer(dwDropInfo.dwDropVnum, &pItemData))
				continue;

			const BYTE bItemSize = pItemData->GetSize();

			while (true)
			{
				const int iPos = grid.FindBlank(1, bItemSize);

				if (iPos >= 0)
				{
					grid.Put(iPos, 1, bItemSize);
					PyList_Append(poList, Py_BuildValue("iiii", iPageCount, iPos, dwDropInfo.dwDropVnum, dwDropInfo.iCount));
					break;
				}
				else
				{
					grid.Clear();
					++iPageCount;
				}
			}
		}
	}

	return iPageCount;
}

PyObject* itemGetDropInfo(PyObject* poSelf, PyObject* poArgs)
{
	int iItemIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemIndex))
		return Py_BadArgument();

	bool bMain;
	if (!PyTuple_GetBoolean(poArgs, 1, &bMain))
		return Py_BadArgument();

	PyObject* poList = PyList_New(0);
	CItemManager::TChestDropItemInfoVec* vDropInfo = nullptr;

	if (bMain)
		vDropInfo = CItemManager::Instance().GetItemDropInfoVec(iItemIndex);
	else
		vDropInfo = CItemManager::Instance().GetBaseItemDropInfoVec(iItemIndex);

	return Py_BuildValue("iO", __CreateDropPage(vDropInfo, poList), poList);
}
#endif

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
PyObject* itemGetItemByName(PyObject* poSelf, PyObject* poArgs)
{
	char* c_szText;
	if (!PyTuple_GetString(poArgs, 0, &c_szText))
		return Py_BuildValue("is", -1, "");

	CItemData* pItemData;
	if (!CItemManager::Instance().GetItemByName(c_szText, &pItemData))
		return Py_BuildValue("is", -1, "");

	return Py_BuildValue("is", pItemData->GetIndex(), pItemData->GetName());
}

PyObject* itemGetItemListByName(PyObject* poSelf, PyObject* poArgs)
{
	char* c_szText;
	if (!PyTuple_GetString(poArgs, 0, &c_szText))
		return Py_BuildNone();

	std::vector<std::pair<DWORD, std::string>> vec_itemName;
	CItemManager::Instance().GetItemListByName(c_szText, vec_itemName);

	PyObject* pyList = PyList_New(vec_itemName.size());

	for (size_t i = 0; i < vec_itemName.size(); ++i)
	{
		const auto& pair = vec_itemName.at(i);

		PyObject* pyTuple = PyTuple_New(2);

		PyTuple_SetItem(pyTuple, 0, Py_BuildValue("i", pair.first));
		PyTuple_SetItem(pyTuple, 1, Py_BuildValue("s", pair.second.c_str()));

		PyList_SetItem(pyList, i, pyTuple);
	}

	return pyList;
}
#endif

#if defined(ENABLE_OFFLINE_SHOP) || defined(ENABLE_INGAME_WIKI)
PyObject* itemGetItemDataByNamePart(PyObject* poSelf, PyObject* poArgs)
{
	char* nameStart;
	if (!PyTuple_GetString(poArgs, 0, &nameStart))
		return Py_BadArgument();

	std::tuple<const char*, int> itemData = CItemManager::Instance().SelectByNamePart(nameStart);
	return Py_BuildValue("(si)", std::get<0>(itemData), std::get<1>(itemData));
}

#endif
#ifdef ENABLE_WIKI
PyObject* itemGetRefineSet(PyObject* poSelf, PyObject* poArgs)
{
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");
	return Py_BuildValue("i", pItemData->GetRefinedSet());
}
PyObject* itemGetRefinedVnum(PyObject* poSelf, PyObject* poArgs)
{
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");
	return Py_BuildValue("i", pItemData->GetRefinedVnum());
}

PyObject* itemSelectItemWiki(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	if (!CItemManager::Instance().SelectItemData(iIndex))
		return Py_BuildValue("i", 0);
	return Py_BuildValue("i", 1);
}
#endif

void initItem()
{
	static PyMethodDef s_methods[] =
	{
		{ "GetItemsByName",			itemGetItemsByName,				METH_VARARGS },
		{ "SetUseSoundFileName",			itemSetUseSoundFileName,				METH_VARARGS },
		{ "SetDropSoundFileName",			itemSetDropSoundFileName,				METH_VARARGS },
		{ "SelectItem",						itemSelectItem,							METH_VARARGS },

		{ "GetItemName",					itemGetItemName,						METH_VARARGS },
		{ "GetItemVnum",					itemGetItemVnum,						METH_VARARGS },
		{ "GetItemDescription",				itemGetItemDescription,					METH_VARARGS },
		{ "GetItemSummary",					itemGetItemSummary,						METH_VARARGS },
		{ "GetIconImage",					itemGetIconImage,						METH_VARARGS },
		{ "GetIconImageFileName",			itemGetIconImageFileName,				METH_VARARGS },
		{ "GetItemSize",					itemGetItemSize,						METH_VARARGS },
		{ "GetItemType",					itemGetItemType,						METH_VARARGS },
		{ "GetItemSubType",					itemGetItemSubType,						METH_VARARGS },
		{ "GetIBuyItemPrice",				itemGetIBuyItemPrice,					METH_VARARGS },
		{ "GetISellItemPrice",				itemGetISellItemPrice,					METH_VARARGS },
		{ "IsAntiFlag",						itemIsAntiFlag,							METH_VARARGS },
		{ "IsFlag",							itemIsFlag,								METH_VARARGS },
		{ "IsWearableFlag",					itemIsWearableFlag,						METH_VARARGS },
		{ "Is1GoldItem",					itemIs1GoldItem,						METH_VARARGS },
		{ "GetLimit",						itemGetLimit,							METH_VARARGS },
		{ "GetAffect",						itemGetAffect,							METH_VARARGS },
		{ "GetValue",						itemGetValue,							METH_VARARGS },
		{ "GetSocket",						itemGetSocket,							METH_VARARGS },
		{ "GetIconInstance",				itemGetIconInstance,					METH_VARARGS },
		{ "GetUseType",						itemGetUseType,							METH_VARARGS },
		{ "DeleteIconInstance",				itemDeleteIconInstance,					METH_VARARGS },
		{ "IsEquipmentVID",					itemIsEquipmentVID,						METH_VARARGS },
		{ "IsRefineScroll",					itemIsRefineScroll,						METH_VARARGS },
		{ "IsDetachScroll",					itemIsDetachScroll,						METH_VARARGS },
		{ "IsKey",							itemIsKey,								METH_VARARGS },
		{ "IsMetin",						itemIsMetin,							METH_VARARGS },
		{ "CanAddToQuickSlotItem",			itemCanAddToQuickSlotItem,				METH_VARARGS },
#ifdef WJ_SPLIT_INVENTORY_SYSTEM
		{ "GetSpecialInvType",				itemGetSpecialInvType,					METH_VARARGS },
		{ "CanUseInSpecialInv",				itemCanUseInSpecialInv,					METH_VARARGS },
#endif
#ifdef ENABLE_SEARCH_SHOP
		{ "GetNameItem",					itemGetNameItem,					 METH_VARARGS },
#endif

		{ "Update",							itemUpdate,								METH_VARARGS },
		{ "Render",							itemRender,								METH_VARARGS },
		{ "CreateItem",						itemCreateItem,							METH_VARARGS },
		{ "DeleteItem",						itemDeleteItem,							METH_VARARGS },
		{ "Pick",							itemPick,								METH_VARARGS },

		{ "LoadItemTable",					itemLoadItemTable,						METH_VARARGS },
#ifdef ENABLE_ITEMSHOP
		{ "GetWeaponSockets",				itemGetWeaponSockets,					METH_VARARGS },
		{ "GetBodySockets",					itemGetBodySockets,						METH_VARARGS },
		{ "GetNames",						itemGetItemNames,						METH_VARARGS },
#endif
#ifdef ENABLE_SEALBIND_SYSTEM
		{ "IsSealScroll",					itemIsSealScroll,						METH_VARARGS },
		{ "GetDefaultSealDate",				itemGetDefaultSealDate,					METH_VARARGS },
		{ "GetUnlimitedSealDate",			itemGetUnlimitedSealDate,				METH_VARARGS },
#endif
#if defined(__BL_CHEST_DROP_INFO__)
		{ "HasDropInfo",					itemHasDropInfo,						METH_VARARGS },
		{ "GetDropInfo",					itemGetDropInfo,						METH_VARARGS },
#endif
		{ "GetApplyPoint",					itemGetApplyPoint,						METH_VARARGS },
#if defined(ENABLE_OFFLINE_SHOP) || defined(ENABLE_INGAME_WIKI)
		{ "GetItemDataByNamePart",			itemGetItemDataByNamePart,				METH_VARARGS},
#endif
		#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
		{ "GetItemByName",					itemGetItemByName,						METH_VARARGS },
		{ "GetItemListByName",				itemGetItemListByName,					METH_VARARGS },
#endif
#ifdef ENABLE_WIKI
		{ "SelectItemWiki", itemSelectItemWiki,											METH_VARARGS},
		{ "GetRefineSet",					itemGetRefineSet,							METH_VARARGS },
		{ "GetRefinedVnum",					itemGetRefinedVnum,							METH_VARARGS },
#endif
		{ NULL,								NULL,									NULL		 },
	};

	PyObject* poModule = Py_InitModule("item", s_methods);

	PyModule_AddIntConstant(poModule, "USESOUND_ACCESSORY", CPythonItem::USESOUND_ACCESSORY);
	PyModule_AddIntConstant(poModule, "USESOUND_ARMOR", CPythonItem::USESOUND_ARMOR);
	PyModule_AddIntConstant(poModule, "USESOUND_BOW", CPythonItem::USESOUND_BOW);
	PyModule_AddIntConstant(poModule, "USESOUND_DEFAULT", CPythonItem::USESOUND_DEFAULT);
	PyModule_AddIntConstant(poModule, "USESOUND_WEAPON", CPythonItem::USESOUND_WEAPON);
	PyModule_AddIntConstant(poModule, "USESOUND_POTION", CPythonItem::USESOUND_POTION);
	PyModule_AddIntConstant(poModule, "USESOUND_PORTAL", CPythonItem::USESOUND_PORTAL);

	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_SAFEBOX", CItemData::ITEM_ANTIFLAG_SAFEBOX);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_SAFEBOX", CItemData::ITEM_ANTIFLAG_SAFEBOX);

	PyModule_AddIntConstant(poModule, "DROPSOUND_ACCESSORY", CPythonItem::DROPSOUND_ACCESSORY);
	PyModule_AddIntConstant(poModule, "DROPSOUND_ARMOR", CPythonItem::DROPSOUND_ARMOR);
	PyModule_AddIntConstant(poModule, "DROPSOUND_BOW", CPythonItem::DROPSOUND_BOW);
	PyModule_AddIntConstant(poModule, "DROPSOUND_DEFAULT", CPythonItem::DROPSOUND_DEFAULT);
	PyModule_AddIntConstant(poModule, "DROPSOUND_WEAPON", CPythonItem::DROPSOUND_WEAPON);

	PyModule_AddIntConstant(poModule, "EQUIPMENT_COUNT", c_Equipment_Count);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_HEAD", c_Equipment_Head);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_BODY", c_Equipment_Body);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_WEAPON", c_Equipment_Weapon);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_WRIST", c_Equipment_Wrist);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_SHOES", c_Equipment_Shoes);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_NECK", c_Equipment_Neck);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_EAR", c_Equipment_Ear);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_UNIQUE1", c_Equipment_Unique1);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_UNIQUE2", c_Equipment_Unique2);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_ARROW", c_Equipment_Arrow);
#ifdef ENABLE_GLOVE_SYSTEM
	PyModule_AddIntConstant(poModule, "EQUIPMENT_GLOVE", c_Equipment_Glove);
#endif
	PyModule_AddIntConstant(poModule, "EQUIPMENT_SHIELD", c_Equipment_Shield);

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
	PyModule_AddIntConstant(poModule, "EQUIPMENT_RING1", c_Equipment_Ring1);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_RING2", c_Equipment_Ring2);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_BELT", c_Equipment_Belt);
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
	PyModule_AddIntConstant(poModule, "SLOT_ITEM_NEW_PET", c_SlotNewPet);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_NEW_PET_EQ0", c_SlotNewPetEqNeck);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_NEW_PET_EQ1", c_SlotNewPetEqBelt);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_NEW_PET_EQ2", c_SlotNewPetEqWrist);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_NEW_PET_EQ3", c_SlotNewPetEqHelm);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_NEW_PET_EQ4", c_SlotNewPetEqShield);
#endif
#ifdef ENABLE_SYSTEM_RUNE
	PyModule_AddIntConstant(poModule, "EQUIPMENT_RUNE", c_Equipment_Rune);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_RUNE_RED", c_Equipment_Rune_Red);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_RUNE_YELLOW", c_Equipment_Rune_Yellow);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_RUNE_GREEN", c_Equipment_Rune_Green);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_RUNE_BLUE", c_Equipment_Rune_Blue);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_RUNE_BLACK", c_Equipment_Rune_Black);
#endif
//#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
//	PyModule_AddIntConstant(poModule, "EQUIPMENT_MOUNT", c_Equipment_Mount);
//#endif
#ifdef WJ_SPLIT_INVENTORY_SYSTEM
	PyModule_AddIntConstant(poModule, "SKILL_BOOK_INVENTORY_SLOT_START", c_Skill_Book_Inventory_Slot_Start);
	PyModule_AddIntConstant(poModule, "UPGRADE_ITEMS_INVENTORY_SLOT_START", c_Upgrade_Items_Inventory_Slot_Start);
	PyModule_AddIntConstant(poModule, "STONE_INVENTORY_SLOT_START", c_Stone_Inventory_Slot_Start);
	PyModule_AddIntConstant(poModule, "BOX_INVENTORY_SLOT_START", c_Box_Inventory_Slot_Start);
	PyModule_AddIntConstant(poModule, "EFSUN_INVENTORY_SLOT_START", c_Efsun_Inventory_Slot_Start);
	PyModule_AddIntConstant(poModule, "CICEK_INVENTORY_SLOT_START", c_Cicek_Inventory_Slot_Start);

	PyModule_AddIntConstant(poModule, "SKILL_BOOK_INVENTORY_SLOT_END", c_Skill_Book_Inventory_Slot_End);
	PyModule_AddIntConstant(poModule, "UPGRADE_ITEMS_INVENTORY_SLOT_END", c_Upgrade_Items_Inventory_Slot_End);
	PyModule_AddIntConstant(poModule, "STONE_INVENTORY_SLOT_END", c_Stone_Inventory_Slot_End);
	PyModule_AddIntConstant(poModule, "BOX_INVENTORY_SLOT_END", c_Box_Inventory_Slot_End);
	PyModule_AddIntConstant(poModule, "EFSUN_INVENTORY_SLOT_END", c_Efsun_Inventory_Slot_End);
	PyModule_AddIntConstant(poModule, "CICEK_INVENTORY_SLOT_END", c_Cicek_Inventory_Slot_End);
#endif

#ifdef ENABLE_ARTEFAKT_SYSTEM
	PyModule_AddIntConstant(poModule, "EQUIPMENT_ARTEFAKT1", c_Artefakty_Slot1);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_ARTEFAKT2", c_Artefakty_Slot2);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_ARTEFAKT3", c_Artefakty_Slot3);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_ARTEFAKT4", c_Artefakty_Slot4);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_ARTEFAKT5", c_Artefakty_Slot5);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_ARTEFAKT6", c_Artefakty_Slot6);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_ARTEFAKT7", c_Artefakty_Slot7);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_ARTEFAKT8", c_Artefakty_Slot8);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_ARTEFAKT9", c_Artefakty_Slot9);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_ARTEFAKT10", c_Artefakty_Slot10);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_ARTEFAKT11", c_Artefakty_Slot11);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_ARTEFAKT12", c_Artefakty_Slot12);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_ARTEFAKT13", c_Artefakty_Slot13);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_ARTEFAKT14", c_Artefakty_Slot14);
	PyModule_AddIntConstant(poModule, "ARTEFAKT_SLOT_MAX", c_Artefakty_Slots);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_ARTEFAKT", CItemData::ITEM_TYPE_ARTEFAKT);
	PyModule_AddIntConstant(poModule, "ARTEFAKT_TYPE_1", CItemData::ARTEFAKT_1);
	PyModule_AddIntConstant(poModule, "ARTEFAKT_TYPE_2", CItemData::ARTEFAKT_2);
	PyModule_AddIntConstant(poModule, "ARTEFAKT_TYPE_3", CItemData::ARTEFAKT_3);
	PyModule_AddIntConstant(poModule, "ARTEFAKT_TYPE_4", CItemData::ARTEFAKT_4);
	PyModule_AddIntConstant(poModule, "ARTEFAKT_TYPE_5", CItemData::ARTEFAKT_5);
	PyModule_AddIntConstant(poModule, "ARTEFAKT_TYPE_6", CItemData::ARTEFAKT_6);
	PyModule_AddIntConstant(poModule, "ARTEFAKT_TYPE_7", CItemData::ARTEFAKT_7);
	PyModule_AddIntConstant(poModule, "ARTEFAKT_TYPE_8", CItemData::ARTEFAKT_8);
	PyModule_AddIntConstant(poModule, "ARTEFAKT_TYPE_9", CItemData::ARTEFAKT_9);
	PyModule_AddIntConstant(poModule, "ARTEFAKT_TYPE_10", CItemData::ARTEFAKT_10);
	PyModule_AddIntConstant(poModule, "ARTEFAKT_TYPE_11", CItemData::ARTEFAKT_11);
	PyModule_AddIntConstant(poModule, "ARTEFAKT_TYPE_12", CItemData::ARTEFAKT_12);
	PyModule_AddIntConstant(poModule, "ARTEFAKT_TYPE_13", CItemData::ARTEFAKT_13);
	PyModule_AddIntConstant(poModule, "ARTEFAKT_TYPE_14", CItemData::ARTEFAKT_14);
	PyModule_AddIntConstant(poModule, "MOUNT_SHOE", CItemData::MOUNT_SHOE);
	PyModule_AddIntConstant(poModule, "MOUNT_SADDLE", CItemData::MOUNT_SADDLE);
	PyModule_AddIntConstant(poModule, "MOUNT_ARMOR", CItemData::MOUNT_ARMOR);
#endif

	PyModule_AddIntConstant(poModule, "ITEM_TYPE_NONE", CItemData::ITEM_TYPE_NONE);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_WEAPON", CItemData::ITEM_TYPE_WEAPON);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_ARMOR", CItemData::ITEM_TYPE_ARMOR);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_USE", CItemData::ITEM_TYPE_USE);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_AUTOUSE", CItemData::ITEM_TYPE_AUTOUSE);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_MATERIAL", CItemData::ITEM_TYPE_MATERIAL);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_SPECIAL", CItemData::ITEM_TYPE_SPECIAL);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_TOOL", CItemData::ITEM_TYPE_TOOL);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_LOTTERY", CItemData::ITEM_TYPE_LOTTERY);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_ELK", CItemData::ITEM_TYPE_ELK);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_METIN", CItemData::ITEM_TYPE_METIN);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_CONTAINER", CItemData::ITEM_TYPE_CONTAINER);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_FISH", CItemData::ITEM_TYPE_FISH);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_ROD", CItemData::ITEM_TYPE_ROD);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_RESOURCE", CItemData::ITEM_TYPE_RESOURCE);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_CAMPFIRE", CItemData::ITEM_TYPE_CAMPFIRE);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_UNIQUE", CItemData::ITEM_TYPE_UNIQUE);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_SKILLBOOK", CItemData::ITEM_TYPE_SKILLBOOK);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_QUEST", CItemData::ITEM_TYPE_QUEST);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_POLYMORPH", CItemData::ITEM_TYPE_POLYMORPH);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_TREASURE_BOX", CItemData::ITEM_TYPE_TREASURE_BOX);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_TREASURE_KEY", CItemData::ITEM_TYPE_TREASURE_KEY);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_PICK", CItemData::ITEM_TYPE_PICK);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_BLEND", CItemData::ITEM_TYPE_BLEND);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_DS", CItemData::ITEM_TYPE_DS);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_SPECIAL_DS", CItemData::ITEM_TYPE_SPECIAL_DS);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_RING", CItemData::ITEM_TYPE_RING);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_BELT", CItemData::ITEM_TYPE_BELT);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_GIFTBOX", CItemData::ITEM_TYPE_GIFTBOX);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_GUILD_GUARD", CItemData::ITEM_TYPE_GUILD_GUARD);
#ifdef ENABLE_CHEQUE_SYSTEM
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_CHEQUE", CItemData::ITEM_TYPE_CHEQUE);
#endif
#ifdef ENABLE_SYSTEM_RUNE
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_RUNE", CItemData::ITEM_TYPE_RUNE);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_RUNE_RED", CItemData::ITEM_TYPE_RUNE_RED);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_RUNE_BLUE", CItemData::ITEM_TYPE_RUNE_BLUE);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_RUNE_GREEN", CItemData::ITEM_TYPE_RUNE_GREEN);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_RUNE_YELLOW", CItemData::ITEM_TYPE_RUNE_YELLOW);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_RUNE_BLACK", CItemData::ITEM_TYPE_RUNE_BLACK);
#endif
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_MOUNT_EQUIPMENT", CItemData::ITEM_TYPE_MOUNT_EQUIPMENT);
#endif
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_MOUNT", CItemData::ITEM_TYPE_MOUNT);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_GACHA", CItemData::ITEM_TYPE_GACHA);
#ifdef ENABLE_NEW_PET_SYSTEM
	PyModule_AddIntConstant(poModule, "ITEM_NEW_PET", CItemData::ITEM_NEW_PET);
	PyModule_AddIntConstant(poModule, "ITEM_NEW_PET_EQ", CItemData::ITEM_NEW_PET_EQ);
#endif
#ifdef TITLE_SYSTEM_BYLUZER
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_TITLE", CItemData::ITEM_TYPE_TITLE);
#endif
#ifdef ENABLE_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_COSTUME", CItemData::ITEM_TYPE_COSTUME);

	// Item Sub Type
	PyModule_AddIntConstant(poModule, "COSTUME_TYPE_BODY", CItemData::COSTUME_BODY);
	PyModule_AddIntConstant(poModule, "COSTUME_TYPE_HAIR", CItemData::COSTUME_HAIR);
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "COSTUME_TYPE_MOUNT", CItemData::COSTUME_MOUNT);
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "COSTUME_TYPE_ACCE", CItemData::COSTUME_ACCE);
#endif
#ifdef ENABLE_STOLE_COSTUME
	PyModule_AddIntConstant(poModule, "COSTUME_TYPE_STOLE", CItemData::COSTUME_STOLE);
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "COSTUME_TYPE_WEAPON", CItemData::COSTUME_WEAPON);
#endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	PyModule_AddIntConstant(poModule, "COSTUME_TYPE_BUFF_WEAPON", CItemData::BUFF_COSTUME_WEAPON);
	PyModule_AddIntConstant(poModule, "COSTUME_TYPE_BUFF_BODY", CItemData::BUFF_COSTUME_BODY);
	PyModule_AddIntConstant(poModule, "COSTUME_TYPE_BUFF_HAIR", CItemData::BUFF_COSTUME_HAIR);
#endif
#ifdef __ENABLE_TOGGLE_ITEMS__
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_TOGGLE", CItemData::ITEM_TOGGLE);
#endif

	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_START", c_Costume_Slot_Start);
	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_COUNT", c_Costume_Slot_Count);
	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_BODY", c_Costume_Slot_Body);
	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_HAIR", c_Costume_Slot_Hair);
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_MOUNT", c_Costume_Slot_Mount);
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_ACCE", c_Costume_Slot_Acce);
#endif
#ifdef ENABLE_STOLE_COSTUME
	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_STOLE", c_Costume_Slot_Stole);
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_WEAPON", c_Costume_Slot_Weapon);
#endif

	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_END", c_Costume_Slot_End);
#endif
#ifdef ENABLE_COSTUME_EMBLEMAT
	PyModule_AddIntConstant(poModule, "COSTUME_TYPE_EMBLEMAT", CItemData::COSTUME_EMBLEMAT);
	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_EMBLEMAT", c_Costume_Slot_Emblemat);
#endif
#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
	PyModule_AddIntConstant(poModule, "BELT_INVENTORY_SLOT_START", c_Belt_Inventory_Slot_Start);
	PyModule_AddIntConstant(poModule, "BELT_INVENTORY_SLOT_COUNT", c_Belt_Inventory_Slot_Count);
	PyModule_AddIntConstant(poModule, "BELT_INVENTORY_SLOT_END", c_Belt_Inventory_Slot_End);

#endif
#ifdef __PENDANT__
	PyModule_AddIntConstant(poModule, "PENDANT_FIRE_SLOT", c_EquipmentPendantFire);
	PyModule_AddIntConstant(poModule, "PENDANT_ICE_SLOT", c_EquipmentPendantIce);
	PyModule_AddIntConstant(poModule, "PENDANT_THUNDER_SLOT", c_EquipmentPendantThunder);
	PyModule_AddIntConstant(poModule, "PENDANT_WIND_SLOT", c_EquipmentPendantWind);
	PyModule_AddIntConstant(poModule, "PENDANT_DARK_SLOT", c_EquipmentPendantDark);
	PyModule_AddIntConstant(poModule, "PENDANT_EARTH_SLOT", c_EquipmentPendantEarth);
	PyModule_AddIntConstant(poModule, "PENDANT_FIRST_SOUL_SLOT", c_EquipmentPendantFirst);
	PyModule_AddIntConstant(poModule, "PENDANT_SECOND_SOUL_SLOT", c_EquipmentPendantSecond);

	PyModule_AddIntConstant(poModule, "ARMOR_PENDANT_FIRE", CItemData::ARMOR_PENDANT_FIRE);
	PyModule_AddIntConstant(poModule, "ARMOR_PENDANT_ICE", CItemData::ARMOR_PENDANT_ICE);
	PyModule_AddIntConstant(poModule, "ARMOR_PENDANT_THUNDER", CItemData::ARMOR_PENDANT_THUNDER);
	PyModule_AddIntConstant(poModule, "ARMOR_PENDANT_WIND", CItemData::ARMOR_PENDANT_WIND);
	PyModule_AddIntConstant(poModule, "ARMOR_PENDANT_EARTH", CItemData::ARMOR_PENDANT_EARTH);
	PyModule_AddIntConstant(poModule, "ARMOR_PENDANT_DARK", CItemData::ARMOR_PENDANT_DARK);
	PyModule_AddIntConstant(poModule, "ARMOR_PENDANT_SOUL", CItemData::ARMOR_PENDANT_SOUL);
#endif

	PyModule_AddIntConstant(poModule, "WEAPON_SWORD", CItemData::WEAPON_SWORD);
	PyModule_AddIntConstant(poModule, "WEAPON_DAGGER", CItemData::WEAPON_DAGGER);
	PyModule_AddIntConstant(poModule, "WEAPON_BOW", CItemData::WEAPON_BOW);
	PyModule_AddIntConstant(poModule, "WEAPON_TWO_HANDED", CItemData::WEAPON_TWO_HANDED);
	PyModule_AddIntConstant(poModule, "WEAPON_BELL", CItemData::WEAPON_BELL);
	PyModule_AddIntConstant(poModule, "WEAPON_FAN", CItemData::WEAPON_FAN);
	PyModule_AddIntConstant(poModule, "WEAPON_ARROW", CItemData::WEAPON_ARROW);
	PyModule_AddIntConstant(poModule, "WEAPON_MOUNT_SPEAR", CItemData::WEAPON_MOUNT_SPEAR);
#ifdef ENABLE_QUIVER_SYSTEM
	PyModule_AddIntConstant(poModule, "WEAPON_QUIVER", CItemData::WEAPON_QUIVER);
#endif
#ifdef ENABLE_WOLFMAN_CHARACTER
	PyModule_AddIntConstant(poModule, "WEAPON_CLAW", CItemData::WEAPON_CLAW);
#endif
	PyModule_AddIntConstant(poModule, "WEAPON_NUM_TYPES", CItemData::WEAPON_NUM_TYPES);

	PyModule_AddIntConstant(poModule, "USE_POTION", CItemData::USE_POTION);
	PyModule_AddIntConstant(poModule, "USE_TALISMAN", CItemData::USE_TALISMAN);
	PyModule_AddIntConstant(poModule, "USE_TUNING", CItemData::USE_TUNING);
	PyModule_AddIntConstant(poModule, "USE_MOVE", CItemData::USE_MOVE);
	PyModule_AddIntConstant(poModule, "USE_TREASURE_BOX", CItemData::USE_TREASURE_BOX);
	PyModule_AddIntConstant(poModule, "USE_MONEYBAG", CItemData::USE_MONEYBAG);
	PyModule_AddIntConstant(poModule, "USE_BAIT", CItemData::USE_BAIT);
	PyModule_AddIntConstant(poModule, "USE_ABILITY_UP", CItemData::USE_ABILITY_UP);
	PyModule_AddIntConstant(poModule, "USE_ADD_IS_BONUS", CItemData::USE_ADD_IS_BONUS);
	PyModule_AddIntConstant(poModule, "USE_AFFECT", CItemData::USE_AFFECT);
	PyModule_AddIntConstant(poModule, "USE_CREATE_STONE", CItemData::USE_CREATE_STONE);
	PyModule_AddIntConstant(poModule, "USE_SPECIAL", CItemData::USE_SPECIAL);
	PyModule_AddIntConstant(poModule, "USE_POTION_NODELAY", CItemData::USE_POTION_NODELAY);
	PyModule_AddIntConstant(poModule, "USE_CLEAR", CItemData::USE_CLEAR);
	PyModule_AddIntConstant(poModule, "USE_INVISIBILITY", CItemData::USE_INVISIBILITY);
	PyModule_AddIntConstant(poModule, "USE_DETACHMENT", CItemData::USE_DETACHMENT);
	PyModule_AddIntConstant(poModule, "USE_TIME_CHARGE_PER", CItemData::USE_TIME_CHARGE_PER);
	PyModule_AddIntConstant(poModule, "USE_TIME_CHARGE_FIX", CItemData::USE_TIME_CHARGE_FIX);

	PyModule_AddIntConstant(poModule, "MATERIAL_DS_REFINE_NORMAL", CItemData::MATERIAL_DS_REFINE_NORMAL);
	PyModule_AddIntConstant(poModule, "MATERIAL_DS_REFINE_BLESSED", CItemData::MATERIAL_DS_REFINE_BLESSED);
	PyModule_AddIntConstant(poModule, "MATERIAL_DS_REFINE_HOLLY", CItemData::MATERIAL_DS_REFINE_HOLLY);
#if defined(ENABLE_DS_CHANGE_ATTR)
	PyModule_AddIntConstant(poModule, "MATERIAL_DS_CHANGE_ATTR", CItemData::MATERIAL_DS_CHANGE_ATTR);
#endif

	PyModule_AddIntConstant(poModule, "METIN_NORMAL", CItemData::METIN_NORMAL);
	PyModule_AddIntConstant(poModule, "METIN_GOLD", CItemData::METIN_GOLD);

	PyModule_AddIntConstant(poModule, "LIMIT_NONE", CItemData::LIMIT_NONE);
	PyModule_AddIntConstant(poModule, "LIMIT_LEVEL", CItemData::LIMIT_LEVEL);
	PyModule_AddIntConstant(poModule, "LIMIT_STR", CItemData::LIMIT_STR);
	PyModule_AddIntConstant(poModule, "LIMIT_DEX", CItemData::LIMIT_DEX);
	PyModule_AddIntConstant(poModule, "LIMIT_INT", CItemData::LIMIT_INT);
	PyModule_AddIntConstant(poModule, "LIMIT_CON", CItemData::LIMIT_CON);
	PyModule_AddIntConstant(poModule, "LIMIT_PCBANG", CItemData::LIMIT_PCBANG);
	PyModule_AddIntConstant(poModule, "LIMIT_REAL_TIME", CItemData::LIMIT_REAL_TIME);
	PyModule_AddIntConstant(poModule, "LIMIT_REAL_TIME_START_FIRST_USE", CItemData::LIMIT_REAL_TIME_START_FIRST_USE);
	PyModule_AddIntConstant(poModule, "LIMIT_TIMER_BASED_ON_WEAR", CItemData::LIMIT_TIMER_BASED_ON_WEAR);
#ifdef ENABLE_MOUNT_SYSTEM
	PyModule_AddIntConstant(poModule, "LIMIT_MOUNT_LEVEL", CItemData::LIMIT_MOUNT_LEVEL);
#endif
	PyModule_AddIntConstant(poModule, "LIMIT_TYPE_MAX_NUM", CItemData::LIMIT_MAX_NUM);
	PyModule_AddIntConstant(poModule, "LIMIT_MAX_NUM", CItemData::ITEM_LIMIT_MAX_NUM);

	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_FEMALE", CItemData::ITEM_ANTIFLAG_FEMALE);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_MALE", CItemData::ITEM_ANTIFLAG_MALE);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_WARRIOR", CItemData::ITEM_ANTIFLAG_WARRIOR);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_ASSASSIN", CItemData::ITEM_ANTIFLAG_ASSASSIN);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_SURA", CItemData::ITEM_ANTIFLAG_SURA);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_SHAMAN", CItemData::ITEM_ANTIFLAG_SHAMAN);
#ifdef ENABLE_WOLFMAN_CHARACTER
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_WOLFMAN", CItemData::ITEM_ANTIFLAG_WOLFMAN);
#endif
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_GET", CItemData::ITEM_ANTIFLAG_GET);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_DROP", CItemData::ITEM_ANTIFLAG_DROP);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_SELL", CItemData::ITEM_ANTIFLAG_SELL);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_EMPIRE_A", CItemData::ITEM_ANTIFLAG_EMPIRE_A);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_EMPIRE_B", CItemData::ITEM_ANTIFLAG_EMPIRE_B);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_EMPIRE_R", CItemData::ITEM_ANTIFLAG_EMPIRE_R);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_SAVE", CItemData::ITEM_ANTIFLAG_SAVE);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_GIVE", CItemData::ITEM_ANTIFLAG_GIVE);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_PKDROP", CItemData::ITEM_ANTIFLAG_PKDROP);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_STACK", CItemData::ITEM_ANTIFLAG_STACK);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_MYSHOP", CItemData::ITEM_ANTIFLAG_MYSHOP);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_SAFEBOX", CItemData::ITEM_ANTIFLAG_SAFEBOX);

	PyModule_AddIntConstant(poModule, "ITEM_FLAG_REFINEABLE", CItemData::ITEM_FLAG_REFINEABLE);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_SAVE", CItemData::ITEM_FLAG_SAVE);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_STACKABLE", CItemData::ITEM_FLAG_STACKABLE);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_COUNT_PER_1GOLD", CItemData::ITEM_FLAG_COUNT_PER_1GOLD);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_SLOW_QUERY", CItemData::ITEM_FLAG_SLOW_QUERY);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_RARE", CItemData::ITEM_FLAG_RARE);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_UNIQUE", CItemData::ITEM_FLAG_UNIQUE);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_MAKECOUNT", CItemData::ITEM_FLAG_MAKECOUNT);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_IRREMOVABLE", CItemData::ITEM_FLAG_IRREMOVABLE);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_CONFIRM_WHEN_USE", CItemData::ITEM_FLAG_CONFIRM_WHEN_USE);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_STACKABLE", CItemData::ITEM_FLAG_STACKABLE);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_QUEST_USE", CItemData::ITEM_FLAG_QUEST_USE);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_QUEST_USE_MULTIPLE", CItemData::ITEM_FLAG_QUEST_USE_MULTIPLE);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_QUEST_GIVE", CItemData::ITEM_FLAG_QUEST_GIVE);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_LOG", CItemData::ITEM_FLAG_LOG);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_APPLICABLE", CItemData::ITEM_FLAG_APPLICABLE);

	PyModule_AddIntConstant(poModule, "ANTIFLAG_FEMALE", CItemData::ITEM_ANTIFLAG_FEMALE);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_MALE", CItemData::ITEM_ANTIFLAG_MALE);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_WARRIOR", CItemData::ITEM_ANTIFLAG_WARRIOR);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_ASSASSIN", CItemData::ITEM_ANTIFLAG_ASSASSIN);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_SURA", CItemData::ITEM_ANTIFLAG_SURA);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_SHAMAN", CItemData::ITEM_ANTIFLAG_SHAMAN);
#ifdef ENABLE_WOLFMAN_CHARACTER
	PyModule_AddIntConstant(poModule, "ANTIFLAG_WOLFMAN", CItemData::ITEM_ANTIFLAG_WOLFMAN);
#endif
	PyModule_AddIntConstant(poModule, "ANTIFLAG_GET", CItemData::ITEM_ANTIFLAG_GET);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_DROP", CItemData::ITEM_ANTIFLAG_DROP);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_SELL", CItemData::ITEM_ANTIFLAG_SELL);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_EMPIRE_A", CItemData::ITEM_ANTIFLAG_EMPIRE_A);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_EMPIRE_B", CItemData::ITEM_ANTIFLAG_EMPIRE_B);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_EMPIRE_R", CItemData::ITEM_ANTIFLAG_EMPIRE_R);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_SAVE", CItemData::ITEM_ANTIFLAG_SAVE);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_GIVE", CItemData::ITEM_ANTIFLAG_GIVE);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_PKDROP", CItemData::ITEM_ANTIFLAG_PKDROP);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_STACK", CItemData::ITEM_ANTIFLAG_STACK);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_MYSHOP", CItemData::ITEM_ANTIFLAG_MYSHOP);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_SAFEBOX", CItemData::ITEM_ANTIFLAG_SAFEBOX);

	PyModule_AddIntConstant(poModule, "WEARABLE_BODY", CItemData::WEARABLE_BODY);
	PyModule_AddIntConstant(poModule, "WEARABLE_HEAD", CItemData::WEARABLE_HEAD);
	PyModule_AddIntConstant(poModule, "WEARABLE_FOOTS", CItemData::WEARABLE_FOOTS);
	PyModule_AddIntConstant(poModule, "WEARABLE_WRIST", CItemData::WEARABLE_WRIST);
	PyModule_AddIntConstant(poModule, "WEARABLE_WEAPON", CItemData::WEARABLE_WEAPON);
	PyModule_AddIntConstant(poModule, "WEARABLE_NECK", CItemData::WEARABLE_NECK);
	PyModule_AddIntConstant(poModule, "WEARABLE_EAR", CItemData::WEARABLE_EAR);
	PyModule_AddIntConstant(poModule, "WEARABLE_UNIQUE", CItemData::WEARABLE_UNIQUE);
	PyModule_AddIntConstant(poModule, "WEARABLE_SHIELD", CItemData::WEARABLE_SHIELD);
	PyModule_AddIntConstant(poModule, "WEARABLE_ARROW", CItemData::WEARABLE_ARROW);

	PyModule_AddIntConstant(poModule, "ARMOR_BODY", CItemData::ARMOR_BODY);
	PyModule_AddIntConstant(poModule, "ARMOR_HEAD", CItemData::ARMOR_HEAD);
	PyModule_AddIntConstant(poModule, "ARMOR_SHIELD", CItemData::ARMOR_SHIELD);
	PyModule_AddIntConstant(poModule, "ARMOR_WRIST", CItemData::ARMOR_WRIST);
	PyModule_AddIntConstant(poModule, "ARMOR_FOOTS", CItemData::ARMOR_FOOTS);
	PyModule_AddIntConstant(poModule, "ARMOR_NECK", CItemData::ARMOR_NECK);
	PyModule_AddIntConstant(poModule, "ARMOR_EAR", CItemData::ARMOR_EAR);
	PyModule_AddIntConstant(poModule, "ARMOR_GLOVE", CItemData::ARMOR_GLOVE);

	PyModule_AddIntConstant(poModule, "ITEM_APPLY_MAX_NUM", CItemData::ITEM_APPLY_MAX_NUM);
	PyModule_AddIntConstant(poModule, "ITEM_SOCKET_MAX_NUM", CItemData::ITEM_SOCKET_MAX_NUM);

	PyModule_AddIntConstant(poModule, "APPLY_NONE", CItemData::APPLY_NONE);
	PyModule_AddIntConstant(poModule, "APPLY_STR", CItemData::APPLY_STR);
	PyModule_AddIntConstant(poModule, "APPLY_DEX", CItemData::APPLY_DEX);
	PyModule_AddIntConstant(poModule, "APPLY_CON", CItemData::APPLY_CON);
	PyModule_AddIntConstant(poModule, "APPLY_INT", CItemData::APPLY_INT);
	PyModule_AddIntConstant(poModule, "APPLY_MAX_HP", CItemData::APPLY_MAX_HP);
	PyModule_AddIntConstant(poModule, "APPLY_MAX_SP", CItemData::APPLY_MAX_SP);
	PyModule_AddIntConstant(poModule, "APPLY_HP_REGEN", CItemData::APPLY_HP_REGEN);
	PyModule_AddIntConstant(poModule, "APPLY_SP_REGEN", CItemData::APPLY_SP_REGEN);
	PyModule_AddIntConstant(poModule, "APPLY_DEF_GRADE_BONUS", CItemData::APPLY_DEF_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_ATT_GRADE_BONUS", CItemData::APPLY_ATT_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_ATT_SPEED", CItemData::APPLY_ATT_SPEED);
	PyModule_AddIntConstant(poModule, "APPLY_MOV_SPEED", CItemData::APPLY_MOV_SPEED);
	PyModule_AddIntConstant(poModule, "APPLY_CAST_SPEED", CItemData::APPLY_CAST_SPEED);
	PyModule_AddIntConstant(poModule, "APPLY_MAGIC_ATT_GRADE", CItemData::APPLY_MAGIC_ATT_GRADE);
	PyModule_AddIntConstant(poModule, "APPLY_MAGIC_DEF_GRADE", CItemData::APPLY_MAGIC_DEF_GRADE);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL", CItemData::APPLY_SKILL);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_ANIMAL", CItemData::APPLY_ATTBONUS_ANIMAL);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_UNDEAD", CItemData::APPLY_ATTBONUS_UNDEAD);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_DEVIL", CItemData::APPLY_ATTBONUS_DEVIL);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_HUMAN", CItemData::APPLY_ATTBONUS_HUMAN);
	PyModule_AddIntConstant(poModule, "APPLY_BOW_DISTANCE", CItemData::APPLY_BOW_DISTANCE);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_BOW", CItemData::APPLY_RESIST_BOW);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_FIRE", CItemData::APPLY_RESIST_FIRE);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_ELEC", CItemData::APPLY_RESIST_ELEC);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_MAGIC", CItemData::APPLY_RESIST_MAGIC);
	PyModule_AddIntConstant(poModule, "APPLY_POISON_PCT", CItemData::APPLY_POISON_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_SLOW_PCT", CItemData::APPLY_SLOW_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_STUN_PCT", CItemData::APPLY_STUN_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_CRITICAL_PCT", CItemData::APPLY_CRITICAL_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_PENETRATE_PCT", CItemData::APPLY_PENETRATE_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_ORC", CItemData::APPLY_ATTBONUS_ORC);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_MILGYO", CItemData::APPLY_ATTBONUS_MILGYO);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_UNDEAD", CItemData::APPLY_ATTBONUS_UNDEAD);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_DEVIL", CItemData::APPLY_ATTBONUS_DEVIL);
	PyModule_AddIntConstant(poModule, "APPLY_STEAL_HP", CItemData::APPLY_STEAL_HP);
	PyModule_AddIntConstant(poModule, "APPLY_STEAL_SP", CItemData::APPLY_STEAL_SP);
	PyModule_AddIntConstant(poModule, "APPLY_MANA_BURN_PCT", CItemData::APPLY_MANA_BURN_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_DAMAGE_SP_RECOVER", CItemData::APPLY_DAMAGE_SP_RECOVER);
	PyModule_AddIntConstant(poModule, "APPLY_BLOCK", CItemData::APPLY_BLOCK);
	PyModule_AddIntConstant(poModule, "APPLY_DODGE", CItemData::APPLY_DODGE);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_SWORD", CItemData::APPLY_RESIST_SWORD);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_TWOHAND", CItemData::APPLY_RESIST_TWOHAND);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_DAGGER", CItemData::APPLY_RESIST_DAGGER);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_BELL", CItemData::APPLY_RESIST_BELL);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_FAN", CItemData::APPLY_RESIST_FAN);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_WIND", CItemData::APPLY_RESIST_WIND);
	PyModule_AddIntConstant(poModule, "APPLY_REFLECT_MELEE", CItemData::APPLY_REFLECT_MELEE);
	PyModule_AddIntConstant(poModule, "APPLY_REFLECT_CURSE", CItemData::APPLY_REFLECT_CURSE);
	PyModule_AddIntConstant(poModule, "APPLY_POISON_REDUCE", CItemData::APPLY_POISON_REDUCE);
	PyModule_AddIntConstant(poModule, "APPLY_KILL_SP_RECOVER", CItemData::APPLY_KILL_SP_RECOVER);
	PyModule_AddIntConstant(poModule, "APPLY_EXP_DOUBLE_BONUS", CItemData::APPLY_EXP_DOUBLE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_GOLD_DOUBLE_BONUS", CItemData::APPLY_GOLD_DOUBLE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_ITEM_DROP_BONUS", CItemData::APPLY_ITEM_DROP_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_POTION_BONUS", CItemData::APPLY_POTION_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_KILL_HP_RECOVER", CItemData::APPLY_KILL_HP_RECOVER);
	PyModule_AddIntConstant(poModule, "APPLY_IMMUNE_STUN", CItemData::APPLY_IMMUNE_STUN);
	PyModule_AddIntConstant(poModule, "APPLY_IMMUNE_SLOW", CItemData::APPLY_IMMUNE_SLOW);
	PyModule_AddIntConstant(poModule, "APPLY_IMMUNE_FALL", CItemData::APPLY_IMMUNE_FALL);
	PyModule_AddIntConstant(poModule, "APPLY_MAX_STAMINA", CItemData::APPLY_MAX_STAMINA);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_WARRIOR", CItemData::APPLY_ATT_BONUS_TO_WARRIOR);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_ASSASSIN", CItemData::APPLY_ATT_BONUS_TO_ASSASSIN);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_SURA", CItemData::APPLY_ATT_BONUS_TO_SURA);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_SHAMAN", CItemData::APPLY_ATT_BONUS_TO_SHAMAN);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_MONSTER", CItemData::APPLY_ATT_BONUS_TO_MONSTER);
	PyModule_AddIntConstant(poModule, "APPLY_MALL_ATTBONUS", CItemData::APPLY_MALL_ATTBONUS);
	PyModule_AddIntConstant(poModule, "APPLY_MALL_DEFBONUS", CItemData::APPLY_MALL_DEFBONUS);
	PyModule_AddIntConstant(poModule, "APPLY_MALL_EXPBONUS", CItemData::APPLY_MALL_EXPBONUS);
	PyModule_AddIntConstant(poModule, "APPLY_MALL_ITEMBONUS", CItemData::APPLY_MALL_ITEMBONUS);
	PyModule_AddIntConstant(poModule, "APPLY_MALL_GOLDBONUS", CItemData::APPLY_MALL_GOLDBONUS);
	PyModule_AddIntConstant(poModule, "APPLY_MAX_HP_PCT", CItemData::APPLY_MAX_HP_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_MAX_SP_PCT", CItemData::APPLY_MAX_SP_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_BONUS", CItemData::APPLY_SKILL_DAMAGE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_NORMAL_HIT_DAMAGE_BONUS", CItemData::APPLY_NORMAL_HIT_DAMAGE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DEFEND_BONUS", CItemData::APPLY_SKILL_DEFEND_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_NORMAL_HIT_DEFEND_BONUS", CItemData::APPLY_NORMAL_HIT_DEFEND_BONUS);

	PyModule_AddIntConstant(poModule, "APPLY_PC_BANG_EXP_BONUS", CItemData::APPLY_PC_BANG_EXP_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_PC_BANG_DROP_BONUS", CItemData::APPLY_PC_BANG_DROP_BONUS);
#ifdef ENABLE_COLLECTIONS_SYSTEM
	PyModule_AddIntConstant(poModule, "USE_COLLECTION_SCROLL", CItemData::USE_COLLECTION_SCROLL);
#endif
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_WARRIOR", CItemData::APPLY_RESIST_WARRIOR);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_ASSASSIN", CItemData::APPLY_RESIST_ASSASSIN);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_SURA", CItemData::APPLY_RESIST_SURA);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_SHAMAN", CItemData::APPLY_RESIST_SHAMAN);
	PyModule_AddIntConstant(poModule, "APPLY_ENERGY", CItemData::APPLY_ENERGY);
	PyModule_AddIntConstant(poModule, "APPLY_COSTUME_ATTR_BONUS", CItemData::APPLY_COSTUME_ATTR_BONUS);

	PyModule_AddIntConstant(poModule, "APPLY_MAGIC_ATTBONUS_PER", CItemData::APPLY_MAGIC_ATTBONUS_PER);
	PyModule_AddIntConstant(poModule, "APPLY_MELEE_MAGIC_ATTBONUS_PER", CItemData::APPLY_MELEE_MAGIC_ATTBONUS_PER);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_ICE", CItemData::APPLY_RESIST_ICE);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_EARTH", CItemData::APPLY_RESIST_EARTH);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_DARK", CItemData::APPLY_RESIST_DARK);
	PyModule_AddIntConstant(poModule, "APPLY_ANTI_CRITICAL_PCT", CItemData::APPLY_ANTI_CRITICAL_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_ANTI_PENETRATE_PCT", CItemData::APPLY_ANTI_PENETRATE_PCT);
#ifdef ENABLE_WOLFMAN_CHARACTER
	PyModule_AddIntConstant(poModule, "APPLY_BLEEDING_PCT", CItemData::APPLY_BLEEDING_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_BLEEDING_REDUCE", CItemData::APPLY_BLEEDING_REDUCE);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_WOLFMAN", CItemData::APPLY_ATT_BONUS_TO_WOLFMAN);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_WOLFMAN", CItemData::APPLY_RESIST_WOLFMAN);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_CLAW", CItemData::APPLY_RESIST_CLAW);
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "APPLY_ACCEDRAIN_RATE", CItemData::APPLY_ACCEDRAIN_RATE);
#endif

#ifdef ENABLE_AURA_SYSTEM
	PyModule_AddIntConstant(poModule, "COSTUME_TYPE_AURA", CItemData::COSTUME_AURA);
	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_AURA", c_Costume_Slot_Aura);
#endif

//#ifdef ENABLE_GLOVE_SYSTEM
//	PyModule_AddIntConstant(poModule, "APPLY_RANDOM", CItemData::APPLY_RANDOM);
//#endif

#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_MAGIC_REDUCTION", CItemData::APPLY_RESIST_MAGIC_REDUCTION);
#endif

	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_ELECT", CItemData::APPLY_ENCHANT_ELECT);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_FIRE", CItemData::APPLY_ENCHANT_FIRE);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_ICE", CItemData::APPLY_ENCHANT_ICE);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_WIND", CItemData::APPLY_ENCHANT_WIND);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_EARTH", CItemData::APPLY_ENCHANT_EARTH);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_DARK", CItemData::APPLY_ENCHANT_DARK);

	PyModule_AddIntConstant(poModule, "APPLY_RESIST_HUMAN", CItemData::APPLY_RESIST_HUMAN);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_BOSS", CItemData::APPLY_ATTBONUS_BOSS);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_WLADCA", CItemData::APPLY_ATTBONUS_WLADCA);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_STONE", CItemData::APPLY_ATTBONUS_STONE);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_BOSS", CItemData::APPLY_RESIST_BOSS);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_WLADCA", CItemData::APPLY_RESIST_WLADCA);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_MONSTER", CItemData::APPLY_RESIST_MONSTER);
	PyModule_AddIntConstant(poModule, "APPLY_STAT_BONUS", CItemData::APPLY_STAT_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_DUNGEON", CItemData::APPLY_ATTBONUS_DUNGEON);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_LEGENDA", CItemData::APPLY_ATTBONUS_LEGENDA);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_KLASY", CItemData::APPLY_ATTBONUS_KLASY);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_KLASY", CItemData::APPLY_RESIST_KLASY);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_LEGENDA", CItemData::APPLY_ATTBONUS_LEGENDA);
	PyModule_AddIntConstant(poModule, "APPLY_DMG_BONUS", CItemData::APPLY_DMG_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_FINAL_DMG_BONUS", CItemData::APPLY_FINAL_DMG_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_MINING_SUCCESS_CHANCE", CItemData::APPLY_MINING_SUCCESS_CHANCE);
	PyModule_AddIntConstant(poModule, "APPLY_MINING_MORE_ORES_CHANCE", CItemData::APPLY_MINING_MORE_ORES_CHANCE);
	PyModule_AddIntConstant(poModule, "APPLY_MINING_LESS_MINE_TIME", CItemData::APPLY_MINING_LESS_MINE_TIME);
	PyModule_AddIntConstant(poModule, "APPLY_MINING_MORE_POINTS", CItemData::APPLY_MINING_MORE_POINTS);
	PyModule_AddIntConstant(poModule, "APPLY_FISHING_INSTANT_CATCH_CHANCE", CItemData::APPLY_FISHING_INSTANT_CATCH_CHANCE);
	PyModule_AddIntConstant(poModule, "APPLY_FISHING_BIGGER_FISH_CHANCE", CItemData::APPLY_FISHING_BIGGER_FISH_CHANCE);
	PyModule_AddIntConstant(poModule, "APPLY_FISHING_NEXT_SECTION_CHANCE", CItemData::APPLY_FISHING_NEXT_SECTION_CHANCE);
	PyModule_AddIntConstant(poModule, "APPLY_FISHING_MORE_YANG_MULTIPLIER", CItemData::APPLY_FISHING_MORE_YANG_MULTIPLIER);
	PyModule_AddIntConstant(poModule, "APPLY_FISHING_MORE_POINTS_CHANCE", CItemData::APPLY_FISHING_MORE_POINTS_CHANCE);
	PyModule_AddIntConstant(poModule, "APPLY_FISHING_SUCCESS_CHANCE", CItemData::APPLY_FISHING_SUCCESS_CHANCE);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_PVM_SKILL", CItemData::APPLY_ATTBONUS_PVM_SKILL);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_ELEMENT", CItemData::APPLY_ATTBONUS_ELEMENT);
	PyModule_AddIntConstant(poModule, "APPLY_DEFBONUS_ELEMENT", CItemData::APPLY_DEFBONUS_ELEMENT);
	PyModule_AddIntConstant(poModule, "APPLY_DOUBLE_LOOT_SUMMER", CItemData::APPLY_DOUBLE_LOOT_SUMMER);
	PyModule_AddIntConstant(poModule, "APPLY_DOUBLE_LOOT_DROP", CItemData::APPLY_DOUBLE_LOOT_DROP);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_PVM_STR", CItemData::APPLY_ATTBONUS_PVM_STR);
#ifdef ENABLE_SEALBIND_SYSTEM
	PyModule_AddIntConstant(poModule, "E_SEAL_DATE_DEFAULT_TIMESTAMP", CItemData::SEAL_DATE_DEFAULT_TIMESTAMP);
	PyModule_AddIntConstant(poModule, "E_SEAL_DATE_UNLIMITED_TIMESTAMP", CItemData::SEAL_DATE_UNLIMITED_TIMESTAMP);
#endif

#ifdef ENABLE_PET_SYSTEM_EX
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_PET", CItemData::ITEM_TYPE_PET);
	PyModule_AddIntConstant(poModule, "PET_EGG", CItemData::PET_EGG);
	PyModule_AddIntConstant(poModule, "PET_UPBRINGING", CItemData::PET_UPBRINGING);
	PyModule_AddIntConstant(poModule, "PET_BAG", CItemData::PET_BAG);
	PyModule_AddIntConstant(poModule, "PET_FEEDSTUFF", CItemData::PET_FEEDSTUFF);
	PyModule_AddIntConstant(poModule, "PET_SKILL", CItemData::PET_SKILL);
	PyModule_AddIntConstant(poModule, "PET_SKILL_DEL_BOOK", CItemData::PET_SKILL_DEL_BOOK);
	PyModule_AddIntConstant(poModule, "PET_NAME_CHANGE", CItemData::PET_NAME_CHANGE);
	PyModule_AddIntConstant(poModule, "PET_EXPFOOD", CItemData::PET_EXPFOOD);
	PyModule_AddIntConstant(poModule, "PET_SKILL_ALL_DEL_BOOK", CItemData::PET_SKILL_ALL_DEL_BOOK);
	PyModule_AddIntConstant(poModule, "PET_EXPFOOD_PER", CItemData::PET_EXPFOOD_PER);
	PyModule_AddIntConstant(poModule, "PET_ATTR_DETERMINE", CItemData::PET_ATTR_DETERMINE);
	PyModule_AddIntConstant(poModule, "PET_ATTR_CHANGE", CItemData::PET_ATTR_CHANGE);
	PyModule_AddIntConstant(poModule, "PET_PAY", CItemData::PET_PAY);
	PyModule_AddIntConstant(poModule, "PET_PRIMIUM_FEEDSTUFF", CItemData::PET_PRIMIUM_FEEDSTUFF);
#endif

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
	PyModule_AddIntConstant(poModule, "DS_SLOT1", CItemData::DS_SLOT1);
	PyModule_AddIntConstant(poModule, "DS_SLOT2", CItemData::DS_SLOT2);
	PyModule_AddIntConstant(poModule, "DS_SLOT3", CItemData::DS_SLOT3);
	PyModule_AddIntConstant(poModule, "DS_SLOT4", CItemData::DS_SLOT4);
	PyModule_AddIntConstant(poModule, "DS_SLOT5", CItemData::DS_SLOT5);
	PyModule_AddIntConstant(poModule, "DS_SLOT6", CItemData::DS_SLOT6);

	PyModule_AddIntConstant(poModule, "DS_SLOT_NUM_TYPES", CItemData::DS_SLOT_NUM_TYPES);

	PyModule_AddIntConstant(poModule, "ITEM_NAME_MAX_LEN", CItemData::ITEM_NAME_MAX_LEN);
#endif
#ifdef __ENABLE_POLYMORPH_SYSTEM__
	PyModule_AddIntConstant(poModule, "USE_POLY_SKIN", CItemData::USE_POLY_SKIN);
#endif
#ifdef __ENABLE_TOGGLE_ITEMS__
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_TOGGLE", CItemData::ITEM_TOGGLE);
	PyModule_AddIntConstant(poModule, "TOGGLE_AUTO_RECOVERY_HP", CItemData::TOGGLE_AUTO_RECOVERY_HP);
	PyModule_AddIntConstant(poModule, "TOGGLE_AUTO_RECOVERY_SP", CItemData::TOGGLE_AUTO_RECOVERY_SP);
	PyModule_AddIntConstant(poModule, "TOGGLE_AFFECT", CItemData::TOGGLE_AFFECT);
#ifdef __ENABLE_AUTO_PICKUP__
	PyModule_AddIntConstant(poModule, "TOGGLE_PICKUP", CItemData::TOGGLE_PICKUP);
#endif
#endif
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
	PyModule_AddIntConstant(poModule, "LIMIT_ENLIGHT_LEVEL", CItemData::LIMIT_ENLIGHT_LEVEL);
#endif
#ifdef ENABLE_MOUNT_SYSTEM
	PyModule_AddIntConstant(poModule, "MOUNT_SLOT_START", c_Mount_Slot_Start);
	PyModule_AddIntConstant(poModule, "MOUNT_SLOT_COUNT", c_Mount_Slot_Count);
	PyModule_AddIntConstant(poModule, "MOUNT_SLOT_END", c_Mount_Slot_End);
#endif
#ifdef ENABLE_SKILL_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "SKILL_SLOT_START", c_Skill_Costume_Slot_Start);
	PyModule_AddIntConstant(poModule, "SKILL_SLOT_COUNT", c_Skill_Costume_Slot_Count);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_SKILL_COSTUME", CItemData::ITEM_TYPE_SKILL_COSTUME);
	PyModule_AddIntConstant(poModule, "SKILL_TYPE_1", CItemData::SKILL_COSTUME_1);
	PyModule_AddIntConstant(poModule, "SKILL_TYPE_2", CItemData::SKILL_COSTUME_2);
	PyModule_AddIntConstant(poModule, "SKILL_TYPE_3", CItemData::SKILL_COSTUME_3);
	PyModule_AddIntConstant(poModule, "SKILL_TYPE_4", CItemData::SKILL_COSTUME_4);
	PyModule_AddIntConstant(poModule, "SKILL_TYPE_5", CItemData::SKILL_COSTUME_5);
	PyModule_AddIntConstant(poModule, "SKILL_TYPE_6", CItemData::SKILL_COSTUME_6);
	PyModule_AddIntConstant(poModule, "SKILL_SLOT_1", c_Skill_Slot_1);
	PyModule_AddIntConstant(poModule, "SKILL_SLOT_2", c_Skill_Slot_2);
	PyModule_AddIntConstant(poModule, "SKILL_SLOT_3", c_Skill_Slot_3);
	PyModule_AddIntConstant(poModule, "SKILL_SLOT_4", c_Skill_Slot_4);
	PyModule_AddIntConstant(poModule, "SKILL_SLOT_5", c_Skill_Slot_5);
	PyModule_AddIntConstant(poModule, "SKILL_SLOT_6", c_Skill_Slot_6);
	
#endif
#ifdef ENABLE_VIP_SYSTEM
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_VIP", CItemData::ITEM_TYPE_VIP);
#endif
}
//martysama0134's ceqyqttoaf71vasf9t71218
