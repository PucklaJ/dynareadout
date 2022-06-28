#pragma once
#include <cstdlib>
#include <stdexcept>

namespace dro {

template <typename T> class Vector {
public:
  Vector(T *data, size_t size);
  Vector(Vector<T> &&rhs);
  ~Vector();

  T &operator[](size_t index);
  const T &operator[](size_t index) const;
  Vector<T> &operator=(Vector<T> &&rhs);

  T *data() { return m_data; }
  const T *data() const { return m_data; }

private:
  T *m_data;
  size_t m_size;
};

template <typename T>
Vector<T>::Vector(T *data, size_t size) : m_data(data), m_size(size) {}

template <typename T>
Vector<T>::Vector(Vector<T> &&rhs) : m_data(rhs.m_data), m_size(rhs.m_size) {
  rhs.m_data = nullptr;
  rhs.m_size = 0;
}

template <typename T> Vector<T>::~Vector() {
  if (m_data)
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

template <typename T> Vector<T> &Vector<T>::operator=(Vector<T> &&rhs) {
  m_data = rhs.m_data;
  m_size = rhs.m_size;
  rhs.m_data = nullptr;
  rhs.m_size = 0;
  return *this;
}

typedef Vector<char> String;

} // namespace dro

bool operator==(const dro::String &str1, const char *str2);
std::ostream &operator<<(std::ostream &stream, const dro::String &str);
