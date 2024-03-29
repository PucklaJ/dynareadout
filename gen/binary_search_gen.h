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

#ifndef BINARY_SEARCH_GEN_H
#define BINARY_SEARCH_GEN_H

#include <stdio.h>

void binary_search_cmp(FILE *file, const char *type_name);
void binary_search_prototype(FILE *file, const char *func_name,
                             const char *array_type, const char *search_type);
void binary_search_body(FILE *file, const char *func_name, const char *cmp_func,
                        const char *cmp_suf);
void binary_search_insert_prototype(FILE *file, const char *func_name,
                                    const char *array_type,
                                    const char *search_type);
void binary_search_insert_body(FILE *file, const char *func_name,
                               const char *cmp_func, const char *cmp_suf);

#endif