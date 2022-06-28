#ifndef PATH_H
#define PATH_H
#include <stdlib.h>

#define PATH_SEP '/'
#define PATH_SEP_STR "/"

#ifdef __cplusplus
extern "C" {
#endif

char *path_join(char *path, const char *element);
int path_is_abs(const char *path);
char *path_main(const char *path);
char *path_parse(char *path);
char **path_elements(const char *path, size_t *num_elements);
int path_elements_contain(char **elements, size_t num_elements,
                          const char *value);
void path_free_elements(char **elements, size_t num_elements);

char *delete_substr(char *path, size_t start, size_t end);

#ifdef __cplusplus
}
#endif

#endif