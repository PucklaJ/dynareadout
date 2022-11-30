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

#include "path_view.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

path_view_t path_view_new(const char *string) {
  assert(string != NULL && strlen(string) > 0);

  path_view_t pv;
  pv.string = string;
  pv.start = 0;
  pv.end = 0;

  if (string[0] != PATH_SEP) {
    const int len = strlen(string);
    if (len != 1) {
      while (pv.end < len - 1 && string[pv.end + 1] != PATH_SEP) {
        pv.end++;
      }
    }
  }

  return pv;
}

int path_view_advance(path_view_t *pv) {
  const int len = strlen(pv->string);

  if (pv->end == len - 1 ||
      (pv->end == len - 2 && pv->string[pv->end + 1] == PATH_SEP)) {
    return 0;
  }

  int i = pv->end + 1 + (pv->end != 0);
  while (i < len - 1 && pv->string[i + 1] != PATH_SEP) {
    i++;
  }

  pv->start = pv->end + 1 + (pv->end != 0);
  pv->end = i;

  while (pv->string[pv->start] == PATH_SEP) {
    pv->start++;
  }

  return 1;
}

int path_view_strcmp(const path_view_t *pv, const char *str) {
  const int cmp_val = strncmp(&pv->string[pv->start], str, PATH_VIEW_LEN(pv));
  /* We need to also consider the lengths of the strings, because by calling
   * strncmp, "legend" and "legend_ids" also returns 0*/
  if (cmp_val == 0) {
    return (strlen(str) > PATH_VIEW_LEN(pv)) * -1;
  }

  return cmp_val;
}

char *path_view_stralloc(const path_view_t *pv) {
  char *str = malloc(PATH_VIEW_LEN(pv) + 1);
  PATH_VIEW_CPY(str, pv);
  str[PATH_VIEW_LEN(pv)] = '\0';
  return str;
}

int path_view_peek(const path_view_t *pv) {
  int counter = 1;
  const int len = strlen(pv->string);

  int i = pv->end + 2;
  while (i < len) {
    if (pv->string[i] == PATH_SEP) {
      counter++;
    }

    i++;
  }

  return counter + (pv->string[len - 1] != PATH_SEP);
}

void path_view_print(const path_view_t *pv) {
  printf("%s (%d - %d): ", pv->string, pv->start, pv->end);
  int i = pv->start;
  while (i <= pv->end) {
    putchar(pv->string[i]);

    i++;
  }
  putchar('\n');
}
