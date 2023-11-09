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
#include <cstring>

namespace dro {

template <> constexpr bool is_string_v<char *> = true;
template <> constexpr bool is_string_v<String> = true;
template <> constexpr bool is_string_v<SizedString> = true;
template <> constexpr bool is_string_v<std::string> = true;

Card::Card(Card &&rhs) noexcept { *this = std::move(rhs); }

Card::Card(const Card &rhs) noexcept { *this = rhs; }

Card::Card(card_t *handle) noexcept
    : m_handle(handle), m_delete_handle(false) {}

Card::~Card() noexcept {
  if (m_delete_handle) {
    free(m_handle->string);
    free(m_handle);
    m_delete_handle = false;
  }
}

Card &Card::operator=(Card &&rhs) noexcept {
  m_handle = rhs.m_handle;
  m_delete_handle = rhs.m_delete_handle;
  rhs.m_handle = nullptr;
  rhs.m_delete_handle = false;
  return *this;
}

Card &Card::operator=(const Card &rhs) noexcept {
  m_handle = reinterpret_cast<card_t *>(malloc(sizeof(card_t)));
  m_handle->string = strdup(rhs.m_handle->string);
  m_handle->value_width = rhs.m_handle->value_width;
  m_handle->current_index = rhs.m_handle->current_index;
  m_delete_handle = true;
  return *this;
}

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
SizedString
Card::parse_string_no_trim<SizedString>(uint8_t value_width) const noexcept {
  char *value = card_parse_string_width_no_trim(m_handle, value_width);
  return SizedString(value, strlen(value));
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

template <> SizedString Card::parse_string_whole<SizedString>() const noexcept {
  char *value = card_parse_whole(m_handle);
  return SizedString(value, strlen(value));
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
SizedString Card::parse_string_whole_no_trim<SizedString>() const noexcept {
  char *value = card_parse_whole_no_trim(m_handle);
  return SizedString(value, strlen(value));
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

KeyFile::Exception::Exception(
    KeyFile::Exception::ErrorString error_str) noexcept
    : m_error_str(std::move(error_str)) {}

const char *KeyFile::Exception::what() const noexcept {
  return m_error_str.data();
}

KeyFile::ParseConfig::ParseConfig(
    bool parse_includes, bool ignore_not_found_includes,
    std::vector<std::filesystem::path> extra_include_paths) noexcept {
  m_handle.parse_includes = parse_includes;
  m_handle.ignore_not_found_includes = ignore_not_found_includes;
  if (!extra_include_paths.empty()) {
    m_handle.num_extra_include_paths = extra_include_paths.size();
    m_handle.extra_include_paths = reinterpret_cast<char **>(
        malloc(m_handle.num_extra_include_paths * sizeof(char *)));
    for (size_t i = 0; i < extra_include_paths.size(); i++) {
      const auto str(extra_include_paths[i].string());
      m_handle.extra_include_paths[i] = strdup(str.c_str());
    }
  } else {
    m_handle.extra_include_paths = NULL;
    m_handle.num_extra_include_paths = 0;
  }
}

KeyFile::ParseConfig::ParseConfig(ParseConfig &&rhs) noexcept {
  m_handle = rhs.m_handle;
  rhs.m_handle = {0};
}

KeyFile::ParseConfig::ParseConfig(const ParseConfig &rhs) noexcept {
  m_handle = rhs.m_handle;
  if (m_handle.num_extra_include_paths != 0) {
    m_handle.extra_include_paths = reinterpret_cast<char **>(
        malloc(m_handle.num_extra_include_paths * sizeof(char *)));
    for (size_t i = 0; i < m_handle.num_extra_include_paths; i++) {
      m_handle.extra_include_paths[i] =
          strdup(rhs.m_handle.extra_include_paths[i]);
    }
  }
}

KeyFile::ParseConfig::~ParseConfig() noexcept {
  free(m_handle.extra_include_paths);
}

KeyFile::ParseInfo::ParseInfo(key_parse_info_t *handle) noexcept
    : m_handle(handle) {}

std::vector<std::filesystem::path> KeyFile::ParseInfo::include_paths() {
  std::vector<std::filesystem::path> p(m_handle->num_include_paths);
  for (size_t i = 0; i < m_handle->num_include_paths; i++) {
    p[i] = m_handle->include_paths[i];
  }
  return p;
}

Keywords KeyFile::parse(const std::filesystem::path &file_name,
                        KeyFile::ParseConfig parse_config,
                        std::optional<dro::String> *warnings) {
  size_t num_keywords;
  char *error_string, *warning_string;

  keyword_t *keywords =
      key_file_parse(file_name.string().c_str(), &num_keywords,
                     parse_config.get_handle(), &error_string, &warning_string);
  if (warning_string) {
    if (warnings == nullptr) {
      free(warning_string);
    } else {
      *warnings = dro::String(warning_string);
    }
  }
  if (error_string) {
    throw Exception(Exception::ErrorString(error_string));
  }

  return Keywords(keywords, num_keywords);
}

void KeyFile::parse_with_callback(const std::filesystem::path &file_name,
                                  KeyFile::Callback callback,
                                  KeyFile::ParseConfig parse_config,
                                  std::optional<dro::String> *warnings) {
  char *error_string, *warning_string;

  key_file_parse_with_callback(
      file_name.string().c_str(),
      [](key_parse_info_t info, const char *keyword_name, card_t *card,
         size_t card_index, void *user_data) {
        KeyFile::Callback *callback =
            reinterpret_cast<KeyFile::Callback *>(user_data);

        auto card_opt = card ? std::make_optional<Card>(card) : std::nullopt;

        (*callback)(ParseInfo(&info),
                    String(const_cast<char *>(keyword_name), false),
                    std::move(card_opt), card_index);
      },
      parse_config.get_handle(), &error_string, &warning_string, &callback,
      NULL);

  if (warning_string) {
    if (warnings == nullptr) {
      free(warning_string);
    } else {
      *warnings = dro::String(warning_string);
    }
  }
  if (error_string) {
    throw Exception(Exception::ErrorString(error_string));
  }
}

} // namespace dro
