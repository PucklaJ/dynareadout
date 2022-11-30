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

#ifndef BINOUT_DIRECTORY_H
#define BINOUT_DIRECTORY_H
#include "path_view.h"
#include <stddef.h>
#include <stdint.h>

enum { BINOUT_FILE, BINOUT_FOLDER };

/* A folder or file used to read the first byte to find out if it is a folder or
 * file*/
typedef struct {
  uint8_t type; /* Either BINOUT_FOLDER or BINOUT_FILE*/
} binout_folder_or_file_t;

/* Stores the index of the file and the position in that file at which the data
 * can be found*/
typedef struct {
  uint8_t type;
  char *name;         /* Name of the variable*/
  uint8_t var_type;   /* Type of the variable*/
  size_t size;        /* Size of the data of the variable in bytes*/
  uint8_t file_index; /* Index into the file_handles array of binout_file*/
  long file_pos;      /* The file position used in fseek*/
} binout_file_t;

/* Has a name and stores subfolders or files*/
typedef struct {
  uint8_t type;
  char *name;          /* Name of the folder*/
  void *children;      /* An array containing the subfolders or files*/
  size_t num_children; /* Size of the children array*/
} binout_folder_t;

/* A directory structure of a binout file(s).
 * Each file represents a variable and where to find it inside the file(s)
 */
typedef struct {
  binout_folder_t *children;
  size_t num_children;
} binout_directory_t;

#ifdef __cplusplus
extern "C" {
#endif

void binout_directory_insert_folder(binout_directory_t *dir, char *name);

binout_folder_t *
binout_directory_insert_folder_by_path_view(binout_directory_t *dir,
                                            path_view_t *path);

binout_folder_t *binout_folder_insert_folder(binout_folder_t *dir,
                                             path_view_t *path);

void binout_folder_insert_file(binout_folder_t *dir, path_view_t *path,
                               char *name, uint8_t var_type, size_t size,
                               uint8_t file_index, long file_pos);

const binout_file_t *binout_directory_get_file(const binout_directory_t *dir,
                                               path_view_t *path);

const binout_folder_or_file_t *
binout_directory_get_children(const binout_directory_t *dir, path_view_t *path,
                              size_t *num_children);
const binout_folder_or_file_t *
binout_folder_get_children(const binout_folder_t *folder, path_view_t *path,
                           size_t *num_children);

const binout_file_t *
binout_folder_get_file_by_path_view(const binout_folder_t *dir,
                                    path_view_t *path);

void binout_directory_free(binout_directory_t *dir);

void binout_folder_free(binout_folder_t *folder);

size_t binout_directory_binary_search_folder_by_path_view(
    binout_folder_t *folders, size_t start_index, size_t end_index,
    const path_view_t *name);
size_t binout_directory_binary_search_folder_by_path_view_insert(
    binout_folder_t *folders, size_t start_index, size_t end_index,
    const path_view_t *name, int *found);
size_t binout_directory_binary_search_folder_by_name(binout_folder_t *folders,
                                                     size_t start_index,
                                                     size_t end_index,
                                                     const char *name,
                                                     int *found);
size_t binout_directory_binary_search_file_by_path_view(
    binout_file_t *files, size_t start_index, size_t end_index,
    const path_view_t *name);
size_t binout_directory_binary_search_file_by_name(binout_file_t *files,
                                                   size_t start_index,
                                                   size_t end_index,
                                                   const char *name,
                                                   int *found);

#ifdef __cplusplus
}
#endif

#endif
