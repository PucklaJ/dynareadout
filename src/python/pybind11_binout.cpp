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
      .def("read_i8", &dro::Binout::read<int8_t>)
      .def("read_u8", &dro::Binout::read<uint8_t>)
      .def("read_i16", &dro::Binout::read<int16_t>)
      .def("read_u16", &dro::Binout::read<uint16_t>)
      .def("read_i32", &dro::Binout::read<int32_t>)
      .def("read_u32", &dro::Binout::read<uint32_t>)
      .def("read_i64", &dro::Binout::read<int64_t>)
      .def("read_u64", &dro::Binout::read<uint64_t>)
      .def("read_f32", &dro::Binout::read<float>)
      .def("read_f64", &dro::Binout::read<double>)
      .def("read_timed_f32", &dro::Binout::read_timed<float>)
      .def("read_timed_f64", &dro::Binout::read_timed<double>)
      .def("get_type_id", &dro::Binout::get_type_id)
      .def("variable_exists", &dro::Binout::variable_exists)
      .def("get_children", &dro::Binout::get_children)
      .def("get_num_timesteps", &dro::Binout::get_num_timesteps)
      .def("simple_path_to_real",
           [](const dro::Binout &bin_file, const std::string &simple) {
             dro::BinoutType type_id;
             bool timed;

             auto real_path =
                 bin_file.simple_path_to_real(simple, type_id, timed);

             return std::make_tuple(std::move(real_path), type_id, timed);
           })

      ;
}