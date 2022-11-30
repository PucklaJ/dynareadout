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

#include "binout_glob.h"
#include "path.h"
#include "profiling.h"

#ifdef _WIN32
#include <io.h>
#include <limits.h>

char **binout_glob(const char *pattern, size_t *num_files) {
  BEGIN_PROFILE_FUNC();

  struct _finddatai64_t find_buffer;
  intptr_t result = 0;
  *num_files = 0;
  char **globed_files = NULL;

  const intptr_t find_handle = _findfirsti64(pattern, &find_buffer);
  while (result == 0 && find_handle != -1) {
    (*num_files)++;
    globed_files = realloc(globed_files, *num_files * sizeof(char *));

    const size_t name_length = strlen(find_buffer.name);
    globed_files[*num_files - 1] = malloc(name_length + 1);
    memcpy(globed_files[*num_files - 1], find_buffer.name, name_length + 1);

    result = _findnexti64(find_handle, &find_buffer);
  }

  _findclose(find_handle);

  /* Add the parent folder to the results*/
  const size_t pattern_len = strlen(pattern);
  size_t parent_len = SIZE_MAX;

  size_t i = pattern_len - 2;
  while (i != SIZE_MAX) {
    if (pattern[i] == '\\' || pattern[i] == '/') {
      parent_len = i + 1;
      break;
    }

    i--;
  }

  /* If it has no parent path just return*/
  if (parent_len == SIZE_MAX) {
    END_PROFILE_FUNC();
    return globed_files;
  }

  i = 0;
  while (i < *num_files) {
    const size_t name_length = strlen(globed_files[i]);
    /* Reallocate so that we don't need to free*/
    globed_files[i] = realloc(globed_files[i], parent_len + name_length + 1);
    /* Just copy from one part of the string to another*/
    memcpy(&globed_files[i][parent_len], globed_files[i], name_length);
    memcpy(globed_files[i], pattern, parent_len);
    globed_files[i][parent_len + name_length] = '\0';

    i++;
  }

  END_PROFILE_FUNC();
  return globed_files;
}

#else
#include <glob.h>

char **binout_glob(const char *pattern, size_t *num_files) {
  BEGIN_PROFILE_FUNC();

  glob_t glob_buffer;
  const int error_code = glob(pattern, GLOB_TILDE, NULL, &glob_buffer);

  if (error_code != 0) {
    *num_files = 0;
    globfree(&glob_buffer);
    END_PROFILE_FUNC();
    return NULL;
  }

  /* Directly use the values from glob_buffer. Why not?*/
  *num_files = glob_buffer.gl_pathc;

  END_PROFILE_FUNC();
  return glob_buffer.gl_pathv;
}

#endif

void binout_free_glob(char **globed_files, size_t num_files) {
  size_t i = 0;
  while (i < num_files) {
    free(globed_files[i]);

    i++;
  }
  free(globed_files);
}
