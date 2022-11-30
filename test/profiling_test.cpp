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

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <cmath>
#include <doctest/doctest.h>
#include <iostream>
#include <profiling.h>

void profile_test_func1() {
  BEGIN_PROFILE_FUNC();

  for (size_t i = 0; i < 204987298347;) {
    if (i < 10) {
      i++;
    } else {
      i += (i * i) / (i / 2);
    }
  }

  END_PROFILE_FUNC();
}

void profile_test_func2() {
  BEGIN_PROFILE_FUNC();

  std::cout << "Hello World" << std::endl;
  std::cout << "I am a function" << std::endl;
  std::cout << "I just want to help you :-)" << std::endl;

  END_PROFILE_FUNC();
}

void profile_test_func3(int i = 0) {
  BEGIN_PROFILE_FUNC();

  if (i == 10000) {
    END_PROFILE_FUNC();
    return;
  }

  profile_test_func3(i + 1);

  END_PROFILE_FUNC();
}

TEST_CASE("profiling") {
  BEGIN_PROFILE_SECTION(math_equation);
  float math_value = sin(cos(sin(tan(5.0f + tan(5.0f)))));
  math_value += sin(cos(sin(tan(math_value + tan(math_value)))));
  END_PROFILE_SECTION(math_equation);

  profile_test_func1();
  profile_test_func2();
  profile_test_func3();

  END_PROFILING("test_data/profiling_test.txt");
}