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