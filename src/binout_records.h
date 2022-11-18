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
  size_t records_capacity;     /* The number of allocated elements in records*/
} binout_record_data_pointer;

#endif