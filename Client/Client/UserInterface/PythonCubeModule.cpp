#include "StdAfx.h"
#include "PythonCube.hpp"
#include "PythonDef.hpp"

#define CUBE PythonCube::Instance()

PY_METHOD(cubeSendMakePacket) {
	uint8_t id;
	if (!PyTuple_GetInteger(args, 0, &id))
		return Py_BuildException();

	uint16_t count;
	if (!PyTuple_GetInteger(args, 1, &count))
		return Py_BuildException();

	TItemPos position;
	if (!PyTuple_GetByte(args, 2, &position.window_type) || !PyTuple_GetInteger(args, 3, &position.cell))
		return Py_BuildException();

	CUBE.SendMakePacket(id, count, std::move(position));
	return Py_BuildNone();
}

PY_METHOD(cubeSendClosePacket) {
	CUBE.SendClosePacket();
	return Py_BuildNone();
}

void initCube() {
	PY_METHODS(
		PY_METHOD_DEF("SendMakePacket", cubeSendMakePacket),
		PY_METHOD_DEF("SendClosePacket", cubeSendClosePacket)
	);

	PY_MODULE_INIT("cube");
}
