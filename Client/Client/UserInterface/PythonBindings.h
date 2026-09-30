#pragma once
#include <Bind11/pybind11.h>
#include <Bind11/stl.h>
#include <Bind11/embed.h>

namespace py = pybind11;

void init_template(py::module&);