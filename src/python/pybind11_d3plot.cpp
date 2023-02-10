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
#include <d3plot.hpp>
#include <pybind11/pybind11.h>

namespace py = pybind11;

inline void add_d3plot_arrays_to_module(py::module_ &m) {
  dro::add_array_type_to_module<d3plot_solid_con>(m);
  dro::add_array_type_to_module<d3plot_beam_con>(m);
  dro::add_array_type_to_module<d3plot_shell_con>(m);
  dro::add_array_type_to_module<d3plot_solid>(m);
  dro::add_array_type_to_module<d3plot_thick_shell>(m);
  dro::add_array_type_to_module<d3plot_beam>(m);
  dro::add_array_type_to_module<d3plot_shell>(m);
  dro::add_array_type_to_module<dro::dVec3>(m);
}

void add_d3plot_library_to_module(py::module_ &m) {
  add_d3plot_arrays_to_module(m);

  py::class_<d3plot_solid_con>(m, "d3plot_solid_con")
      .def_readonly("node_indices", &d3plot_solid_con::node_indices)
      .def_readonly("material_index", &d3plot_solid_con::material_index)

      ;

  py::class_<d3plot_beam_con>(m, "d3plot_beam_con")
      .def_readonly("node_indices", &d3plot_beam_con::node_indices)
      .def_readonly("orientation_node_index",
                    &d3plot_beam_con::orientation_node_index)
      .def_readonly("material_index", &d3plot_beam_con::material_index)

      ;

  py::class_<d3plot_shell_con>(m, "d3plot_shell_con")
      .def_readonly("node_indices", &d3plot_shell_con::node_indices)
      .def_readonly("material_index", &d3plot_shell_con::material_index)

      ;

  py::class_<d3plot_tensor>(m, "d3plot_tensor")
      .def_readonly("x", &d3plot_tensor::x)
      .def_readonly("y", &d3plot_tensor::y)
      .def_readonly("z", &d3plot_tensor::z)
      .def_readonly("xy", &d3plot_tensor::xy)
      .def_readonly("yx", &d3plot_tensor::yx)
      .def_readonly("yz", &d3plot_tensor::yz)
      .def_readonly("zy", &d3plot_tensor::zy)
      .def_readonly("xz", &d3plot_tensor::xz)
      .def_readonly("zx", &d3plot_tensor::zx)

      ;

  py::class_<d3plot_surface>(m, "d3plot_surface")
      .def_readonly("sigma", &d3plot_surface::sigma)
      .def_readonly("stress", &d3plot_surface::stress)
      .def_readonly("effective_plastic_strain",
                    &d3plot_surface::effective_plastic_strain)
      .def_readonly("material_dependent_value",
                    &d3plot_surface::material_dependent_value)

      ;

  py::class_<d3plot_solid>(m, "d3plot_solid")
      .def_readonly("sigma", &d3plot_solid::sigma)
      .def_readonly("stress", &d3plot_solid::stress)
      .def_readonly("effective_plastic_strain",
                    &d3plot_solid::effective_plastic_strain)
      .def_readonly("material_dependent_value",
                    &d3plot_solid::material_dependent_value)
      .def_readonly("extra1", &d3plot_solid::extra1)
      .def_readonly("extra1", &d3plot_solid::extra2)
      .def_readonly("epsilon", &d3plot_solid::epsilon)
      .def_readonly("strain", &d3plot_solid::strain)

      ;

  py::class_<d3plot_thick_shell>(m, "d3plot_thick_shell")
      .def_readonly("mid", &d3plot_thick_shell::mid)
      .def_readonly("inner", &d3plot_thick_shell::inner)
      .def_readonly("outer", &d3plot_thick_shell::outer)
      .def_readonly("inner_epsilon", &d3plot_thick_shell::inner_epsilon)
      .def_readonly("inner_strain", &d3plot_thick_shell::inner_strain)
      .def_readonly("outer_epsilon", &d3plot_thick_shell::outer_epsilon)
      .def_readonly("outer_strain", &d3plot_thick_shell::outer_strain)

      ;

  py::class_<d3plot_beam>(m, "d3plot_beam")
      .def_readonly("axial_force", &d3plot_beam::axial_force)
      .def_readonly("s_shear_resultant", &d3plot_beam::s_shear_resultant)
      .def_readonly("t_shear_resultant", &d3plot_beam::t_shear_resultant)
      .def_readonly("s_bending_moment", &d3plot_beam::s_bending_moment)
      .def_readonly("t_bending_moment", &d3plot_beam::t_bending_moment)
      .def_readonly("torsional_resultant", &d3plot_beam::torsional_resultant)

      ;

  py::class_<d3plot_shell>(m, "d3plot_shell")
      .def_readonly("mid", &d3plot_shell::mid)
      .def_readonly("inner", &d3plot_shell::inner)
      .def_readonly("outer", &d3plot_shell::outer)
      .def_readonly("inner_epsilon", &d3plot_shell::inner_epsilon)
      .def_readonly("inner_strain", &d3plot_shell::inner_strain)
      .def_readonly("outer_epsilon", &d3plot_shell::outer_epsilon)
      .def_readonly("outer_strain", &d3plot_shell::outer_strain)
      .def_readonly("internal_energy", &d3plot_shell::internal_energy)

      ;

  py::class_<dro::D3plot>(m, "D3plot")
      .def(py::init<const std::string &>())
      .def("read_node_ids", &dro::D3plot::read_node_ids)
      .def("read_solid_element_ids", &dro::D3plot::read_solid_element_ids)
      .def("read_beam_element_ids", &dro::D3plot::read_beam_element_ids)
      .def("read_shell_element_ids", &dro::D3plot::read_shell_element_ids)
      .def("read_solid_shell_element_ids",
           &dro::D3plot::read_solid_shell_element_ids)
      .def("read_all_element_ids", &dro::D3plot::read_all_element_ids)
      .def("read_part_ids", &dro::D3plot::read_part_ids)

      .def("read_part_titles", &dro::D3plot::read_part_titles)

      .def("read_node_coordinates", &dro::D3plot::read_node_coordinates)
      .def("read_node_velocity", &dro::D3plot::read_node_velocity)
      .def("read_node_acceleration", &dro::D3plot::read_node_acceleration)
      .def("read_time", &dro::D3plot::read_time)
      .def("read_solids_state", &dro::D3plot::read_solids_state)
      .def("read_beams_state", &dro::D3plot::read_beams_state)
      .def("read_shells_state", &dro::D3plot::read_shells_state)

      .def("read_solid_elements", &dro::D3plot::read_solid_elements)
      .def("read_thick_shell_elements", &dro::D3plot::read_thick_shell_elements)
      .def("read_beam_elements", &dro::D3plot::read_beam_elements)
      .def("read_shell_elements", &dro::D3plot::read_shell_elements)
      .def("read_title", &dro::D3plot::read_title)
      /*TODO: read_run_time*/
      .def("read_part", &dro::D3plot::read_part)
      .def("read_part_by_id", &dro::D3plot::read_part_by_id, py::arg("part_id"),
           py::arg("part_ids") = dro::Array<d3_word>())

      .def("num_time_steps", &dro::D3plot::num_time_steps)

      ;

  py::class_<dro::D3plotPart>(m, "D3plotPart")
      .def("get_solid_elements", &dro::D3plotPart::get_solid_elements)
      .def("get_thick_shell_elements",
           &dro::D3plotPart::get_thick_shell_elements)
      .def("get_beam_elements", &dro::D3plotPart::get_beam_elements)
      .def("get_shell_elements", &dro::D3plotPart::get_shell_elements)
      .def("get_node_ids", &dro::D3plotPart::get_node_ids, py::arg("plot_file"),
           py::arg("solid_ids") = static_cast<dro::Array<d3_word> *>(nullptr),
           py::arg("beam_ids") = static_cast<dro::Array<d3_word> *>(nullptr),
           py::arg("shell_ids") = static_cast<dro::Array<d3_word> *>(nullptr),
           py::arg("thick_shell_ids") =
               static_cast<dro::Array<d3_word> *>(nullptr),
           py::arg("node_ids") = static_cast<dro::Array<d3_word> *>(nullptr),
           py::arg("solid_cons") =
               static_cast<dro::Array<d3plot_solid_con> *>(nullptr),
           py::arg("beam_cons") =
               static_cast<dro::Array<d3plot_beam_con> *>(nullptr),
           py::arg("shell_cons") =
               static_cast<dro::Array<d3plot_shell_con> *>(nullptr),
           py::arg("thick_shell_cons") =
               static_cast<dro::Array<d3plot_thick_shell_con> *>(nullptr))
      .def("get_node_indices", &dro::D3plotPart::get_node_indices,
           py::arg("plot_file"),
           py::arg("solid_ids") = static_cast<dro::Array<d3_word> *>(nullptr),
           py::arg("beam_ids") = static_cast<dro::Array<d3_word> *>(nullptr),
           py::arg("shell_ids") = static_cast<dro::Array<d3_word> *>(nullptr),
           py::arg("thick_shell_ids") =
               static_cast<dro::Array<d3_word> *>(nullptr),
           py::arg("solid_cons") =
               static_cast<dro::Array<d3plot_solid_con> *>(nullptr),
           py::arg("beam_cons") =
               static_cast<dro::Array<d3plot_beam_con> *>(nullptr),
           py::arg("shell_cons") =
               static_cast<dro::Array<d3plot_shell_con> *>(nullptr),
           py::arg("thick_shell_cons") =
               static_cast<dro::Array<d3plot_thick_shell_con> *>(nullptr))

      ;
}
