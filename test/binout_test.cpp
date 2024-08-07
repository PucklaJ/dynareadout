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

#include <stdio.h>
#define DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#include <binout.h>
#include <binout_defines.h>
#include <binout_directory.h>
#include <binout_glob.h>
#include <cstdint>
#include <cstring>
#include <doctest/doctest.h>
#include <iomanip>
#include <iostream>
#include <path.h>
#include <path_view.h>
#include <sstream>
#include <string>
#include <string_builder.h>
#ifdef BUILD_CPP
#include "main_test.hpp"
#include <algorithm>
#include <binout.hpp>
#endif

// Returns wether value can be found insed arr
bool strarr_contains(char *const *arr, const size_t size, const char *value) {
  for (size_t i = 0; i < size; i++) {
    if (strcmp(arr[i], value) == 0) {
      return true;
    }
  }
  return false;
}

TEST_CASE("binout0000") {
  {
    binout_file bin_file = binout_open("test_data/i_dont_exist");
    char *open_error = binout_open_error(&bin_file);
    if (open_error) {
      CHECK(open_error == "test_data/i_dont_exist: No files have been found");
      free(open_error);
    } else {
      FAIL("No error occurred");
    }
    binout_close(&bin_file);
  }

  const char *binout_file_name = "test_data/binout0*";

  binout_file bin_file = binout_open(binout_file_name);
  char *open_error = binout_open_error(&bin_file);
  if (open_error) {
    FAIL(open_error);
    free(open_error);
    binout_close(&bin_file);
    return;
  }

  int timed = 1234;
  uint8_t type_id = BINOUT_TYPE_INVALID;
  CHECK(binout_simple_path_to_real(&bin_file, "nodout", &type_id, &timed) ==
        "/nodout");
  CHECK(timed == 0);
  CHECK(type_id == BINOUT_TYPE_INVALID);
  CHECK(binout_simple_path_to_real(&bin_file, "/nodout", &type_id, &timed) ==
        "/nodout");
  CHECK(timed == 0);
  CHECK(type_id == BINOUT_TYPE_INVALID);
  CHECK(binout_simple_path_to_real(&bin_file, "nodout/ids", &type_id, &timed) ==
        "/nodout/metadata/ids");
  CHECK(timed == 0);
  CHECK(type_id == BINOUT_TYPE_INT64);
  CHECK(binout_simple_path_to_real(&bin_file, "nodout/schinken", &type_id,
                                   &timed) == NULL);
  CHECK(timed == 0);
  CHECK(type_id == BINOUT_TYPE_INVALID);
  CHECK(binout_simple_path_to_real(&bin_file, "/", &type_id, &timed) == "/");
  CHECK(timed == 0);
  CHECK(type_id == BINOUT_TYPE_INVALID);
  CHECK(binout_simple_path_to_real(&bin_file, "/nodout/x_displacement",
                                   &type_id,
                                   &timed) == "/nodout/x_displacement");
  CHECK(timed == 1);
  CHECK(type_id == BINOUT_TYPE_FLOAT32);
  CHECK(binout_simple_path_to_real(&bin_file, "nodout/d000001/x_displacement",
                                   &type_id,
                                   &timed) == "/nodout/d000001/x_displacement");
  CHECK(timed == 0);
  CHECK(type_id == BINOUT_TYPE_FLOAT32);
  CHECK(binout_simple_path_to_real(&bin_file, "//////nodout////ids/////",
                                   &type_id, &timed) == "/nodout/metadata/ids");
  CHECK(timed == 0);
  CHECK(type_id == BINOUT_TYPE_INT64);
  CHECK(binout_simple_path_to_real(&bin_file,
                                   "//////nodout/////metadata////ids/////",
                                   &type_id, &timed) == "/nodout/metadata/ids");
  CHECK(timed == 0);
  CHECK(type_id == BINOUT_TYPE_INT64);
  CHECK(binout_simple_path_to_real(
            &bin_file, "//////nodout/////d000001////x_displacement/////",
            &type_id, &timed) == "/nodout/d000001/x_displacement");
  CHECK(timed == 0);
  CHECK(type_id == BINOUT_TYPE_FLOAT32);
  CHECK(binout_simple_path_to_real(&bin_file, "/nodout/d000100/u_displacement",
                                   &type_id, &timed) == NULL);
  CHECK(binout_simple_path_to_real(&bin_file, "/nodout/metadata/schinken",
                                   &type_id, &timed) == NULL);
  CHECK(binout_simple_path_to_real(&bin_file, "/nodout/metadata/ids/schinken",
                                   &type_id, &timed) == NULL);

  size_t num_binout_children;
  char **binout_children =
      binout_get_children(&bin_file, "/", &num_binout_children);
  REQUIRE(num_binout_children >= 2);

  REQUIRE(strarr_contains(binout_children, num_binout_children, "nodout") ==
          true);
  REQUIRE(strarr_contains(binout_children, num_binout_children, "rcforc") ==
          true);

  binout_free_children(binout_children);

  CHECK(binout_get_num_timesteps(&bin_file, "/nodout") >= 601);

  binout_children =
      binout_get_children(&bin_file, "/nodout", &num_binout_children);
  REQUIRE(num_binout_children >= 602);
  CHECK(binout_children[num_binout_children - 1] == "metadata");
  for (size_t i = 0; i < 601; i++) {
    std::stringstream stream;
    stream << "d" << std::setfill('0') << std::right << std::setw(6) << (i + 1);
    const std::string str(stream.str());

    CHECK(binout_children[i] == str.c_str());
  }

  binout_free_children(binout_children);

  binout_children =
      binout_get_children(&bin_file, "/nodout/metadata/", &num_binout_children);
  REQUIRE(num_binout_children == 7);
  CHECK(binout_children[0] == "date");
  CHECK(binout_children[1] == "ids");
  CHECK(binout_children[2] == "legend");
  CHECK(binout_children[3] == "legend_ids");
  CHECK(binout_children[4] == "revision");
  CHECK(binout_children[5] == "title");
  CHECK(binout_children[6] == "version");

  binout_free_children(binout_children);

  REQUIRE(binout_variable_exists(&bin_file, "/nodout/metadata/legend"));
  REQUIRE(binout_get_type_id(&bin_file, "/nodout/metadata/legend") ==
          BINOUT_TYPE_INT8);

  size_t legend_size;
  int8_t *legend =
      binout_read_i8(&bin_file, "/nodout/metadata/legend", &legend_size);
  REQUIRE(legend);
  CHECK(legend_size == 80);
  if (binout_get_num_timesteps(&bin_file, "/nodout") == 601) {
    legend = (int8_t *)realloc(legend, legend_size + 1);
    legend[legend_size] = '\0';
    CHECK((const char *)legend ==
          "History_node_1                                                 "
          "                 ");
    free(legend);
  }

  REQUIRE(binout_variable_exists(&bin_file, "/nodout/metadata/ids"));
  REQUIRE(binout_get_type_id(&bin_file, "/nodout/metadata/ids") ==
          BINOUT_TYPE_INT64);

  size_t node_ids_size;
  int64_t *node_ids =
      binout_read_i64(&bin_file, "/nodout/metadata/ids", &node_ids_size);
  REQUIRE(node_ids);
  CHECK(node_ids_size == 1);
  free(node_ids);

  REQUIRE(binout_variable_exists(&bin_file, "/rcforc/metadata/title"));
  REQUIRE(binout_get_type_id(&bin_file, "/rcforc/metadata/title") ==
          BINOUT_TYPE_INT8);

  size_t title_size;
  int8_t *title =
      binout_read_i8(&bin_file, "/rcforc/metadata/title", &title_size);
  REQUIRE(title);
  CHECK(title_size == 80);
  if (binout_get_num_timesteps(&bin_file, "/rcforc") == 601) {
    title = (int8_t *)realloc(title, title_size + 1);
    title[title_size] = '\0';
    CHECK((const char *)title ==
          "Pouch_macro_37Ah                                                 "
          "               ");
    free(title);
  }

  size_t num_nodes, num_timesteps;
  float *y_displacement = binout_read_timed_f32(
      &bin_file, "/nodout/y_displacement", &num_nodes, &num_timesteps);
  REQUIRE(y_displacement);
  CHECK(num_nodes == 1);
  REQUIRE(num_timesteps >= 601);

  if (num_timesteps == 601) {
    for (size_t t = 0; t < 601; t++) {
      for (size_t i = 0; i < num_nodes; i++) {
        if (t == 0) {
          CHECK(y_displacement[t * num_nodes + i] == 0.0f);
        } else {
          CHECK(y_displacement[t * num_nodes + i] != 0.0f);
        }
      }
    }
  }

  free(y_displacement);

  int32_t *cycle = binout_read_timed_i32(&bin_file, "/nodout/cycle", &num_nodes,
                                         &num_timesteps);
  REQUIRE(cycle);
  CHECK(num_nodes == 1);
  REQUIRE(num_timesteps >= 601);

  CHECK(cycle[0 * num_nodes + 0] == 1);
  CHECK(cycle[1 * num_nodes + 0] == 10);
  CHECK(cycle[2 * num_nodes + 0] == 20);
  CHECK(cycle[99 * num_nodes + 0] == 945);
  CHECK(cycle[600 * num_nodes + 0] == 5723);

  free(cycle);

  binout_close(&bin_file);
}

TEST_CASE("ncforc.binout") {
  binout_file b = binout_open("test_data/ncforc.binout");
  if (b.error_string) {
    FAIL(b.error_string);
    binout_close(&b);
    return;
  }

  uint8_t type_id;
  int timed;
  char *real_path =
      binout_simple_path_to_real(&b, "ncforc/master_100000", &type_id, &timed);
  REQUIRE(real_path != NULL);
  CHECK(real_path == "/ncforc/master_100000");
  CHECK(type_id == BINOUT_TYPE_INVALID);
  free(real_path);

  real_path = binout_simple_path_to_real(&b, "ncforc/master_100000/x_force",
                                         &type_id, &timed);
  REQUIRE(real_path != NULL);
  CHECK(real_path == "/ncforc/master_100000/x_force");
  CHECK(type_id == BINOUT_TYPE_FLOAT32);
  CHECK(timed != 0);
  free(real_path);

  real_path = binout_simple_path_to_real(&b, "ncforc/master_100000/ids",
                                         &type_id, &timed);
  REQUIRE(real_path != NULL);
  CHECK(real_path == "/ncforc/master_100000/metadata/ids");
  CHECK(type_id == BINOUT_TYPE_INT32);
  CHECK(timed == 0);
  free(real_path);

  size_t num_values, num_timesteps;
  float *data = binout_read_timed_f32(&b, "/ncforc/master_100000/x_force",
                                      &num_values, &num_timesteps);
  CHECK(data != NULL);
  CHECK(num_values != 0);
  CHECK(num_timesteps != 0);
  free(data);

  binout_close(&b);
}

TEST_CASE("rwforc") {
  binout_file b = binout_open("test_data/rwforc_sim/binout");
  if (b.error_string) {
    FAIL(b.error_string);
    binout_close(&b);
    return;
  }

  uint8_t type_id;
  int timed;
  char *real =
      binout_simple_path_to_real(&b, "rwforc/forces/x_force", &type_id, &timed);
  REQUIRE(real != NULL);
  CHECK(real == "/rwforc/forces/x_force");
  CHECK(type_id == BINOUT_TYPE_FLOAT32);
  CHECK(timed == 1);

  size_t nv = 0, nt = 0;
  float *v = binout_read_timed_f32(&b, real, &nv, &nt);
  free(real);
  if (b.error_string) {
    FAIL(b.error_string);
  }
  CHECK(v != NULL);
  CHECK(nv != 0);
  CHECK(nt != 0);
  free(v);

  real = binout_simple_path_to_real(&b, "rwforc/transducer/x_force", &type_id,
                                    &timed);
  REQUIRE(real != NULL);
  CHECK(real == "/rwforc/transducer/x_force");
  CHECK(type_id == BINOUT_TYPE_FLOAT32);
  CHECK(timed == 1);
  free(real);

  binout_close(&b);
}

TEST_CASE("binout_multi") {
  binout_file binout = binout_open("test_data/binout_multi/binout*");
  if (binout.error_string) {
    FAIL(binout.error_string);
    binout_close(&binout);
    return;
  }

  uint8_t type_id;
  int timed;
  const char *real_path = binout_simple_path_to_real(
      &binout, "bndout/velocity/rigidbodies/misc/z_moment0", &type_id, &timed);
  REQUIRE(real_path != NULL);
  REQUIRE(timed != 0);
  CHECK(type_id == BINOUT_TYPE_FLOAT32);

  size_t num_values, num_timesteps;
  float *data =
      binout_read_timed_f32(&binout, real_path, &num_values, &num_timesteps);
  if (binout.error_string) {
    FAIL(binout.error_string);
  }
  REQUIRE(data != NULL);

  CHECK(num_timesteps == 1000);
  CHECK(num_values == 1);

  CHECK(data[000] == 31.256439f);
  CHECK(data[999] == 50.736660f);
  CHECK(data[499] == 40.736500f);
  free(data);

  binout_close(&binout);
}

TEST_CASE("binout_MATSUM") {
  binout_file binout = binout_open("test_data/matsum/binout");
  if (binout.error_string) {
    FAIL(binout.error_string);
    binout_close(&binout);
    return;
  }

  size_t num_values, num_timesteps;
  float *data = binout_read_timed_f32(&binout, "/matsum/kinetic_energy",
                                      &num_values, &num_timesteps);
  if (binout.error_string != NULL) {
    FAIL(binout.error_string);
  }

  REQUIRE(data != NULL);
  CHECK(num_values == 754);
  CHECK(num_timesteps == 151);

  free(data);

  binout_close(&binout);
}

#ifdef BUILD_CPP
TEST_CASE("binout0000C++") {
  {
    try {
      dro::Binout bin_file("test_data/i_dont_exist");
      FAIL("No exception occurred");
    } catch (const dro::Binout::Exception &e) {
      CHECK(e.what() == "test_data/i_dont_exist: No files have been found");
    }
  }

  dro::Binout bin_file("test_data/binout0*");

  dro::BinoutType type_id;
  bool timed;
  CHECK(bin_file.simple_path_to_real("nodout", type_id, timed) == "/nodout");

  try {
    bin_file.simple_path_to_real("/nodout/schinken", type_id, timed);
    FAIL("simple_path_to_real should have thrown an exception");
  } catch (const dro::Binout::Exception &) {
  }

  {
    const auto children = bin_file.get_children("/");
    REQUIRE(children.size() == 5);
    CHECK(children[0] == dro::String(strdup("bndout")));
    CHECK(children[1] == dro::SizedString(strdup("glstat"), strlen("glstat")));
    CHECK(children[1] == std::string("glstat"));
  }

  CHECK(bin_file.get_num_timesteps("/nodout") == 14998);

  try {
    bin_file.get_num_timesteps("/schinken");
    FAIL("Binout::get_num_timesteps should throw an exception if an invalid "
         "path is supplied");
  } catch (const dro::Binout::Exception &e) {
    CHECK(e.what() == "The path does not exist or has files as children");
  }

  try {
    bin_file.get_num_timesteps("/nodout/schinken");
    FAIL("Binout::get_num_timesteps should throw an exception if an invalid "
         "path is supplied");
  } catch (const dro::Binout::Exception &e) {
    CHECK(e.what() == "The path does not exist or has files as children");
  }

  {
    const auto children = bin_file.get_children("/nodout/metadata/");
    REQUIRE(children.size() == 7);
    CHECK(children[0] == "date");
    CHECK(children[1] == "ids");
    CHECK(children[2] == "legend");
    CHECK(children[3] == "legend_ids");
    CHECK(children[4] == "revision");
    CHECK(children[5] == "title");
    CHECK(children[6] == "version");
  }

  REQUIRE(bin_file.variable_exists("/nodout/metadata//legend"));
  REQUIRE(bin_file.get_type_id("/nodout/metadata//legend") ==
          dro::BinoutType::Int8);

  {
    const auto legend = bin_file.read<int8_t>("/nodout/metadata//legend");
    REQUIRE(legend.size() == 80);
    CHECK(legend ==
          "                                                               "
          "                 ");
  }

  REQUIRE(bin_file.variable_exists("/nodout/metadata/ids"));
  REQUIRE(bin_file.get_type_id("/nodout/metadata/ids") ==
          dro::BinoutType::Int64);

  {
    const auto node_ids = bin_file.read<int64_t>("/nodout/metadata/ids");
    REQUIRE(node_ids.size() == 1);
  }

  REQUIRE(bin_file.variable_exists("/rcforc/metadata/title"));
  REQUIRE(bin_file.get_type_id("/rcforc/metadata/title") ==
          dro::BinoutType::Int8);

  {
    const auto title = bin_file.read<int8_t>("/rcforc/metadata/title");
    CHECK(title.size() == 80);
    CHECK(title == "LS-DYNA keyword deck by LS-PrePost                         "
                   "                     ");
  }

  {
    const auto z_displacement =
        bin_file.read_timed<float>("/nodout/z_displacement");
    CHECK(z_displacement.size() == 14998);
    CHECK(z_displacement[0].size() == 1);

    for (size_t t = 0; t < z_displacement.size(); t++) {
      for (size_t i = 0; i < z_displacement[t].size(); i++) {
        if (t == 0) {
          CHECK(z_displacement[t][i] == 0.0f);
        } else {
          CHECK(z_displacement[t][i] != 0.0f);
        }
      }
    }
  }

  {
    const auto cycle = bin_file.read_timed<int32_t>("/nodout/cycle");
    CHECK(cycle.size() == 14998);
    CHECK(cycle[0].size() == 1);

    CHECK(cycle[0][0] == 1);
    CHECK(cycle[1][0] == 10);
    CHECK(cycle[2][0] == 20);
    CHECK(cycle[99][0] == 945);
    CHECK(cycle[600][0] == 5723);
  }
}

TEST_CASE("Array") {
  SUBCASE("Iterator") {
    int *data = (int *)malloc(5 * sizeof(int));
    data[0] = 1;
    data[1] = 2;
    data[2] = 3;
    data[3] = 4;
    data[4] = 5;

    dro::Array<int> v1(data, 5);
    int counter = 0;
    for (const auto i : v1) {
      CHECK(i == (counter + 1));
      counter++;
    }
    CHECK(counter == 5);

    counter = 0;
    for (auto iter = v1.begin(); iter != v1.end(); iter++) {
      CHECK(*iter == (counter + 1));
      counter++;
    }
    CHECK(counter == 5);

    CHECK(std::find(v1.begin(), v1.end(), 5) != v1.end());
    CHECK(std::find(v1.begin(), v1.end(), 6) == v1.end());

    for (auto &i : v1) {
      i += 5;
    }
    CHECK(v1[0] == 6);
    CHECK(v1[1] == 7);
    CHECK(v1[2] == 8);
    CHECK(v1[3] == 9);
    CHECK(v1[4] == 10);
  }

  SUBCASE("binary_search") {
    {
      int *is_arr = (int *)malloc(10 * sizeof(int));
      is_arr[0] = 0;
      is_arr[1] = 6;
      is_arr[2] = 6;
      is_arr[3] = 7;
      is_arr[4] = 8;
      is_arr[5] = 9;
      is_arr[6] = 20;
      is_arr[7] = 56;
      is_arr[8] = 178;
      is_arr[9] = 290;

      const dro::Array<int> is(is_arr, 10);

      CHECK(std::binary_search(is.begin(), is.end(), 8) == true);
      CHECK(std::binary_search(is.begin(), is.end(), 177) == false);
    }

    auto is = dro::Array<int>::New(10);
    is[0] = 0;
    is[1] = 6;
    is[2] = 6;
    is[3] = 7;
    is[4] = 8;
    is[5] = 9;
    is[6] = 20;
    is[7] = 56;
    is[8] = 178;
    is[9] = 290;

    CHECK(std::binary_search(is.begin(), is.end(), 8) == true);
    CHECK(std::binary_search(is.begin(), is.end(), 177) == false);
  }

  SUBCASE("const") {
    int *data = (int *)malloc(sizeof(int) * 5);
    data[0] = 1;
    data[1] = 2;
    data[2] = 4;
    data[3] = 8;
    data[4] = 16;

    const dro::Array<int> d(data, 5);

    int i = 1;
    for (auto it = d.begin(); it != d.end(); it++, i *= 2) {
      CHECK(*it == i);
    }
  }

  SUBCASE("move/copy") {
    dro::Array<int> m;

    {
      auto d = dro::Array<int>::New(5);
      d[0] = 1;
      d[1] = 2;
      d[2] = 3;
      d[3] = 4;
      d[4] = 5;
      m = std::move(d);
    }

    CHECK(m[0] == 1);
    CHECK(m[1] == 2);
    CHECK(m[2] == 3);
    CHECK(m[3] == 4);
    CHECK(m[4] == 5);

    dro::Array<int> n = m;
    for (auto &i : m) {
      i *= 2;
    }

    CHECK(m[0] == 2);
    CHECK(m[1] == 4);
    CHECK(m[2] == 6);
    CHECK(m[3] == 8);
    CHECK(m[4] == 10);

    CHECK(n[0] == 1);
    CHECK(n[1] == 2);
    CHECK(n[2] == 3);
    CHECK(n[3] == 4);
    CHECK(n[4] == 5);
  }
}
#endif

TEST_CASE("glob") {
  size_t num_files;
  char **globed_files = binout_glob("src/*.c", &num_files);

  CHECK(num_files == 20);
  CHECK(strarr_contains(globed_files, num_files, "src/binary_search.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/binout_directory.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/binout_glob.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/binout_read.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/binout.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/d3_buffer.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/d3plot_data.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/d3plot_part_nodes.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/d3plot_state.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/d3plot.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/extra_string.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/include_transform.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/key.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/line.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/multi_file.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/path_view.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/path.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/profiling.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/string_builder.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/sync.c"));
  binout_free_glob(globed_files, num_files);
}

TEST_CASE("string_builder") {
  string_builder_t b = string_builder_new();

  string_builder_append_char(&b, '/');
  string_builder_append(&b, "ncforc");
  string_builder_append_char(&b, '/');
  string_builder_append(&b, "master_100000");
  string_builder_append_char(&b, '/');
  string_builder_append(&b, "metadata");
  string_builder_append_char(&b, '/');
  string_builder_append(&b, "ids");

  char *path = string_builder_move(&b);
  string_builder_free(&b);

  CHECK(path == "/ncforc/master_100000/metadata/ids");
  free(path);
}

#ifdef BUILD_CPP
TEST_CASE("Array::New") {
  auto arr = dro::Array<int>::New(5);
  arr[0] = 1;
  arr[1] = 10;
  arr[2] = -3;
  arr[3] = 500;
  arr[4] = 670;
  try {
    arr[5] = 1;
  } catch (const std::runtime_error &) {
    CHECK(arr[0] == 1);
    CHECK(arr[1] == 10);
    CHECK(arr[2] == -3);
    CHECK(arr[3] == 500);
    CHECK(arr[4] == 670);

    return;
  }
  FAIL("arr[5] should have thrown an exception");
}
#endif

char *stralloc(const char *str) {
  const int len = strlen(str);
  char *data = reinterpret_cast<char *>(malloc(len + 1));
  memcpy(data, str, len + 1);
  return data;
}

TEST_CASE("binout_directory") {
  binout_directory_t dir;
  dir.children = NULL;
  dir.num_children = 0;

  /*
    /
    I--nodout
         I--metadata
                I--ids
                I--time
         I--d000001
                I--x_displacement
                I--y_displacement
    I--nodfor
         I--metadata
                I--ids
                I--time
         I--d000010
                I--x_force
                I--y_force
  */

  path_view_t p1 = path_view_new("/nodfor");
  path_view_t p2 = path_view_new("/nodout");

  path_view_advance(&p1);
  path_view_advance(&p2);

  // Insert '/nodfor'
  binout_directory_insert_folder(&dir, &p1);
  // Insert '/nodout'
  binout_directory_insert_folder(&dir, &p2);

  p1 = path_view_new("d000001");
  p2 = path_view_new("metadata");

  // Insert 'd000001' into '/nodout'
  binout_folder_insert_folder(&dir.children[1], &p1);
  // Insert 'metadata' into '/nodout'
  binout_folder_insert_folder(&dir.children[1], &p2);
  // Insert 'ids' into '/nodout/metadata'
  binout_folder_insert_file(
      &reinterpret_cast<binout_entry_t *>(dir.children[1].children)[1],
      stralloc("ids"), BINOUT_TYPE_INT32, 10, 0, 200);
  // Insert 'time' into '/nodout/metadata'
  binout_folder_insert_file(
      &reinterpret_cast<binout_entry_t *>(dir.children[1].children)[1],
      stralloc("time"), BINOUT_TYPE_FLOAT32, 1, 0, 180);
  // Insert 'x_displacement' into '/nodout/d000001'
  binout_folder_insert_file(
      &reinterpret_cast<binout_entry_t *>(dir.children[1].children)[0],
      stralloc("x_displacement"), BINOUT_TYPE_FLOAT64, 10, 0, 300);
  // Insert 'y_displacement' into '/nodout/d000001'
  binout_folder_insert_file(
      &reinterpret_cast<binout_entry_t *>(dir.children[1].children)[0],
      stralloc("y_displacement"), BINOUT_TYPE_FLOAT64, 10, 0, 380);

  p1 = path_view_new("metadata");
  p2 = path_view_new("d000010");

  // Insert 'metadata' into '/nodfor'
  binout_folder_insert_folder(&dir.children[0], &p1);
  // Insert 'd000010' into '/nodfor'
  binout_folder_insert_folder(&dir.children[0], &p2);

  // Insert 'ids' into '/nodfor/metadata'
  binout_folder_insert_file(
      &reinterpret_cast<binout_entry_t *>(dir.children[0].children)[1],
      stralloc("ids"), BINOUT_TYPE_INT64, 10, 1, 10);
  // Insert 'time' into '/nodfor/metadata'
  binout_folder_insert_file(
      &reinterpret_cast<binout_entry_t *>(dir.children[0].children)[1],
      stralloc("time"), BINOUT_TYPE_FLOAT64, 1, 1, 20);
  // Insert 'x_force' into '/nodfor/d000010'
  binout_folder_insert_file(
      &reinterpret_cast<binout_entry_t *>(dir.children[0].children)[0],
      stralloc("x_force"), BINOUT_TYPE_FLOAT64, 10, 1, 100);
  // Insert 'y_force' into '/nodfor/d000010'
  binout_folder_insert_file(
      &reinterpret_cast<binout_entry_t *>(dir.children[0].children)[0],
      stralloc("y_force"), BINOUT_TYPE_FLOAT64, 10, 1, 150);

  {
    p1 = path_view_new("/nodout/metadata/ids");
    const auto *file = binout_directory_get_file(&dir, &p1);
    REQUIRE(file != nullptr);
    CHECK(file->type == BINOUT_FILE);
    CHECK(file->name == "ids");
    CHECK(file->var_type == BINOUT_TYPE_INT32);
    CHECK(file->size == 10);
    CHECK(file->file_index == 0);
    CHECK(file->file_pos == 200);
  }

  {
    p1 = path_view_new("/nodout/metadata/time");
    const auto *file = binout_directory_get_file(&dir, &p1);
    REQUIRE(file != nullptr);
    CHECK(file->type == BINOUT_FILE);
    CHECK(file->name == "time");
    CHECK(file->var_type == BINOUT_TYPE_FLOAT32);
    CHECK(file->size == 1);
    CHECK(file->file_index == 0);
    CHECK(file->file_pos == 180);
  }

  {
    p1 = path_view_new("/nodout/d000001/x_displacement");
    const auto *file = binout_directory_get_file(&dir, &p1);
    REQUIRE(file != nullptr);
    CHECK(file->type == BINOUT_FILE);
    CHECK(file->name == "x_displacement");
    CHECK(file->var_type == BINOUT_TYPE_FLOAT64);
    CHECK(file->size == 10);
    CHECK(file->file_index == 0);
    CHECK(file->file_pos == 300);
  }

  {
    p1 = path_view_new("/nodout/d000001/y_displacement");
    const auto *file = binout_directory_get_file(&dir, &p1);
    REQUIRE(file != nullptr);
    CHECK(file->type == BINOUT_FILE);
    CHECK(file->name == "y_displacement");
    CHECK(file->var_type == BINOUT_TYPE_FLOAT64);
    CHECK(file->size == 10);
    CHECK(file->file_index == 0);
    CHECK(file->file_pos == 380);
  }

  {
    p1 = path_view_new("/nodfor/metadata/ids");
    const auto *file = binout_directory_get_file(&dir, &p1);
    REQUIRE(file != nullptr);
    CHECK(file->type == BINOUT_FILE);
    CHECK(file->name == "ids");
    CHECK(file->var_type == BINOUT_TYPE_INT64);
    CHECK(file->size == 10);
    CHECK(file->file_index == 1);
    CHECK(file->file_pos == 10);
  }

  {
    p1 = path_view_new("/nodfor/metadata/time");
    const auto *file = binout_directory_get_file(&dir, &p1);
    REQUIRE(file != nullptr);
    CHECK(file->type == BINOUT_FILE);
    CHECK(file->name == "time");
    CHECK(file->var_type == BINOUT_TYPE_FLOAT64);
    CHECK(file->size == 1);
    CHECK(file->file_index == 1);
    CHECK(file->file_pos == 20);
  }

  {
    p1 = path_view_new("/nodfor/d000010/x_force");
    const auto *file = binout_directory_get_file(&dir, &p1);
    REQUIRE(file != nullptr);
    CHECK(file->type == BINOUT_FILE);
    CHECK(file->name == "x_force");
    CHECK(file->var_type == BINOUT_TYPE_FLOAT64);
    CHECK(file->size == 10);
    CHECK(file->file_index == 1);
    CHECK(file->file_pos == 100);
  }

  {
    p1 = path_view_new("/nodfor/d000010/y_force");
    const auto *file = binout_directory_get_file(&dir, &p1);
    REQUIRE(file != nullptr);
    CHECK(file->type == BINOUT_FILE);
    CHECK(file->name == "y_force");
    CHECK(file->var_type == BINOUT_TYPE_FLOAT64);
    CHECK(file->size == 10);
    CHECK(file->file_index == 1);
    CHECK(file->file_pos == 150);
  }

  {
    path_view_t p = path_view_new("/nodout/metadata");

    size_t num_children;
    const binout_entry_t *folder_or_file =
        binout_directory_get_children(&dir, &p, &num_children);

    REQUIRE(num_children == 2);
    REQUIRE(folder_or_file->type == BINOUT_FILE);
    CHECK(((binout_entry_t *)folder_or_file)[0].name == "ids");
    CHECK(((binout_entry_t *)folder_or_file)[1].name == "time");
  }

  binout_directory_free(&dir);
}

TEST_CASE("path_move_up") {
  CHECK(path_move_up("/nodout/metadata/ids") == 16);
  CHECK(path_move_up("/nodout/metadata") == 7);
  CHECK(path_move_up("/nodout") == 0);

  CHECK(path_move_up("/nodout/////////metadata//////ids") == 24);
  CHECK(path_move_up("////nodout/////metadata") == 10);
  CHECK(path_move_up("/////nodout/////") == 0);
  CHECK(path_move_up("/") == (size_t)~0);
  CHECK(path_move_up("/////") == (size_t)~0);

  CHECK(path_move_up("file.txt") == (size_t)~0);

  CHECK(path_move_up("/home/user/file.txt") == 10);
}

TEST_CASE("path_join") {
  CHECK(path_join("Hello", "World") == "Hello/World");
  CHECK(path_join("Hello/", "World") == "Hello/World");
  CHECK(path_join("Hello/", "/World") == "Hello/World");
  CHECK(path_join("Hello/////", "//////World") == "Hello/World");
  CHECK(path_join("Hello", "/World") == "Hello/World");
}

TEST_CASE("path_is_abs") {
#ifdef _WIN32
  CHECK(path_is_abs("C:\\temp") != 0);
#else
  CHECK(path_is_abs("/tmp") != 0);
#endif
}

TEST_CASE("path_is_file") {
  CHECK(path_is_file("test_data/key_file.k") != 0);
  CHECK(path_is_file("test_data/binout0000") != 0);
  CHECK(path_is_file("test_data/schinken") == 0);
}

TEST_CASE("path_view") {
  {
    const char *str = "/nodout/metadata/ids";

    path_view_t pv = path_view_new(str);
    CHECK(path_view_strcmp(&pv, "/") == 0);

    CHECK(path_view_advance(&pv) == 1);
    CHECK(path_view_strcmp(&pv, "nodout") == 0);
    CHECK(path_view_strcmp(&pv, "apple") > 0);
    CHECK(path_view_strcmp(&pv, "nodout_ids") < 0);

    CHECK(path_view_advance(&pv) == 1);
    CHECK(path_view_strcmp(&pv, "metadata") == 0);
    CHECK(path_view_strcmp(&pv, "metatime") < 0);
    CHECK(path_view_strcmp(&pv, "metaapple") > 0);
    CHECK(path_view_strcmp(&pv, "metadata_extra") < 0);

    CHECK(path_view_advance(&pv) == 1);
    CHECK(path_view_strcmp(&pv, "ids") == 0);

    CHECK(path_view_advance(&pv) == 0);
    CHECK(path_view_strcmp(&pv, "ids") == 0);
  }

  {
    const char *str = "metadata/build";

    path_view_t pv = path_view_new(str);
    CHECK(path_view_strcmp(&pv, "metadata") == 0);

    CHECK(path_view_advance(&pv) == 1);
    CHECK(path_view_strcmp(&pv, "build") == 0);

    CHECK(path_view_advance(&pv) == 0);
    CHECK(path_view_strcmp(&pv, "build") == 0);
  }

  {
    const char *str = "x_force";

    path_view_t pv = path_view_new(str);
    CHECK(path_view_strcmp(&pv, "x_force") == 0);

    CHECK(path_view_advance(&pv) == 0);
    CHECK(path_view_strcmp(&pv, "x_force") == 0);
  }

  {
    const char *str = "/nodout/metadata//legend";

    path_view_t pv = path_view_new(str);
    CHECK(path_view_advance(&pv) == 1);
    CHECK(path_view_advance(&pv) == 1);
    CHECK(path_view_advance(&pv) == 1);

    CHECK(path_view_strcmp(&pv, "legend") == 0);
  }

  {
    path_view_t pv = path_view_new("/");
    CHECK(pv.start == 0);
    CHECK(pv.end == 0);

    pv = path_view_new("//");
    CHECK(pv.start == 1);
    CHECK(pv.end == 1);

    pv = path_view_new("//////////");
    CHECK(pv.start == 9);
    CHECK(pv.end == 9);

    pv = path_view_new("///nodout/metadata");
    CHECK(pv.start == 2);
    CHECK(pv.end == 2);

    CHECK(path_view_advance(&pv) == 1);
    CHECK(pv.start == 3);
    CHECK(pv.end == 8);
  }

  {
    path_view_t pv = path_view_new("nodout/metadata/");
    CHECK(pv.start == 0);
    CHECK(pv.end == 5);
    CHECK(path_view_advance(&pv) == 1);
    CHECK(pv.start == 7);
    CHECK(pv.end == 14);
    CHECK(path_view_advance(&pv) == 0);
    CHECK(pv.start == 7);
    CHECK(pv.end == 14);

    pv = path_view_new("n/m//");
    CHECK(pv.start == 0);
    CHECK(pv.end == 0);
    CHECK(path_view_advance(&pv) == 1);
    CHECK(pv.start == 2);
    CHECK(pv.end == 2);
    CHECK(path_view_advance(&pv) == 0);
    CHECK(pv.start == 2);
    CHECK(pv.end == 2);

    pv = path_view_new("a/b/////////");
    CHECK(pv.start == 0);
    CHECK(pv.end == 0);
    CHECK(path_view_advance(&pv) == 1);
    CHECK(pv.start == 2);
    CHECK(pv.end == 2);
    CHECK(path_view_advance(&pv) == 0);
    CHECK(pv.start == 2);
    CHECK(pv.end == 2);

    pv = path_view_new("//////////////nodout//////////////metadata///////////"
                       "ids///////////////");
    CHECK(pv.start == 13);
    CHECK(pv.end == 13);
    CHECK(path_view_advance(&pv) == 1);
    CHECK(pv.start == 14);
    CHECK(pv.end == 19);
    CHECK(path_view_advance(&pv) == 1);
    CHECK(pv.start == 34);
    CHECK(pv.end == 41);
    CHECK(path_view_advance(&pv) == 1);
    CHECK(pv.start == 53);
    CHECK(pv.end == 55);
    CHECK(path_view_advance(&pv) == 0);
    CHECK(pv.start == 53);
    CHECK(pv.end == 55);
  }
}
