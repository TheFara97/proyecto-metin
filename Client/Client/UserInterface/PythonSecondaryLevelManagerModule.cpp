#include "StdAfx.h"

#ifdef ENABLE_SECONDARY_LEVEL
#include "PythonSecondaryLevelManager.h"
#include "PythonNetworkStream.h"

PyObject* secondaryLevelGetApply(PyObject* poSelf, PyObject* poArgs)
{
	int idx = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &idx))
		return Py_BuildException();

	auto iType = CSecondaryLevelManager::Instance().GetApplyType(idx);
	auto iValue = CSecondaryLevelManager::Instance().GetApplyValue(idx);

	return Py_BuildValue("ii", iType, iValue);
}

PyObject* secondaryLevelGetNextApply(PyObject* poSelf, PyObject* poArgs)
{
	int idx = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &idx))
		return Py_BuildException();

	auto iType = CSecondaryLevelManager::Instance().GetNextApplyType(idx);
	auto iValue = CSecondaryLevelManager::Instance().GetNextApplyValue(idx);

	return Py_BuildValue("ii", iType, iValue);
}

PyObject* secondaryLevelGetRequiredItem(PyObject* poSelf, PyObject* poArgs)
{
	int idx = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &idx))
		return Py_BuildException();

	auto iVnum = CSecondaryLevelManager::Instance().GetRequiredItemVnum(idx);
	auto iCount = CSecondaryLevelManager::Instance().GetRequiredItemCount(idx);

	return Py_BuildValue("ii", iVnum, iCount);
}

PyObject* secondaryLevelGetRequiredGold(PyObject* poSelf, PyObject* poArgs)
{
	int option = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &option))
		return Py_BuildException();

	int iCost = CSecondaryLevelManager::Instance().GetRequiredGold(option);

	return Py_BuildValue("i", iCost);
}

PyObject* secondaryLevelGetChance(PyObject* poSelf, PyObject* poArgs)
{
	int option = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &option))
		return Py_BuildException();

	int iCost = CSecondaryLevelManager::Instance().GetChance(option);

	return Py_BuildValue("i", iCost);
}

PyObject* secondaryLevelSendPacket(PyObject* poSelf, PyObject* poArgs)
{
	int bSubHeader = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &bSubHeader))
		return Py_BuildException();

	int byOption = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &byOption))
		return Py_BuildException();

	CPythonNetworkStream::Instance().SendSecondaryLevelPacket(static_cast<ESecondaryLevelSubheader>(bSubHeader), byOption);
	return Py_BuildNone();
}

void initSecondaryLevelManager()
{
	static PyMethodDef s_methods[] =
	{
		{ "SendPacket",			secondaryLevelSendPacket,			METH_VARARGS	},
		{ "GetApply",			secondaryLevelGetApply,				METH_VARARGS	},
		{ "GetNextApply",		secondaryLevelGetNextApply,			METH_VARARGS	},
		{ "GetRequiredItem",	secondaryLevelGetRequiredItem,		METH_VARARGS	},
		{ "GetRequiredGold",	secondaryLevelGetRequiredGold,		METH_VARARGS	},
		{ "GetChance",			secondaryLevelGetChance,			METH_VARARGS	},
		{ NULL, NULL, NULL },
	};

	PyObject* poModule = Py_InitModule("sndLevelMgr", s_methods);
}
#endif