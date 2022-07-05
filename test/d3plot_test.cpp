/***********************************************************************************
 *                         This file is part of dynareadout
 *                    https://github.com/PucklaMotzer09/dynareadout
 ***********************************************************************************
 * Copyright (c) 2022 PucklaMotzer09
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
#define DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#include <d3_buffer.h>
#include <doctest/doctest.h>

TEST_CASE("d3_buffer") {

  d3_buffer buffer = d3_buffer_open("test_data/d3plot");
  if (buffer.error_string) {
    FAIL(buffer.error_string);
    d3_buffer_close(&buffer);
    return;
  }

  CHECK(buffer.word_size == 4);
  CHECK(buffer.num_file_handles == 28);

  char title[10 * 4 + 1];
  title[10 * 4] = '\0';
  d3_buffer_read_words(&buffer, title, 10);

  CHECK(title == "Pouch_macro_37Ah                        ");

  uint8_t *probe = new uint8_t[40 * 1000 * 1000 * 4];
  d3_buffer_read_words_at(&buffer, probe, 40 * 1000 * 1000, 0);

  // d3plot
  CHECK(probe[0x00000000] == 0x50);
  CHECK(probe[0x0057EFFF] == 0x00);
  // d3plot01
  CHECK(probe[0x0057F000] == 0x00);
  CHECK(probe[0x0057F008] == 0x08);
  CHECK(probe[0x048A8FFF] == 0x00);
  // d3plot02
  CHECK(probe[0x048A9000] == 0x94);
  CHECK(probe[0x048A9008] == 0x00);
  CHECK(probe[0x08BD2FFF] == 0x00);

  delete[] probe;

  d3_buffer_close(&buffer);
}