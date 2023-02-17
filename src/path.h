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

#ifndef PATH_H
#define PATH_H
#include <stdlib.h>

/* TODO: Make sure to use the correct PATH_SEP on windows when working with the
 * real file system*/
#define PATH_SEP '/'
#ifdef _WIN32
#define REAL_PATH_SEP '\\'
#else
#define REAL_PATH_SEP '/'
#endif

#define PATH_IS_ABS(str) (str[0] == PATH_SEP)

#ifdef __cplusplus
extern "C" {
#endif

#define path_move_up(path) _path_move_up(path, PATH_SEP)
#define path_move_up_real(path) _path_move_up(path, REAL_PATH_SEP)

/* Returns the index at which the new path string would end (index of PATH_SEP)
 * when moving up one folder. If no parent folder exists, then ~0 is returned.*/
size_t _path_move_up(const char *path, char path_sep);

/* Join two paths together by inserting a PATH_SEP. Needs to be deallocated by
 * free*/
char *path_join(const char *lhs, const char *rhs);

/* Returns wether the given path exists and is a file*/
int path_is_file(const char *path_name);

/* Returns the current working directory. Needs to be deallocated by free.*/
char *path_working_directory();

/* Returns wether a path is absolute*/
int path_is_abs(const char *path_name);

#ifdef __cplusplus
}
#endif

#endif
