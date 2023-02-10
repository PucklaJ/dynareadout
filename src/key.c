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
#include "profiling.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KEY_COMMENT '$'
#define LINE_WIDTH 80

#define ERROR_ERRNO(msg)                                                       \
  *error_string = malloc(1024);                                                \
  sprintf(*error_string, msg, strerror(errno));

typedef struct {
  keyword_t *current_keyword;
  keyword_t *keywords;
  size_t *num_keywords;
} key_file_parse_data;

void key_file_parse_callback(const char *keyword_name, const card_t *card,
                             size_t card_index, void *user_data) {
  key_file_parse_data *data = (key_file_parse_data *)user_data;

  if (!data->current_keyword ||
      strcmp(data->current_keyword->name, keyword_name) != 0) {
    size_t index = 0;

    if (data->keywords) {
      int found;
      index = key_file_binary_search_insert(
          data->keywords, 0, *data->num_keywords - 1, keyword_name, &found);

      if (found) {
        index++;
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

    /* Always allocate LINE_WIDTH bytes*/
    data->current_keyword->name = malloc(LINE_WIDTH);
    strcpy(data->current_keyword->name, keyword_name);
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
  keyword_card->string = malloc(LINE_WIDTH + 1);
  memcpy(keyword_card->string, card->string, LINE_WIDTH + 1);
}

keyword_t *key_file_parse(const char *file_name, size_t *num_keywords,
                          char **error_string) {
  BEGIN_PROFILE_FUNC();

  key_file_parse_data data;
  data.current_keyword = NULL;
  data.keywords = NULL;
  data.num_keywords = num_keywords;

  key_file_parse_with_callback(file_name, key_file_parse_callback, error_string,
                               &data);

  END_PROFILE_FUNC();
  return data.keywords;
}

void key_file_parse_with_callback(const char *file_name,
                                  key_file_callback callback,
                                  char **error_string, void *user_data) {
  BEGIN_PROFILE_FUNC();

  *error_string = NULL;

  FILE *file = fopen(file_name, "r");
  if (!file) {
    ERROR_ERRNO("Failed to open key file: %s")
    END_PROFILE_FUNC();
    return;
  }

  char current_keyword_name[LINE_WIDTH + 1];
  size_t card_index = 0;

  char line[LINE_WIDTH + 1];

  while (1) {
    const int n = fread(line, 1, LINE_WIDTH, file);
    if (n == 0 && feof(file)) {
      break;
    }

    /* Find the new line*/
    size_t i = 0;
    while (i < (size_t)n) {
      if (line[i] == '\n') {
        break;
      }

      i++;
    }

    /* New line is exactly after the line (perfect!)*/
    if (i == (size_t)n) {
      char buffer = '\0';
      while (buffer != '\n') {
        fread(&buffer, 1, 1, file);
      }
    } else if (i != 0) {
      /* New line is somewhere within LINE_WIDTH characters*/
      /* Seek back to the beginning of the line that has been "accidentally"
       * read*/
      fseek(file, i - n + 1, SEEK_CUR);
    } else {
      /* Line is just a new line*/
      continue;
    }

    line[i] = '\0';

    /* Check if the line starts with a comment*/
    int is_comment = 0;
    size_t comment_index = (size_t)~0;
    i = 0;
    while (line[i] != '\0') {
      if (line[i] == KEY_COMMENT) {
        comment_index = i;
        is_comment = 1 - (is_comment / 2);
        break;
      } else if (line[i] != ' ') {
        is_comment = 2;
      }

      i++;
    }

    if (is_comment && is_comment != 2) {
      continue;
    }

    if (comment_index != (size_t)~0) {
      line[comment_index] = '\0';
    }

    /* ------- 🐉 Here be parsings 🐉 --------- */

    /* Check if the line is a keyword (starts with '*')
     * Support lines being preceded by ' ' */
    i = 0;
    while (line[i] == ' ') {
      i++;
    }
    const int is_keyword = line[i] == '*';

    if (is_keyword) {
      memcpy(current_keyword_name, &line[i + 1], LINE_WIDTH + 1);

      /* Quit on "END"*/
      if (strcmp(current_keyword_name, "END") == 0) {
        break;
      }

      card_index = 0;
    } else {
      /* This means that we have a card*/
      card_t card;
      card.string = line;

      callback(current_keyword_name, &card, card_index, user_data);

      card_index++;
    }

    /* ---------------------------------------- */
  }

  /* Call the callback for the last keyword if it is not "END", because some
   * keywords can have no cards*/
  if (strcmp(current_keyword_name, "END") != 0) {
    callback(current_keyword_name, NULL, (size_t)~0, user_data);
  }

  fclose(file);

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
    return NULL;
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
    i += j;
  }

  if (j == index && strcmp(keywords[i].name, name) == 0) {
    keyword_t *value = &keywords[i];
    END_PROFILE_FUNC();
    return value;
  }

  END_PROFILE_FUNC();
  return NULL;
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

int card_parse_int(const card_t *card) {
  BEGIN_PROFILE_FUNC();

  const char temp = card->string[card->current_index + card->value_width];
  card->string[card->current_index + card->value_width] = '\0';

  const int value = atoi(&card->string[card->current_index]);

  card->string[card->current_index + card->value_width] = temp;

  END_PROFILE_FUNC();
  return value;
}

int card_parse_int_width(const card_t *card, uint8_t value_width) {
  BEGIN_PROFILE_FUNC();

  const char temp = card->string[card->current_index + value_width];
  card->string[card->current_index + value_width] = '\0';

  const int value = atoi(&card->string[card->current_index]);

  card->string[card->current_index + value_width] = temp;

  END_PROFILE_FUNC();
  return value;
}

float card_parse_float32(const card_t *card) {
  /* atof always uses double*/
  return (float)card_parse_float64(card);
}

float card_parse_float32_width(const card_t *card, uint8_t value_width) {
  /* atof always uses double*/
  return (float)card_parse_float64_width(card, value_width);
}

double card_parse_float64(const card_t *card) {
  BEGIN_PROFILE_FUNC();

  const char temp = card->string[card->current_index + card->value_width];
  card->string[card->current_index + card->value_width] = '\0';

  const double value = atof(&card->string[card->current_index]);

  card->string[card->current_index + card->value_width] = temp;

  END_PROFILE_FUNC();
  return value;
}

double card_parse_float64_width(const card_t *card, uint8_t value_width) {
  BEGIN_PROFILE_FUNC();

  const char temp = card->string[card->current_index + value_width];
  card->string[card->current_index + value_width] = '\0';

  const double value = atof(&card->string[card->current_index]);

  card->string[card->current_index + value_width] = temp;

  END_PROFILE_FUNC();
  return value;
}

char *card_parse_string(const card_t *card) {
  BEGIN_PROFILE_FUNC();

  char *value = malloc(card->value_width + 1);
  memcpy(value, &card->string[card->current_index], card->value_width);
  value[card->value_width] = '\0';

  END_PROFILE_FUNC();
  return value;
}

char *card_parse_string_width(const card_t *card, uint8_t value_width) {
  BEGIN_PROFILE_FUNC();

  char *value = malloc(value_width + 1);
  memcpy(value, &card->string[card->current_index], value_width);
  value[value_width] = '\0';

  END_PROFILE_FUNC();
  return value;
}

const char *card_parse_whole(const card_t *card) {
  BEGIN_PROFILE_FUNC();

  const char *value = card->string;

  END_PROFILE_FUNC();
  return value;
}
