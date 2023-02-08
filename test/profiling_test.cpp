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

  if (i == 1000) {
    END_PROFILE_FUNC();
    return;
  }

  profile_test_func3(i + 1);

  END_PROFILE_FUNC();
}

void profile_test_func411() {
  BEGIN_PROFILE_FUNC();

  for (size_t i = 0; i < 204987298347;) {
    if (i < 10) {
      i++;
    } else {
      i += (i * i) / (i / 2);
    }
  }

  BEGIN_PROFILE_SECTION(profile_test_4111);

  for (size_t i = 0; i < 204;) {
    if (i < 10) {
      i++;
    } else {
      i += (i * i) / (i / 2);
      cos(i);
    }
  }

  END_PROFILE_SECTION(profile_test_4111);

  BEGIN_PROFILE_SECTION(profile_test_4112);

  for (size_t i = 0; i < 204;) {
    if (i < 10) {
      i++;
    } else {
      i += (i * i) / (i / 2);
      sin(i);
    }
  }

  END_PROFILE_SECTION(profile_test_4112);

  END_PROFILE_FUNC();
}

void profile_test_func41(int i) {
  BEGIN_PROFILE_FUNC();

  int j = 5 * i;
  for (int k = j; k < 234; k++) {
    k = k + 1;
    k = k - 1;
  }

  profile_test_func411();

  END_PROFILE_FUNC();
}

void profile_test_func4() {
  BEGIN_PROFILE_FUNC();

  for (size_t j = 0; j < 10; j++) {
    for (size_t i = 0; i < 2040;) {
      if (i < 10) {
        i++;
      } else {
        i += (i * i) / (i / 2);
        profile_test_func41(i);
      }
    }
  }

  END_PROFILE_FUNC();
}

TEST_CASE("profiling") {
  BEGIN_PROFILE_SECTION(math_equation);
  float math_value = sin(cos(sin(tan(5.0f + tan(5.0f)))));
  for (size_t i = 0; i < 10000; i++)
    math_value += sin(tan(sin(cos(sin(tan(math_value + tan(math_value)))))));
  END_PROFILE_SECTION(math_equation);

  profile_test_func1();
  profile_test_func2();
  profile_test_func3();
  profile_test_func4();

  END_PROFILING("test_data/profiling_test.txt");
}
