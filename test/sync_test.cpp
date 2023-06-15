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
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <doctest/doctest.h>
#include <multi_file.h>
#include <path.h>

#ifdef THREAD_SAFE
TEST_CASE("sync") {
  sync_t snc = sync_create();
  CHECK(sync_lock(&snc) == 0);
  CHECK(sync_unlock(&snc) == 0);

  CHECK(sync_trylock(&snc) == 0);
  CHECK(sync_trylock(&snc) == EBUSY);
  CHECK(sync_trylock(&snc) == EBUSY);

  CHECK(sync_unlock(&snc) == 0);
  sync_destroy(&snc);
}
#endif

TEST_CASE("multi_file") {
  if (!path_is_file("test_data/multi_file_test")) {
    FILE *file = fopen("test_data/multi_file_test", "wb");
    if (!file) {
      FAIL(strerror(errno));
      return;
    }

    fprintf(file, "Hello World!");

    fclose(file);
  }

  multi_file_t f = multi_file_open("test_data/multi_file_test");

  size_t is[3] = {multi_file_access(&f), multi_file_access(&f),
                  multi_file_access(&f)};

  CHECK(is[0] != is[1]);
  CHECK(is[1] != is[2]);
  CHECK(is[0] != is[2]);

  char data[13];
  data[12] = '\0';

  multi_file_read(&f, is[0], data, 1, 12);
  CHECK(data == "Hello World!");
  multi_file_read(&f, is[1], data, 1, 12);
  CHECK(data == "Hello World!");
  multi_file_read(&f, is[2], data, 1, 12);
  CHECK(data == "Hello World!");

  CHECK(multi_file_tell(&f, is[0]) == 12);
  CHECK(multi_file_tell(&f, is[1]) == 12);
  CHECK(multi_file_tell(&f, is[2]) == 12);

  multi_file_seek(&f, is[0], 0, SEEK_SET);
  multi_file_seek(&f, is[1], 0, SEEK_SET);
  multi_file_seek(&f, is[2], 0, SEEK_SET);

  CHECK(multi_file_read(&f, is[0], data, 1, 5) == 5);
  data[5] = '\0';
  CHECK(data == "Hello");

  multi_file_seek(&f, is[1], 6, SEEK_SET);
  CHECK(multi_file_read(&f, is[1], data, 1, 5) == 5);
  CHECK(data == "World");

  multi_file_read(&f, is[2], data, 1, 2);
  data[2] = '\0';
  CHECK(data == "He");

  CHECK(multi_file_tell(&f, is[0]) == 5);
  CHECK(multi_file_tell(&f, is[1]) == 11);
  CHECK(multi_file_tell(&f, is[2]) == 2);

  multi_file_return(&f, is[0]);
  multi_file_return(&f, is[1]);
  multi_file_return(&f, is[2]);

  multi_file_close(&f);
}