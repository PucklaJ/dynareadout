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

#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

#include <stddef.h>

/* Used to construct string out of multiple other strings and characters*/
typedef struct {
  char *buffer;
  size_t ptr;
  size_t cap;
} string_builder_t;

#ifdef __cplusplus
extern "C" {
#endif

/* Returns a string builder with all values initialized to the default. Needs to
 * be deallocated by string_builder_free*/
string_builder_t string_builder_new();
/* Add a string*/
void string_builder_append(string_builder_t *b, const char *s);
/* Add a char*/
void string_builder_append_char(string_builder_t *b, char c);
/* Add a string and use l instead of strlen*/
void string_builder_append_len(string_builder_t *b, const char *s, size_t l);
/* Returns the underlying buffer and sets all values of b to the default*/
char *string_builder_move(string_builder_t *b);
/* Deallocates all memory of b*/
void string_builder_free(string_builder_t *b);

/* Same as libc's strdup: Allocates memory and copies the contents of str and
 * returns them. Needs to be deallocated by free.*/
char *string_clone(const char *str);

/* Same as string_clone but use len instead of strlen: Allocates memory and
 * copies the contents of str and returns them. Needs to be deallocated by
 * free.*/
char *string_clone_len(const char *str, size_t len);

#ifdef __cplusplus
}
#endif

#endif