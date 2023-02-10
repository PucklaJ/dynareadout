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
#include <doctest/doctest.h>
#include <iostream>
#include <key.h>

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
    std::cout << keywords[i].name << ": " << keywords[i].num_cards << std::endl;
  }

  key_file_free(keywords, num_keywords);
}