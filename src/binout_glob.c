#include "binout_glob.h"
#include "path.h"

#ifdef _WIN32
#include <io.h>
#include <limits.h>

char **binout_glob(const char *pattern, size_t *num_files) {
  struct _finddatai64_t find_buffer;
  intptr_t result = 0;
  *num_files = 0;
  char **globed_files = NULL;

  const intptr_t find_handle = _findfirsti64(pattern, &find_buffer);
  while (result == 0 && find_handle != -1) {
    (*num_files)++;
    globed_files = realloc(globed_files, *num_files * sizeof(char *));

    const size_t name_length = strlen(find_buffer.name);
    globed_files[*num_files - 1] = malloc(name_length + 1);
    memcpy(globed_files[*num_files - 1], find_buffer.name, name_length + 1);

    result = _findnexti64(find_handle, &find_buffer);
  }

  _findclose(find_handle);

  /* Add the parent folder to the results*/
  const size_t pattern_len = strlen(pattern);
  size_t parent_len = SIZE_MAX;

  size_t i = pattern_len - 2;
  while (i != SIZE_MAX) {
    if (pattern[i] == '\\' || pattern[i] == '/') {
      parent_len = i + 1;
      break;
    }

    i--;
  }

  /* If it has no parent path just return*/
  if (parent_len == SIZE_MAX) {
    return globed_files;
  }

  i = 0;
  while (i < *num_files) {
    const size_t name_length = strlen(globed_files[i]);
    /* Reallocate so that we don't need to free*/
    globed_files[i] = realloc(globed_files[i], parent_len + name_length + 1);
    /* Just copy from one part of the string to another*/
    memcpy(&globed_files[i][parent_len], globed_files[i], name_length);
    memcpy(globed_files[i], pattern, parent_len);
    globed_files[i][parent_len + name_length] = '\0';

    i++;
  }

  return globed_files;
}

#else
#include <glob.h>

char **binout_glob(const char *pattern, size_t *num_files) {
  glob_t glob_buffer;
  const int error_code = glob(pattern, GLOB_TILDE, NULL, &glob_buffer);

  if (error_code != 0) {
    *num_files = 0;
    globfree(&glob_buffer);
    return NULL;
  }

  /* Directly use the values from glob_buffer. Why not?*/
  *num_files = glob_buffer.gl_pathc;
  return glob_buffer.gl_pathv;
}

#endif

void binout_free_glob(char **globed_files, size_t num_files) {
  path_free_elements(globed_files, num_files);
}