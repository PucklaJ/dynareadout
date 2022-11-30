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

#ifndef PROFILING_H
#define PROFILING_H

#ifdef PROFILING

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

typedef struct {
  char const **execution_names;
  double *execution_times;
  size_t num_executions;

  char const **current_executions;
  size_t num_current_executions;
} profiling_context_t;

typedef struct {
  clock_t start_time;
  uint8_t should_end;
  size_t current_execution_index;
} execution_t;

extern profiling_context_t profiling_context;

#ifdef __cplusplus
extern "C" {
#endif

execution_t _BEGIN_PROFILE_SECTION(const char *name);
void _END_PROFILE_SECTION(const char *name, execution_t start_time);
void END_PROFILING(const char *out_file_name);

#ifdef __cplusplus
}
#endif

#define BEGIN_PROFILE_FUNC()                                                   \
  const execution_t func_profiling_start = _BEGIN_PROFILE_SECTION(__FUNCTION__)
#define END_PROFILE_FUNC()                                                     \
  _END_PROFILE_SECTION(__FUNCTION__, func_profiling_start)
#define BEGIN_PROFILE_SECTION(section_name)                                    \
  const execution_t section_name##_profiling_start =                           \
      _BEGIN_PROFILE_SECTION(#section_name)
#define END_PROFILE_SECTION(section_name)                                      \
  _END_PROFILE_SECTION(#section_name, section_name##_profiling_start)

#else

#define BEGIN_PROFILE_FUNC()
#define END_PROFILE_FUNC()
#define BEGIN_PROFILE_SECTION(section_name)
#define END_PROFILE_SECTION(section_name)
#define END_PROFILING(out_file_name)

#endif

#endif
