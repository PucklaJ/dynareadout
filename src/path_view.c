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
#include "profiling.h"
#include "string_builder.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

path_view_t path_view_new(const char *string) {
  BEGIN_PROFILE_FUNC();

  assert(string != NULL && string[0] != '\0');

  path_view_t pv;
  pv.string = string;
  pv.start = 0;
  pv.end = 0;

  if (string[0] != PATH_SEP) {
    /* Determine the first element*/
    while (string[pv.end + 1] != PATH_SEP && string[pv.end + 1] != '\0') {
      pv.end++;
    }
  } else if (string[1] == PATH_SEP) {
    /* Support multiple leading path separators*/
    pv.start = 2;
    while (string[pv.start] == PATH_SEP) {
      pv.start++;
    }
    pv.start--;
    pv.end = pv.start;
  }

  END_PROFILE_FUNC();
  return pv;
}

int path_view_advance(path_view_t *pv) {
  BEGIN_PROFILE_FUNC();

  /* Loop until we are at a none path separator. This handles path separators at
   * the start, in the middle and at the end.*/
  int i = pv->end + 1;
  while (pv->string[i] == PATH_SEP) {
    i++;
  }
  /* If we are at '\0', we are at the end*/
  if (pv->string[i] == '\0') {
    END_PROFILE_FUNC();
    return 0;
  }

  /* Loop until a PATH_SEP has been found or the end has been reached*/
  pv->start = i;
  while (pv->string[i + 1] != PATH_SEP && pv->string[i + 1] != '\0') {
    i++;
  }
  pv->end = i;

  END_PROFILE_FUNC();
  return 1;
}

int path_view_strcmp(const path_view_t *pv, const char *str) {
  BEGIN_PROFILE_FUNC();

  assert(str != NULL);

  /* Loop over the entire string*/
  int i = 0;
  while (i < PATH_VIEW_LEN(pv) && str[i] != '\0') {
    /* If the characters are not equal, return the difference*/
    const int cmp_val = (int)(pv->string[pv->start + i] - str[i]);
    if (cmp_val != 0) {
      END_PROFILE_FUNC();
      return cmp_val;
    }

    i++;
  }

  /* Handle the case when the path view is a substring of str
   * (Example: "legend" and "legend_ids")*/
  /* If i points to the end of the path view and str then path view == str (0)*/
  /* If str is a substring of path view then path view > str (1)*/
  /* Otherwise path view is a substring of str and therefore path view < str
   * (-1)*/
  const int cmp_val = (str[i] == '\0') * 2 - 1 - (i == PATH_VIEW_LEN(pv));

  END_PROFILE_FUNC();
  return cmp_val;
}

char *path_view_stralloc(const path_view_t *pv) {
  BEGIN_PROFILE_FUNC();

  char *str = string_clone_len(&pv->string[pv->start], pv->end - pv->start + 1);

  END_PROFILE_FUNC();
  return str;
}

void path_view_print(const path_view_t *pv) {
  BEGIN_PROFILE_FUNC();

  printf("%s (%d - %d): ", pv->string, pv->start, pv->end);
  int i = pv->start;
  while (i <= pv->end) {
    putchar(pv->string[i]);

    i++;
  }
  putchar('\n');

  END_PROFILE_FUNC();
}
