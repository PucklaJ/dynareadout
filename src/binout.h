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

#ifndef BINOUT_H
#define BINOUT_H
#include "binout_directory.h"
#include "path.h"
#include <stdint.h>
#include <stdio.h>

/* The header of a binout file (The first 8 byte of a binout file)*/
typedef struct {
  uint8_t header_size;
  uint8_t record_length_field_size;
  uint8_t record_offset_field_size;
  uint8_t record_command_field_size;
  uint8_t record_typeid_field_size;
  uint8_t endianess;
  uint8_t float_format;
  uint8_t _unused;
} binout_header;

/* A binout file used to read data from a binout file*/
typedef struct {
  /* A data structure which holds the structure of the files*/
  binout_directory_t directory;

  FILE **file_handles;
  size_t num_file_handles;

  char **file_errors;
  size_t num_file_errors;

  /* Holds errors from read and other functions that are not open. If NULL no
   * error occurred*/
  char *error_string;
} binout_file;

#include "binout_read.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ----- Public functions ------ */

/* Open a binout file (or multiple files by globbing) and parse its records to
 * be ready to read data After opening it needs to be closed by binout_close*/
binout_file binout_open(const char *file_name);
/* Closes the binout file and deallocates all memory*/
void binout_close(binout_file *bin_file);
/* Returns the type id of the given variable. The type ids can be found in
 * binout_defines.h*/
uint8_t binout_get_type_id(binout_file *bin_file, const char *path_to_variable);
/* Returns whether a record with the given path and variable name exists*/
int binout_variable_exists(binout_file *bin_file, const char *path_to_variable);
/* Returns the entries under a given path. The return value needs to be
 * deallocated by binout_free_children*/
char **binout_get_children(binout_file *bin_file, const char *path,
                           size_t *num_children);
/* Free the allocated memory*/
void binout_free_children(char **children);
/* Returns all file errors as one string. This gives information about files
 * that failed in binout_open. These errors are not fatal. If the return value
 * is NULL, no error occurred. The return value needs to be deallocated by
 * free. The given path needs to be absolute.*/
char *binout_open_error(binout_file *bin_file);

/* A utility function that returns the number of dxxxxxx entries under a certain
 * path. Returns 0 if the path exists and no dxxxxxx entries have been found and
 * returns ~0 if the path does not exist or if the path points to files.*/
size_t binout_get_num_timesteps(const binout_file *bin_file, const char *path);

/* ----------------------------- */

/* ----- Private functions ----- */

/* Returns the command as a human readable string*/
const char *_binout_get_command_name(const uint64_t command);
/* Returns the size of a given type in bytes*/
uint8_t _binout_get_type_size(const uint64_t type_id);
/* Returns the type id as a human readable string*/
const char *_binout_get_type_name(const uint64_t type_id);
/* Add to the file_errors array:
 * Example: "test_data/binout0000: Failed to open file"*/
void _binout_add_file_error(binout_file *bin_file, const char *file_name,
                            const char *message);
/* Returns 1 if the given folder_name is a dxxxxxx folder and 0 otherwise. The
 * string can not be empty and must be null-terminated.*/
int _binout_is_d_string(const char *folder_name);
/* ----------------------------- */
#ifdef __cplusplus
}
#endif

#endif
