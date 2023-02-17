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

#include "path.h"
#include "profiling.h"
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

size_t path_move_up(const char *path) {
  BEGIN_PROFILE_FUNC();

  const int len = strlen(path);
  /* Only support absolute paths*/
  assert(len > 1 && path[0] == PATH_SEP);

  /* Support trailing PATH_SEPs*/
  size_t i = len - 1;
  while (i > 0 && path[i] == PATH_SEP) {
    i--;
  }

  while (i > 1 && !(path[i] == PATH_SEP && path[i - 1] != PATH_SEP)) {
    i--;
  }

  const size_t index = i - (i == 1);
  END_PROFILE_FUNC();
  return index;
}

char *path_join(const char *lhs, const char *rhs) {
  BEGIN_PROFILE_FUNC();

  const size_t lhs_len = strlen(lhs);
  const size_t rhs_len = strlen(rhs);

  size_t i = lhs_len - 1, j = 0;
  /* Loop until the last non path seperator has been found*/
  while (lhs[i] == PATH_SEP) {
    if (i == 0) {
      break;
    }
    i--;
  }

  /* Loop unti the first non path seperator has been found*/
  while (rhs[j] == PATH_SEP) {
    if (j == rhs_len - 1) {
      break;
    }
    j++;
  }

  const size_t str_len = (i + 1) + (rhs_len - j) + 1;

  char *str = malloc(str_len + 1);
  memcpy(str, lhs, i + 1);
  memcpy(&str[i + 2], &rhs[j], rhs_len - j);
  str[i + 1] = PATH_SEP;
  str[str_len] = '\0';

  END_PROFILE_FUNC();
  return str;
}

int path_is_file(const char *path_name) {
  struct stat path_stat;
  if (stat(path_name, &path_stat) != 0) {
    return 0;
  }

  return S_ISREG(path_stat.st_mode);
}

char *path_working_directory() {
  char *buffer = malloc(1024);

  char *rv = getcwd(buffer, 1024);
  if (!rv) {
    free(buffer);
    buffer = NULL;
  }

  return buffer;
}