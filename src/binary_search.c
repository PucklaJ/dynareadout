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

#include "binary_search.h"
#include "profiling.h"
#include <string.h>

#define DEFINE_BINARY_SEARCH_FUNCTION(func_name, arr_type, search_type,        \
                                      cmp_func, cmp_suf)                       \
  DEFINE_BINARY_SEARCH_PROTO(func_name, arr_type, search_type) {               \
    BEGIN_PROFILE_FUNC();                                                      \
                                                                               \
    if (start_index == end_index) {                                            \
      if (cmp_func(value, arr[start_index] cmp_suf) == 0) {                    \
        END_PROFILE_FUNC();                                                    \
        return start_index;                                                    \
      }                                                                        \
                                                                               \
      END_PROFILE_FUNC();                                                      \
      return ~0;                                                               \
    }                                                                          \
                                                                               \
    const size_t half_index = start_index + (end_index - start_index) / 2;     \
    const int cmp_val = cmp_func(value, arr[half_index] cmp_suf);              \
                                                                               \
    if (cmp_val < 0) {                                                         \
      const size_t index = func_name(arr, start_index, half_index, value);     \
      END_PROFILE_FUNC();                                                      \
      return index;                                                            \
    } else if (cmp_val > 0) {                                                  \
      if (half_index == end_index - 1) {                                       \
        const size_t index = func_name(arr, end_index, end_index, value);      \
        END_PROFILE_FUNC();                                                    \
        return index;                                                          \
      }                                                                        \
      const size_t index = func_name(arr, half_index, end_index, value);       \
      END_PROFILE_FUNC();                                                      \
      return index;                                                            \
    }                                                                          \
                                                                               \
    END_PROFILE_FUNC();                                                        \
    return half_index;                                                         \
  }

#define DEFINE_BINARY_SEARCH_INSERT_FUNCTION(func_name, arr_type, search_type, \
                                             cmp_func, cmp_suf)                \
  DEFINE_BINARY_SEARCH_INSERT_PROTO(func_name, arr_type, search_type) {        \
    BEGIN_PROFILE_FUNC();                                                      \
                                                                               \
    if (start_index == end_index) {                                            \
      const int cmp_value = cmp_func(value, arr[start_index] cmp_suf);         \
                                                                               \
      if (cmp_value == 0) {                                                    \
        *found = 1;                                                            \
        END_PROFILE_FUNC();                                                    \
        return start_index;                                                    \
      }                                                                        \
      if (cmp_value > 0) {                                                     \
        *found = 0;                                                            \
        END_PROFILE_FUNC();                                                    \
        return start_index + 1;                                                \
      }                                                                        \
                                                                               \
      *found = 0;                                                              \
      END_PROFILE_FUNC();                                                      \
      return start_index;                                                      \
    }                                                                          \
                                                                               \
    const size_t half_index = start_index + (end_index - start_index) / 2;     \
    const int cmp_val = cmp_func(value, arr[half_index] cmp_suf);              \
                                                                               \
    if (cmp_val < 0) {                                                         \
      const size_t index =                                                     \
          func_name(arr, start_index, half_index, value, found);               \
      END_PROFILE_FUNC();                                                      \
      return index;                                                            \
    } else if (cmp_val > 0) {                                                  \
      if (half_index == end_index - 1) {                                       \
        const size_t index =                                                   \
            func_name(arr, end_index, end_index, value, found);                \
        END_PROFILE_FUNC();                                                    \
        return index;                                                          \
      }                                                                        \
      const size_t index =                                                     \
          func_name(arr, half_index, end_index, value, found);                 \
      END_PROFILE_FUNC();                                                      \
      return index;                                                            \
    }                                                                          \
                                                                               \
    *found = 1;                                                                \
    END_PROFILE_FUNC();                                                        \
    return half_index;                                                         \
  }

#define INTCMP(a, b) (a == b ? 0 : (a > b ? 1 : -1))

DEFINE_BINARY_SEARCH_FUNCTION(binout_directory_binary_search_folder,
                              const binout_folder_t *, const path_view_t *,
                              path_view_strcmp, .name)

DEFINE_BINARY_SEARCH_INSERT_FUNCTION(
    binout_directory_binary_search_folder_insert, const binout_folder_t *,
    const path_view_t *, path_view_strcmp, .name)

DEFINE_BINARY_SEARCH_FUNCTION(binout_directory_binary_search_file,
                              const binout_file_t *, const path_view_t *,
                              path_view_strcmp, .name)

DEFINE_BINARY_SEARCH_INSERT_FUNCTION(binout_directory_binary_search_file_insert,
                                     const binout_file_t *, const char *,
                                     strcmp, .name)

DEFINE_BINARY_SEARCH_FUNCTION(d3_word_binary_search, const d3_word *, d3_word,
                              INTCMP, )

#ifdef PROFILING
DEFINE_BINARY_SEARCH_INSERT_FUNCTION(string_binary_search_insert, char const **,
                                     const char *, strcmp, )
#endif
