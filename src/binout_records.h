#ifndef BINOUT_RECORDS_H
#define BINOUT_RECORDS_H
#include "path.h"
#include <stdint.h>
#include <stdlib.h>

typedef struct {
  path_t path;
  size_t file_pos;
} binout_record_data;

typedef struct {
  uint64_t type_id;
  char *name;
  uint64_t data_length;
  binout_record_data *records;
  size_t records_size;
} binout_record_data_pointer;

#endif