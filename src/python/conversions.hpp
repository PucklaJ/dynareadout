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

#include <array.hpp>
#include <cstring>
#include <d3_defines.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <sstream>
#include <type_traits>
#include <vec.hpp>
#include <vector>

namespace py = pybind11;

namespace dro {

template <typename T> Array<T> array_constructor(size_t size) {
  T *data = reinterpret_cast<T *>(malloc(size * sizeof(T)));
  return Array<T>(data, size);
}

template <typename T>
void array_setitem(Array<T> &self, size_t index, py::object other) {
  if (py::isinstance<py::str>(other)) {
    const py::str other_str(other);
    if (py::len(other_str) == 1) {
      const py::bytes other_bytes(other_str);
      if constexpr (std::is_same_v<T, char>) {
        const auto temp{other_bytes[py::int_(0)].cast<int>()};
        self[index] = static_cast<char>(temp);
      } else {
        self[index] = other_bytes[py::int_(0)].cast<T>();
      }
    } else {
      throw py::value_error("Unable to set Array value to string");
    }
  } else {
    self[index] = other.cast<T>();
  }
}

template <typename T> T &array_getitem(Array<T> &self, size_t index) {
  try {
    return self[index];
  } catch (const std::runtime_error &) {
    throw py::index_error("Index out of range");
  }
}

template <typename T>
bool array_equals(const Array<T> &self, const py::object &other) {
  if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t> ||
                std::is_same_v<T, char>) {
    if (py::isinstance<py::str>(other)) {
      const py::str other_str(other);
      const py::bytes other_bytes(other_str);
      // - 1 because the String is null-terminated and the null character does
      // count too
      if ((self.size() - 1) != py::len(other_bytes)) {
        return false;
      }

      for (size_t i = 0; i < self.size() - 1; i++) {
        const int other_byte = other_bytes[py::int_(i)].cast<int>();

        if (self[i] != static_cast<T>(other_byte)) {
          return false;
        }
      }

      return true;
    }
  } else {
    if (py::isinstance<py::str>(other)) {
      return false;
    }
  }

  if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
    if (self.size() != py::len(other)) {
      return false;
    }

    for (size_t i = 0; i < self.size(); i++) {
      if (self[i] != other[py::int_(i)].cast<T>()) {
        return false;
      }
    }

    return true;
  }

  return false;
}

template <typename T>
bool array_less_than(const Array<T> &self, const Array<T> &other) {
  if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t> ||
                std::is_same_v<T, char>) {
    if (self.size() <= other.size()) {
      return strncmp(reinterpret_cast<const char *>(self.data()),
                     reinterpret_cast<const char *>(other.data()),
                     self.size()) < 0;
    } else {
      return strncmp(reinterpret_cast<const char *>(other.data()),
                     reinterpret_cast<const char *>(self.data()),
                     other.size()) > 0;
    }
  }

  throw py::type_error("This array can not be compared");
}

template <typename T>
bool array_greater_than(const Array<T> &self, const Array<T> &other) {
  if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t> ||
                std::is_same_v<T, char>) {
    if (self.size() <= other.size()) {
      return strncmp(reinterpret_cast<const char *>(self.data()),
                     reinterpret_cast<const char *>(other.data()),
                     self.size()) > 0;
    } else {
      return strncmp(reinterpret_cast<const char *>(other.data()),
                     reinterpret_cast<const char *>(self.data()),
                     other.size()) < 0;
    }
  }

  throw py::type_error("This array can not be compared");
}

template <typename T> inline const char *get_array_name() {
  if constexpr (std::is_same_v<T, int8_t>) {
    return "Int8Array";
  } else if constexpr (std::is_same_v<T, uint8_t>) {
    return "Uint8Array";
  } else if constexpr (std::is_same_v<T, int16_t>) {
    return "Int16Array";
  } else if constexpr (std::is_same_v<T, uint16_t>) {
    return "Uint16Array";
  } else if constexpr (std::is_same_v<T, int32_t>) {
    return "Int32Array";
  } else if constexpr (std::is_same_v<T, uint32_t>) {
    return "Uint32Array";
  } else if constexpr (std::is_same_v<T, int64_t>) {
    return "Int64Array";
  } else if constexpr (std::is_same_v<T, uint64_t>) {
    return "Uint64Array";
  } else if constexpr (std::is_same_v<T, float>) {
    return "FloatArray";
  } else if constexpr (std::is_same_v<T, double>) {
    return "DoubleArray";
  } else if constexpr (std::is_same_v<T, dVec3>) {
    return "Vec3Array";
  } else if constexpr (std::is_same_v<T, d3plot_solid_con>) {
    return "SolidConArray";
  } else if constexpr (std::is_same_v<T, d3plot_beam_con>) {
    return "BeamConArray";
  } else if constexpr (std::is_same_v<T, d3plot_shell_con>) {
    return "ShellConArray";
  } else if constexpr (std::is_same_v<T, d3plot_thick_shell_con>) {
    return "ThickShellConArray";
  } else if constexpr (std::is_same_v<T, d3plot_solid>) {
    return "SolidArray";
  } else if constexpr (std::is_same_v<T, d3plot_thick_shell>) {
    return "ThickShellArray";
  } else if constexpr (std::is_same_v<T, d3plot_beam>) {
    return "BeamArray";
  } else if constexpr (std::is_same_v<T, d3plot_shell>) {
    return "ShellArray";
  }
}

template <typename T> inline void add_array_type_to_module(py::module_ &m) {
  auto arr(py::class_<Array<T>>(m, get_array_name<T>())
               .def(py::init(&array_constructor<T>))
               .def("__len__", &Array<T>::size)
               .def("__setitem__", &array_setitem<T>)
               .def("__getitem__", &array_getitem<T>)
               .def("__eq__", &array_equals<T>)
               .def("__lt__", &array_less_than<T>)
               .def("__gt__", &array_greater_than<T>)

  );

  if constexpr (std::is_same_v<T, uint8_t> || std::is_same_v<T, int8_t>) {
    arr.def("__str__", &Array<T>::str);
    arr.def("__repr__", [](Array<T> &arr) { return "'" + arr.str() + "'"; });
  } else if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
    arr.def("__repr__", [](Array<T> &arr) {
      std::stringstream str;

      str << '[';

      for (size_t i = 0; i < arr.size(); i++) {
        str << arr[i];
        if (i != arr.size() - 1)
          str << ", ";
      }

      str << ']';

      return str.str();
    });
  }
}

inline void add_array_to_module(py::module_ &m) {
  add_array_type_to_module<int8_t>(m);
  add_array_type_to_module<uint8_t>(m);
  add_array_type_to_module<int16_t>(m);
  add_array_type_to_module<uint16_t>(m);
  add_array_type_to_module<int32_t>(m);
  add_array_type_to_module<uint32_t>(m);
  add_array_type_to_module<int64_t>(m);
  add_array_type_to_module<uint64_t>(m);
  add_array_type_to_module<float>(m);
  add_array_type_to_module<double>(m);

  py::class_<String>(m, "String")
      .def(py::init([](size_t size) {
        char *data = reinterpret_cast<char *>(malloc(size * sizeof(char)));
        return String(data, size, true);
      }))
      .def("__len__", &String::size)
      .def("__setitem__",
           [](String &self, size_t index, py::object other) {
             Array<char> arr(self.data(), self.size(), false);
             array_setitem(arr, index, std::move(other));
           })
      .def("__getitem__",
           [](String &self, size_t index) {
             Array<char> arr(self.data(), self.size(), false);
             return array_getitem(arr, index);
           })
      .def("__eq__",
           [](String &self, const py::object &other) {
             Array<char> arr(self.data(), self.size(), false);
             return array_equals(arr, other);
           })
      .def("__lt__",
           [](String &self, String &other) {
             Array<char> arr_self(self.data(), self.size(), false);
             Array<char> arr_other(other.data(), other.size(), false);
             return array_less_than(arr_self, arr_other);
           })
      .def("__gt__",
           [](String &self, String &other) {
             Array<char> arr_self(self.data(), self.size(), false);
             Array<char> arr_other(other.data(), other.size(), false);
             return array_greater_than(arr_self, arr_other);
           })
      .def("__str__", [](String &arr) { return arr.str(); })
      .def("__repr__", [](String &arr) { return "'" + arr.str() + "'"; })

      ;
}

} // namespace dro