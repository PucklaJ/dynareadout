/***********************************************************************************
 *                         This file is part of dynareadout
 *                    https://github.com/PucklaJ/dynareadout
 ***********************************************************************************
 * Copyright (c) 2022 Jonas Pucher
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not claim
 * that you wrote the original software. If you use this software in a product,
 * an acknowledgment in the product documentation would be appreciated but is
 * not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 ************************************************************************************/

#include "conversions.hpp"
#include <pybind11/pybind11.h>

namespace py = pybind11;

void add_binout_library_to_module(py::module_ &m);
void add_d3plot_library_to_module(py::module_ &m);
void add_key_library_to_module(py::module_ &m);

#ifdef NDEBUG
PYBIND11_MODULE(dynareadout, m) {
#else
PYBIND11_MODULE(dynareadout_d, m) {
#endif
  dro::add_array_to_module(m);
  add_binout_library_to_module(m);
  add_d3plot_library_to_module(m);
  add_key_library_to_module(m);
}