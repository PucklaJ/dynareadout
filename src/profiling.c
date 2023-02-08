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

profiling_context_t profiling_context = {0};

signed long partition_execution_times(profiling_stack_t *stack, signed long low,
                                      signed long high) {
  const double pivot = stack[high].execution_time;

  signed long i = low - 1, j = low;

  while (j < high) {
    if (stack[j].execution_time > pivot) {
      i++;
      /* Swap i and j*/
      const profiling_stack_t temp = stack[i];
      stack[i] = stack[j];
      stack[j] = temp;
    }

    j++;
  }

  /* Swap i + 1 and high*/
  const profiling_stack_t temp = stack[i + 1];
  stack[i + 1] = stack[high];
  stack[high] = temp;

  return i + 1;
}

void quick_sort_execution_times(profiling_stack_t *stack, signed long low,
                                signed long high) {
  if (low < high) {
    const signed long pi = partition_execution_times(stack, low, high);

    quick_sort_execution_times(stack, low, pi - 1);
    quick_sort_execution_times(stack, pi + 1, high);
  }

  if (stack->num_sub_executions != 0) {
    quick_sort_execution_times(stack->sub_executions, 0,
                               stack->num_sub_executions - 1);
  }
}

profiling_execution_t _BEGIN_PROFILE_SECTION(const char *name) {
  profiling_execution_t rv;
  rv.profiling_start_time = clock();

  if (profiling_context.disable) {
    return rv;
  }

  profiling_stack_t *current_stack;

  profiling_stack_t **stack_array;
  size_t *stack_array_size;

  if (profiling_context.current_stack) {
    if (profiling_context.disable_stacking) {
      rv.current_stack = profiling_context.current_stack;
      rv.should_end = 0;
      rv.start_time = clock();
      return rv;
    }

    /* Avoid recursion*/
    if (strcmp(name, profiling_context.current_stack->execution_name) == 0) {
      rv.current_stack = profiling_context.current_stack;
      rv.should_end = 0;
      rv.start_time = clock();
      return rv;
    }

    stack_array =
        (profiling_stack_t **)&profiling_context.current_stack->sub_executions;
    stack_array_size = &profiling_context.current_stack->num_sub_executions;
  } else {
    stack_array = &profiling_context.execution_stacks;
    stack_array_size = &profiling_context.num_execution_stacks;
  }

  if (*stack_array) {
    int found;
    size_t index;

    profiling_context.disable = 1;
    index = profiling_stack_binary_search_insert(
        *stack_array, 0, *stack_array_size - 1, name, &found);
    profiling_context.disable = 0;

    if (found) {
      current_stack = &(*stack_array)[index];
    } else {
      (*stack_array_size)++;
      *stack_array =
          realloc(*stack_array, *stack_array_size * sizeof(profiling_stack_t));

      /* Move everything to the right*/
      size_t i = *stack_array_size - 1;
      while (i > index) {
        (*stack_array)[i] = (*stack_array)[i - 1];
        i--;
      }

      current_stack = &(*stack_array)[index];

      current_stack->execution_name = name;
      current_stack->execution_time = 0.0;
      current_stack->profiling_time = 0.0;
      current_stack->sub_executions = NULL;
      current_stack->num_sub_executions = 0;
    }
  } else {
    *stack_array_size = 1;
    *stack_array = malloc(sizeof(profiling_stack_t));
    current_stack = &(*stack_array)[0];

    current_stack->execution_name = name;
    current_stack->execution_time = 0.0;
    current_stack->profiling_time = 0.0;
    current_stack->sub_executions = NULL;
    current_stack->num_sub_executions = 0;
  }

  current_stack->parent = profiling_context.current_stack;
  profiling_context.current_stack = current_stack;

  rv.should_end = 1;
  rv.current_stack = current_stack;
  rv.start_time = clock();

  return rv;
}

void _END_PROFILE_SECTION(const char *name, profiling_execution_t start) {
  clock_t end_time = clock();

  if (profiling_context.disable) {
    return;
  }

  const double elapsed_execution_time =
      (double)(end_time - start.start_time) / CLOCKS_PER_SEC;

  if (start.should_end) {
    const double current_execution_time =
        elapsed_execution_time - start.current_stack->profiling_time;

    start.current_stack->execution_time += current_execution_time;
    start.current_stack->profiling_time = 0.0;
    profiling_context.current_stack =
        (profiling_stack_t *)start.current_stack->parent;

    end_time = clock();
    const double elapsed_total_time =
        (double)(end_time - start.profiling_start_time) / CLOCKS_PER_SEC;
    if (start.current_stack->parent) {
      ((profiling_stack_t *)start.current_stack->parent)->profiling_time +=
          elapsed_total_time - current_execution_time;
    }
  } else {
    end_time = clock();
    const double elapsed_total_time =
        (double)(end_time - start.profiling_start_time) / CLOCKS_PER_SEC;
    if (start.current_stack) {
      start.current_stack->profiling_time +=
          elapsed_total_time - elapsed_execution_time;
    }
  }
}

void END_PROFILING(const char *out_file_name) {
  if (profiling_context.disable) {
    return;
  }

  if (profiling_context.current_stack) {
    char const **current_stack = NULL;
    size_t current_stack_size = 0;

    const profiling_stack_t *stack = profiling_context.current_stack;
    while (stack) {
      current_stack_size++;
      current_stack =
          realloc(current_stack, sizeof(const char *) * current_stack_size);
      current_stack[current_stack_size - 1] = stack->execution_name;

      stack = (const profiling_stack_t *)stack->parent;
    }

    fprintf(stderr, "Profiling is still running!\n");

    size_t i = 0;
    while (i < current_stack_size) {
      size_t j = i;
      while (j > 0) {
        fprintf(stderr, "-");

        j--;
      }

      fprintf(stderr, "%s\n", current_stack[i]);

      i++;
    }

    free(current_stack);
    return;
  }

  if (out_file_name && profiling_context.num_execution_stacks != 0) {
    FILE *out_file = fopen(out_file_name, "w");
    if (!out_file) {
      fprintf(stderr, "Failed to open profiling file: %s\n", strerror(errno));
      return;
    }

    quick_sort_execution_times(profiling_context.execution_stacks, 0,
                               profiling_context.num_execution_stacks - 1);

    fprintf(out_file,
            "--------------------- Profiling ----------------------\n");

    size_t i = 0;
    while (i < profiling_context.num_execution_stacks) {
      fprint_profiling_stack(out_file, &profiling_context.execution_stacks[i],
                             0);

      i++;
    }

    fprintf(out_file,
            "------------------------------------------------------\n");

    fclose(out_file);
    printf("Profiling written to \"%s\"\n", out_file_name);
  }

  size_t i = 0;
  while (i < profiling_context.num_execution_stacks) {
    free_profiling_stack(&profiling_context.execution_stacks[i]);

    i++;
  }

  free(profiling_context.execution_stacks);
  profiling_context.execution_stacks = NULL;
  profiling_context.num_execution_stacks = 0;
}

void fprint_profiling_stack(FILE *file, const profiling_stack_t *stack,
                            int level) {
  fprintf(file, "| ");

  {
    char name_buffer[1024];

    int i = level;
    while (i > 0) {
      sprintf(&name_buffer[level - i], "-");
      i--;
    }

    sprintf(&name_buffer[level], "%s", stack->execution_name);
    size_t pos = (size_t)level + strlen(stack->execution_name);
    name_buffer[pos] = ' ';
    size_t j = pos + 1;
    while (j < 40) {
      sprintf(&name_buffer[j], ".");

      j++;
    }
    name_buffer[40] = '\0';

    fprintf(file, "%s", name_buffer);

    sprintf(name_buffer, "%7.1f", stack->execution_time * 1000.0);
    j = 0;
    while (name_buffer[j] == ' ') {
      name_buffer[j] = '.';

      j++;
    }
    if (j != 0) {
      name_buffer[j - 1] = ' ';
    }

    fprintf(file, "%s ms |\n", name_buffer);
  }

  size_t j = 0;
  while (j < stack->num_sub_executions) {
    fprint_profiling_stack(file, &GET_SUB_EXECUTION(stack, j), level + 1);

    j++;
  }
}

void free_profiling_stack(profiling_stack_t *stack) {
  size_t i = 0;
  while (i < stack->num_sub_executions) {
    free_profiling_stack(&GET_SUB_EXECUTION(stack, i));

    i++;
  }

  free(stack->sub_executions);
}