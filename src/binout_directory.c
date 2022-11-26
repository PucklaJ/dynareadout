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

#include "binout_directory.h"
#include "path.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void binout_directory_insert_folder(binout_directory_t *dir, char *name) {
  /* Only insert the folder if it does not already exist*/
  if (binout_folder_get_folder(dir->children, dir->num_children, name)) {
    return;
  }

  dir->num_children++;
  dir->children =
      realloc(dir->children, dir->num_children * sizeof(binout_folder_t));

  binout_folder_t folder;
  folder.type = BINOUT_FOLDER;
  folder.name = name;
  folder.children = NULL;
  folder.num_children = 0;
  dir->children[dir->num_children - 1] = folder;
}

binout_folder_t *binout_folder_insert_folder(binout_folder_t *dir,
                                             const char *path, char *name) {
  if (path == NULL) {
    /* Add the folder directly underneath it*/
    /* Only add the folder it does not already exist*/
    binout_folder_t *folder = binout_folder_get_folder(
        (binout_folder_t *)dir->children, dir->num_children, name);
    if (folder) {
      free(name);
      return folder;
    }

    /* Allocate memory for the new child*/
    dir->num_children++;
    dir->children =
        realloc(dir->children, dir->num_children * sizeof(binout_folder_t));

    folder = &((binout_folder_t *)dir->children)[dir->num_children - 1];
    folder->type = BINOUT_FOLDER;
    folder->name = name;
    folder->children = NULL;
    folder->num_children = 0;

    return folder;
  } else {
    path_t p;
    p.elements = path_elements(path, &p.num_elements);

    /* Check if the parent folder already exists*/
    binout_folder_t *parent_folder = binout_folder_get_folder(
        (binout_folder_t *)dir->children, dir->num_children, p.elements[0]);
    if (!parent_folder) {
      /* Allocate memory for the new child*/
      dir->num_children++;
      dir->children =
          realloc(dir->children, dir->num_children * sizeof(binout_folder_t));

      /* Add a parent folder under which to add the folder*/
      parent_folder =
          &((binout_folder_t *)dir->children)[dir->num_children - 1];
      parent_folder->type = BINOUT_FOLDER;
      parent_folder->name = p.elements[0];
      parent_folder->children = NULL;
      parent_folder->num_children = 0;
    } else {
      /* Avoid memory leaks*/
      free(p.elements[0]);
    }

    /* Advance to the next element and ignore p.elements[0]*/
    char **elements = p.elements;
    p.elements = &p.elements[1];
    p.num_elements--;

    if (p.num_elements == 1) {
      /* Need to free manually. Otherwise this causes double free*/
      size_t j = 0;
      while (j < p.num_elements) {
        free(p.elements[j++]);
      }
      free(elements);

      /* Directly add it underneath parent*/
      parent_folder->num_children++;
      parent_folder->children =
          realloc(parent_folder->children,
                  parent_folder->num_children * sizeof(binout_folder_t));
      binout_folder_t *folder =
          &((binout_folder_t *)
                parent_folder->children)[parent_folder->num_children - 1];

      folder->type = BINOUT_FOLDER;
      folder->name = name;
      folder->children = NULL;
      folder->num_children = 0;

      return folder;
    } else {
      char *parent_path = path_str(&p);
      /* Need to free manually. Otherwise this causes double free*/
      size_t j = 0;
      while (j < p.num_elements) {
        free(p.elements[j++]);
      }
      free(elements);

      /* Recursively insert parent folders*/
      binout_folder_t *folder =
          binout_folder_insert_folder(parent_folder, parent_path, name);
      free(parent_path);

      return folder;
    }
  }
}

void binout_folder_insert_file(binout_folder_t *dir, const char *path,
                               char *name, uint8_t var_type, size_t size,
                               uint8_t file_index, long file_pos) {
  if (path == NULL) {
    /* Only add the file if it not already exists*/
    if (binout_folder_get_file(dir, name)) {
      free(name);
      return;
    }

    /* Allocate memory for the new child. In this case files*/
    dir->num_children++;
    dir->children =
        realloc(dir->children, dir->num_children * sizeof(binout_file_t));
    binout_file_t *file =
        &((binout_file_t *)dir->children)[dir->num_children - 1];

    file->type = BINOUT_FILE;
    file->name = name;
    file->var_type = var_type;
    file->size = size;
    file->file_index = file_index;
    file->file_pos = file_pos;
  } else {
    path_t p;
    p.elements = path_elements(path, &p.num_elements);
    p.num_elements--;

    char *parent_path = path_str(&p);

    /* Insert the parent folder before inserting the file*/
    binout_folder_t *parent_folder = binout_folder_insert_folder(
        dir, parent_path, p.elements[p.num_elements]);

    free(parent_path);
    path_free(&p);

    binout_folder_insert_file(parent_folder, NULL, name, var_type, size,
                              file_index, file_pos);
  }
}

binout_folder_t *binout_folder_get_folder(binout_folder_t *folders,
                                          size_t num_folders,
                                          const char *name) {
  size_t i = 0;
  while (i < num_folders) {
    if (strcmp(folders[i].name, name) == 0) {
      return &folders[i];
    }

    i++;
  }

  return NULL;
}

const binout_file_t *binout_directory_get_file(const binout_directory_t *dir,
                                               const char *path) {
  assert(path != NULL && strlen(path) != 0 && path_is_abs(path));
  assert(dir->num_children != 0);

  path_t p;
  p.elements = path_elements(path, &p.num_elements);

  /* The path needs to have at least 3 elements.
   * 1. The root folder
   * 2. The folder containing the file
   * 3. The file itself
   */
  assert(p.num_elements > 2);

  /* Search for the folder. Linear Search*/
  size_t i = 0;
  while (i < dir->num_children) {
    const binout_folder_t *folder = &dir->children[i];
    if (strcmp(folder->name, p.elements[1]) == 0) {
      free(p.elements[0]);
      free(p.elements[1]);
      char **elements = p.elements;
      p.elements = &p.elements[2];
      p.num_elements -= 2;

      char *file_path = path_str(&p);
      /* Need to free manually. Otherwise this would give a double free*/
      size_t j = 0;
      while (j < p.num_elements) {
        free(p.elements[j++]);
      }
      free(elements);

      const binout_file_t *file = binout_folder_get_file(folder, file_path);
      free(file_path);
      return file;
    }

    i++;
  }

  return NULL;
}

const binout_file_t *binout_folder_get_file(const binout_folder_t *dir,
                                            const char *path) {
  if (dir->num_children == 0) {
    return NULL;
  }

  /* Check if the folder contains folders or files*/
  if (((binout_folder_or_file_t *)dir->children)[0].type == BINOUT_FILE) {
    /* Linear Search for the correct file*/
    size_t i = 0;
    while (i < dir->num_children) {
      const binout_file_t *file = &((binout_file_t *)dir->children)[i];
      if (strcmp(file->name, path) == 0) {
        return file;
      }

      i++;
    }
  } else {
    path_t p;
    p.elements = path_elements(path, &p.num_elements);

    /* The path needs at least two elements, since this otherwise means that we
     * are looking for a file in a folder with only folders*/
    if (p.num_elements < 2) {
      path_free(&p);
      return NULL;
    }

    /* Linear Search for the correct folder*/
    size_t i = 0;
    while (i < dir->num_children) {
      const binout_folder_t *folder = &((binout_folder_t *)dir->children)[i];
      if (strcmp(folder->name, p.elements[0]) == 0) {
        /* Advance the path and create a string from it*/
        free(p.elements[0]);
        char **elements = p.elements;
        p.elements = &p.elements[1];
        p.num_elements--;
        char *file_path = path_str(&p);
        /* Need to free manually. Otherwise this causes double free*/
        size_t j = 0;
        while (j < p.num_elements) {
          free(p.elements[j++]);
        }
        free(elements);

        /* Recursivly look for the file*/
        const binout_file_t *file = binout_folder_get_file(folder, file_path);
        free(file_path);
        return file;
      }

      i++;
    }
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
