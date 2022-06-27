#include <binout.h>

int main(int args, char *argv[]) {
  binout_file bin_file = binout_open("test_data/binout0000");
  if (bin_file.error_string) {
    fprintf(stderr, "Failed to open binout: %s\n", bin_file.error_string);
    return 1;
  }

  binout_print_header(&bin_file);
  binout_print_records(&bin_file);

  binout_close(&bin_file);

  return 0;
}
