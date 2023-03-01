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
#include <array>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <functional>
#include <key.h>
#include <limits>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeinfo>

#define THROW_KEY_FILE_EXCEPTION(msg, ...)                                     \
  const int error_buffer_size = snprintf(NULL, 0, msg, __VA_ARGS__);           \
  char *error_buffer = reinterpret_cast<char *>(malloc(error_buffer_size));    \
  sprintf(error_buffer, msg, __VA_ARGS__);                                     \
  throw dro::KeyFile::Exception(dro::String(error_buffer))

namespace dro {
// A Card inside of a LS Dyna key file (input deck)
class Card {
public:
  Card(card_t *handle) noexcept;

  // Initialises the parsing of the card
  void begin(uint8_t value_width = DEFAULT_VALUE_WIDTH) noexcept;
  // Advance to the next value. Uses the value width from begin.
  void next() noexcept;
  // Advance to the next value. Uses the value width provided here.
  void next(uint8_t value_width) noexcept;
  // Returns wether the card has been completely parsed. Breaks if incorrect
  // value widths have been supplied
  bool done() const noexcept;
  // Returns the type of the current value. Uses the value width from
  // begin. If CARD_PARSE_INT is returned the value can be parsed with an
  // integer type. If CARD_PARSE_FLOAT is returned the value can be parsed with
  // a floating point type. Else the value can only be parsed with a string
  // type.
  card_parse_type parse_get_type() const noexcept;
  // Returns the type of the current value. Uses the value width provided here.
  // If CARD_PARSE_INT is returned the value can be parsed with an integer type.
  // If CARD_PARSE_FLOAT is returned the value can be parsed with a floating
  // point type. Else the value can only be parsed with a string type.
  card_parse_type parse_get_type(uint8_t value_width) const noexcept;

  // Parses the current value as the given type. This will throw a
  // dro::KeyFile::Exception if it can not be parsed. Uses the value width from
  // begin. If a string type is used leading and trailing spaces are trimmed.
  template <typename T> T parse() const;
  // Parses the current value as the given type. This will throw a
  // dro::KeyFile::Exception if it can not be parsed. Uses the value width
  // provided here. If a string type is used leading and trailing spaces are
  // trimmed.
  template <typename T> T parse(uint8_t value_width) const;

  // Parses the current value as a string. Uses the value width
  // from begin. No spaces are trimmed.
  template <typename T> T parse_string_no_trim() const noexcept;
  // Parses the current value as a string. Uses the value width
  // provided here. No spaces are trimmed.
  template <typename T>
  T parse_string_no_trim(uint8_t value_width) const noexcept;
  // Returns the whole card as a string. Trims trailing and leading spaces.
  template <typename T> T parse_string_whole() const noexcept;
  // Returns the whole card as a string. Performs no trimming.
  template <typename T> T parse_string_whole_no_trim() const noexcept;

  // Parses all values of a card as the given types. Uses DEFAULT_VALUE_WIDTH as
  // the value width.
  template <typename... T> void parse_whole(T &...rv);
  // Parses all values of a card as the given types. Uses the values of the
  // value_widths array as the value width for every value.
  // Example NODE: card.parse_whole_width({8, 16, 16, 16, 8, 8}, nid, x, y, z,
  // tc, rc);
  template <typename... T>
  void parse_whole_width(std::array<uint8_t, sizeof...(T)> value_widths,
                         T &...rv);

  // Parses all values of a card as the given types and returns them in a tuple.
  // Uses DEFAULT_VALUE_WIDTH as the value width.
  // Example SET_NODE_LIST_TITLE: auto [sid, da1, da2, da3, da4, solver] =
  // card.parse_whole<int, float, float, float, float, dro::String>();
  template <typename... T> std::tuple<T...> parse_whole();

  // Parses all values of a card as the given types and returns them in a tuple.
  // Uses the values of the value_widths array as the value width for every
  // value. Example NODE: auto [nid, x, y, z, tc, rc] =
  // card.parse_whole<int, float, float, float, int, int>({8, 16, 16, 16, 8,
  // 8});
  template <typename... T>
  std::tuple<T...> parse_whole(std::array<uint8_t, sizeof...(T)> value_widths);

private:
  card_t *m_handle;
};

// A Keyword of a LS Dyna key file (input deck) with all its cards
class Keyword {
public:
  // An iterator to iterate over all cards of the keyword
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
    Card operator*() noexcept { return Card(&m_data[m_index]); }

  public:
    card_t *m_data;
    difference_type m_index;
  };

  Keyword(keyword_t *handle) noexcept;

  inline size_t num_cards() const noexcept { return m_handle->num_cards; }

  // Returns the nth card of a keyword. Where 0 refers to the first card
  // encountered in the key file.
  Card operator[](size_t index);

  CardsIterator begin() noexcept { return CardsIterator(m_handle->cards, 0); }
  CardsIterator end() noexcept {
    return CardsIterator(m_handle->cards, m_handle->num_cards);
  }

private:
  keyword_t *m_handle;
};

// A part of all keywords returned by dro::KeyFile::parse. A KeywordSlice
// contains only keyword with the same name.
class KeywordSlice {
public:
  // An iterator to iterate over all keywords of the slice
  class Iterator {
  public:
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;

    explicit Iterator(keyword_t *data, size_t left) noexcept
        : m_data(data), m_left(left) {}
    Iterator operator++() noexcept {
      if (m_left == 0) {
        m_data = nullptr;
      } else {
        m_data = &m_data[1];
        m_left--;
      }
      return *this;
    }
    Iterator operator++(int) noexcept {
      auto rv = *this;
      ++(*this);
      return rv;
    }
    bool operator==(const Iterator &rhs) const noexcept {
      return m_data == rhs.m_data;
    }
    bool operator!=(const Iterator &rhs) const noexcept {
      return m_data != rhs.m_data;
    }
    difference_type operator-(const Iterator &rhs) const noexcept {
      return (difference_type)m_data - (difference_type)rhs.m_data;
    }
    Keyword operator->() noexcept { return Keyword(m_data); }
    Keyword operator*() noexcept { return Keyword(m_data); }

  public:
    keyword_t *m_data;
    size_t m_left;
  };

  KeywordSlice(keyword_t *ptr, size_t size) noexcept;

  inline size_t size() const noexcept { return m_size; }

  // Access the nth keyword in the slice. Where 0 refers to the first keyword
  // encountered in the key file.
  Keyword operator[](size_t index);

  Iterator begin() noexcept { return Iterator(m_ptr, m_size - 1); }
  Iterator end() noexcept { return Iterator(nullptr, 0); }

private:
  keyword_t *m_ptr;
  size_t m_size;
};

// An array of keywords of a LS Dyna key file (input deck)
class Keywords : public Array<keyword_t> {
public:
  Keywords(keyword_t *data, size_t size) noexcept;
  Keywords(Keywords &&rhs) noexcept;
  ~Keywords() noexcept override;

  // Return a slice of keywords with the same name
  KeywordSlice operator[](const std::string &name);
};

// This static class holds the functions for parsing LS Dyna key files
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

  // Parses a LS Dyna key file for keywords and their respective cards. Returns
  // an array keywords
  // parse_includes: tells the function wether to parse include files via the
  // *INCLUDE and similar keywords or if they should be added as regular
  // keywords to the array.
  // Throws a dro::KeyFile::Exception if an error occurs.
  static Keywords parse(const std::filesystem::path &file_name,
                        bool parse_includes = true);
  // Same as parse, but instead of returning an array it calls a callback every
  // time a card (or empty keyword) is encountered.
  static void parse_with_callback(const std::filesystem::path &file_name,
                                  Callback callback,
                                  bool parse_includes = true);
};

template <typename T> static constexpr bool is_string_v = false;
template <typename T>
static constexpr bool is_number_v =
    std::is_integral_v<T> || std::is_floating_point_v<T>;

template <> constexpr bool is_string_v<char *> = true;
template <> constexpr bool is_string_v<String> = true;
template <> constexpr bool is_string_v<std::string> = true;

template <typename T> T Card::parse() const {
  return parse<T>(m_handle->value_width);
}

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
    char *value = card_parse_string_width(m_handle, value_width);
    return value;
  } else if constexpr (std::is_same_v<T, String>) {
    char *value = card_parse_string_width(m_handle, value_width);
    return String(value);
  } else if constexpr (std::is_same_v<T, std::string>) {
    char *value = card_parse_string_width(m_handle, value_width);
    std::string str(value);
    free(value);
    return str;
  } else {
    // clang-format off
    static_assert((is_number_v<T> || is_string_v<T>) && "Can not parse a card to the given type");
    // clang-format on
    return T{0};
  }
}

template <typename T> T Card::parse_string_no_trim() const noexcept {
  return parse_string_no_trim<T>(m_handle->value_width);
}

#ifdef __GNUC__
// clang-format off
template <typename T>
T Card::parse_string_no_trim(uint8_t value_width) const noexcept {
  static_assert(is_string_v<T> && "Can parse the whole card only as string (char*, dro::String, std::string)");
  return T{0};
}

template <typename T> T Card::parse_string_whole() const noexcept {
  static_assert(is_string_v<T> && "Can parse the whole card only as string (char*, dro::String, std::string)");
  return T{0};
}

template <typename T> T Card::parse_string_whole_no_trim() const noexcept {
  static_assert(is_string_v<T> && "Can parse the whole card only as string (char*, dro::String, std::string)");
  return T{0};
}
// clang-format on
#endif

template <typename... T> void Card::parse_whole(T &...rv) {

  int i = 0;

  begin();

  (
      [&] {
        using parse_type = std::remove_reference_t<decltype(rv)>;

        if (done()) {
          THROW_KEY_FILE_EXCEPTION(
              "Trying to parse %d values out of card \"%s\" with width %d",
              i + 1, m_handle->string, DEFAULT_VALUE_WIDTH);
        }

        rv = parse<parse_type>();

        next();
        i++;
      }(),
      ...);
}

template <typename... T>
void Card::parse_whole_width(std::array<uint8_t, sizeof...(T)> value_widths,
                             T &...rv) {
  int i = 0;

  begin();

  (
      [&] {
        using parse_type = std::remove_reference_t<decltype(rv)>;

        if (done()) {
          THROW_KEY_FILE_EXCEPTION("Trying to parse %d values out of card "
                                   "\"%s\"",
                                   i + 1, m_handle->string);
        }

        if (value_widths[i] == 0) {
          value_widths[i] = DEFAULT_VALUE_WIDTH;
        }

        rv = parse<parse_type>(value_widths[i]);

        next(value_widths[i]);
        i++;
      }(),
      ...);
}

template <typename... T> std::tuple<T...> Card::parse_whole() {
  int i = 0;
  begin();

  std::tuple<T...> t = {([&] {
    if (done()) {
      THROW_KEY_FILE_EXCEPTION(
          "Trying to parse %d values out of card \"%s\" with width %d", i + 1,
          m_handle->string, DEFAULT_VALUE_WIDTH);
    }

    T value = parse<T>();

    next();
    i++;

    return value;
  }())...};

  return t;
}

template <typename... T>
std::tuple<T...>
Card::parse_whole(std::array<uint8_t, sizeof...(T)> value_widths) {
  int i = 0;
  begin();

  std::tuple<T...> t = {([&] {
    if (done()) {
      THROW_KEY_FILE_EXCEPTION(
          "Trying to parse %d values out of card \"%s\" with width %d", i + 1,
          m_handle->string, DEFAULT_VALUE_WIDTH);
    }

    if (value_widths[i] == 0) {
      value_widths[i] = DEFAULT_VALUE_WIDTH;
    }

    T value = parse<T>(value_widths[i]);

    next(value_widths[i++]);

    return value;
  }())...};

  return t;
}

} // namespace dro