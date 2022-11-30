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

int path_elements_contain(char **elements, size_t num_elements,
                          const char *value) {
  BEGIN_PROFILE_FUNC();

  size_t i = 0;
  while (i < num_elements) {
    if (strcmp(elements[i], value) == 0) {
      END_PROFILE_FUNC();
      return 1;
    }

    i++;
  }

  END_PROFILE_FUNC();
  return 0;
}

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
