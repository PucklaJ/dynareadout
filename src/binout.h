#ifndef BINOUT_H
#define BINOUT_H
#include "binout_records.h"
#include <stdint.h>
#include <stdio.h>

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

typedef struct {
  binout_header header;
  uint64_t symbol_table_offset;
  binout_record_data_pointer *data_pointers;
  uint64_t data_pointers_size;

  FILE *file_handle;
  const char *error_string;
} binout_file;

#ifdef __cplusplus
extern "C" {
#endif

/* ----- Public functions ------ */

binout_file binout_open(const char *file_name);
void binout_close(binout_file *bin_file);
void binout_print_header(binout_file *bin_file);
void binout_print_records(binout_file *bin_file);
void *binout_read(binout_file *bin_file, binout_record_data_pointer *dp,
                  const char *path, const char *variable, size_t type_size,
                  size_t *data_size);
#define DEFINE_BINOUT_READ_TYPE_PROTO(c_type)                                  \
  c_type *binout_read_##c_type(binout_file *bin_file, const char *path,        \
                               const char *variable, size_t *data_size);
DEFINE_BINOUT_READ_TYPE_PROTO(int8_t)
DEFINE_BINOUT_READ_TYPE_PROTO(int16_t)
DEFINE_BINOUT_READ_TYPE_PROTO(int32_t)
DEFINE_BINOUT_READ_TYPE_PROTO(int64_t)
DEFINE_BINOUT_READ_TYPE_PROTO(uint8_t)
DEFINE_BINOUT_READ_TYPE_PROTO(uint16_t)
DEFINE_BINOUT_READ_TYPE_PROTO(uint32_t)
DEFINE_BINOUT_READ_TYPE_PROTO(uint64_t)
DEFINE_BINOUT_READ_TYPE_PROTO(float)
DEFINE_BINOUT_READ_TYPE_PROTO(double)
uint64_t binout_get_type_id(binout_file *bin_file, const char *path,
                            const char *variable);
int binout_variable_exists(binout_file *bin_file, const char *path,
                           const char *variable);
char **binout_get_children(binout_file *bin_file, const char *path,
                           size_t *num_children);
void binout_free_children(char **children, size_t num_children);

/* ----------------------------- */

/* ----- Private functions ----- */

const char *_binout_get_command_name(const uint64_t command);
uint8_t _binout_get_type_size(const uint64_t type_id);
const char *_binout_get_type_name(const uint64_t type_id);
binout_record_data_pointer *_binout_get_data_pointer(binout_file *bin_file,
                                                     const char *path,
                                                     const char *variable);
binout_record_data *_binout_get_data(binout_record_data_pointer *dp,
                                     const char *path);

/* ----------------------------- */
#ifdef __cplusplus
}
#endif

#endif
