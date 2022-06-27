#ifndef BINOUT_RECORDS_H
#define BINOUT_RECORDS_H

typedef struct {
  char *path;
} binout_record_cd;

typedef struct {
  uint64_t type_id;
  char *name;
  uint8_t *data;
  uint64_t data_length;
} binout_record_data;

#endif