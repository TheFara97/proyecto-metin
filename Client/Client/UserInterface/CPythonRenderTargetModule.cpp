#include "StdAfx.h"
#include "PythonApplication.h"


PyObject* renderTargetSelectModel(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	int modelIndex = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &modelIndex))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->SelectModel(modelIndex);

	return Py_BuildNone();
}
PyObject* renderTargetSetArmor(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	int modelIndex = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &modelIndex))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->SetArmor(modelIndex);

	return Py_BuildNone();

}

PyObject* renderTargetSetWeapon(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	int modelIndex = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &modelIndex))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->SetWeapon(modelIndex);

	return Py_BuildNone();

}

PyObject* renderTargetSetHair(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	int modelIndex = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &modelIndex))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->ChangeHair(modelIndex);

	return Py_BuildNone();

}

PyObject* renderTargetSetSkillCostume(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	int vnum = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &vnum))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->SetSkillCostume(static_cast<DWORD>(vnum));

	return Py_BuildNone();
}

PyObject* renderTargetChangeEffect(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->ChangeEffect();

	return Py_BuildNone();
}	

PyObject* renderTargetSetAcce(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	int modelIndex = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &modelIndex))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->SetAcce(modelIndex);

	return Py_BuildNone();
}

PyObject* renderTargetSetVisibility(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	bool isShow;
	if (!PyTuple_GetBoolean(poArgs, 1, &isShow))
		return Py_BadArgument();
	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
		target->SetVisibility(isShow);
	return Py_BuildNone();
}

PyObject* renderTargetSetBackground(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	char * szPathName;
	if (!PyTuple_GetString(poArgs, 1, &szPathName))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->CreateBackground(
		szPathName, CPythonApplication::Instance().GetWidth(),
		CPythonApplication::Instance().GetHeight());
	return Py_BuildNone();
}

PyObject* renderTargetResetModel(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if(target)
		target->ResetModel();
	return Py_BuildNone();
}

PyObject* renderTargetRemoveRenderTarget(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	CRenderTargetManager::Instance().RemoveRenderTarget(iIndex);
	return Py_BuildNone();
}

PyObject* renderTargetSetZoom(PyObject* poSelf, PyObject* poArgs)
{
	uint8_t byRenderIndex = 0;
	if (!PyTuple_GetByte(poArgs, 0, &byRenderIndex))
		return Py_BadArgument();

	bool bZoom = true;
	if (!PyTuple_GetBoolean(poArgs, 1, &bZoom))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(byRenderIndex)->SetZoom(bZoom);
	return Py_BuildNone();
}

PyObject* renderTargetSetCustomZoom(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	float zoom = 0.0f;
	if (!PyTuple_GetFloat(poArgs, 1, &zoom))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->SetCustomZoom(zoom);

	return Py_BuildNone();
}



PyObject* renderGetFreeIndex(PyObject* poSelf, PyObject* poArgs)
{
	int iMin;
	if (!PyTuple_GetInteger(poArgs, 0, &iMin))
		return Py_BadArgument();
	int iMax;
	if (!PyTuple_GetInteger(poArgs, 1, &iMax))
		return Py_BadArgument();
	for (int j = iMin; j < iMax; ++j)
	{
		if (!CRenderTargetManager::Instance().GetRenderTarget(j))
			return Py_BuildValue("i", j);
	}
	return Py_BuildValue("i", iMin + 20);
}

void initRenderTarget() {
	static PyMethodDef s_methods[] =
	{
		{ "SelectModel", renderTargetSelectModel, METH_VARARGS },
		{ "SetVisibility", renderTargetSetVisibility, METH_VARARGS },
		{ "SetBackground", renderTargetSetBackground, METH_VARARGS },
		{ "SetArmor", renderTargetSetArmor, METH_VARARGS },
		{ "SetWeapon", renderTargetSetWeapon, METH_VARARGS },
		{ "SetHair", renderTargetSetHair, METH_VARARGS },
		{ "ChangeEffect", renderTargetChangeEffect, METH_VARARGS },
		{ "SetSkillCostume", renderTargetSetSkillCostume, METH_VARARGS },
		{ "SetAcce", renderTargetSetAcce, METH_VARARGS },
		{ "SetZoom", renderTargetSetZoom, METH_VARARGS },
		{ "GetFreeIndex", renderGetFreeIndex, METH_VARARGS },
		{ "SetCustomZoom", renderTargetSetCustomZoom, METH_VARARGS },
		{"ResetModel", renderTargetResetModel, METH_VARARGS},
		{"RemoveRenderTarget", renderTargetRemoveRenderTarget, METH_VARARGS},
		{nullptr, nullptr, 0 },
	};

	PyObject* poModule = Py_InitModule("renderTarget", s_methods);

}