#ifndef PATH_H
#define PATH_H

#define PATH_SEP '/'

#ifdef __cplusplus
extern "C" {
#endif

char *path_join(char *path, const char *element);
int path_is_abs(const char *path);
char *path_main(const char *path);

#ifdef __cplusplus
}
#endif

#endif