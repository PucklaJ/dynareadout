#include "binout.h"
#include "binout_defines.h"
#include "binout_records.h"
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define BIN_FILE_READ(dst, size, count)                                        \
  read_count = fread(&dst, size, count, bin_file.file_handle);                 \
  if (read_count != count) {                                                   \
    bin_file.error_string = strerror(errno);                                   \
    /* free(current_path); */                                                  \
    binout_close(&bin_file);                                                   \
    return bin_file;                                                           \
  }

#define BIN_FILE_READ_FREE(dst, size, count, obj)                              \
  read_count = fread(dst, size, count, bin_file.file_handle);                  \
  if (read_count != count) {                                                   \
    bin_file.error_string = strerror(errno);                                   \
    /* free(current_path); */                                                  \
    free(obj);                                                                 \
    binout_close(&bin_file);                                                   \
    return bin_file;                                                           \
  }

#define PATH_SEP '/'

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
      fread(&bin_file.header, sizeof(binout_header), 1, bin_file.file_handle);
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

  /* Parse all records */
  bin_file.data_pointers_size = 0;
  bin_file.data_pointers = NULL;

  char *current_path = malloc(2);
  current_path[0] = '.';
  current_path[1] = '\0';

  while (ftell(bin_file.file_handle) < file_size) {
    uint64_t record_length = 0, record_command = 0;

    BIN_FILE_READ(record_length, bin_file.header.record_length_field_size, 1);
    BIN_FILE_READ(record_command, bin_file.header.record_command_field_size, 1);

    const uint64_t record_data_length =
        record_length - bin_file.header.record_length_field_size -
        bin_file.header.record_command_field_size;

    if (record_command == BINOUT_COMMAND_SYMBOLTABLEOFFSET) {
      if (record_data_length > 8) {
        bin_file.error_string =
            "A symbol table offset length larger than 8 is not supported";
        binout_close(&bin_file);
        return bin_file;
      }

      BIN_FILE_READ(bin_file.symbol_table_offset, record_data_length, 1);
    } else if (record_command == BINOUT_COMMAND_NULL) {
      if (fseek(bin_file.file_handle, record_data_length, SEEK_CUR) != 0) {
        bin_file.error_string = strerror(errno);
        binout_close(&bin_file);
        return bin_file;
      }
    } else if (record_command == BINOUT_COMMAND_CD) {
      char *path = malloc(record_data_length + 1);
      path[record_data_length] = '\0';

      BIN_FILE_READ_FREE(path, 1, record_data_length, path);

      if (_path_is_abs(path) || strcmp(current_path, ".") == 0) {
        free(current_path);
        current_path = path;
      } else {
        current_path = _path_join(current_path, path);
        free(path);
      }
    } else if (record_command == BINOUT_COMMAND_DATA) {
      uint64_t type_id = 0;
      uint8_t name_length;

      BIN_FILE_READ(type_id, bin_file.header.record_typeid_field_size, 1);
      BIN_FILE_READ(name_length, BINOUT_DATA_NAME_LENGTH, 1);

      char *name = malloc(name_length + 1);
      name[name_length] = '\0';
      BIN_FILE_READ_FREE(name, 1, name_length, name);

      const uint64_t data_length = record_data_length -
                                   bin_file.header.record_typeid_field_size -
                                   BINOUT_DATA_NAME_LENGTH - name_length;
      uint8_t *data = malloc(data_length);
      BIN_FILE_READ_FREE(data, data_length, 1, data);

      const size_t current_path_len = strlen(current_path);
      char *data_path = malloc(current_path_len + 1);
      memcpy(data_path, current_path, current_path_len + 1);
      data_path = _path_join(data_path, name);
      free(name);

      binout_record_data_pointer *dp = NULL;
      {
        uint64_t i = 0;
        while (i < bin_file.data_pointers_size) {
          binout_record_data_pointer *bin_dp = &bin_file.data_pointers[i];
          if (strcmp(bin_dp->path, data_path) == 0) {
            dp = bin_dp;
            break;
          }
          i++;
        }
      }

      if (dp) {
        assert(data_length == dp->data_length);
      } else {
        bin_file.data_pointers_size++;
        if (bin_file.data_pointers_size == 1) {
          bin_file.data_pointers = malloc(sizeof(binout_record_data_pointer));
        } else {
          bin_file.data_pointers = realloc(
              bin_file.data_pointers,
              bin_file.data_pointers_size * sizeof(binout_record_data_pointer));
        }

        dp = &bin_file.data_pointers[bin_file.data_pointers_size - 1];
        dp->path = data_path;
        dp->data_size = 0;
        dp->data_length = data_length;
        dp->type_id = type_id;
        dp->data = NULL;
      }

      dp->data_size++;
      if (dp->data_size == 1) {
        dp->data = malloc(sizeof(uint8_t *));
      } else {
        dp->data = realloc(dp->data, dp->data_size * sizeof(uint8_t *));
      }
      dp->data[dp->data_size - 1] = data;
    } else {
      if (fseek(bin_file.file_handle, record_data_length, SEEK_CUR) != 0) {
        bin_file.error_string = strerror(errno);
        binout_close(&bin_file);
        return bin_file;
      }
    }
  }

  free(current_path);

  return bin_file;
}

void binout_close(binout_file *bin_file) {
  if (bin_file->data_pointers) {
    uint64_t i = 0;
    while (i < bin_file->data_pointers_size) {
      binout_record_data_pointer *dp = &bin_file->data_pointers[i];
      uint64_t j = 0;
      while (j < dp->data_size) {
        free(dp->data[j]);
        j++;
      }
      free(dp->path);
      i++;
    }
    free(bin_file->data_pointers);
  }

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
  printf("Symbol Table Offset: %d\n", bin_file->symbol_table_offset);
  printf("\n\n");
}

void binout_print_records(binout_file *bin_file) {
  printf("------ %d Data Pointers ------\n", bin_file->data_pointers_size);

  uint64_t i = 0;
  while (i < bin_file->data_pointers_size) {
    const binout_record_data_pointer *dp = &bin_file->data_pointers[i];
    printf("---- %s ----\n", dp->path);
    printf("- Data Size: %d --\n", dp->data_size);
    printf("- Data Length: %d --\n", dp->data_length);
    printf("- Type: %s -----\n", _binout_get_type_name(dp->type_id));
    printf("---------------------\n");

    i++;
  }

  printf("-----------------------------------\n");
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

uint8_t _binout_get_type_size(const uint64_t type_id) {
  switch (type_id) {
  case BINOUT_TYPE_INT8:
    return 1;
  case BINOUT_TYPE_INT16:
    return 2;
  case BINOUT_TYPE_INT32:
    return 4;
  case BINOUT_TYPE_INT64:
    return 8;
  case BINOUT_TYPE_UINT8:
    return 1;
  case BINOUT_TYPE_UINT16:
    return 2;
  case BINOUT_TYPE_UINT32:
    return 4;
  case BINOUT_TYPE_UINT64:
    return 8;
  case BINOUT_TYPE_FLOAT32:
    return 4;
  case BINOUT_TYPE_FLOAT64:
    return 8;
  default:
    return 255;
  }
}

const char *_binout_get_type_name(const uint64_t type_id) {
  switch (type_id) {
  case BINOUT_TYPE_INT8:
    return "INT8";
  case BINOUT_TYPE_INT16:
    return "INT16";
  case BINOUT_TYPE_INT32:
    return "INT32";
  case BINOUT_TYPE_INT64:
    return "INT64";
  case BINOUT_TYPE_UINT8:
    return "UINT8";
  case BINOUT_TYPE_UINT16:
    return "UINT16";
  case BINOUT_TYPE_UINT32:
    return "UINT32";
  case BINOUT_TYPE_UINT64:
    return "UINT64";
  case BINOUT_TYPE_FLOAT32:
    return "FLOAT32";
  case BINOUT_TYPE_FLOAT64:
    return "FLOAT64";
  default:
    return "UNKNOWN";
  }
}

char *_path_join(char *path, const char *element) {
  const size_t path_length = strlen(path);
  size_t element_length = strlen(element);
  /* path + PATH_SEP + element + '\0' */
  size_t new_size = path_length + element_length + 1;
  int needs_path_sep = 0;
  if (path[path_length - 1] != PATH_SEP) {
    new_size += 1;
    needs_path_sep = 1;
  }

  path = realloc(path, new_size);

  path[new_size - 1] = '\0';
  if (needs_path_sep)
    path[path_length] = PATH_SEP;

  size_t element_offset = 0;
  if (element[0] == PATH_SEP) {
    element_offset = 1;
    element_length--;
  }
  memcpy(&path[path_length + needs_path_sep], &element[element_offset],
         element_length);
  return path;
}

int _path_is_abs(const char *path) { return path[0] == PATH_SEP; }