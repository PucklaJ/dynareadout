#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <binout.h>
#include <binout_defines.h>
#include <doctest/doctest.h>
#include <iomanip>
#include <path.h>
#include <sstream>
#include <string>
#ifdef BINOUT_CPP
#include <cpp/binout.hpp>
#endif

TEST_CASE("binout0000") {
  const char *binout_file_name = "test_data/binout0000";

  binout_file bin_file = binout_open(binout_file_name);
  if (bin_file.error_string) {
    fprintf(stderr, "Failed to open binout: %s\n", bin_file.error_string);
    return;
  }

  binout_print_records(&bin_file);

  size_t num_binout_children;
  char **binout_children =
      binout_get_children(&bin_file, "/", &num_binout_children);
  REQUIRE(num_binout_children == 2);
  CHECK(strcmp(binout_children[0], "nodout") == 0);
  CHECK(strcmp(binout_children[1], "rcforc") == 0);

  binout_free_children(binout_children, num_binout_children);

  binout_children =
      binout_get_children(&bin_file, "/nodout", &num_binout_children);
  REQUIRE(num_binout_children == 602);
  CHECK(strcmp(binout_children[0], "metadata") == 0);
  for (size_t i = 1; i <= 601; i++) {
    std::stringstream stream;
    stream << "d" << std::setfill('0') << std::right << std::setw(6) << i;
    const std::string str(stream.str());

    CHECK(strcmp(binout_children[i], str.c_str()) == 0);
  }

  binout_free_children(binout_children, num_binout_children);

  binout_children =
      binout_get_children(&bin_file, "/nodout/metadata/", &num_binout_children);
  REQUIRE(num_binout_children == 7);
  CHECK(strcmp(binout_children[0], "title") == 0);
  CHECK(strcmp(binout_children[1], "version") == 0);
  CHECK(strcmp(binout_children[2], "revision") == 0);
  CHECK(strcmp(binout_children[3], "date") == 0);
  CHECK(strcmp(binout_children[4], "legend") == 0);
  CHECK(strcmp(binout_children[5], "legend_ids") == 0);
  CHECK(strcmp(binout_children[6], "ids") == 0);

  binout_free_children(binout_children, num_binout_children);

  REQUIRE(binout_variable_exists(&bin_file, "/nodout/metadata", "legend"));
  REQUIRE(binout_get_type_id(&bin_file, "/nodout/metadata", "legend") ==
          BINOUT_TYPE_INT8);

  size_t legend_size;
  int8_t *legend =
      binout_read_int8_t(&bin_file, "/nodout/metadata", "legend", &legend_size);
  REQUIRE(legend);
  CHECK(legend_size == 80);
  legend = (int8_t *)realloc(legend, legend_size + 1);
  legend[legend_size] = '\0';
  CHECK(strcmp((const char *)legend,
               "History_node_1                                                 "
               "                 ") == 0);
  free(legend);

  REQUIRE(binout_variable_exists(&bin_file, "/nodout/metadata", "ids"));
  REQUIRE(binout_get_type_id(&bin_file, "/nodout/metadata", "ids") ==
          BINOUT_TYPE_INT64);

  size_t node_ids_size;
  int64_t *node_ids =
      binout_read_int64_t(&bin_file, "/nodout/metadata", "ids", &node_ids_size);
  REQUIRE(node_ids);
  CHECK(node_ids_size == 1);
  free(node_ids);

  REQUIRE(binout_variable_exists(&bin_file, "/rcforc/metadata", "title"));
  REQUIRE(binout_get_type_id(&bin_file, "/rcforc/metadata", "title") ==
          BINOUT_TYPE_INT8);

  size_t title_size;
  int8_t *title =
      binout_read_int8_t(&bin_file, "/rcforc/metadata", "title", &title_size);
  REQUIRE(title);
  CHECK(title_size == 80);
  title = (int8_t *)realloc(title, title_size + 1);
  title[title_size] = '\0';
  CHECK(
      strcmp((const char *)title,
             "Pouch_macro_37Ah                                                 "
             "               ") == 0);
  free(title);

  binout_close(&bin_file);
}

#ifdef BINOUT_CPP
TEST_CASE("binout0000 C++") {
  dro::Binout bin_file("test_data/binout0000");
  {
    const auto children = bin_file.get_children("/");
    REQUIRE(children.size() == 2);
    CHECK((children[0] == "nodout") == true);
    CHECK((children[1] == "rcforc") == true);
  }

  {
    const auto children = bin_file.get_children("/nodout/metadata/");
    REQUIRE(children.size() == 7);
    CHECK((children[0] == "title") == true);
    CHECK((children[1] == "version") == true);
    CHECK((children[2] == "revision") == true);
    CHECK((children[3] == "date") == true);
    CHECK((children[4] == "legend") == true);
    CHECK((children[5] == "legend_ids") == true);
    CHECK((children[6] == "ids") == true);
  }

  REQUIRE(bin_file.variable_exists("/nodout/metadata", "legend"));
  REQUIRE(bin_file.get_type_id("/nodout/metadata", "legend") ==
          dro::BinoutType::Int8);

  {
    const auto legend = bin_file.read<int8_t>("/nodout/metadata", "legend");
    REQUIRE(legend.size() == 80);
    CHECK((legend ==
           "History_node_1                                                 "
           "                 ") == true);
  }

  REQUIRE(bin_file.variable_exists("/nodout/metadata", "ids"));
  REQUIRE(bin_file.get_type_id("/nodout/metadata", "ids") ==
          dro::BinoutType::Int64);

  {
    const auto node_ids = bin_file.read<int64_t>("/nodout/metadata", "ids");
    REQUIRE(node_ids.size() == 1);
  }

  REQUIRE(bin_file.variable_exists("/rcforc/metadata", "title"));
  REQUIRE(bin_file.get_type_id("/rcforc/metadata", "title") ==
          dro::BinoutType::Int8);

  {
    const auto title = bin_file.read<int8_t>("/rcforc/metadata", "title");
    CHECK(title.size() == 80);
    CHECK((title ==
           "Pouch_macro_37Ah                                                 "
           "               ") == true);
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
  char *p = (char *)malloc(6);
  p[0] = '/';
  p[1] = 'a';
  p[2] = 'b';
  p[3] = 'c';
  p[4] = 'd';
  p[5] = '\0';
  p = path_join(p, "nodout");
  CHECK(strcmp(p, "/abcd/nodout") == 0);

  free(p);
}

TEST_CASE("delete_substr") {
  {
    const char *p1 = "Hello World";
    const size_t p1_len = strlen(p1);
    char *_p1 = (char *)malloc(p1_len + 1);
    memcpy(_p1, p1, p1_len + 1);

    char *new_p1 = delete_substr(_p1, 2, 3);
    CHECK(strcmp(new_p1, "Heo World") == 0);
    free(new_p1);
  }

  {
    const char *p2 = "Please use this as an test string";
    const size_t p2_len = strlen(p2);
    char *_p2 = (char *)malloc(p2_len + 1);
    memcpy(_p2, p2, p2_len + 1);

    char *new_p2 = delete_substr(_p2, 0, 6);
    CHECK(strcmp(new_p2, "use this as an test string") == 0);
    free(new_p2);
  }

  {
    const char *p2 = "Please use this as an test string";
    const size_t p2_len = strlen(p2);
    char *_p2 = (char *)malloc(p2_len + 1);
    memcpy(_p2, p2, p2_len + 1);

    char *new_p2 = delete_substr(_p2, 26, 32);
    CHECK(strcmp(new_p2, "Please use this as an test") == 0);
    free(new_p2);
  }
}

TEST_CASE("path_parse") {
  {
    const char *p1 = "/nodout/metadata/../d000001";
    const size_t p1_len = strlen(p1);
    char *_p1 = (char *)malloc(p1_len + 1);
    memcpy(_p1, p1, p1_len + 1);

    char *new_path = path_parse(_p1);

    CHECK(strcmp(new_path, "/nodout/d000001") == 0);
    free(new_path);
  }

  {
    const char *p1 = "/nodout/../d000001";
    const size_t p1_len = strlen(p1);
    char *_p1 = (char *)malloc(p1_len + 1);
    memcpy(_p1, p1, p1_len + 1);

    char *new_path = path_parse(_p1);

    CHECK(strcmp(new_path, "/d000001") == 0);
    free(new_path);
  }

  {
    const char *p1 = "/nodout/d000001/..";
    const size_t p1_len = strlen(p1);
    char *_p1 = (char *)malloc(p1_len + 1);
    memcpy(_p1, p1, p1_len + 1);

    char *new_path = path_parse(_p1);

    CHECK(strcmp(new_path, "/nodout") == 0);
    free(new_path);
  }

  {
    const char *p1 = "/nodout/d000001/../metadata/../d000002";
    const size_t p1_len = strlen(p1);
    char *_p1 = (char *)malloc(p1_len + 1);
    memcpy(_p1, p1, p1_len + 1);

    char *new_path = path_parse(_p1);

    CHECK(strcmp(new_path, "/nodout/d000002") == 0);
    free(new_path);
  }

  {
    const char *p1 = "/ncforc/slave_100000/../master_100000/metadata";
    const size_t p1_len = strlen(p1);
    char *_p1 = (char *)malloc(p1_len + 1);
    memcpy(_p1, p1, p1_len + 1);

    char *new_path = path_parse(_p1);

    CHECK(strcmp(new_path, "/ncforc/master_100000/metadata") == 0);
    free(new_path);
  }

  {
    const char *p1 = "/ncforc/slave_100000/../../master_100000/metadata";
    const size_t p1_len = strlen(p1);
    char *_p1 = (char *)malloc(p1_len + 1);
    memcpy(_p1, p1, p1_len + 1);

    char *new_path = path_parse(_p1);

    CHECK(strcmp(new_path, "/master_100000/metadata") == 0);
    free(new_path);
  }
}

TEST_CASE("path_elements") {
  {
    const char *p1 = "/ncforc/slave_100000/../../master_100000/metadata";

    size_t num_elements;
    char **p1_elements = path_elements(p1, &num_elements);
    REQUIRE(num_elements == 7);
    CHECK(strcmp(p1_elements[0], "/") == 0);
    CHECK(strcmp(p1_elements[1], "ncforc") == 0);
    CHECK(strcmp(p1_elements[2], "slave_100000") == 0);
    CHECK(strcmp(p1_elements[3], "..") == 0);
    CHECK(strcmp(p1_elements[4], "..") == 0);
    CHECK(strcmp(p1_elements[5], "master_100000") == 0);
    CHECK(strcmp(p1_elements[6], "metadata") == 0);
    path_free_elements(p1_elements, num_elements);
  }

  {
    const char *p1 = "/";

    size_t num_elements;
    char **p1_elements = path_elements(p1, &num_elements);
    REQUIRE(num_elements == 1);
    CHECK(strcmp(p1_elements[0], "/") == 0);
    path_free_elements(p1_elements, num_elements);
  }

  {
    const char *p1 = "/nodout";

    size_t num_elements;
    char **p1_elements = path_elements(p1, &num_elements);
    REQUIRE(num_elements == 2);
    CHECK(strcmp(p1_elements[0], "/") == 0);
    CHECK(strcmp(p1_elements[1], "nodout") == 0);
    path_free_elements(p1_elements, num_elements);
  }
}