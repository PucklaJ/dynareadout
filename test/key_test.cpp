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

#define DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#include <cstdlib>
#include <cstring>
#include <doctest/doctest.h>
#include <extra_string.h>
#include <iostream>
#include <key.h>

extra_string extra_string_new(const char *str) {
  extra_string xstr;
  xstr.extra = NULL;
  size_t i = 0;
  while (i < EXTRA_STRING_BUFFER_SIZE) {
    xstr.buffer[i] = str[i];
    if (str[i] == '\0') {
      return xstr;
    }

    i++;
  }

  while (1) {
    xstr.extra = (char *)realloc((void *)xstr.extra, i * sizeof(char));
    size_t j = 0;
    while (j < EXTRA_STRING_BUFFER_SIZE) {
      xstr.extra[(i / EXTRA_STRING_BUFFER_SIZE - 1) * EXTRA_STRING_BUFFER_SIZE +
                 j] = str[i];
      if (str[i] == '\0') {
        return xstr;
      }

      j++;
      i++;
    }
  }
}

TEST_CASE("key_file_parse") {
  char *error_string;
  size_t num_keywords;
  keyword_t *keywords =
      key_file_parse("test_data/key_file.k", &num_keywords, &error_string);
  if (error_string) {
    FAIL(error_string);
    free(error_string);
    return;
  }

  for (size_t i = 0; i < num_keywords; i++) {
    std::cout << keywords[i].name << std::endl;
  }

  keyword_t *keyword = key_file_get(keywords, num_keywords, "TITLE", 0);
  REQUIRE(keyword != NULL);
  CHECK(keyword->num_cards == 1);
  CHECK(card_parse_whole(&keyword->cards[0]) == "Cube_Test");

  keyword = key_file_get(keywords, num_keywords, "KEYWORD", 0);
  REQUIRE(keyword != NULL);
  CHECK(keyword->num_cards == 1);
  CHECK(card_parse_whole(&keyword->cards[0]) == "");

  keyword = key_file_get(keywords, num_keywords, "", 0);
  REQUIRE(keyword != NULL);
  CHECK(keyword->num_cards == 1);
  CHECK(card_parse_whole(&keyword->cards[0]) == "Start of File");
  CHECK(
      card_parse_whole_no_trim(&keyword->cards[0]) ==
      "                                 Start of File                        ");

  keyword = key_file_get(keywords, num_keywords, "PART", 0);
  REQUIRE(keyword != NULL);
  CHECK(keyword->num_cards == 2);

  card_t *card = &keyword->cards[1];
  card_parse_begin(card, DEFAULT_VALUE_WIDTH);
  CHECK(card_parse_int(card) == 71000063);
  card_parse_next(card);
  CHECK(card_parse_done(card) == 0);
  CHECK(card_parse_float32(card) == 71000063.0f);
  card_parse_next(card);
  CHECK(card_parse_done(card) == 0);
  CHECK(card_parse_int(card) == 6);
  card_parse_next(card);
  CHECK(card_parse_done(card) == 0);
  CHECK(card_parse_int(card) == 0);
  card_parse_next(card);
  CHECK(card_parse_done(card) == 0);
  CHECK(card_parse_int(card) == 0);
  card_parse_next(card);
  CHECK(card_parse_done(card) == 0);
  CHECK(card_parse_int(card) == 0);
  card_parse_next(card);
  CHECK(card_parse_done(card) == 0);
  CHECK(card_parse_int(card) == 0);
  card_parse_next(card);
  CHECK(card_parse_done(card) == 0);
  CHECK(card_parse_int(card) == 0);
  card_parse_next(card);
  CHECK(card_parse_done(card) != 0);

  keyword = key_file_get(keywords, num_keywords, "SECTION_SHELL_TITLE", 0);
  REQUIRE(keyword != NULL);
  CHECK(keyword->num_cards == 3);

  card = &keyword->cards[2];
  card_parse_begin(card, DEFAULT_VALUE_WIDTH);
  CHECK(card_parse_float64(card) == 0.15);
  card_parse_next(card);
  CHECK(card_parse_done(card) == 0);
  CHECK(card_parse_float64(card) == 0.15);
  card_parse_next(card);
  CHECK(card_parse_done(card) == 0);
  CHECK(card_parse_float64(card) == 0.15);
  card_parse_next(card);
  CHECK(card_parse_done(card) == 0);
  CHECK(card_parse_float64(card) == 0.15);
  card_parse_next(card);
  CHECK(card_parse_done(card) == 0);
  CHECK(card_parse_float64(card) == 0.0);
  card_parse_next(card);
  CHECK(card_parse_done(card) == 0);
  CHECK(card_parse_float64(card) == 0.0);
  card_parse_next(card);
  CHECK(card_parse_done(card) == 0);
  CHECK(card_parse_float64(card) == 0.0);
  card_parse_next(card);
  CHECK(card_parse_done(card) == 0);
  CHECK(card_parse_int(card) == 0);
  card_parse_next(card);
  CHECK(card_parse_done(card) != 0);

  keyword_t *nodes = key_file_get(keywords, num_keywords, "NODE", 0);
  REQUIRE(nodes != NULL);
  CHECK(nodes->num_cards == 30);
  size_t i = 0;
  while (nodes) {
    size_t j = 0;
    while (j < nodes->num_cards) {
      card_t *node = &nodes->cards[j];

      card_parse_begin(node, NODE_VALUE_WIDTH);
      const int nid = card_parse_int(node);
      card_parse_next(node);
      const double x = card_parse_float64_width(node, NODE_VALUE_WIDTH * 2);
      card_parse_next_width(node, NODE_VALUE_WIDTH * 2);
      const double y = card_parse_float64_width(node, NODE_VALUE_WIDTH * 2);
      card_parse_next_width(node, NODE_VALUE_WIDTH * 2);
      const double z = card_parse_float64_width(node, NODE_VALUE_WIDTH * 2);

      if (j <= 8 && j >= 0) {
        CHECK(y == -10.0);
      } else if (j <= 16 && j >= 9) {
        CHECK(y == -5.0);
      } else if (j <= 25 && j >= 17) {
        CHECK(y == 0.0);
      } else if (j == 26 || j == 28) {
        CHECK(y == 0.125169);
      } else {
        CHECK(y == 0.125161);
      }

      CHECK(x >= -10.0);
      CHECK(x <= 20.0);
      CHECK(z >= -10.0);
      CHECK(z <= 20.0);

      CHECK(nid == (j + 1));

      j++;
    }

    i++;
    nodes = key_file_get(keywords, num_keywords, "NODE", i);
  }

  keyword = key_file_get(keywords, num_keywords, "SET_NODE_LIST_TITLE", 0);
  REQUIRE(keyword != NULL);
  CHECK(keyword->num_cards == 6);
  card = &keyword->cards[1];
  card_parse_begin(card, DEFAULT_VALUE_WIDTH);
  card_parse_next(card);
  card_parse_next(card);
  card_parse_next(card);
  card_parse_next(card);
  card_parse_next(card);
  CHECK(card_parse_done(card) == 0);
  CHECK(card_parse_string(card) == "MECH");
  CHECK(card_parse_string_no_trim(card) == "MECH      ");
  CHECK(card_parse_string_width(card, 1) == "M");
  CHECK(card_parse_string_width_no_trim(card, 5) == "MECH ");

  keyword = key_file_get(keywords, num_keywords, "SET_NODE_LIST_TITLE", 1);
  REQUIRE(keyword != NULL);
  CHECK(keyword->num_cards == 4);
  card = &keyword->cards[1];
  card_parse_begin(card, DEFAULT_VALUE_WIDTH);
  card_parse_next(card);
  card_parse_next(card);
  card_parse_next(card);
  card_parse_next(card);
  card_parse_next(card);
  CHECK(card_parse_done(card) == 0);
  CHECK(card_parse_string(card) == "MECH");

  keyword = key_file_get(keywords, num_keywords, "SET_NODE_LIST_TITLE", 2);
  REQUIRE(keyword != NULL);
  CHECK(keyword->num_cards == 4);
  card = &keyword->cards[2];
  card_parse_begin(card, DEFAULT_VALUE_WIDTH);
  card_parse_next(card);
  card_parse_next(card);
  card_parse_next(card);
  card_parse_next(card);
  card_parse_next(card);
  card_parse_next(card);
  card_parse_next(card);
  CHECK(card_parse_done(card) == 0);
  CHECK(card_parse_float64(card) == 25.05);
  card = &keyword->cards[3];
  card_parse_begin(card, DEFAULT_VALUE_WIDTH);
  card_parse_next(card);
  card_parse_next(card);
  card_parse_next(card);
  card_parse_next(card);
  card_parse_next(card);
  card_parse_next(card);
  card_parse_next(card);
  CHECK(card_parse_done(card) == 0);
  CHECK(card_parse_int(card) == 14);

  i = 0;
  keyword = key_file_get(keywords, num_keywords, "SET_NODE_LIST_TITLE", i++);
  while (keyword) {
    char *title = card_parse_whole_no_trim(&keyword->cards[0]);
    if (i == 1) {
      CHECK(title == "Pouch_nodes_out_displ");
    } else if (i == 2) {
      CHECK(title == "Pressure_Top");
    } else if (i == 3) {
      CHECK(title == "Pressure_Bottom");
    }

    free(title);

    keyword = key_file_get(keywords, num_keywords, "SET_NODE_LIST_TITLE", i++);
  }

  CHECK(i == 4);

  i = 0;
  keyword = key_file_get(keywords, num_keywords, "DEFINE_CURVE_TITLE", i++);
  while (keyword) {
    char *title = card_parse_whole_no_trim(&keyword->cards[0]);
    if (i == 1) {
      CHECK(title == "Pressure");
    } else if (i == 2) {
      CHECK(title == "Pouch_flow");
    } else if (i == 3) {
      CHECK(title == "Velocity Impactor");
    }

    free(title);

    keyword = key_file_get(keywords, num_keywords, "DEFINE_CURVE_TITLE", i++);
  }

  CHECK(i == 4);

  key_file_free(keywords, num_keywords);
}

TEST_CASE("key_file_parse_with_callback") {
  char *error_string;
  size_t num_keywords;

  key_file_parse_with_callback(
      "test_data/key_file.k",
      [](const char *keyword_name, const card_t *card, size_t card_index,
         void *user_data) {
        CHECK(user_data == NULL);
        REQUIRE(card != NULL);
        REQUIRE(keyword_name != NULL);

        if (strcmp(keyword_name, "NODE") == 0) {
          const size_t j = card_index;
          card_t node;
          node.string = card->string;

          card_parse_begin(&node, NODE_VALUE_WIDTH);
          const int nid = card_parse_int(&node);
          card_parse_next(&node);
          const double x =
              card_parse_float64_width(&node, NODE_VALUE_WIDTH * 2);
          card_parse_next_width(&node, NODE_VALUE_WIDTH * 2);
          const double y =
              card_parse_float64_width(&node, NODE_VALUE_WIDTH * 2);
          card_parse_next_width(&node, NODE_VALUE_WIDTH * 2);
          const double z =
              card_parse_float64_width(&node, NODE_VALUE_WIDTH * 2);

          if (j <= 8 && j >= 0) {
            CHECK(y == -10.0);
          } else if (j <= 16 && j >= 9) {
            CHECK(y == -5.0);
          } else if (j <= 25 && j >= 17) {
            CHECK(y == 0.0);
          } else if (j == 26 || j == 28) {
            CHECK(y == 0.125169);
          } else {
            CHECK(y == 0.125161);
          }

          CHECK(x >= -10.0);
          CHECK(x <= 20.0);
          CHECK(z >= -10.0);
          CHECK(z <= 20.0);

          CHECK(nid == (j + 1));
        }
      },
      &error_string, NULL);
  if (error_string) {
    FAIL(error_string);
    free(error_string);
    return;
  }
}

TEST_CASE("extra_string") {
  {
    const char *cstr = "Hello World";
    extra_string str = extra_string_new(cstr);
    CHECK(extra_string_compare(&str, cstr) == 0);
    CHECK(extra_string_compare(&str, "Bye World") > 0);
    CHECK(
        extra_string_compare(
            &str,
            "Hello World. I wanted to introduce myself, but your establishment "
            "made me overthink that thought therefore I will just kindly avoid "
            "your "
            "eye contact until you agree to do what I came for.") < 0);
  }

  {
    const char *cstr =
        "This is a test to test out the capabilities of the world of computer "
        "programming. Because of that I need to exam the very foundation of "
        "the cosmos itself so that you know that I am not overthinking what "
        "you think is otherwise too much for a regular "
        "conversation.";
    extra_string str = extra_string_new(cstr);
    CHECK(extra_string_compare(&str, cstr) == 0);
    CHECK(extra_string_compare(&str, "This") > 0);
    CHECK(extra_string_compare(&str, "Why") < 0);

    CHECK(extra_string_get(&str, 0) == 'T');
    CHECK(extra_string_get(&str, 3) == 's');
    CHECK(extra_string_get(&str, strlen(cstr) - 1) == '.');
    extra_string_set(&str, strlen(cstr) - 1, '-');
    CHECK(extra_string_get(&str, strlen(cstr) - 1) == '-');

    free(str.extra);
  }
}