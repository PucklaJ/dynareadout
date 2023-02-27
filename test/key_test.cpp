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
#ifdef BUILD_CPP
#include <key.hpp>
#endif

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
  {
    char *error_string;
    size_t num_keywords;
    keyword_t *keywords =
        key_file_parse("test_data/schinken.k", &num_keywords, 0, &error_string);
    CHECK(error_string != NULL);
    CHECK(keywords == NULL);
    free(error_string);

    CHECK(key_file_parse("test_data/schinken.k", &num_keywords, 0, NULL) ==
          NULL);
  }

  char *error_string;
  size_t num_keywords;
  keyword_t *keywords =
      key_file_parse("test_data/key_file.k", &num_keywords, 1, &error_string);
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
  CHECK(errno == EINVAL);
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

  keyword = key_file_get(keywords, num_keywords, "SET_PART_LIST_TITLE", 0);
  REQUIRE(keyword != NULL);
  CHECK(keyword->num_cards == 3);
  card = &keyword->cards[1];
  card_parse_begin(card, DEFAULT_VALUE_WIDTH);
  card_parse_next(card);
  card_parse_next(card);
  card_parse_next(card);
  card_parse_next(card);
  card_parse_next(card);
  CHECK(card_parse_done(card) == 0);
  CHECK(card_parse_string(card) == "");
  CHECK(card_parse_string_no_trim(card) == "          ");
  CHECK(card_parse_int(card) == 0);
  CHECK(card_parse_float32(card) == 0.0f);
  CHECK(card_parse_float64(card) == 0.0);

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

  keyword = key_file_get(keywords, num_keywords, "TEST_KEYWORD", 0);
  REQUIRE(keyword != NULL);
  card = &keyword->cards[0];
  card_parse_begin(card, DEFAULT_VALUE_WIDTH);
  CHECK(card_parse_float32(card) == 7.89f);
  CHECK(card_parse_float64(card) == 7.89);
  card = &keyword->cards[1];
  card_parse_begin(card, DEFAULT_VALUE_WIDTH);
  CHECK(card_parse_float32(card) == 12E1f);
  CHECK(card_parse_float64(card) == 12E1);
  card = &keyword->cards[2];
  card_parse_begin(card, DEFAULT_VALUE_WIDTH);
  CHECK(card_parse_float32(card) == -4.16e-5f);
  CHECK(card_parse_float64(card) == -4.16e-5);
  card = &keyword->cards[3];
  card_parse_begin(card, DEFAULT_VALUE_WIDTH);
  CHECK(card_parse_float32(card) == 0.0f);
  CHECK(errno == EINVAL);
  CHECK(card_parse_float64(card) == 0.0);
  CHECK(errno == EINVAL);
  card = &keyword->cards[4];
  card_parse_begin(card, DEFAULT_VALUE_WIDTH);
  CHECK(card_parse_float32(card) == 4.0f);
  CHECK(errno == EINVAL);
  CHECK(card_parse_float64(card) == 4.0);
  CHECK(errno == EINVAL);
  card = &keyword->cards[5];
  card_parse_begin(card, DEFAULT_VALUE_WIDTH);
  CHECK(card_parse_float32(card) == 0.0f);
  CHECK(errno == EINVAL);
  CHECK(card_parse_float64(card) == 0.0);
  CHECK(errno == EINVAL);
  card = &keyword->cards[6];
  card_parse_begin(card, DEFAULT_VALUE_WIDTH);
  CHECK(card_parse_float32(card) == 0.0f);
  CHECK(errno == EINVAL);
  CHECK(card_parse_float64(card) == 0.0);
  CHECK(errno == EINVAL);
  card = &keyword->cards[7];
  card_parse_begin(card, DEFAULT_VALUE_WIDTH);
  CHECK(card_parse_float32(card) == 5.0f);
  CHECK(errno == EINVAL);
  CHECK(card_parse_float64(card) == 5.0);
  CHECK(errno == EINVAL);
  card = &keyword->cards[8];
  card_parse_begin(card, DEFAULT_VALUE_WIDTH);
  CHECK(card_parse_float32(card) == 567.0f);
  CHECK(card_parse_float64(card) == 567.0);

  keyword = key_file_get(keywords, num_keywords, "KEYWORD_OF_INCLUDE", 0);
  REQUIRE(keyword != NULL);
  CHECK(keyword->num_cards == 1);
  card = &keyword->cards[0];
  CHECK(card_parse_whole_no_trim(card) == "I am inside an include");

  keyword =
      key_file_get(keywords, num_keywords, "KEYWORD_OF_INCLUDE_INCLUDE", 0);
  REQUIRE(keyword != NULL);
  CHECK(keyword->num_cards == 1);
  card = &keyword->cards[0];
  CHECK(card_parse_whole_no_trim(card) == "I am inside another include");

  keyword = key_file_get(keywords, num_keywords, "KEYWORD_OF_MORE", 0);
  REQUIRE(keyword != NULL);
  CHECK(keyword->num_cards == 1);
  card = &keyword->cards[0];
  CHECK(card_parse_whole_no_trim(card) == "I am more");

  keyword = key_file_get(keywords, num_keywords, "KEYWORD_OF_LESS", 0);
  REQUIRE(keyword != NULL);
  CHECK(keyword->num_cards == 1);
  card = &keyword->cards[0];
  CHECK(card_parse_whole_no_trim(card) == "This is less");

  keyword = key_file_get(keywords, num_keywords, "USER_LOADING", 0);
  REQUIRE(keyword != NULL);
  CHECK(keyword->num_cards == 0);

  keyword = key_file_get(keywords, num_keywords, "USER_NOT_LOADING", 0);
  REQUIRE(keyword != NULL);
  CHECK(keyword->num_cards == 0);

  keyword = key_file_get(keywords, num_keywords, "NODE", 0);
  REQUIRE(keyword != NULL);

  card = &keyword->cards[26];
  card_parse_begin(card, DEFAULT_VALUE_WIDTH);
  CHECK(card_parse_string_width(card, 8) == "27");
  card_parse_next_width(card, 8);
  CHECK(card_parse_string_width(card, 16) == "-10.0");
  card_parse_next_width(card, 16);
  CHECK(card_parse_string_width(card, 16) == "0.125169");
  card_parse_next_width(card, 16);
  CHECK(card_parse_string_width(card, 16) == "20.0");
  CHECK(card_parse_done(card) == 0);

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
        REQUIRE(keyword_name != NULL);

        if (!card) {
          return;
        }

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
      1, &error_string, NULL, NULL, NULL);
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

  {
    const char *cstr = "Hello World My dudes";
    extra_string str = extra_string_new(cstr);
    CHECK(extra_string_starts_with(&str, "Hello") != 0);
    CHECK(extra_string_starts_with(&str, "Helo") == 0);
    CHECK(extra_string_starts_with(&str, "Hello World") != 0);
    CHECK(extra_string_starts_with(
              &str, "Hello World My dudes, you are awesome") == 0);
    CHECK(extra_string_starts_with(&str, "Hello World My dudes") != 0);
  }

  {
    const char *cstr = "Hello World My incredible dudes, you are as always an "
                       "awesome bunch, with which it is so nice to work with, "
                       "but I have some bad news for you.";
    extra_string str = extra_string_new(cstr);
    CHECK(extra_string_starts_with(&str, "Hello") != 0);
    CHECK(extra_string_starts_with(&str, "Helo") == 0);
    CHECK(extra_string_starts_with(&str, "Hello World") != 0);
    CHECK(extra_string_starts_with(&str, "Hello World My incredible dudes") !=
          0);

    CHECK(extra_string_starts_with(
              &str,
              "Hello World My incredible dudes, you are as always an awesome "
              "bunch, with which it is so nice to work with, ") != 0);
    CHECK(extra_string_starts_with(
              &str,
              "Hello World My incredible dudes, you are as always an awesome "
              "bunch, with which it is so nice to work with, 1") == 0);
    CHECK(extra_string_starts_with(
              &str, "Hello World My incredible dudes, you are as always an "
                    "awesome bunch, with which it is so nice to work with, "
                    "but I have some bad news for you. The news are.") != 0);
    CHECK(extra_string_starts_with(
              &str, "Hello World My incredible dudes, you are as always an "
                    "awesome bunch, with which it is so nice to work with, "
                    "but I have some bad news for you.") != 0);

    free(str.extra);
  }
}

#ifdef BUILD_CPP
#define FABS(x) ((x) > 0 ? (x) : -(x))

TEST_CASE("key_file_parseC++") {
  auto keywords = dro::KeyFile::parse("test_data/key_file.k");

  // Takes the first found keyword of "MAT_PIECEWISE_LINEAR_PLASTICITY_TITLE"
  // (in this case is only one) and the second card of that keyword
  auto card = keywords["MAT_PIECEWISE_LINEAR_PLASTICITY_TITLE"][0][1];

  card.begin();
  CHECK(card.done() == false);
  CHECK(card.parse<int>() == 6);
  CHECK(card.parse<unsigned int>() == 6);
  card.next();
  CHECK(card.done() == false);
  CHECK(FABS(card.parse<double>() - 1.57e-6) < 1e-10);
  card.next();
  CHECK(card.done() == false);
  CHECK(card.parse<float>() == 11.05647f);
  CHECK(card.parse<double>() == 11.05647);
  card.next();
  CHECK(card.done() == false);
  CHECK(card.parse<float>() == 0.3f);
  CHECK(card.parse<double>() == 0.3);
  card.next();
  CHECK(card.done() == false);
  CHECK(card.parse<float>() == 0.01597f);
  CHECK(card.parse<double>() == 0.01597);
  card.next();
  CHECK(card.done() == false);
  CHECK(card.parse<float>() == 0.0f);
  CHECK(card.parse<double>() == 0.0);
  card.next();
  CHECK(card.done() == false);
  CHECK(card.parse<float>() == 1e21f);
  CHECK(card.parse<double>() == 1e21);
  card.next();
  CHECK(card.done() == false);
  CHECK(card.parse<float>() == 0.0);
  CHECK(card.parse<double>() == 0.0f);
  card.next();
  CHECK(card.done() == true);

  card = keywords["TEST_KEYWORD_CPP"][0][0];

  card.begin();
  CHECK(card.parse<int>() == -10);
  try {
    card.parse<unsigned int>();
    FAIL("parse should fail, because it tries to read a negative value and "
         "convert it into an unsigned int");
  } catch (const dro::KeyFile::Exception &e) {
    CHECK(e.what() != NULL);
  }

  card = keywords["PART"][1][0];

  card.begin();
  CHECK(card.parse<char *>() == "Ground");
  auto str = card.parse<dro::String>();
  CHECK(str.data() == "Ground");
  CHECK(card.parse<std::string>() == "Ground");

  card = keywords[""][0][0];

  card.begin();
  CHECK(card.parse_string_whole<char *>() == "Start of File");
  str = card.parse_string_whole<dro::String>();
  CHECK(str.data() == "Start of File");
  CHECK(card.parse_string_whole<std::string>() == "Start of File");

  CHECK(
      card.parse_string_whole_no_trim<char *>() ==
      "                                 Start of File                        ");
  str = card.parse_string_whole_no_trim<dro::String>();
  CHECK(
      str.data() ==
      "                                 Start of File                        ");
  CHECK(
      card.parse_string_whole_no_trim<std::string>() ==
      "                                 Start of File                        ");

  card = keywords["NODE"][0][26];

  card.begin();
  CHECK(card.parse<char *>(8) == "27");
  card.next(8);
  CHECK(card.parse<char *>(16) == "-10.0");
  card.next(16);
  CHECK(card.parse<char *>(16) == "0.125169");
  card.next(16);
  CHECK(card.parse<char *>(16) == "20.0");
  CHECK(card.done() == false);

  card = keywords["SET_NODE_LIST_TITLE"][0][1];

  {
    int sid;
    float da1, da2, da3, da4;
    std::string solver;

    card.parse_whole(sid, da1, da2, da3, da4, solver);

    CHECK(sid == 1);
    CHECK(da1 == 0.0f);
    CHECK(da2 == 0.0f);
    CHECK(da3 == 0.0f);
    CHECK(da4 == 0.0f);
    CHECK(solver == "MECH");
  }

  {
    int sid;
    float da1, da2, da3, da4;
    std::string solver;

    card.parse_whole_width({0, 0, 0, 0, 0, 0}, sid, da1, da2, da3, da4, solver);

    CHECK(sid == 1);
    CHECK(da1 == 0.0f);
    CHECK(da2 == 0.0f);
    CHECK(da3 == 0.0f);
    CHECK(da4 == 0.0f);
    CHECK(solver == "MECH");
  }

  {
    auto [_sid, _da1, _da2, _da3, _da4, _solver] =
        card.parse_whole<int, float, float, float, float, std::string>();

    const auto sid = _sid;
    const auto da1 = _da1;
    const auto da2 = _da2;
    const auto da3 = _da3;
    const auto da4 = _da4;
    const auto solver = _solver;

    CHECK(sid == 1);
    CHECK(da1 == 0.0f);
    CHECK(da2 == 0.0f);
    CHECK(da3 == 0.0f);
    CHECK(da4 == 0.0f);
    CHECK(solver == "MECH");
  }

  {
    auto [_sid, _da1, _da2, _da3, _da4, _solver] =
        card.parse_whole<int, float, float, float, float, std::string>(
            {0, 0, 0, 0, 0, 0});

    const auto sid = _sid;
    const auto da1 = _da1;
    const auto da2 = _da2;
    const auto da3 = _da3;
    const auto da4 = _da4;
    const auto solver = _solver;

    CHECK(sid == 1);
    CHECK(da1 == 0.0f);
    CHECK(da2 == 0.0f);
    CHECK(da3 == 0.0f);
    CHECK(da4 == 0.0f);
    CHECK(solver == "MECH");
  }

  {
    try {
      card.parse_whole<int, float, float, float, float, std::string,
                       std::string>();
      FAIL("parse_whole should throw an exception because it tries to read too "
           "much values");
    } catch (const dro::KeyFile::Exception &e) {
      CHECK(e.what() != NULL);
    }
  }

  {
    try {
      int sid;
      float da1, da2, da3, da4;
      std::string solver, crash_causer;

      card.parse_whole(sid, da1, da2, da3, da4, solver, crash_causer);
      FAIL("parse_whole should throw an exception because it tries to read too "
           "much values");
    } catch (const dro::KeyFile::Exception &e) {
      CHECK(e.what() != NULL);
    }
  }

  card = keywords["NODE"][0][2];

  {
    int nid, tc, rc;
    float x, y, z;

    card.parse_whole_width({8, 16, 16, 16, 8, 8}, nid, x, y, z, tc, rc);

    CHECK(nid == 3);
    CHECK(x == 10.0f);
    CHECK(y == -10.0f);
    CHECK(z == 0.0f);
    CHECK(tc == 0);
    CHECK(rc == 0);
  }

  {
    auto [_nid, _x, _y, _z, _tc, _rc] =
        card.parse_whole<int, float, float, float, int, int>(
            {8, 16, 16, 16, 8, 8});

    const auto nid = _nid;
    const auto x = _x;
    const auto y = _y;
    const auto z = _z;
    const auto tc = _tc;
    const auto rc = _rc;

    CHECK(nid == 3);
    CHECK(x == 10.0f);
    CHECK(y == -10.0f);
    CHECK(z == 0.0f);
    CHECK(tc == 0);
    CHECK(rc == 0);
  }

  {
    try {
      card.parse_whole<int, float, float, float, int, int, std::string>(
          {8, 16, 16, 16, 8, 8, 12});
      FAIL("parse_whole should throw an exception because it tries to read too "
           "much values");
    } catch (const dro::KeyFile::Exception &e) {
      CHECK(e.what() != NULL);
    }
  }

  {
    try {
      int nid, tc, rc;
      float x, y, z;
      std::string crash_causer;

      card.parse_whole_width({8, 16, 16, 16, 8, 8, 12}, nid, x, y, z, tc, rc,
                             crash_causer);
      FAIL("parse_whole should throw an exception because it tries to read too "
           "much values");
    } catch (const dro::KeyFile::Exception &e) {
      CHECK(e.what() != NULL);
    }
  }
}

TEST_CASE("key_file_parse_with_callbackC++") {
  dro::KeyFile::parse_with_callback(
      "test_data/key_file.k",
      [](dro::String keyword_name, dro::Card card, size_t card_index) {
        const auto card_str = card.parse_string_whole_no_trim<dro::String>();
        std::cout << keyword_name << "[" << card_index << "]: " << card_str
                  << std::endl;
      });
}
#endif