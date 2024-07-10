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

#include "conversions.hpp"
#include <include_transform.hpp>
#include <key.hpp>
#include <pybind11/pybind11.h>
#include <sstream>

#ifndef __APPLE__
#include <pybind11/stl/filesystem.h>
#endif

namespace py = pybind11;

py::list python_card_parse_whole(dro::Card &self, py::list value_widths) {
  self.begin();

  py::list rv;
  int i = 0;

  for (int i = 0;
       (value_widths.empty() && !self.done()) || i < value_widths.size(); i++) {
    uint8_t value_width;
    if (value_widths.empty()) {
      value_width = DEFAULT_VALUE_WIDTH;
    } else {
      value_width = value_widths[i].cast<uint8_t>();
    }

    if (self.is_empty(value_width)) {
      rv.append(py::none());
    } else {
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
    }

    self.next(value_width);
  }

  return rv;
}

#ifdef __APPLE__
#define RDRT(val) read_return(std::move(val))
template <typename T> inline py::object read_return(T arr) noexcept {
  return py::cast(std::move(arr));
}

py::object
#else
#define RDRT(val) (val)

std::variant<int64_t, double, dro::String, py::none>
#endif
python_card_parse(dro::Card &self, bool trim_string, py::object value_width) {
  if (value_width.is_none()) {
    if (self.is_empty()) {
      return py::none();
    }

    const auto ty = self.parse_get_type();

    switch (ty) {
    case CARD_PARSE_INT:
      return RDRT(self.parse<int64_t>());
    case CARD_PARSE_FLOAT:
      return RDRT(self.parse<double>());
    case CARD_PARSE_STRING:
      if (trim_string) {
        return RDRT(self.parse<dro::String>());
      } else {
        return RDRT(self.parse_string_no_trim<dro::String>());
      }
    }
  } else {
    const auto vw = value_width.cast<uint8_t>();
    if (self.is_empty(vw)) {
      return py::none();
    }

    const auto ty = self.parse_get_type(vw);

    switch (ty) {
    case CARD_PARSE_INT:
      return RDRT(self.parse<int64_t>(vw));
    case CARD_PARSE_FLOAT:
      return RDRT(self.parse<double>(vw));
    case CARD_PARSE_STRING:
      if (trim_string) {
        return RDRT(self.parse<dro::String>(vw));
      } else {
        return RDRT(self.parse_string_no_trim<dro::String>(vw));
      }
    }
  }

  return py::none();
}

void add_key_library_to_module(py::module_ &m) {
  py::class_<dro::Keywords>(m, "Keywords")
      .def("__len__", [](dro::Keywords &self) { return self.size(); })
      .def("__getitem__", &dro::Keywords::operator[],
           py::return_value_policy::take_ownership)

      ;

  py::class_<dro::KeywordSlice>(m, "KeywordSlice")
      .def("__len__", &dro::KeywordSlice::size)
      .def("__getitem__", &dro::KeywordSlice::operator[],
           py::return_value_policy::take_ownership)

      ;

  py::class_<dro::Keyword>(m, "Keyword")
      .def("__len__", &dro::Keyword::num_cards)
      .def("__getitem__", &dro::Keyword::operator[],
           py::return_value_policy::take_ownership)

      ;

  py::class_<dro::Card>(m, "Card")
      .def("begin", &dro::Card::begin, "Initialises the parsing of the card",
           py::arg("value_width") = DEFAULT_VALUE_WIDTH)
      .def(
          "next",
          [](dro::Card &self, py::object value_width) {
            if (value_width.is_none()) {
              self.next();
            } else {
              self.next(value_width.cast<uint8_t>());
            }
          },
          "Advance to the next value. Uses the value width from begin",
          py::arg("value_width") = py::none())
      .def(
          "next_width",
          [](dro::Card &self, uint8_t value_width) { self.next(value_width); },
          "DEPRECATED(use Card.next) Advance to the next value. Uses the value "
          "width provided here",
          py::arg("value_width"))
      .def("done", &dro::Card::done,
           "Returns wether the card has been completely parsed. Breaks if "
           "incorrect value widths have been supplied")
      .def(
          "is_empty",
          [](dro::Card &self, py::object value_width) {
            if (value_width.is_none()) {
              return self.is_empty();
            }

            return self.is_empty(value_width.cast<uint8_t>());
          },
          "Returns wether the currently value that shall be parsed is empty "
          "(i.e. consists only of whitespace). Uses the value_width "
          "provided here. If the card is done this also returns True",
          py::arg("value_width") = py::none())
      .def(
          "parse_i64",
          [](const dro::Card &self, py::object value_width) {
            if (value_width.is_none()) {
              return self.parse<int64_t>();
            } else {
              return self.parse<int64_t>(value_width.cast<uint8_t>());
            }
          },
          "Parses the current value as an integer",
          py::arg("value_width") = py::none())
      .def(
          "parse_f64",
          [](const dro::Card &self, py::object value_width) {
            if (value_width.is_none()) {
              return self.parse<double>();
            } else {
              return self.parse<double>(value_width.cast<uint8_t>());
            }
          },
          "Parses the current value as a float",
          py::arg("value_width") = py::none())
      .def(
          "parse_str",
          [](const dro::Card &self, bool trim, py::object value_width) {
            if (value_width.is_none()) {
              if (trim) {
                return self.parse<dro::String>();
              } else {
                return self.parse_string_no_trim<dro::String>();
              }
            } else {
              if (trim) {
                return self.parse<dro::String>(value_width.cast<uint8_t>());
              } else {
                return self.parse_string_no_trim<dro::String>(
                    value_width.cast<uint8_t>());
              }
            }
          },
          "Parses the current value as a string. If trim is set to True then "
          "it trims leading and "
          "trailing whitespace",
          py::arg("trim") = true, py::arg("value_width") = py::none(),
          py::return_value_policy::take_ownership)
      .def(
          "parse_width_i64",
          [](const dro::Card &self, uint8_t value_width) {
            return self.parse<int64_t>(value_width);
          },
          "DEPRECATED(use Card.parse_i64) Parses the current value as an "
          "integer. Uses the value "
          "width provided here")
      .def(
          "parse_width_f64",
          [](const dro::Card &self, uint8_t value_width) {
            return self.parse<double>(value_width);
          },
          "DEPRECATED(use Card.parse_f64) Parses the current value as a float. "
          "Uses the value "
          "width provided here")
      .def(
          "parse_width_str",
          [](const dro::Card &self, uint8_t value_width, bool trim) {
            if (trim)
              return self.parse<dro::String>(value_width);
            else
              return self.parse_string_no_trim<dro::String>(value_width);
          },
          "DEPRECATED(use Card.parse_str) Parses the current value as a "
          "string. Uses the value "
          "width provided here. If trim is set to True then it trims leading "
          "and "
          "trailing whitespace",
          py::arg("value_width"), py::arg("trim") = true,
          py::return_value_policy::take_ownership)
      .def("parse", &python_card_parse,
           "Parses the current value as the recognized type (if it looks like "
           "an int its an int and so on). If the value is empty then None is "
           "returned",
           py::arg("trim_string") = true, py::arg("value_width") = py::none())
      .def("parse_whole", &python_card_parse_whole,
           "Parses all values of the card as the correct types. The width of "
           "each value needs to be provided as a list through value_widths",
           py::arg("value_widths") = py::list(),
           py::return_value_policy::take_ownership)
      .def("__str__", &dro::Card::parse_string_whole_no_trim<std::string>)

      ;

  m.def(
      "key_file_parse",
      [](const fs::path &file_name, bool output_warnings, bool parse_includes,
         bool ignore_not_found_includes,
         std::vector<fs::path> extra_include_paths) {
        std::optional<dro::String> warnings;
        auto keywords = dro::KeyFile::parse(
            file_name,
            dro::KeyFile::ParseConfig(parse_includes, ignore_not_found_includes,
                                      std::move(extra_include_paths)),
            &warnings);

        if (output_warnings && warnings) {
          std::cout << *warnings << std::endl;
        }

        return keywords;
      },
      "Parses an LS Dyna key file for keywords and their respective cards. "
      "Returns an array of keywords.\nparse_config: Configure how the file is "
      "parsed",
      py::arg("file_name"), py::arg("output_warnings") = true,
      py::arg("parse_includes") = true,
      py::arg("ignore_not_found_includes") = false,
      py::arg("extra_include_paths") = std::vector<fs::path>(),
      py::return_value_policy::take_ownership);

  dro::add_array_type_to_module<dro::TransformationOption>(m);
  dro::add_array_type_to_module<transformation_option_t>(m);

  py::class_<dro::IncludeTransform>(m, "IncludeTransform")
      .def(py::init<dro::Keyword &>(), py::arg("keyword"))
      .def(py::init<>())
      .def("parse_include_transform_card",
           &dro::IncludeTransform::parse_include_transform_card,
           py::arg("card"), py::arg("card_index"))
      .def("get_file_name", &dro::IncludeTransform::get_file_name,
           py::return_value_policy::take_ownership)
      .def("get_suffix", &dro::IncludeTransform::get_suffix,
           py::return_value_policy::take_ownership)
      .def("get_fcttem", &dro::IncludeTransform::get_fcttem,
           py::return_value_policy::take_ownership)
      .def("get_idnoff", &dro::IncludeTransform::get_idnoff)
      .def("get_ideoff", &dro::IncludeTransform::get_ideoff)
      .def("get_idpoff", &dro::IncludeTransform::get_idpoff)
      .def("get_idmoff", &dro::IncludeTransform::get_idmoff)
      .def("get_idsoff", &dro::IncludeTransform::get_idsoff)
      .def("get_idfoff", &dro::IncludeTransform::get_idfoff)
      .def("get_iddoff", &dro::IncludeTransform::get_iddoff)
      .def("get_idroff", &dro::IncludeTransform::get_idroff)
      .def("get_incout1", &dro::IncludeTransform::get_incout1)
      .def("get_tranid", &dro::IncludeTransform::get_tranid)
      .def("get_fctmas", &dro::IncludeTransform::get_fctmas)
      .def("get_fcttim", &dro::IncludeTransform::get_fcttim)
      .def("get_fctlen", &dro::IncludeTransform::get_fctlen)

      ;

  py::class_<dro::TransformationOption>(m, "TransformOption")
      .def("get_name", &dro::TransformationOption::get_name,
           py::return_value_policy::take_ownership)
      .def("get_parameters", &dro::TransformationOption::get_parameters,
           py::return_value_policy::take_ownership)

      ;

  py::class_<dro::DefineTransformation>(m, "DefineTransformation")
      .def(py::init<dro::Keyword &>(), py::arg("keyword"))
      .def(py::init())
      .def("parse_define_transformation_card",
           &dro::DefineTransformation::parse_define_transformation_card,
           py::arg("card"), py::arg("card_index"), py::arg("is_title") = true)
      .def("get_tranid", &dro::DefineTransformation::get_tranid)
      .def("get_options", &dro::DefineTransformation::get_options,
           py::return_value_policy::take_ownership)

      ;
}