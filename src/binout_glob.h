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

#ifndef BINOUT_GLOB_H
#define BINOUT_GLOB_H
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Takes in a glob pattern and returns all globed files and writes the number
 * of files into num_files The return value needs to be deallocated using
 * binout_free_glob*/
char **binout_glob(const char *pattern, size_t *num_files);
/* Free the allocated memory*/
void binout_free_glob(char **globed_files, size_t num_files);

#ifdef __cplusplus
}
#endif

#endif