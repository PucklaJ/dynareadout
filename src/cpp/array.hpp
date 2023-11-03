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
#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace dro {

// An Array takes ownership over some arbitrary C array and frees its memory in
// the destructor. The memory of the C array NEEDS to be allocated by malloc,
// calloc, realloc or reallocarray!
template <typename T> class Array {
public:
  // An iterator which can not write the array
  class ConstIterator {
  public:
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T *;         // or also value_type*
    using reference = const T &; // or also value_type&

    explicit ConstIterator(pointer data, difference_type index) noexcept
        : m_data(data), m_index(index) {}
    ConstIterator operator++() noexcept {
      m_index++;
      return *this;
    }
    ConstIterator operator++(int) noexcept {
      auto rv = *this;
      ++(*this);
      return rv;
    }
    bool operator==(const ConstIterator &rhs) const noexcept {
      return m_index == rhs.m_index;
    }
    bool operator!=(const ConstIterator &rhs) const noexcept {
      return m_index != rhs.m_index;
    }
    difference_type operator-(const ConstIterator &rhs) const {
      return m_index - rhs.m_index;
    }
    reference operator*() const { return m_data[m_index]; }
    const pointer operator->() const { return &m_data[m_index]; }

  public:
    pointer m_data;
    difference_type m_index;
  };

  // An Iterator which can write the array
  class Iterator {
  public:
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T *;   // or also value_type*
    using reference = T &; // or also value_type&
    using const_reference = const T &;

    explicit Iterator(pointer data, difference_type index) noexcept
        : m_data(data), m_index(index) {}
    Iterator operator++() noexcept {
      m_index++;
      return *this;
    }
    Iterator operator++(int) noexcept {
      auto rv = *this;
      ++(*this);
      return rv;
    }
    bool operator==(const Iterator &rhs) const noexcept {
      return m_index == rhs.m_index;
    }
    bool operator!=(const Iterator &rhs) const noexcept {
      return m_index != rhs.m_index;
    }
    difference_type operator-(const Iterator &rhs) const {
      return m_index - rhs.m_index;
    }
    const_reference operator*() const { return m_data[m_index]; }
    const pointer operator->() const { return &m_data[m_index]; }
    reference operator*() { return m_data[m_index]; }
    pointer operator->() { return &m_data[m_index]; }

  public:
    pointer m_data;
    difference_type m_index;
  };

  // Allocates memory and creates a new array with it
  // size ........... Number of elements of the new array
  static Array<T> New(size_t size);

  // data ........... An C array allocated by malloc
  // size ........... The number of elements inside data
  // delete_data .... Wether to free data in the destructor
  Array() noexcept;
  Array(T *data, size_t size, bool delete_data = true) noexcept;
  Array(Array<T> &&rhs) noexcept;
  Array(const Array<T> &rhs) noexcept;
  virtual ~Array() noexcept;

  virtual inline T &operator[](size_t index);
  virtual inline const T &operator[](size_t index) const;
  Array<T> &operator=(Array<T> &&rhs) noexcept;
  Array<T> &operator=(const Array<T> &rhs) noexcept;
  bool operator==(const char *str2) const noexcept;
  bool operator==(const std::string &str2) const noexcept;
  bool operator==(const Array<T> &rhs) const noexcept;

  T *data() noexcept { return m_data; }
  const T *data() const noexcept { return m_data; }
  size_t size() const noexcept { return m_size; }
  bool empty() const noexcept { return m_size == 0; }

  inline std::pair<T **, size_t *> access() noexcept {
    return std::make_pair(&m_data, &m_size);
  }

  // Convert the array to a std::string. Only works for uint8_t, int8_t and
  // char
  std::string str() const noexcept;

  Iterator begin() noexcept { return Iterator(m_data, 0); }
  Iterator end() noexcept { return Iterator(m_data, m_size); }
  ConstIterator begin() const noexcept { return ConstIterator(m_data, 0); }
  ConstIterator end() const noexcept { return ConstIterator(m_data, m_size); }

protected:
  T *m_data;
  size_t m_size;
  bool m_delete_data;
};

class String : public Array<char> {
public:
  inline String(String &&rhs) noexcept;
  inline String(const String &rhs) noexcept;
  String(char *str, bool delete_data = true) noexcept
      : Array<char>(str, ~0, delete_data) {}

  inline String &operator=(String &&rhs) noexcept;
  inline String &operator=(const String &rhs) noexcept;

  inline size_t size() const noexcept { return strlen(m_data); }
  inline bool empty() const noexcept { return m_data[0] == '\0'; }

  std::string str() const noexcept { return std::string(m_data); }

  bool operator==(const char *other) const noexcept {
    size_t i = 0;
    for (; m_data[i] != '\0' && other[i] != '\0'; i++) {
      if (m_data[i] != other[i]) {
        return false;
      }
    }
    return m_data[i] == '\0' && other[i] == '\0';
  }

  bool operator==(const String &rhs) const noexcept {
    size_t i = 0;
    for (; m_data[i] != '\0' && rhs.m_data[i] != '\0'; i++) {
      if (m_data[i] != rhs.m_data[i]) {
        return false;
      }
    }

    return m_data[i] == '\0' && rhs.m_data[i] == '\0';
  }

  bool operator==(const std::string &rhs) const noexcept {
    return operator==(rhs.c_str());
  }

  inline char &operator[](size_t index) override;
  inline const char &operator[](size_t index) const override;
};

class SizedString : public Array<char> {
public:
  SizedString(char *str, size_t size, bool delete_data = true) noexcept
      : Array<char>(str, size, delete_data) {}

  std::string str() const noexcept {
    if (empty()) {
      return std::string();
    }
    return std::string(m_data, m_size);
  }

  bool operator==(const char *other) const noexcept {
    if (m_size == 0) {
      return other[0] == '\0';
    }

    size_t i = 0;
    for (; i < m_size && other[i] != '\0'; i++) {
      if (m_data[i] != other[i]) {
        return false;
      }
    }

    return i == m_size && other[i] == '\0';
  }

  bool operator==(const SizedString &rhs) const noexcept {
    if (m_size != rhs.m_size) {
      return false;
    }

    for (size_t i = 0; i < m_size; i++) {
      if (m_data[i] != rhs.m_data[i]) {
        return false;
      }
    }

    return true;
  }

  bool operator==(const std::string &rhs) const noexcept {
    return operator==(rhs.c_str());
  }
};

static bool operator==(const String &str1, const SizedString &str2) noexcept {
  size_t i = 0;
  for (; i < str1.data()[i] != '\0' && i < str2.size(); i++) {
    if (str1[i] != str2[i]) {
      return false;
    }
  }

  return i == str2.size() && str1.data()[i] == '\0';
}

inline bool operator==(const SizedString &str2, const String &str1) noexcept {
  return str1 == str2;
}

static std::ostream &operator<<(std::ostream &stream, const String &str) {
  return stream << str.data();
}

static std::ostream &operator<<(std::ostream &stream, const SizedString &str) {
  for (const auto &v : str) {
    stream << v;
  }
  return stream;
}

template <typename T>
static std::ostream &operator<<(std::ostream &stream, const Array<T> &arr) {
  stream << "[";

  for (size_t i = 0; i < arr.size(); i++) {
    if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t> ||
                  std::is_same_v<T, char>) {
      stream << "'" << static_cast<char>(arr[i]) << "'";
    } else {
      stream << arr[i];
    }

    if (i + 1 != arr.size()) {
      stream << ", ";
    }
  }

  stream << "]";

  return stream;
}

template <typename T> Array<T> Array<T>::New(size_t size) {
  T *data = reinterpret_cast<T *>(malloc(size * sizeof(T)));
  if (!data) {
    throw std::runtime_error(
        std::string("Failed to allocate memory for new array: ") +
        strerror(errno));
  }
  return Array(data, size);
}

template <typename T>
Array<T>::Array() noexcept : m_data(nullptr), m_size(0), m_delete_data(true) {}

template <typename T>
Array<T>::Array(T *data, size_t size, bool delete_data) noexcept
    : m_data(data), m_size(size), m_delete_data(delete_data) {}

template <typename T> Array<T>::Array(Array<T> &&rhs) noexcept {
  *this = std::move(rhs);
}

template <typename T> Array<T>::Array(const Array<T> &rhs) noexcept {
  *this = rhs;
}

template <typename T> Array<T>::~Array() noexcept {
  if (m_delete_data && m_data) {
    free(m_data);
    m_data = nullptr;
  }
}

template <typename T> T &Array<T>::operator[](size_t index) {
  if (empty() || index > m_size - 1) {
    throw std::runtime_error("Index out of Range");
  }

  return m_data[index];
}

template <typename T> const T &Array<T>::operator[](size_t index) const {
  if (empty() || index > m_size - 1) {
    throw std::runtime_error("Index out of Range");
  }

  return m_data[index];
}

char &String::operator[](size_t index) {
  for (size_t i = 0; m_data[i] != '\0'; i++) {
    if (i == index) {
      return m_data[i];
    }
  }

  throw std::runtime_error("Index out of Range");
}

const char &String::operator[](size_t index) const {
  for (size_t i = 0; m_data[i] != '\0'; i++) {
    if (i == index) {
      return m_data[i];
    }
  }

  throw std::runtime_error("Index out of Range");
}

template <typename T> Array<T> &Array<T>::operator=(Array<T> &&rhs) noexcept {
  m_data = rhs.m_data;
  m_size = rhs.m_size;
  m_delete_data = rhs.m_delete_data;
  rhs.m_data = nullptr;
  rhs.m_size = 0;
  rhs.m_delete_data = false;
  return *this;
}

template <typename T>
Array<T> &Array<T>::operator=(const Array<T> &rhs) noexcept {
  m_size = rhs.m_size;
  m_data = reinterpret_cast<T *>(malloc(m_size * sizeof(T)));
  memcpy(m_data, rhs.m_data, m_size * sizeof(T));
  m_delete_data = true;
  return *this;
}

template <typename T> std::string Array<T>::str() const noexcept {
  static_assert(std::is_same_v<T, char> || std::is_same_v<T, int8_t> ||
                std::is_same_v<T, uint8_t>);
  return SizedString(reinterpret_cast<char *>(m_data), m_size, false).str();
}

template <typename T>
bool Array<T>::operator==(const char *str2) const noexcept {
  static_assert(std::is_same_v<T, char> || std::is_same_v<T, int8_t> ||
                std::is_same_v<T, uint8_t>);
  return SizedString(const_cast<char *>(reinterpret_cast<const char *>(m_data)),
                     m_size, false) == str2;
}

template <typename T>
bool Array<T>::operator==(const std::string &str2) const noexcept {
  static_assert(std::is_same_v<T, char> || std::is_same_v<T, int8_t> ||
                std::is_same_v<T, uint8_t>);
  return SizedString(const_cast<char *>(reinterpret_cast<const char *>(m_data)),
                     m_size, false) == str2;
}

template <typename T>
bool Array<T>::operator==(const Array<T> &rhs) const noexcept {
  if (m_size != rhs.m_size) {
    return false;
  }

  for (size_t i = 0; i < m_size; i++) {
    if (m_data[i] != rhs.m_data[i]) {
      return false;
    }
  }

  return true;
}

String::String(String &&rhs) noexcept { *this = std::move(rhs); }

String::String(const String &rhs) noexcept { *this = rhs; }

String &String::operator=(String &&rhs) noexcept {
  m_data = rhs.m_data;
  m_delete_data = rhs.m_delete_data;
  rhs.m_data = nullptr;
  rhs.m_delete_data = false;
  return *this;
}

String &String::operator=(const String &rhs) noexcept {
  m_data = strdup(rhs.m_data);
  m_delete_data = true;
  return *this;
}

} // namespace dro
