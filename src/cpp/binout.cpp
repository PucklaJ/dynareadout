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

#include "binout.hpp"
#include <cstring>

namespace dro {

Binout::Exception::Exception(String error_str) noexcept
    : m_error_str(std::move(error_str)) {}

const char *Binout::Exception::what() const noexcept {
  return m_error_str.data();
}

Binout::Binout(const std::filesystem::path &file_name) {
  m_handle = binout_open(file_name.string().c_str());
  char *open_error = binout_open_error(&m_handle);
  if (open_error) {
    // Call binout_close since the destructor is not getting called
    binout_close(&m_handle);
    throw Exception(String(open_error));
  }
}

Binout::~Binout() noexcept { binout_close(&m_handle); }

BinoutType Binout::get_type_id(const std::string &path_to_variable) const {
  const BinoutType type_id{static_cast<BinoutType>(binout_get_type_id(
      const_cast<binout_file *>(&m_handle), path_to_variable.c_str()))};
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return type_id;
}

bool Binout::variable_exists(
    const std::string &path_to_variable) const noexcept {
  return binout_variable_exists(const_cast<binout_file *>(&m_handle),
                                path_to_variable.c_str());
}

std::vector<String>
Binout::get_children(const std::string &path) const noexcept {
  size_t num_children;
  char **children = binout_get_children(const_cast<binout_file *>(&m_handle),
                                        path.c_str(), &num_children);

  std::vector<String> children_vec;
  for (size_t i = 0; i < num_children; i++) {
    children_vec.emplace_back(children[i], strlen(children[i]) + 1, false);
  }

  free(children);
  return children_vec;
}

size_t Binout::get_num_timesteps(const std::string &path) const {
  const size_t num_timesteps{binout_get_num_timesteps(&m_handle, path.c_str())};

  if (num_timesteps == (size_t)~0) {
    throw Exception(String(
        const_cast<char *>("The path does not exist or has files as children"),
        false));
  }

  return num_timesteps;
}

template <> Array<int8_t> Binout::read(const std::string &path_to_variable) {
  size_t data_size;
  int8_t *data =
      binout_read_int8_t(&m_handle, path_to_variable.c_str(), &data_size);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<int8_t>(data, data_size);
}

template <> Array<int16_t> Binout::read(const std::string &path_to_variable) {
  size_t data_size;
  int16_t *data =
      binout_read_int16_t(&m_handle, path_to_variable.c_str(), &data_size);
  return Array<int16_t>(data, data_size);
}

template <> Array<int32_t> Binout::read(const std::string &path_to_variable) {
  size_t data_size;
  int32_t *data =
      binout_read_int32_t(&m_handle, path_to_variable.c_str(), &data_size);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<int32_t>(data, data_size);
}

template <> Array<int64_t> Binout::read(const std::string &path_to_variable) {
  size_t data_size;
  int64_t *data =
      binout_read_int64_t(&m_handle, path_to_variable.c_str(), &data_size);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<int64_t>(data, data_size);
}

template <> Array<uint8_t> Binout::read(const std::string &path_to_variable) {
  size_t data_size;
  uint8_t *data =
      binout_read_uint8_t(&m_handle, path_to_variable.c_str(), &data_size);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<uint8_t>(data, data_size);
}

template <> Array<uint16_t> Binout::read(const std::string &path_to_variable) {
  size_t data_size;
  uint16_t *data =
      binout_read_uint16_t(&m_handle, path_to_variable.c_str(), &data_size);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<uint16_t>(data, data_size);
}

template <> Array<uint32_t> Binout::read(const std::string &path_to_variable) {
  size_t data_size;
  uint32_t *data =
      binout_read_uint32_t(&m_handle, path_to_variable.c_str(), &data_size);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<uint32_t>(data, data_size);
}

template <> Array<uint64_t> Binout::read(const std::string &path_to_variable) {
  size_t data_size;
  uint64_t *data =
      binout_read_uint64_t(&m_handle, path_to_variable.c_str(), &data_size);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<uint64_t>(data, data_size);
}

template <> Array<float> Binout::read(const std::string &path_to_variable) {
  size_t data_size;
  float *data =
      binout_read_float(&m_handle, path_to_variable.c_str(), &data_size);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<float>(data, data_size);
}

template <> Array<double> Binout::read(const std::string &path_to_variable) {
  size_t data_size;
  double *data =
      binout_read_double(&m_handle, path_to_variable.c_str(), &data_size);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<double>(data, data_size);
}

} // namespace dro
