#include "path.h"
#include <stdlib.h>
#include <string.h>

char *path_join(char *path, const char *element) {
  /* TODO: Process '..' elements */
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

int path_is_abs(const char *path) { return path[0] == PATH_SEP; }

char *path_main(const char *path) {
  const size_t path_len = strlen(path);
  size_t sep_index = -1;
  size_t i = 1;
  while (i < path_len) {
    if (path[i] == PATH_SEP) {
      sep_index = i;
      break;
    }

    i++;
  }

  if (sep_index == -1) {
    char *main_path = malloc(path_len);
    memcpy(main_path, &path[1], path_len);
    return main_path;
  }

  char *main_path = malloc(sep_index);
  memcpy(main_path, &path[1], sep_index - 1);
  main_path[sep_index - 1] = '\0';

  return main_path;
}

char *path_parse(char *path) {
  size_t path_len = strlen(path);
  size_t i = 0;
  char buffer[4];
  while (i < path_len - 2) {
    buffer[0] = path[i];
    buffer[1] = path[i + 1];
    buffer[2] = path[i + 2];
    buffer[3] = path[i + 3];

    if (buffer[0] == PATH_SEP && buffer[1] == '.' && buffer[2] == '.' &&
        (buffer[3] == PATH_SEP || buffer[3] == '\0')) {
      size_t j = i - 1;
      while (j >= 0) {
        if (path[j] == PATH_SEP) {
          break;
        }

        j--;
      }

      const size_t element_start = j;
      const size_t element_end = i + 2;
      path = delete_substr(path, element_start, element_end);
      path_len -= element_end - element_start + 1;
      i = j;
    }

    i++;
  }

  return path;
}

char *delete_substr(char *path, size_t start, size_t end) {
  const size_t path_len = strlen(path);
  const size_t delete_size = end - start + 1;
  const size_t new_size = path_len - delete_size + 1;

  char *new_path = malloc(new_size);
  memcpy(new_path, path, start);
  memcpy(&new_path[start], &path[end + 1], path_len - 1 - end);
  new_path[new_size - 1] = '\0';
  free(path);

  return new_path;
}