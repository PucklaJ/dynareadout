/***********************************************************************************
 *                         This file is part of dynareadout
 *                    https://github.com/PucklaMotzer09/dynareadout
 ***********************************************************************************
 * Copyright (c) 2022 PucklaMotzer09
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

#include "d3_buffer.h"
#ifdef _WIN32
#include <io.h>
#define F_OK 0
#define access _access
#else
#include <unistd.h>
#endif
#include <errno.h>
#include <stdlib.h>
#include <string.h>

d3_buffer d3_buffer_open(const char *root_file_name) {
  d3_buffer buffer;
  buffer.num_file_handles = 0;
  buffer.cur_file_handle = 0;
  buffer.cur_word = 0;
  buffer.file_handles = NULL;
  buffer.file_sizes = NULL;
  buffer.error_string = NULL;

  /* Store number 01 through 999*/
  char numbers[3];
  numbers[0] = '\0';

  const size_t root_len = strlen(root_file_name);
  /* Store the root name + numbers + '\0'*/
  char *file_name_buffer = malloc(root_len + 3 + 1);
  memcpy(file_name_buffer, root_file_name, root_len);
  memcpy(&file_name_buffer[root_len], numbers, 3);
  file_name_buffer[root_len + 3] = '\0';

  const char *patterns[2] = {"%d", "%02d"};

  size_t i = 0;
  while (i < 1000) {
    if (access(file_name_buffer, F_OK) != 0) {
      break;
    }

    FILE *file = fopen(file_name_buffer, "rb");
    if (!file) {
      const char *error_string = strerror(errno);
      buffer.error_string = malloc(root_len + 3 + 2 + strlen(error_string) + 1);
      sprintf(buffer.error_string, "%s: %s", file_name_buffer, error_string);
      free(file_name_buffer);
      return buffer;
    }

    /* Calculate the file size*/
    if (fseek(file, 0, SEEK_END) != 0) {
      /*TODO: Error*/
    }
    const size_t file_size = ftell(file);
    if (fseek(file, 0, SEEK_SET) != 0) {
      /*TODO: Error*/
    }

    buffer.num_file_handles++;
    buffer.file_handles =
        realloc(buffer.file_handles, buffer.num_file_handles * sizeof(FILE *));
    buffer.file_handles[buffer.num_file_handles - 1] = file;

    buffer.file_sizes =
        realloc(buffer.file_sizes, buffer.num_file_handles * sizeof(size_t));
    buffer.file_sizes[buffer.num_file_handles - 1] = file_size;

    /* Generate the new file name*/
    i++;
    sprintf(numbers, patterns[i < 10], i);
    memcpy(&file_name_buffer[root_len], numbers, 3);
  }

  free(file_name_buffer);

  if (buffer.num_file_handles == 0) {
    buffer.error_string = malloc(32 + root_len + 1);
    sprintf(buffer.error_string, "No files with the name %s do exist",
            root_file_name);
    return buffer;
  }

  /* Determine word_size by reading NDIM*/
  buffer.word_size = 4;
  uint32_t ndim32;
  d3_buffer_read_words_at(&buffer, &ndim32, 1, 15);

  buffer.word_size = 8;
  uint64_t ndim64;
  d3_buffer_read_words_at(&buffer, &ndim64, 1, 15);

  const int makes_sense32 = ndim32 >= 2 && ndim32 <= 7;
  const int makes_sense64 = ndim64 >= 2 && ndim64 <= 7;

  if ((!makes_sense32 && !makes_sense64) || (makes_sense32 && makes_sense64)) {
    buffer.error_string = malloc(27 + 1);
    sprintf(buffer.error_string, "The d3plot files are broken");
    return buffer;
  }

  /* The word size could be determined*/
  buffer.word_size = 4 + 4 * makes_sense64;

  /* Seek back to the beginning. We know that NDIM is inside the first file*/
  if (fseek(buffer.file_handles[0], 0, SEEK_SET) != 0) {
    /*TODO: Error*/
  }
  buffer.cur_word = 0;

  return buffer;
}

void d3_buffer_close(d3_buffer *buffer) {
  /* Close all files*/
  buffer->cur_file_handle = 0;
  while (buffer->cur_file_handle < buffer->num_file_handles) {
    fclose(buffer->file_handles[buffer->cur_file_handle++]);
  }

  free(buffer->file_handles);
  free(buffer->file_sizes);
  free(buffer->error_string);

  /* Set everything to NULL so that access after close does not crash*/
  buffer->file_handles = NULL;
  buffer->file_sizes = NULL;
  buffer->error_string = NULL;
  buffer->num_file_handles = 0;
  buffer->cur_word = 0;
}

void d3_buffer_read_words(d3_buffer *buffer, void *words, size_t num_words) {
  size_t cur_file_pos = ftell(buffer->file_handles[buffer->cur_file_handle]);
  uint8_t *words_ptr = (uint8_t *)words;

  if (buffer->file_sizes[buffer->cur_file_handle] - cur_file_pos >=
      num_words * buffer->word_size) {
    /* We can read everything from the current file*/
    if (fread(words, buffer->word_size, num_words,
              buffer->file_handles[buffer->cur_file_handle]) < num_words) {
      /* TODO: Error*/
    }
    buffer->cur_word += num_words;
    return;
  } else {
    size_t words_read = 0;
    while (words_read < num_words) {
      const size_t words_from_cur_file =
          (buffer->file_sizes[buffer->cur_file_handle] - cur_file_pos) /
          buffer->word_size;

      if (words_from_cur_file >= num_words - words_read) {
        if (fread(&words_ptr[words_read * buffer->word_size], buffer->word_size,
                  num_words - words_read,
                  buffer->file_handles[buffer->cur_file_handle]) <
            num_words - words_read) {
          /* TODO: Error*/
        }

        buffer->cur_word += num_words - words_read;
        words_read = num_words;
      } else {
        if (fread(&words_ptr[words_read * buffer->word_size], buffer->word_size,
                  words_from_cur_file,
                  buffer->file_handles[buffer->cur_file_handle]) <
            words_from_cur_file) {
          /* TODO: Error*/
        }

        buffer->cur_word += words_from_cur_file;
        words_read += words_from_cur_file;
        buffer->cur_file_handle++;
        /* TODO: Check if out of bounds*/
        cur_file_pos = 0;
        if (fseek(buffer->file_handles[buffer->cur_file_handle], 0, SEEK_SET) !=
            0) {
          /* TODO: Error*/
        }
      }
    }
  }
}

void d3_buffer_read_words_at(d3_buffer *buffer, void *words, size_t num_words,
                             size_t word_pos) {
  if (word_pos == 0) {
    buffer->cur_word = 0;
    buffer->cur_file_handle = 0;
    if (fseek(buffer->file_handles[0], 0, SEEK_SET) != 0) {
      /* TODO: Error*/
    }

    d3_buffer_read_words(buffer, words, num_words);
    return;
  }

  size_t pos_in_bytes = word_pos * buffer->word_size;
  buffer->cur_file_handle = 0;

  while (pos_in_bytes > 0) {
    const size_t file_size = buffer->file_sizes[buffer->cur_file_handle];

    if (file_size > pos_in_bytes) {
      if (fseek(buffer->file_handles[buffer->cur_file_handle], pos_in_bytes,
                SEEK_SET) != 0) {
        /* TODO: Error*/
      }
      pos_in_bytes = 0;
    } else {
      pos_in_bytes -= file_size;
      buffer->cur_file_handle++;
      /* TODO: Check if out of bounds*/

      if (pos_in_bytes == 0) {
        if (fseek(buffer->file_handles[buffer->cur_file_handle], 0, SEEK_SET) !=
            0) {
          /* TODO: Error*/
        }
      }
    }
  }

  buffer->cur_word = word_pos;
  d3_buffer_read_words(buffer, words, num_words);
}

void d3_buffer_read_double_word(d3_buffer *buffer, double *word) {
  if (buffer->word_size == 4) {
    float word32;
    d3_buffer_read_words(buffer, &word32, 1);
    *word = word32;
  } else {
    d3_buffer_read_words(buffer, word, 1);
  }
}

void d3_buffer_next_file(d3_buffer *buffer) {
  buffer->cur_word =
      buffer->file_sizes[buffer->cur_file_handle] / buffer->word_size;
  buffer->cur_file_handle++;
  /* TODO: Bounds check*/
  if (fseek(buffer->file_handles[buffer->cur_file_handle], 0, SEEK_SET) != 0) {
    /* TODO: Error*/
  }
}