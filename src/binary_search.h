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

/* This file has been automatically generated*/

#ifndef BINARY_SEARCH_H
#define BINARY_SEARCH_H
#include "binout_directory.h"
#include "d3_defines.h"
#include "key.h"
#include <stddef.h>
#ifdef PROFILING
#include "profiling.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

size_t binout_directory_binary_search_folder(const binout_folder_t *arr,
                                             size_t start_index,
                                             size_t end_index,
                                             const path_view_t *value);

size_t binout_directory_binary_search_folder_insert(const binout_folder_t *arr,
                                                    size_t start_index,
                                                    size_t end_index,
                                                    const path_view_t *value,
                                                    int *found);

size_t binout_directory_binary_search_file(const binout_file_t *arr,
                                           size_t start_index, size_t end_index,
                                           const path_view_t *value);

size_t binout_directory_binary_search_file_insert(const binout_file_t *arr,
                                                  size_t start_index,
                                                  size_t end_index,
                                                  const char *value,
                                                  int *found);

size_t d3_word_binary_search(const d3_word *arr, size_t start_index,
                             size_t end_index, d3_word value);

size_t d3_word_binary_search_insert(const d3_word *arr, size_t start_index,
                                    size_t end_index, d3_word value,
                                    int *found);

size_t key_file_binary_search_insert(const keyword_t *arr, size_t start_index,
                                     size_t end_index, const char *value,
                                     int *found);

size_t key_file_binary_search(const keyword_t *arr, size_t start_index,
                              size_t end_index, const char *value);

#ifdef PROFILING
size_t string_binary_search_insert(char const **arr, size_t start_index,
                                   size_t end_index, const char *value,
                                   int *found);

size_t profiling_stack_binary_search_insert(const profiling_stack_t *arr,
                                            size_t start_index,
                                            size_t end_index, const char *value,
                                            int *found);

#endif

#ifdef __cplusplus
}
#endif

#endif