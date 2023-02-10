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
#include <stdlib.h>
#include <string.h>

void *_binout_read(binout_file *bin_file, const char *path_to_variable,
                   size_t *data_size, const uint8_t binout_type) {
  CLEAR_ERROR_STRING();

  path_view_t path = path_view_new(path_to_variable);
  const binout_file_t *file =
      binout_directory_get_file(&bin_file->directory, &path);
  if (!file) {
    NEW_ERROR_STRING("The given variable has not been found");
    return NULL;
  }

  if (file->var_type != binout_type) {
    char buffer[50];
    sprintf(buffer, "The data is of type %s instead of %s",
            _binout_get_type_name(file->var_type),
            _binout_get_type_name((uint64_t)binout_type));
    NEW_ERROR_STRING(buffer);
    return NULL;
  }

  const size_t type_size = (size_t)_binout_get_type_size((uint64_t)binout_type);
  FILE *file_handle = bin_file->file_handles[file->file_index];

  if (fseek(file_handle, file->file_pos, SEEK_SET) != 0) {
    NEW_ERROR_STRING("Failed to seek to the position of the data");
    return NULL;
  }

  void *data = malloc(file->size);
  if (fread(data, file->size, 1, file_handle) != 1) {
    free(data);
    NEW_ERROR_STRING("Failed to read the data");
    return NULL;
  }

  *data_size = file->size / type_size;
  return data;
}

void *_binout_read_timed(binout_file *bin_file, const char *variable,
                         size_t *num_values, size_t *num_timesteps,
                         const uint8_t binout_type) {
  CLEAR_ERROR_STRING();

  if (bin_file->directory.num_children == 0) {
    NEW_ERROR_STRING("The binout directory is empty");
    return NULL;
  }

  path_view_t path = path_view_new(variable);
  if (!path_view_advance(&path)) {
    NEW_ERROR_STRING("The variable path is too short");
    return NULL;
  }

  const size_t folder_index = binout_directory_binary_search_folder(
      bin_file->directory.children, 0, bin_file->directory.num_children - 1,
      &path);

  if (folder_index == (size_t)~0) {
    NEW_ERROR_STRING("The folder of the variable does not exist");
    return NULL;
  }

  if (BINOUT_FOLDER_CHILDREN_GET_TYPE(
          (&bin_file->directory.children[folder_index])) != BINOUT_FOLDER) {
    NEW_ERROR_STRING("The folder contains files");
    return NULL;
  }

  if (!path_view_advance(&path)) {
    NEW_ERROR_STRING("The variable path is too short");
    return NULL;
  }

  const binout_folder_t *d_folders =
      (const binout_folder_t *)bin_file->directory.children[folder_index]
          .children;
  const size_t num_folders =
      bin_file->directory.children[folder_index].num_children;

  if (num_folders == 0) {
    NEW_ERROR_STRING("The folder is empty");
    return NULL;
  }

  /* Loop until the first dxxxxxx string has been found. It's probably the first
   * one.*/
  size_t start_index = 0;
  while (start_index < num_folders &&
         !_binout_is_d_string(d_folders[start_index].name)) {
    start_index++;
  }

  /* If no dxxxxxx folders are found*/
  if (start_index == num_folders) {
    NEW_ERROR_STRING("The folder does not contain timed data");
    return NULL;
  }

  /* Loop until the last dxxxxxx string has been found. It's probably the
   * penultimate one, after "metadata"*/
  size_t end_index = num_folders - 1;
  while (end_index > start_index &&
         !_binout_is_d_string(d_folders[end_index].name)) {
    end_index--;
  }

  *num_timesteps = end_index - start_index + 1;
  void *data = NULL;
  size_t file_index;

  size_t i = start_index;
  while (i <= end_index) {
    const binout_folder_t *current_d_folder = &d_folders[i];

    if (current_d_folder->num_children == 0) {
      free(data);
      NEW_ERROR_STRING("The folder which should contain the data is empty");
      return NULL;
    }

    if (BINOUT_FOLDER_CHILDREN_GET_TYPE(current_d_folder) != BINOUT_FILE) {
      free(data);
      NEW_ERROR_STRING(
          "The folder which should contain the data does contain more folders");
      return NULL;
    }

    /* Look for the variable in the first folder*/
    if (!data) {
      file_index = binout_directory_binary_search_file(
          (const binout_file_t *)current_d_folder->children, 0,
          current_d_folder->num_children - 1, &path);

      if (file_index == (size_t)~0) {
        NEW_ERROR_STRING("The variable has not been found");
        return NULL;
      }

      const binout_file_t *file =
          &((const binout_file_t *)current_d_folder->children)[file_index];

      if (file->var_type != binout_type) {
        NEW_ERROR_STRING("The variable is of the wrong type");
        return NULL;
      }

      *num_values = file->size /
                    (size_t)_binout_get_type_size((const uint64_t)binout_type);

      data = malloc(file->size * *num_timesteps);
    }

    const binout_file_t *file =
        &((const binout_file_t *)current_d_folder->children)[file_index];

    if (path_view_strcmp(&path, file->name) != 0) {
      file_index = binout_directory_binary_search_file(
          (const binout_file_t *)current_d_folder->children, 0,
          current_d_folder->num_children, &path);

      if (file_index == (size_t)~0) {
        free(data);
        NEW_ERROR_STRING("The variable has not been found");
        return NULL;
      }

      file = &((const binout_file_t *)current_d_folder->children)[file_index];
    }

    FILE *file_handle = bin_file->file_handles[file->file_index];
    if (fseek(file_handle, file->file_pos, SEEK_SET) != 0) {
      free(data);
      NEW_ERROR_STRING("Failed to seek to the data");
      return NULL;
    }

    if (fread(&((uint8_t *)data)[(i - start_index) * file->size], file->size, 1,
              file_handle) != 1) {
      free(data);
      NEW_ERROR_STRING("Failed to read the data");
      return NULL;
    }

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