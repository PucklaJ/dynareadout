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

#include <key.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/stl/filesystem.h>

namespace py = pybind11;

py::list python_card_parse_whole(dro::Card &self, py::list value_widths) {
  self.begin();

  py::list rv;
  int i = 0;

  while ((value_widths.empty() && !self.done()) ||
         (!value_widths.empty() && i < value_widths.size())) {
    uint8_t value_width;
    if (value_widths.empty()) {
      value_width = DEFAULT_VALUE_WIDTH;
    } else {
      if (self.done()) {
        dro::String card_string =
            self.parse_string_whole_no_trim<dro::String>();

        THROW_KEY_FILE_EXCEPTION(
            "Trying to parse %d values out of card \"%s\" with", i + 1,
            card_string.data());
      }

      value_width = value_widths[i].cast<uint8_t>();
    }

    switch (self.parse_get_type(value_width)) {
    case CARD_PARSE_INT:
      rv.append(self.parse<int64_t>(value_width));
      break;
    case CARD_PARSE_FLOAT:
      rv.append(self.parse<double>(value_width));
      break;
    case CARD_PARSE_STRING:
      rv.append(self.parse<dro::String>(value_width));
      break;
    }

    self.next(value_width);
    i++;
  }

  return rv;
}

void add_key_library_to_module(py::module_ &m) {
  m.def("key_file_parse", &dro::KeyFile::parse, py::arg("file_name"),
        py::arg("parse_includes") = true);

  py::class_<dro::Keywords>(m, "Keywords")
      .def("__len__", &dro::Keywords::size)
      .def("__getitem__", &dro::Keywords::operator[])

      ;

  py::class_<dro::KeywordSlice>(m, "KeywordSlice")
      .def("__len__", &dro::KeywordSlice::size)
      .def("__getitem__", &dro::KeywordSlice::operator[])

      ;

  py::class_<dro::Keyword>(m, "Keyword")
      .def("__len__", &dro::Keyword::num_cards)
      .def("__getitem__", &dro::Keyword::operator[])

      ;

  py::class_<dro::Card>(m, "Card")
      .def("begin", &dro::Card::begin,
           py::arg("value_width") = DEFAULT_VALUE_WIDTH)
      .def("next", [](dro::Card &self) { self.next(); })
      .def("next_width",
           [](dro::Card &self, uint8_t value_width) { self.next(value_width); })
      .def("done", &dro::Card::done)
      .def("parse_i64",
           [](const dro::Card &self) { return self.parse<int64_t>(); })
      .def("parse_f64",
           [](const dro::Card &self) { return self.parse<double>(); })
      .def(
          "parse_str",
          [](const dro::Card &self, bool trim) {
            if (trim)
              return self.parse<dro::String>();
            else
              return self.parse_string_no_trim<dro::String>();
          },
          py::arg("trim") = true)
      .def("parse_width_i64",
           [](const dro::Card &self, uint8_t value_width) {
             return self.parse<int64_t>(value_width);
           })
      .def("parse_width_f64",
           [](const dro::Card &self, uint8_t value_width) {
             return self.parse<double>(value_width);
           })
      .def(
          "parse_width_str",
          [](const dro::Card &self, uint8_t value_width, bool trim) {
            if (trim)
              return self.parse<dro::String>(value_width);
            else
              return self.parse_string_no_trim<dro::String>(value_width);
          },
          py::arg("value_width"), py::arg("trim") = true)
      .def("parse_whole", &python_card_parse_whole,
           py::arg("value_widths") = py::list())

      ;
}