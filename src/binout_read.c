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

#include "binary_search.h"
#include "binout.h"
#include "binout_defines.h"
#include "profiling.h"
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void *_binout_read(binout_file *bin_file, const char *path_to_variable,
                   size_t *data_size, const uint8_t binout_type) {
  BINOUT_CLEAR_ERROR_STRING();

  path_view_t path = path_view_new(path_to_variable);
  const binout_file_t *file =
      binout_directory_get_file(&bin_file->directory, &path);
  if (!file) {
    NEW_ERROR_STRING_F("\"%s\" has not been found", path_to_variable);
    return NULL;
  }

  if (file->var_type != binout_type) {
    NEW_ERROR_STRING_F("\"%s\" is of type %s instead of %s", path_to_variable,
                       _binout_get_type_name(file->var_type),
                       _binout_get_type_name((uint64_t)binout_type));
    return NULL;
  }

  if (file->size == 0) {
    NEW_ERROR_STRING_F("The file \"%s\" is empty", path_to_variable);
    return NULL;
  }

  const size_t type_size = (size_t)_binout_get_type_size((uint64_t)binout_type);
  multi_file_t *multi_file = &bin_file->files[file->file_index];

  multi_file_index_t multi_file_index = multi_file_access(multi_file);
#ifndef NO_THREAD_SAFETY
  if (multi_file_index.index == ULONG_MAX) {
    NEW_ERROR_STRING_F("Failed to access file of \"%s\": %s", path_to_variable,
                       strerror(errno));
    return NULL;
  }
#endif

  if (multi_file_seek(multi_file, &multi_file_index, file->file_pos,
                      SEEK_SET) != 0) {
    multi_file_return(multi_file, &multi_file_index);
    NEW_ERROR_STRING_F("Failed to seek to the position of \"%s\"",
                       path_to_variable);
    return NULL;
  }

  void *data = malloc(file->size);
  if (multi_file_read(multi_file, &multi_file_index, data, file->size, 1) !=
      1) {
    free(data);
    multi_file_return(multi_file, &multi_file_index);
    NEW_ERROR_STRING_F("Failed to read \"%s\"", path_to_variable);
    return NULL;
  }
  multi_file_return(multi_file, &multi_file_index);

  *data_size = file->size / type_size;
  return data;
}

binout_folder_t *_binout_search_timed(binout_file *bin_file,
                                      const char *variable,
                                      size_t *file_index) {
  BINOUT_CLEAR_ERROR_STRING();

  if (bin_file->directory.num_children == 0) {
    NEW_ERROR_STRING("The binout directory is empty");
    return NULL;
  }

  path_view_t path = path_view_new(variable);
  if (!path_view_advance(&path)) {
    NEW_ERROR_STRING_F("The path \"%s\" is too short", variable);
    return NULL;
  }

  size_t search_index = binout_directory_binary_search_folder(
      bin_file->directory.children, 0, bin_file->directory.num_children - 1,
      &path);
  if (search_index == (size_t)~0) {
    NEW_ERROR_STRING_F("The variable \"%s\" does not exist", variable);
    return NULL;
  }

  binout_folder_t *folder = &bin_file->directory.children[search_index];

  while (path_view_advance(&path)) {
    if (folder->num_children == 0) {
      NEW_ERROR_STRING_F("The variable \"%s\" does not exist", variable);
      return NULL;
    }

    if (BINOUT_FOLDER_CHILDREN_GET_TYPE(folder) == BINOUT_FILE) {
      if (path_view_advance(&path)) {
        NEW_ERROR_STRING_F("The variable \"%s\" does not exist", variable);
        return NULL;
      }

      NEW_ERROR_STRING_F("The variable \"%s\" is not timed", variable);
      return NULL;
    } else {
      search_index = binout_directory_binary_search_folder(
          folder->children, 0, folder->num_children - 1, &path);
      if (search_index == (size_t)~0) {
        /* See if the is a d folder*/
        binout_folder_t *d_folder = NULL;
        size_t i = 0;
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
              d_folder->children, 0, d_folder->num_children - 1, &path);
          if (search_index != (size_t)~0) {
            *file_index = search_index;
            return folder;
          }
        }

        NEW_ERROR_STRING_F("The variable \"%s\" does not exist", variable);
        return NULL;
      } else {
        folder = &((binout_folder_t *)folder->children)[search_index];
      }
    }
  }

  NEW_ERROR_STRING_F(
      "The variable \"%s\" is either metadata (not timed) or does not exist",
      variable);
  return NULL;
}

void *_binout_read_timed(binout_file *bin_file, const char *variable,
                         size_t *num_values, size_t *num_timesteps,
                         const uint8_t binout_type) {
  size_t file_index;
  binout_folder_t *folder =
      _binout_search_timed(bin_file, variable, &file_index);
  if (!folder) {
    return NULL;
  }

  binout_folder_t *ds = (binout_folder_t *)folder->children;
  binout_file_t *df = &((binout_file_t *)ds->children)[file_index];

  size_t start_index = 0;
  while (start_index < folder->num_children &&
         !_binout_is_d_string(ds[start_index].name))
    start_index++;

  size_t end_index = folder->num_children - 1;
  while (!_binout_is_d_string(ds[end_index].name))
    end_index--;

  *num_timesteps = end_index - start_index + 1;
  *num_values =
      df->size / (size_t)_binout_get_type_size((const uint64_t)binout_type);

  if (*num_values == 0) {
    NEW_ERROR_STRING_F("The files of \"%s\" are empty", variable);
    return NULL;
  }

  void *data = malloc(df->size * *num_timesteps);

  size_t i = start_index;
  while (i <= end_index) {
    binout_folder_t *d = &ds[i];
    binout_file_t *dfs = (binout_file_t *)d->children;
    /* Just a fail safe. Should not happen. All d folders are structured the
     * same.*/
    if (d->num_children < file_index + 1 ||
        strcmp(dfs[file_index].name, df->name) != 0) {
      if (d->num_children == 0) {
        free(data);
        NEW_ERROR_STRING_F("The structure of variable \"%s\" is invalid. Time "
                           "Step %zu does not contain any files",
                           variable, i - start_index);
        return NULL;
      }

      path_view_t name;
      name.string = df->name;
      name.start = 0;
      name.end = strlen(df->name) - 1;

      file_index = binout_directory_binary_search_file(
          dfs, 0, d->num_children - 1, &name);
      if (file_index == (size_t)~0) {
        free(data);
        NEW_ERROR_STRING_F("The structure of variable \"%s\" is invalid. Time "
                           "Step %zu does not contain the variable",
                           variable, i - start_index);
        return NULL;
      }
    }

    df = &dfs[file_index];

    multi_file_t *mf = &bin_file->files[df->file_index];
    multi_file_index_t mf_idx = multi_file_access(mf);
#ifndef NO_THREAD_SAFETY
    if (mf_idx.index == ULONG_MAX) {
      free(data);
      NEW_ERROR_STRING_F("Failed to access the file of \"%s\": %s", variable,
                         strerror(errno));
      return NULL;
    }
#endif

    if (multi_file_seek(mf, &mf_idx, df->file_pos, SEEK_SET) != 0) {
      free(data);
      multi_file_return(mf, &mf_idx);
      NEW_ERROR_STRING_F("Failed to seek to the data of \"%s\"", variable);
      return NULL;
    }

    if (multi_file_read(mf, &mf_idx,
                        &((uint8_t *)data)[(i - start_index) * df->size],
                        df->size, 1) != 1) {
      free(data);
      multi_file_return(mf, &mf_idx);
      NEW_ERROR_STRING_F("Failed to read time step %zu of \"%s\"",
                         i - start_index, variable);
      return NULL;
    }

    multi_file_return(mf, &mf_idx);

    i++;
  }

  return data;
}

int8_t *binout_read_i8(binout_file *bin_file, const char *path_to_variable,
                       size_t *data_size) {
  BEGIN_PROFILE_FUNC();

  int8_t *data = (int8_t *)_binout_read(bin_file, path_to_variable, data_size,
                                        BINOUT_TYPE_INT8);

  END_PROFILE_FUNC();
  return data;
}

int16_t *binout_read_i16(binout_file *bin_file, const char *path_to_variable,
                         size_t *data_size) {
  BEGIN_PROFILE_FUNC();

  int16_t *data = (int16_t *)_binout_read(bin_file, path_to_variable, data_size,
                                          BINOUT_TYPE_INT16);

  END_PROFILE_FUNC();
  return data;
}

int32_t *binout_read_i32(binout_file *bin_file, const char *path_to_variable,
                         size_t *data_size) {
  BEGIN_PROFILE_FUNC();

  int32_t *data = (int32_t *)_binout_read(bin_file, path_to_variable, data_size,
                                          BINOUT_TYPE_INT32);

  END_PROFILE_FUNC();
  return data;
}

int64_t *binout_read_i64(binout_file *bin_file, const char *path_to_variable,
                         size_t *data_size) {
  BEGIN_PROFILE_FUNC();

  int64_t *data = (int64_t *)_binout_read(bin_file, path_to_variable, data_size,
                                          BINOUT_TYPE_INT64);

  END_PROFILE_FUNC();
  return data;
}

uint8_t *binout_read_u8(binout_file *bin_file, const char *path_to_variable,
                        size_t *data_size) {
  BEGIN_PROFILE_FUNC();

  uint8_t *data = (uint8_t *)_binout_read(bin_file, path_to_variable, data_size,
                                          BINOUT_TYPE_UINT8);

  END_PROFILE_FUNC();
  return data;
}

uint16_t *binout_read_u16(binout_file *bin_file, const char *path_to_variable,
                          size_t *data_size) {
  BEGIN_PROFILE_FUNC();

  uint16_t *data = (uint16_t *)_binout_read(bin_file, path_to_variable,
                                            data_size, BINOUT_TYPE_UINT16);

  END_PROFILE_FUNC();
  return data;
}

uint32_t *binout_read_u32(binout_file *bin_file, const char *path_to_variable,
                          size_t *data_size) {
  BEGIN_PROFILE_FUNC();

  uint32_t *data = (uint32_t *)_binout_read(bin_file, path_to_variable,
                                            data_size, BINOUT_TYPE_UINT32);

  END_PROFILE_FUNC();
  return data;
}

uint64_t *binout_read_u64(binout_file *bin_file, const char *path_to_variable,
                          size_t *data_size) {
  BEGIN_PROFILE_FUNC();

  uint64_t *data = (uint64_t *)_binout_read(bin_file, path_to_variable,
                                            data_size, BINOUT_TYPE_UINT64);

  END_PROFILE_FUNC();
  return data;
}

float *binout_read_f32(binout_file *bin_file, const char *path_to_variable,
                       size_t *data_size) {
  BEGIN_PROFILE_FUNC();

  float *data = (float *)_binout_read(bin_file, path_to_variable, data_size,
                                      BINOUT_TYPE_FLOAT32);

  END_PROFILE_FUNC();
  return data;
}

double *binout_read_f64(binout_file *bin_file, const char *path_to_variable,
                        size_t *data_size) {
  BEGIN_PROFILE_FUNC();

  double *data = (double *)_binout_read(bin_file, path_to_variable, data_size,
                                        BINOUT_TYPE_FLOAT64);

  END_PROFILE_FUNC();
  return data;
}

int8_t *binout_read_timed_i8(binout_file *bin_file, const char *variable,
                             size_t *num_values, size_t *num_timesteps) {
  BEGIN_PROFILE_FUNC();

  int8_t *data = (int8_t *)_binout_read_timed(bin_file, variable, num_values,
                                              num_timesteps, BINOUT_TYPE_INT8);

  END_PROFILE_FUNC();
  return data;
}

int16_t *binout_read_timed_i16(binout_file *bin_file, const char *variable,
                               size_t *num_values, size_t *num_timesteps) {
  BEGIN_PROFILE_FUNC();

  int16_t *data = (int16_t *)_binout_read_timed(
      bin_file, variable, num_values, num_timesteps, BINOUT_TYPE_INT16);

  END_PROFILE_FUNC();
  return data;
}

int32_t *binout_read_timed_i32(binout_file *bin_file, const char *variable,
                               size_t *num_values, size_t *num_timesteps) {
  BEGIN_PROFILE_FUNC();

  int32_t *data = (int32_t *)_binout_read_timed(
      bin_file, variable, num_values, num_timesteps, BINOUT_TYPE_INT32);

  END_PROFILE_FUNC();
  return data;
}

int64_t *binout_read_timed_i64(binout_file *bin_file, const char *variable,
                               size_t *num_values, size_t *num_timesteps) {
  BEGIN_PROFILE_FUNC();

  int64_t *data = (int64_t *)_binout_read_timed(
      bin_file, variable, num_values, num_timesteps, BINOUT_TYPE_INT64);

  END_PROFILE_FUNC();
  return data;
}

uint8_t *binout_read_timed_u8(binout_file *bin_file, const char *variable,
                              size_t *num_values, size_t *num_timesteps) {
  BEGIN_PROFILE_FUNC();

  uint8_t *data = (uint8_t *)_binout_read_timed(
      bin_file, variable, num_values, num_timesteps, BINOUT_TYPE_UINT8);

  END_PROFILE_FUNC();
  return data;
}

uint16_t *binout_read_timed_u16(binout_file *bin_file, const char *variable,
                                size_t *num_values, size_t *num_timesteps) {
  BEGIN_PROFILE_FUNC();

  uint16_t *data = (uint16_t *)_binout_read_timed(
      bin_file, variable, num_values, num_timesteps, BINOUT_TYPE_UINT16);

  END_PROFILE_FUNC();
  return data;
}

uint32_t *binout_read_timed_u32(binout_file *bin_file, const char *variable,
                                size_t *num_values, size_t *num_timesteps) {
  BEGIN_PROFILE_FUNC();

  uint32_t *data = (uint32_t *)_binout_read_timed(
      bin_file, variable, num_values, num_timesteps, BINOUT_TYPE_UINT32);

  END_PROFILE_FUNC();
  return data;
}

uint64_t *binout_read_timed_u64(binout_file *bin_file, const char *variable,
                                size_t *num_values, size_t *num_timesteps) {
  BEGIN_PROFILE_FUNC();

  uint64_t *data = (uint64_t *)_binout_read_timed(
      bin_file, variable, num_values, num_timesteps, BINOUT_TYPE_UINT64);

  END_PROFILE_FUNC();
  return data;
}

float *binout_read_timed_f32(binout_file *bin_file, const char *variable,
                             size_t *num_values, size_t *num_timesteps) {
  BEGIN_PROFILE_FUNC();

  float *data = (float *)_binout_read_timed(bin_file, variable, num_values,
                                            num_timesteps, BINOUT_TYPE_FLOAT32);

  END_PROFILE_FUNC();
  return data;
}

double *binout_read_timed_f64(binout_file *bin_file, const char *variable,
                              size_t *num_values, size_t *num_timesteps) {
  BEGIN_PROFILE_FUNC();

  double *data = (double *)_binout_read_timed(
      bin_file, variable, num_values, num_timesteps, BINOUT_TYPE_FLOAT64);

  END_PROFILE_FUNC();
  return data;
}