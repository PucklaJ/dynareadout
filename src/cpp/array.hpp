/***********************************************************************************
 *                         This file is part of dynareadout
 *                    https://github.com/PucklaMotzer09/dynareadout
 ***********************************************************************************
 * Copyright (c) 2022 PucklaMotzer09
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
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace dro {

template <typename T> class Array {
public:
  class Iterator {
  public:
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T *;   // or also value_type*
    using reference = T &; // or also value_type&

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
    reference operator*() { return m_data[m_index]; }
    const reference operator*() const { return m_data[m_index]; }
    pointer operator->() { return &m_data[m_index]; }
    const pointer operator->() const { return &m_data[m_index]; }

  private:
    pointer m_data;
    difference_type m_index;
  };

  Array(T *data, size_t size, bool delete_data = true) noexcept;
  Array(Array<T> &&rhs) noexcept;
  ~Array() noexcept;

  inline T &operator[](size_t index);
  inline const T &operator[](size_t index) const;
  Array<T> &operator=(Array<T> &&rhs) noexcept;

  T *data() noexcept { return m_data; }
  const T *data() const noexcept { return m_data; }
  size_t size() const noexcept { return m_size; }

  std::string str() const noexcept;

  Iterator begin() noexcept { return Iterator(m_data, 0); }
  Iterator end() noexcept { return Iterator(m_data, m_size); }

private:
  T *m_data;
  size_t m_size;
  bool m_delete_data;
};

template <typename T>
Array<T>::Array(T *data, size_t size, bool delete_data) noexcept
    : m_data(data), m_size(size), m_delete_data(delete_data) {}

template <typename T>
Array<T>::Array(Array<T> &&rhs) noexcept
    : m_data(rhs.m_data), m_size(rhs.m_size), m_delete_data(rhs.m_delete_data) {
  rhs.m_data = nullptr;
  rhs.m_size = 0;
}

template <typename T> Array<T>::~Array() noexcept {
  if (m_delete_data && m_data)
    free(m_data);
}

template <typename T> T &Array<T>::operator[](size_t index) {
  if (index > m_size - 1) {
    throw std::runtime_error("Index out of Range");
  }

  return m_data[index];
}

template <typename T> const T &Array<T>::operator[](size_t index) const {
  if (index > m_size - 1) {
    throw std::runtime_error("Index out of Range");
  }

  return m_data[index];
}

template <typename T> Array<T> &Array<T>::operator=(Array<T> &&rhs) noexcept {
  m_data = rhs.m_data;
  m_size = rhs.m_size;
  m_delete_data = rhs.m_delete_data;
  rhs.m_data = nullptr;
  rhs.m_size = 0;
  return *this;
}

template <typename T> std::string Array<T>::str() const noexcept {
  static_assert(std::is_same_v<T, char> || std::is_same_v<T, int8_t> ||
                std::is_same_v<T, uint8_t>);

  std::string str(reinterpret_cast<const char *>(m_data), m_size);
  return str;
}

template <typename T>
bool operator==(const Array<T> &str1, const char *str2) noexcept {
  static_assert(std::is_same_v<T, char> || std::is_same_v<T, int8_t> ||
                std::is_same_v<T, uint8_t>);
  if (str1.data()[str1.size() - 1] == '\0') {
    return strcmp(reinterpret_cast<const char *>(str1.data()), str2) == 0;
  }

  if (strlen(str2) != str1.size()) {
    return false;
  }

  for (size_t i = 0; i < str1.size(); i++) {
    if (static_cast<char>(str1[i]) != str2[i]) {
      return false;
    }
  }

  return true;
}

template <typename T>
inline bool operator==(const char *str2, const Array<T> &str1) noexcept {
  return str1 == str2;
}

template <typename T>
inline bool operator==(const std::string &str2, const Array<T> &str1) noexcept {
  return str1 == str2.c_str();
}

template <typename T>
inline bool operator==(const Array<T> &str1, const std::string &str2) noexcept {
  return str1 == str2.c_str();
}

template <typename T>
inline std::ostream &operator<<(std::ostream &stream, const Array<T> &str) {
  static_assert(std::is_same_v<T, char> || std::is_same_v<T, int8_t> ||
                std::is_same_v<T, uint8_t>);

  for (size_t i = 0; i < str.size(); i++) {
    stream << static_cast<char>(str[i]);
  }
  return stream;
}

typedef Array<char> String;

} // namespace dro
