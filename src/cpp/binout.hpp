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

#pragma once
#include "array.hpp"
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

// This class is used to open and read data from a binout file (or multiple
// files by globbing)
class Binout {
public:
  class Exception : public std::exception {
  public:
#ifdef _WIN32
    // On windows python scripts crash when deallocated the memory of a thrown
    // exception
    class ErrorString : public std::string {
    public:
      ErrorString(char *str, bool delete_data = true) noexcept
          : std::string(str) {
        if (delete_data)
          free(str);
      }
    };
#else
    using ErrorString = String;
#endif

    Exception(ErrorString error_str) noexcept;

    const char *what() const noexcept override;

  private:
    const ErrorString m_error_str;
  };

  Binout(Binout &&rhs) noexcept;
  // Open a binout file (or multiple files by globbing) and parse its records to
  // be ready to read data
  Binout(const std::filesystem::path &file_name);
  ~Binout() noexcept;

  Binout &operator=(Binout &&rhs) noexcept;

  // Read data from the file. The type id of the data has to match T
  template <typename T> Array<T> read(const std::string &path_to_variable);
  template <typename T>
  std::vector<Array<T>> read_timed(const std::string &variable);
  // Returns the type id of the given variable. The type id is one of BinoutType
  BinoutType get_type_id(const std::string &path_to_variable) const;
  // Returns whether a record with the given path and variable name exists
  bool variable_exists(const std::string &path_to_variable) const noexcept;
  // Returns the entries under a given path
  std::vector<String> get_children(const std::string &path = "/") const;
  // Returns the number of dxxxxxx folders inside of a given path
  size_t get_num_timesteps(const std::string &path) const;

  binout_file &get_handle() noexcept { return m_handle; }
  const binout_file &get_handle() const noexcept { return m_handle; }

  std::string simple_path_to_real(const std::string &simple,
                                  BinoutType &type_id, bool &timed) const;

private:
  // The underlying C handle of the binout file
  binout_file m_handle;
};

} // namespace dro