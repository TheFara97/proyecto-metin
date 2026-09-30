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

	const auto render = CRenderTargetManager::Instance().GetRenderTarget(index);

	if (!render)
		return Py_BuildNone();

	render->SelectModel(modelIndex);
	return Py_BuildNone();
}

PyObject* renderTargetSetVisibility(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	bool isShow = false;
	if (!PyTuple_GetBoolean(poArgs, 1, &isShow))
		return Py_BadArgument();

	const auto render = CRenderTargetManager::Instance().GetRenderTarget(index);

	if (!render)
		return Py_BuildNone();

	render->SetVisibility(isShow);
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

	const auto render = CRenderTargetManager::Instance().GetRenderTarget(index);

	if (!render)
		return Py_BuildNone();

	render->CreateBackground(
		szPathName, CPythonApplication::Instance().GetWidth(),
		CPythonApplication::Instance().GetHeight());
	return Py_BuildNone();
}

PyObject* renderTargetChangeArmor(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	int vnum;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	if (!PyTuple_GetInteger(poArgs, 1, &vnum))
		return Py_BadArgument();

	const auto render = CRenderTargetManager::Instance().GetRenderTarget(index);

	if (!render)
		return Py_BuildNone();

	render->ChangeArmor(vnum);
	return Py_BuildNone();
}

PyObject* renderTargetChangeWeapon(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	int vnum;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	if (!PyTuple_GetInteger(poArgs, 1, &vnum))
		return Py_BadArgument();

	const auto render = CRenderTargetManager::Instance().GetRenderTarget(index);

	if (!render)
		return Py_BuildNone();

	render->ChangeWeapon(vnum);
	return Py_BuildNone();
}

PyObject* renderTargetChangeHair(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	int vnum;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	if (!PyTuple_GetInteger(poArgs, 1, &vnum))
		return Py_BadArgument();

	const auto render = CRenderTargetManager::Instance().GetRenderTarget(index);

	if (!render)
		return Py_BuildNone();

	render->ChangeHair(vnum);
	return Py_BuildNone();
}

void initRenderTarget() {
	static PyMethodDef s_methods[] =
	{
		{ "SelectModel",			renderTargetSelectModel,	METH_VARARGS },
		{ "SetVisibility",			renderTargetSetVisibility,	METH_VARARGS },
		{ "SetBackground",			renderTargetSetBackground,	METH_VARARGS },

		{ "ChangeArmor", renderTargetChangeArmor, METH_VARARGS },
		{ "ChangeWeapon", renderTargetChangeWeapon, METH_VARARGS },
		{ "ChangeHair", renderTargetChangeHair, METH_VARARGS },

		{ NULL,						NULL,						NULL		 },
	};

	Py_InitModule("renderTarget", s_methods);

}