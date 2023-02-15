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

#ifndef KEY_H
#define KEY_H

#include <stddef.h>
#include <stdint.h>

#define DEFAULT_VALUE_WIDTH 10
#define NODE_VALUE_WIDTH 8

typedef struct {
  char *string;

  uint8_t current_index;
  uint8_t value_width;
} card_t;

typedef struct {
  char *name;
  card_t *cards;
  size_t num_cards;
} keyword_t;

typedef void (*key_file_callback)(const char *keyword_name, const card_t *card,
                                  size_t card_index, void *user_data);

#ifdef __cplusplus
extern "C" {
#endif

/* Needs to be deallocated by key_file_free*/
keyword_t *key_file_parse(const char *file_name, size_t *num_keywords,
                          char **error_string);
void key_file_parse_with_callback(const char *file_name,
                                  key_file_callback callback,
                                  char **error_string, void *user_data);
void key_file_free(keyword_t *keywords, size_t num_keywords);
keyword_t *key_file_get(keyword_t *keywords, size_t num_keywords,
                        const char *name, size_t index);

/*
card_parse_begin(card);
while (!card_parse_done()) {
  int val = card_parse_int(card);

  card_parse_next();
}

card_parse_begin(card);

double x = card_parse_double(card);
double y = card_parse_double(card);
double z = card_parse_double(card);

*/
void card_parse_begin(card_t *card, uint8_t value_width);
void card_parse_next(card_t *card);
void card_parse_next_width(card_t *card, uint8_t value_width);
int card_parse_done(const card_t *card);
int card_parse_int(const card_t *card);
int card_parse_int_width(const card_t *card, uint8_t value_width);
float card_parse_float32(const card_t *card);
float card_parse_float32_width(const card_t *card, uint8_t value_width);
double card_parse_float64(const card_t *card);
double card_parse_float64_width(const card_t *card, uint8_t value_width);
/* Needs to be deallocated by free*/
char *card_parse_string(const card_t *card);
/* Needs to be deallocated by free*/
char *card_parse_string_width(const card_t *card, uint8_t value_width);
/* Trims trailing and leading spaces. Return value should be deallocated by
 * free*/
char *card_parse_whole(const card_t *card);
/* Does not apply trimming of trailing and leading spaces and returns the
 * complete string of the card. Return value should be deallocated by free.*/
char *card_parse_whole_no_trim(const card_t *card);

#ifdef __cplusplus
}
#endif

#endif