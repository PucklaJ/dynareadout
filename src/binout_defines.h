/***********************************************************************************
 *                         This file is part of dynareadout
 *                    https://github.com/PucklaJ/dynareadout
 ***********************************************************************************
 * Copyright (c) 2022 Jonas Pucher
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

#ifndef BINOUT_DEFINES_H
#define BINOUT_DEFINES_H
#include "string_builder.h"
#include <limits.h>

#define BINOUT_HEADER_BIG_ENDIAN 0
#define BINOUT_HEADER_LITTLE_ENDIAN 1
#define BINOUT_HEADER_FLOAT_IEEE 0

#define BINOUT_COMMAND_NULL 1
#define BINOUT_COMMAND_CD 2
#define BINOUT_COMMAND_DATA 3
#define BINOUT_COMMAND_VARIABLE 4
#define BINOUT_COMMAND_BEGINSYMBOLTABLE 5
#define BINOUT_COMMAND_ENDSYMBOLTABLE 6
#define BINOUT_COMMAND_SYMBOLTABLEOFFSET 7

#define BINOUT_TYPE_INT8 1
#define BINOUT_TYPE_INT16 2
#define BINOUT_TYPE_INT32 3
#define BINOUT_TYPE_INT64 4
#define BINOUT_TYPE_UINT8 5
#define BINOUT_TYPE_UINT16 6
#define BINOUT_TYPE_UINT32 7
#define BINOUT_TYPE_UINT64 8
#define BINOUT_TYPE_FLOAT32 9
#define BINOUT_TYPE_FLOAT64 10
#define BINOUT_TYPE_INVALID UCHAR_MAX

#define BINOUT_DATA_NAME_LENGTH 1
#define BINOUT_DATA_POINTER_PREALLOC 100
#define BINOUT_DATA_POINTER_ALLOC_ADV 10
#define BINOUT_DATA_RECORD_PREALLOC 1000
#define BINOUT_DATA_RECORD_ALLOC_ADV 100

#define NEW_ERROR_STRING(message)                                              \
  if (bin_file->error_string)                                                  \
    free(bin_file->error_string);                                              \
  bin_file->error_string = string_clone(message)

#define NEW_ERROR_STRING_F(format_str, ...)                                    \
  char format_buffer[1024];                                                    \
  sprintf(format_buffer, format_str, __VA_ARGS__);                             \
  NEW_ERROR_STRING(format_buffer)

#define BINOUT_CLEAR_ERROR_STRING()                                            \
  free(bin_file->error_string);                                                \
  bin_file->error_string = NULL;

#endif
