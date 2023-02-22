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
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

size_t _path_move_up(const char *path, char path_sep) {
  BEGIN_PROFILE_FUNC();

  /* Loop over the string and store the last position of a path seperator*/
  size_t last_path_sep = (size_t)~0;
  size_t i = 0;
  while (path[i] != '\0') {
    if (path[i] == path_sep) {
      last_path_sep = i;
    }

    i++;
  }

  if (last_path_sep == (size_t)~0) {
    END_PROFILE_FUNC();
    return last_path_sep;
  }

  /* Support trailing path separators*/
  if (path[last_path_sep + 1] == '\0') {
    /* Support multiple path separators*/
    while (path[last_path_sep] == path_sep) {
      if (last_path_sep == 0) {
        break;
      }
      last_path_sep--;
    }

    /* The path consists of only path separators*/
    if (last_path_sep == 0) {
      END_PROFILE_FUNC();
      return (size_t)~0;
    }

    /* Loop until the first path seperator has been found*/
    while (path[last_path_sep] != path_sep) {
      if (last_path_sep == 0) {
        break;
      }
      last_path_sep--;
    }
  }

  /* Support multiple path separators*/
  while (path[last_path_sep] == path_sep) {
    if (last_path_sep == 0) {
      break;
    }
    last_path_sep--;
  }

  if (last_path_sep != 0) {
    last_path_sep++;
  }

  END_PROFILE_FUNC();
  return last_path_sep;
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

  /* Loop until the first non path seperator has been found*/
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

#ifdef _WIN32
int path_is_file(const char *path_name) {
  BEGIN_PROFILE_FUNC();

  const DWORD attributes = GetFileAttributes(path_name);
  const int rv =
      (attributes != INVALID_FILE_ATTRIBUTES &&
       !(attributes & FILE_ATTRIBUTE_DIRECTORY) &&
       (attributes & (FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ARCHIVE)));

  END_PROFILE_FUNC();
  return rv;
}
#else
int path_is_file(const char *path_name) {
  BEGIN_PROFILE_FUNC();

  struct stat path_stat;
  if (stat(path_name, &path_stat) != 0) {
    END_PROFILE_FUNC();
    return 0;
  }

  const int rv = S_ISREG(path_stat.st_mode);

  END_PROFILE_FUNC();
  return rv;
}
#endif

#ifdef _WIN32
char *path_working_directory() {
  BEGIN_PROFILE_FUNC();

  char *buffer;

  const DWORD buffer_size = GetCurrentDirectory(0, NULL);
  if (buffer_size == 0) {
    buffer = NULL;
  } else {
    buffer = malloc(buffer_size);
    if (GetCurrentDirectory(buffer_size, buffer) == 0) {
      free(buffer);
      buffer = NULL;
    }
  }

  if (!buffer) {
    buffer = malloc(2);
    buffer[0] = '.';
    buffer[1] = '\n';
  }

  END_PROFILE_FUNC();
  return buffer;
}
#else
char *path_working_directory() {
  BEGIN_PROFILE_FUNC();

  char *buffer = malloc(1024);

  char *rv = getcwd(buffer, 1024);
  if (!rv) {
    buffer[0] = '.';
    buffer[1] = '\0';
  }

  END_PROFILE_FUNC();
  return buffer;
}
#endif

int path_is_abs(const char *path_name) {
  BEGIN_PROFILE_FUNC();

#ifdef _WIN32
  const int rv = ((path_name[0] >= 'A' && path_name[0] <= 'Z') &&
                  (path_name[1] == ':') && (path_name[2] == REAL_PATH_SEP));
#else
  const int rv = path_name[0] == PATH_SEP;
#endif

  END_PROFILE_FUNC();
  return rv;
}