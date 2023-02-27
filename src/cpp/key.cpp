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

#include "key.hpp"
#include <cstdio>
#include <cstdlib>

namespace dro {
Card::Card(card_t *handle) noexcept : m_handle(handle) {}

void Card::begin(uint8_t value_width) noexcept {
  card_parse_begin(m_handle, value_width);
}

void Card::next() noexcept { card_parse_next(m_handle); }

void Card::next(uint8_t value_width) noexcept {
  card_parse_next_width(m_handle, value_width);
}

bool Card::done() const noexcept { return card_parse_done(m_handle) != 0; }

card_parse_type Card::parse_get_type() const noexcept {
  return card_parse_get_type(m_handle);
}

card_parse_type Card::parse_get_type(uint8_t value_width) const noexcept {
  return card_parse_get_type_width(m_handle, value_width);
}

template <>
char *Card::parse_string_no_trim<char *>(uint8_t value_width) const noexcept {
  return card_parse_string_width_no_trim(m_handle, value_width);
}

template <>
String Card::parse_string_no_trim<String>(uint8_t value_width) const noexcept {
  return String(card_parse_string_width_no_trim(m_handle, value_width));
}

template <>
std::string
Card::parse_string_no_trim<std::string>(uint8_t value_width) const noexcept {
  char *data = card_parse_string_width_no_trim(m_handle, value_width);
  std::string str(data);
  free(data);
  return str;
}

template <> char *Card::parse_string_whole<char *>() const noexcept {
  return card_parse_whole(m_handle);
}

template <> String Card::parse_string_whole<String>() const noexcept {
  return String(card_parse_whole(m_handle));
}

template <> std::string Card::parse_string_whole<std::string>() const noexcept {
  char *data = card_parse_whole(m_handle);
  std::string str(data);
  free(data);
  return str;
}

template <> char *Card::parse_string_whole_no_trim<char *>() const noexcept {
  return card_parse_whole_no_trim(m_handle);
}

template <> String Card::parse_string_whole_no_trim<String>() const noexcept {
  return String(card_parse_whole_no_trim(m_handle));
}

template <>
std::string Card::parse_string_whole_no_trim<std::string>() const noexcept {
  char *data = card_parse_whole_no_trim(m_handle);
  std::string str(data);
  free(data);
  return str;
}

Keyword::Keyword(keyword_t *handle) noexcept : m_handle(handle) {}

Card Keyword::operator[](size_t index) {
  if (m_handle->num_cards == 0 || index > m_handle->num_cards - 1) {
    throw std::runtime_error("Index out of Range");
  }

  return Card(&m_handle->cards[index]);
}

KeywordSlice::KeywordSlice(keyword_t *ptr, size_t size) noexcept
    : m_ptr(ptr), m_size(size) {}

Keyword KeywordSlice::operator[](size_t index) {
  if (m_size == 0 || index > m_size - 1) {
    throw std::runtime_error("Index out of Range");
  }

  return Keyword(&m_ptr[index]);
}

Keywords::Keywords(keyword_t *data, size_t size) noexcept
    : Array<keyword_t>(data, size, false) {}

Keywords::Keywords(Keywords &&rhs) noexcept {
  m_data = rhs.m_data;
  m_size = rhs.m_size;
  m_delete_data = rhs.m_delete_data;

  rhs.m_data = nullptr;
  rhs.m_size = 0;
  rhs.m_delete_data = false;
}

Keywords::~Keywords() noexcept { key_file_free(m_data, m_size); }

KeywordSlice Keywords::operator[](const std::string &name) {
  size_t slice_size;
  keyword_t *slice =
      key_file_get_slice(m_data, m_size, name.c_str(), &slice_size);
  if (!slice) {
    THROW_KEY_FILE_EXCEPTION("The keyword \"%s\" could not be found",
                             name.c_str());
  }

  return KeywordSlice(slice, slice_size);
}

KeyFile::Exception::Exception(String error_str) noexcept
    : m_error_str(std::move(error_str)) {}

const char *KeyFile::Exception::what() const noexcept {
  return m_error_str.data();
}

Keywords KeyFile::parse(const std::filesystem::path &file_name,
                        bool parse_includes) {
  size_t num_keywords;
  char *error_string;

  keyword_t *keywords =
      key_file_parse(file_name.string().c_str(), &num_keywords,
                     static_cast<int>(parse_includes), &error_string);
  if (error_string) {
    throw Exception(String(error_string));
  }

  return Keywords(keywords, num_keywords);
}

void KeyFile::parse_with_callback(const std::filesystem::path &file_name,
                                  KeyFile::Callback callback,
                                  bool parse_includes) {
  char *error_string;

  key_file_parse_with_callback(
      file_name.string().c_str(),
      [](const char *keyword_name, const card_t *card, size_t card_index,
         void *user_data) {
        KeyFile::Callback *callback =
            reinterpret_cast<KeyFile::Callback *>(user_data);

        (*callback)(String(const_cast<char *>(keyword_name), false),
                    Card(const_cast<card_t *>(card)), card_index);
      },
      static_cast<int>(parse_includes), &error_string, &callback, NULL, NULL);

  if (error_string) {
    throw Exception(String(error_string));
  }
}

} // namespace dro
