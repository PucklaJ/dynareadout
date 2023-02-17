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

#include "extra_string.h"
#include <stdlib.h>
#include <string.h>

char extra_string_get(const extra_string *str, size_t index) {
  if (index < EXTRA_STRING_BUFFER_SIZE) {
    return str->buffer[index];
  }
  return str->extra[index - EXTRA_STRING_BUFFER_SIZE];
}

void extra_string_set(extra_string *str, size_t index, char value) {
  if (index < EXTRA_STRING_BUFFER_SIZE) {
    str->buffer[index] = value;
  } else {
    str->extra[index - EXTRA_STRING_BUFFER_SIZE] = value;
  }
}

void extra_string_copy(extra_string *dst, const extra_string *src,
                       size_t src_len, size_t offset) {
  memcpy(dst->buffer, &src->buffer[offset],
         (EXTRA_STRING_BUFFER_SIZE - offset) * sizeof(char));
  if (src_len >= EXTRA_STRING_BUFFER_SIZE) {
    dst->extra = realloc(dst->extra, (src_len - EXTRA_STRING_BUFFER_SIZE + 1) *
                                         sizeof(char));
    if (offset != 0) {
      memcpy(&dst->buffer[EXTRA_STRING_BUFFER_SIZE - offset], src->extra,
             offset * sizeof(char));
      memcpy(dst->extra, &src->extra[offset],
             (src_len - EXTRA_STRING_BUFFER_SIZE + 1 - offset) * sizeof(char));
    } else {
      memcpy(dst->extra, src->extra,
             (src_len - EXTRA_STRING_BUFFER_SIZE + 1) * sizeof(char));
    }
  }
}

void extra_string_copy_to_string(char *dst, const extra_string *src,
                                 size_t dst_len) {
  if (dst_len < EXTRA_STRING_BUFFER_SIZE) {
    memcpy(dst, src->buffer, dst_len);
  } else {
    memcpy(dst, src->buffer, EXTRA_STRING_BUFFER_SIZE * sizeof(char));
    memcpy(&dst[EXTRA_STRING_BUFFER_SIZE], src->extra,
           dst_len - EXTRA_STRING_BUFFER_SIZE + 1);
  }
}

int extra_string_compare(const extra_string *lhs, const char *rhs) {
  size_t i = 0;
  while ((i < EXTRA_STRING_BUFFER_SIZE && lhs->buffer[i] != '\0') &&
         rhs[i] != '\0') {
    if (lhs->buffer[i] != rhs[i]) {
      return (int)(lhs->buffer[i] - rhs[i]);
    }

    i++;
  }

  if (i == EXTRA_STRING_BUFFER_SIZE) {
    if (lhs->extra[i] == '\0' && rhs[i] == '\0') {
      return 0;
    }

    if (lhs->extra[i] == '\0') {
      return -1;
    }

    return strcmp(lhs->extra, &rhs[i]);
  }

  if (lhs->buffer[i] == '\0' && rhs[i] == '\0') {
    return 0;
  }

  if (lhs->buffer[i] == '\0') {
    return -1;
  }

  return 1;
}

int extra_string_starts_with(const extra_string *str, const char *prefix) {
  size_t i = 0;
  while (i < EXTRA_STRING_BUFFER_SIZE && str->buffer[i] != '\0' &&
         prefix[i] != '\0') {
    if (str->buffer[i] != prefix[i]) {
      return 0;
    }

    i++;
  }

  if (i == EXTRA_STRING_BUFFER_SIZE) {
    if (prefix[i] == '\0') {
      return 1;
    }
  } else if (str->buffer[i] == '\0') {
    if (prefix[i] == '\0') {
      return 1;
    } else {
      return 0;
    }
  } else { /* prefix[i] == '\0'*/
    return 1;
  }

  size_t j = 0;
  while (str->extra[j] != '\0' && prefix[i] != '\0') {
    if (str->extra[j] != prefix[i]) {
      return 0;
    }

    j++;
    i++;
  }

  if (str->extra[j] == '\0') {
    if (prefix[i] == '\0') {
      return 1;
    }
  }

  /* prefix[i] == '\0*/
  return 1;
}