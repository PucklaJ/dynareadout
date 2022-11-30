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

#ifndef BINARY_SEARCH_H
#define BINARY_SEARCH_H
#include "binout_directory.h"
#include "d3_defines.h"
#include <stddef.h>

#define DEFINE_BINARY_SEARCH_PROTO(func_name, arr_type, search_type)           \
  size_t func_name(arr_type arr, size_t start_index, size_t end_index,         \
                   search_type value)
#define DEFINE_BINARY_SEARCH_INSERT_PROTO(func_name, arr_type, search_type)    \
  size_t func_name(arr_type arr, size_t start_index, size_t end_index,         \
                   search_type value, int *found)

#ifdef __cplusplus
extern "C" {
#endif

DEFINE_BINARY_SEARCH_PROTO(binout_directory_binary_search_folder,
                           const binout_folder_t *, const path_view_t *);
DEFINE_BINARY_SEARCH_INSERT_PROTO(binout_directory_binary_search_folder_insert,
                                  const binout_folder_t *, const path_view_t *);

DEFINE_BINARY_SEARCH_PROTO(binout_directory_binary_search_file,
                           const binout_file_t *, const path_view_t *);
DEFINE_BINARY_SEARCH_INSERT_PROTO(binout_directory_binary_search_file_insert,
                                  const binout_file_t *, const char *);

DEFINE_BINARY_SEARCH_PROTO(d3_word_binary_search, const d3_word *, d3_word);

#ifdef PROFILING
DEFINE_BINARY_SEARCH_INSERT_PROTO(string_binary_search_insert, char const **,
                                  const char *);
#endif

#ifdef __cplusplus
}
#endif

#endif
