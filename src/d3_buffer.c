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

#include "d3_buffer.h"
#include "path.h"
#include "profiling.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define ERROR_AND_RETURN_BUFFER(msg)                                           \
  if (buffer.error_string)                                                     \
    free(buffer.error_string);                                                 \
  buffer.error_string = malloc(strlen(msg) + 1);                               \
  sprintf(buffer.error_string, "%s", msg);                                     \
  END_PROFILE_FUNC();                                                          \
  return buffer;
#define ERROR_AND_RETURN_BUFFER_F(format_str, ...)                             \
  {                                                                            \
    char format_buffer[1024];                                                  \
    sprintf(format_buffer, format_str, __VA_ARGS__);                           \
    ERROR_AND_RETURN_BUFFER(format_buffer);                                    \
  }
#define ERROR_AND_NO_RETURN_BUFFER_PTR(msg)                                    \
  if (buffer->error_string)                                                    \
    free(buffer->error_string);                                                \
  buffer->error_string = malloc(strlen(msg) + 1);                              \
  sprintf(buffer->error_string, "%s", msg);
#define ERROR_AND_RETURN_BUFFER_PTR(msg)                                       \
  ERROR_AND_NO_RETURN_BUFFER_PTR(msg);                                         \
  END_PROFILE_FUNC();                                                          \
  return;
#define ERROR_AND_RETURN_BUFFER_F_PTR(format_str, ...)                         \
  {                                                                            \
    char format_buffer[1024];                                                  \
    sprintf(format_buffer, format_str, __VA_ARGS__);                           \
    ERROR_AND_RETURN_BUFFER_PTR(format_buffer);                                \
  }
#define ERROR_AND_NO_RETURN_BUFFER_F_PTR(format_str, ...)                      \
  {                                                                            \
    char format_buffer[1024];                                                  \
    sprintf(format_buffer, format_str, __VA_ARGS__);                           \
    ERROR_AND_NO_RETURN_BUFFER_PTR(format_buffer);                             \
  }

d3_buffer d3_buffer_open(const char *root_file_name) {
  BEGIN_PROFILE_FUNC();

  d3_buffer buffer;
  buffer.num_files = 0;
  buffer.cur_file = 0;
  buffer.cur_word = 0;
  buffer.first_open_file = 0;
  buffer.last_open_file = ~0;
  buffer.files = NULL;
  buffer.error_string = NULL;

  /* Allocate 1000 d3_files*/
  buffer.files = malloc(1000 * sizeof(d3_file));
  buffer.files[0].index_string[0] = '\0';

  buffer.root_file_name_length = strlen(root_file_name);
  /* Store the root file name which is used to open unopened d3plot files in the
   * future*/
  buffer.root_file_name = malloc(buffer.root_file_name_length + 3 + 1);
  memcpy(buffer.root_file_name, root_file_name, buffer.root_file_name_length);

  /* Store the root name + numbers + '\0'*/
  char *file_name_buffer = malloc(buffer.root_file_name_length + 3 + 1);
  memcpy(file_name_buffer, root_file_name, buffer.root_file_name_length);
  memcpy(&file_name_buffer[buffer.root_file_name_length],
         buffer.files[0].index_string, 3);
  file_name_buffer[buffer.root_file_name_length + 3] = '\0';

  const char *patterns[2] = {"%zu", "%02zu"};

  size_t i = 0;
  while (i < 1000) {
    if (!path_is_file(file_name_buffer)) {
      break;
    }

    /* Store number 01 through 999*/
    buffer.files[i].file_size = path_get_file_size(file_name_buffer);
    buffer.files[i].file_handle = fopen(file_name_buffer, "rb");
    if (!buffer.files[i].file_handle) {
      /* If the error is not 'Too many open files'*/
      if (errno != EMFILE) {
        const char *error_string = strerror(errno);
        buffer.error_string = malloc(buffer.root_file_name_length + 3 + 2 +
                                     strlen(error_string) + 1);
        sprintf(buffer.error_string, "%s: %s", file_name_buffer, error_string);
        free(file_name_buffer);

        END_PROFILE_FUNC();
        return buffer;
      }
    } else {
      buffer.last_open_file = i;
    }

    buffer.num_files++;

    /* Generate the new file name*/
    i++;
    sprintf(buffer.files[i].index_string, patterns[i < 10], i);
    memcpy(&file_name_buffer[buffer.root_file_name_length],
           buffer.files[i].index_string, 3);
  }

  free(file_name_buffer);

  if (buffer.num_files == 0) {
    ERROR_AND_RETURN_BUFFER_F("No files with the name %s do exist",
                              root_file_name);
  }

  if (buffer.last_open_file == ~0) {
    ERROR_AND_RETURN_BUFFER(
        "No files could be opened because too many files are open");
  }

  /* Shrink to fit*/
  buffer.files = realloc(buffer.files, buffer.num_files * sizeof(d3_file));

  /* Determine word_size by reading NDIM*/
  buffer.word_size = 4;
  uint32_t ndim32;
  d3_buffer_read_words_at(&buffer, &ndim32, 1, 15);
  if (buffer.error_string) {
    ndim32 = 0;
    free(buffer.error_string);
    buffer.error_string = NULL;
  }

  buffer.word_size = 8;
  uint64_t ndim64;
  d3_buffer_read_words_at(&buffer, &ndim64, 1, 15);
  if (buffer.error_string) {
    ndim64 = 0;
    free(buffer.error_string);
    buffer.error_string = NULL;
  }

  const int makes_sense32 = ndim32 >= 2 && ndim32 <= 7;
  const int makes_sense64 = ndim64 >= 2 && ndim64 <= 7;

  if ((!makes_sense32 && !makes_sense64) || (makes_sense32 && makes_sense64)) {
    ERROR_AND_RETURN_BUFFER("The d3plot files are broken");
  }

  /* The word size could be determined*/
  buffer.word_size = 4 + 4 * makes_sense64;

  /* Seek back to the beginning. We know that NDIM is inside the first file*/
  if (fseek(buffer.files[0].file_handle, 0, SEEK_SET) != 0) {
    ERROR_AND_RETURN_BUFFER("Failed to seek back after determining word size");
  }
  buffer.cur_word = 0;

  END_PROFILE_FUNC();
  return buffer;
}

void d3_buffer_close(d3_buffer *buffer) {
  BEGIN_PROFILE_FUNC();

  /* Close all files*/
  size_t i = 0;
  while (i < buffer->num_files) {
    if (buffer->files[i].file_handle)
      fclose(buffer->files[i].file_handle);
    i++;
  }

  free(buffer->files);
  free(buffer->error_string);
  free(buffer->root_file_name);

  /* Set everything to NULL so that access after close does not crash*/
  buffer->files = NULL;
  buffer->error_string = NULL;
  buffer->root_file_name = NULL;
  buffer->root_file_name_length = 0;
  buffer->num_files = 0;
  buffer->cur_word = 0;
  buffer->cur_file = 0;

  END_PROFILE_FUNC();
}

void d3_buffer_read_words(d3_buffer *buffer, void *words, size_t num_words) {
  BEGIN_PROFILE_FUNC();

  long cur_file_pos = ftell(buffer->files[buffer->cur_file].file_handle);
  uint8_t *words_ptr = (uint8_t *)words;

  if (buffer->files[buffer->cur_file].file_size - cur_file_pos >=
      num_words * buffer->word_size) {
    /* We can read everything from the current file*/
    if (fread(words, buffer->word_size, num_words,
              buffer->files[buffer->cur_file].file_handle) < num_words) {
      ERROR_AND_RETURN_BUFFER_PTR("Read Error");
    }
    buffer->cur_word += num_words;

    END_PROFILE_FUNC();
    return;
  } else {
    /* Read from as much files as necessary to read all number of words*/
    size_t words_read = 0;
    while (words_read < num_words) {

      /* How much words can be read from the current file*/
      const size_t words_from_cur_file =
          (buffer->files[buffer->cur_file].file_size - cur_file_pos) /
          buffer->word_size;

      const size_t words_left = num_words - words_read;
      if (words_from_cur_file >= words_left) {
        /* We can read all of the rest of the words from the current file*/
        if (fread(&words_ptr[words_read * buffer->word_size], buffer->word_size,
                  words_left,
                  buffer->files[buffer->cur_file].file_handle) < words_left) {
          ERROR_AND_RETURN_BUFFER_PTR("Read Error");
        }

        buffer->cur_word += words_left;
        words_read = num_words;
        break;
      } else {

        if (words_from_cur_file != 0) {
          /* Read the rest of the current file*/
          if (fread(&words_ptr[words_read * buffer->word_size],
                    buffer->word_size, words_from_cur_file,
                    buffer->files[buffer->cur_file].file_handle) <
              words_from_cur_file) {
            ERROR_AND_RETURN_BUFFER_PTR("Read Error");
          }

          buffer->cur_word += words_from_cur_file;
          words_read += words_from_cur_file;
        }

        if (!d3_buffer_next_file(buffer)) {
          if (buffer->error_string) {
            ERROR_AND_RETURN_BUFFER_F_PTR(
                "Error when switching to next file: %s", buffer->error_string);
          }

          ERROR_AND_RETURN_BUFFER_PTR("Requested too much words");
        }

        cur_file_pos = 0;
      }
    }
  }

  END_PROFILE_FUNC();
}

void d3_buffer_read_words_at(d3_buffer *buffer, void *words, size_t num_words,
                             size_t word_pos) {
  BEGIN_PROFILE_FUNC();

  d3_buffer_seek(buffer, word_pos);
  if (buffer->error_string) {
    ERROR_AND_RETURN_BUFFER_F_PTR("Failed to seek the buffer: %s",
                                  buffer->error_string);
  }

  d3_buffer_read_words(buffer, words, num_words);

  END_PROFILE_FUNC();
}

void d3_buffer_read_double_word(d3_buffer *buffer, double *word) {
  BEGIN_PROFILE_FUNC();

  if (buffer->word_size == 4) {
    float word32;
    d3_buffer_read_words(buffer, &word32, 1);
    *word = word32;
  } else {
    d3_buffer_read_words(buffer, word, 1);
  }

  END_PROFILE_FUNC();
}

void d3_buffer_read_vec3(d3_buffer *buffer, double *words) {
  BEGIN_PROFILE_FUNC();

  if (buffer->word_size == 4) {
    float words32[3];
    d3_buffer_read_words(buffer, words32, 3);
    words[0] = words32[0];
    words[1] = words32[1];
    words[2] = words32[2];
  } else {
    d3_buffer_read_words(buffer, words, 3);
  }

  END_PROFILE_FUNC();
}

void d3_buffer_skip_words(d3_buffer *buffer, size_t num_words) {
  BEGIN_PROFILE_FUNC();
  d3_buffer_seek(buffer, buffer->cur_word + num_words);
  END_PROFILE_FUNC();
}

void d3_buffer_skip_bytes(d3_buffer *buffer, size_t num_bytes) {
  BEGIN_PROFILE_FUNC();
  d3_buffer_skip_words(buffer, num_bytes / buffer->word_size);
  END_PROFILE_FUNC();
}

int d3_buffer_next_file(d3_buffer *buffer) {
  BEGIN_PROFILE_FUNC();

  const long cur_file_pos = ftell(buffer->files[buffer->cur_file].file_handle);
  buffer->cur_word +=
      (buffer->files[buffer->cur_file].file_size - cur_file_pos) /
      buffer->word_size;
  buffer->cur_file++;

  if (buffer->cur_file == buffer->num_files) {
    END_PROFILE_FUNC();
    return 0;
  }

  /* Open the next file if it isn't open and close the first opened file*/
  if (!buffer->files[buffer->cur_file].file_handle) {
    fclose(buffer->files[buffer->first_open_file].file_handle);
    buffer->files[buffer->first_open_file].file_handle = NULL;
    buffer->first_open_file++;
    buffer->last_open_file++;

    memcpy(&buffer->root_file_name[buffer->root_file_name_length],
           buffer->files[buffer->cur_file].index_string, 4);

    buffer->files[buffer->cur_file].file_handle =
        fopen(buffer->root_file_name, "rb");
    if (!buffer->files[buffer->cur_file].file_handle) {
      ERROR_AND_NO_RETURN_BUFFER_F_PTR("Failed to open next file(%zu): %s: %s",
                                       buffer->cur_file, buffer->root_file_name,
                                       strerror(errno));
      END_PROFILE_FUNC();
      return 0;
    }
  }

  if (fseek(buffer->files[buffer->cur_file].file_handle, 0, SEEK_SET) != 0) {
    ERROR_AND_NO_RETURN_BUFFER_PTR("Seek Error");
    END_PROFILE_FUNC();
    return 0;
  }

  END_PROFILE_FUNC();
  return 1;
}

void d3_buffer_seek(d3_buffer *buffer, size_t word_pos) {
  BEGIN_PROFILE_FUNC();

  buffer->cur_word = word_pos;

  /* Determine to which file the word position points*/
  size_t i = 0;
  while (i < buffer->num_files) {
    const size_t file_size_words =
        ((size_t)buffer->files[i].file_size) / buffer->word_size;
    if (file_size_words > word_pos) {
      break;
    }
    word_pos -= file_size_words;

    i++;
  }

  if (i == buffer->num_files) {
    /* Out of bounds*/
    ERROR_AND_RETURN_BUFFER_PTR("Out of bounds");
  }

  buffer->cur_file = i;
  /* If the file is not yet open close enough files so that it can be opened*/
  if (!buffer->files[buffer->cur_file].file_handle) {
    /* Wether all files need to be closed and opened at the current file,
     * because moving towards the file would open too many files*/
    int out_of_range = 0;
    if (buffer->first_open_file < buffer->last_open_file) {
      if (buffer->cur_file > buffer->last_open_file) {
        if (buffer->cur_file - buffer->last_open_file >
            buffer->last_open_file - buffer->first_open_file + 1) {
          out_of_range = 1;
        }
      } else {
        if (buffer->first_open_file - buffer->cur_file >
            buffer->last_open_file - buffer->first_open_file + 1) {
          out_of_range = 1;
        }
      }
    }

    if (out_of_range) {
      /* Close all files*/
      size_t file_range = 0;
      i = buffer->first_open_file;
      while (i <= buffer->last_open_file) {
        fclose(buffer->files[i].file_handle);
        buffer->files[i].file_handle = NULL;

        file_range++;
        i++;
      }

      /* Make sure that first_open_file < last_open_file*/
      size_t start_offset = 0;
      if (buffer->num_files - buffer->cur_file < file_range) {
        start_offset = buffer->cur_file - (buffer->num_files - file_range);
      }

      /* Open all files from the new position*/
      i = buffer->cur_file - start_offset;
      buffer->first_open_file = i;
      while (file_range > 0) {
        memcpy(&buffer->root_file_name[buffer->root_file_name_length],
               buffer->files[i].index_string, 4);
        buffer->files[i].file_handle = fopen(buffer->root_file_name, "rb");
        if (!buffer->files[i].file_handle) {
          ERROR_AND_RETURN_BUFFER_F_PTR("Error when opening file(%zu): %s: %s",
                                        i, buffer->root_file_name,
                                        strerror(errno));
        }

        i++;
        file_range--;
      }
      buffer->last_open_file = i - 1;
    } else {
      if (buffer->first_open_file < buffer->last_open_file) {
        if (buffer->cur_file < buffer->first_open_file) {
          const size_t files_to_close =
              buffer->first_open_file - buffer->cur_file;
          /* Close backwards*/
          i = buffer->last_open_file;
          while (i > buffer->last_open_file - files_to_close) {
            fclose(buffer->files[i].file_handle);
            buffer->files[i].file_handle = NULL;
            i--;
          }
          buffer->last_open_file -= files_to_close;

          /* Open backwards*/
          i = buffer->first_open_file;
          while (i >= buffer->cur_file) {
            memcpy(&buffer->root_file_name[buffer->root_file_name_length],
                   buffer->files[i].index_string, 4);

            buffer->files[i].file_handle = fopen(buffer->root_file_name, "rb");
            if (!buffer->files[i].file_handle) {
              ERROR_AND_RETURN_BUFFER_F_PTR(
                  "Error when opening file(%zu): %s: %s", i,
                  buffer->root_file_name, strerror(errno));
            }
            i--;
          }
          buffer->first_open_file = buffer->cur_file;
        } else {
          const size_t files_to_close =
              buffer->cur_file - buffer->last_open_file;

          /* Close forwards*/
          i = buffer->first_open_file;
          while (i < buffer->first_open_file + files_to_close) {
            fclose(buffer->files[i].file_handle);
            buffer->files[i].file_handle = NULL;
            i++;
          }
          buffer->first_open_file += files_to_close;

          /* Open forwards*/
          i = buffer->last_open_file;
          while (i <= buffer->cur_file) {
            memcpy(&buffer->root_file_name[buffer->root_file_name_length],
                   buffer->files[i].index_string, 4);

            buffer->files[i].file_handle = fopen(buffer->root_file_name, "rb");
            if (!buffer->files[i].file_handle) {
              ERROR_AND_RETURN_BUFFER_F_PTR(
                  "Error when opening file(%zu): %s: %s", i,
                  buffer->root_file_name, strerror(errno));
            }
            i++;
          }
          buffer->last_open_file = buffer->cur_file;
        }
      }
    }
  }

  /* Seek to the correct location in the file*/
  const long seek_pos = (long)(word_pos * (size_t)buffer->word_size);

  if (fseek(buffer->files[buffer->cur_file].file_handle, seek_pos, SEEK_SET) !=
      0) {
    ERROR_AND_RETURN_BUFFER_PTR("Seek Error");
  }

  END_PROFILE_FUNC();
}