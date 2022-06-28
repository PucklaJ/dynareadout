#ifndef PATH_H
#define PATH_H
#include <stdlib.h>

#define PATH_SEP '/'

#ifdef __cplusplus
extern "C" {
#endif

char *path_join(char *path, const char *element);
int path_is_abs(const char *path);
char *path_main(const char *path);
char *path_parse(char *path);

char *delete_substr(char *path, size_t start, size_t end);

#ifdef __cplusplus
}
#endif

#endif