#include "binout.h"
#include "binout_defines.h"
#include <errno.h>
#include <stdlib.h>
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
  size_t read_count =
      fread(&bin_file.header, 1, sizeof(bin_file.header), bin_file.file_handle);
  if (read_count == 0) {
    bin_file.error_string = strerror(errno);
    fclose(bin_file.file_handle);
    return bin_file;
  }

  /* Only support Little Endian */
  if (bin_file.header.endianess == BINOUT_HEADER_BIG_ENDIAN) {
    bin_file.error_string = "Big Endian is not supported";
    fclose(bin_file.file_handle);
    return bin_file;
  }
  if (bin_file.header.record_length_field_size > 8) {
    bin_file.error_string = "This length field size is not supported";
    fclose(bin_file.file_handle);
    return bin_file;
  }
  if (bin_file.header.record_command_field_size > 8) {
    bin_file.error_string = "This command field size is not supported";
    fclose(bin_file.file_handle);
    return bin_file;
  }

  const long cur_pos = ftell(bin_file.file_handle);
  if (fseek(bin_file.file_handle, 0, SEEK_END) != 0) {
    bin_file.error_string = strerror(errno);
    fclose(bin_file.file_handle);
    return bin_file;
  }

  const long file_size = ftell(bin_file.file_handle);
  if (fseek(bin_file.file_handle, cur_pos, SEEK_SET) != 0) {
    bin_file.error_string = strerror(errno);
    fclose(bin_file.file_handle);
    return bin_file;
  }

  /* Parse positions of all records */
  bin_file.record_count = 0;
  bin_file.records = NULL;
  while (ftell(bin_file.file_handle) < file_size) {
    bin_file.record_count++;
    if (!bin_file.records) {
      bin_file.records = malloc(sizeof(binout_record));
    } else {
      bin_file.records = realloc(bin_file.records,
                                 bin_file.record_count * sizeof(binout_record));
    }

    binout_record *last = &bin_file.records[bin_file.record_count - 1];
    last->length = 0;
    last->command = 0;
    last->data_pos = 0;

    read_count =
        fread(&last->length, 1, bin_file.header.record_length_field_size,
              bin_file.file_handle);
    if (read_count == 0) {
      bin_file.error_string = strerror(errno);
      binout_close(&bin_file);
      return bin_file;
    }

    read_count =
        fread(&last->command, 1, bin_file.header.record_command_field_size,
              bin_file.file_handle);
    if (read_count == 0) {
      bin_file.error_string = strerror(errno);
      binout_close(&bin_file);
      return bin_file;
    }

    last->length -= bin_file.header.record_length_field_size +
                    bin_file.header.record_command_field_size;

    last->data_pos = ftell(bin_file.file_handle);
    if (fseek(bin_file.file_handle, last->length, SEEK_CUR) != 0) {
      bin_file.error_string = strerror(errno);
      binout_close(&bin_file);
      return bin_file;
    }
  }

  return bin_file;
}

void binout_close(binout_file *bin_file) {
  if (bin_file->records)
    free(bin_file->records);

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

void binout_print_records(binout_file *bin_file) {
  printf("------- %d Records ---------\n", bin_file->record_count);

  uint64_t i = 0;

  while (i < bin_file->record_count) {
    const binout_record *cur = &bin_file->records[i];
    printf("---- %s -----\n", _binout_get_command_name(cur->command));
    printf("- Length: %d --\n", cur->length);
    printf("- Data Pos: %d --\n", cur->data_pos);
    printf("----------------------\n");

    i++;
  }

  printf("----------------------------\n");
}

const char *_binout_get_command_name(const uint64_t command) {
  switch (command) {
  case BINOUT_COMMAND_NULL:
    return "NULL";
    break;
  case BINOUT_COMMAND_CD:
    return "CD";
    break;
  case BINOUT_COMMAND_DATA:
    return "DATA";
    break;
  case BINOUT_COMMAND_VARIABLE:
    return "VARIABLE";
    break;
  case BINOUT_COMMAND_BEGINSYMBOLTABLE:
    return "BEGINSYMBOLTABLE";
    break;
  case BINOUT_COMMAND_ENDSYMBOLTABLE:
    return "ENDSYMBOLTABLE";
    break;
  case BINOUT_COMMAND_SYMBOLTABLEOFFSET:
    return "SYMBOLTABLEOFFSET";
    break;
  default:
    return "UNKNOWN";
    break;
  }
}