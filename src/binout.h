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

#ifndef BINOUT_H
#define BINOUT_H
#include "binout_records.h"
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
  /* Holds one element for every variable
   * Holds file positions for every data record of a binout file*/
  binout_record_data_pointer **data_pointers;
  size_t *data_pointers_sizes;
  size_t *data_pointers_capacities;

  FILE **file_handles;
  size_t num_file_handles;

  char **file_errors;
  size_t num_file_errors;

  /* Holds errors from read and other functions that are not open. If NULL no
   * error occurred*/
  char *error_string;
} binout_file;

#ifdef __cplusplus
extern "C" {
#endif

/* ----- Public functions ------ */

/* Open a binout file (or multiple files by globbing) and parse its records to
 * be ready to read data After opening it needs to be closed by binout_close*/
binout_file binout_open(const char *file_name);
/* Closes the binout file and deallocates all memory*/
void binout_close(binout_file *bin_file);
/* A helper functions which prints all data records and where to find them*/
void binout_print_records(binout_file *bin_file);
/* Don't use this use one of the typed functions*/
void *binout_read(binout_file *bin_file, FILE *file_handle,
                  binout_record_data_pointer *dp, path_t *path_to_variable,
                  size_t type_size, size_t *data_size);
#define DEFINE_BINOUT_READ_TYPE_PROTO(c_type)                                  \
  c_type *binout_read_##c_type(                                                \
      binout_file *bin_file, const char *path_to_variable, size_t *data_size);
/* Read data from the file as int8_t. The type id of the data has to match*/
DEFINE_BINOUT_READ_TYPE_PROTO(int8_t)
/* Read data from the file as int16_t. The type id of the data has to match*/
DEFINE_BINOUT_READ_TYPE_PROTO(int16_t)
/* Read data from the file as int32_t. The type id of the data has to match*/
DEFINE_BINOUT_READ_TYPE_PROTO(int32_t)
/* Read data from the file as int64_t. The type id of the data has to match*/
DEFINE_BINOUT_READ_TYPE_PROTO(int64_t)
/* Read data from the file as uint8_t. The type id of the data has to match*/
DEFINE_BINOUT_READ_TYPE_PROTO(uint8_t)
/* Read data from the file as uint16_t. The type id of the data has to match*/
DEFINE_BINOUT_READ_TYPE_PROTO(uint16_t)
/* Read data from the file as uint32_t. The type id of the data has to match*/
DEFINE_BINOUT_READ_TYPE_PROTO(uint32_t)
/* Read data from the file as uint64_t. The type id of the data has to match*/
DEFINE_BINOUT_READ_TYPE_PROTO(uint64_t)
/* Read data from the file as float. The type id of the data has to match*/
DEFINE_BINOUT_READ_TYPE_PROTO(float)
/* Read data from the file as double. The type id of the data has to match*/
DEFINE_BINOUT_READ_TYPE_PROTO(double)
/* Returns the type id of the given variable. The type ids can be found in
 * binout_defines.h*/
uint64_t binout_get_type_id(binout_file *bin_file,
                            const char *path_to_variable);
/* Returns whether a record with the given path and variable name exists*/
int binout_variable_exists(binout_file *bin_file, const char *path_to_variable);
/* Returns the entries under a given path. The return value needs to be
 * deallocated by binout_free_children*/
char **binout_get_children(binout_file *bin_file, const char *path,
                           size_t *num_children);
/* Free the allocated memory*/
void binout_free_children(char **children, size_t num_children);
/* Returns all file errors as one string. This gives information about files
 * that failed in binout_open. These errors are not fatal. If the return value
 * is NULL, no error occurred. The return value needs to be deallocated by
 * free.*/
char *binout_open_error(binout_file *bin_file);

/* ----------------------------- */

/* ----- Private functions ----- */

/* Returns the command as a human readable string*/
const char *_binout_get_command_name(const uint64_t command);
/* Returns the size of a given type in bytes*/
uint8_t _binout_get_type_size(const uint64_t type_id);
/* Returns the type id as a human readable string*/
const char *_binout_get_type_name(const uint64_t type_id);
/* Returns the data pointer of a given path and variable name*/
binout_record_data_pointer *_binout_get_data_pointer(binout_file *bin_file,
                                                     size_t file_index,
                                                     path_t *path_to_variable);
binout_record_data_pointer *_binout_get_data_pointer2(binout_file *bin_file,
                                                      size_t file_index,
                                                      path_t *path,
                                                      const char *variable);
/* Returns the data record of a given path. Returns NULL if no record has been
 * found.*/
binout_record_data *_binout_get_data(binout_record_data_pointer *dp,
                                     path_t *path);
/* Add to the file_errors array:
 * Example: "test_data/binout0000: Failed to open file"*/
void _binout_add_file_error(binout_file *bin_file, const char *file_name,
                            const char *message);
/* Returns ~0 if it has not been found*/
size_t _binout_data_record_binary_search(binout_record_data *arr,
                                         size_t start_index, size_t end_index,
                                         const path_t *path);
/* Insert ele into arr so that arr is in ascending order. The ordering is based
 * of path_cmp.*/
void _binout_data_record_insert_sorted(binout_record_data **arr,
                                       size_t *arr_size, size_t *arr_cap,
                                       binout_record_data ele);
/* ----------------------------- */
#ifdef __cplusplus
}
#endif

#endif
