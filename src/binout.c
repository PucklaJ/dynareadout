/***********************************************************************************
 *                         This file is part of dynareadout
 *                    https://github.com/PucklaJ/dynareadout
 ***********************************************************************************
 * Copyright (c) 2022 Jonas Pucher
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not claim
 * that you wrote the original software. If you use this software in a product,
 * an acknowledgment in the product documentation would be appreciated but is
 * not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 ************************************************************************************/

#include "binout.h"
#include "binary_search.h"
#include "binout_defines.h"
#include "binout_glob.h"
#include "path.h"
#include "profiling.h"
#include "string_builder.h"
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define FILE_FAILED(message)                                                   \
  multi_file_close(file);                                                      \
  _binout_add_file_error(&bin_file, file_names[cur_file_index], message);      \
  cur_file_index++;                                                            \
  continue

#define BIN_FILE_READ(dst, size, count, message)                               \
  read_count = multi_file_read(file, &file_index, &dst, size, count);          \
  if (read_count != count) {                                                   \
    cur_file_failed = 1;                                                       \
    _binout_add_file_error(&bin_file, file_names[cur_file_index], message);    \
    break;                                                                     \
  }

#define BIN_FILE_READ_FREE(dst, size, count, obj, message)                     \
  read_count = multi_file_read(file, &file_index, dst, size, count);           \
  if (read_count != count) {                                                   \
    free(obj);                                                                 \
    _binout_add_file_error(&bin_file, file_names[cur_file_index], message);    \
    cur_file_failed = 1;                                                       \
    break;                                                                     \
  }

binout_file binout_open(const char *file_name) {
  BEGIN_PROFILE_FUNC();

  binout_file bin_file;
  bin_file.directory.children = NULL;
  bin_file.directory.num_children = 0;
  bin_file.files = NULL;
  bin_file.file_errors = NULL;
  bin_file.error_string = NULL;
  bin_file.num_files = 0;
  bin_file.num_file_errors = 0;

  char **file_names = binout_glob(file_name, &bin_file.num_files);
  if (bin_file.num_files == 0) {
    _binout_add_file_error(&bin_file, file_name, "No files have been found");
    END_PROFILE_FUNC();
    return bin_file;
  }

  bin_file.files = malloc(bin_file.num_files * sizeof(multi_file_t));

  size_t cur_file_index = 0;
  while (cur_file_index < bin_file.num_files) {
    bin_file.files[cur_file_index] =
        multi_file_open(file_names[cur_file_index]);
#ifdef NO_THREAD_SAFETY
    if (!bin_file.files[cur_file_index]) {
      _binout_add_file_error(&bin_file, file_names[cur_file_index],
                             strerror(errno));
    }
#endif

    cur_file_index++;
  }

  cur_file_index = 0;
  while (cur_file_index < bin_file.num_files) {
    multi_file_t *file = &bin_file.files[cur_file_index];
    multi_file_index_t file_index = multi_file_access(file);

    /* Just ignore the file if it failed to open*/
#ifndef NO_THREAD_SAFETY
    if (file_index.index == ULONG_MAX) {
      FILE_FAILED(strerror(errno));
    }
#else
    if (!(*file)) {
      cur_file_index++;
      continue;
    }
#endif

    binout_header header;

    /* Read header */
    size_t read_count =
        multi_file_read(file, &file_index, &header, sizeof(binout_header), 1);
    if (read_count == 0) {
      FILE_FAILED("Failed to read header");
    }

    /* Check if the binout file is actually supported (Might also be an
     * indicator that the given file is not a binout) */
    if (header.endianess != BINOUT_HEADER_LITTLE_ENDIAN) {
      FILE_FAILED("Unsupported Endianess");
    }
    if (header.record_length_field_size > 8) {
      FILE_FAILED("The record length field size is unsupported");
    }
    if (header.record_command_field_size > 8) {
      FILE_FAILED("The command length field size is unsupported");
    }
    if (header.record_typeid_field_size > 8) {
      FILE_FAILED("The typeid field size is unsupported");
    }
    if (header.float_format != BINOUT_HEADER_FLOAT_IEEE) {
      FILE_FAILED("The float format is unsupported");
    }

    /* Get the file size*/
    const long file_size = (long)path_get_file_size(file_names[cur_file_index]);

    /* Parse all records */

    /* Store the current path which is changed by the CD commands*/
    /* Store 1KB on the stack, this should totally suffice*/
    char current_path_string[1024];
    current_path_string[0] = PATH_SEP;
    current_path_string[1] = '\0';
    path_view_t current_path = path_view_new(current_path_string);
    binout_folder_t *current_folder = NULL;

    int cur_file_failed = 0;

    /* A buffer for the path of the CD command*/
    char path_buffer[1024];

    /* We cannot use EOF, so we use this*/
    while (1) {
      /* Check if we are already at the end or if an error occurred in ftell*/
      const long current_file_pos = multi_file_tell(file, &file_index);
      if (current_file_pos == -1 || current_file_pos == file_size) {
        break;
      }

      uint64_t record_length = 0, record_command = 0;

      BIN_FILE_READ(record_length, header.record_length_field_size, 1,
                    "Failed to read record length");
      BIN_FILE_READ(record_command, header.record_command_field_size, 1,
                    "Failed to read command");

      const uint64_t record_data_length = record_length -
                                          header.record_length_field_size -
                                          header.record_command_field_size;

      /* Execute code for all the different commands
       * Currently only CD and DATA. All other commands are ignored*/
      if (record_command == BINOUT_COMMAND_CD) {
        assert(record_data_length < 1024);

        path_buffer[record_data_length] = '\0';
        BIN_FILE_READ(path_buffer, 1, record_data_length,
                      "Failed to read PATH of CD record");

        if PATH_IS_ABS (path_buffer) {
          memcpy(current_path_string, path_buffer, record_data_length + 1);
          current_path = path_view_new(current_path_string);
          /* Only insert the current folder if the current path is not the
           * root folder*/
          if (path_view_advance(&current_path)) {
            current_folder = binout_directory_insert_folder(&bin_file.directory,
                                                            &current_path);
          }
        } else {
          path_view_t path = path_view_new(path_buffer);

          while (1) {
            if (path_view_strcmp(&path, "..") == 0) {
              size_t index = path_move_up(current_path_string);
              index += index == 0;

              current_path_string[index] = '\0';
            } else {
              /* Join current_path_string with path*/
              const int path_len = PATH_VIEW_LEN((&path));
              int len = strlen(current_path_string);
              assert((len + path_len + 1) < 1024);

              if (current_path_string[len - 1] != PATH_SEP) {
                current_path_string[len] = PATH_SEP;
                len++;

                assert(len < 1024);
              }

              PATH_VIEW_CPY(&current_path_string[len], (&path));
              current_path_string[len + path_len] = '\0';
            }

            if (!path_view_advance(&path)) {
              break;
            }
          }

          current_path = path_view_new(current_path_string);
          path_view_advance(&current_path);

          current_folder = binout_directory_insert_folder(&bin_file.directory,
                                                          &current_path);
        }
      } else if (record_command == BINOUT_COMMAND_DATA) {
        /* If current_folder is NULL, this means that there are files inside
         * '/', which we do not support. And LS Dyna does also not do this.
         */
        assert(current_folder != NULL);

        uint64_t type_id = 0;
        uint8_t variable_name_length;

        BIN_FILE_READ(type_id, header.record_typeid_field_size, 1,
                      "Failed to read TYPEID of DATA record");
        BIN_FILE_READ(variable_name_length, BINOUT_DATA_NAME_LENGTH, 1,
                      "Failed to read Name length of DATA record");

        char *variable_name = malloc(variable_name_length + 1);
        variable_name[variable_name_length] = '\0';

        BIN_FILE_READ_FREE(variable_name, 1, variable_name_length,
                           variable_name, "Failed to read Name of DATA record");

        /* How large the data segment of the data record is*/
        const uint64_t data_length =
            record_data_length - header.record_typeid_field_size -
            BINOUT_DATA_NAME_LENGTH - variable_name_length;
        const long file_pos = multi_file_tell(file, &file_index);
        /* Skip the data since we will read it at a later point, if it is
         * requested by the programmer*/
        if (multi_file_seek(file, &file_index, data_length, SEEK_CUR) != 0) {
          free(variable_name);
          cur_file_failed = 1;
          _binout_add_file_error(&bin_file, file_names[cur_file_index],
                                 "Failed to skip Data of DATA record");
          break;
        }

        binout_folder_insert_file(current_folder, variable_name,
                                  (uint8_t)type_id, data_length,
                                  (uint8_t)cur_file_index, file_pos);
      } else {
        /* Just skip the record and ignore its data*/
        if (multi_file_seek(file, &file_index, record_data_length, SEEK_CUR) !=
            0) {
          cur_file_failed = 1;
          _binout_add_file_error(&bin_file, file_names[cur_file_index],
                                 "Failed to skip data of a record");
          break;
        }
      }
    }

    multi_file_return(file, &file_index);

    if (cur_file_failed) {
      multi_file_close(file);
    }

    cur_file_index++;
  }

  binout_free_glob(file_names, bin_file.num_files);

  /* Clean up failed files*/
  cur_file_index = 0;
  while (cur_file_index < bin_file.num_files) {
#ifndef NO_THREAD_SAFETY
    if (bin_file.files[cur_file_index].file_handles == NULL) {
#else
    if (!bin_file.files[cur_file_index]) {
#endif
      /* Swap with the last element*/
      bin_file.files[cur_file_index] = bin_file.files[bin_file.num_files - 1];

      /* Reallocate memory*/
      bin_file.num_files--;
      bin_file.files =
          realloc(bin_file.files, bin_file.num_files * sizeof(multi_file_t));

      cur_file_index--;
    }

    cur_file_index++;
  }

  END_PROFILE_FUNC();

  return bin_file;
}

void binout_close(binout_file *bin_file) {
  BEGIN_PROFILE_FUNC();

  /* Free all files*/
  size_t cur_file_index = 0;
  while (cur_file_index < bin_file->num_files) {
    multi_file_close(&bin_file->files[cur_file_index]);

    cur_file_index++;
  }

  /* Free all file errors*/
  size_t i = 0;
  while (i < bin_file->num_file_errors) {
    free(bin_file->file_errors[i]);

    i++;
  }

  binout_directory_free(&bin_file->directory);

  /* Set everything to 0 so that no error happens if function get called after
   * binout_close*/
  bin_file->directory.children = NULL;
  bin_file->directory.num_children = 0;
  bin_file->files = NULL;
  bin_file->file_errors = NULL;
  bin_file->error_string = NULL;
  bin_file->num_files = 0;
  bin_file->num_file_errors = 0;

  END_PROFILE_FUNC();
}

uint8_t binout_get_type_id(binout_file *bin_file,
                           const char *path_to_variable) {
  BEGIN_PROFILE_FUNC();
  BINOUT_CLEAR_ERROR_STRING();

  path_view_t path = path_view_new(path_to_variable);
  const binout_file_t *file =
      binout_directory_get_file(&bin_file->directory, &path);
  if (!file) {
    NEW_ERROR_STRING_F("\"%s\" has not been found", path_to_variable);
    END_PROFILE_FUNC();
    return BINOUT_TYPE_INVALID;
  }

  END_PROFILE_FUNC();
  return file->var_type;
}

int binout_variable_exists(binout_file *bin_file,
                           const char *path_to_variable) {
  BEGIN_PROFILE_FUNC();

  path_view_t path = path_view_new(path_to_variable);
  const binout_file_t *file =
      binout_directory_get_file(&bin_file->directory, &path);

  END_PROFILE_FUNC();
  return file != NULL;
}

char **binout_get_children(binout_file *bin_file,
                           const char *path_to_folder_or_file,
                           size_t *num_children) {
  BEGIN_PROFILE_FUNC();

  path_view_t path = path_view_new(path_to_folder_or_file);
  const binout_folder_or_file_t *folder_or_file =
      binout_directory_get_children(&bin_file->directory, &path, num_children);
  if (!folder_or_file) {
    return NULL;
  }

  char **children = malloc(*num_children * sizeof(char *));

  if (folder_or_file->type == BINOUT_FOLDER) {
    size_t i = 0;
    while (i < *num_children) {
      children[i] = ((const binout_folder_t *)folder_or_file)[i].name;

      i++;
    }
  } else {
    size_t i = 0;
    while (i < *num_children) {
      children[i] = ((const binout_file_t *)folder_or_file)[i].name;

      i++;
    }
  }

  END_PROFILE_FUNC();
  return children;
}

void binout_free_children(char **children) {
  BEGIN_PROFILE_FUNC();

  free(children);

  END_PROFILE_FUNC();
}

char *binout_open_error(binout_file *bin_file) {
  BEGIN_PROFILE_FUNC();

  string_builder_t file_error = string_builder_new();

  size_t i = 0;
  while (i < bin_file->num_file_errors) {
    string_builder_append(&file_error, bin_file->file_errors[i]);
    if (i != bin_file->num_file_errors - 1) {
      string_builder_append_char(&file_error, '\n');
    }

    i++;
  }

  END_PROFILE_FUNC();
  return string_builder_move(&file_error);
}

size_t binout_get_num_timesteps(const binout_file *bin_file, const char *path) {
  BEGIN_PROFILE_FUNC();

  path_view_t pv = path_view_new(path);

  size_t num_children;
  const binout_folder_or_file_t *folder_or_file =
      binout_directory_get_children(&bin_file->directory, &pv, &num_children);

  if (num_children == 0) {
    END_PROFILE_FUNC();
    return 0;
  }
  if (num_children == (size_t)~0 || folder_or_file->type == BINOUT_FILE) {
    END_PROFILE_FUNC();
    return ~0;
  }

  const binout_folder_t *folders = (const binout_folder_t *)folder_or_file;

  /* Loop until the first dxxxxxx string has been found. It's probably the
   * first one.*/
  size_t start_index = 0;
  while (start_index < num_children &&
         !_binout_is_d_string(folders[start_index].name)) {
    start_index++;
  }

  /* If no dxxxxxx folders are found*/
  if (start_index == num_children) {
    END_PROFILE_FUNC();
    return 0;
  }

  /* Loop until the last dxxxxxx string has been found. It's probably the
   * penultimate one, after "metadata"*/
  size_t end_index = num_children - 1;
  while (!_binout_is_d_string(folders[end_index].name)) {
    end_index--;
  }

  END_PROFILE_FUNC();
  return end_index - start_index + 1;
}

char *binout_simple_path_to_real(const binout_file *bin_file,
                                 const char *simple, uint8_t *type_id,
                                 int *timed) {
  BEGIN_PROFILE_FUNC();

  *type_id = BINOUT_TYPE_INVALID;
  *timed = 0;

  path_view_t simple_path = path_view_new(simple);
  /* The current folder in this recursive operation*/
  binout_folder_t *folder;
  size_t search_index;
  string_builder_t real_path = string_builder_new();
  string_builder_append_char(&real_path, PATH_SEP);

  /* If the simple path starts with PATH_SEP advance*/
  if (PATH_IS_ABS(simple)) {
    /* The simple path is just the root folder*/
    if (!path_view_advance(&simple_path)) {
      END_PROFILE_FUNC();
      return string_builder_move(&real_path);
    }
  }

  if (bin_file->directory.num_children == 0) {
    string_builder_free(&real_path);
    END_PROFILE_FUNC();
    return NULL;
  }

  search_index = binout_directory_binary_search_folder(
      bin_file->directory.children, 0, bin_file->directory.num_children - 1,
      &simple_path);
  if (search_index == (size_t)~0) {
    string_builder_free(&real_path);
    END_PROFILE_FUNC();
    return NULL;
  }

  folder = &bin_file->directory.children[search_index];

  string_builder_append(&real_path, folder->name);

  /* Advance until we are done*/
  while (path_view_advance(&simple_path)) {
    if (folder->num_children == 0) {
      string_builder_free(&real_path);
      END_PROFILE_FUNC();
      return NULL;
    }

    switch (BINOUT_FOLDER_CHILDREN_GET_TYPE(folder)) {
    case BINOUT_FILE: {
      search_index = binout_directory_binary_search_file(
          folder->children, 0, folder->num_children - 1, &simple_path);
      if (search_index == (size_t)~0) {
        string_builder_free(&real_path);
        END_PROFILE_FUNC();
        return NULL;
      }

      /* Make sure that the simple path is also done*/
      if (path_view_advance(&simple_path)) {
        string_builder_free(&real_path);
        END_PROFILE_FUNC();
        return NULL;
      }

      binout_file_t *file = &((binout_file_t *)folder->children)[search_index];
      *type_id = file->var_type;

      string_builder_append_char(&real_path, PATH_SEP);
      string_builder_append(&real_path, file->name);

      char *rv = string_builder_move(&real_path);
      END_PROFILE_FUNC();
      return rv;
    } break;
    case BINOUT_FOLDER: {
      search_index = binout_directory_binary_search_folder(
          folder->children, 0, folder->num_children - 1, &simple_path);

      /* If the simple folder can be found directly advance to the next folder*/
      if (search_index != (size_t)~0) {
        folder = &((binout_folder_t *)folder->children)[search_index];

        string_builder_append_char(&real_path, PATH_SEP);
        string_builder_append(&real_path, folder->name);
        break;
      }

      /* If a metadata folder exists beneath the current folder search it for
       * the simple path*/
      binout_folder_t *metadata = NULL;
      /* The metadata folder should be the last one*/
      size_t i = folder->num_children - 1;
      while (1) {
        metadata = &((binout_folder_t *)folder->children)[i];
        if (_binout_is_metadata_string(metadata->name)) {
          break;
        }
        metadata = NULL;

        if (i == 0) {
          break;
        }
        i--;
      }

      if (metadata && metadata->num_children != 0 &&
          BINOUT_FOLDER_CHILDREN_GET_TYPE(metadata) == BINOUT_FILE) {
        search_index = binout_directory_binary_search_file(
            metadata->children, 0, metadata->num_children - 1, &simple_path);
        if (search_index != (size_t)~0) {
          /* We should be done*/
          if (path_view_advance(&simple_path)) {
            string_builder_free(&real_path);
            END_PROFILE_FUNC();
            return NULL;
          }

          binout_file_t *file =
              &((binout_file_t *)metadata->children)[search_index];

          string_builder_append_char(&real_path, PATH_SEP);
          string_builder_append(&real_path, "metadata");
          string_builder_append_char(&real_path, PATH_SEP);
          string_builder_append(&real_path, file->name);

          *type_id = file->var_type;

          char *rv = string_builder_move(&real_path);
          END_PROFILE_FUNC();
          return rv;
        }
      }

      /* If timed data exists in the folder search it for the simple path*/
      binout_folder_t *d_folder = NULL;
      /* The d folder of the first time step should be the first one*/
      i = 0;
      while (i < folder->num_children) {
        d_folder = &((binout_folder_t *)folder->children)[i];
        if (_binout_is_d_string(d_folder->name)) {
          break;
        }
        d_folder = NULL;

        i++;
      }

      if (d_folder && d_folder->num_children != 0 &&
          BINOUT_FOLDER_CHILDREN_GET_TYPE(d_folder) == BINOUT_FILE) {
        search_index = binout_directory_binary_search_file(
            d_folder->children, 0, d_folder->num_children - 1, &simple_path);
        if (search_index != (size_t)~0) {
          /* We should be done*/
          if (path_view_advance(&simple_path)) {
            string_builder_free(&real_path);
            END_PROFILE_FUNC();
            return NULL;
          }

          binout_file_t *file =
              &((binout_file_t *)d_folder->children)[search_index];

          string_builder_append_char(&real_path, PATH_SEP);
          string_builder_append(&real_path, file->name);

          *type_id = file->var_type;
          *timed = 1;

          char *rv = string_builder_move(&real_path);
          END_PROFILE_FUNC();
          return rv;
        }
      }

      string_builder_free(&real_path);
      END_PROFILE_FUNC();
      return NULL;
    } break;
    }
  }

  char *rv = string_builder_move(&real_path);
  END_PROFILE_FUNC();
  return rv;
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

void _binout_add_file_error(binout_file *bin_file, const char *file_name,
                            const char *message) {
  bin_file->num_file_errors++;
  bin_file->file_errors = realloc(bin_file->file_errors,
                                  bin_file->num_file_errors * sizeof(char *));

  string_builder_t new_file_error = string_builder_new();
  string_builder_append(&new_file_error, file_name);
  string_builder_append(&new_file_error, ": ");
  string_builder_append(&new_file_error, message);
  bin_file->file_errors[bin_file->num_file_errors - 1] =
      string_builder_move(&new_file_error);
}

int _binout_is_d_string(const char *folder_name) {
  if (folder_name[0] != 'd') {
    return 0;
  }

  size_t i = 1;
  while (1) {
    if (folder_name[i] == '\0') {
      break;
    }
    /* If the character is not a number*/
    if (!(folder_name[i] >= 48 && folder_name[i] <= 57)) {
      return 0;
    }
    i++;
  }

  /* So that the string can not just be "d"*/
  return i != 1;
}

int _binout_is_metadata_string(const char *folder_name) {
  const char *m = "metadata";
  const size_t m_len = 8;
  size_t i = 0;
  /* This also checks if the null terminator is at the end*/
  while (i < (m_len + 1)) {
    if (folder_name[i] != m[i]) {
      return 0;
    }

    i++;
  }

  return 1;
}

int _binout_path_view_is_d_string(const path_view_t *pv) {
  if (pv->string[pv->start] != 'd') {
    return 0;
  }

  int i = pv->start + 1;
  while (i <= pv->end) {
    /* If the character is not a number*/
    if (!(pv->string[i] >= 48 && pv->string[i] <= 57)) {
      return 0;
    }
    i++;
  }

  /* So that the string can not just be "d"*/
  return i != 1;
}