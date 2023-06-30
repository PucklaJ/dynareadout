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

#include "line.h"
#include "profiling.h"
#include <stdint.h>
#include <stdlib.h>

line_reader_t new_line_reader(FILE *file) {
  line_reader_t lr;
  lr.file = file;
  lr.line.extra = NULL;
  lr.bytes_read = 0;
  lr.extra_capacity = 0;
  return lr;
}

int read_line(line_reader_t *lr) {
  BEGIN_PROFILE_FUNC();

  lr->line_length = 0;

  if ((feof(lr->file) || ferror(lr->file)) &&
      lr->buffer_index >= lr->bytes_read) {
    END_PROFILE_FUNC();
    return 0;
  }

  char *cursor = lr->line.buffer;

  while (1) {
    if (lr->bytes_read == 0 || lr->buffer_index >= lr->bytes_read) {
      lr->bytes_read = fread(lr->buffer, 1, EXTRA_STRING_BUFFER_SIZE, lr->file);
      if (lr->bytes_read == 0) {
        break;
      }
      lr->buffer_index = 0;
    }

    while (lr->buffer_index < lr->bytes_read) {
      *cursor = lr->buffer[lr->buffer_index++];

      if (*cursor == '\r') {
        continue;
      } else if (*cursor == '\n') {
        break;
      }

      lr->line_length++;

      if (lr->line_length >= EXTRA_STRING_BUFFER_SIZE &&
          (lr->extra_capacity == 0 ||
           lr->line_length - EXTRA_STRING_BUFFER_SIZE > lr->extra_capacity)) {
        lr->extra_capacity += EXTRA_STRING_BUFFER_SIZE;
        lr->line.extra = realloc(lr->line.extra, lr->extra_capacity);
        cursor = &lr->line.extra[lr->line_length - EXTRA_STRING_BUFFER_SIZE];
      } else if (lr->line_length == EXTRA_STRING_BUFFER_SIZE) {
        cursor = lr->line.extra;
      } else {
        cursor++;
      }
    }

    if (*cursor == '\n') {
      break;
    }
  }

  *cursor = '\0';

  END_PROFILE_FUNC();
  return 1;
}