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

#ifndef PATH_VIEW_H
#define PATH_VIEW_H

#define PATH_SEP '/'

/* A view into a string that always views one path element at one time*/
typedef struct {
  const char *string;
  int start;
  int end;
} path_view_t;

#ifdef __cplusplus
extern "C" {
#endif

/* Create a new path view and start to view the first path element*/
path_view_t path_view_new(const char *string);

/* Advance to the next path element.
 * Returns 0 if it already is at the last element
 * and 1 otherwise.
 * This supports multiple PATH_SEPs in the middle of the path,
 * but multiple trailing or leading PATH_SEPs will lead to undefined behaviour.
 */
int path_view_advance(path_view_t *pv);

/* Like strcmp but with a path view and a string*/
int path_view_strcmp(const path_view_t *pv, const char *str);

/* Allocate memory and copy the current path view into it*/
char *path_view_stralloc(const path_view_t *pv);

/* Returns the number of path elements that are ahead plus the current one*/
int path_view_peek(const path_view_t *pv);

/* Returns wether the string of the path view starts with the root folder*/
int path_view_is_abs(const path_view_t *pv);

int path_view_len(const path_view_t *pv);

void path_view_cpy(char *dst, const path_view_t *src);

void path_view_print(const path_view_t *pv);

#ifdef __cplusplus
}
#endif

#endif
