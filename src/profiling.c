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

#include "profiling.h"
#include "binary_search.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

profiling_context_t profiling_context = {NULL, NULL, 0, NULL, 0};

signed long partition_execution_times(char const ***names, double **times,
                                      signed long low, signed long high) {
  const double pivot = (*times)[high];

  signed long i = low - 1, j = low;

  while (j < high) {
    if ((*times)[j] > pivot) {
      i++;
      /* Swap i and j*/
      const char *temp_name = (*names)[i];
      (*names)[i] = (*names)[j];
      (*names)[j] = temp_name;
      const double temp_time = (*times)[i];
      (*times)[i] = (*times)[j];
      (*times)[j] = temp_time;
    }

    j++;
  }

  /* Swap i + 1 and high*/
  const char *temp_name = (*names)[i + 1];
  (*names)[i + 1] = (*names)[high];
  (*names)[high] = temp_name;
  const double temp_time = (*times)[i + 1];
  (*times)[i + 1] = (*times)[high];
  (*times)[high] = temp_time;

  return i + 1;
}

void quick_sort_execution_times(char const ***names, double **times,
                                signed long low, signed long high) {
  if (low < high) {
    const signed long pi = partition_execution_times(names, times, low, high);

    quick_sort_execution_times(names, times, low, pi - 1);
    quick_sort_execution_times(names, times, pi + 1, high);
  }
}

execution_t _BEGIN_PROFILE_SECTION(const char *name) {
  /* Only start profiling if it isn't already getting profiled (to avoid
   * recursion issues)*/
  execution_t rv = {0, 0, ~0};
  size_t i = 0, index = ~0;
  while (i < profiling_context.num_current_executions) {
    if (index == ~0 && !profiling_context.current_executions[i]) {
      index = i;
    } else if (profiling_context.current_executions[i] &&
               strcmp(profiling_context.current_executions[i], name) == 0) {
      return rv;
    }

    i++;
  }

  if (index == ~0) {
    profiling_context.num_current_executions++;
    profiling_context.current_executions = realloc(
        profiling_context.current_executions,
        profiling_context.num_current_executions * sizeof(const char *));
    index = profiling_context.num_current_executions - 1;
  }

  profiling_context.current_executions[index] = name;

  rv.should_end = 1;
  rv.current_execution_index = index;
  rv.start_time = clock();

  return rv;
}

void _END_PROFILE_SECTION(const char *name, execution_t start) {
  if (!start.should_end) {
    return;
  }

  clock_t end_time = clock();
  const double elapsed_time =
      (double)(end_time - start.start_time) / CLOCKS_PER_SEC;

  int execution_found = profiling_context.num_executions != 0;
  size_t index = 0;
  if (execution_found) {
    index = string_binary_search_insert(profiling_context.execution_names, 0,
                                        profiling_context.num_executions - 1,
                                        name, &execution_found);
  }

  profiling_context.current_executions[start.current_execution_index] = NULL;

  if (!execution_found) {
    profiling_context.num_executions++;
    profiling_context.execution_names =
        realloc(profiling_context.execution_names,
                profiling_context.num_executions * sizeof(const char *));
    profiling_context.execution_times =
        realloc(profiling_context.execution_times,
                profiling_context.num_executions * sizeof(double));

    if (profiling_context.num_executions != 1) {
      /* Move everything to the right*/
      size_t i = profiling_context.num_executions - 1;
      while (i > index) {
        profiling_context.execution_names[i] =
            profiling_context.execution_names[i - 1];
        profiling_context.execution_times[i] =
            profiling_context.execution_times[i - 1];
        i--;
      }
    }

    profiling_context.execution_times[index] = 0.0;
    profiling_context.execution_names[index] = name;
  }

  profiling_context.execution_times[index] += elapsed_time;
}

void END_PROFILING(const char *out_file_name) {
  if (out_file_name && profiling_context.num_executions) {
    FILE *out_file = fopen(out_file_name, "w");
    if (!out_file) {
      fprintf(stderr, "[PROFILING] Failed to open profiling output file: %s\n",
              strerror(errno));
    } else {
      /* Sort execution times in descending order*/
      quick_sort_execution_times(
          &profiling_context.execution_names,
          &profiling_context.execution_times, 0,
          (signed long)(profiling_context.num_executions - 1));

      fprintf(out_file, "---------- %d Profiling Entries ---------\n",
              profiling_context.num_executions);
      size_t i = 0;
      while (i < profiling_context.num_executions) {
        fprintf(out_file, "--- %20s: %10.3f ms ---\n",
                profiling_context.execution_names[i],
                profiling_context.execution_times[i] * 1000.0);

        i++;
      }
      fprintf(out_file, "--------------------------------------------\n");
      fclose(out_file);
    }
  }

  size_t i = 0;
  while (i < profiling_context.num_current_executions) {
    if (profiling_context.current_executions[i]) {
      fprintf(stderr,
              "[PROFILING] %s is still executing when ending profiling\n",
              profiling_context.current_executions[i]);
    }

    i++;
  }

  free(profiling_context.execution_names);
  free(profiling_context.execution_times);
  free(profiling_context.current_executions);
  profiling_context.execution_names = NULL;
  profiling_context.execution_times = NULL;
  profiling_context.current_executions = NULL;
  profiling_context.num_executions = 0;
  profiling_context.num_current_executions = 0;
}
