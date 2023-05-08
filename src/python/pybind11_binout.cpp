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
#include <binout.hpp>
#include <pybind11/pybind11.h>
#include <tuple>

namespace py = pybind11;

void add_binout_library_to_module(py::module_ &m) {
  py::enum_<dro::BinoutType>(m, "BinoutType")
      .value("Int8", dro::BinoutType::Int8)
      .value("Int16", dro::BinoutType::Int16)
      .value("Int32", dro::BinoutType::Int32)
      .value("Int64", dro::BinoutType::Int64)
      .value("Uint8", dro::BinoutType::Uint8)
      .value("Uint16", dro::BinoutType::Uint16)
      .value("Uint32", dro::BinoutType::Uint32)
      .value("Uint64", dro::BinoutType::Uint64)
      .value("Float32", dro::BinoutType::Float32)
      .value("Float64", dro::BinoutType::Float64)
      .value("Invalid", dro::BinoutType::Invalid)

      ;

  py::class_<dro::Binout>(m, "Binout")
      .def(py::init<const std::string &>())
      .def("read_i8", &dro::Binout::read<int8_t>, "For internal use",
           py::arg("path_to_variable"))
      .def("read_u8", &dro::Binout::read<uint8_t>, "For internal use",
           py::arg("path_to_variable"))
      .def("read_i16", &dro::Binout::read<int16_t>, "For internal use",
           py::arg("path_to_variable"))
      .def("read_u16", &dro::Binout::read<uint16_t>, "For internal use",
           py::arg("path_to_variable"))
      .def("read_i32", &dro::Binout::read<int32_t>, "For internal use",
           py::arg("path_to_variable"))
      .def("read_u32", &dro::Binout::read<uint32_t>, "For internal use",
           py::arg("path_to_variable"))
      .def("read_i64", &dro::Binout::read<int64_t>, "For internal use",
           py::arg("path_to_variable"))
      .def("read_u64", &dro::Binout::read<uint64_t>, "For internal use",
           py::arg("path_to_variable"))
      .def("read_f32", &dro::Binout::read<float>, "For internal use",
           py::arg("path_to_variable"))
      .def("read_f64", &dro::Binout::read<double>, "For internal use",
           py::arg("path_to_variable"))
      .def("read_timed_f32", &dro::Binout::read_timed<float>,
           "For internal use", py::arg("path_to_variable"))
      .def("read_timed_f64", &dro::Binout::read_timed<double>,
           "For internal use", py::arg("path_to_variable"))
      .def("get_type_id", &dro::Binout::get_type_id,
           "Returns the type id of the given variable.",
           py::arg("path_to_variable"))
      .def("variable_exists", &dro::Binout::variable_exists,
           "Returns whether a record with the given path and variable name "
           "exists.",
           py::arg("path_to_variable"))
      .def("get_children", &dro::Binout::get_children,
           "Returns the entries under a given path.", py::arg("path"))
      .def("get_num_timesteps", &dro::Binout::get_num_timesteps,
           "Returns the number of dxxxxxx folders inside of a given path. "
           "Each folder inside a binout can have a different number of time "
           "steps. This method is used to get the time steps of one single "
           "folder (e.g. /nodout or /rcforc).",
           py::arg("path"))
      .def(
          "simple_path_to_real",
          [](const dro::Binout &bin_file, const std::string &simple) {
            dro::BinoutType type_id;
            bool timed;

            auto real_path =
                bin_file.simple_path_to_real(simple, type_id, timed);

            return std::make_tuple(std::move(real_path), type_id, timed);
          },
          "This takes a path that without metadata or dxxxxxx in it and "
          "converts it into a path which can be given to the read_type and "
          "read_timed functions. Returns the path that points to the correct "
          "file or throws an exception if the file does not exist or the path "
          "is invalid. The bool inside the tuple (timed) will be set to false "
          "if it's a file of metadata and to true if it's a file of on of the "
          "dxxxxxx folders which means that the returned path should be given "
          "to the read_timed function. The BinoutType of the tuple (type_id) "
          "will be set to the correct type id of the file. This is used "
          "internally for the read method, so that a simpler path can be "
          "provided. This method can be useful if writing an application that "
          "gets a path from an user.",
          py::arg("simple"))

      ;
}