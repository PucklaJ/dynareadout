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
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <functional>
#include <key.h>
#include <limits>
#include <string>
#include <type_traits>
#include <typeinfo>

#define THROW_KEY_FILE_EXCEPTION(msg, ...)                                     \
  const int error_buffer_size = snprintf(NULL, 0, msg, __VA_ARGS__);           \
  char *error_buffer = reinterpret_cast<char *>(malloc(error_buffer_size));    \
  sprintf(error_buffer, msg, __VA_ARGS__);                                     \
  throw KeyFile::Exception(String(error_buffer));

namespace dro {
class Card {
public:
  Card(card_t *handle) noexcept;

  void begin(uint8_t value_width = DEFAULT_VALUE_WIDTH) noexcept;
  void next(uint8_t value_width = DEFAULT_VALUE_WIDTH) noexcept;
  bool done() const noexcept;

  template <typename T>
  T parse(uint8_t value_width = DEFAULT_VALUE_WIDTH) const;

  template <typename T>
  T parse_string_no_trim(
      uint8_t value_width = DEFAULT_VALUE_WIDTH) const noexcept;
  template <typename T> T parse_whole() const noexcept;
  template <typename T> T parse_whole_no_trim() const noexcept;

private:
  card_t *m_handle;
};

class Keyword {
public:
  class CardsIterator {
  public:
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;

    explicit CardsIterator(card_t *data, difference_type index) noexcept
        : m_data(data), m_index(index) {}
    CardsIterator operator++() noexcept {
      m_index++;
      return *this;
    }
    CardsIterator operator++(int) noexcept {
      auto rv = *this;
      ++(*this);
      return rv;
    }
    bool operator==(const CardsIterator &rhs) const noexcept {
      return m_index == rhs.m_index;
    }
    bool operator!=(const CardsIterator &rhs) const noexcept {
      return m_index != rhs.m_index;
    }
    difference_type operator-(const CardsIterator &rhs) const noexcept {
      return m_index - rhs.m_index;
    }
    Card operator->() noexcept { return Card(&m_data[m_index]); }

  public:
    card_t *m_data;
    difference_type m_index;
  };

  Keyword(keyword_t *handle) noexcept;

  Card operator[](size_t index);

  CardsIterator begin() noexcept { return CardsIterator(m_handle->cards, 0); }
  CardsIterator end() noexcept {
    return CardsIterator(m_handle->cards, m_handle->num_cards);
  }

private:
  keyword_t *m_handle;
};

class KeywordSlice {
public:
  KeywordSlice(keyword_t *ptr, size_t size) noexcept;

  inline size_t size() const noexcept { return m_size; }

  Keyword operator[](size_t index);

private:
  keyword_t *m_ptr;
  size_t m_size;
};

class Keywords : public Array<keyword_t> {
public:
  Keywords(keyword_t *data, size_t size) noexcept;
  ~Keywords() noexcept override;

  KeywordSlice operator[](const std::string &name);
};

class KeyFile {
public:
  class Exception : public std::exception {
  public:
    Exception(String error_str) noexcept;

    const char *what() const noexcept override;

  private:
    const String m_error_str;
  };

  using Callback =
      std::function<void(String keyword_name, Card card, size_t card_index)>;

  static Keywords parse(const std::filesystem::path &file_name,
                        bool parse_includes = true);
  static void parse_with_callback(const std::filesystem::path &file_name,
                                  Callback callback,
                                  bool parse_includes = true);
};

template <typename T> T Card::parse(uint8_t value_width) const {
  /* TODO: Bounds check*/
  if constexpr (std::is_integral_v<T>) {
    const auto value = card_parse_int_width(m_handle, value_width);
    if (value < 0 && std::is_unsigned_v<T>) {
      THROW_KEY_FILE_EXCEPTION("Can not convert %ld into %s because of sign",
                               value, typeid(T).name());
    }
    return value;
  } else if constexpr (std::is_floating_point_v<T>) {
    if constexpr (sizeof(T) == sizeof(float)) {
      const auto value = card_parse_float32_width(m_handle, value_width);
      if (value < 0.0f && std::is_unsigned_v<T>) {
        THROW_KEY_FILE_EXCEPTION("Can not convert %f into %s because of sign",
                                 value, typeid(T).name());
      }
      return value;
    } else {
      const auto value = card_parse_float64_width(m_handle, value_width);
      if (value < 0.0 && std::is_unsigned_v<T>) {
        THROW_KEY_FILE_EXCEPTION("Can not convert %f into %s because of sign",
                                 value, typeid(T).name());
      }
      return value;
    }
  } else if constexpr (std::is_same_v<T, char *>) {
    char *value = card_parse_string(m_handle);
    return value;
  } else if constexpr (std::is_same_v<T, String>) {
    char *value = card_parse_string(m_handle);
    return String(value);
  } else if constexpr (std::is_same_v<T, std::string>) {
    char *value = card_parse_string(m_handle);
    std::string str(value);
    free(value);
    return str;
  } else {
    static_assert(false && "Can not convert a card to the given type");
  }
}

} // namespace dro