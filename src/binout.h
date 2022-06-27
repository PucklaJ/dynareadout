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

/* ----- Public functions ------ */

binout_file binout_open(const char *file_name);
void binout_close(binout_file *bin_file);
void binout_print_header(binout_file *bin_file);
void binout_print_records(binout_file *bin_file);

/* ----------------------------- */

/* ----- Private functions ----- */

const char *_binout_get_command_name(const uint64_t command);
uint8_t _binout_get_type_size(const uint64_t type_id);
const char *_binout_get_type_name(const uint64_t type_id);
char *_path_join(char *path, const char *element);
int _path_is_abs(const char *path);
char *_path_main(const char *path);

/* ----------------------------- */
#endif
