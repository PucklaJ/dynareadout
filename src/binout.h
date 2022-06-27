#ifndef BINOUT_H
#define BINOUT_H
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

  FILE *file_handle;
  const char *error_string;
} binout_file;

/* ----- Public functions ------ */

binout_file binout_open(const char *file_name);
void binout_close(binout_file *bin_file);
void binout_print_header(binout_file *bin_file);

/* ----------------------------- */

/* ----- Private functions ----- */
/* ----------------------------- */
#endif
