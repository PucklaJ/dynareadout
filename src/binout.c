#include "binout.h"
#include "binout_defines.h"
#include <errno.h>
#include <string.h>

binout_file binout_open(const char *file_name) {
  binout_file bin_file;
  bin_file.error_string = NULL;

  bin_file.file_handle = fopen(file_name, "r");
  if (!bin_file.file_handle) {
    bin_file.error_string = strerror(errno);
    return bin_file;
  }

  /* Read header */
  const size_t count =
      fread(&bin_file.header, 1, sizeof(bin_file.header), bin_file.file_handle);
  if (count == 0) {
    bin_file.error_string = strerror(errno);
    return bin_file;
  }

  /* Only support Little Endian */
  if (bin_file.header.endianess == BINOUT_HEADER_BIG_ENDIAN) {
    bin_file.error_string = "Big Endian is not supported";
  }

  return bin_file;
}

void binout_close(binout_file *bin_file) {
  if (fclose(bin_file->file_handle) != 0) {
    bin_file->error_string = strerror(errno);
  }
}

void binout_print_header(binout_file *bin_file) {
  printf("Number of Bytes in header: %d\n", bin_file->header.header_size);
  printf("Number of Bytes used in record LENGTH fields: %d\n",
         bin_file->header.record_length_field_size);
  printf("Number of Bytes used in record OFFSET fields: %d\n",
         bin_file->header.record_offset_field_size);
  printf("Number of Bytes used in record COMMAND fields: %d\n",
         bin_file->header.record_command_field_size);
  printf("Number of Bytes used in record TYPEID fields: %d\n",
         bin_file->header.record_typeid_field_size);
  printf("Endianess: %s\n",
         bin_file->header.endianess == BINOUT_HEADER_BIG_ENDIAN
             ? "Big Endian"
             : (bin_file->header.endianess == BINOUT_HEADER_LITTLE_ENDIAN
                    ? "Little Endian"
                    : "Unknown"));
  printf("Floating Point Format: %s\n",
         bin_file->header.float_format == BINOUT_HEADER_FLOAT_IEEE ? "IEEE"
                                                                   : "Unknown");
}