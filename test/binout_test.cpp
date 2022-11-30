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
#include <binout.h>
#include <binout_defines.h>
#include <binout_directory.h>
#include <binout_glob.h>
#include <cstdint>
#include <doctest/doctest.h>
#include <iomanip>
#include <iostream>
#include <path.h>
#include <path_view.h>
#include <sstream>
#include <string>
#ifdef BINOUT_CPP
#include <binout.hpp>
#endif

namespace dro {
template <typename T> doctest::String toString(const Array<T> &str) {
  static_assert(std::is_same_v<T, char> || std::is_same_v<T, int8_t> ||
                std::is_same_v<T, uint8_t>);

  return doctest::String(reinterpret_cast<const char *>(str.data()),
                         str.size());
}
} // namespace dro

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

  size_t num_binout_children;
  char **binout_children =
      binout_get_children(&bin_file, "/", &num_binout_children);
  REQUIRE(num_binout_children == 2);

  CHECK(binout_children[0] == "nodout");
  CHECK(binout_children[1] == "rcforc");

  binout_free_children(binout_children);

  binout_children =
      binout_get_children(&bin_file, "/nodout", &num_binout_children);
  REQUIRE(num_binout_children == 602);
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
      binout_read_int8_t(&bin_file, "/nodout/metadata/legend", &legend_size);
  REQUIRE(legend);
  CHECK(legend_size == 80);
  legend = (int8_t *)realloc(legend, legend_size + 1);
  legend[legend_size] = '\0';
  CHECK((const char *)legend ==
        "History_node_1                                                 "
        "                 ");
  free(legend);

  REQUIRE(binout_variable_exists(&bin_file, "/nodout/metadata/ids"));
  REQUIRE(binout_get_type_id(&bin_file, "/nodout/metadata/ids") ==
          BINOUT_TYPE_INT64);

  size_t node_ids_size;
  int64_t *node_ids =
      binout_read_int64_t(&bin_file, "/nodout/metadata/ids", &node_ids_size);
  REQUIRE(node_ids);
  CHECK(node_ids_size == 1);
  free(node_ids);

  REQUIRE(binout_variable_exists(&bin_file, "/rcforc/metadata/title"));
  REQUIRE(binout_get_type_id(&bin_file, "/rcforc/metadata/title") ==
          BINOUT_TYPE_INT8);

  size_t title_size;
  int8_t *title =
      binout_read_int8_t(&bin_file, "/rcforc/metadata/title", &title_size);
  REQUIRE(title);
  CHECK(title_size == 80);
  title = (int8_t *)realloc(title, title_size + 1);
  title[title_size] = '\0';
  CHECK((const char *)title ==
        "Pouch_macro_37Ah                                                 "
        "               ");
  free(title);

  binout_close(&bin_file);
}

#ifdef BINOUT_CPP
TEST_CASE("binout0000 C++") {
  {
    try {
      dro::Binout bin_file("test_data/i_dont_exist");
      FAIL("No exception occurred");
    } catch (const dro::Binout::Exception &e) {
      CHECK(e.what() == "test_data/i_dont_exist: No files have been found");
    }
  }

  dro::Binout bin_file("test_data/binout0*");
  {
    const auto children = bin_file.get_children("/");
    REQUIRE(children.size() == 2);
    CHECK(children[0] == "nodout");
    CHECK(children[1] == "rcforc");
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
          "History_node_1                                                 "
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
    CHECK(title ==
          "Pouch_macro_37Ah                                                 "
          "               ");
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
}
#endif

TEST_CASE("glob") {
  size_t num_files;
  char **globed_files = binout_glob("src/*.c", &num_files);

  CHECK(num_files == 10);
  CHECK(strarr_contains(globed_files, num_files, "src/binout_glob.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/binout.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/binout_directory.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/d3_buffer.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/d3plot_data.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/d3plot_state.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/d3plot.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/path.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/path_view.c"));
  CHECK(strarr_contains(globed_files, num_files, "src/profiling.c"));
  binout_free_glob(globed_files, num_files);
}

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
      &reinterpret_cast<binout_folder_t *>(dir.children[1].children)[1],
      stralloc("ids"), BINOUT_TYPE_INT32, 10, 0, 200);
  // Insert 'time' into '/nodout/metadata'
  binout_folder_insert_file(
      &reinterpret_cast<binout_folder_t *>(dir.children[1].children)[1],
      stralloc("time"), BINOUT_TYPE_FLOAT32, 1, 0, 180);
  // Insert 'x_displacement' into '/nodout/d000001'
  binout_folder_insert_file(
      &reinterpret_cast<binout_folder_t *>(dir.children[1].children)[0],
      stralloc("x_displacement"), BINOUT_TYPE_FLOAT64, 10, 0, 300);
  // Insert 'y_displacement' into '/nodout/d000001'
  binout_folder_insert_file(
      &reinterpret_cast<binout_folder_t *>(dir.children[1].children)[0],
      stralloc("y_displacement"), BINOUT_TYPE_FLOAT64, 10, 0, 380);

  p1 = path_view_new("metadata");
  p2 = path_view_new("d000010");

  // Insert 'metadata' into '/nodfor'
  binout_folder_insert_folder(&dir.children[0], &p1);
  // Insert 'd000010' into '/nodfor'
  binout_folder_insert_folder(&dir.children[0], &p2);

  // Insert 'ids' into '/nodfor/metadata'
  binout_folder_insert_file(
      &reinterpret_cast<binout_folder_t *>(dir.children[0].children)[1],
      stralloc("ids"), BINOUT_TYPE_INT64, 10, 1, 10);
  // Insert 'time' into '/nodfor/metadata'
  binout_folder_insert_file(
      &reinterpret_cast<binout_folder_t *>(dir.children[0].children)[1],
      stralloc("time"), BINOUT_TYPE_FLOAT64, 1, 1, 20);
  // Insert 'x_force' into '/nodfor/d000010'
  binout_folder_insert_file(
      &reinterpret_cast<binout_folder_t *>(dir.children[0].children)[0],
      stralloc("x_force"), BINOUT_TYPE_FLOAT64, 10, 1, 100);
  // Insert 'y_force' into '/nodfor/d000010'
  binout_folder_insert_file(
      &reinterpret_cast<binout_folder_t *>(dir.children[0].children)[0],
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
    const binout_folder_or_file_t *folder_or_file =
        binout_directory_get_children(&dir, &p, &num_children);

    REQUIRE(num_children == 2);
    REQUIRE(folder_or_file->type == BINOUT_FILE);
    CHECK(((binout_file_t *)folder_or_file)[0].name == "ids");
    CHECK(((binout_file_t *)folder_or_file)[1].name == "time");
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
}

TEST_CASE("path_view") {
  {
    const char *str = "/nodout/metadata/ids";

    path_view_t pv = path_view_new(str);
    CHECK(path_view_strcmp(&pv, "/") == 0);

    CHECK(path_view_advance(&pv) == 1);
    CHECK(path_view_strcmp(&pv, "nodout") == 0);

    CHECK(path_view_advance(&pv) == 1);
    CHECK(path_view_strcmp(&pv, "metadata") == 0);

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
}

#ifdef PROFILING
int main(int args, char *argv[]) {
  doctest::Context ctx;

  ctx.addFilter("test-case", "binout0000");
  ctx.applyCommandLine(args, argv);

  const int res = ctx.run();

  if (ctx.shouldExit()) {
    return res;
  }

  END_PROFILING("test_data/binout_profiling.txt");

  return res;
}
#endif
