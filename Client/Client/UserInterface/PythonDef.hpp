#pragma once

#define PY_METHOD(name) PyObject* name(PyObject* self, PyObject* args)

#define PY_METHODS_EMPTY() static PyMethodDef pythonMethods[] = {{NULL, NULL, NULL}}
#define PY_METHODS(...) static PyMethodDef pythonMethods[] = {__VA_ARGS__, {NULL, NULL, NULL}}
#define PY_METHOD_DEF(name, function) {name, function, METH_VARARGS}

#define PY_MODULE_INIT(moduleName) PyObject* pythonModule = Py_InitModule(moduleName, pythonMethods)

#define PY_MODULE_CINT(name, value) PyModule_AddIntConstant(pythonModule, name, value)
#define PY_MODULE_CSTRING(name, value) PyModule_AddStringConstant(pythonModule, name, value)
