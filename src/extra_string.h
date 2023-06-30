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

#ifndef EXTRA_STRING_H
#define EXTRA_STRING_H
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* A string which allocates a part on the stack and allocates more on the heap
 * if it needs "extra" memory*/
#define EXTRA_STRING_BUFFER_SIZE                                               \
  (80 + 2) /* LENGTH_OF_DYNA_LINE + carriage return (\r) and newline (\n)*/
typedef struct {
  char buffer[EXTRA_STRING_BUFFER_SIZE];
  char *extra;
} extra_string;

char extra_string_get(const extra_string *str, size_t index);

void extra_string_set(extra_string *str, size_t index, char value);

void extra_string_copy(extra_string *dst, const extra_string *src,
                       size_t src_len, size_t offset);

void extra_string_copy_to_string(char *dst, const extra_string *src,
                                 size_t dst_len);

int extra_string_compare(const extra_string *lhs, const char *rhs);

int extra_string_starts_with(const extra_string *str, const char *prefix);

#ifdef __cplusplus
}
#endif

#endif