#include "vector.hpp"
#include <cstring>
#include <iostream>

bool operator==(const dro::String &str1, const char *str2) {
  return strcmp(str1.data(), str2) == 0;
}

std::ostream &operator<<(std::ostream &stream, const dro::String &str) {
  return stream << str.data();
}