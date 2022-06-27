#ifndef BINOUT_RECORDS_H
#define BINOUT_RECORDS_H
#include <stdint.h>

typedef struct {
  uint64_t type_id;
  char *path;
  uint64_t data_length;
  uint8_t **data;
  uint64_t data_size;
} binout_record_data_pointer;

#endif