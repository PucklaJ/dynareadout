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
#include <sstream>

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
  py::class_<dro::KeyFile::ParseConfig>(m, "key_file_parse_config")
      .def(py::init<bool, bool>(), py::arg("parse_includes") = true,
           py::arg("ignore_not_found_includes") = false,
           "Configure how a key file is parsed");

  py::class_<dro::Keywords>(m, "Keywords")
      .def("__len__", [](dro::Keywords &self) { return self.size(); })
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
      .def("begin", &dro::Card::begin, "Initialises the parsing of the card",
           py::arg("value_width") = DEFAULT_VALUE_WIDTH)
      .def(
          "next", [](dro::Card &self) { self.next(); },
          "Advance to the next value. Uses the value width from begin")
      .def(
          "next_width",
          [](dro::Card &self, uint8_t value_width) { self.next(value_width); },
          "Advance to the next value. Uses the value width provided here")
      .def("done", &dro::Card::done,
           "Returns wether the card has been completely parsed. Breaks if "
           "incorrect value widths have been supplied")
      .def(
          "parse_i64",
          [](const dro::Card &self) { return self.parse<int64_t>(); },
          "Parses the current value as an integer. Uses the value "
          "width from begin")
      .def(
          "parse_f64",
          [](const dro::Card &self) { return self.parse<double>(); },
          "Parses the current value as a float. Uses the value "
          "width from begin")
      .def(
          "parse_str",
          [](const dro::Card &self, bool trim) {
            if (trim)
              return self.parse<dro::String>();
            else
              return self.parse_string_no_trim<dro::String>();
          },
          "Parses the current value as a string. Uses the value "
          "width from begin. If trim is set to True then it trims leading and "
          "trailing whitespace",
          py::arg("trim") = true)
      .def(
          "parse_width_i64",
          [](const dro::Card &self, uint8_t value_width) {
            return self.parse<int64_t>(value_width);
          },
          "Parses the current value as an integer. Uses the value "
          "width provided here")
      .def(
          "parse_width_f64",
          [](const dro::Card &self, uint8_t value_width) {
            return self.parse<double>(value_width);
          },
          "Parses the current value as a float. Uses the value "
          "width provided here")
      .def(
          "parse_width_str",
          [](const dro::Card &self, uint8_t value_width, bool trim) {
            if (trim)
              return self.parse<dro::String>(value_width);
            else
              return self.parse_string_no_trim<dro::String>(value_width);
          },
          "Parses the current value as a string. Uses the value "
          "width provided here. If trim is set to True then it trims leading "
          "and "
          "trailing whitespace",
          py::arg("value_width"), py::arg("trim") = true)
      .def("parse_whole", &python_card_parse_whole,
           "Parses all values of the card as the correct types. The width of "
           "each value needs to be provided as a list through value_widths",
           py::arg("value_widths") = py::list())
      .def("__str__", &dro::Card::parse_string_whole_no_trim<dro::String>)

      ;

  m.def(
      "key_file_parse",
      [](const std::filesystem::path &file_name, bool output_warnings,
         dro::KeyFile::ParseConfig parse_config) {
        std::optional<dro::String> warnings;
        auto keywords =
            dro::KeyFile::parse(file_name, warnings, std::move(parse_config));

        if (output_warnings && warnings) {
          std::cout << warnings->data() << std::endl;
        }

        return keywords;
      },
      "Parses an LS Dyna key file for keywords and their respective cards. "
      "Returns an array of keywords.\nparse_config: Configure how the file is "
      "parsed",
      py::arg("file_name"), py::arg("output_warnings") = true,
      py::arg("parse_config") = dro::KeyFile::ParseConfig());
}