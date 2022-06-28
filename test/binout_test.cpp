#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <binout.h>
#include <binout_defines.h>
#include <doctest/doctest.h>
#include <path.h>

TEST_CASE("binout0000") {
  const char *binout_file_name = "test_data/binout0000";

  binout_file bin_file = binout_open(binout_file_name);
  if (bin_file.error_string) {
    fprintf(stderr, "Failed to open binout: %s\n", bin_file.error_string);
    return;
  }

  binout_print_header(&bin_file);
  binout_print_records(&bin_file);

  if (binout_variable_exists(&bin_file, "/nodout/metadata", "legend") &&
      binout_get_type_id(&bin_file, "/nodout/metadata", "legend") ==
          BINOUT_TYPE_INT8) {
    size_t legend_size;
    int8_t *legend = binout_read_int8_t(&bin_file, "/nodout/metadata", "legend",
                                        &legend_size);
    if (!legend) {
      fprintf(stderr, "Failed to read legend: %s\n", bin_file.error_string);
      binout_close(&bin_file);
      return;
    }

    legend = (int8_t *)realloc(legend, legend_size + 1);
    legend[legend_size] = '\0';
    printf("nodout legend: %s\n", legend);
    free(legend);
  }

  if (binout_variable_exists(&bin_file, "/nodout/metadata", "ids") &&
      binout_get_type_id(&bin_file, "/nodout/metadata", "ids") ==
          BINOUT_TYPE_INT64) {
    size_t node_ids_size;
    int64_t *node_ids = binout_read_int64_t(&bin_file, "/nodout/metadata",
                                            "ids", &node_ids_size);
    if (!node_ids) {
      fprintf(stderr, "Failed to read node ids: %s\n", bin_file.error_string);
      binout_close(&bin_file);
      return;
    }

    printf("---- Node IDs: %d -----\n", node_ids_size);
    uint64_t i = 0;
    while (i < node_ids_size) {
      printf("%d, ", node_ids[i]);

      i++;
    }
    printf("\n--------------------------\n");
    free(node_ids);
  }

  if (binout_variable_exists(&bin_file, "/ncforc/master_100002/metadata",
                             "title") &&
      binout_get_type_id(&bin_file, "/ncforc/master_100002/metadata",
                         "title") == BINOUT_TYPE_INT8) {
    size_t title_size;
    int8_t *title = binout_read_int8_t(
        &bin_file, "/ncforc/master_100002/metadata", "title", &title_size);
    title = (int8_t *)realloc(title, title_size + 1);
    title[title_size] = '\0';
    printf("Title: %s\n", title);
    free(title);
  }

  binout_close(&bin_file);
}

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
    printf("%s\n", new_p1);
    CHECK(strcmp(new_p1, "Heo World") == 0);
    free(new_p1);
  }

  {
    const char *p2 = "Please use this as an test string";
    const size_t p2_len = strlen(p2);
    char *_p2 = (char *)malloc(p2_len + 1);
    memcpy(_p2, p2, p2_len + 1);

    char *new_p2 = delete_substr(_p2, 0, 6);
    printf("%s\n", new_p2);
    CHECK(strcmp(new_p2, "use this as an test string") == 0);
    free(new_p2);
  }

  {
    const char *p2 = "Please use this as an test string";
    const size_t p2_len = strlen(p2);
    char *_p2 = (char *)malloc(p2_len + 1);
    memcpy(_p2, p2, p2_len + 1);

    char *new_p2 = delete_substr(_p2, 26, 32);
    printf("%s\n", new_p2);
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

    printf("%s\n", new_path);
    CHECK(strcmp(new_path, "/nodout/d000001") == 0);
  }

  {
    const char *p1 = "/nodout/../d000001";
    const size_t p1_len = strlen(p1);
    char *_p1 = (char *)malloc(p1_len + 1);
    memcpy(_p1, p1, p1_len + 1);

    char *new_path = path_parse(_p1);

    printf("%s\n", new_path);
    CHECK(strcmp(new_path, "/d000001") == 0);
  }

  {
    const char *p1 = "/nodout/d000001/..";
    const size_t p1_len = strlen(p1);
    char *_p1 = (char *)malloc(p1_len + 1);
    memcpy(_p1, p1, p1_len + 1);

    char *new_path = path_parse(_p1);

    printf("%s\n", new_path);
    CHECK(strcmp(new_path, "/nodout") == 0);
  }

  {
    const char *p1 = "/nodout/d000001/../metadata/../d000002";
    const size_t p1_len = strlen(p1);
    char *_p1 = (char *)malloc(p1_len + 1);
    memcpy(_p1, p1, p1_len + 1);

    char *new_path = path_parse(_p1);

    printf("%s\n", new_path);
    CHECK(strcmp(new_path, "/nodout/d000002") == 0);
  }

  {
    const char *p1 = "/ncforc/slave_100000/../master_100000/metadata";
    const size_t p1_len = strlen(p1);
    char *_p1 = (char *)malloc(p1_len + 1);
    memcpy(_p1, p1, p1_len + 1);

    char *new_path = path_parse(_p1);

    printf("%s\n", new_path);
    CHECK(strcmp(new_path, "/ncforc/master_100000/metadata") == 0);
  }

  {
    const char *p1 = "/ncforc/slave_100000/../../master_100000/metadata";
    const size_t p1_len = strlen(p1);
    char *_p1 = (char *)malloc(p1_len + 1);
    memcpy(_p1, p1, p1_len + 1);

    char *new_path = path_parse(_p1);

    printf("%s\n", new_path);
    CHECK(strcmp(new_path, "/master_100000/metadata") == 0);
  }
}