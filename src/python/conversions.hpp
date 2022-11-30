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
#include <d3_defines.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <sstream>
#include <type_traits>
#include <vec.hpp>
#include <vector>

namespace py = pybind11;

namespace dro {

template <typename T>
inline std::vector<T> array_vector_wrapper(Array<T> &&arr) noexcept {
  std::vector<T> vec;
  vec.resize(arr.size());
  memcpy(vec.data(), arr.data(), arr.size() * sizeof(T));
  return vec;
}

template <typename T> py::list array_to_python_list(Array<T> &&arr) {
  py::list l;

  for (const auto &v : arr) {
    l.append(v);
  }

  return l;
}

template <typename T> py::str array_to_python_string(Array<T> &&arr) {
  return py::str(arr.str());
}

template <typename T> T &array_getitem(Array<T> &self, size_t index) {
  try {
    return self[index];
  } catch (const std::runtime_error &) {
    throw py::index_error("Index out of range");
  }
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
               .def("__len__", &Array<T>::size)
               .def("__getitem__", &array_getitem<T>)

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
      .def("__len__", &String::size)
      .def("__getitem__", &array_getitem<char>)
      .def("__str__", [](String &arr) { return arr.str(); })
      .def("__repr__", [](String &arr) { return "'" + arr.str() + "'"; })

      ;
}

} // namespace dro