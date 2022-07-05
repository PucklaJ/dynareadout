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

  uint8_t word_size; /* 4 byte for single precision and 8 byte for double
                        precision*/
  char *error_string;
} d3_buffer;

#ifdef __cplusplus
extern "C" {
#endif

/* Opens all d3plot files that belong to this root_file_name and also detects
 * the word_size*/
d3_buffer d3_buffer_open(const char *root_file_name);
/* Cleans everything up. Should be called sometime after d3_buffer_open*/
void d3_buffer_close(d3_buffer *buffer);
/* Read a given number of words from the current position. words already needs
 * to be allocated with at least num_words*word_size bytes*/
void d3_buffer_read_words(d3_buffer *buffer, void *words, size_t num_words);
/* Read a given number of words from the given position. words already needs
 * to be allocated with at least num_words*word_size bytes*/
void d3_buffer_read_words_at(d3_buffer *buffer, void *words, size_t num_words,
                             size_t word_pos);

#ifdef __cplusplus
}
#endif

#endif