#pragma once
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace dro {

template <typename T> class Vector {
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

  Vector(T *data, size_t size, bool delete_data = true) noexcept;
  Vector(Vector<T> &&rhs) noexcept;
  ~Vector() noexcept;

  inline T &operator[](size_t index);
  inline const T &operator[](size_t index) const;
  Vector<T> &operator=(Vector<T> &&rhs) noexcept;

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
Vector<T>::Vector(T *data, size_t size, bool delete_data) noexcept
    : m_data(data), m_size(size), m_delete_data(delete_data) {}

template <typename T>
Vector<T>::Vector(Vector<T> &&rhs) noexcept
    : m_data(rhs.m_data), m_size(rhs.m_size), m_delete_data(rhs.m_delete_data) {
  rhs.m_data = nullptr;
  rhs.m_size = 0;
}

template <typename T> Vector<T>::~Vector() noexcept {
  if (m_delete_data && m_data)
    free(m_data);
}

template <typename T> T &Vector<T>::operator[](size_t index) {
  if (index > m_size - 1) {
    throw std::runtime_error("Index out of Range");
  }

  return m_data[index];
}

template <typename T> const T &Vector<T>::operator[](size_t index) const {
  if (index > m_size - 1) {
    throw std::runtime_error("Index out of Range");
  }

  return m_data[index];
}

template <typename T>
Vector<T> &Vector<T>::operator=(Vector<T> &&rhs) noexcept {
  m_data = rhs.m_data;
  m_size = rhs.m_size;
  m_delete_data = rhs.m_delete_data;
  rhs.m_data = nullptr;
  rhs.m_size = 0;
  return *this;
}

template <typename T> std::string Vector<T>::str() const noexcept {
  static_assert(std::is_same_v<T, char> || std::is_same_v<T, int8_t> ||
                std::is_same_v<T, uint8_t>);

  std::string str(reinterpret_cast<const char *>(m_data), m_size);
  return str;
}

template <typename T>
bool operator==(const Vector<T> &str1, const char *str2) noexcept {
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
inline bool operator==(const char *str2, const Vector<T> &str1) noexcept {
  return str1 == str2;
}

template <typename T>
inline bool operator==(const std::string &str2,
                       const Vector<T> &str1) noexcept {
  return str1 == str2.c_str();
}

template <typename T>
inline bool operator==(const Vector<T> &str1,
                       const std::string &str2) noexcept {
  return str1 == str2.c_str();
}

template <typename T>
inline std::ostream &operator<<(std::ostream &stream, const Vector<T> &str) {
  static_assert(std::is_same_v<T, char> || std::is_same_v<T, int8_t> ||
                std::is_same_v<T, uint8_t>);

  for (size_t i = 0; i < str.size(); i++) {
    stream << static_cast<char>(str[i]);
  }
  return stream;
}

typedef Vector<char> String;

} // namespace dro
