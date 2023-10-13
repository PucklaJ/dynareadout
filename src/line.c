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

#define KEY_COMMENT '$'

line_reader_t new_line_reader(FILE *file) {
  BEGIN_PROFILE_FUNC();

  line_reader_t lr;
  lr.buffer = malloc(LINE_READER_BUFFER_SIZE);
  lr.file = file;
  lr.line.extra = NULL;
  lr.bytes_read = 0;
  lr.extra_capacity = 0;

  END_PROFILE_FUNC();
  return lr;
}

int read_line(line_reader_t *lr) {
  BEGIN_PROFILE_FUNC();

  lr->line_length = 0;
  lr->comment_index = (size_t)~0;

  if ((feof(lr->file) || ferror(lr->file)) &&
      lr->buffer_index >= lr->bytes_read) {
    END_PROFILE_FUNC();
    return 0;
  }

  /* Points where to write the next character*/
  char *cursor = lr->line.buffer;

  /* Loop until a new line has been encountered or EOF is reached or an error
   * occurred*/
  while (1) {
    /* Only read from the file if the buffer has been completely read or it's
     * the first read_line call*/
    if (lr->bytes_read == 0 || lr->buffer_index >= lr->bytes_read) {
      /* Read the file in LINE_READER_BUFFER_SIZE sized chunks*/
      lr->bytes_read = fread(lr->buffer, 1, LINE_READER_BUFFER_SIZE, lr->file);
      if (lr->bytes_read == 0) {
        break;
      }
      lr->buffer_index = 0;
    }

    /* Read bytes from the buffer*/
    while (lr->buffer_index < lr->bytes_read) {
      *cursor = lr->buffer[lr->buffer_index++];

      if (lr->comment_index == (size_t)~0 && *cursor == KEY_COMMENT) {
        lr->comment_index = lr->line_length;
      }

      /* Ignore carriage return*/
      if (*cursor == '\r') {
        continue;
      } else if (*cursor == '\n') {
        break;
      }

      lr->line_length++;

      /* Switch to extra if too much characters have been read and make sure
       * that enough memory is allocated*/
      if (lr->line_length >= EXTRA_STRING_BUFFER_SIZE &&
          lr->line_length + 1 - EXTRA_STRING_BUFFER_SIZE > lr->extra_capacity) {
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

void free_line_reader(line_reader_t lr) {
  BEGIN_PROFILE_FUNC();

  free(lr.buffer);

  END_PROFILE_FUNC();
}