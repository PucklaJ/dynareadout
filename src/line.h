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

#ifndef LINE_H
#define LINE_H

#include "extra_string.h"
#include <stdio.h>

#define LINE_READER_BUFFER_SIZE (1024 * 1024) /* 1MB */

/* All the state for the read_line function*/
typedef struct {
  FILE *file;
  extra_string line; /* This stores the read line after a read_line call*/
  size_t
      line_length; /* The length of the line when considering inline comments*/
  size_t comment_index; /* An index into line to where a comment can be found.
                           Is ~0 if no comment has been found*/

  /* Internal variables used in read_line*/
  char *buffer; /* The file is read in LINE_READER_BUFFER_SIZE sized chunks*/
  size_t buffer_index;
  size_t bytes_read;
  size_t extra_capacity;
} line_reader_t;

#ifdef __cplusplus
extern "C" {
#endif

/* Initialise all variables of the line reader. Needs to be deallocated by
 * free_line_reader*/
line_reader_t new_line_reader(FILE *file);

/* Reads from file until it encounters the next new line and stores the
 * resulting string in line. Also supports carriage return. Returns 0 if the
 * file has been completely parsed and non 0 if the line can be processed*/
int read_line(line_reader_t *lr);

/* Frees all allocated memory of a line reader*/
void free_line_reader(line_reader_t lr);

#ifdef __cplusplus
}
#endif

#endif