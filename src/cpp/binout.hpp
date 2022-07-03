#pragma once
#include "vector.hpp"
#include <binout.h>
#include <binout_defines.h>
#include <exception>
#include <filesystem>
#include <vector>

namespace dro {
enum BinoutType {
  Int8 = BINOUT_TYPE_INT8,
  Int16 = BINOUT_TYPE_INT16,
  Int32 = BINOUT_TYPE_INT32,
  Int64 = BINOUT_TYPE_INT64,
  Uint8 = BINOUT_TYPE_UINT8,
  Uint16 = BINOUT_TYPE_UINT16,
  Uint32 = BINOUT_TYPE_UINT32,
  Uint64 = BINOUT_TYPE_UINT64,
  Float32 = BINOUT_TYPE_FLOAT32,
  Float64 = BINOUT_TYPE_FLOAT64,
  Invalid = BINOUT_TYPE_INVALID
};

class Binout {
public:
  class Exception : public std::exception {
  public:
    Exception(String error_str) noexcept;

    const char *what() const noexcept override;

  private:
    String m_error_str;
  };

  Binout(const std::filesystem::path &file_name);
  ~Binout() noexcept;

  template <typename T> Vector<T> read(const std::string &path_to_variable);
  BinoutType get_type_id(const std::string &path_to_variable) const;
  bool variable_exists(const std::string &path_to_variable) const noexcept;
  std::vector<String> get_children(const std::string &path) const noexcept;

  binout_file &get_handle() noexcept { return m_handle; }
  const binout_file &get_handle() const noexcept { return m_handle; }

private:
  binout_file m_handle;
};

} // namespace dro