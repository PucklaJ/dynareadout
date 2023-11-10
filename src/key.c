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

#include "key.h"
#include "binary_search.h"
#include "line.h"
#include "profiling.h"
#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void _message_stack_push(string_builder_t *stack, const char *msg) {
  /* Add a new line after the previous message*/
  if (stack->cap != 0) {
    string_builder_append_char(stack, '\n');
  }
  string_builder_append(stack, msg);
}

void _message_stack_push_f(string_builder_t *stack, const char *f, ...) {
  va_list args;

  char buffer[1024];

  va_start(args, f);
  vsprintf(buffer, f, args);
  va_end(args);

  _message_stack_push(stack, buffer);
}

#define ERROR_MSG(msg) _message_stack_push(&error_stack, msg)
#define ERROR_F(f, ...) _message_stack_push_f(&error_stack, f, __VA_ARGS__)
#define ERROR_ERRNO(msg) ERROR_F(msg, strerror(errno));
#define WARNING_MSG(msg) _message_stack_push(&warning_stack, msg)
#define WARNING_F(f, ...) _message_stack_push_f(&warning_stack, f, __VA_ARGS__)
#define WARNING_ERRNO(msg) WARNING_F(msg, strerror(errno));
#define ERROR_KEYWORD_NOT_IMPLEMENTED(keyword)                                 \
  ERROR_F("%s:%lu: The keyword %s is not implemented", file_name,              \
          current_keyword_line, keyword)

#define KEY_PARSE_INFO()                                                       \
  key_parse_info_t info;                                                       \
  info.file_name = file_name;                                                  \
  info.line_number = line_count;                                               \
  info.include_paths = rec_ptr->include_paths;                                 \
  info.num_include_paths = rec_ptr->num_include_paths;                         \
  info.root_folder = rec_ptr->root_folder

key_parse_config_t key_default_parse_config() {
  key_parse_config_t c = {0};
  c.parse_includes = 1;
  return c;
}

typedef struct {
  keyword_t *current_keyword;
  keyword_t *keywords;
  size_t *num_keywords;
} key_file_parse_data;

void key_file_parse_callback(key_parse_info_t info, const char *keyword_name,
                             card_t *card, size_t card_index, void *user_data) {
  key_file_parse_data *data = (key_file_parse_data *)user_data;

  if (!data->current_keyword || card_index == 0 || card_index == (size_t)~0) {
    size_t index = 0;

    if (data->keywords) {
      int found;
      index = key_file_binary_search_insert(
          data->keywords, 0, *data->num_keywords - 1, keyword_name, &found);

      if (found) {
        /* Look for the last occurrence of the keyword*/
        index++;
        if (index != *data->num_keywords) {
          while (strcmp(data->keywords[index].name, keyword_name) == 0) {
            index++;
          }
        }
      }
    }

    (*data->num_keywords)++;
    data->keywords =
        realloc(data->keywords, *data->num_keywords * sizeof(keyword_t));

    /* Move everything to the right*/
    size_t j = *data->num_keywords - 1;
    while (j > index) {
      data->keywords[j] = data->keywords[j - 1];
      j--;
    }

    data->current_keyword = &data->keywords[index];

    data->current_keyword->cards = NULL;
    data->current_keyword->num_cards = 0;

    data->current_keyword->name = string_clone(keyword_name);
  }

  if (!card) {
    return;
  }

  /* Add the card to the keyword*/
  data->current_keyword->num_cards++;
  data->current_keyword->cards =
      realloc(data->current_keyword->cards,
              data->current_keyword->num_cards * sizeof(card_t));
  card_t *keyword_card =
      &data->current_keyword->cards[data->current_keyword->num_cards - 1];
  keyword_card->string = string_clone(card->string);
}

keyword_t *key_file_parse(const char *file_name, size_t *num_keywords,
                          const key_parse_config_t *parse_config,
                          char **error_string, char **warning_string) {
  BEGIN_PROFILE_FUNC();

  key_file_parse_data data;
  data.current_keyword = NULL;
  data.keywords = NULL;
  data.num_keywords = num_keywords;
  *num_keywords = 0;

  char *internal_error_string;

  key_file_parse_with_callback(file_name, key_file_parse_callback, parse_config,
                               &internal_error_string, warning_string, &data,
                               NULL);

  /* Deallocate the memory if an error occurred*/
  if (internal_error_string) {
    key_file_free(data.keywords, *data.num_keywords);
    data.keywords = NULL;
    *data.num_keywords = 0;
    if (error_string) {
      *error_string = internal_error_string;
    } else {
      free(internal_error_string);
    }
  } else if (error_string) {
    *error_string = NULL;
  }

  END_PROFILE_FUNC();
  return data.keywords;
}

void key_file_parse_with_callback(const char *file_name,
                                  key_file_callback callback,
                                  const key_parse_config_t *_parse_config,
                                  char **error_string, char **warning_string,
                                  void *user_data, key_parse_recursion_t *rec) {
  BEGIN_PROFILE_FUNC();

  if (error_string) {
    *error_string = NULL;
  }
  if (warning_string) {
    *warning_string = NULL;
  }

  /* Variables to stack multiple errors*/
  string_builder_t error_stack = string_builder_new(),
                   warning_stack = string_builder_new();

  FILE *file = fopen(file_name, "rb");
  if (!file) {
    if (error_string) {
      ERROR_ERRNO("Failed to open key file: %s");
      *error_string = string_builder_move(&error_stack);
    }
    if (warning_string) {
      *warning_string = NULL;
    }
    END_PROFILE_FUNC();
    return;
  }

  /* Data for recursion. Stores all include paths*/
  key_parse_recursion_t *rec_ptr = NULL;

  if (!rec) {
    rec_ptr = malloc(sizeof(key_parse_recursion_t));
    rec_ptr->include_paths = NULL;
    rec_ptr->num_include_paths = 0;
    rec_ptr->extra_include_paths_applied = 0;

    const size_t index = path_move_up_real(file_name);
    if (index == (size_t)~0) {
      rec_ptr->root_folder = path_working_directory();
    } else {
      if (path_is_abs(file_name)) {
        rec_ptr->root_folder = string_clone_len(file_name, index + 1);
      } else {
        char *current_wd = path_working_directory();
        rec_ptr->root_folder = path_join_real(current_wd, file_name);
        rec_ptr->root_folder[path_move_up_real(rec_ptr->root_folder) + 1] =
            '\0';
        free(current_wd);
      }
    }
  } else {
    rec_ptr = rec;
  }

  if (rec_ptr->num_include_paths == 0) {
    /* Add the current working directory (local directory) to the
     * include paths*/
    rec_ptr->num_include_paths = 1;
    rec_ptr->include_paths = malloc(sizeof(char *));
    rec_ptr->include_paths[0] = path_working_directory();
  }

  /* Setup parse config*/
  key_parse_config_t parse_config;
  if (_parse_config) {
    parse_config = *_parse_config;
  } else {
    parse_config = key_default_parse_config();
  }

  /* Add the additional include paths to the include_paths array*/
  if (!rec_ptr->extra_include_paths_applied) {
    const size_t j = rec_ptr->num_include_paths;
    rec_ptr->num_include_paths += parse_config.num_extra_include_paths;
    rec_ptr->include_paths = realloc(
        rec_ptr->include_paths, rec_ptr->num_include_paths * sizeof(char *));

    size_t i = 0;
    while (i < parse_config.num_extra_include_paths) {
      rec_ptr->include_paths[j + i] =
          string_clone(parse_config.extra_include_paths[i]);

      i++;
    }

    rec_ptr->extra_include_paths_applied = 1;
  }

  line_reader_t line_reader = new_line_reader(file);
  extra_string current_keyword_name;
  current_keyword_name.buffer[0] = '\0';
  current_keyword_name.extra = NULL;

  size_t current_keyword_length = 0;
  size_t current_keyword_line = (size_t)~0;
  size_t card_index = 0;
  size_t line_count = 0;

  string_builder_t current_multi_line_string = string_builder_new();

  /* Loop until all lines have been read or an error occurred*/
  while (read_line(&line_reader)) {
    line_count++;

    /* Check if the line starts with a comment or contains a comment
     * character*/
    if (line_reader.comment_index == 0) {
      /* The entire line is a comment. Ignore it.*/
      continue;
    } else if (line_reader.comment_index != (size_t)~0) {
      extra_string_set(&line_reader.line, line_reader.comment_index, '\0');
    }

    /* ------- 🐉 Here be parsings 🐉 --------- */

    /* Check if the line is a keyword (starts with '*')
     * Support lines being preceded by ' ' */
    int is_keyword = 0;
    size_t i = 0;
    if (line_reader.line_length != 0) {
      while (extra_string_get(&line_reader.line, i) == ' ') {
        i++;
      }
      is_keyword = extra_string_get(&line_reader.line, i) == '*';
    }

    /* ------ 🔑 Keyword Parsing 🔑 --------- */
    if (is_keyword) {
      /* If we already read a keyword we need to call the callback if the
       * keyword had no cards*/
      if (current_keyword_length != 0 && card_index == 0) {
        char *keyword_name;
        if (current_keyword_length < EXTRA_STRING_BUFFER_SIZE) {
          keyword_name = current_keyword_name.buffer;
        } else {
          keyword_name = malloc(current_keyword_length + 1);
          extra_string_copy_to_string(keyword_name, &current_keyword_name,
                                      current_keyword_length);
          keyword_name[current_keyword_length] = '\0';
        }

        KEY_PARSE_INFO();
        callback(info, keyword_name, NULL, (size_t)~0, user_data);

        if (keyword_name != current_keyword_name.buffer) {
          free(keyword_name);
        }
      }

      extra_string_copy(&current_keyword_name, &line_reader.line,
                        line_reader.line_length, i + 1);

      /* Compute the length of the keyword*/
      current_keyword_length = 0;
      while (1) {
        const char c =
            extra_string_get(&current_keyword_name, current_keyword_length);
        if (c == ' ' || c == '\0') {
          break;
        }
        current_keyword_length++;
      }
      extra_string_set(&current_keyword_name, current_keyword_length, '\0');

      current_keyword_line = line_count;

      /* Quit on "END"*/
      if (current_keyword_length == 3 &&
          extra_string_compare(&current_keyword_name, "END") == 0) {
        break;
      }

      card_index = 0;
    } else {
      /* -------- 🃏 Card Parsing 🃏 ----------*/
      card_t card;
      if (line_reader.line_length < EXTRA_STRING_BUFFER_SIZE) {
        card.string = line_reader.line.buffer;
      } else {
        card.string = malloc(line_reader.line_length + 1);
        extra_string_copy_to_string(card.string, &line_reader.line,
                                    line_reader.line_length);
        card.string[line_reader.line_length] = '\0';
      }

      /* -------- ⛅ Include Parsing ⛅ -------*/
      if (extra_string_starts_with(&current_keyword_name, "INCLUDE")) {
        /* Also parse the INCLUDE keywords even when parse_includes is set to
         * 0 to support multi line include file names*/
        if (parse_config.parse_includes) {
          /* Parse the current card as a file name that should be included*/
          if (extra_string_compare(&current_keyword_name, "INCLUDE") == 0 ||
              (extra_string_compare(&current_keyword_name, "INCLUDE_BINARY") ==
                   0 &&
               card_index == 0) ||
              (extra_string_compare(&current_keyword_name, "INCLUDE_NASTRAN") ==
                   0 &&
               card_index == 0)) {
            /* Support multi line file names (LS Dyna Manual Volume I
             * *INCLUDE Remark 2, p. 2690)*/
            if (_parse_multi_line_string(&current_multi_line_string, &card,
                                         line_reader.line_length)) {
              /* Loop over all include paths and look for the file*/
              char *full_include_file_name = NULL;
              size_t i = 0;
              while (i < rec_ptr->num_include_paths) {
                full_include_file_name =
                    path_join(rec_ptr->include_paths[i],
                              current_multi_line_string.buffer);
                if (path_is_file(full_include_file_name)) {
                  break;
                }
                free(full_include_file_name);
                full_include_file_name = NULL;

                i++;
              }

              if (full_include_file_name) {
                char *include_error, *include_warning;
                /* Call the function recursively*/
                key_file_parse_with_callback(
                    full_include_file_name, callback, &parse_config,
                    &include_error, &include_warning, user_data, rec_ptr);
                free(full_include_file_name);

                /* Add the error to the error stack if an error occurred in
                 * the recursive call*/
                if (include_error != NULL) {
                  ERROR_MSG(include_error);
                  free(include_error);
                }
                /* Add the warning to the warning stack if a warning occurred
                 * in the recursive call*/
                if (include_warning != NULL) {
                  WARNING_MSG(include_warning);
                  free(include_warning);
                }
              } else {
                const char *format_str = "%s:%zu: \"%s\" could not be found";

                if (parse_config.ignore_not_found_includes) {
                  WARNING_F(format_str, file_name, line_count,
                            current_multi_line_string.buffer);
                } else {
                  ERROR_F(format_str, file_name, line_count,
                          current_multi_line_string.buffer);
                }
              }

              string_builder_free(&current_multi_line_string);
              card_index++;
            }

            if (card.string != line_reader.line.buffer) {
              free(card.string);
            }
            continue;
          } else if (extra_string_compare(&current_keyword_name,
                                          "INCLUDE_PATH") == 0) {
            /* Support multi line file names (LS Dyna Manual Volume I
             * *INCLUDE Remark 2, p. 2690)*/
            if (!_parse_multi_line_string(&current_multi_line_string, &card,
                                          line_reader.line_length)) {
              /* continue without calling the callback for the card*/
              if (card.string != line_reader.line.buffer) {
                free(card.string);
              }
              continue;
            }

            if (!path_is_directory(current_multi_line_string.buffer)) {
              WARNING_F("%s:%zu: INCLUDE_PATH has not been found: \"%s\"",
                        file_name, line_count,
                        current_multi_line_string.buffer);
              string_builder_free(&current_multi_line_string);
            } else {
              rec_ptr->num_include_paths++;
              rec_ptr->include_paths =
                  realloc(rec_ptr->include_paths,
                          rec_ptr->num_include_paths * sizeof(char *));
              rec_ptr->include_paths[rec_ptr->num_include_paths - 1] =
                  string_builder_move(&current_multi_line_string);
            }

            /* continue without calling the callback for the card*/
            if (card.string != line_reader.line.buffer) {
              free(card.string);
            }

            card_index++;
            continue;
          } else if (extra_string_compare(&current_keyword_name,
                                          "INCLUDE_PATH_RELATIVE") == 0) {
            /* Support multi line file names (LS Dyna Manual Volume I
             * *INCLUDE Remark 2, p. 2690)*/
            if (!_parse_multi_line_string(&current_multi_line_string, &card,
                                          line_reader.line_length)) {
              /* continue without calling the callback for the card*/
              if (card.string != line_reader.line.buffer) {
                free(card.string);
              }
              continue;
            }

            char *full_include_path_name = path_join(
                rec_ptr->root_folder, current_multi_line_string.buffer);
            string_builder_free(&current_multi_line_string);

            if (!path_is_directory(full_include_path_name)) {
              WARNING_F(
                  "%s:%zu: INCLUDE_PATH_RELATIVE has not been found: \"%s\"",
                  file_name, line_count, full_include_path_name);
              free(full_include_path_name);
            } else {
              rec_ptr->num_include_paths++;
              rec_ptr->include_paths =
                  realloc(rec_ptr->include_paths,
                          rec_ptr->num_include_paths * sizeof(char *));
              rec_ptr->include_paths[rec_ptr->num_include_paths - 1] =
                  full_include_path_name;
            }

            /* continue without calling the callback for the card*/
            if (card.string != line_reader.line.buffer) {
              free(card.string);
            }

            card_index++;
            continue;
          } else if (extra_string_compare(&current_keyword_name,
                                          "INCLUDE_BINARY") == 0) {
            /* INCLUDE_BINARY does only have one card (which is parsed
             * above)*/
            WARNING_F("%s:%zu: Invalid number of cards (%zu) for "
                      "INCLUDE_BINARY keyword",
                      file_name, line_count, card_index + 1);

            /* continue without calling the callback for the card*/
            if (card.string != line_reader.line.buffer) {
              free(card.string);
            }

            card_index++;
            continue;
          } else if (extra_string_compare(&current_keyword_name,
                                          "INCLUDE_NASTRAN") == 0) {
            /* The include file name is already parsed above*/
            if (card_index != 1) {
              WARNING_F("%s:%zu: Invalid number of cards (%zu) for "
                        "INCLUDE_NASTRAN keyword",
                        file_name, line_count, card_index + 1);
            }

            /* continue without calling the callback for the card*/
            if (card.string != line_reader.line.buffer) {
              free(card.string);
            }

            card_index++;
            continue;
          } else if (extra_string_starts_with(&current_keyword_name,
                                              "INCLUDE_MULTISCALE")) {
            /* These keywords do not start with a filename therefore the user
             * needs to parse them himself*/
          } else {
            if (card_index == 0 &&
                !_parse_multi_line_string(&current_multi_line_string, &card,
                                          line_reader.line_length)) {
              /* continue without calling the callback for the card*/
              if (card.string != line_reader.line.buffer) {
                free(card.string);
              }
              continue;
            }
          }
        } else {
          if (!extra_string_starts_with(&current_keyword_name,
                                        "INCLUDE_MULTISCALE")) {
            const int all_cards_are_filenames =
                extra_string_compare(&current_keyword_name, "INCLUDE") == 0 ||
                extra_string_compare(&current_keyword_name, "INCLUDE_PATH") ==
                    0 ||
                extra_string_compare(&current_keyword_name,
                                     "INCLUDE_PATH_RELATIVE") == 0;

            if (all_cards_are_filenames || card_index == 0) {
              if (!_parse_multi_line_string(&current_multi_line_string, &card,
                                            line_reader.line_length)) {
                /* continue without calling the callback for the card*/
                if (card.string != line_reader.line.buffer) {
                  free(card.string);
                }
                continue;
              }
            }
          }
        }
      }
      /* ------- ⛅ End of Include Parsing ⛅ -------*/

      if (current_multi_line_string.buffer) {
        if (card.string != line_reader.line.buffer) {
          free(card.string);
        }
        card.string = string_builder_move(&current_multi_line_string);
      }

      char *keyword_name;
      if (current_keyword_length < EXTRA_STRING_BUFFER_SIZE) {
        keyword_name = current_keyword_name.buffer;
      } else {
        keyword_name = malloc(current_keyword_length + 1);
        extra_string_copy_to_string(keyword_name, &current_keyword_name,
                                    current_keyword_length);
        keyword_name[current_keyword_length] = '\0';
      }

      KEY_PARSE_INFO();
      callback(info, keyword_name, &card, card_index, user_data);

      if (card.string != line_reader.line.buffer) {
        free(card.string);
      }
      if (keyword_name != current_keyword_name.buffer) {
        free(keyword_name);
      }

      card_index++;
    }

    /* ---------------------------------------- */
  }

  if (ferror(file)) {
    ERROR_F("An error occurred while reading \"%s\": %s", file_name,
            strerror(errno));
  } else {
    /* Call the callback for the last keyword if it is not "END", because some
     * keywords can have no cards*/
    if (card_index == 0 &&
        (current_keyword_length != 3 ||
         extra_string_compare(&current_keyword_name, "END") != 0)) {
      char *keyword_name;
      if (current_keyword_length < EXTRA_STRING_BUFFER_SIZE) {
        keyword_name = current_keyword_name.buffer;
      } else {
        keyword_name = malloc(current_keyword_length + 1);
        extra_string_copy_to_string(keyword_name, &current_keyword_name,
                                    current_keyword_length);
        keyword_name[current_keyword_length] = '\0';
      }

      card_t card;
      if (current_multi_line_string.buffer) {
        card.string = string_builder_move(&current_multi_line_string);
      } else {
        card.string = NULL;
      }

      KEY_PARSE_INFO();
      callback(info, keyword_name, card.string ? &card : NULL, (size_t)~0,
               user_data);

      if (keyword_name != current_keyword_name.buffer) {
        free(keyword_name);
      }
      if (card.string) {
        free(card.string);
      }
    }
  }

  /* Free all recursion data (include paths, root folder)*/
  if (!rec) {
    size_t i = 0;
    while (i < rec_ptr->num_include_paths) {
      free(rec_ptr->include_paths[i]);

      i++;
    }
    free(rec_ptr->include_paths);
    free(rec_ptr->root_folder);
    free(rec_ptr);
  }

  free(line_reader.line.extra);
  free(current_keyword_name.extra);

  free_line_reader(line_reader);
  fclose(file);

  /* Convert the error stack into an error string*/
  if (error_stack.buffer && error_string) {
    *error_string = string_builder_move(&error_stack);
  }

  /* Convert the warning stack into a warning string*/
  if (warning_stack.buffer && warning_string) {
    *warning_string = string_builder_move(&warning_stack);
  }

  string_builder_free(&error_stack);
  string_builder_free(&warning_stack);

  END_PROFILE_FUNC();
}

void key_file_free(keyword_t *keywords, size_t num_keywords) {
  BEGIN_PROFILE_FUNC();

  size_t i = 0;
  while (i < num_keywords) {
    free(keywords[i].name);
    size_t j = 0;
    while (j < keywords[i].num_cards) {
      free(keywords[i].cards[j].string);

      j++;
    }
    free(keywords[i].cards);

    i++;
  }

  free(keywords);

  END_PROFILE_FUNC();
}

keyword_t *key_file_get(keyword_t *keywords, size_t num_keywords,
                        const char *name, size_t index) {
  BEGIN_PROFILE_FUNC();

  if (num_keywords == 0) {
    END_PROFILE_FUNC();
    return NULL;
  }

  const size_t find_index =
      key_file_binary_search(keywords, 0, num_keywords - 1, name);
  if (find_index == (size_t)~0) {
    END_PROFILE_FUNC();
    return NULL;
  }

  if (find_index == 0 && index == 0) {
    END_PROFILE_FUNC();
    return keywords;
  }

  /* Find the first of the keyword*/
  size_t i = find_index;
  while (i > 0 && strcmp(keywords[i].name, name) == 0) {
    i--;
  }

  if (i != 0 || strcmp(keywords[i].name, name) != 0) {
    i++;
  }

  size_t j = 0;
  while (j < index && i < num_keywords) {
    j++;
    i++;
  }

  if (j == index && strcmp(keywords[i].name, name) == 0) {
    keyword_t *value = &keywords[i];
    END_PROFILE_FUNC();
    return value;
  }

  END_PROFILE_FUNC();
  return NULL;
}

keyword_t *key_file_get_slice(keyword_t *keywords, size_t num_keywords,
                              const char *name, size_t *slice_size) {
  BEGIN_PROFILE_FUNC();

  if (num_keywords == 0) {
    *slice_size = 0;
    END_PROFILE_FUNC();
    return NULL;
  }

  const size_t find_index =
      key_file_binary_search(keywords, 0, num_keywords - 1, name);
  if (find_index == (size_t)~0) {
    *slice_size = 0;
    END_PROFILE_FUNC();
    return NULL;
  }

  /* Find the first of the keyword*/
  size_t start_index = find_index;
  if (find_index != 0) {
    while (start_index > 0 && strcmp(keywords[start_index].name, name) == 0) {
      start_index--;
    }

    start_index++;
  }

  /* Find the last of the keyword*/
  size_t end_index = find_index;
  if (find_index != num_keywords - 1) {
    while (end_index < num_keywords &&
           strcmp(keywords[end_index].name, name) == 0) {
      end_index++;
    }

    end_index--;
  }

  *slice_size = end_index - start_index + 1;

  keyword_t *slice = &keywords[start_index];

  END_PROFILE_FUNC();
  return slice;
}

void card_parse_begin(card_t *card, uint8_t value_width) {
  BEGIN_PROFILE_FUNC();

  card->current_index = 0;
  card->value_width = value_width;

  END_PROFILE_FUNC();
}

void card_parse_next(card_t *card) {
  BEGIN_PROFILE_FUNC();

  card->current_index += card->value_width;

  END_PROFILE_FUNC();
}

void card_parse_next_width(card_t *card, uint8_t value_width) {
  BEGIN_PROFILE_FUNC();

  card->current_index += value_width;

  END_PROFILE_FUNC();
}

int card_parse_done(const card_t *card) {
  BEGIN_PROFILE_FUNC();

  const int is_done = card->string[card->current_index] == '\0';

  END_PROFILE_FUNC();
  return is_done;
}

int64_t card_parse_int(const card_t *card) {
  return card_parse_int_width(card, card->value_width);
}

int64_t card_parse_int_width(const card_t *card, uint8_t value_width) {
  BEGIN_PROFILE_FUNC();

  errno = 0;

  uint8_t i = card->current_index;
  /* Loop until leading whitespace is trimmed*/
  while (i < card->current_index + value_width && card->string[i] == ' ') {
    i++;
  }

  /* The string is completely empty or just spaces*/
  if (i == card->current_index + value_width || card->string[i] == '\0') {
    errno = EINVAL;

    END_PROFILE_FUNC();
    return 0;
  }

  /* Check for negative sign*/
  int64_t sign = 1;
  if (card->string[i] == '-') {
    sign = -1;
    i++;
  }
  int64_t result = 0;

  /* Iterate over the digits of the string and compute the integer value*/
  while (i < card->current_index + value_width && card->string[i] != '\0') {
    if (card->string[i] >= '0' && card->string[i] <= '9') {
      result = result * 10 + (card->string[i] - '0');
    } else if (card->string[i] == ' ') {
      /* Quit when encountering whitespace*/
      result *= sign;

      END_PROFILE_FUNC();
      return result;
    } else {
      /* If a non-digit character is encountered, return 0 to indicate an
       * error*/
      errno = EINVAL;

      END_PROFILE_FUNC();
      return 0;
    }

    i++;
  }

  result *= sign;

  END_PROFILE_FUNC();
  return result;
}

float card_parse_float32(const card_t *card) {
  return card_parse_float32_width(card, card->value_width);
}

float card_parse_float32_width(const card_t *card, uint8_t value_width) {
  BEGIN_PROFILE_FUNC();

  errno = 0;

  float result = 0.0f;
  float fraction = 0.0f;
  float exponent = 1.0f;
  float sign = 1.0f;
  int has_integer = 0;
  int has_fraction = 0;
  int has_exponent = 0;
  int exponent_sign = 1;
  int exponent_value = 0;

  uint8_t i = card->current_index;
  /* Loop until leading whitespace is trimmed*/
  while (i < card->current_index + value_width && card->string[i] == ' ') {
    i++;
  }

  /* The string is completely empty or just spaces*/
  if (i == card->current_index + value_width || card->string[i] == '\0') {
    errno = EINVAL;

    END_PROFILE_FUNC();
    return 0.0f;
  }

  /* Check for sign*/
  if (card->string[i] == '-') {
    sign = -1.0f;
    i++;
  } else if (card->string[i] == '+') {
    i++;
  }

  if (i == card->current_index + value_width || card->string[i] == '\0' ||
      card->string[i] == ' ') {
    errno = EINVAL;

    END_PROFILE_FUNC();
    return 0.0f;
  }

  /* Parse integer part*/
  while (i < card->current_index + value_width && card->string[i] != '\0' &&
         card->string[i] != '.' && card->string[i] != 'e' &&
         card->string[i] != 'E') {
    if (card->string[i] >= '0' && card->string[i] <= '9') {
      has_integer = 1;

      result = result * 10.0f + (card->string[i] - '0');
    } else if (card->string[i] == ' ') {
      /* Quit when encountering whitespace*/
      result *= sign;

      END_PROFILE_FUNC();
      return result;
    } else {
      /* Invalid character*/
      errno = EINVAL;

      END_PROFILE_FUNC();
      return 0.0f;
    }
    i++;
  }

  /* If we already reached the end*/
  if (i == card->current_index + value_width || card->string[i] == '\0') {
    result *= sign;

    END_PROFILE_FUNC();
    return result;
  }

  /* Parse fraction part*/
  if (card->string[i] == '.') {
    i++;
    has_fraction = 1;
    while (i < card->current_index + value_width && card->string[i] != '\0' &&
           card->string[i] != 'e' && card->string[i] != 'E') {
      if (card->string[i] >= '0' && card->string[i] <= '9') {
        fraction = fraction * 10.0f + (card->string[i] - '0');
        exponent *= 10.0f;
        i++;
      } else if (card->string[i] == ' ') {
        /* Quit when encountering whitespace*/
        fraction /= exponent;
        result += fraction;
        result *= sign;

        END_PROFILE_FUNC();
        return result;
      } else {
        /* Invalid character*/
        errno = EINVAL;

        return 0.0;
      }
    }

    /* If we already reached the end*/
    if (i == card->current_index + value_width || card->string[i] == '\0') {
      fraction /= exponent;
      result += fraction;
      result *= sign;

      END_PROFILE_FUNC();
      return result;
    }
  }

  /* Parse exponent part*/
  if (card->string[i] == 'e' || card->string[i] == 'E') {
    if (!has_integer) {
      errno = EINVAL;

      END_PROFILE_FUNC();
      return 0.0f;
    }

    i++;
    if (card->string[i] == '-') {
      exponent_sign = -1;
      i++;
    } else if (card->string[i] == '+') {
      i++;
    }

    if (i == card->current_index + value_width || card->string[i] == '\0' ||
        card->string[i] == ' ') {
      if (has_fraction) {
        fraction /= exponent;
        result += fraction;
      }

      result *= sign;

      errno = EINVAL;

      END_PROFILE_FUNC();
      return result;
    }

    while (i < card->current_index + value_width && card->string[i] != '\0') {
      if (card->string[i] >= '0' && card->string[i] <= '9') {
        has_exponent = 1;

        exponent_value = exponent_value * 10 + (card->string[i] - '0');
        i++;
      } else if (card->string[i] == ' ') {
        /* Quit when encountering whitespace*/
        if (has_fraction) {
          fraction /= exponent;
          result += fraction;
        }
        result *= pow(10, exponent_sign * exponent_value);
        result *= sign;

        END_PROFILE_FUNC();
        return result;
      } else {
        /* Invalid character*/
        errno = EINVAL;

        END_PROFILE_FUNC();
        return 0.0f;
      }
    }

    if (!has_exponent) {
      if (has_fraction) {
        fraction /= exponent;
        result += fraction;
      }

      result *= sign;

      errno = EINVAL;

      END_PROFILE_FUNC();
      return result;
    }
  }

  /* Combine integer, fraction, and exponent parts */
  if (has_fraction) {
    fraction /= exponent;
    result += fraction;
  }
  if (has_exponent) {
    result *= pow(10, exponent_sign * exponent_value);
  }

  result *= sign;

  END_PROFILE_FUNC();
  return result;
}

double card_parse_float64(const card_t *card) {
  return card_parse_float64_width(card, card->value_width);
}

double card_parse_float64_width(const card_t *card, uint8_t value_width) {
  BEGIN_PROFILE_FUNC();

  errno = 0;

  double result = 0.0;
  double fraction = 0.0;
  double exponent = 1.0;
  double sign = 1.0;
  int has_integer = 0;
  int has_fraction = 0;
  int has_exponent = 0;
  int exponent_sign = 1;
  int exponent_value = 0;

  uint8_t i = card->current_index;
  /* Loop until leading whitespace is trimmed*/
  while (i < card->current_index + value_width && card->string[i] == ' ') {
    i++;
  }

  /* The string is completely empty or just spaces*/
  if (i == card->current_index + value_width || card->string[i] == '\0') {
    errno = EINVAL;

    END_PROFILE_FUNC();
    return 0.0;
  }

  /* Check for sign*/
  if (card->string[i] == '-') {
    sign = -1.0;
    i++;
  } else if (card->string[i] == '+') {
    i++;
  }

  if (i == card->current_index + value_width || card->string[i] == '\0' ||
      card->string[i] == ' ') {
    errno = EINVAL;

    END_PROFILE_FUNC();
    return 0.0;
  }

  /* Parse integer part*/
  while (i < card->current_index + value_width && card->string[i] != '\0' &&
         card->string[i] != '.' && card->string[i] != 'e' &&
         card->string[i] != 'E') {
    if (card->string[i] >= '0' && card->string[i] <= '9') {
      has_integer = 1;

      result = result * 10.0 + (card->string[i] - '0');
    } else if (card->string[i] == ' ') {
      /* Quit when encountering whitespace*/
      result *= sign;

      END_PROFILE_FUNC();
      return result;
    } else {
      /* Invalid character*/
      errno = EINVAL;

      END_PROFILE_FUNC();
      return 0.0;
    }
    i++;
  }

  /* If we already reached the end*/
  if (i == card->current_index + value_width || card->string[i] == '\0') {
    result *= sign;

    END_PROFILE_FUNC();
    return result;
  }

  /* Parse fraction part*/
  if (card->string[i] == '.') {
    i++;
    has_fraction = 1;
    while (i < card->current_index + value_width && card->string[i] != '\0' &&
           card->string[i] != 'e' && card->string[i] != 'E') {
      if (card->string[i] >= '0' && card->string[i] <= '9') {
        fraction = fraction * 10.0 + (card->string[i] - '0');
        exponent *= 10.0;
        i++;
      } else if (card->string[i] == ' ') {
        /* Quit when encountering whitespace*/
        fraction /= exponent;
        result += fraction;
        result *= sign;

        END_PROFILE_FUNC();
        return result;
      } else {
        /* Invalid character*/
        errno = EINVAL;

        return 0.0;
      }
    }

    /* If we already reached the end*/
    if (i == card->current_index + value_width || card->string[i] == '\0') {
      fraction /= exponent;
      result += fraction;
      result *= sign;

      END_PROFILE_FUNC();
      return result;
    }
  }

  /* Parse exponent part*/
  if (card->string[i] == 'e' || card->string[i] == 'E') {
    if (!has_integer) {
      errno = EINVAL;

      END_PROFILE_FUNC();
      return 0.0;
    }

    i++;
    if (card->string[i] == '-') {
      exponent_sign = -1;
      i++;
    } else if (card->string[i] == '+') {
      i++;
    }

    if (i == card->current_index + value_width || card->string[i] == '\0' ||
        card->string[i] == ' ') {
      if (has_fraction) {
        fraction /= exponent;
        result += fraction;
      }

      result *= sign;

      errno = EINVAL;

      END_PROFILE_FUNC();
      return result;
    }

    while (i < card->current_index + value_width && card->string[i] != '\0') {
      if (card->string[i] >= '0' && card->string[i] <= '9') {
        has_exponent = 1;

        exponent_value = exponent_value * 10 + (card->string[i] - '0');
        i++;
      } else if (card->string[i] == ' ') {
        /* Quit when encountering whitespace*/
        if (has_fraction) {
          fraction /= exponent;
          result += fraction;
        }
        result *= pow(10, exponent_sign * exponent_value);
        result *= sign;

        END_PROFILE_FUNC();
        return result;
      } else {
        /* Invalid character*/
        errno = EINVAL;

        END_PROFILE_FUNC();
        return 0.0;
      }
    }

    if (!has_exponent) {
      if (has_fraction) {
        fraction /= exponent;
        result += fraction;
      }

      result *= sign;

      errno = EINVAL;

      END_PROFILE_FUNC();
      return result;
    }
  }

  /* Combine integer, fraction, and exponent parts */
  if (has_fraction) {
    fraction /= exponent;
    result += fraction;
  }
  if (has_exponent) {
    result *= pow(10, exponent_sign * exponent_value);
  }

  result *= sign;

  END_PROFILE_FUNC();
  return result;
}

char *card_parse_string(const card_t *card) {
  return card_parse_string_width(card, card->value_width);
}

char *card_parse_string_no_trim(const card_t *card) {
  return card_parse_string_width_no_trim(card, card->value_width);
}

char *card_parse_string_width(const card_t *card, uint8_t value_width) {
  BEGIN_PROFILE_FUNC();

  uint8_t i = 0;
  while (i < value_width && card->string[i + card->current_index] != '\0' &&
         card->string[i + card->current_index] == ' ') {
    i++;
  }

  /* There is no string. Just spaces*/
  if (i == value_width || card->string[i + card->current_index] == '\0') {
    char *value = malloc(1);
    *value = '\0';
    END_PROFILE_FUNC();
    return value;
  }

  const uint8_t start_index = i;

  uint8_t end_index = i;
  while (i < value_width && card->string[i + card->current_index] != '\0') {
    if (card->string[i + card->current_index] != ' ') {
      end_index = i;
    }
    i++;
  }

  char *value =
      string_clone_len(&card->string[start_index + card->current_index],
                       end_index - start_index + 1);

  END_PROFILE_FUNC();
  return value;
}

char *card_parse_string_width_no_trim(const card_t *card, uint8_t value_width) {
  BEGIN_PROFILE_FUNC();

  char *value =
      string_clone_len(&card->string[card->current_index], value_width);

  END_PROFILE_FUNC();
  return value;
}

char *card_parse_whole(const card_t *card) {
  BEGIN_PROFILE_FUNC();

  size_t i = 0;
  while (card->string[i] == ' ') {
    i++;
  }

  const size_t start_index = i;

  size_t end_index = i;
  while (card->string[i] != '\0') {
    if (card->string[i] != ' ') {
      end_index = i;
    }
    i++;
  }

  char *value =
      string_clone_len(&card->string[start_index], end_index - start_index + 1);

  END_PROFILE_FUNC();
  return value;
}

char *card_parse_whole_no_trim(const card_t *card) {
  BEGIN_PROFILE_FUNC();

  char *value = string_clone(card->string);

  END_PROFILE_FUNC();
  return value;
}

card_parse_type card_parse_get_type(const card_t *card) {
  return card_parse_get_type_width(card, card->value_width);
}

card_parse_type card_parse_get_type_width(const card_t *card,
                                          uint8_t value_width) {
  BEGIN_PROFILE_FUNC();

  if (value_width == 0) {
    END_PROFILE_FUNC();
    return CARD_PARSE_STRING;
  }

  uint8_t i = card->current_index;

  /* Loop until the leading spaces have been trimmed*/
  while (card->string[i] == ' ' && (i - card->current_index) < value_width) {
    i++;
  }

  /* Only spaces*/
  if (card->string[i] == '\0' || (i - card->current_index) == value_width) {
    END_PROFILE_FUNC();
    return CARD_PARSE_STRING;
  }

  if (card->string[i] == '+' || card->string[i] == '-') {
    i++;
    if ((i - card->current_index) == value_width ||
        !(card->string[i] >= '0' && card->string[i] <= '9')) {
      END_PROFILE_FUNC();
      return CARD_PARSE_STRING;
    }
  } else if (card->string[i] >= '0' && card->string[i] <= '9') {
    i++;
  } else {
    END_PROFILE_FUNC();
    return CARD_PARSE_STRING;
  }

  /* Loop until there are no numbers*/
  while (card->string[i] >= '0' && card->string[i] <= '9' &&
         (i - card->current_index) < value_width) {
    i++;
  }

  /* If we are done*/
  if (card->string[i] == '\0' || card->string[i] == ' ' ||
      (i - card->current_index) == value_width) {
    END_PROFILE_FUNC();
    return CARD_PARSE_INT;
  }

  if (card->string[i] == '.') {
    i++;
    if ((i - card->current_index) == value_width ||
        !(card->string[i] >= '0' && card->string[i] <= '9')) {
      END_PROFILE_FUNC();
      return CARD_PARSE_STRING;
    }

    /* Loop until there are no numbers*/
    while (card->string[i] >= '0' && card->string[i] <= '9' &&
           (i - card->current_index) < value_width) {
      i++;
    }

    if (card->string[i] == '\0' || card->string[i] == ' ' ||
        (i - card->current_index) == value_width) {
      END_PROFILE_FUNC();
      return CARD_PARSE_FLOAT;
    }

    if (card->string[i] == 'e' || card->string[i] == 'E') {
      i++;
      if (card->string[i] == '+' || card->string[i] == '-') {
        i++;
      }

      if ((i - card->current_index) == value_width ||
          !(card->string[i] >= '0' && card->string[i] <= '9')) {
        END_PROFILE_FUNC();
        return CARD_PARSE_STRING;
      }

      /* Loop until there are no numbers*/
      while (card->string[i] >= '0' && card->string[i] <= '9' &&
             (i - card->current_index) < value_width) {
        i++;
      }

      if (card->string[i] == '\0' || card->string[i] == ' ' ||
          (i - card->current_index) == value_width) {
        END_PROFILE_FUNC();
        return CARD_PARSE_FLOAT;
      }
    }
  } else if (card->string[i] == 'e' || card->string[i] == 'E') {
    i++;
    if (card->string[i] == '+' || card->string[i] == '-') {
      i++;
    }

    if ((i - card->current_index) == value_width ||
        !(card->string[i] >= '0' && card->string[i] <= '9')) {
      END_PROFILE_FUNC();
      return CARD_PARSE_STRING;
    }

    /* Loop until there are no numbers*/
    while (card->string[i] >= '0' && card->string[i] <= '9' &&
           (i - card->current_index) < value_width) {
      i++;
    }

    if (card->string[i] == '\0' || card->string[i] == ' ' ||
        (i - card->current_index) == value_width) {
      END_PROFILE_FUNC();
      return CARD_PARSE_FLOAT;
    }
  }

  END_PROFILE_FUNC();
  return CARD_PARSE_STRING;
}

void _card_cpy(const card_t *card, char *dst, size_t len) {
  BEGIN_PROFILE_FUNC();

  memcpy(dst, card->string, len);

  END_PROFILE_FUNC();
}

int _parse_multi_line_string(string_builder_t *multi_line_string,
                             const card_t *card, size_t line_length) {
  BEGIN_PROFILE_FUNC();

  size_t card_string_index = 0;

  if (multi_line_string->cap == 0) {
    /* Trim leading white space*/
    while (card_string_index != line_length &&
           card->string[card_string_index] == ' ') {
      card_string_index++;
    }
    if (card_string_index == line_length) {
      string_builder_append_char(multi_line_string, '\0');
      END_PROFILE_FUNC();
      return 1;
    }
  }

  line_length -= card_string_index;

  string_builder_append_len(multi_line_string, &card->string[card_string_index],
                            line_length);

  if (multi_line_string->buffer[multi_line_string->ptr - 2] == ' ' &&
      multi_line_string->buffer[multi_line_string->ptr - 1] == '+') {
    /* We have a multi line file name*/
    multi_line_string->ptr -= 2;

    END_PROFILE_FUNC();
    return 0;
  }

  /* Trim trailing white space*/
  while (multi_line_string->buffer[multi_line_string->ptr - 1] == ' ' &&
         multi_line_string->ptr != 0) {
    multi_line_string->ptr--;
  }

  multi_line_string->buffer[multi_line_string->ptr] = '\0';

  END_PROFILE_FUNC();
  return 1;
}
