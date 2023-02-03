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

#ifndef PATH_VIEW_H
#define PATH_VIEW_H
#include "path.h"

#define PATH_VIEW_IS_ABS(pv) (PATH_IS_ABS(pv->string))
#define PATH_VIEW_LEN(pv) (pv->end - pv->start + 1)
#define PATH_VIEW_CPY(dst, src)                                                \
  memcpy(dst, &src->string[src->start], PATH_VIEW_LEN(src))
#define PATH_VIEW_IS_AT_ROOT(pv)                                               \
  (pv->start == pv->end && pv->string[pv->start] == PATH_SEP)

/* A view into a string that always views one path element at one time.
 * Example:
 * path_view_new("/nodout/metadata/ids") -> path_view_t;
 * string: "/nodout/metadata/ids"
 * start: 0-^
 * end: 0---^
 * path_view_advance(...) -> 1;
 * string: "/nodout/metadata/ids"
 * start: 1--^
 * end: 6---------^
 * path_view_advance(...) -> 1:
 * string: "/nodout/metadata/ids"
 * start: 8---------^
 * end: 15-----------------^
 * path_view_advance(...) -> 1:
 * string: "/nodout/metadata/ids"
 * start: 17-----------------^
 * end: 19---------------------^
 * path_view_advance(...) -> 0:
 * string: "/nodout/metadata/ids"
 * start: 17-----------------^
 * end: 19---------------------^
 */
typedef struct {
  const char *string;
  int start;
  int end;
} path_view_t;

#ifdef __cplusplus
extern "C" {
#endif

/* Create a new path view and start to view the first path element. The string
 * needs to be not null, be null-terminated and have at least a length of one.*/
path_view_t path_view_new(const char *string);

/* Advance to the next path element.
 * Returns 0 if it already is at the last element
 * and 1 otherwise. This supports multiple path separators at the beginning, in
 * the middle and at the end.*/
int path_view_advance(path_view_t *pv);

/* Like strcmp but with the current element of the path view and a string*/
int path_view_strcmp(const path_view_t *pv, const char *str);

/* Allocate memory and copy the current path view into it. Insert a '\0' at the
 * end. Needs to be deallocated by free.
 */
char *path_view_stralloc(const path_view_t *pv);

/* Print the current element of the path view and the start and end indices.*/
void path_view_print(const path_view_t *pv);

#ifdef __cplusplus
}
#endif

#endif
