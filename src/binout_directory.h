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

#ifndef BINOUT_DIRECTORY_H
#define BINOUT_DIRECTORY_H
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
  size_t size;        /* Number of values (bytes = sizeof(var_type) * size)*/
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

binout_folder_t *binout_folder_insert_folder(binout_folder_t *dir,
                                             const char *path, char *name);

void binout_folder_insert_file(binout_folder_t *dir, const char *path,
                               char *name, uint8_t var_type, size_t size,
                               uint8_t file_index, long file_pos);

binout_folder_t *binout_folder_get_folder(binout_folder_t *folders,
                                          size_t num_folders, const char *name);

const binout_file_t *binout_directory_get_file(const binout_directory_t *dir,
                                               const char *path);

const binout_file_t *binout_folder_get_file(const binout_folder_t *dir,
                                            const char *path);

void binout_directory_free(binout_directory_t *dir);

void binout_folder_free(binout_folder_t *folder);

#ifdef __cplusplus
}
#endif

#endif
