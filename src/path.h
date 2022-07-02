#ifndef PATH_H
#define PATH_H
#include <stdlib.h>

#define PATH_SEP '/'
#define PATH_SEP_STR "/"

typedef struct {
  char **elements;
  size_t num_elements;
} path_t;

#ifdef __cplusplus
extern "C" {
#endif

/* Add element to the elements of path*/
void path_join(path_t *path, const char *element);
/* Check wether path is absolute instead of relative (Just checks if the first
 * char is PATH_SEP)*/
int path_is_abs(const char *path);
/* Checks if the main path of path1 equals the main path of path2
 * The main path of a path consists of the first two elements of the path or
 * of the first element if there are only two elements
 * Example: /nodout/metadata -> /nodout/metadata
 *          /nodout/slave_10002/metadata -> /nodout/slave_10002
 *          /nodout -> /nodout */
int path_main_equals(path_t *path1, path_t *path2);
/* Parses the path for ".." and changes its elements accordingly
 * Example: /nodout/metadata/../d000001 -> /nodout/d000001*/
void path_parse(path_t *path);
/* Return multiple strings containing all elements of path
 * The return value must be deallocated using path_free_elements or path_free if
 * path_t is used*/
char **path_elements(const char *path, size_t *num_elements);
/* Returns if elements contain value */
int path_elements_contain(char **elements, size_t num_elements,
                          const char *value);
/* Frees all the memory allocated*/
void path_free_elements(char **elements, size_t num_elements);
/* Frees all the memory allocated*/
void path_free(path_t *path);
/* Wether the elements of path1 are the same as path2*/
int path_equals(path_t *path1, path_t *path2);
/* Create a copy of src and store it in dst
 * dst needs to be deallocated wit path_free*/
void path_copy(path_t *dst, path_t *src);
/* Converts a path_t to a human readable string*/
char *path_str(path_t *path);

#ifdef __cplusplus
}
#endif

#endif