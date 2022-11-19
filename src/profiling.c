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

#include "profiling.h"
#include <stdlib.h>

profiling_context_t profiling_context = {NULL, NULL, 0};

void _END_PROFILE_SECTION(const char *name, clock_t start_time) {
  clock_t end_time = clock();
  const double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

  size_t index = ~0;
  size_t i = 0;
  while (i < profiling_context.num_execution_times) {
    if (strcmp(profiling_context.execution_times_names[i], name) == 0) {
      index = i;
      break;
    }

    i++;
  }

  if (index == ~0) {
    profiling_context.num_execution_times++;
    profiling_context.execution_times_names =
        realloc(profiling_context.execution_times_names,
                profiling_context.num_execution_times * sizeof(char *));
    profiling_context.execution_times =
        realloc(profiling_context.execution_times,
                profiling_context.num_execution_times * sizeof(double));
    index = profiling_context.num_execution_times - 1;
    profiling_context.execution_times[index] = 0.0;
    profiling_context.execution_times_names[index] = name;
  }

  profiling_context.execution_times[index] += elapsed_time;
}

void END_PROFILING() {
  free(profiling_context.execution_times_names);
  free(profiling_context.execution_times);
  profiling_context.execution_times_names = NULL;
  profiling_context.execution_times = NULL;
  profiling_context.num_execution_times = 0;
}