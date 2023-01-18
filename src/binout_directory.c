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

#include "binout_directory.h"
#include "binary_search.h"
#include "path.h"
#include "profiling.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

binout_folder_t *binout_directory_insert_folder(binout_directory_t *dir,
                                                path_view_t *path) {
  BEGIN_PROFILE_FUNC();

  /* Make sure the path is absolute, but is the first element after the root
   * folder*/
  assert(PATH_VIEW_IS_ABS(path) && path->start == 1);

  /* Only insert the folder if it does not already exist*/
  size_t index = 0;
  binout_folder_t *folder = NULL;
  if (dir->num_children != 0) {
    int found;
    index = binout_directory_binary_search_folder_insert(
        dir->children, 0, dir->num_children - 1, path, &found);
    if (found) {
      folder = &dir->children[index];
    }
  }

  if (!folder) {
    dir->num_children++;
    dir->children =
        realloc(dir->children, dir->num_children * sizeof(binout_folder_t));

    /* Move everything to the right*/
    size_t i = dir->num_children - 1;
    while (i > index) {
      dir->children[i] = dir->children[i - 1];
      i--;
    }

    folder = &dir->children[index];
    folder->type = BINOUT_FOLDER;
    folder->name = path_view_stralloc(path);
    folder->children = NULL;
    folder->num_children = 0;
  }

  if (!path_view_advance(path)) {
    END_PROFILE_FUNC();
    return folder;
  }

  folder = binout_folder_insert_folder(folder, path);

  END_PROFILE_FUNC();
  return folder;
}

binout_folder_t *binout_folder_insert_folder(binout_folder_t *dir,
                                             path_view_t *path) {
  BEGIN_PROFILE_FUNC();

  size_t index = 0;
  binout_folder_t *folder = NULL;
  if (dir->num_children != 0) {
    int found;
    index = binout_directory_binary_search_folder_insert(
        (binout_folder_t *)dir->children, 0, dir->num_children - 1, path,
        &found);
    if (found) {
      folder = &((binout_folder_t *)dir->children)[index];
    }
  }

  if (!folder) {
    dir->num_children++;
    dir->children =
        realloc(dir->children, dir->num_children * sizeof(binout_folder_t));

    /* Move everything to the right*/
    size_t i = dir->num_children - 1;
    while (i > index) {
      ((binout_folder_t *)dir->children)[i] =
          ((binout_folder_t *)dir->children)[i - 1];
      i--;
    }

    folder = &((binout_folder_t *)dir->children)[index];
    folder->type = BINOUT_FOLDER;
    folder->name = path_view_stralloc(path);
    folder->children = NULL;
    folder->num_children = 0;
  }

  if (!path_view_advance(path)) {
    END_PROFILE_FUNC();
    return folder;
  }

  folder = binout_folder_insert_folder(folder, path);

  END_PROFILE_FUNC();
  return folder;
}

void binout_folder_insert_file(binout_folder_t *dir, char *name,
                               uint8_t var_type, size_t size,
                               uint8_t file_index, long file_pos) {
  BEGIN_PROFILE_FUNC();

  /* If the file already exists, overwrite it.
   * Apparently files with the exact same path and name
   * can exist (or this was just a bug -_(`_`)_-).
   */
  size_t index = 0;
  binout_file_t *file = NULL;
  if (dir->num_children != 0) {
    int found;
    index = binout_directory_binary_search_file_insert(
        (const binout_file_t *)dir->children, 0, dir->num_children - 1, name,
        &found);
    if (found) {
      free(name);
      file = &((binout_file_t *)dir->children)[index];
    }
  }

  if (!file) {
    /* Allocate memory for the new child. In this case files*/
    dir->num_children++;
    dir->children =
        realloc(dir->children, dir->num_children * sizeof(binout_file_t));

    /* Move everything to the right*/
    size_t i = dir->num_children - 1;
    while (i > index) {
      ((binout_file_t *)dir->children)[i] =
          ((binout_file_t *)dir->children)[i - 1];
      i--;
    }

    file = &((binout_file_t *)dir->children)[index];
  }

  file->type = BINOUT_FILE;
  file->name = name;
  file->var_type = var_type;
  file->size = size;
  file->file_index = file_index;
  file->file_pos = file_pos;

  END_PROFILE_FUNC();
}

const binout_folder_or_file_t *
binout_directory_get_children(const binout_directory_t *dir, path_view_t *path,
                              size_t *num_children) {
  BEGIN_PROFILE_FUNC();

  assert(PATH_VIEW_IS_ABS(path));

  if (dir->num_children == 0) {
    *num_children = 0;
    END_PROFILE_FUNC();
    return NULL;
  }

  if (!path_view_advance(path)) {
    *num_children = dir->num_children;
    END_PROFILE_FUNC();
    return (const binout_folder_or_file_t *)dir->children;
  }

  size_t index = binout_directory_binary_search_folder(
      dir->children, 0, dir->num_children - 1, path);
  if (index == (size_t)~0) {
    *num_children = ~0;
    END_PROFILE_FUNC();
    return NULL;
  }

  const binout_folder_t *folder = &dir->children[index];

  if (!path_view_advance(path)) {
    *num_children = folder->num_children;
    END_PROFILE_FUNC();
    return (const binout_folder_or_file_t *)folder->children;
  }

  const binout_folder_or_file_t *folder_or_file =
      binout_folder_get_children(folder, path, num_children);

  END_PROFILE_FUNC();
  return folder_or_file;
}

const binout_folder_or_file_t *
binout_folder_get_children(const binout_folder_t *folder, path_view_t *path,
                           size_t *num_children) {
  BEGIN_PROFILE_FUNC();

  if (folder->num_children == 0) {
    *num_children = 0;
    END_PROFILE_FUNC();
    return NULL;
  }

  if (((binout_folder_or_file_t *)folder->children)[0].type == BINOUT_FILE) {
    *num_children = 0;
    END_PROFILE_FUNC();
    return NULL;
  }

  size_t index = binout_directory_binary_search_folder(
      (binout_folder_t *)folder->children, 0, folder->num_children - 1, path);
  if (index == (size_t)~0) {
    *num_children = ~0;
    END_PROFILE_FUNC();
    return NULL;
  }

  const binout_folder_t *child =
      &((const binout_folder_t *)folder->children)[index];

  if (!path_view_advance(path)) {
    *num_children = child->num_children;
    END_PROFILE_FUNC();
    return (const binout_folder_or_file_t *)child->children;
  }

  const binout_folder_or_file_t *folder_or_file =
      binout_folder_get_children(child, path, num_children);

  END_PROFILE_FUNC();
  return folder_or_file;
}

const binout_file_t *binout_directory_get_file(const binout_directory_t *dir,
                                               path_view_t *path) {
  BEGIN_PROFILE_FUNC();

  if (dir->num_children == 0) {
    return NULL;
  }

  assert(PATH_VIEW_IS_ABS(path));

  /* Advance over the root folder*/
  if (!path_view_advance(path)) {
    /* This means that the path only consists of the root folder.*/
    END_PROFILE_FUNC();
    return NULL;
  }

  /* Search for the folder*/
  const size_t index = binout_directory_binary_search_folder(
      dir->children, 0, dir->num_children - 1, path);
  if (index == (size_t)~0) {
    /* The folder has not been found*/
    END_PROFILE_FUNC();
    return NULL;
  }

  /* If we are already at the end*/
  if (!path_view_advance(path)) {
    END_PROFILE_FUNC();
    return NULL;
  }

  /* Recursively search for the file*/
  const binout_file_t *file =
      binout_folder_get_file(&dir->children[index], path);

  END_PROFILE_FUNC();
  return file;
}

const binout_file_t *binout_folder_get_file(const binout_folder_t *dir,
                                            path_view_t *path) {
  BEGIN_PROFILE_FUNC();

  if (dir->num_children == 0) {
    END_PROFILE_FUNC();
    return NULL;
  }

  /* Check if the folder contains folders or files*/
  if (((binout_folder_or_file_t *)dir->children)[0].type == BINOUT_FILE) {
    /* Search for the correct file*/
    const size_t index = binout_directory_binary_search_file(
        (binout_file_t *)dir->children, 0, dir->num_children - 1, path);
    if (index == (size_t)~0) {
      /* The file has not been found*/
      END_PROFILE_FUNC();
      return NULL;
    }

    END_PROFILE_FUNC();
    return &((binout_file_t *)dir->children)[index];
  } else {
    /* Search for the correct folder*/
    const size_t index = binout_directory_binary_search_folder(
        (binout_folder_t *)dir->children, 0, dir->num_children - 1, path);
    if (index == (size_t)~0) {
      /* The folder has not been found*/
      END_PROFILE_FUNC();
      return NULL;
    }

    /* Advance the path and check if we were already at the end*/
    if (!path_view_advance(path)) {
      END_PROFILE_FUNC();
      return NULL;
    }

    const binout_file_t *file = binout_folder_get_file(
        &((binout_folder_t *)dir->children)[index], path);

    END_PROFILE_FUNC();
    return file;
  }

  /* The file has not been found*/
  END_PROFILE_FUNC();
  return NULL;
}

void binout_directory_free(binout_directory_t *dir) {
  BEGIN_PROFILE_FUNC();

  size_t i = 0;
  while (i < dir->num_children) {
    binout_folder_free(&dir->children[i]);

    i++;
  }

  free(dir->children);
  dir->children = NULL;
  dir->num_children = 0;

  END_PROFILE_FUNC();
}

void binout_folder_free(binout_folder_t *folder) {
  BEGIN_PROFILE_FUNC();

  free(folder->name);
  folder->name = NULL;

  if (folder->num_children == 0) {
    END_PROFILE_FUNC();
    return;
  }

  const uint8_t type = ((binout_folder_or_file_t *)folder->children)[0].type;

  if (type == BINOUT_FILE) {
    size_t i = 0;
    while (i < folder->num_children) {
      free(((binout_file_t *)folder->children)[i].name);

      i++;
    }
  } else {
    size_t i = 0;
    while (i < folder->num_children) {
      binout_folder_free(&((binout_folder_t *)folder->children)[i]);

      i++;
    }
  }

  free(folder->children);
  folder->children = NULL;
  folder->num_children = 0;

  END_PROFILE_FUNC();
}
