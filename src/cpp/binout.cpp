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
    throw Exception(String(open_error, strlen(open_error) + 1));
  }
}

Binout::~Binout() noexcept { binout_close(&m_handle); }

BinoutType Binout::get_type_id(const std::string &path_to_variable) const {
  const BinoutType type_id{static_cast<BinoutType>(binout_get_type_id(
      const_cast<binout_file *>(&m_handle), path_to_variable.c_str()))};
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string,
                           strlen(m_handle.error_string) + 1, false));
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
    children_vec.emplace_back(children[i], strlen(children[i]) + 1);
  }

  free(children);
  return children_vec;
}

template <> Vector<int8_t> Binout::read(const std::string &path_to_variable) {
  size_t data_size;
  int8_t *data =
      binout_read_int8_t(&m_handle, path_to_variable.c_str(), &data_size);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string,
                           strlen(m_handle.error_string) + 1, false));
  }

  return Vector<int8_t>(data, data_size);
}

template <> Vector<int16_t> Binout::read(const std::string &path_to_variable) {
  size_t data_size;
  int16_t *data =
      binout_read_int16_t(&m_handle, path_to_variable.c_str(), &data_size);
  return Vector<int16_t>(data, data_size);
}

template <> Vector<int32_t> Binout::read(const std::string &path_to_variable) {
  size_t data_size;
  int32_t *data =
      binout_read_int32_t(&m_handle, path_to_variable.c_str(), &data_size);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string,
                           strlen(m_handle.error_string) + 1, false));
  }

  return Vector<int32_t>(data, data_size);
}

template <> Vector<int64_t> Binout::read(const std::string &path_to_variable) {
  size_t data_size;
  int64_t *data =
      binout_read_int64_t(&m_handle, path_to_variable.c_str(), &data_size);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string,
                           strlen(m_handle.error_string) + 1, false));
  }

  return Vector<int64_t>(data, data_size);
}

template <> Vector<uint8_t> Binout::read(const std::string &path_to_variable) {
  size_t data_size;
  uint8_t *data =
      binout_read_uint8_t(&m_handle, path_to_variable.c_str(), &data_size);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string,
                           strlen(m_handle.error_string) + 1, false));
  }

  return Vector<uint8_t>(data, data_size);
}

template <> Vector<uint16_t> Binout::read(const std::string &path_to_variable) {
  size_t data_size;
  uint16_t *data =
      binout_read_uint16_t(&m_handle, path_to_variable.c_str(), &data_size);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string,
                           strlen(m_handle.error_string) + 1, false));
  }

  return Vector<uint16_t>(data, data_size);
}

template <> Vector<uint32_t> Binout::read(const std::string &path_to_variable) {
  size_t data_size;
  uint32_t *data =
      binout_read_uint32_t(&m_handle, path_to_variable.c_str(), &data_size);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string,
                           strlen(m_handle.error_string) + 1, false));
  }

  return Vector<uint32_t>(data, data_size);
}

template <> Vector<uint64_t> Binout::read(const std::string &path_to_variable) {
  size_t data_size;
  uint64_t *data =
      binout_read_uint64_t(&m_handle, path_to_variable.c_str(), &data_size);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string,
                           strlen(m_handle.error_string) + 1, false));
  }

  return Vector<uint64_t>(data, data_size);
}

template <> Vector<float> Binout::read(const std::string &path_to_variable) {
  size_t data_size;
  float *data =
      binout_read_float(&m_handle, path_to_variable.c_str(), &data_size);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string,
                           strlen(m_handle.error_string) + 1, false));
  }

  return Vector<float>(data, data_size);
}

template <> Vector<double> Binout::read(const std::string &path_to_variable) {
  size_t data_size;
  double *data =
      binout_read_double(&m_handle, path_to_variable.c_str(), &data_size);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string,
                           strlen(m_handle.error_string) + 1, false));
  }

  return Vector<double>(data, data_size);
}

} // namespace dro