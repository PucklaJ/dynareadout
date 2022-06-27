#include <binout.h>
#include <binout_defines.h>

int main(int args, char *argv[]) {
  if (args < 2) {
    fprintf(stderr, "Invalid Arguments\n");
    return 1;
  }

  const char *binout_file_name = argv[1];

  binout_file bin_file = binout_open(binout_file_name);
  if (bin_file.error_string) {
    fprintf(stderr, "Failed to open binout: %s\n", bin_file.error_string);
    return 1;
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
      return 1;
    }

    legend = realloc(legend, legend_size + 1);
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
      return 1;
    }

    printf("---- Node IDs: %d -----\n", node_ids_size);
    uint64_t i = 0;
    while (i < node_ids_size) {
      printf("%d, ", node_ids[i]);

      i++;
    }
    printf("\n--------------------------\n");
  }

  binout_close(&bin_file);

  return 0;
}
