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

#include "string_builder.h"
#include "profiling.h"
#include <stdlib.h>
#include <string.h>

string_builder_t string_builder_new() {
  BEGIN_PROFILE_FUNC();

  string_builder_t b = {0};

  END_PROFILE_FUNC();
  return b;
}

void string_builder_append(string_builder_t *b, const char *s) {
  BEGIN_PROFILE_FUNC();

  const size_t s_len = strlen(s);
  b->cap += s_len;
  if (b->ptr == 0) {
    b->cap++;
  }

  b->buffer = realloc(b->buffer, b->cap);
  memcpy(&b->buffer[b->ptr], s, s_len + 1);
  b->ptr += s_len;

  END_PROFILE_FUNC();
}

void string_builder_append_char(string_builder_t *b, char c) {
  BEGIN_PROFILE_FUNC();

  b->cap += 1;
  if (b->ptr == 0) {
    b->cap++;
  }

  b->buffer = realloc(b->buffer, b->cap);
  b->buffer[b->ptr++] = c;
  b->buffer[b->ptr] = '\0';

  END_PROFILE_FUNC();
}

char *string_builder_move(string_builder_t *b) {
  BEGIN_PROFILE_FUNC();

  char *buffer = b->buffer;
  b->buffer = NULL;
  b->cap = 0;
  b->ptr = 0;

  END_PROFILE_FUNC();
  return buffer;
}

void string_builder_free(string_builder_t *b) {
  BEGIN_PROFILE_FUNC();

  free(b->buffer);
  b->buffer = NULL;
  b->cap = 0;
  b->ptr = 0;

  END_PROFILE_FUNC();
}
