#include "binout.h"
#include "binout_defines.h"
#include "binout_records.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define BIN_FILE_READ(dst, size, count)                                        \
  read_count = fread(&dst, size, count, bin_file.file_handle);                 \
  if (read_count != count) {                                                   \
    bin_file.error_string = strerror(errno);                                   \
    free(current_path);                                                        \
    binout_close(&bin_file);                                                   \
    return bin_file;                                                           \
  }

#define BIN_FILE_READ_FREE(dst, size, count, obj)                              \
  read_count = fread(dst, size, count, bin_file.file_handle);                  \
  if (read_count != count) {                                                   \
    bin_file.error_string = strerror(errno);                                   \
    free(current_path);                                                        \
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
      fread(&bin_file.header, sizeof(bin_file.header), 1, bin_file.file_handle);
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
  bin_file.record_count = 0;
  bin_file.records = NULL;
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

      printf("Changed Directory: %s\n", current_path);
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

      printf("Data Name: %s\n", name);
      printf("Data Type: %s\n", _binout_get_type_name(type_id));
      const uint8_t type_size = _binout_get_type_size(type_id);
      const uint64_t value_count = data_length / (uint64_t)type_size;
      printf("Data Values: %d\n", value_count);
      {
        uint64_t offset = 0;
        uint64_t i = 0;
        while (i < value_count) {
          switch (type_id) {
          case BINOUT_TYPE_INT8:
            int8_t value;
            memcpy(&value, &data[offset], type_size);
            printf("%c, ", value);
            break;
          case BINOUT_TYPE_INT16:
            int16_t value1;
            memcpy(&value1, &data[offset], type_size);
            printf("%d, ", value1);
            break;
          case BINOUT_TYPE_INT32:
            int32_t value2;
            memcpy(&value2, &data[offset], type_size);
            printf("%d, ", value2);
            break;
          case BINOUT_TYPE_INT64:
            int64_t value3;
            memcpy(&value3, &data[offset], type_size);
            printf("%d, ", value3);
            break;
          case BINOUT_TYPE_UINT8:
            uint8_t value4;
            memcpy(&value4, &data[offset], type_size);
            printf("%d, ", value4);
            break;
          case BINOUT_TYPE_UINT16:
            uint16_t value5;
            memcpy(&value5, &data[offset], type_size);
            printf("%d, ", value5);
            break;
          case BINOUT_TYPE_UINT32:
            uint32_t value6;
            memcpy(&value6, &data[offset], type_size);
            printf("%d, ", value6);
            break;
          case BINOUT_TYPE_UINT64:
            uint64_t value7;
            memcpy(&value7, &data[offset], type_size);
            printf("%d, ", value7);
            break;
          case BINOUT_TYPE_FLOAT32:
            float value8;
            memcpy(&value8, &data[offset], type_size);
            printf("%f, ", value8);
            break;
          case BINOUT_TYPE_FLOAT64:
            double value9;
            memcpy(&value9, &data[offset], type_size);
            printf("%f, ", value9);
            break;
          }

          offset += (uint64_t)type_size;
          i++;
        }
        printf("\n");
      }

      free(name);
      free(data);
    } else {
      bin_file.record_count++;
      if (!bin_file.records) {
        bin_file.records = malloc(sizeof(binout_record));
      } else {
        bin_file.records = realloc(bin_file.records, bin_file.record_count *
                                                         sizeof(binout_record));
      }

      binout_record *last = &bin_file.records[bin_file.record_count - 1];
      last->length = record_length;
      last->command = record_command;
      last->data_pos = 0;

      last->length -= bin_file.header.record_length_field_size +
                      bin_file.header.record_command_field_size;

      last->data_pos = ftell(bin_file.file_handle);
      if (fseek(bin_file.file_handle, last->length, SEEK_CUR) != 0) {
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
  printf("Symbol Table Offset: %d\n", bin_file->symbol_table_offset);
  printf("\n\n");
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
  const size_t new_size = path_length + 1 + element_length + 1;
  path = realloc(path, new_size);
  if (path[path_length - 1] != PATH_SEP) {
    path[path_length] = PATH_SEP;
  }
  path[new_size] = '\0';

  size_t element_offset = 0;
  if (element[0] == PATH_SEP) {
    element_offset = 1;
    element_length--;
  }
  memcpy(&path[path_length + 1], &element[element_offset], element_length);
  return path;
}

int _path_is_abs(const char *path) { return path[0] == PATH_SEP; }