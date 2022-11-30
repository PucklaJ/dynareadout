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

#ifndef D3_BUFFER_H
#define D3_BUFFER_H
#include <stdint.h>
#include <stdio.h>

/* Represents a whole family of d3 files*/
typedef struct {
  FILE **file_handles;
  size_t *file_sizes;
  size_t num_file_handles;
  size_t cur_file_handle;
  size_t cur_word;

  uint8_t word_size; /* 4 byte for single precision and 8 byte for double
                        precision*/
  char *error_string;
} d3_buffer;

#ifdef __cplusplus
extern "C" {
#endif

/* Opens all d3plot files that belong to this root_file_name and also detects
 * the word_size. Sets error_string on error*/
d3_buffer d3_buffer_open(const char *root_file_name);
/* Cleans everything up. Should be called sometime after d3_buffer_open*/
void d3_buffer_close(d3_buffer *buffer);
/* Read a given number of words from the current position. words already needs
 * to be allocated with at least num_words*word_size bytes. Sets error_string on
 * error*/
void d3_buffer_read_words(d3_buffer *buffer, void *words, size_t num_words);
/* Read a given number of words from the given position. words already needs
 * to be allocated with at least num_words*word_size bytes. Sets error_string on
 * error*/
void d3_buffer_read_words_at(d3_buffer *buffer, void *words, size_t num_words,
                             size_t word_pos);
/* Sets error_string on error*/
void d3_buffer_read_double_word(d3_buffer *buffer, double *word);
/* Sets error_string on error*/
void d3_buffer_read_vec3(d3_buffer *buffer, double *words);
/* Skip an arbitrary amount of words. Also handles skips across multiple files.
 * Sets error_string on error*/
void d3_buffer_skip_words(d3_buffer *buffer, size_t num_words);
/* Similar to d3_buffer_skip_words, but it skips bytes instead of words. Try to
 * use the words one. This function ist just for some cases where the number of
 * bytes is always the same no matter what the words size is*/
void d3_buffer_skip_bytes(d3_buffer *buffer, size_t num_bytes);
/* Skip to the next file. Especially needed a the end of the first file when
 * done with all the headers. Sets error_string on error*/
int d3_buffer_next_file(d3_buffer *buffer);

#ifdef __cplusplus
}
#endif

#endif