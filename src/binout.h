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
  binout_record_data_pointer *data_pointers;
  uint64_t data_pointers_size;

  FILE *file_handle;
  /* This string is non NULL if an error occurred
   * It does NOT need to be deallocated*/
  const char *error_string;
} binout_file;

#ifdef __cplusplus
extern "C" {
#endif

/* ----- Public functions ------ */

/* Open a binout file and parse its records to be ready to read data
 * After opening it needs to be closed by binout_close*/
binout_file binout_open(const char *file_name);
/* Closes the binout file and deallocates all memory*/
void binout_close(binout_file *bin_file);
/* A helper functions which prints all data records and where to find them*/
void binout_print_records(binout_file *bin_file);
/* Don't use this use one of the typed functions*/
void *binout_read(binout_file *bin_file, binout_record_data_pointer *dp,
                  const char *path, const char *variable, size_t type_size,
                  size_t *data_size);
#define DEFINE_BINOUT_READ_TYPE_PROTO(c_type)                                  \
  c_type *binout_read_##c_type(binout_file *bin_file, const char *path,        \
                               const char *variable, size_t *data_size);
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
uint64_t binout_get_type_id(binout_file *bin_file, const char *path,
                            const char *variable);
/* Returns whether a record with the given path and variable name exists*/
int binout_variable_exists(binout_file *bin_file, const char *path,
                           const char *variable);
/* Returns the entries under a given path. The returns value needs to be
 * deallocated by binout_free_children*/
char **binout_get_children(binout_file *bin_file, const char *path,
                           size_t *num_children);
/* Free the allocated memory*/
void binout_free_children(char **children, size_t num_children);

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
                                                     path_t *path,
                                                     const char *variable);
/* Returns the data record of a given path*/
binout_record_data *_binout_get_data(binout_record_data_pointer *dp,
                                     path_t *path);

/* ----------------------------- */
#ifdef __cplusplus
}
#endif

#endif
