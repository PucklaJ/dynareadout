/***********************************************************************************
 *                         This file is part of dynareadout
 *                    https://github.com/PucklaMotzer09/dynareadout
 ***********************************************************************************
 * Copyright (c) 2022 PucklaMotzer09
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
#include <d3plot.hpp>
#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace dro {

py::list dVec3_array_to_python_list(Array<dVec3> &&arr) {
  py::list l;
  for (const auto &v : arr) {
    py::list l1;
    l1.append(v[0]);
    l1.append(v[1]);
    l1.append(v[2]);
    l.append(l1);
  }

  return l;
}

} // namespace dro

#define D3PLOT_READ_WRAPPER(func)                                              \
  [](dro::D3plot &plot_file) { return array_vector_wrapper(plot_file.func()); }
#define D3PLOT_READ_STATE_WRAPPER(func)                                        \
  [](dro::D3plot &plot_file, size_t state) {                                   \
    return dVec3_array_to_python_list(plot_file.func(state));                  \
  }

PYBIND11_MODULE(libd3plot, m) {
  py::class_<d3plot_solid>(m, "d3plot_solid")
      .def_readonly("node_ids", &d3plot_solid::node_ids)
      .def_readonly("material_id", &d3plot_solid::material_id)

      ;

  py::class_<d3plot_beam>(m, "d3plot_beam")
      .def_readonly("node_ids", &d3plot_beam::node_ids)
      .def_readonly("orientation_node_id", &d3plot_beam::orientation_node_id)
      .def_readonly("material_id", &d3plot_beam::material_id)

      ;

  py::class_<d3plot_shell>(m, "d3plot_shell")
      .def_readonly("node_ids", &d3plot_shell::node_ids)
      .def_readonly("material_id", &d3plot_shell::material_id)

      ;

  py::class_<dro::D3plot>(m, "D3plot")
      .def(py::init<const std::string &>())
      .def("read_node_ids", D3PLOT_READ_WRAPPER(read_node_ids))
      .def("read_solid_element_ids",
           D3PLOT_READ_WRAPPER(read_solid_element_ids))
      .def("read_beam_element_ids", D3PLOT_READ_WRAPPER(read_beam_element_ids))
      .def("read_shell_element_ids",
           D3PLOT_READ_WRAPPER(read_shell_element_ids))
      .def("read_solid_shell_element_ids",
           D3PLOT_READ_WRAPPER(read_solid_shell_element_ids))
      .def("read_all_element_ids", D3PLOT_READ_WRAPPER(read_all_element_ids))
      .def("read_node_coordinates",
           D3PLOT_READ_STATE_WRAPPER(read_node_coordinates))
      .def("read_node_velocity", D3PLOT_READ_STATE_WRAPPER(read_node_velocity))
      .def("read_node_acceleration",
           D3PLOT_READ_STATE_WRAPPER(read_node_acceleration))
      .def("read_time", &dro::D3plot::read_time)
      .def("read_solid_elements", D3PLOT_READ_WRAPPER(read_solid_elements))
      .def("read_thick_shell_elements",
           D3PLOT_READ_WRAPPER(read_thick_shell_elements))
      .def("read_beam_elements", D3PLOT_READ_WRAPPER(read_beam_elements))
      .def("read_shell_elements", D3PLOT_READ_WRAPPER(read_shell_elements))
      .def("read_title",
           [](dro::D3plot &plot_file) { return plot_file.read_title().str(); })
      /*TODO: read_run_time*/
      .def("num_time_steps", &dro::D3plot::num_time_steps)

      ;
}