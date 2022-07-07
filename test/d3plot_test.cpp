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
#include <d3plot.h>
#include <doctest/doctest.h>

#define CHECK_APPROX(lhs, rhs) CHECK_LT(fabs(lhs - rhs), 10e-6)

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

TEST_CASE("d3plot") {
  d3plot_file plot_file = d3plot_open("test_data/d3plot");
  if (plot_file.error_string) {
    FAIL(plot_file.error_string);
    d3plot_close(&plot_file);
    return;
  }

  REQUIRE(plot_file.num_states == 102);

  size_t num_nodes;
  d3_word *node_ids = d3plot_read_node_ids(&plot_file, &num_nodes);

  REQUIRE(num_nodes == 114893);
  CHECK(node_ids[59530] == 84285019);
  CHECK(node_ids[0] == 10);
  CHECK(node_ids[114892] == 84340381);
  CHECK(node_ids[2458] == 2852);

  free(node_ids);

  size_t num_elements;
  d3_word *element_ids = d3plot_read_all_element_ids(&plot_file, &num_elements);

  REQUIRE(num_elements == 133456);
  CHECK(element_ids[0] == 1);
  CHECK(element_ids[1] == 2);
  CHECK(element_ids[2] == 3);
  CHECK(element_ids[3] == 4);
  CHECK(element_ids[133318] == 72044862);

  free(element_ids);

  CHECK_APPROX(d3plot_read_time(&plot_file, 0), 0.0);
  CHECK_APPROX(d3plot_read_time(&plot_file, 1), 0.0999492854);
  CHECK_APPROX(d3plot_read_time(&plot_file, 2), 0.1998985708);
  CHECK_APPROX(d3plot_read_time(&plot_file, 3), 0.2998797894);
  CHECK_APPROX(d3plot_read_time(&plot_file, 4), 0.399907);
  CHECK_APPROX(d3plot_read_time(&plot_file, 5), 0.499967);
  CHECK_APPROX(d3plot_read_time(&plot_file, 6), 0.599917);
  CHECK_APPROX(d3plot_read_time(&plot_file, 7), 0.699972);
  CHECK_APPROX(d3plot_read_time(&plot_file, 8), 0.799929);
  CHECK_APPROX(d3plot_read_time(&plot_file, 9), 0.899985);
  CHECK_APPROX(d3plot_read_time(&plot_file, 10), 0.999915);
  CHECK_APPROX(d3plot_read_time(&plot_file, 11), 1.099944);
  CHECK_APPROX(d3plot_read_time(&plot_file, 12), 1.199965);
  CHECK_APPROX(d3plot_read_time(&plot_file, 13), 1.299985);
  CHECK_APPROX(d3plot_read_time(&plot_file, 14), 1.399986);
  CHECK_APPROX(d3plot_read_time(&plot_file, 15), 1.499983);
  CHECK_APPROX(d3plot_read_time(&plot_file, 16), 1.599941);
  CHECK_APPROX(d3plot_read_time(&plot_file, 17), 1.699989);
  CHECK_APPROX(d3plot_read_time(&plot_file, 18), 1.799880);
  CHECK_APPROX(d3plot_read_time(&plot_file, 19), 1.899986);

  double *node_data = d3plot_read_node_coordinates(&plot_file, 0, &num_nodes);
  REQUIRE(num_nodes == 114893);
  CHECK_APPROX(node_data[0], 0.031293001);
  CHECK_APPROX(node_data[1], -0.075000003);
  CHECK_APPROX(node_data[2], 69.1887664795);

  free(node_data);

  node_data = d3plot_read_node_velocity(&plot_file, 0, &num_nodes);
  REQUIRE(num_nodes == 114893);
  CHECK_APPROX(node_data[0], 0.0);
  CHECK_APPROX(node_data[1], 0.0);
  CHECK_APPROX(node_data[2], 0.0);

  free(node_data);

  node_data = d3plot_read_node_acceleration(&plot_file, 0, &num_nodes);
  REQUIRE(num_nodes == 114893);
  CHECK_APPROX(node_data[0], 0.0);
  CHECK_APPROX(node_data[1], 0.0);
  CHECK_APPROX(node_data[2], 0.0);

  free(node_data);

  d3plot_close(&plot_file);
}

TEST_CASE("_get_nth_digit") {
  const d3_word value = 123456;
  CHECK(_get_nth_digit(value, 0) == 6);
  CHECK(_get_nth_digit(value, 1) == 5);
  CHECK(_get_nth_digit(value, 2) == 4);
  CHECK(_get_nth_digit(value, 3) == 3);
  CHECK(_get_nth_digit(value, 4) == 2);
  CHECK(_get_nth_digit(value, 5) == 1);

  const d3_word value1 = 10;
  CHECK(_get_nth_digit(value1, 0) == 0);
  CHECK(_get_nth_digit(value1, 1) == 1);
  CHECK(_get_nth_digit(value1, 2) == 0);
  CHECK(_get_nth_digit(value1, 3) == 0);
  CHECK(_get_nth_digit(value1, 4) == 0);
}

TEST_CASE("_insert_sorted") {
  {
    const d3_word dst[7] = {1, 2, 3, 10, 11, 12, 13};
    const d3_word src[6] = {4, 5, 6, 7, 8, 9};
    d3_word *_dst = (d3_word *)malloc(7 * sizeof(d3_word));
    memcpy(_dst, dst, sizeof(dst));

    _dst = _insert_sorted(_dst, 7, src, 6);
    size_t i = 1;
    while (i <= 13) {
      CHECK(_dst[i - 1] == i);
      i++;
    }

    free(_dst);
  }
  {
    const d3_word dst[4] = {10, 11, 12, 13};
    const d3_word src[6] = {4, 5, 6, 7, 8, 9};
    d3_word *_dst = (d3_word *)malloc(4 * sizeof(d3_word));
    memcpy(_dst, dst, sizeof(dst));

    _dst = _insert_sorted(_dst, 4, src, 6);
    size_t i = 4;
    while (i <= 13) {
      CHECK(_dst[i - 4] == i);
      i++;
    }

    free(_dst);
  }
  {
    const d3_word dst[6] = {4, 5, 6, 7, 8, 9};
    const d3_word src[4] = {10, 11, 12, 13};
    d3_word *_dst = (d3_word *)malloc(6 * sizeof(d3_word));
    memcpy(_dst, dst, sizeof(dst));

    _dst = _insert_sorted(_dst, 6, src, 4);
    size_t i = 4;
    while (i <= 13) {
      CHECK(_dst[i - 4] == i);
      i++;
    }

    free(_dst);
  }
}