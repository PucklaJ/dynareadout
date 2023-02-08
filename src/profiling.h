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
#include <stdio.h>
#include <string.h>
#include <time.h>

#define GET_SUB_EXECUTION(stack, index)                                        \
  (((profiling_stack_t *)stack->sub_executions)[index])

typedef struct {
  const char *execution_name;
  double execution_time;
  double profiling_time;

  void *sub_executions;
  size_t num_sub_executions;
  void *parent;
} profiling_stack_t;

typedef struct {
  uint8_t disable;
  uint8_t disable_stacking;

  profiling_stack_t *current_stack;
  profiling_stack_t *execution_stacks;
  size_t num_execution_stacks;
} profiling_context_t;

typedef struct {
  clock_t start_time;
  clock_t profiling_start_time;
  uint8_t should_end;
  profiling_stack_t *current_stack;
} profiling_execution_t;

extern profiling_context_t profiling_context;

#ifdef __cplusplus
extern "C" {
#endif

profiling_execution_t _BEGIN_PROFILE_SECTION(const char *name);
void _END_PROFILE_SECTION(const char *name, profiling_execution_t start_time);
void END_PROFILING(const char *out_file_name);

void fprint_profiling_stack(FILE *file, const profiling_stack_t *stack,
                            int level);
void free_profiling_stack(profiling_stack_t *stack);

#ifdef __cplusplus
}
#endif

#define BEGIN_PROFILE_FUNC()                                                   \
  const profiling_execution_t func_profiling_start =                           \
      _BEGIN_PROFILE_SECTION(__FUNCTION__)
#define END_PROFILE_FUNC()                                                     \
  _END_PROFILE_SECTION(__FUNCTION__, func_profiling_start)
#define BEGIN_PROFILE_SECTION(section_name)                                    \
  const profiling_execution_t section_name##_profiling_start =                 \
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
