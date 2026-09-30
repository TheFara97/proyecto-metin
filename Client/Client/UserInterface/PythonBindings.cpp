#include "StdAfx.h"
#include "PythonBindings.h"
#include <Bind11/embed.h>

PYBIND11_EMBEDDED_MODULE(src, m)
{
	init_template(m);
}

void init_template(py::module& m)
{
}