#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#include "binout_glob.h"
#include <binout.h>
#include <binout_defines.h>
#include <doctest/doctest.h>
#include <iomanip>
#include <iostream>
#include <path.h>
#include <sstream>
#include <string>
#ifdef BINOUT_CPP
#include <cpp/binout.hpp>
#endif

namespace dro {
template <typename T> doctest::String toString(const Vector<T> &str) {
  static_assert(std::is_same_v<T, char> || std::is_same_v<T, int8_t> ||
                std::is_same_v<T, uint8_t>);

  return doctest::String(reinterpret_cast<const char *>(str.data()),
                         str.size());
}
} // namespace dro

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

  const char *binout_file_name = "test_data/binout*";

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

  binout_free_children(binout_children, num_binout_children);

  binout_children =
      binout_get_children(&bin_file, "/nodout", &num_binout_children);
  REQUIRE(num_binout_children == 602);
  CHECK(binout_children[0] == "metadata");
  for (size_t i = 1; i <= 601; i++) {
    std::stringstream stream;
    stream << "d" << std::setfill('0') << std::right << std::setw(6) << i;
    const std::string str(stream.str());

    CHECK(binout_children[i] == str.c_str());
  }

  binout_free_children(binout_children, num_binout_children);

  binout_children =
      binout_get_children(&bin_file, "/nodout/metadata/", &num_binout_children);
  REQUIRE(num_binout_children == 7);
  CHECK(binout_children[0] == "title");
  CHECK(binout_children[1] == "version");
  CHECK(binout_children[2] == "revision");
  CHECK(binout_children[3] == "date");
  CHECK(binout_children[4] == "legend");
  CHECK(binout_children[5] == "legend_ids");
  CHECK(binout_children[6] == "ids");

  binout_free_children(binout_children, num_binout_children);

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

  dro::Binout bin_file("test_data/binout*");
  {
    const auto children = bin_file.get_children("/");
    REQUIRE(children.size() == 2);
    CHECK(children[0] == "nodout");
    CHECK(children[1] == "rcforc");
  }

  {
    const auto children = bin_file.get_children("/nodout/metadata/");
    REQUIRE(children.size() == 7);
    CHECK(children[0] == "title");
    CHECK(children[1] == "version");
    CHECK(children[2] == "revision");
    CHECK(children[3] == "date");
    CHECK(children[4] == "legend");
    CHECK(children[5] == "legend_ids");
    CHECK(children[6] == "ids");
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

TEST_CASE("Vector") {
  SUBCASE("Iterator") {
    int *data = (int *)malloc(5 * sizeof(int));
    data[0] = 1;
    data[1] = 2;
    data[2] = 3;
    data[3] = 4;
    data[4] = 5;

    dro::Vector<int> v1(data, 5);
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
  }
}
#endif

TEST_CASE("path_join") {
  char *p0 = (char *)malloc(2);
  p0[0] = '/';
  p0[1] = '\0';
  char *p = (char *)malloc(5);
  p[0] = 'a';
  p[1] = 'b';
  p[2] = 'c';
  p[3] = 'd';
  p[4] = '\0';
  path_t path;
  path.num_elements = 2;
  path.elements = (char **)malloc(path.num_elements * sizeof(char *));
  path.elements[0] = p0;
  path.elements[1] = p;
  path_join(&path, "nodout");
  REQUIRE(path.num_elements == 3);
  CHECK(path.elements[0] == "/");
  CHECK(path.elements[1] == "abcd");
  CHECK(path.elements[2] == "nodout");

  path_free(&path);
}

TEST_CASE("path_parse") {
  {
    const char *p1 = "/nodout/metadata/../d000001";
    path_t path;
    path.elements = path_elements(p1, &path.num_elements);

    path_parse(&path);

    REQUIRE(path.num_elements == 3);
    CHECK(path.elements[0] == "/");
    CHECK(path.elements[1] == "nodout");
    CHECK(path.elements[2] == "d000001");
    path_free(&path);
  }

  {
    const char *p1 = "/nodout/../d000001";
    path_t path;
    path.elements = path_elements(p1, &path.num_elements);

    path_parse(&path);

    REQUIRE(path.num_elements == 2);
    CHECK(path.elements[0] == "/");
    CHECK(path.elements[1] == "d000001");
    path_free(&path);
  }

  {
    const char *p1 = "/nodout/d000001/..";
    path_t path;
    path.elements = path_elements(p1, &path.num_elements);

    path_parse(&path);

    REQUIRE(path.num_elements == 2);
    CHECK(path.elements[0] == "/");
    CHECK(path.elements[1] == "nodout");
    path_free(&path);
  }

  {
    const char *p1 = "/nodout/d000001/../metadata/../d000002";
    path_t path;
    path.elements = path_elements(p1, &path.num_elements);

    path_parse(&path);

    REQUIRE(path.num_elements == 3);
    CHECK(path.elements[0] == "/");
    CHECK(path.elements[1] == "nodout");
    CHECK(path.elements[2] == "d000002");
    path_free(&path);
  }

  {
    const char *p1 = "/ncforc/slave_100000/../master_100000/metadata";
    path_t path;
    path.elements = path_elements(p1, &path.num_elements);

    path_parse(&path);

    REQUIRE(path.num_elements == 4);
    CHECK(path.elements[0] == "/");
    CHECK(path.elements[1] == "ncforc");
    CHECK(path.elements[2] == "master_100000");
    CHECK(path.elements[3] == "metadata");
    path_free(&path);
  }

  {
    const char *p1 = "/ncforc/slave_100000/../../master_100000/metadata";
    path_t path;
    path.elements = path_elements(p1, &path.num_elements);

    path_parse(&path);

    REQUIRE(path.num_elements == 3);
    CHECK(path.elements[0] == "/");
    CHECK(path.elements[1] == "master_100000");
    CHECK(path.elements[2] == "metadata");
    path_free(&path);
  }

  {
    const char *p1 = "../hello/world/../man/";
    path_t path;
    path.elements = path_elements(p1, &path.num_elements);

    path_parse(&path);

    REQUIRE(path.num_elements == 3);
    CHECK(path.elements[0] == "..");
    CHECK(path.elements[1] == "hello");
    CHECK(path.elements[2] == "man");
    path_free(&path);
  }
}

TEST_CASE("path_elements") {
  {
    const char *p1 = "/ncforc/slave_100000/../../master_100000/metadata";

    size_t num_elements;
    char **p1_elements = path_elements(p1, &num_elements);
    REQUIRE(num_elements == 7);
    CHECK(p1_elements[0] == "/");
    CHECK(p1_elements[1] == "ncforc");
    CHECK(p1_elements[2] == "slave_100000");
    CHECK(p1_elements[3] == "..");
    CHECK(p1_elements[4] == "..");
    CHECK(p1_elements[5] == "master_100000");
    CHECK(p1_elements[6] == "metadata");
    path_free_elements(p1_elements, num_elements);
  }

  {
    const char *p1 = "/";

    size_t num_elements;
    char **p1_elements = path_elements(p1, &num_elements);
    REQUIRE(num_elements == 1);
    CHECK(p1_elements[0] == "/");
    path_free_elements(p1_elements, num_elements);
  }

  {
    const char *p1 = "/nodout";

    size_t num_elements;
    char **p1_elements = path_elements(p1, &num_elements);
    REQUIRE(num_elements == 2);
    CHECK(p1_elements[0] == "/");
    CHECK(p1_elements[1] == "nodout");
    path_free_elements(p1_elements, num_elements);
  }

  {
    const char *p1 = "nodout";

    size_t num_elements;
    char **p1_elements = path_elements(p1, &num_elements);
    REQUIRE(num_elements == 1);
    CHECK(p1_elements[0] == "nodout");
    path_free_elements(p1_elements, num_elements);
  }

  {
    const char *p1 = "nodout/apple/two";

    size_t num_elements;
    char **p1_elements = path_elements(p1, &num_elements);
    REQUIRE(num_elements == 3);
    CHECK(p1_elements[0] == "nodout");
    CHECK(p1_elements[1] == "apple");
    CHECK(p1_elements[2] == "two");
    path_free_elements(p1_elements, num_elements);
  }

  {
    const char *p1 = "/nodout///////////////apple///two//three///";

    size_t num_elements;
    char **p1_elements = path_elements(p1, &num_elements);
    REQUIRE(num_elements == 5);
    CHECK(p1_elements[0] == "/");
    CHECK(p1_elements[1] == "nodout");
    CHECK(p1_elements[2] == "apple");
    CHECK(p1_elements[3] == "two");
    CHECK(p1_elements[4] == "three");
    path_free_elements(p1_elements, num_elements);
  }
}

TEST_CASE("path_main_equals") {
  {
    const char *p1 = "/nodout/metadata";
    const char *p2 = "/nodout/metadata";
    path_t p1t, p2t;
    p1t.elements = path_elements(p1, &p1t.num_elements);
    p2t.elements = path_elements(p2, &p2t.num_elements);

    CHECK(path_main_equals(&p1t, &p2t));
  }
}

TEST_CASE("path_str") {
  {
    const char *p1 = "/nodout/master_10000/metadata/ids";
    path_t p1t;
    p1t.elements = path_elements(p1, &p1t.num_elements);
    char *p1_str = path_str(&p1t);
    CHECK(p1_str == p1);
    free(p1_str);
  }

  {
    const char *p1 = "/nodout";
    path_t p1t;
    p1t.elements = path_elements(p1, &p1t.num_elements);
    char *p1_str = path_str(&p1t);
    CHECK(p1_str == p1);
    free(p1_str);
  }

  {
    const char *p1 = "/nodout/master_10000////////metadata/ids";
    path_t p1t;
    p1t.elements = path_elements(p1, &p1t.num_elements);
    char *p1_str = path_str(&p1t);
    CHECK(p1_str == "/nodout/master_10000/metadata/ids");
    free(p1_str);
  }

  {
    const char *p1 = "/";
    path_t p1t;
    p1t.elements = path_elements(p1, &p1t.num_elements);
    char *p1_str = path_str(&p1t);
    CHECK(p1_str == p1);
    free(p1_str);
  }

  {
    const char *p1 = "../hello/world/../my/dudes";
    path_t p1t;
    p1t.elements = path_elements(p1, &p1t.num_elements);
    char *p1_str = path_str(&p1t);
    CHECK(p1_str == p1);
    free(p1_str);
  }
}

TEST_CASE("glob") {
  size_t num_files;
  char **globed_files = binout_glob("src/*.c", &num_files);

  CHECK(num_files == 3);
  CHECK(path_elements_contain(globed_files, num_files, "src/binout.c"));
  CHECK(path_elements_contain(globed_files, num_files, "src/binout_glob.c"));
  CHECK(path_elements_contain(globed_files, num_files, "src/path.c"));
  binout_free_glob(globed_files, num_files);
}
