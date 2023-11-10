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
#include <algorithm>
#include <binout.hpp>
#include <cstring>
#include <pybind11/pybind11.h>
#include <tuple>
#include <variant>

namespace py = pybind11;

std::variant<dro::Array<int8_t>, dro::Array<int16_t>, dro::Array<int32_t>,
             dro::Array<int64_t>, dro::Array<uint8_t>, dro::Array<uint16_t>,
             dro::Array<uint32_t>, dro::Array<uint64_t>, dro::Array<float>,
             dro::Array<double>, std::vector<dro::Array<int8_t>>,
             std::vector<dro::Array<int16_t>>, std::vector<dro::Array<int32_t>>,
             std::vector<dro::Array<int64_t>>, std::vector<dro::Array<uint8_t>>,
             std::vector<dro::Array<uint16_t>>,
             std::vector<dro::Array<uint32_t>>,
             std::vector<dro::Array<uint64_t>>, std::vector<dro::Array<float>>,
             std::vector<dro::Array<double>>, std::vector<dro::String>>
Binout_read(dro::Binout &self, std::string path) {
  dro::BinoutType type_id;
  bool timed;
  const auto real_path = self.simple_path_to_real(path, type_id, timed);

  if (timed) {
    char *error_buffer;

    switch (type_id) {
    case dro::BinoutType::Int8:
      return self.read_timed<int8_t>(real_path);
    case dro::BinoutType::Int16:
      return self.read_timed<int16_t>(real_path);
    case dro::BinoutType::Int32:
      return self.read_timed<int32_t>(real_path);
    case dro::BinoutType::Int64:
      return self.read_timed<int64_t>(real_path);
    case dro::BinoutType::Uint8:
      return self.read_timed<uint8_t>(real_path);
    case dro::BinoutType::Uint16:
      return self.read_timed<uint16_t>(real_path);
    case dro::BinoutType::Uint32:
      return self.read_timed<uint32_t>(real_path);
    case dro::BinoutType::Uint64:
      return self.read_timed<uint64_t>(real_path);
    case dro::BinoutType::Float32:
      return self.read_timed<float>(real_path);
    case dro::BinoutType::Float64:
      return self.read_timed<double>(real_path);
    default:
      error_buffer = (char *)malloc(1024);
      sprintf(error_buffer,
              "Unable to read \"%s\" because it has an invalid type",
              path.c_str());
      throw dro::Binout::Exception(
          dro::Binout::Exception::ErrorString(error_buffer));
    }
  } else {
    switch (type_id) {
    case dro::BinoutType::Int8:
      return self.read<int8_t>(real_path);
    case dro::BinoutType::Int16:
      return self.read<int16_t>(real_path);
    case dro::BinoutType::Int32:
      return self.read<int32_t>(real_path);
    case dro::BinoutType::Int64:
      return self.read<int64_t>(real_path);
    case dro::BinoutType::Uint8:
      return self.read<uint8_t>(real_path);
    case dro::BinoutType::Uint16:
      return self.read<uint16_t>(real_path);
    case dro::BinoutType::Uint32:
      return self.read<uint32_t>(real_path);
    case dro::BinoutType::Uint64:
      return self.read<uint64_t>(real_path);
    case dro::BinoutType::Float32:
      return self.read<float>(real_path);
    case dro::BinoutType::Float64:
      return self.read<double>(real_path);
    default:
      // If the type is invalid it's likely to be a folder and then the children
      // should be returned
      auto children = self.get_children(real_path);
      if (children[children.size() - 1] == "metadata" &&
          children[0] == "d000001") {
        const auto metadata = real_path + "/metadata";
        const auto d000001 = real_path + "/d000001";

        auto metadata_children = self.get_children(metadata);
        auto d000001_children = self.get_children(d000001);

        std::move(std::make_move_iterator(d000001_children.begin()),
                  std::make_move_iterator(d000001_children.end()),
                  std::back_inserter(metadata_children));

        std::sort(metadata_children.begin(), metadata_children.end(),
                  [](const auto &lhs, const auto &rhs) {
                    return strcmp(lhs.data(), rhs.data()) < 0;
                  });

        return metadata_children;
      }
      return children;
    }
  }
}

dro::BinoutType Binout_get_type_id(dro::Binout &self, std::string path) {
  dro::BinoutType type_id;
  bool _;
  self.simple_path_to_real(path, type_id, _);
  return type_id;
}

bool Binout_variable_exists(dro::Binout &self, std::string path) noexcept {
  dro::BinoutType _;
  bool __;
  try {
    self.simple_path_to_real(path, _, __);
  } catch (const dro::Binout::Exception &) {
    return false;
  }
  return true;
}

size_t Binout_get_num_timesteps(dro::Binout &self, std::string path) {
  dro::BinoutType _;
  bool __;
  const std::string real_path = self.simple_path_to_real(path, _, __);
  return self.get_num_timesteps(real_path);
}

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
      .def("read", &Binout_read,
           "Read data from the file. This can return a 1D array or a 2D array "
           "if the data under the path is timed (has multiple time steps e.g. "
           "nodout/x_displacement). If the path points to a folder it returns "
           "a list of strings holding the children of said folder.",
           py::arg("path") = "/", py::return_value_policy::take_ownership)
      .def("get_type_id", &Binout_get_type_id,
           "Returns the type id of the given variable.",
           py::arg("path_to_variable"))
      .def("variable_exists", &Binout_variable_exists,
           "Returns whether a record with the given path and variable name "
           "exists.",
           py::arg("path_to_variable"))
      .def("get_num_timesteps", &Binout_get_num_timesteps,
           "Returns the number of dxxxxxx folders inside of a given path. "
           "Each folder inside a binout can have a different number of time "
           "steps. This method is used to get the time steps of one single "
           "folder (e.g. nodout or rcforc).",
           py::arg("path"))

      ;
}