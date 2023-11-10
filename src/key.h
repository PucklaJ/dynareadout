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

#include "extra_string.h"
#include "string_builder.h"
#include <stdint.h>

#define DEFAULT_VALUE_WIDTH 10
#define NODE_VALUE_WIDTH 8
#define INCLUDE_FILE_PATH_MAX 236

/* The different types a value inside of a card can have*/
typedef enum {
  CARD_PARSE_INT,
  CARD_PARSE_FLOAT,
  CARD_PARSE_STRING
} card_parse_type;

/* A LS Dyna Card inside a key file (input deck)*/
typedef struct {
  char *string; /* The whole line*/

  uint8_t current_index; /* Used for parsing. Stores the current index from
                            where to parse the current value*/
  uint8_t value_width;   /* Used for parsing. Stores how wide a value is.*/
} card_t;

/* A Keyword with its cards inside a key file (input deck)*/
typedef struct {
  char *name;       /* The name of the keyword*/
  card_t *cards;    /* An array stroing the cards*/
  size_t num_cards; /* The number of cards in the array*/
} keyword_t;

/* Contains options to configure how a key file is parsed*/
typedef struct {
  int parse_includes; /* Wether to parse supported INCLUDE keywords and
                         recursively parse those files. Default: 1*/
  int ignore_not_found_includes; /* Wether to not parse and not output an error
                                   when not finding an include file. Default:
                                   0*/
  char **extra_include_paths; /* Define some additional include paths which are
                                 used to look for files under the INCLUDE
                                 keyword and such*/
  size_t num_extra_include_paths; /* The number of strings in the
                                     extra_include_paths array*/
} key_parse_config_t;

/* Holds all variables used for recursion*/
typedef struct {
  char **include_paths;     /* Holds all paths that are added with INCLUDE_PATH
                               keywords and such*/
  size_t num_include_paths; /* The number of include paths*/
  char *root_folder;        /* The folder which contains the file with which the
                               function was invoked first*/
  int extra_include_paths_applied; /* Wether the additional include paths of the
                                      parse config have already been applied*/
} key_parse_recursion_t;

/* Returns a key_parse_config_t with all values set to the default*/
key_parse_config_t key_default_parse_config();

/* Holds information about the current state when parsing a key file*/
typedef struct {
  const char *file_name;      /* Name of the current file*/
  size_t line_number;         /* Current line*/
  char *const *include_paths; /* Array containing the include paths where
                                        it looks for include files (including
                                        working directory)*/
  size_t num_include_paths;   /* The number of elements in include_paths*/
  const char *root_folder; /* The folder which contains the initial file of the
                              parse call*/
} key_parse_info_t;

/* The type of the callback that is called in key_file_parse_with_callback*/
typedef void (*key_file_callback)(key_parse_info_t info,
                                  const char *keyword_name, card_t *card,
                                  size_t card_index, void *user_data);

#ifdef __cplusplus
extern "C" {
#endif

/* Parses a LS Dyna key file for keywords and their respective cards. Returns an
 * array of keyword_t and sets num_keywords to the number of elements in the
 * array. Needs to be deallocated by key_file_free.
 * parse_config: Configure how the file is parsed
 * error_string: if set to a non NULL value a error message will be set that
 * tells about an error if one occurred. If it gets set to a non NULL value it
 * needs to be deallocated by free.
 * warning_string: if set to a non NULL value warning messages that are
 * generated during parsing will be stored here. If it gets set to a non NULL
 * value it needs to deallocated by free.*/
keyword_t *key_file_parse(const char *file_name, size_t *num_keywords,
                          const key_parse_config_t *parse_config,
                          char **error_string, char **warning_string);
/* Same as key_file_parse, but instead of returning an array it calls an
 * callback every time a card (or empty keyword) is encountered.
 * user_data: will be given to the callback untouched.
 * rec: only used for recursion and should be set to NULL*/
void key_file_parse_with_callback(const char *file_name,
                                  key_file_callback callback,
                                  const key_parse_config_t *parse_config,
                                  char **error_string, char **warning_string,
                                  void *user_data, key_parse_recursion_t *rec);
/* Deallocates the data returned by key_file_parse*/
void key_file_free(keyword_t *keywords, size_t num_keywords);
/* Returns a certain keyword with name. If the key file contains more keywords
 * of the same name, index should be used to get the desired one. Where 0 refers
 * to the first one encountered in the file.*/
keyword_t *key_file_get(keyword_t *keywords, size_t num_keywords,
                        const char *name, size_t index);
/* Similar to key_file_get, but this returns a pointer to the first keyword with
 * a given name and sets slice_size to the number of keywords with the same
 * name*/
keyword_t *key_file_get_slice(keyword_t *keywords, size_t num_keywords,
                              const char *name, size_t *slice_size);

/*
The parsing of a card works by iterating over the values of a card.
Therefore the widths of the different values of the card need to be known. In
general DEFAULT_VALUE_WIDTH can be provided.

This is an example parsing a NODE card (tc and rc are ignored):
card->string: "      27             0.0            10.0           -25.6"

card_parse_begin(card);
int64_t nid = card_parse_int_width(card, 8);
card_parse_next_width(card, 8);
double x = card_parse_double_width(card, 16);
card_parse_next_width(card, 16);
double y = card_parse_double_width(card, 16);
card_parse_next_width(card, 16);
double z = card_parse_double_width(card, 16);

*/

/* Initialises the parsing of the card*/
void card_parse_begin(card_t *card, uint8_t value_width);
/* Advance to the next value. Uses the value width from card_parse_begin.*/
void card_parse_next(card_t *card);
/* Advance to the next value. Uses the value width provided here.*/
void card_parse_next_width(card_t *card, uint8_t value_width);
/* Returns wether the card has been completely parsed. Breaks if incorrect value
 * widths have been supplied*/
int card_parse_done(const card_t *card);
/* Parses the current value as an int. Uses the value width from
 * card_parse_begin. Note: This does not handle overflow, since the width of key
 * file values is too small to reach the maximum of int64_t*/
int64_t card_parse_int(const card_t *card);
/* Parses the current value as an int. Uses the value width provided here. Note:
 * This does not handle overflow, since the width of key file values is too
 * small to reach the maximum of int64_t*/
int64_t card_parse_int_width(const card_t *card, uint8_t value_width);
/* Parses the current value as an float. Uses the value width from
 * card_parse_begin. Note: This does not handle overflow, since the width of key
 * file values is too small to reach the maximum of float*/
float card_parse_float32(const card_t *card);
/* Parses the current value as an float. Uses the value width provided here.
 * Note: This does not handle overflow, since the width of key file values is
 * too small to reach the maximum of float*/
float card_parse_float32_width(const card_t *card, uint8_t value_width);
/* Parses the current value as an double. Uses the value width from
 * card_parse_begin. Note: This does not handle overflow, since the width of key
 * file values is too small to reach the maximum of double*/
double card_parse_float64(const card_t *card);
/* Parses the current value as an double. Uses the value width provided here.
 * Note: This does not handle overflow, since the width of key file values is
 * too small to reach the maximum of double*/
double card_parse_float64_width(const card_t *card, uint8_t value_width);
/* Parses the current value as an string (which means no parsing at all).
 * Trims trailing and leading spaces. Uses the value width from
 * card_parse_begin. Return value should be deallocated by
 * free*/
char *card_parse_string(const card_t *card);
/* Parses the current value as an string (which means no parsing at all).
 * Performs no trimming. Uses the value width from
 * card_parse_begin. Return value should be deallocated by
 * free*/
char *card_parse_string_no_trim(const card_t *card);
/* Parses the current value as an string (which means no parsing at all).
 * Trims trailing and leading spaces. Uses the value width provided here. Return
 * value should be deallocated by free*/
char *card_parse_string_width(const card_t *card, uint8_t value_width);
/* Parses the current value as an string (which means no parsing at all).
 * Performs no trimming. Uses the value width provided here. Return
 * value should be deallocated by free*/
char *card_parse_string_width_no_trim(const card_t *card, uint8_t value_width);
/* Returns the whole card as a string. Trims trailing and leading spaces Return
 * value should be deallocated by free*/
char *card_parse_whole(const card_t *card);
/* Does not apply trimming of trailing and leading spaces and returns the
 * complete string of the card. Return value should be deallocated by free.*/
char *card_parse_whole_no_trim(const card_t *card);
/* Returns the type of the current value. Uses the value width from
 * card_parse_begin. If CARD_PARSE_INT is returned the card_parse_int functions
 * can parse it. If CARD_PARSE_FLOAT is returned the card_parse_float functions
 * can parse it. Else only the card_parse_string functions can parse it. Uses
 * the value width from card_parse_begin.*/
card_parse_type card_parse_get_type(const card_t *card);
/* Returns the type of the current value. Uses the value width from
 * card_parse_begin. If CARD_PARSE_INT is returned the card_parse_int functions
 * can parse it. If CARD_PARSE_FLOAT is returned the card_parse_float functions
 * can parse it. Else only the card_parse_string functions can parse it. Uses
 * the value width provided here.*/
card_parse_type card_parse_get_type_width(const card_t *card,
                                          uint8_t value_width);

/* ----- Private Functions -----*/
/* Copy the contents of the card as a string directly into dst.*/
void _card_cpy(const card_t *card, char *dst, size_t len);
/* Handles the parsing of multi line string for include file names. Returns
 * wether the multi line string has been completely parsed.*/
int _parse_multi_line_string(string_builder_t *multi_line_string,
                             const card_t *card, size_t line_length);
/* -----------------------------*/

#ifdef __cplusplus
}
#endif

#endif