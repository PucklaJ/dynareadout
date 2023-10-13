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

#ifndef D3PLOT_ERROR_MACROS_H
#define D3PLOT_ERROR_MACROS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERROR_AND_NO_RETURN_PTR(msg)                                           \
  if (plot_file->error_string)                                                 \
    free(plot_file->error_string);                                             \
  plot_file->error_string = malloc(strlen(msg) + 1);                           \
  sprintf(plot_file->error_string, "%s", msg);
#define ERROR_AND_NO_RETURN_F_PTR(format_str, ...)                             \
  {                                                                            \
    char format_buffer[1024];                                                  \
    sprintf(format_buffer, format_str, __VA_ARGS__);                           \
    ERROR_AND_NO_RETURN_PTR(format_buffer);                                    \
  }
#define ERROR_AND_RETURN(msg)                                                  \
  d3_pointer_close(&plot_file.buffer, &d3_ptr);                                \
  if (plot_file.error_string)                                                  \
    free(plot_file.error_string);                                              \
  plot_file.error_string = malloc(strlen(msg) + 1);                            \
  sprintf(plot_file.error_string, "%s", msg);                                  \
  END_PROFILE_FUNC();                                                          \
  return plot_file;
#define ERROR_AND_RETURN_F(format_str, ...)                                    \
  {                                                                            \
    char format_buffer[1024];                                                  \
    sprintf(format_buffer, format_str, __VA_ARGS__);                           \
    ERROR_AND_RETURN(format_buffer);                                           \
  }
#define ERROR_AND_RETURN_PTR(msg)                                              \
  if (plot_file->error_string)                                                 \
    free(plot_file->error_string);                                             \
  plot_file->error_string = malloc(strlen(msg) + 1);                           \
  sprintf(plot_file->error_string, "%s", msg);                                 \
  return;
#define ERROR_AND_RETURN_F_PTR(format_str, ...)                                \
  {                                                                            \
    char format_buffer[1024];                                                  \
    sprintf(format_buffer, format_str, __VA_ARGS__);                           \
    ERROR_AND_RETURN_PTR(format_buffer);                                       \
  }
#define D3PLOT_CLEAR_ERROR_STRING()                                            \
  free(plot_file->error_string);                                               \
  plot_file->error_string = NULL;

#endif
