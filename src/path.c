#include "path.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

void path_join(path_t *path, const char *element) {
  /* First get the elements of element, since it could be a path*/
  size_t num_elements;
  char **elements = path_elements(element, &num_elements);

  /* Add all found elements to path*/
  size_t i = 0;
  while (i < num_elements) {
    path->num_elements++;
    path->elements =
        realloc(path->elements, path->num_elements * sizeof(char *));
    path->elements[path->num_elements - 1] = elements[i];

    i++;
  }

  /* We don't need to call path_free_elements, since we use the elements
   * directly in path*/
  free(elements);
}

int path_is_abs(const char *path) { return path[0] == PATH_SEP; }

int path_main_equals(path_t *path1, path_t *path2) {
  const size_t path1_main_size = path1->num_elements > 2 ? 2 : 1;
  const size_t path2_main_size = path2->num_elements > 2 ? 2 : 1;
  if (path1_main_size != path2_main_size) {
    return 0;
  }

  size_t i = 0;
  while (i < path1_main_size) {
    /* We need to do 'i+1' because the first element of an absolute path is
     * always PATH_SEP*/
    if (strcmp(path1->elements[i + 1], path2->elements[i + 1]) != 0) {
      return 0;
    }

    i++;
  }

  return 1;
}

void path_parse(path_t *path) {
  /* We start at 1, since we don't care about the first element of absolute
   * paths and if the first element is ".."*/
  size_t i = 1;
  while (i < path->num_elements) {
    if (strcmp(path->elements[i], "..") == 0) {
      /* Loop over all elements after the current one and move them up by two,
       * since we want to delete the current and the previous element*/
      size_t j = i + 1;
      while (j < path->num_elements) {
        /* Swap them*/
        char *temp = path->elements[j - 2];
        path->elements[j - 2] = path->elements[j];
        path->elements[j] = temp;
        j++;
      }
      /* Free the last two elements and realloc the memory*/
      free(path->elements[path->num_elements - 1]);
      free(path->elements[path->num_elements - 2]);
      path->num_elements -= 2;
      path->elements =
          realloc(path->elements, path->num_elements * sizeof(char *));
      /* Go back by 2 since everything got moved up*/
      i -= 2;
    }

    i++;
  }
}

char **path_elements(const char *path, size_t *num_elements) {
  const size_t path_len = strlen(path);

  /* Handle the case for when there's only one character in the path. Real
   * example: "/"*/
  if (path_len == 1) {
    *num_elements = 1;
    char **elements = malloc(sizeof(char *));
    elements[0] = malloc(2);
    elements[0][0] = path[0];
    elements[0][1] = '\0';
    return elements;
  }

  *num_elements = 0;
  char **elements = NULL;

  /* Holds the index of the last found path seperator
   * Is SIZE_MAX as long as there has not been found a seperator */
  size_t last_sep = SIZE_MAX;

  /* Loop over the entire string and also include the terminating character
   * '\0'*/
  size_t i = 0;
  while (i < path_len + 1) {
    /* Check if it's a seperator and also check if we are at the end*/
    if (path[i] == PATH_SEP || i == path_len) {
      /* This sets last_sep to 0 if the first character of path is PATH_SEP*/
      last_sep *= i != 0;

      /* Calculate the length of the element that should be added.
       * If no seperator has been found the entire string up until here should
       * be used. This is the case because i - SIZE_MAX == i+1 therefore
       * i - SIZE_MAX - 1 == 1*/
      const size_t element_length = i - last_sep - 1;

      /* Only add an element if element_length is greater than 0 which means
       * that there is something between the current seperator and the last
       * seperator.
       * Or also add an element if the last_sep == i which is the case on the
       * first character of a path starting with PATH_SEP*/
      if (last_sep == i || element_length > 0) {
        /* Add a new element*/
        (*num_elements)++;
        elements = realloc(elements, *num_elements * sizeof(char *));
        char **last_element = &elements[*num_elements - 1];

        /* If we are at the first character of a path starting with PATH_SEP*/
        if (last_sep == i) {
          *last_element = malloc(2);
          (*last_element)[0] = PATH_SEP;
          (*last_element)[1] = '\0';
        } else {
          *last_element = malloc(element_length + 1);
          /* Copy the part of path into the element
           * If no seperator has been found last_sep will be SIZE_MAX
           * Which means that because of overflow the expression will result in
           * 0. Therefore the characters starting at index 0 will be copied*/
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

void path_free(path_t *path) {
  path_free_elements(path->elements, path->num_elements);
  path->elements = NULL;
  path->num_elements = 0;
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

    /* Calculate how much additional memory is necessary. Add 1 character if we
     * are at the end ('\0') or if the element does not end with PATH_SEP to add
     * a PATH_SEP*/
    str_size += element_len + (i == path->num_elements - 1 ||
                               path->elements[i][element_len - 1] != PATH_SEP);
    str = realloc(str, str_size);
    memcpy(&str[old_size], path->elements[i], element_len);
    /* Insert a PATH_SEP or '\0' if we are at the end*/
    str[str_size - 1] = PATH_SEP * (i != path->num_elements - 1);

    i++;
  }

  return str;
}