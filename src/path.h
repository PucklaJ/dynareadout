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

#define PATH_SEP '/'

#define PATH_IS_ABS(str) (str[0] == PATH_SEP)

#ifdef __cplusplus
extern "C" {
#endif

/* Returns the index at which the new path string would end (index of PATH_SEP)
 * when moving up one folder*/
size_t path_move_up(const char *path);

#ifdef __cplusplus
}
#endif

#endif
