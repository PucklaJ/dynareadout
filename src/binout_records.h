#ifndef BINOUT_RECORDS_H
#define BINOUT_RECORDS_H
#include "path.h"
#include <stdint.h>
#include <stdlib.h>

/* Represents a data record of a binout file*/
typedef struct {
  path_t path;     /* The path inside the binout file*/
  size_t file_pos; /* At which file position the data segment of the record can
                      be found*/
} binout_record_data;

/* Holds all the data records of a variable*/
typedef struct {
  uint64_t type_id;     /* The type id of the variable*/
  char *name;           /* The name of the variable*/
  uint64_t data_length; /* The length of a data segment of a record in bytes*/
  binout_record_data *records; /* An array holding all records*/
  size_t records_size;         /* The number of elements in records*/
} binout_record_data_pointer;

#endif