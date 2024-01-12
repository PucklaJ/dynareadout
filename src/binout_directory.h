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

struct binout_entry_t;
typedef struct binout_entry_t binout_entry_t;

/* An entry inside of a binout_directory_t. Can either be a file or a folder. */
struct binout_entry_t {
  uint8_t type; /* Either BINOUT_FOLDER or BINOUT_FILE */
  char *name;   /* Name of the variable or folder */
  union {
    size_t size;         /* Size of the data of the variable in bytes */
    size_t num_children; /* Size of the children array */
  };
  binout_entry_t *children; /* An array containing the sub entries if this entry
                               is a folder */
  uint8_t var_type;         /* Type of the variable */
  uint8_t file_index; /* Index into the file_handles array of binout_file */
  long file_pos;      /* The file position used in fseek */
};

/* A directory structure of a binout file(s).
 * Each file represents a variable and where to find it inside the file(s)
 */
typedef struct {
  binout_entry_t *children;
  size_t num_children;
} binout_directory_t;

#ifdef __cplusplus
extern "C" {
#endif

/* Inserts a folder with the absolute path of path into dir.
 * path needs to be an absolute path (start with '/') and have its current
 * element one after the root folder (start == 1). The folder will only be
 * inserted if it does not already exist. Returns the newly created (or found)
 * folder.
 */
binout_entry_t *binout_directory_insert_folder(binout_directory_t *dir,
                                               path_view_t *path);

/* Same as binout_directory_insert_folder, but for binout_folder_t. Without the
 * requirement of an absolute path. Used for recursion.
 */
binout_entry_t *binout_folder_insert_folder(binout_entry_t *dir,
                                            path_view_t *path);

/* Insert a file with the given name and parameters into the folder specified by
 * dir. If a file with the same name already exists, it will be overwritten.
 * This function takes ownership of name, which means that name needs to be
 * allocated by malloc, etc. and that you do not need to deallocate it.
 */
void binout_folder_insert_file(binout_entry_t *dir, char *name,
                               uint8_t var_type, size_t size,
                               uint8_t file_index, long file_pos);

/* Returns the file at the given path if it does exist and NULL otherwise.
 * path needs to be absolute and start at the root folder (start == 0) and
 * have at least three elements.
 */
const binout_entry_t *binout_directory_get_file(const binout_directory_t *dir,
                                                path_view_t *path);

/* Same as binout_directory_get_file. Used for recursion.*/
const binout_entry_t *binout_folder_get_file(const binout_entry_t *dir,
                                             path_view_t *path);

/* Returns an pointer to children of an folder.
 * If the given path is a file this returns NULL and sets num_children to 0.
 * If the given path does not exist this returns NULL and sets num_children to
 * ~0. The path needs to be absolute and start at the root folder (start == 0).
 */
const binout_entry_t *
binout_directory_get_children(const binout_directory_t *dir, path_view_t *path,
                              size_t *num_children);

/* Same as binout_directory_get_children. Used for recursion.*/
const binout_entry_t *binout_folder_get_children(const binout_entry_t *folder,
                                                 path_view_t *path,
                                                 size_t *num_children);

/* Deallocates all memory of a binout_directory_t.
 * You need to call this if you use any of the insert functions
 * to avoid memory leaks.
 */
void binout_directory_free(binout_directory_t *dir);

/* Deallocates all memory of a binout_entry_t with type BINOUT_FOLDER. Used for
 * recursion.*/
void binout_folder_free(binout_entry_t *folder);

#ifdef __cplusplus
}
#endif

#endif
