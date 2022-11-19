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

#ifndef PATH_H
#define PATH_H
#include <stdlib.h>

#define PATH_SEP '/'
#define PATH_SEP_STR "/"

typedef struct {
  char **elements;
  size_t num_elements;
} path_t;

#ifdef __cplusplus
extern "C" {
#endif

/* Add element to the elements of path*/
void path_join(path_t *path, const char *element);
/* Check wether path is absolute instead of relative (Just checks if the first
 * char is PATH_SEP)*/
int path_is_abs(const char *path);
/* Checks if two paths are compatible. This is used to determine if a certain
 * record can be added to a certain data pointer.
 * path1 ............... path of the new record (without variable name)
 * path2 ............... path of the record of the data pointer (without
 * variable name)*/
int path_compatible(const path_t *path1, const path_t *path2);
/* Parses the path for ".." and changes its elements accordingly
 * Example: /nodout/metadata/../d000001 -> /nodout/d000001*/
void path_parse(path_t *path);
/* Return multiple strings containing all elements of path
 * The return value must be deallocated using path_free_elements or path_free if
 * path_t is used*/
char **path_elements(const char *path, size_t *num_elements);
/* Returns if elements contain value */
int path_elements_contain(char **elements, size_t num_elements,
                          const char *value);
/* Frees all the memory allocated*/
void path_free_elements(char **elements, size_t num_elements);
/* Frees all the memory allocated*/
void path_free(path_t *path);
/* Wether the elements of path1 are the same as path2*/
int path_equals(path_t *path1, path_t *path2);
/* Create a copy of src and store it in dst
 * dst needs to be deallocated wit path_free*/
void path_copy(path_t *dst, path_t *src);
/* Converts a path_t to a human readable string*/
char *path_str(path_t *path);
int path_cmp(const path_t *path1, const path_t *path2);

#ifdef __cplusplus
}
#endif

#endif