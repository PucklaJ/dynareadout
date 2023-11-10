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
#include "d3plot_state.hpp"
#include <d3plot.hpp>
#include <pybind11/chrono.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;

inline void add_d3plot_arrays_to_module(py::module_ &m) {
  dro::add_array_type_to_module<d3plot_solid_con>(m);
  dro::add_array_type_to_module<d3plot_beam_con>(m);
  dro::add_array_type_to_module<d3plot_shell_con>(m);
  dro::add_array_type_to_module<d3plot_solid>(m);
  dro::add_array_type_to_module<d3plot_beam>(m);
  dro::add_array_type_to_module<dro::dVec3>(m).def(
      "__repr__", dro::stream_to_string<dro::dVec3>);

  py::class_<dro::D3plotShellsState>(m, dro::get_array_name<d3plot_shell>())
      .def("__len__", &dro::D3plotShellsState::size)
      .def("__getitem__",
           [](dro::D3plotShellsState &self, size_t index) {
             try {
               return self[index];
             } catch (const std::runtime_error &) {
               throw py::index_error("Index out of range");
             }
           })
      .def("get_num_history_variables",
           &dro::D3plotShellsState::get_num_history_variables,
           "Returns the number of history variables per surface")
      .def("get_mid_history_variables",
           &dro::D3plotShellsState::get_mid_history_variables,
           "Returns the history variables for the mid surface of the shell "
           "under index",
           py::arg("index"))
      .def("get_inner_history_variables",
           &dro::D3plotShellsState::get_inner_history_variables,
           "Returns the history variables for the inner surface of the shell "
           "under index",
           py::arg("index"))
      .def("get_outer_history_variables",
           &dro::D3plotShellsState::get_outer_history_variables,
           "Returns the history variables for the outer surface of the shell "
           "under index",
           py::arg("index"))

      ;

  py::class_<dro::D3plotThickShellsState>(
      m, dro::get_array_name<d3plot_thick_shell>())
      .def("__len__", &dro::D3plotThickShellsState::size)
      .def("__getitem__",
           [](dro::D3plotThickShellsState &self, size_t index) {
             try {
               return self[index];
             } catch (const std::runtime_error &) {
               throw py::index_error("Index out of range");
             }
           })
      .def("get_num_history_variables",
           &dro::D3plotThickShellsState::get_num_history_variables,
           "Returns the number of history variables per surface")
      .def("get_mid_history_variables",
           &dro::D3plotThickShellsState::get_mid_history_variables,
           "Returns the history variables of the mid surface of the thick "
           "shell under index",
           py::arg("index"))
      .def("get_inner_history_variables",
           &dro::D3plotThickShellsState::get_inner_history_variables,
           "Returns the history variables of the inner surface of the thick "
           "shell under index",
           py::arg("index"))
      .def("get_outer_history_variables",
           &dro::D3plotThickShellsState::get_outer_history_variables,
           "Returns the history variables of the outer surface of the thick "
           "shell under index",
           py::arg("index"))

      ;
}

void add_d3plot_library_to_module(py::module_ &m) {
  add_d3plot_arrays_to_module(m);

  py::class_<d3plot_solid_con>(m, "d3plot_solid_con")
      .def_readonly(
          "node_indices", &d3plot_solid_con::node_indices,
          "Stores indices into the node_ids, node_coords, etc. arrays")
      .def_readonly("material_index", &d3plot_solid_con::material_index,
                    "Index into the parts (this those indeed refer to parts "
                    "even though the documentation does not say so)")
      .def("__str__", &dro::stream_to_string<d3plot_solid_con>,
           py::return_value_policy::take_ownership)

      ;

  py::class_<d3plot_beam_con>(m, "d3plot_beam_con")
      .def_readonly(
          "node_indices", &d3plot_beam_con::node_indices,
          "Stores indices into the node_ids, node_coords, etc. arrays")
      .def_readonly("orientation_node_index",
                    &d3plot_beam_con::orientation_node_index)
      .def_readonly("material_index", &d3plot_beam_con::material_index,
                    "Index into the parts (this those indeed refer to parts "
                    "even though the documentation does not say so)")
      .def("__str__", &dro::stream_to_string<d3plot_beam_con>,
           py::return_value_policy::take_ownership)

      ;

  py::class_<d3plot_shell_con>(m, "d3plot_shell_con")
      .def_readonly(
          "node_indices", &d3plot_shell_con::node_indices,
          "Stores indices into the node_ids, node_coords, etc. arrays")
      .def_readonly("material_index", &d3plot_shell_con::material_index,
                    "Index into the parts (this those indeed refer to parts "
                    "even though the documentation does not say so)")
      .def("__str__", &dro::stream_to_string<d3plot_shell_con>,
           py::return_value_policy::take_ownership)

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
      .def("__str__", &dro::stream_to_string<d3plot_tensor>,
           py::return_value_policy::take_ownership)

      ;

  py::class_<d3plot_x_y>(m, "d3plot_x_y")
      .def_readonly("x", &d3plot_x_y::x)
      .def_readonly("y", &d3plot_x_y::y)
      .def("__str__", &dro::stream_to_string<d3plot_x_y>,
           py::return_value_policy::take_ownership)

      ;

  py::class_<d3plot_x_y_xy>(m, "d3plot_x_y_xy")
      .def_readonly("x", &d3plot_x_y_xy::x)
      .def_readonly("y", &d3plot_x_y_xy::y)
      .def_readonly("xy", &d3plot_x_y_xy::xy)
      .def("__str__", &dro::stream_to_string<d3plot_x_y_xy>,
           py::return_value_policy::take_ownership)

      ;

  py::class_<d3plot_surface>(m, "d3plot_surface")
      .def_readonly("sigma", &d3plot_surface::sigma)
      .def_readonly("stress", &d3plot_surface::stress)
      .def_readonly("effective_plastic_strain",
                    &d3plot_surface::effective_plastic_strain)
      .def_readonly("material_dependent_value",
                    &d3plot_surface::material_dependent_value)
      .def("__str__", &dro::stream_to_string<d3plot_surface>,
           py::return_value_policy::take_ownership)

      ;

  py::class_<d3plot_solid>(m, "d3plot_solid")
      .def_readonly("sigma", &d3plot_solid::sigma)
      .def_readonly("stress", &d3plot_solid::stress)
      .def_readonly("effective_plastic_strain",
                    &d3plot_solid::effective_plastic_strain)
      .def_readonly("material_dependent_value",
                    &d3plot_solid::material_dependent_value)
      .def_readonly("epsilon", &d3plot_solid::epsilon)
      .def_readonly("strain", &d3plot_solid::strain)
      .def("__str__", &dro::stream_to_string<d3plot_solid>,
           py::return_value_policy::take_ownership)

      ;

  py::class_<d3plot_thick_shell>(m, "d3plot_thick_shell")
      .def_readonly("mid", &d3plot_thick_shell::mid)
      .def_readonly("inner", &d3plot_thick_shell::inner)
      .def_readonly("outer", &d3plot_thick_shell::outer)
      .def_readonly("inner_epsilon", &d3plot_thick_shell::inner_epsilon)
      .def_readonly("inner_strain", &d3plot_thick_shell::inner_strain)
      .def_readonly("outer_epsilon", &d3plot_thick_shell::outer_epsilon)
      .def_readonly("outer_strain", &d3plot_thick_shell::outer_strain)
      .def("__str__", &dro::stream_to_string<d3plot_thick_shell>,
           py::return_value_policy::take_ownership)

      ;

  py::class_<d3plot_beam>(m, "d3plot_beam")
      .def_readonly("axial_force", &d3plot_beam::axial_force)
      .def_readonly("s_shear_resultant", &d3plot_beam::s_shear_resultant)
      .def_readonly("t_shear_resultant", &d3plot_beam::t_shear_resultant)
      .def_readonly("s_bending_moment", &d3plot_beam::s_bending_moment)
      .def_readonly("t_bending_moment", &d3plot_beam::t_bending_moment)
      .def_readonly("torsional_resultant", &d3plot_beam::torsional_resultant)
      .def("__str__", &dro::stream_to_string<d3plot_beam>,
           py::return_value_policy::take_ownership)

      ;

  py::class_<d3plot_shell>(m, "d3plot_shell")
      .def_readonly("mid", &d3plot_shell::mid)
      .def_readonly("inner", &d3plot_shell::inner)
      .def_readonly("outer", &d3plot_shell::outer)
      .def_readonly("inner_epsilon", &d3plot_shell::inner_epsilon)
      .def_readonly("inner_strain", &d3plot_shell::inner_strain)
      .def_readonly("outer_epsilon", &d3plot_shell::outer_epsilon)
      .def_readonly("outer_strain", &d3plot_shell::outer_strain)
      .def_readonly("bending_moment", &d3plot_shell::bending_moment)
      .def_readonly("shear_resultant", &d3plot_shell::shear_resultant)
      .def_readonly("normal_resultant", &d3plot_shell::normal_resultant)
      .def_readonly("thickness", &d3plot_shell::thickness)
      .def_readonly("element_dependent_variables",
                    &d3plot_shell::element_dependent_variables)
      .def_readonly("internal_energy", &d3plot_shell::internal_energy)
      .def("__str__", &dro::stream_to_string<d3plot_shell>,
           py::return_value_policy::take_ownership)

      ;

  py::class_<dro::D3plot>(m, "D3plot")
      .def(py::init<const std::string &>(),
           "Open a d3plot file family by giving the root file name\nExample: "
           "d3plot of d3plot01, d3plot02, d3plot03, etc.",
           py::arg("root_file_name"))
      .def("read_node_ids", &dro::D3plot::read_node_ids,
           "Read all ids of the nodes.",
           py::return_value_policy::take_ownership)
      .def("read_solid_element_ids", &dro::D3plot::read_solid_element_ids,
           "Read all ids of the solid elements.",
           py::return_value_policy::take_ownership)
      .def("read_beam_element_ids", &dro::D3plot::read_beam_element_ids,
           "Read all ids of the beam elements.",
           py::return_value_policy::take_ownership)
      .def("read_shell_element_ids", &dro::D3plot::read_shell_element_ids,
           "Read all ids of the shell elements.",
           py::return_value_policy::take_ownership)
      .def("read_thick_shell_element_ids",
           &dro::D3plot::read_thick_shell_element_ids,
           "Read all ids of the thick shell elements.",
           py::return_value_policy::take_ownership)
      .def("read_all_element_ids", &dro::D3plot::read_all_element_ids,
           "Read all ids of the solid, beam, shell and thick shell elements.",
           py::return_value_policy::take_ownership)
      .def("read_part_ids", &dro::D3plot::read_part_ids,
           "Read all ids of the parts.",
           py::return_value_policy::take_ownership)

      .def("read_part_titles", &dro::D3plot::read_part_titles,
           "Returns a list containing all part titles as sized strings.",
           py::return_value_policy::take_ownership)

      .def(
          "read_node_coordinates", &dro::D3plot::read_node_coordinates,
          "Read the node coordinates of all nodes of a given state (time step)",
          py::arg("state") = static_cast<size_t>(0),
          py::return_value_policy::take_ownership)
      .def("read_all_node_coordinates", &dro::D3plot::read_all_node_coordinates,
           "Reads all node coordinates of all time steps and returns them as "
           "one big array.",
           py::return_value_policy::take_ownership)
      .def("read_node_velocity", &dro::D3plot::read_node_velocity,
           "Read the node velocity of all nodes of a given state (time step).",
           py::arg("state"), py::return_value_policy::take_ownership)
      .def("read_all_node_velocity", &dro::D3plot::read_all_node_velocity,
           "Reads all node velocities of all time steps and returns them as "
           "one big array.",
           py::return_value_policy::take_ownership)
      .def("read_node_acceleration", &dro::D3plot::read_node_acceleration,
           "Read the node acceleration of all nodes of a given state (time "
           "step).",
           py::arg("state"), py::return_value_policy::take_ownership)
      .def("read_all_node_acceleration",
           &dro::D3plot::read_all_node_acceleration,
           "Read all node accelerations of all time steps and returns the as "
           "one big array.",
           py::return_value_policy::take_ownership)
      .def("read_time", &dro::D3plot::read_time,
           "Read the time of a given state (time step) in milliseconds.",
           py::arg("state"), py::return_value_policy::take_ownership)
      .def("read_all_time", &dro::D3plot::read_all_time,
           "Reads all time of every state (time step) in milliseconds.",
           py::return_value_policy::take_ownership)
      .def("read_solids_state", &dro::D3plot::read_solids_state,
           "Returns stress, strain (if NEIPH >= 6) for a given state.",
           py::arg("state"), py::return_value_policy::take_ownership)
      .def("read_thick_shells_state", &dro::D3plot::read_thick_shells_state,
           "Returns stress, strain (if ISTRN == 1) for a given state.",
           py::arg("state"), py::return_value_policy::take_ownership)
      .def("read_beams_state", &dro::D3plot::read_beams_state,
           "Returns Axial Force, S shear resultant, T shear resultant, S "
           "bending moment, T bending moment and Torsional resultant of all "
           "beams for a given state.",
           py::arg("state"), py::return_value_policy::take_ownership)
      .def("read_shells_state", &dro::D3plot::read_shells_state,
           "Returns stress, strain (if ISTRN == 1) and some other variables "
           "(see docs pg. 36) of all shells for a given state.",
           py::arg("state"), py::return_value_policy::take_ownership)

      .def("read_solid_elements", &dro::D3plot::read_solid_elements,
           "Returns the node connectivity + material number of all 8 node "
           "solid elements.",
           py::return_value_policy::take_ownership)
      .def(
          "read_thick_shell_elements", &dro::D3plot::read_thick_shell_elements,
          "Returns the node connectivity + material number of all 8 node thick "
          "shell elements. Technically it returns a SolidConArray, since the "
          "connectivity is the same between solids and thick shells.",
          py::return_value_policy::take_ownership)
      .def("read_beam_elements", &dro::D3plot::read_beam_elements,
           "Returns the node connectivity + orientation node + material number "
           "of all beam elements.",
           py::return_value_policy::take_ownership)
      .def("read_shell_elements", &dro::D3plot::read_shell_elements,
           "Returns the node connectivity + material number of all shell "
           "elements.",
           py::return_value_policy::take_ownership)
      .def("read_title", &dro::D3plot::read_title,
           "Returns a string holding the Title of the d3plot file.",
           py::return_value_policy::take_ownership)
      .def("read_run_time", &dro::D3plot::read_run_time,
           "Returns the time at which the simulation has been run as calender "
           "time.",
           py::return_value_policy::take_ownership)
      .def("read_part", &dro::D3plot::read_part,
           "Returns all elements (solid, thick shell, beam, shell) of a part. "
           "The part_index can retrieved by iterating over the array returned "
           "by read_part_ids.",
           py::arg("part_index"), py::return_value_policy::take_ownership)
      .def(
          "read_part_by_id", &dro::D3plot::read_part_by_id,
          "The same as read_part, but instead of an index into the parts, this "
          "functions takes an id. You can supply this function with the part "
          "ids returned by read_part_ids. If you do not do this, they will be "
          "read in this function call. Which means that if you intend to call "
          "this function multiple times, it is best to preload the part ids.",
          py::arg("part_id"), py::arg("part_ids") = dro::Array<d3_word>(),
          py::return_value_policy::take_ownership)

      .def("num_time_steps", &dro::D3plot::num_time_steps,
           "Returns the number of states (time steps).")

      ;

  py::class_<dro::D3plotPart>(m, "D3plotPart")
      .def("get_solid_elements", &dro::D3plotPart::get_solid_elements,
           "Returns all solid element ids of the part.",
           py::return_value_policy::take_ownership)
      .def("get_thick_shell_elements",
           &dro::D3plotPart::get_thick_shell_elements,
           "Returns all thick shell element ids of the part.",
           py::return_value_policy::take_ownership)
      .def("get_beam_elements", &dro::D3plotPart::get_beam_elements,
           "Returns all beam element ids of the part.",
           py::return_value_policy::take_ownership)
      .def("get_shell_elements", &dro::D3plotPart::get_shell_elements,
           "Returns all shell element ids of the part.",
           py::return_value_policy::take_ownership)
      .def("get_solid_element_indices",
           &dro::D3plotPart::get_solid_element_indices,
           "Returns all solid element indices (into the array returned by "
           "dynareadout.D3plot.read_solid_element_ids) of the part.",
           py::return_value_policy::take_ownership)
      .def("get_thick_shell_element_indices",
           &dro::D3plotPart::get_thick_shell_element_indices,
           "Returns all thick shell element indices (into the array returned "
           "by dynareadout.D3plot.read_solid_element_ids) of the part.",
           py::return_value_policy::take_ownership)
      .def("get_beam_element_indices",
           &dro::D3plotPart::get_beam_element_indices,
           "Returns all beam element indices (into the array returned by "
           "dynareadout.D3plot.read_solid_element_ids) of the part.",
           py::return_value_policy::take_ownership)
      .def("get_shell_element_indices",
           &dro::D3plotPart::get_shell_element_indices,
           "Returns all shell element indices (into the array returned by "
           "dynareadout.D3plot.read_solid_element_ids) of the part.",
           py::return_value_policy::take_ownership)
      .def("get_node_ids", &dro::D3plotPart::get_node_ids,
           "Returns all node ids of the part. All ids and connectivities of "
           "all different elements can be provided to improve performance, "
           "since they would not need to be loaded.",
           py::arg("plot_file"),
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
               static_cast<dro::Array<d3plot_thick_shell_con> *>(nullptr),
           py::return_value_policy::take_ownership)
      .def("get_node_indices", &dro::D3plotPart::get_node_indices,
           "Returns all node indices of the part. All ids and connectivities "
           "of all different elements can be provided to improve performance, "
           "since they would not need to be loaded. This returns indices into "
           "the node ids array returned by dynareadout.D3plot.read_node_ids.",
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
               static_cast<dro::Array<d3plot_thick_shell_con> *>(nullptr),
           py::return_value_policy::take_ownership)
      .def("get_num_nodes", &dro::D3plotPart::get_num_nodes,
           "Returns the number of nodes of a part. Internally it just calls "
           "get_node_indices and returns the number of node indices.",
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
      .def("get_num_elements", &dro::D3plotPart::get_num_elements,
           "Returns the number of all elements of a part.")
      .def("get_all_element_ids", &dro::D3plotPart::get_all_element_ids,
           "Returns an array containing all element ids.",
           py::return_value_policy::take_ownership)

      ;
}
