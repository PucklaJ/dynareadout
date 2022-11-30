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

#ifdef PROFILING
#define DOCTEST_CONFIG_IMPLEMENT
#include <profiling.h>
#else
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif
#define DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#include <doctest/doctest.h>
#include <iostream>

#ifdef PROFILING
int main(int args, char *argv[]) {
  std::cout << "Running with Profiling" << std::endl;
  doctest::Context ctx;

  ctx.applyCommandLine(args, argv);

  const int res = ctx.run();

  if (ctx.shouldExit()) {
    return res;
  }

  END_PROFILING("test_data/profiling_data.txt");
  return res;
}
#endif
