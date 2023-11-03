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
#include <functional>

namespace dro {

Binout::Exception::Exception(Binout::Exception::ErrorString error_str) noexcept
    : m_error_str(std::move(error_str)) {}

const char *Binout::Exception::what() const noexcept {
  return m_error_str.data();
}

Binout::Binout(Binout &&rhs) noexcept { *this = std::move(rhs); }

Binout::Binout(const std::filesystem::path &file_name) {
  m_handle = binout_open(file_name.string().c_str());
  char *open_error = binout_open_error(&m_handle);
  if (open_error) {
    // Call binout_close since the destructor is not getting called
    binout_close(&m_handle);
    throw Exception(Exception::ErrorString(open_error));
  }
}

Binout::~Binout() noexcept { binout_close(&m_handle); }

Binout &Binout::operator=(Binout &&rhs) noexcept {
  m_handle = rhs.m_handle;
  rhs.m_handle = {0};
  return *this;
}

BinoutType Binout::get_type_id(const std::string &path_to_variable) const {
  const BinoutType type_id{static_cast<BinoutType>(binout_get_type_id(
      const_cast<binout_file *>(&m_handle), path_to_variable.c_str()))};
  if (m_handle.error_string) {
    throw Exception(Exception::ErrorString(m_handle.error_string, false));
  }

  return type_id;
}

bool Binout::variable_exists(
    const std::string &path_to_variable) const noexcept {
  return binout_variable_exists(const_cast<binout_file *>(&m_handle),
                                path_to_variable.c_str());
}

std::vector<String> Binout::get_children(const std::string &path) const {
  size_t num_children;
  char **children = binout_get_children(const_cast<binout_file *>(&m_handle),
                                        path.c_str(), &num_children);

  // If the path does not exist
  if (children == nullptr && num_children == static_cast<size_t>(~0)) {
    char *msg = reinterpret_cast<char *>(malloc(256 + path.length()));
    sprintf(msg, "The path \"%s\" does not exist", path.c_str());
    throw Exception(Exception::ErrorString(msg));
  }

  std::vector<String> children_vec;
  for (size_t i = 0; i < num_children; i++) {
    children_vec.emplace_back(children[i], false);
  }

  free(children);
  return children_vec;
}

size_t Binout::get_num_timesteps(const std::string &path) const {
  const size_t num_timesteps{binout_get_num_timesteps(&m_handle, path.c_str())};

  if (num_timesteps == (size_t)~0) {
    throw Exception(Exception::ErrorString(
        const_cast<char *>("The path does not exist or has files as children"),
        false));
  }

  return num_timesteps;
}

template <typename T>
inline Array<T>
Binout_read(Binout &bin_file,
            std::function<T *(binout_file *handle, const char *path_to_variable,
                              size_t *data_size)>
                load_func,
            const std::string &path_to_variable) {
  size_t data_size;
  T *data =
      load_func(&bin_file.get_handle(), path_to_variable.c_str(), &data_size);
  if (bin_file.get_handle().error_string) {
    throw Binout::Exception(Binout::Exception::ErrorString(
        bin_file.get_handle().error_string, false));
  }

  return Array<T>(data, data_size);
}

template <> Array<int8_t> Binout::read(const std::string &path_to_variable) {
  return Binout_read<int8_t>(*this, binout_read_i8, path_to_variable);
}

template <> Array<int16_t> Binout::read(const std::string &path_to_variable) {
  return Binout_read<int16_t>(*this, binout_read_i16, path_to_variable);
}

template <> Array<int32_t> Binout::read(const std::string &path_to_variable) {
  return Binout_read<int32_t>(*this, binout_read_i32, path_to_variable);
}

template <> Array<int64_t> Binout::read(const std::string &path_to_variable) {
  return Binout_read<int64_t>(*this, binout_read_i64, path_to_variable);
}

template <> Array<uint8_t> Binout::read(const std::string &path_to_variable) {
  return Binout_read<uint8_t>(*this, binout_read_u8, path_to_variable);
}

template <> Array<uint16_t> Binout::read(const std::string &path_to_variable) {
  return Binout_read<uint16_t>(*this, binout_read_u16, path_to_variable);
}

template <> Array<uint32_t> Binout::read(const std::string &path_to_variable) {
  return Binout_read<uint32_t>(*this, binout_read_u32, path_to_variable);
}

template <> Array<uint64_t> Binout::read(const std::string &path_to_variable) {
  return Binout_read<uint64_t>(*this, binout_read_u64, path_to_variable);
}

template <> Array<float> Binout::read(const std::string &path_to_variable) {
  return Binout_read<float>(*this, binout_read_f32, path_to_variable);
}

template <> Array<double> Binout::read(const std::string &path_to_variable) {
  return Binout_read<double>(*this, binout_read_f64, path_to_variable);
}

template <typename T>
inline std::vector<Array<T>>
Binout_read_timed(Binout &bin_file,
                  std::function<T *(binout_file *handle, const char *variable,
                                    size_t *num_values, size_t *num_timesteps)>
                      load_func,
                  const std::string &variable) {
  size_t num_values, num_timesteps;
  T *data = load_func(&bin_file.get_handle(), variable.c_str(), &num_values,
                      &num_timesteps);
  if (bin_file.get_handle().error_string) {
    throw Binout::Exception(Binout::Exception::ErrorString(
        bin_file.get_handle().error_string, false));
  }

  std::vector<Array<T>> vec(num_timesteps);

  for (size_t t = 0; t < num_timesteps; t++) {
    vec[t] = Array<T>(&data[t * num_values], num_values, t == 0);
  }

  return vec;
}

template <>
std::vector<Array<int8_t>> Binout::read_timed(const std::string &variable) {
  return Binout_read_timed<int8_t>(*this, binout_read_timed_i8, variable);
}

template <>
std::vector<Array<int16_t>> Binout::read_timed(const std::string &variable) {
  return Binout_read_timed<int16_t>(*this, binout_read_timed_i16, variable);
}

template <>
std::vector<Array<int32_t>> Binout::read_timed(const std::string &variable) {
  return Binout_read_timed<int32_t>(*this, binout_read_timed_i32, variable);
}

template <>
std::vector<Array<int64_t>> Binout::read_timed(const std::string &variable) {
  return Binout_read_timed<int64_t>(*this, binout_read_timed_i64, variable);
}

template <>
std::vector<Array<uint8_t>> Binout::read_timed(const std::string &variable) {
  return Binout_read_timed<uint8_t>(*this, binout_read_timed_u8, variable);
}

template <>
std::vector<Array<uint16_t>> Binout::read_timed(const std::string &variable) {
  return Binout_read_timed<uint16_t>(*this, binout_read_timed_u16, variable);
}

template <>
std::vector<Array<uint32_t>> Binout::read_timed(const std::string &variable) {
  return Binout_read_timed<uint32_t>(*this, binout_read_timed_u32, variable);
}

template <>
std::vector<Array<uint64_t>> Binout::read_timed(const std::string &variable) {
  return Binout_read_timed<uint64_t>(*this, binout_read_timed_u64, variable);
}

template <>
std::vector<Array<float>> Binout::read_timed(const std::string &variable) {
  return Binout_read_timed<float>(*this, binout_read_timed_f32, variable);
}

template <>
std::vector<Array<double>> Binout::read_timed(const std::string &variable) {
  return Binout_read_timed<double>(*this, binout_read_timed_f64, variable);
}

std::string Binout::simple_path_to_real(const std::string &simple,
                                        BinoutType &type_id,
                                        bool &timed) const {
  uint8_t type_id_c;
  int timed_c;

  char *real_path = binout_simple_path_to_real(&m_handle, simple.c_str(),
                                               &type_id_c, &timed_c);
  if (!real_path) {
    char *msg = reinterpret_cast<char *>(malloc(256 + simple.length()));
    sprintf(msg, "The simple path \"%s\" can not be found", simple.c_str());
    throw Exception(Exception::ErrorString(msg));
  }

  type_id = static_cast<BinoutType>(type_id_c);
  timed = timed_c != 0;

  std::string str(real_path);
  free(real_path);

  return str;
}

} // namespace dro
