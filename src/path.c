#include "path.h"
#include <stdlib.h>
#include <string.h>

void path_join(path_t *path, const char *element) {
  size_t num_elements;
  char **elements = path_elements(element, &num_elements);

  size_t i = 0;
  while (i < num_elements) {
    path->num_elements++;
    path->elements =
        realloc(path->elements, path->num_elements * sizeof(char *));
    path->elements[path->num_elements - 1] = elements[i];

    i++;
  }

  free(elements);
}

int path_is_abs(const char *path) { return path[0] == PATH_SEP; }

int path_main_equals(path_t *path1, path_t *path2) {
  const size_t path1_main_size = path1->num_elements > 1 ? 2 : 1;
  const size_t path2_main_size = path2->num_elements > 1 ? 2 : 1;
  if (path1_main_size != path2_main_size) {
    return 0;
  }

  size_t i = 0;
  while (i < path1_main_size) {
    if (strcmp(path1->elements[i], path2->elements[i]) != 0) {
      return 0;
    }

    i++;
  }

  return 1;
}

void path_parse(path_t *path) {
  size_t i = 0;
  while (i < path->num_elements) {
    if (i != 0 && strcmp(path->elements[i], "..") == 0) {
      size_t j = i + 1;
      while (j < path->num_elements) {
        char *temp = path->elements[j - 2];
        path->elements[j - 2] = path->elements[j];
        path->elements[j] = temp;
        j++;
      }
      free(path->elements[path->num_elements - 1]);
      free(path->elements[path->num_elements - 2]);
      path->num_elements -= 2;
      path->elements =
          realloc(path->elements, path->num_elements * sizeof(char *));
      i -= 2;
    }

    i++;
  }
}

char **path_elements(const char *path, size_t *num_elements) {
  const size_t path_len = strlen(path);
  size_t i = 0;
  *num_elements = 0;
  char **elements = NULL;
  size_t last_sep = 0;

  while (i < path_len + 1) {
    if (path[i] == PATH_SEP || (i == path_len && i - last_sep > 1)) {
      const size_t element_length = i - last_sep - 1;
      if (last_sep == i || element_length > 0) {
        (*num_elements)++;
        elements = realloc(elements, *num_elements * sizeof(char *));
        char **last_element = &elements[*num_elements - 1];
        if (last_sep == i) {
          *last_element = malloc(2);
          (*last_element)[0] = path[i];
          (*last_element)[1] = '\0';
        } else {
          *last_element = malloc(element_length + 1);
          memcpy(*last_element, &path[last_sep + 1], element_length);
          (*last_element)[element_length] = '\0';
        }
      }

      last_sep = i;
    }

    i++;
  }

  return elements;
}

int path_elements_contain(char **elements, size_t num_elements,
                          const char *value) {
  size_t i = 0;
  while (i < num_elements) {
    if (strcmp(elements[i], value) == 0) {
      return 1;
    }

    i++;
  }

  return 0;
}

void path_free_elements(char **elements, size_t num_elements) {
  size_t i = 0;
  while (i < num_elements) {
    free(elements[i]);

    i++;
  }
  if (elements)
    free(elements);
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

int path_equals(path_t *path1, path_t *path2) {
  if (path1->num_elements != path2->num_elements) {
    return 0;
  }

  size_t i = 0;
  while (i < path1->num_elements) {
    if (strcmp(path1->elements[i], path2->elements[i]) != 0) {
      return 0;
    }

    i++;
  }

  return 1;
}

void path_copy(path_t *dst, path_t *src) {
  dst->num_elements = src->num_elements;
  dst->elements = malloc(dst->num_elements * sizeof(char *));
  size_t i = 0;
  while (i < dst->num_elements) {
    const size_t element_len = strlen(src->elements[i]);
    dst->elements[i] = malloc(element_len + 1);
    memcpy(dst->elements[i], src->elements[i], element_len + 1);

    i++;
  }
}

char *path_str(path_t *path) {
  char *str = NULL;
  size_t str_size = 0;
  size_t i = 0;
  while (i < path->num_elements) {
    const size_t element_len = strlen(path->elements[i]);
    const size_t old_size = str_size;
    str_size += element_len;
    str = realloc(str, str_size);
    memcpy(&str[old_size], path->elements[i], element_len);
    if (i != path->num_elements - 1 && strcmp(path->elements[i], "/") != 0) {
      str_size++;
      str = realloc(str, str_size);
      str[str_size - 1] = PATH_SEP;
    }

    i++;
  }

  str = realloc(str, str_size + 1);
  str[str_size] = '\0';

  return str;
}