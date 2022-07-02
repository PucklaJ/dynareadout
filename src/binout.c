#include "binout.h"
#include "binout_defines.h"
#include "binout_records.h"
#include "path.h"
#include <errno.h>
#include <string.h>

#define BIN_FILE_READ(dst, size, count)                                        \
  read_count = fread(&dst, size, count, bin_file.file_handle);                 \
  if (read_count != count) {                                                   \
    bin_file.error_string = strerror(errno);                                   \
    path_free(&current_path);                                                  \
    binout_close(&bin_file);                                                   \
    return bin_file;                                                           \
  }

#define BIN_FILE_READ_FREE(dst, size, count, obj)                              \
  read_count = fread(dst, size, count, bin_file.file_handle);                  \
  if (read_count != count) {                                                   \
    bin_file.error_string = strerror(errno);                                   \
    path_free(&current_path);                                                  \
    free(obj);                                                                 \
    binout_close(&bin_file);                                                   \
    return bin_file;                                                           \
  }

binout_file binout_open(const char *file_name) {
  binout_file bin_file;
  bin_file.error_string = NULL;

  bin_file.file_handle = fopen(file_name, "r");
  if (!bin_file.file_handle) {
    bin_file.error_string = strerror(errno);
    return bin_file;
  }

  binout_header header;

  /* Read header */
  size_t read_count =
      fread(&header, sizeof(binout_header), 1, bin_file.file_handle);
  if (read_count == 0) {
    bin_file.error_string = strerror(errno);
    fclose(bin_file.file_handle);
    return bin_file;
  }

  /* Check if the binout file is actually supported (Might also be an indicator
   * that the given file is not a binout) */
  if (header.endianess == BINOUT_HEADER_BIG_ENDIAN) {
    bin_file.error_string = "Big Endian is not supported";
    fclose(bin_file.file_handle);
    return bin_file;
  }
  if (header.record_length_field_size > 8) {
    bin_file.error_string = "This length field size is not supported";
    fclose(bin_file.file_handle);
    return bin_file;
  }
  if (header.record_command_field_size > 8) {
    bin_file.error_string = "This command field size is not supported";
    fclose(bin_file.file_handle);
    return bin_file;
  }
  if (header.record_typeid_field_size > 8) {
    bin_file.error_string = "This type id field size is not supported";
    fclose(bin_file.file_handle);
    return bin_file;
  }
  if (header.float_format != BINOUT_HEADER_FLOAT_IEEE) {
    bin_file.error_string = "This float format is not supported";
    fclose(bin_file.file_handle);
    return bin_file;
  }

  /* Get the file size*/
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

  /* Store the current path which is changed by the CD commands*/
  path_t current_path;
  current_path.elements = NULL;
  current_path.num_elements = 0;

  /* We cannot use EOF, so we use this*/
  while (ftell(bin_file.file_handle) < file_size) {
    uint64_t record_length = 0, record_command = 0;

    BIN_FILE_READ(record_length, header.record_length_field_size, 1);
    BIN_FILE_READ(record_command, header.record_command_field_size, 1);

    const uint64_t record_data_length = record_length -
                                        header.record_length_field_size -
                                        header.record_command_field_size;

    /* Execute code for all the different commands
     * Currently only CD and DATA. All other commands are ignored*/
    if (record_command == BINOUT_COMMAND_CD) {
      char *path = malloc(record_data_length + 1);
      path[record_data_length] = '\0';

      BIN_FILE_READ_FREE(path, 1, record_data_length, path);

      if (path_is_abs(path) || !current_path.elements) {
        if (current_path.elements) {
          path_free(&current_path);
        }
        current_path.elements = path_elements(path, &current_path.num_elements);
      } else {
        path_join(&current_path, path);
      }
      free(path);

      path_parse(&current_path);
    } else if (record_command == BINOUT_COMMAND_DATA) {
      uint64_t type_id = 0;
      uint8_t variable_name_length;

      BIN_FILE_READ(type_id, header.record_typeid_field_size, 1);
      BIN_FILE_READ(variable_name_length, BINOUT_DATA_NAME_LENGTH, 1);

      char *variable_name = malloc(variable_name_length + 1);
      variable_name[variable_name_length] = '\0';
      BIN_FILE_READ_FREE(variable_name, 1, variable_name_length, variable_name);

      /* How large the data segment of the data record is*/
      const uint64_t data_length =
          record_data_length - header.record_typeid_field_size -
          BINOUT_DATA_NAME_LENGTH - variable_name_length;
      const size_t file_pos = ftell(bin_file.file_handle);
      /* Skip the data since we will read it at a later point, if it is
       * requested by the programmer*/
      if (fseek(bin_file.file_handle, data_length, SEEK_CUR) != 0) {
        bin_file.error_string = strerror(errno);
        free(variable_name);
        path_free(&current_path);
        binout_close(&bin_file);
        return bin_file;
      }

      /* Get the according data pointer if there already is one or create a new
       * one*/
      binout_record_data_pointer *dp =
          _binout_get_data_pointer2(&bin_file, &current_path, variable_name);

      if (dp) {
        /* Just an assertion to make sure that the data_length stays
         * consistent*/
        if (data_length != dp->data_length) {
          bin_file.error_string =
              "The data_length of one record is different from another even "
              "though they are of the same variable";
          free(variable_name);
          path_free(&current_path);
          binout_close(&bin_file);
          return bin_file;
        }
      } else {
        bin_file.data_pointers_size++;
        bin_file.data_pointers = realloc(
            bin_file.data_pointers,
            bin_file.data_pointers_size * sizeof(binout_record_data_pointer));

        dp = &bin_file.data_pointers[bin_file.data_pointers_size - 1];
        dp->name = variable_name;
        dp->records_size = 0;
        dp->data_length = data_length;
        dp->type_id = type_id;
        dp->records = NULL;
      }

      /* Overwrite it if a record with the same name already exists.
       * Apparently this can indeed be the case*/
      binout_record_data *rd = _binout_get_data(dp, &current_path);
      if (!rd) {
        dp->records_size++;
        dp->records =
            realloc(dp->records, dp->records_size * sizeof(binout_record_data));

        rd = &dp->records[dp->records_size - 1];
        rd->path.elements = NULL;
        rd->path.num_elements = 0;
        path_copy(&rd->path, &current_path);
      }

      rd->file_pos = file_pos;
    } else {
      /* Just skip the record and ignore its data*/
      if (fseek(bin_file.file_handle, record_data_length, SEEK_CUR) != 0) {
        bin_file.error_string = strerror(errno);
        binout_close(&bin_file);
        return bin_file;
      }
    }
  }

  path_free(&current_path);

  return bin_file;
}

void binout_close(binout_file *bin_file) {
  /* Free all data pointers*/
  if (bin_file->data_pointers) {
    uint64_t i = 0;
    while (i < bin_file->data_pointers_size) {
      binout_record_data_pointer *dp = &bin_file->data_pointers[i];
      /* Free all records*/
      uint64_t j = 0;
      while (j < dp->records_size) {
        path_free(&dp->records[j].path);
        j++;
      }
      free(dp->records);
      free(dp->name);
      i++;
    }
    free(bin_file->data_pointers);
  }

  if (fclose(bin_file->file_handle) != 0) {
    bin_file->error_string = strerror(errno);
  }
}

void binout_print_records(binout_file *bin_file) {
  printf("------ %d Data Pointers ------\n", bin_file->data_pointers_size);

  uint64_t i = 0;
  while (i < bin_file->data_pointers_size) {
    const binout_record_data_pointer *dp = &bin_file->data_pointers[i];
    printf("---- %s ----\n", dp->name);
    printf("- Data Length: %d --\n", dp->data_length);
    printf("- Type: %s -----\n", _binout_get_type_name(dp->type_id));
    printf("---------------------\n");
    printf("- Records: %d --\n", dp->records_size);

    uint64_t j = 0;
    while (j < dp->records_size) {
      char *path_string = path_str(&dp->records[j].path);
      printf("- Path: %s ---\n", path_string);
      free(path_string);
      printf("- File Pos: 0x%x ---\n", dp->records[j].file_pos);

      j++;
    }

    i++;
  }

  printf("-----------------------------------\n");
}

void *binout_read(binout_file *bin_file, binout_record_data_pointer *dp,
                  path_t *path_to_variable, size_t type_size,
                  size_t *data_size) {
  path_to_variable->num_elements--;
  binout_record_data *record = _binout_get_data(dp, path_to_variable);
  path_to_variable->num_elements++;
  path_free(path_to_variable);
  if (!record) {
    bin_file->error_string = "The given path has not been found";
    return NULL;
  }

  if (fseek(bin_file->file_handle, record->file_pos, SEEK_SET) != 0) {
    bin_file->error_string = strerror(errno);
    return NULL;
  }

  void *data = malloc(dp->data_length);
  if (fread(data, dp->data_length, 1, bin_file->file_handle) != 1) {
    bin_file->error_string = strerror(errno);
    free(data);
    return NULL;
  }

  *data_size = dp->data_length / type_size;

  return data;
}

#define DEFINE_BINOUT_READ_TYPE(c_type, binout_type)                           \
  c_type *binout_read_##c_type(binout_file *bin_file,                          \
                               const char *path_to_variable,                   \
                               size_t *data_size) {                            \
    path_t _path_to_variable;                                                  \
    _path_to_variable.elements =                                               \
        path_elements(path_to_variable, &_path_to_variable.num_elements);      \
    binout_record_data_pointer *dp =                                           \
        _binout_get_data_pointer(bin_file, &_path_to_variable);                \
    if (!dp) {                                                                 \
      bin_file->error_string = "The given variable has not been found";        \
      path_free(&_path_to_variable);                                           \
      return NULL;                                                             \
    }                                                                          \
                                                                               \
    if (dp->type_id != binout_type) {                                          \
      bin_file->error_string =                                                 \
          "The given variable is of the wrong type. Mind "                     \
          "Single or Double Precision";                                        \
      path_free(&_path_to_variable);                                           \
      return NULL;                                                             \
    }                                                                          \
                                                                               \
    const size_t type_size = _binout_get_type_size(dp->type_id);               \
                                                                               \
    return binout_read(bin_file, dp, &_path_to_variable, type_size,            \
                       data_size);                                             \
  }

DEFINE_BINOUT_READ_TYPE(int8_t, BINOUT_TYPE_INT8)
DEFINE_BINOUT_READ_TYPE(int16_t, BINOUT_TYPE_INT16)
DEFINE_BINOUT_READ_TYPE(int32_t, BINOUT_TYPE_INT32)
DEFINE_BINOUT_READ_TYPE(int64_t, BINOUT_TYPE_INT64)
DEFINE_BINOUT_READ_TYPE(uint8_t, BINOUT_TYPE_UINT8)
DEFINE_BINOUT_READ_TYPE(uint16_t, BINOUT_TYPE_UINT16)
DEFINE_BINOUT_READ_TYPE(uint32_t, BINOUT_TYPE_UINT32)
DEFINE_BINOUT_READ_TYPE(uint64_t, BINOUT_TYPE_UINT64)
DEFINE_BINOUT_READ_TYPE(float, BINOUT_TYPE_FLOAT32)
DEFINE_BINOUT_READ_TYPE(double, BINOUT_TYPE_FLOAT64)

uint64_t binout_get_type_id(binout_file *bin_file,
                            const char *path_to_variable) {
  path_t _path;
  _path.elements = path_elements(path_to_variable, &_path.num_elements);

  binout_record_data_pointer *dp = _binout_get_data_pointer(bin_file, &_path);
  path_free(&_path);
  if (!dp) {
    bin_file->error_string = "The given variable has not been found";
    return BINOUT_TYPE_INVALID;
  }

  return dp->type_id;
}

int binout_variable_exists(binout_file *bin_file,
                           const char *path_to_variable) {
  path_t _path;
  _path.elements = path_elements(path_to_variable, &_path.num_elements);

  binout_record_data_pointer *dp = _binout_get_data_pointer(bin_file, &_path);
  if (!dp) {
    path_free(&_path);
    return 0;
  }

  _path.num_elements--;
  binout_record_data *record = _binout_get_data(dp, &_path);
  _path.num_elements++;
  path_free(&_path);
  if (!record) {
    return 0;
  }

  return 1;
}

char **binout_get_children(binout_file *bin_file, const char *path,
                           size_t *num_children) {
  path_t _path;
  _path.elements = path_elements(path, &_path.num_elements);

  *num_children = 0;
  char **children = NULL;

  size_t i = 0;
  while (i < bin_file->data_pointers_size) {
    binout_record_data_pointer *dp = &bin_file->data_pointers[i];

    /* Find a record that is a child of path*/
    size_t j = 0;
    while (j < dp->records_size) {
      binout_record_data *data = &dp->records[j];

      /* Add the variable name to the elements of the data path
       * And create a new array also containing the other path elements
       * since this is easier to do than actually appending the existing array*/
      size_t num_data_elements = data->path.num_elements + 1;
      char **data_elements = malloc(num_data_elements * sizeof(char *));
      memcpy(data_elements, data->path.elements,
             data->path.num_elements * sizeof(char *));
      data_elements[num_data_elements - 1] = dp->name;

      /* Start at the last element of path and at the penultimate element of
       * the record path*/
      size_t _path_index = _path.num_elements - 1;
      size_t data_index = num_data_elements - 2;
      /* First find the last element that is equal to the last element of path
       * We use ULONG_MAX since 0 - 1 == ULONG_MAX*/
      while (data_index != ULONG_MAX) {
        if (strcmp(_path.elements[_path_index], data_elements[data_index]) ==
            0) {
          break;
        }

        data_index--;
      }

      int path_fits = 1;

      /* Wether the record is actually a child of path*/
      if (data_index != ULONG_MAX) {
        /* Advance to the next to point to the actual child element */
        data_index++;

        /* If the match from above is not the first element*/
        if (data_index > 1) {
          /* Check if the rest of the path elements also match.
           * We can start at data_index - 2 since we already know that
           * data_index - 1 matches*/
          size_t cur_data_index = data_index - 2;
          _path_index--;
          while (_path_index != ULONG_MAX) {
            if (strcmp(_path.elements[_path_index],
                       data_elements[cur_data_index]) != 0) {
              path_fits = 0;
              break;
            }

            _path_index--;
            cur_data_index--;
          }
        }

        /* If the record is actually a child add it to the children*/
        if (path_fits) {
          char *child = data_elements[data_index];

          if (!path_elements_contain(children, *num_children, child)) {
            (*num_children)++;
            children = realloc(children, *num_children * sizeof(char *));
            const size_t child_len = strlen(child);
            children[*num_children - 1] = malloc(child_len + 1);
            memcpy(children[*num_children - 1], child, child_len + 1);
          }
        }
      }

      free(data_elements);

      j++;
    }

    i++;
  }

  path_free(&_path);

  return children;
}

void binout_free_children(char **children, size_t num_children) {
  path_free_elements(children, num_children);
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

binout_record_data_pointer *_binout_get_data_pointer(binout_file *bin_file,
                                                     path_t *path_to_variable) {
  binout_record_data_pointer *dp = NULL;
  uint64_t i = 0;
  while (i < bin_file->data_pointers_size) {
    binout_record_data_pointer *bin_dp = &bin_file->data_pointers[i];

    if (strcmp(
            bin_dp->name,
            path_to_variable->elements[path_to_variable->num_elements - 1]) ==
            0 &&
        path_main_equals(&bin_dp->records[0].path, path_to_variable)) {
      dp = bin_dp;
      break;
    }

    i++;
  }

  return dp;
}

binout_record_data_pointer *_binout_get_data_pointer2(binout_file *bin_file,
                                                      path_t *path,
                                                      const char *variable) {
  binout_record_data_pointer *dp = NULL;
  uint64_t i = 0;
  while (i < bin_file->data_pointers_size) {
    binout_record_data_pointer *bin_dp = &bin_file->data_pointers[i];

    if (strcmp(bin_dp->name, variable) == 0 &&
        path_main_equals(&bin_dp->records[0].path, path)) {
      dp = bin_dp;
      break;
    }

    i++;
  }

  return dp;
}

binout_record_data *_binout_get_data(binout_record_data_pointer *dp,
                                     path_t *path) {
  binout_record_data *data = NULL;
  uint64_t i = 0;
  while (i < dp->records_size) {
    if (path_equals(&dp->records[i].path, path)) {
      data = &dp->records[i];
      break;
    }

    i++;
  }

  return data;
}
