#include "binout.hpp"
#include <cstring>

namespace dro {

Binout::Binout(const std::filesystem::path &file_name) {
  m_handle = binout_open(file_name.c_str());
  if (m_handle.error_string) {
    throw std::runtime_error(m_handle.error_string);
  }
}

Binout::~Binout() { binout_close(&m_handle); }

BinoutType Binout::get_type_id(const std::string &path,
                               const std::string &variable) const {
  return static_cast<BinoutType>(binout_get_type_id(
      const_cast<binout_file *>(&m_handle), path.c_str(), variable.c_str()));
}

bool Binout::variable_exists(const std::string &path,
                             const std::string &variable) const {
  return binout_variable_exists(const_cast<binout_file *>(&m_handle),
                                path.c_str(), variable.c_str());
}

std::vector<String> Binout::get_children(const std::string &path) const {
  size_t num_children;
  char **children = binout_get_children(const_cast<binout_file *>(&m_handle),
                                        path.c_str(), &num_children);

  std::vector<String> children_vec;
  for (size_t i = 0; i < num_children; i++) {
    children_vec.emplace_back(children[i], strlen(children[i]) + 1);
  }

  free(children);
  return children_vec;
}

template <>
Vector<int8_t> Binout::read(const std::string &path,
                            const std::string &variable) {
  size_t data_size;
  int8_t *data =
      binout_read_int8_t(&m_handle, path.c_str(), variable.c_str(), &data_size);
  if (m_handle.error_string) {
    throw std::runtime_error(m_handle.error_string);
  }
  return Vector<int8_t>(data, data_size);
}

template <>
Vector<int16_t> Binout::read(const std::string &path,
                             const std::string &variable) {
  size_t data_size;
  int16_t *data = binout_read_int16_t(&m_handle, path.c_str(), variable.c_str(),
                                      &data_size);
  if (m_handle.error_string) {
    throw std::runtime_error(m_handle.error_string);
  }
  return Vector<int16_t>(data, data_size);
}

template <>
Vector<int32_t> Binout::read(const std::string &path,
                             const std::string &variable) {
  size_t data_size;
  int32_t *data = binout_read_int32_t(&m_handle, path.c_str(), variable.c_str(),
                                      &data_size);
  if (m_handle.error_string) {
    throw std::runtime_error(m_handle.error_string);
  }
  return Vector<int32_t>(data, data_size);
}

template <>
Vector<int64_t> Binout::read(const std::string &path,
                             const std::string &variable) {
  size_t data_size;
  int64_t *data = binout_read_int64_t(&m_handle, path.c_str(), variable.c_str(),
                                      &data_size);
  if (m_handle.error_string) {
    throw std::runtime_error(m_handle.error_string);
  }
  return Vector<int64_t>(data, data_size);
}

template <>
Vector<uint8_t> Binout::read(const std::string &path,
                             const std::string &variable) {
  size_t data_size;
  uint8_t *data = binout_read_uint8_t(&m_handle, path.c_str(), variable.c_str(),
                                      &data_size);
  if (m_handle.error_string) {
    throw std::runtime_error(m_handle.error_string);
  }
  return Vector<uint8_t>(data, data_size);
}

template <>
Vector<uint16_t> Binout::read(const std::string &path,
                              const std::string &variable) {
  size_t data_size;
  uint16_t *data = binout_read_uint16_t(&m_handle, path.c_str(),
                                        variable.c_str(), &data_size);
  if (m_handle.error_string) {
    throw std::runtime_error(m_handle.error_string);
  }
  return Vector<uint16_t>(data, data_size);
}

template <>
Vector<uint32_t> Binout::read(const std::string &path,
                              const std::string &variable) {
  size_t data_size;
  uint32_t *data = binout_read_uint32_t(&m_handle, path.c_str(),
                                        variable.c_str(), &data_size);
  if (m_handle.error_string) {
    throw std::runtime_error(m_handle.error_string);
  }
  return Vector<uint32_t>(data, data_size);
}

template <>
Vector<uint64_t> Binout::read(const std::string &path,
                              const std::string &variable) {
  size_t data_size;
  uint64_t *data = binout_read_uint64_t(&m_handle, path.c_str(),
                                        variable.c_str(), &data_size);
  if (m_handle.error_string) {
    throw std::runtime_error(m_handle.error_string);
  }
  return Vector<uint64_t>(data, data_size);
}

template <>
Vector<float> Binout::read(const std::string &path,
                           const std::string &variable) {
  size_t data_size;
  float *data =
      binout_read_float(&m_handle, path.c_str(), variable.c_str(), &data_size);
  if (m_handle.error_string) {
    throw std::runtime_error(m_handle.error_string);
  }
  return Vector<float>(data, data_size);
}

template <>
Vector<double> Binout::read(const std::string &path,
                            const std::string &variable) {
  size_t data_size;
  double *data =
      binout_read_double(&m_handle, path.c_str(), variable.c_str(), &data_size);
  if (m_handle.error_string) {
    throw std::runtime_error(m_handle.error_string);
  }
  return Vector<double>(data, data_size);
}

} // namespace dro