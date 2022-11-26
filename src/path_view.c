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

#include "path_view.h"
#include <assert.h>
#include <stdio.h>
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

  if (pv->end == len - 1) {
    return 0;
  }

  int i = pv->end + 1 + (pv->end != 0);
  while (i < len - 1 && pv->string[i + 1] != PATH_SEP) {
    i++;
  }

  pv->start = pv->end + 1 + (pv->end != 0);
  pv->end = i;

  return 1;
}

int path_view_strcmp(const path_view_t *pv, const char *str) {
  return strncmp(&pv->string[pv->start], str, pv->end - pv->start + 1);
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
