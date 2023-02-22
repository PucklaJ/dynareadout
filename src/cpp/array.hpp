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
    using pointer = T *;   // or also value_type*
    using reference = T &; // or also value_type&

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
    const reference operator*() { return m_data[m_index]; }
    const pointer operator->() const { return &m_data[m_index]; }

  public:
    pointer m_data;
    difference_type m_index;
  };

  // An Iterator which can write the array
  class Iterator : public ConstIterator {
  public:
    Iterator(typename ConstIterator::pointer data,
             typename ConstIterator::difference_type index) noexcept
        : ConstIterator(data, index) {}

    typename ConstIterator::reference operator*() {
      return ConstIterator::m_data[ConstIterator::m_index];
    }
    typename ConstIterator::pointer operator->() {
      return &ConstIterator::m_data[ConstIterator::m_index];
    }
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
  Array(const Array<T> &rhs) = delete;
  virtual ~Array() noexcept;

  inline T &operator[](size_t index);
  inline const T &operator[](size_t index) const;
  Array<T> &operator=(Array<T> &&rhs) noexcept;

  T *data() noexcept { return m_data; }
  const T *data() const noexcept { return m_data; }
  size_t size() const noexcept { return m_size; }
  bool empty() const noexcept { return m_size == 0; }

  inline std::pair<T **, size_t *> access() noexcept {
    return std::make_pair(&m_data, &m_size);
  }

  // Convert the array to a std::string. Only works for uint8_t, int8_t and char
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
  if (empty()) {
    return std::string();
  }

  if (m_data[m_size - 1] == '\0') {
    return std::string(reinterpret_cast<const char *>(m_data), m_size - 1);
  }

  return std::string(reinterpret_cast<const char *>(m_data), m_size);
}

template <typename T>
bool operator==(const Array<T> &str1, const char *str2) noexcept {
  static_assert(std::is_same_v<T, char> || std::is_same_v<T, int8_t> ||
                std::is_same_v<T, uint8_t>);
  // An empty String can be equal to a string with 0 length
  if (str1.empty()) {
    return strlen(str2) == 0;
  }

  // If the string ends with a null terminator we can use strcmp()
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

  for (size_t i = 0; i < str.size() && str[i] != '\0'; i++) {
    stream << static_cast<char>(str[i]);
  }
  return stream;
}

class String : public Array<char> {
public:
  String(char *data, size_t size, bool delete_data = true) noexcept
      : Array<char>(data, size, delete_data) {}
  String(char *str, bool delete_data = true) noexcept
      : Array<char>(str, strlen(str) + 1, delete_data) {}
};

} // namespace dro
