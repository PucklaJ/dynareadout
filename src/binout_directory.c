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
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void binout_directory_insert_folder(binout_directory_t *dir, char *name) {
  /* Only insert the folder if it does not already exist*/
  size_t index = 0;
  if (dir->num_children != 0) {
    int found;
    index = binout_directory_binary_search_folder_by_name(
        dir->children, 0, dir->num_children - 1, name, &found);
    if (found) {
      return;
    }
  }

  dir->num_children++;
  dir->children =
      realloc(dir->children, dir->num_children * sizeof(binout_folder_t));

  binout_folder_t folder;
  folder.type = BINOUT_FOLDER;
  folder.name = name;
  folder.children = NULL;
  folder.num_children = 0;

  /* Move everything to the right*/
  size_t i = dir->num_children - 1;
  while (i > index) {
    dir->children[i] = dir->children[i - 1];

    i--;
  }

  dir->children[index] = folder;
}

binout_folder_t *
binout_directory_insert_folder_by_path_view(binout_directory_t *dir,
                                            path_view_t *path) {
  assert(path->string[path->start] != PATH_SEP);

  size_t index = 0;
  binout_folder_t *folder = NULL;
  if (dir->num_children != 0) {
    int found;
    index = binout_directory_binary_search_folder_by_path_view_insert(
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
    return folder;
  }

  return binout_folder_insert_folder(folder, path);
}

binout_folder_t *binout_folder_insert_folder(binout_folder_t *dir,
                                             path_view_t *path) {
  size_t index = 0;
  binout_folder_t *folder = NULL;
  if (dir->num_children != 0) {
    int found;
    index = binout_directory_binary_search_folder_by_path_view_insert(
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
    return folder;
  }

  return binout_folder_insert_folder(folder, path);
}

void binout_folder_insert_file(binout_folder_t *dir, path_view_t *path,
                               char *name, uint8_t var_type, size_t size,
                               uint8_t file_index, long file_pos) {
  if (path == NULL) {
    /* Only add the file if it not already exists*/
    size_t index = 0;
    if (dir->num_children != 0) {
      int found;
      index = binout_directory_binary_search_file_by_name(
          (binout_file_t *)dir->children, 0, dir->num_children - 1, name,
          &found);
      if (found) {
        free(name);
        return;
      }
    }

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

    binout_file_t *file = &((binout_file_t *)dir->children)[index];

    file->type = BINOUT_FILE;
    file->name = name;
    file->var_type = var_type;
    file->size = size;
    file->file_index = file_index;
    file->file_pos = file_pos;
  } else {
    /* Insert the parent folder before inserting the file*/
    binout_folder_t *parent_folder = binout_folder_insert_folder(dir, path);

    binout_folder_insert_file(parent_folder, NULL, name, var_type, size,
                              file_index, file_pos);
  }
}

const binout_folder_or_file_t *
binout_directory_get_children(const binout_directory_t *dir, path_view_t *path,
                              size_t *num_children) {
  if (dir->num_children == 0 || !path_view_is_abs(path)) {
    *num_children = 0;
    return NULL;
  }

  if (!path_view_advance(path)) {
    *num_children = dir->num_children;
    return (const binout_folder_or_file_t *)dir->children;
  }

  size_t index = binout_directory_binary_search_folder_by_path_view(
      dir->children, 0, dir->num_children - 1, path);
  if (index == (size_t)~0) {
    *num_children = 0;
    return NULL;
  }

  const binout_folder_t *folder = &dir->children[index];

  if (!path_view_advance(path)) {
    *num_children = folder->num_children;
    return (const binout_folder_or_file_t *)folder->children;
  }

  return binout_folder_get_children(folder, path, num_children);
}

const binout_folder_or_file_t *
binout_folder_get_children(const binout_folder_t *folder, path_view_t *path,
                           size_t *num_children) {
  if (folder->num_children == 0) {
    *num_children = 0;
    return NULL;
  }

  if (((binout_folder_or_file_t *)folder->children)[0].type == BINOUT_FILE) {
    *num_children = 0;
    return NULL;
  }

  size_t index = binout_directory_binary_search_folder_by_path_view(
      (binout_folder_t *)folder->children, 0, folder->num_children - 1, path);
  if (index == (size_t)~0) {
    *num_children = 0;
    return NULL;
  }

  const binout_folder_t *child =
      &((const binout_folder_t *)folder->children)[index];

  if (!path_view_advance(path)) {
    *num_children = child->num_children;
    return (const binout_folder_or_file_t *)child->children;
  }

  return binout_folder_get_children(child, path, num_children);
}

const binout_file_t *binout_directory_get_file(const binout_directory_t *dir,
                                               path_view_t *path) {
  assert(path != NULL && path_view_is_abs(path));
  assert(dir->num_children != 0);

  /* The path needs to have at least 3 elements.
   * 1. The root folder
   * 2. The folder containing the file
   * 3. The file itself
   */
  assert(path_view_peek(path) > 2);

  /* Advance over the root folder*/
  path_view_advance(path);

  /* Search for the folder*/
  const size_t index = binout_directory_binary_search_folder_by_path_view(
      dir->children, 0, dir->num_children - 1, path);
  if (index == (size_t)~0) {
    /* The folder has not been found*/
    return NULL;
  }

  /* If we are already at the end*/
  if (!path_view_advance(path)) {
    return NULL;
  }

  /* Recursively search for the file*/
  return binout_folder_get_file_by_path_view(&dir->children[index], path);
}

const binout_file_t *
binout_folder_get_file_by_path_view(const binout_folder_t *dir,
                                    path_view_t *path) {
  if (dir->num_children == 0) {
    return NULL;
  }

  /* Check if the folder contains folders or files*/
  if (((binout_folder_or_file_t *)dir->children)[0].type == BINOUT_FILE) {
    /* Search for the correct file*/
    const size_t index = binout_directory_binary_search_file_by_path_view(
        (binout_file_t *)dir->children, 0, dir->num_children - 1, path);
    if (index == (size_t)~0) {
      /* The file has not been found*/
      return NULL;
    }

    return &((binout_file_t *)dir->children)[index];
  } else {
    /* Search for the correct folder*/
    const size_t index = binout_directory_binary_search_folder_by_path_view(
        (binout_folder_t *)dir->children, 0, dir->num_children - 1, path);
    if (index == (size_t)~0) {
      /* The folder has not been found*/
      return NULL;
    }

    /* Advance the path and check if we were already at the end*/
    if (!path_view_advance(path)) {
      return NULL;
    }

    return binout_folder_get_file_by_path_view(
        &((binout_folder_t *)dir->children)[index], path);
  }

  /* The file has not been found*/
  return NULL;
}

void binout_directory_free(binout_directory_t *dir) {
  size_t i = 0;
  while (i < dir->num_children) {
    binout_folder_free(&dir->children[i]);

    i++;
  }

  free(dir->children);
  dir->children = NULL;
  dir->num_children = 0;
}

void binout_folder_free(binout_folder_t *folder) {
  free(folder->name);
  folder->name = NULL;

  if (folder->num_children == 0) {
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
}

size_t binout_directory_binary_search_folder_by_path_view(
    binout_folder_t *folders, size_t start_index, size_t end_index,
    const path_view_t *name) {
  if (start_index == end_index) {
    if (path_view_strcmp(name, folders[start_index].name) == 0) {
      return start_index;
    }
    return ~0;
  }

  const size_t half_index = start_index + (end_index - start_index) / 2;
  const int cmp_val = path_view_strcmp(name, folders[half_index].name);

  if (cmp_val < 0) {
    const size_t index = binout_directory_binary_search_folder_by_path_view(
        folders, start_index, half_index, name);
    return index;
  } else if (cmp_val > 0) {
    if (half_index == end_index - 1) {
      const size_t index = binout_directory_binary_search_folder_by_path_view(
          folders, end_index, end_index, name);
      return index;
    }
    const size_t index = binout_directory_binary_search_folder_by_path_view(
        folders, half_index, end_index, name);
    return index;
  }

  return half_index;
}

size_t binout_directory_binary_search_folder_by_path_view_insert(
    binout_folder_t *folders, size_t start_index, size_t end_index,
    const path_view_t *name, int *found) {
  if (start_index == end_index) {
    const int cmp_value = path_view_strcmp(name, folders[start_index].name);

    if (cmp_value == 0) {
      *found = 1;
      return start_index;
    }
    if (cmp_value > 0) {
      *found = 0;
      return start_index + 1;
    }

    *found = 0;
    return start_index;
  }

  const size_t half_index = start_index + (end_index - start_index) / 2;
  const int cmp_val = path_view_strcmp(name, folders[half_index].name);

  if (cmp_val < 0) {
    const size_t index =
        binout_directory_binary_search_folder_by_path_view_insert(
            folders, start_index, half_index, name, found);
    return index;
  } else if (cmp_val > 0) {
    if (half_index == end_index - 1) {
      const size_t index =
          binout_directory_binary_search_folder_by_path_view_insert(
              folders, end_index, end_index, name, found);
      return index;
    }
    const size_t index =
        binout_directory_binary_search_folder_by_path_view_insert(
            folders, half_index, end_index, name, found);
    return index;
  }

  *found = 1;
  return half_index;
}

size_t binout_directory_binary_search_folder_by_name(binout_folder_t *folders,
                                                     size_t start_index,
                                                     size_t end_index,
                                                     const char *name,
                                                     int *found) {
  if (start_index == end_index) {
    const int cmp_value = strcmp(name, folders[start_index].name);

    if (cmp_value == 0) {
      *found = 1;
      return start_index;
    }
    if (cmp_value > 0) {
      *found = 0;
      return start_index + 1;
    }

    *found = 0;
    return start_index;
  }

  const size_t half_index = start_index + (end_index - start_index) / 2;
  const int cmp_val = strcmp(name, folders[half_index].name);

  if (cmp_val < 0) {
    const size_t index = binout_directory_binary_search_folder_by_name(
        folders, start_index, half_index, name, found);
    return index;
  } else if (cmp_val > 0) {
    if (half_index == end_index - 1) {
      const size_t index = binout_directory_binary_search_folder_by_name(
          folders, end_index, end_index, name, found);
      return index;
    }
    const size_t index = binout_directory_binary_search_folder_by_name(
        folders, half_index, end_index, name, found);
    return index;
  }

  *found = 1;
  return half_index;
}

size_t binout_directory_binary_search_file_by_path_view(
    binout_file_t *files, size_t start_index, size_t end_index,
    const path_view_t *name) {
  if (start_index == end_index) {
    if (path_view_strcmp(name, files[start_index].name) == 0) {
      return start_index;
    }
    return ~0;
  }

  const size_t half_index = start_index + (end_index - start_index) / 2;
  const int cmp_val = path_view_strcmp(name, files[half_index].name);

  if (cmp_val < 0) {
    const size_t index = binout_directory_binary_search_file_by_path_view(
        files, start_index, half_index, name);
    return index;
  } else if (cmp_val > 0) {
    if (half_index == end_index - 1) {
      const size_t index = binout_directory_binary_search_file_by_path_view(
          files, end_index, end_index, name);
      return index;
    }
    const size_t index = binout_directory_binary_search_file_by_path_view(
        files, half_index, end_index, name);
    return index;
  }

  return half_index;
}

size_t binout_directory_binary_search_file_by_name(binout_file_t *files,
                                                   size_t start_index,
                                                   size_t end_index,
                                                   const char *name,
                                                   int *found) {
  if (start_index == end_index) {
    const int cmp_value = strcmp(name, files[start_index].name);

    if (cmp_value == 0) {
      *found = 1;
      return start_index;
    }
    if (cmp_value > 0) {
      *found = 0;
      return start_index + 1;
    }

    *found = 0;
    return start_index;
  }

  const size_t half_index = start_index + (end_index - start_index) / 2;
  const int cmp_val = strcmp(name, files[half_index].name);

  if (cmp_val < 0) {
    const size_t index = binout_directory_binary_search_file_by_name(
        files, start_index, half_index, name, found);
    return index;
  } else if (cmp_val > 0) {
    if (half_index == end_index - 1) {
      const size_t index = binout_directory_binary_search_file_by_name(
          files, end_index, end_index, name, found);
      return index;
    }
    const size_t index = binout_directory_binary_search_file_by_name(
        files, half_index, end_index, name, found);
    return index;
  }

  *found = 1;
  return half_index;
}
