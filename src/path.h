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

void path_join(path_t *path, const char *element);
int path_is_abs(const char *path);
int path_main_equals(path_t *path1, path_t *path2);
void path_parse(path_t *path);
/* Return multiple strings containing all elements of path
 * The return value must be deallocated using path_free_elements or path_free if
 * path_t is used*/
char **path_elements(const char *path, size_t *num_elements);
int path_elements_contain(char **elements, size_t num_elements,
                          const char *value);
void path_free_elements(char **elements, size_t num_elements);
void path_free(path_t *path);
int path_equals(path_t *path1, path_t *path2);
void path_copy(path_t *dst, path_t *src);
char *path_str(path_t *path);

char *delete_substr(char *path, size_t start, size_t end);

#ifdef __cplusplus
}
#endif

#endif